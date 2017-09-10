//-----------------------------------------------------------------------
// FileName:				NDCloudLocalQuestion.cpp
//
// Desc:
//------------------------------------------------------------------------
#include "stdafx.h"
#include "Util/Util.h"
#include "Util/FileTypeFilter.h"
#include "NDCloudLocalQuestion.h"
#include "NDCloudQuestion.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "NDCefLib/NdCefShareIpc.h"
#include "NDCloudFile.h"
#include "NDCloudUser.h"
#include "ChapterTree.h"
#include "NDCloudAPI.h"
#include "NDCloudUpload.h"
#include "NDCloudResModel.h"
#include "DUI/CoursePlayUI.h"
#include "DUI/GroupExplorer.h"
#include "DUI/ExercisesEditFloatWindow.h"

CNDCloudLocalQuestionManager::CNDCloudLocalQuestionManager()
{
	::OnEvent(EVT_SET_CHAPTER_GUID,		MakeEventDelegate(this, &CNDCloudLocalQuestionManager::OnChapterChange));
}
 
CNDCloudLocalQuestionManager::~CNDCloudLocalQuestionManager()
{

}

BOOL CNDCloudLocalQuestionManager::Initialize()
{
	// execute node.js
	//tstring strNDCloudDirectory = NDCloudFileManager::GetInstance()->GetNDCloudDirectory();
	
	tstring strWorkDirectory = GetLocalPath();
	strWorkDirectory += _T("\\Package\\nodejs");

	tstring strExePath = strWorkDirectory;
	strExePath += _T("\\node.exe");

	ShellExecute(NULL, "open", strExePath.c_str(), "app.js", strWorkDirectory.c_str(), SW_HIDE);

	return TRUE;
}

BOOL CNDCloudLocalQuestionManager::Destroy()
{
	KillExeCheckParentPid("node.exe");
	//KillExeCheckParentPid("CoursePlayer.exe");
	return TRUE;
}

//
// Add question
//
BOOL CNDCloudLocalQuestionManager::CreateQuestion()
{
	
	DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();

	TCHAR szUserId[MAX_PATH];
	_stprintf_s(szUserId, _T("%d"), dwUserId);

	tstring strChapterGuid = NDCloudGetChapterGUID();
	tstring strChapterName; 

	CChapterTree* pChapterTree = NULL;
	NDCloudGetChapterTree(pChapterTree);

	if( pChapterTree != NULL )
	{
		ChapterNode* pNode = pChapterTree->FindNode(strChapterGuid);
		if( pNode != NULL )
			strChapterName = pNode->strTitle;
	}

	string strUtf8ChapterGuid = Str2Utf8(strChapterGuid);
	string strUtf8ChapterName = Str2Utf8(strChapterName);
	string strUtf8UserId      = Str2Utf8(szUserId);

	char szUrl[2048];
	_stprintf_s(szUrl, _T("http://localhost:3001/prepare/index.html?chapter_id=%s&chapter_name=%s&creator=%s"), strUtf8ChapterGuid.c_str(), strUtf8ChapterName.c_str(), strUtf8UserId.c_str());

	CCoursePlayUI * pCoursePlayUI = CoursePlayUI::GetInstance();
	pCoursePlayUI->Init((WCHAR *)Utf8ToUnicode(szUrl).c_str(),COURSEPLAY_ADD);

	return TRUE;
}


//
// Insert question
//
BOOL CNDCloudLocalQuestionManager::InsertQuestion(tstring strGuid, bool bBasicQuestion, bool bInsertQuestion)
{
	if (bInsertQuestion)
	{
		CCoursePlayUI * pCoursePlayUI = CoursePlayUI::GetInstance();
		pCoursePlayUI->Hide();
	}

	TCHAR szPath[1024];
	TCHAR szMetaFilePath[1024];
	TCHAR szThumbnailPath[1024];

	tstring strAppDir = GetLocalPath();
	tstring strQuestionFolder = strAppDir;
	tstring strQuestionType = _T("");
	

	if( bBasicQuestion )
	{
		// basic question
		strQuestionFolder += _T("\\Package\\nodejs\\app\\userdatas\\edu\\esp\\questions");
		_stprintf_s(szPath, _T("%s\\%s.pkg\\main.xml"), strQuestionFolder.c_str(), strGuid.c_str());
		_stprintf_s(szThumbnailPath, _T("%s\\%s.pkg\\main.xml.jpg"), strQuestionFolder.c_str(), strGuid.c_str());
		_stprintf_s(szMetaFilePath, _T("%s\\%s.pkg\\metadata.json"), strQuestionFolder.c_str(), strGuid.c_str());	
	}
	else
	{
		// interaction
		strQuestionFolder += _T("\\Package\\nodejs\\app\\userdatas\\edu\\esp\\interaction");
		_stprintf_s(szPath, _T("%s\\%s.pkg\\main.xml"), strQuestionFolder.c_str(), strGuid.c_str());
		_stprintf_s(szThumbnailPath, _T("%s\\%s.pkg\\main.xml.jpg"), strQuestionFolder.c_str(), strGuid.c_str());
		_stprintf_s(szMetaFilePath, _T("%s\\%s.pkg\\metadata.json"), strQuestionFolder.c_str(), strGuid.c_str());
	}

	DeleteFile(szThumbnailPath);

	GetQuestionInfo(szMetaFilePath, strGuid, strQuestionType);
	tstring strTitle = NDCloudQuestionManager::GetInstance()->GetQuestionResDesc(strQuestionType.c_str());

	// generate 
	//tstring strThumbPath = GenerateQuestionThumbnail(str);
	
	if ( bInsertQuestion )
		InsertUpdateQuestionByThread(strTitle.c_str(),bBasicQuestion,szPath, _T(""), strGuid.c_str());

	// save to local resource storage
	int nQuestionType = bBasicQuestion ? FILE_FILTER_BASIC_EXERCISES : FILE_FILTER_INTERACTIVE_EXERCISES;

	CStream stream(256);
	stream.WriteDWORD(nQuestionType);
	stream.WriteString(strTitle);
	stream.WriteString(szPath);
	stream.ResetCursor();
	BroadcastEvent(EVT_LOCALRES_ADD_FILE, &stream);

	
	BOOL result = TRUE;
	//result = UploadQuestion(strTitle,strGuid, bBasicQuestion, MakeHttpDelegate(NULL));

	// interactive question add reportable
	if( !bBasicQuestion )
	{
		// add reportable = true attribute
		TiXmlDocument doc;
		bool res = doc.LoadFile(szPath);
		if( !res )
			return FALSE;

		TiXmlElement* pRootElement = doc.FirstChildElement();
		TiXmlElement* pPagesElement = GetElementsByTagName(pRootElement, "pages");
		if( pPagesElement == NULL )
			return FALSE;

		TiXmlElement* pPageElement = pPagesElement->FirstChildElement();
		while( pPageElement != NULL )
		{
			pPageElement->SetAttribute(_T("id"),strGuid.c_str());
			pPageElement->SetAttribute(_T("name"),strGuid.c_str());
			pPageElement->SetAttribute(_T("reportable"),_T("true"));
			pPageElement = pPageElement->NextSiblingElement();
		}

		doc.SaveFile(szPath); 
	}

	return result;
}

BOOL CNDCloudLocalQuestionManager::UploadQuestion(tstring strTitle,tstring strGuid,bool bBasicQuestion,CHttpDelegateBase &OnCompleteDelegate,void* pUserData)
{
	// upload to personal disk
	bool isCloud=false;
	bool result = true;
	DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();
	if( dwUserId != 0 )
	{
		TCHAR szMetaFilePath[1024];//json文件路径
		bool fileExist = true;
		//
		tstring strQuestionFolder = GetLocalPath() + "\\Package\\nodejs\\app\\userdatas\\edu\\esp\\"+(bBasicQuestion?"questions":"interaction")+"\\"+strGuid;
		strQuestionFolder+=".pkg";
		_stprintf_s(szMetaFilePath, _T("%s\\metadata.json"), strQuestionFolder.c_str());
		WIN32_FIND_DATA FindFileData;
		HANDLE hFile = ::FindFirstFile(szMetaFilePath, &FindFileData);
		if (hFile == INVALID_HANDLE_VALUE)//本地路径不存在的情况下，视为从教育资源库下载，从NDCloud文件夹中获取
		{
			isCloud = true;
			strQuestionFolder = NDCloudFileManager::GetInstance()->GetNDCloudDirectory()+"\\question\\"+strGuid+"_default";
			_stprintf_s(szMetaFilePath, _T("%s\\metadata.json"), strQuestionFolder.c_str());
			WIN32_FIND_DATA FindFileData;
			HANDLE hFile = ::FindFirstFile(szMetaFilePath, &FindFileData);
			if (hFile == INVALID_HANDLE_VALUE)
			{
				result = fileExist = false;
			}
		}
		//
		tstring strFolder = strQuestionFolder;
		if(fileExist)
		{
			tstring strQuestionType = _T("");
			GetQuestionInfo(szMetaFilePath, strGuid, strQuestionType);
			if(OnCompleteDelegate.GetFn()&&OnCompleteDelegate.GetObject())
			{
				NDCloudUploadManager::GetInstance()->UploadFolder(bBasicQuestion ? UPLOAD_QUESTIONS : UPLOAD_COURSEWAREOBJECTS, 
					dwUserId, 
					strQuestionType.c_str(),
					strTitle.c_str(),
					strGuid.c_str(),
					strFolder, 
					OnCompleteDelegate, 
					MakeHttpDelegate(NULL),pUserData);
			}
			else
			{
				NDCloudUploadManager::GetInstance()->UploadFolder(bBasicQuestion ? UPLOAD_QUESTIONS : UPLOAD_COURSEWAREOBJECTS, 
					dwUserId, 
					strQuestionType.c_str(),
					strTitle.c_str(),
					strGuid.c_str(),
					strFolder, 
					MakeHttpDelegate(this, &CNDCloudLocalQuestionManager::OnQuestionFolderUploaded), 
					MakeHttpDelegate(NULL),pUserData);
			}
		}
	}
	return result;
}

//
// update question
//

BOOL CNDCloudLocalQuestionManager::UpdateQuestion(tstring strGuid, bool bBasicQuestion)
{
	TCHAR szPath[1024];
	TCHAR szMetaFilePath[1024];
	TCHAR szThumbnailPath[1024];

	tstring strQuestionType;

	tstring strPath = CoursePlayUI::GetInstance()->GetQuestionPath();

	_stprintf_s(szPath, _T("%s\\main.xml"), strPath.c_str());
	_stprintf_s(szThumbnailPath, _T("%s\\main.xml.jpg"), strPath.c_str());
	_stprintf_s(szMetaFilePath, _T("%s\\metadata.json"), strPath.c_str());	
	
	DeleteFile(szThumbnailPath);

	GetQuestionInfo(szMetaFilePath, strGuid, strQuestionType);

	UpdateQuestionByThread(szPath, strGuid.c_str());

	tstring strTitle = NDCloudQuestionManager::GetInstance()->GetQuestionResDesc(strQuestionType.c_str());

	//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CreateQuestionThunbnailFuc, szPath, 0, NULL);

	//zcs 
	// save to local resource storage
	int nQuestionType = bBasicQuestion ? FILE_FILTER_BASIC_EXERCISES : FILE_FILTER_INTERACTIVE_EXERCISES;
	CStream stream(256);
	stream.WriteDWORD(nQuestionType);
	stream.WriteString(strTitle);
	stream.WriteString(szPath);
	stream.ResetCursor();
	BroadcastEvent(EVT_LOCALRES_ADD_FILE, &stream);

	return TRUE;
}


//
// edit question
//
BOOL CNDCloudLocalQuestionManager::EditQuestion(tstring strGuid, tstring strQuestionType /* = _T */, tstring strQuestionPath /*= _T("")*/)
{
	// http://localhost:3001/questions/question.html#/{uuid}/edit
	// http://localhost:3001/interaction/#/home?question_type={qtype}&id={uuid}&_lang_=zh_CN&space=personal&sys=pptshell

	CCoursePlayUI * pCoursePlayUI = CoursePlayUI::GetInstance();
	if ( pCoursePlayUI != NULL )
	{
		tstring strNDCloudPath = NDCloudFileManager::GetInstance()->GetNDCloudDirectory();
		TCHAR szUrl[2048];

		if( strQuestionPath.find(strNDCloudPath) != -1 )
		{
			tstring strQuestionBase;

			// find "question"
			int pos = strQuestionPath.rfind(_T("\\"));
			if( pos != -1 )
				strQuestionBase = strQuestionPath.substr(0, pos);

			// translate slash
			for(int i = 0; i < strQuestionBase.length(); i++)
			{
				if( strQuestionBase.at(i) == _T('\\') )
					strQuestionBase.replace(i, 1, _T("/"));
			}

			// use /Question as base cos we supply question id already
			_stprintf_s(szUrl, _T("http://localhost:3001/prepare/edit.html?id=%s&question_type=%s&question_base=%s"), strGuid.c_str(), strQuestionType.c_str(), strQuestionBase.c_str());
		}
		else
			_stprintf_s(szUrl, _T("http://localhost:3001/prepare/edit.html?id=%s&question_type=%s"), strGuid.c_str(), strQuestionType.c_str());

		pCoursePlayUI->Init((WCHAR *)AnsiToUnicode(szUrl).c_str(), COURSEPLAY_EDITOR);
		pCoursePlayUI->SetQuestionPath(strQuestionPath);
	}

	return TRUE;
}

BOOL CNDCloudLocalQuestionManager::EditQuestion(tstring strPath)
{
	return TRUE;
}


//
// notify courseplayer.exe when chapter changed
//
bool CNDCloudLocalQuestionManager::OnChapterChange(void* pNotify)
{
	TEventNotify* pEventNotify = (TEventNotify*)pNotify;

	HWND hWnd = FindWindow(_T("CoursePlayer"), NULL);
	HWND hMainWnd = AfxGetApp()->m_pMainWnd->GetSafeHwnd();

	// chapter id / chapter name / user id
	DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();

	TCHAR szUserId[MAX_PATH];
	_stprintf_s(szUserId, _T("%d"), dwUserId);

	tstring strChapterGuid = NDCloudGetChapterGUID();
	tstring strChapterName; 

	CChapterTree* pChapterTree = NULL;
	NDCloudGetChapterTree(pChapterTree);

	if( pChapterTree != NULL )
	{
		ChapterNode* pNode = pChapterTree->FindNode(strChapterGuid);
		if( pNode != NULL )
			strChapterName = pNode->strTitle;
	}

	// 
	//string strUtf8ChapterGuid = Str2Utf8(strChapterGuid);
	//string strUtf8ChapterName = Str2Utf8(strChapterName);
	//string strUtf8UserId      = Str2Utf8(szUserId);

	//CStream stream(1024);
	//stream.WriteString("http://localhost:3001/prepare/index.html?chapter_id=%s&chapter_name=%s&creator=%s");
	//stream.WriteString((char*)strUtf8ChapterGuid.c_str());
	//stream.WriteString((char*)strUtf8ChapterName.c_str());
	//stream.WriteString((char*)strUtf8UserId.c_str());

	//CNdCefShareMemory CefShareMemory;
	//CefShareMemory.CreateFileMemoryMapping(GLOBAL_MEMORY_URL_NAME,GLOBAL_EVENT_URL_NAME);
	//CefShareMemory.WriteMemory(stream.GetBuffer(), stream.GetDataSize(),MEMORY_URL_SIZE);

	//SendMessage(hWnd, WM_USER_CPLAYER_SET_MESSAGE_WND, (WPARAM)hMainWnd, NULL);
	return true;
}

bool CNDCloudLocalQuestionManager::OnQuestionFolderUploaded(void *param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	CStream* pStream = (CStream*)pNotify->pUserData;
	if( pNotify->dwErrorCode != 0 || pStream == NULL )
	{
		return false;
	}
	pStream->ResetCursor();
	int nResModelType		= pStream->ReadInt();
	tstring strResType		= pStream->ReadString();
	tstring strGuid			= pStream->ReadString();
	tstring strLocalPath	= pStream->ReadString();
	tstring strRemotePath	= pStream->ReadString();
	tstring strPreviewPath	= strRemotePath;

	// 
	CNDCloudResourceModel ResModel;
	DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();

	tstring strChapterGuid = NDCloudGetChapterGUID();

	TCHAR szUserId[MAX_PATH];
	_stprintf_s(szUserId, _T("%d"), dwUserId);

	ResModel.SetResourceType(nResModelType, strResType);
	ResModel.SetGuid(strGuid);

	tstring strTitle = NDCloudQuestionManager::GetInstance()->GetQuestionResDesc(strResType.c_str());
	
	if( nResModelType == UPLOAD_QUESTIONS )
	{
		strLocalPath += _T("\\item.json");
		strRemotePath += _T("/item.xml");

		if( strTitle == _T("") )
			strTitle = _T("基础题型");
	}
	else if( nResModelType == UPLOAD_COURSEWAREOBJECTS )
	{
		strLocalPath += _T("\\main.xml");
		strRemotePath += _T("/main.xml");

		if( strTitle == _T("") )
			strTitle = _T("互动题型");
	}

	// title
	ResModel.SetBasicInfo(strTitle);

	// preview
	strPreviewPath += _T("/main.xml.jpg");
	strPreviewPath = _T("${ref-path}") + strPreviewPath;

	ResModel.SetPreview(_T("question_small"), strPreviewPath);
	ResModel.SetTechInfo(strLocalPath, strRemotePath, strRemotePath);
	 

	ResModel.SetLifeCycleInfo();
	ResModel.SetLifeCycleStatus(_T("CREATED"));
	ResModel.AddCoverage(_T("User"), szUserId, _T(""), _T("OWNER") );
	ResModel.AddRelation(strChapterGuid);

	CCategoryTree* pCategory;
	NDCloudGetCategoryTree(pCategory);
	ResModel.SetCategoryInfo(pCategory);


	// Commit to database
	CStream* stream=new CStream(1024);
	stream->WriteInt(nResModelType);
	ResModel.CommitResourceModel(MakeHttpDelegate(this, &CNDCloudLocalQuestionManager::OnQuestionModelCommited),stream);
	return true;
}

bool CNDCloudLocalQuestionManager::OnQuestionModelCommited(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	if( pNotify->dwErrorCode == 0 )
	{
		int nType = 0;
		CStream* pStream = (CStream*)pNotify->pUserData;
		if(pStream)
		{
			pStream->ResetCursor();
			nType = pStream->ReadInt();
			delete pStream;
		}
		if(pNotify->nDataSize>0)
		{
			pNotify->pData[pNotify->nDataSize] = '\0';
			string strContent = Utf8ToAnsi(pNotify->pData);
			Json::Reader reader;
			Json::Value root;
			bool res = reader.parse(strContent, root);
			if(res)
			{
				if( root["code"].isNull() )
				{
					if ( nType == UPLOAD_QUESTIONS )
						CGroupExplorerUI::GetInstance()->AddDBankItemCount(DBankQuestion);
					else if ( nType == UPLOAD_COURSEWAREOBJECTS )
						CGroupExplorerUI::GetInstance()->AddDBankItemCount(DBankCoursewareObjects);
				}
			}
		}
	}
	return true;
}

BOOL CNDCloudLocalQuestionManager::GetQuestionInfo( tstring strJosnPath, tstring& strGuid, tstring& strQuestionType )
{
	tstring strQuestionCode;
	return GetQuestionCodeInfo( strJosnPath,strGuid, strQuestionType,strQuestionCode);
}
BOOL CNDCloudLocalQuestionManager::GetQuestionCodeInfo( tstring strJosnPath, tstring& strGuid, tstring& strQuestionType,tstring& strQuestionCode )
{
	FILE* pf = _tfopen(strJosnPath.c_str(), _T("r"));

	if ( pf == NULL )
		return false;

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

	if ( !jsonReader.parse(pBuff, rootItem) )
	{
		delete pBuff;
		return FALSE;
	}
	
	questionGuid	= rootItem["identifier"];
	questionType	= rootItem["question_type"];

	if( !questionGuid.isNull() )
		strGuid	= questionGuid.asCString();

	if( !questionType.isNull() )
		strQuestionType	= questionType.asCString();
		if( !rootItem["categories"].isNull() && !rootItem["categories"]["res_type"].isNull() )
		{
			Json::Value& resTypeItems = rootItem["categories"]["res_type"];

			for(int i = 0; i < (int)resTypeItems.size(); i++)
			{
				tstring strTaxoncode = Ansi2Str(resTypeItems[i]["taxoncode"].asCString());

				tstring strQuestionTypetemp = NDCloudQuestionManager::GetInstance()->GetQuestionResType(strTaxoncode.c_str());
				if (strQuestionTypetemp.length()==0){
					continue;
				}
				if (strQuestionType.length()==0&&strQuestionTypetemp.length()>0){
					strQuestionType = strQuestionTypetemp;
				}
				if (strQuestionType==strQuestionTypetemp){
					strQuestionCode = strTaxoncode;
					break;
				}
			}
		}
	else
	{
		// categories -> res_type  
		if( !rootItem["categories"].isNull() && !rootItem["categories"]["res_type"].isNull() )
		{
			Json::Value& resTypeItems = rootItem["categories"]["res_type"];

			for(int i = 0; i < (int)resTypeItems.size(); i++)
			{
				tstring strTaxoncode = Ansi2Str(resTypeItems[i]["taxoncode"].asCString());
		
				strQuestionType = NDCloudQuestionManager::GetInstance()->GetQuestionResType(strTaxoncode.c_str());
				if (strQuestionType.length()>0){
					strQuestionCode = strTaxoncode;
					break;
				}
			}
		}

	}

	delete pBuff;
	return true;
}