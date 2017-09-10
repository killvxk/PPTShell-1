#include "StdAfx.h"
#include "LocalInteractiveQuestionItem.h"
#include "DUI/ItemExplorer.h"
#include "NDCloud/NDCloudUser.h"
#include "NDCloud/NDCloudQuestion.h"
#include "NDCloud/NDCloudCoursewareObjects.h"
#include "NDCloud/NDCloudAPI.h"
#include "GUI/MainFrm.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "NDCloud/NDCloudFile.h"
#include "EventCenter/EventDefine.h"
#include "DUI/GroupExplorer.h"
#include "NDCloud/NDCloudLocalQuestion.h"
#include "NDCloud/NDCloudResModel.h"
#include "Util/Stream.h"
#include "NDCloud/NDCloudUpload.h"
#include "CoursePlayUI.h"

#define EduPlatformHost								_T("esp-lifecycle.web.sdp.101.com")
#define EduPlatformUrlQuestionModel					_T("/v0.6/questions/%s")
#define EduPlatformUrlCoursewareObjectModel			_T("/v0.6/coursewareobjects/%s")

CLocalInteractiveQuestionItemUI::CLocalInteractiveQuestionItemUI()
{
	m_strGuid			= _T("");
	m_strQuestionType	= _T("");
	m_strExercisesDir	= _T("");
	m_hThread			= NULL;
	m_OnComplete		+= MakeEventDelegate(this, &CLocalInteractiveQuestionItemUI::OnQuestionThumbnail);
}

CLocalInteractiveQuestionItemUI::~CLocalInteractiveQuestionItemUI()
{
	if ( m_hThread != NULL )
	{
		TerminateThread(m_hThread, 0);
		m_hThread = NULL;
	}
}


void CLocalInteractiveQuestionItemUI::Init()
{
	__super::Init();
	SetContentHeight(110);

	CControlUI* pCtrl = FindSubControl(_T("toolbar"));
	if (pCtrl)
	{
		pCtrl->SetFixedHeight(30);
	}

	tstring strXmlPath		= GetResource();
	tstring strThumbPath	= _T("");

	if ( !strXmlPath.empty() )
	{
		int nPos = strXmlPath.rfind(_T("\\"));
		if ( nPos != tstring::npos )
		{
			m_strExercisesDir	= strXmlPath.substr(0, nPos);
			tstring strJsonPath = m_strExercisesDir;
			strJsonPath += _T("\\metadata.json");
			tstring nQuestionCode;
			NDCloudLocalQuestionManager::GetInstance()->GetQuestionCodeInfo(strJsonPath, m_strGuid, m_strQuestionType,nQuestionCode);
			//GetQuestionInfo(strJsonPath, m_strGuid, m_strQuestionType);
		}

		// thumbnail path	
		strThumbPath = strXmlPath;
		strThumbPath += _T(".jpg");

		// check whether
		DWORD ret = GetFileAttributes(strThumbPath.c_str());
		if( ret == INVALID_FILE_ATTRIBUTES )
		{
			if ( m_hThread == NULL )
				m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CreateQuestionThumbnailFuc, this, 0, NULL);
		}
		else
			SetImage(strThumbPath.c_str());
	}
	else
	{
		SetImage(_T("RightBar\\Item\\item_bg_course.png"));
	}

	SetIcon(_T(""));
}

DWORD CLocalInteractiveQuestionItemUI::CreateQuestionThumbnailFuc(LPARAM lParam)
{
	CLocalInteractiveQuestionItemUI* pThis = (CLocalInteractiveQuestionItemUI*) lParam;

	tstring strThumbPath = GenerateQuestionThumbnail(pThis->GetResource());

	if ( !strThumbPath.empty())
	{
		TEventNotify CompleteNotify;
		memset(&CompleteNotify, 0, sizeof(TEventNotify));

		CStream cStream(1024);
		cStream.WriteString(strThumbPath);
		cStream.ResetCursor();

		CompleteNotify.wParam = (WPARAM)&cStream;

		Sleep(100);

		SendMessage(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), WM_CRATE_QUESTION_THUMBNAIL, (WPARAM)&pThis->m_OnComplete, (LPARAM)&CompleteNotify);
	}	

	CloseHandle(pThis->m_hThread);
	pThis->m_hThread = NULL;

	return 1;
}


BOOL CLocalInteractiveQuestionItemUI::GetQuestionInfo( tstring strJosnPath, tstring& strGuid, tstring& strQuestionType )
{
	FILE* pf = _tfopen(strJosnPath.c_str(), _T("r"));

	if ( pf == NULL )
		return FALSE;

	fseek(pf, 0,  SEEK_END);
	int nSize = ftell(pf);
	fseek(pf, 0, SEEK_SET);

	DWORD dwSize = 0;
	char* pBuff = new char[nSize + 1];
	memset(pBuff, 0, nSize + 1);
	fread(pBuff, nSize, 1, pf);
	fclose(pf);

	Json::Reader	jsonReader;

	Json::Value		rootItem;
	Json::Value		questionGuid;
	Json::Value		questionType;
	Json::Value		questionName;

	if ( jsonReader.parse(pBuff, rootItem) )
	{
		questionGuid	= rootItem["identifier"];
		questionType	= rootItem["question_type"];
		questionName	= rootItem["title"];

		strGuid			= Utf82Str(questionGuid.asString());
		strQuestionType	= Utf82Str(questionType.asString());
	}

	delete pBuff;

	return TRUE;
}

void CLocalInteractiveQuestionItemUI::OnButtonClick( int nButtonIndex, TNotifyUI& msg )
{
	tstring strMainXmlPath = GetResource();

	if( nButtonIndex == 0 )
		InsertQuestionByThread(strMainXmlPath.c_str(), m_strTitle.c_str(), m_strGuid.c_str());
	else
	{
		// translate slash
		tstring strQuestionPath = strMainXmlPath;
		for(int i = 0; i < strQuestionPath.length(); i++)
		{
			if( strQuestionPath.at(i) == _T('\\') )
				strQuestionPath.replace(i, 1, _T("/"));
		}

		// js player path
		tstring strLocalPath = GetLocalPath();
		tstring strPlayerPath=GetHtmlPlayerPath();
		//TCHAR szPlayerPath[MAX_PATH*2] = {0};
		TCHAR szParam[MAX_PATH*3]={0};

 
		_stprintf_s(szParam, _T("%s?main=/%s&sys=pptshell&hidePage=footer"), 
			UrlEncode(Str2Utf8(strPlayerPath)).c_str(), 
			UrlEncode(Str2Utf8(strQuestionPath)).c_str());


		//tstring strExePath = GetCoursePlayerExePath();
	
		//tstring strWorkDirectory = GetCoursePlayerPath();

		CCoursePlayUI * pCoursePlayUI = CoursePlayUI::GetInstance();
		pCoursePlayUI->SetQuestionInfo(strMainXmlPath, m_strTitle, m_strGuid);
		pCoursePlayUI->Init((WCHAR *)Str2Unicode(szParam).c_str(),COURSEPLAY_PREVIEW);
		//ShellExecute(NULL, "open", strExePath.c_str(), szParam, strWorkDirectory.c_str(), SW_SHOWNORMAL);
	}
}

void CLocalInteractiveQuestionItemUI::OnItemClick( TNotifyUI& msg )
{

	CItemExplorerUI::GetInstance()->ShowWindow(false);
}

void CLocalInteractiveQuestionItemUI::ReadStream( CStream* pStream )
{
	m_strTitle			= pStream->ReadString();
	m_strGuid			= pStream->ReadString();
	m_strDesc			= pStream->ReadString();
	m_strQuestionType	= pStream->ReadString();
	m_strPreviewUrl		= pStream->ReadString();

	SetTitle(m_strTitle.c_str());
	SetToolTip(m_strTitle.c_str());
	SetResource(m_strPreviewUrl.c_str());
	SetGroup(_T("LocalQuestionItem"));
}



void CLocalInteractiveQuestionItemUI::DownloadResource( int nButtonIndex, int nType, int nThumbnailSize )
{
	
	CResourceItemUI::m_nButtonIndex = nButtonIndex;

}

 void CLocalInteractiveQuestionItemUI::OnDownloadResourceCompleted( int nButtonIndex, LPCTSTR lptcsPath )
 {	 
	 tstring strMainXmlPath =  lptcsPath;
	 strMainXmlPath += _T("\\main.xml");

	 BOOL res = NDCloudQuestionManager::GetInstance()->ConvertQuestionToCourseFile(m_strGuid, m_strQuestionType, m_strQuestionType, lptcsPath);
	 if( !res )
		 return;

	 if( nButtonIndex == 0 )
		InsertQuestionByThread(strMainXmlPath.c_str(), m_strTitle.c_str(), m_strGuid.c_str());
	 
	 else
	 {
		 // translate slash
		 tstring strQuestionPath = strMainXmlPath;
		 for(int i = 0; i < strQuestionPath.length(); i++)
		 {
			 if( strQuestionPath.at(i) == _T('\\') )
				 strQuestionPath.replace(i, 1, _T("/"));
		 }

		 // js player path
		 tstring strLocalPath = GetLocalPath();
		 tstring strPlayerPath= GetHtmlPlayerPath();
		// TCHAR szPlayerPath[MAX_PATH*2] = {0};
		 TCHAR szParam[MAX_PATH*3]={0};

 
		 _stprintf_s(szParam, _T("%s?main=/%s&sys=pptshell&hidePage=footer"), 
			 UrlEncode(Str2Utf8(strPlayerPath)).c_str(), 
			 UrlEncode(Str2Utf8(strQuestionPath)).c_str());


		 //tstring strExePath=GetCoursePlayerExePath();
 
		 //tstring strWorkDirectory = GetCoursePlayerPath();


		 CCoursePlayUI * pCoursePlayUI = CoursePlayUI::GetInstance();
		 pCoursePlayUI->SetQuestionInfo(strMainXmlPath, m_strTitle, m_strGuid);
		 pCoursePlayUI->Init((WCHAR *)Str2Unicode(szParam).c_str(),COURSEPLAY_PREVIEW);
		//ShellExecute(NULL, "open", strExePath.c_str(), szParam, strWorkDirectory.c_str(), SW_SHOWNORMAL);
	 }
 }

void CLocalInteractiveQuestionItemUI::OnItemDragFinish()
{
	DownloadResource(0,CloudFileQuestion,0);
}

void CLocalInteractiveQuestionItemUI::OnDownloadThumbnailFailed()
{

}

void CLocalInteractiveQuestionItemUI::EditExercises()
{

	if ( !m_strExercisesDir.empty() )
	{
		NDCloudLocalQuestionManager::GetInstance()->EditQuestion(m_strGuid, m_strQuestionType, m_strExercisesDir);
	}
}

void CLocalInteractiveQuestionItemUI::UploadNetdisc(int currentModeType)
{
	DWORD dwUserID = NDCloudUser::GetInstance()->GetUserId();
	if (dwUserID != 0)
	{
		tstring strResource  = GetResource();
		DWORD dwAttri = ::GetFileAttributes(strResource.c_str());
		if ((dwAttri != -1) && !(dwAttri&FILE_ATTRIBUTE_DIRECTORY))
		{
			tstring strTitle = GetTitle();
			tstring strToast = _T("“") + strTitle + _T("”开始上传...");
			CToast::Toast(strToast);
			CStream* pStream = new CStream(1024);
			pStream->WriteInt(currentModeType);
			pStream->WriteInt(dwUserID);
			if(!NDCloudLocalQuestionManager::GetInstance()->UploadQuestion(strTitle,m_strGuid,false,MakeHttpDelegate(this, &CLocalInteractiveQuestionItemUI::OnQuestionFolderUploaded),pStream))
			{
				strToast = _T("“") + strTitle + _T("”上传失败");
				CToast::Toast(strToast);
			}
		}
		else
		{
			CToast::Toast(_T("文件不存在"));
		}
	}
}

bool CLocalInteractiveQuestionItemUI::OnQuestionFolderUploaded( void* param )
{
	bool isSuccess = false;
	THttpNotify* pNotify = (THttpNotify*)param;
	tstring strTitle = _T("");
	CStream* pStream = (CStream*)pNotify->pUserData;
	if( pNotify->dwErrorCode == 0 && pStream != NULL )
	{
		pStream->ResetCursor();
		int nResModelType		= pStream->ReadInt();
		tstring strResType		= pStream->ReadString();
		tstring strGuid			= pStream->ReadString();
		tstring strLocalPath	= pStream->ReadString();
		tstring strRemotePath	= pStream->ReadString();
		tstring strPreviewPath	= strRemotePath;
		strTitle				= pStream->ReadString();
		int currentModeType = pStream->ReadInt();
		int userId = pStream->ReadInt();
		// 
		
		CStream * pNewStream = new CStream(1024);
		pNewStream->WriteInt(nResModelType);
		pNewStream->WriteString(strResType.c_str());
		pNewStream->WriteString(strGuid.c_str());
		pNewStream->WriteString(strLocalPath.c_str());
		pNewStream->WriteString(strRemotePath.c_str());
		pNewStream->WriteString(strTitle.c_str());
		pNewStream->WriteInt(currentModeType);
		pNewStream->WriteInt(userId);
		//先请求一个判断ID是否已经存在
		tstring strUrlPrefix;
		if( nResModelType == UPLOAD_QUESTIONS )
		{
			strUrlPrefix = EduPlatformUrlQuestionModel;
		}
		else if( nResModelType == UPLOAD_COURSEWAREOBJECTS )
		{
			strUrlPrefix = EduPlatformUrlCoursewareObjectModel;
		}

		// Authorization header
		TCHAR szUrl[1024];
		_stprintf_s(szUrl, MAX_PATH, strUrlPrefix.c_str(), strGuid.c_str());
		//
		DWORD dwTaskId = HttpDownloadManager::GetInstance()->AddTask(EduPlatformHost, szUrl, _T(""), _T("GET"), "", 80,
			MakeHttpDelegate(this,&CLocalInteractiveQuestionItemUI::OnResIdQueried), 
			MakeHttpDelegate(NULL), 
			MakeHttpDelegate(NULL),TRUE,FALSE,0,pNewStream);
		if( dwTaskId != 0 )
		{
			isSuccess = true;
		}
	}
	if(!isSuccess)
	{
		if(strTitle.empty())
		{
			CToast::Toast(_T("资源上传失败"));
		}
		else
		{
			tstring strToast=_T("“") + strTitle + _T("”上传失败");
			CToast::Toast(strToast);
		}
	}
	return isSuccess;
}

bool CLocalInteractiveQuestionItemUI::OnResIdQueried( void* param )
{
	bool isSuccess = false;
	tstring strTitle=_T("");
	THttpNotify* pNotify = (THttpNotify*)param;
	CStream* pStream = (CStream*)pNotify->pUserData;
	if( pNotify->dwErrorCode == 0 && pStream != NULL )
	{
		pNotify->pData[pNotify->nDataSize] = 0;
		string strContent = Utf8ToAnsi(pNotify->pData);

		Json::Reader reader;
		Json::Value root;
		bool res = reader.parse(strContent, root);
		if(res)
		{
			pStream->ResetCursor();
			int nResModelType		= pStream->ReadInt();
			tstring strResType		= pStream->ReadString();
			tstring strGuid			= pStream->ReadString();
			tstring strLocalPath	= pStream->ReadString();
			tstring strRemotePath	= pStream->ReadString();
			tstring strPreviewPath	= strRemotePath;
			strTitle = pStream->ReadString();
			int currentModeType = pStream->ReadInt();
			int userId = pStream->ReadInt();
			bool bCover = false;
			if( !root["identifier"].isNull() )
			{
				bCover = true;
			}
			CNDCloudResourceModel ResModel;
			tstring strChapterGuid = NDCloudGetChapterGUID();

			TCHAR szUserId[MAX_PATH];
			_stprintf_s(szUserId, _T("%d"), userId);

			ResModel.SetResourceType(nResModelType, strResType);
			ResModel.SetGuid(strGuid);

			// question type
			if(strTitle==_T(""))
			{
				tstring strTitle1 = NDCloudQuestionManager::GetInstance()->GetQuestionResDesc(strResType.c_str());
				if(strTitle1 != _T(""))
					strTitle = strTitle1;
			}
			ResModel.SetBasicInfo(strTitle);

			if( nResModelType == UPLOAD_QUESTIONS )
			{
				strLocalPath += _T("\\item.json");
				strRemotePath += _T("/item.xml");
			}
			else if( nResModelType == UPLOAD_COURSEWAREOBJECTS )
			{
				strLocalPath += _T("\\main.xml");
				strRemotePath += _T("/main.xml");
			}

			strPreviewPath += _T("/main.xml.jpg");
			strPreviewPath = _T("${ref-path}") + strPreviewPath;

			ResModel.SetPreview(_T("question_small"), strPreviewPath);
			ResModel.SetTechInfo(strLocalPath, strRemotePath, strRemotePath);

			ResModel.SetCover(bCover);
			ResModel.SetLifeCycleInfo();
			ResModel.SetLifeCycleStatus(_T("CREATED"));
			ResModel.AddCoverage(_T("User"), szUserId, _T(""), _T("OWNER") );
			ResModel.AddRelation(strChapterGuid);

			CCategoryTree* pCategory;
			NDCloudGetCategoryTree(pCategory);
			ResModel.SetCategoryInfo(pCategory);


			// Commit to database
			CStream* stream=new CStream(1024);
			stream->WriteString(strTitle);
			stream->WriteInt(currentModeType);
			ResModel.CommitResourceModel(MakeHttpDelegate(this, &CLocalInteractiveQuestionItemUI::OnQuestionModelCommited),stream);

			// pack this question
			NDCloudCoursewareObjectsManager::GetInstance()->PackCoursewareObjects(strGuid, (DWORD)userId, MakeHttpDelegate(NULL));
			isSuccess = true;
		}
	}
	if(!isSuccess)
	{
		if(strTitle.empty())
		{
			CToast::Toast(_T("资源上传失败"));
		}
		else
		{
			tstring strToast=_T("“") + strTitle + _T("”上传失败");
			CToast::Toast(strToast);
		}
	}
	if(pStream)
		delete pStream;
	return true;
}

bool CLocalInteractiveQuestionItemUI::OnQuestionModelCommited( void* param )
{
	THttpNotify* pNotify = (THttpNotify*)param;
	tstring strTitle = _T("");
	int currentModeType=0;
	if(pNotify->pUserData)
	{
		CStream* pStream = (CStream*)pNotify->pUserData;
		if(pStream)
		{
			pStream->ResetCursor();
			strTitle		= pStream->ReadString();
			currentModeType = pStream->ReadInt();
			delete pStream;
		}
	}
	bool isSuccess = false;
	if( pNotify->dwErrorCode == 0 )
	{
		if(pNotify->nDataSize>0)
		{
			pNotify->pData[pNotify->nDataSize] = '\0';
			string strContent = pNotify->pData;
			Json::Reader reader;
			Json::Value root;
			bool res = reader.parse(strContent, root);
			if(res)
			{
				if( root["code"].isNull() )
				{
					isSuccess = true;
				}
			}
		}
	}
	if(!isSuccess)
	{
		if(strTitle.empty())
		{
			CToast::Toast(_T("资源上传失败"));
		}
		else
		{
			tstring strToast=_T("“") + strTitle + _T("”上传失败");
			CToast::Toast(strToast);
		}
	}
	else
	{
		if(strTitle.empty())
		{
			CToast::Toast(_T("资源上传成功"));
		}
		else
		{
			tstring strToast=_T("“") + strTitle + _T("”上传成功");
			CToast::Toast(strToast);
		}
	}
	BroadcastEvent(EVT_MENU_UPLOADNETDISC_COMPELETE,(WPARAM)pNotify->dwErrorCode,(LPARAM)currentModeType);
	return true;
}

bool CLocalInteractiveQuestionItemUI::OnQuestionThumbnail( void* param )
{
	TEventNotify* pNotify	= (TEventNotify*)param;
	CStream* pStream		= (CStream*)pNotify->wParam;
	tstring strPath			= pStream->ReadString();
	SetImage(strPath.c_str());

	if ( this->IsVisible() )
		Invalidate();
		
	return 1;
}
