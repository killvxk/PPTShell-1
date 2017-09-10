#include "StdAfx.h"
#include "QuestionItem.h"
#include "DUI/ItemExplorer.h"
#include "NDCloud/NDCloudUser.h"
#include "NDCloud/NDCloudQuestion.h"
#include "NDCloud/NDCloudAPI.h"
#include "GUI/MainFrm.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "NDCloud/NDCloudFile.h"
#include "EventCenter/EventDefine.h"
#include "DUI/GroupExplorer.h"
#include "CoursePlayUI.h"

CQuestionItemUI::CQuestionItemUI()
{
	m_eContentType	= CONTENT_QUESTIONS;
}

CQuestionItemUI::~CQuestionItemUI()
{
	NDCloudQuestionManager::GetInstance()->CancelDownload(m_strGuid);
}


void CQuestionItemUI::Init()
{
	__super::Init();
	SetContentHeight(110);

	CControlUI* pCtrl = FindSubControl(_T("toolbar"));
	if (pCtrl)
	{
		pCtrl->SetFixedHeight(30);
	}

	SetIcon(_T("RightBar\\Item\\bg_tit_flash.png"));

}

void CQuestionItemUI::OnButtonClick( int nButtonIndex, TNotifyUI& msg )
{
	DownloadResource(nButtonIndex, CloudFileQuestion, 0);
}

void CQuestionItemUI::OnItemClick( TNotifyUI& msg )
{

	CItemExplorerUI::GetInstance()->ShowWindow(false);
}

void CQuestionItemUI::ReadStream( CStream* pStream )
{
	m_strTitle			= pStream->ReadString();
	m_strGuid			= pStream->ReadString();
	m_strDesc			= pStream->ReadString();
	m_strQuestionName	= pStream->ReadString();
	m_strPreviewUrl		= pStream->ReadString();
	m_strXmlUrl			= pStream->ReadString();
	m_strJsonInfo		= pStream->ReadString();
	

	SetTitle(m_strTitle.c_str());
	SetToolTip(m_strTitle.c_str());
	SetResource(m_strPreviewUrl.c_str());
	SetGroup(_T("123"));
}



void CQuestionItemUI::DownloadResource( int nButtonIndex, int nType, int nThumbnailSize )
{
	int nExplorerType = CGroupExplorerUI::GetInstance()->GetCurrentType();
	if( nExplorerType == DBankQuestion )
	{
		// delete question file
		tstring strQuestionFilePath = NDCloudFileManager::GetInstance()->GetNDCloudDirectory();
		strQuestionFilePath += _T("\\Question\\");
		strQuestionFilePath += m_strGuid;
		strQuestionFilePath += _T("_default.zip");

		DeleteFile(strQuestionFilePath.c_str());
	}
	
	CCloudItemUI::OnDownloadResourceBefore();
	CResourceItemUI::m_nButtonIndex = nButtonIndex;
	this->ShowProgress(true);

	BOOL bRet = NDCloudQuestionManager::GetInstance()->DownloadQuestion(m_strGuid, 0,
		MakeHttpDelegate(this, &CCloudItemUI::OnDownloadResourceCompleted),
		MakeHttpDelegate(this, &CCloudItemUI::OnDownloadResourceProgress));
	if (!bRet)
	{
		this->ShowProgress(false);
		return;
	}
}

 void CQuestionItemUI::OnDownloadResourceCompleted( int nButtonIndex, LPCTSTR lptcsPath )
 {	 
	 tstring strMainXmlPath =  lptcsPath;
	 strMainXmlPath += _T("\\main.xml");

	 tstring strQuestionType = NDCloudQuestionManager::GetInstance()->GetQuestionResType(m_strQuestionName.c_str());

	 BOOL res = NDCloudQuestionManager::GetInstance()->ConvertQuestionToCourseFile(m_strGuid, strQuestionType, m_strQuestionName, lptcsPath);
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
		// TCHAR szPlayerPath[MAX_PATH*2] = {0};
		 tstring strPlayerPath=GetHtmlPlayerPath();
		 TCHAR szParam[MAX_PATH*3]={0};

		 tstring strAppDir = GetLocalPath();
		 tstring ntitle = _T("预览");
		 _stprintf_s(szParam, _T("%s?main=/%s&sys=pptshell&hidePage=footer %s"), 
			 UrlEncode(Str2Utf8(strPlayerPath)).c_str(), 
			 UrlEncode(Str2Utf8(strQuestionPath)).c_str(),
			 ntitle.c_str());

		 //tstring strExePath=GetCoursePlayerExePath();
		 //tstring strWorkDirectory = GetCoursePlayerPath();
		
	
		 CCoursePlayUI * pCoursePlayUI = CoursePlayUI::GetInstance();
		 pCoursePlayUI->SetQuestionInfo(strMainXmlPath, m_strTitle, m_strGuid);
		 pCoursePlayUI->Init((WCHAR *)Str2Unicode(szParam).c_str(),COURSEPLAY_PREVIEW);

		 //ShellExecute(NULL, "open", strExePath.c_str(), szParam, strWorkDirectory.c_str(), SW_SHOWNORMAL);
	 }
 }

void CQuestionItemUI::OnItemDragFinish()
{
	DownloadResource(0,CloudFileQuestion,0);
}

void CQuestionItemUI::OnDownloadThumbnailFailed()
{

}

void CQuestionItemUI::Delete(int currentModeType)
{
	bool notify = true;
	DWORD dwUserID = NDCloudUser::GetInstance()->GetUserId();
	if( dwUserID != 0 )
	{
		int pos = m_strXmlUrl.rfind(_T("/item.xml"));
		if( pos == -1 )
			pos = m_strXmlUrl.rfind(_T("/main.xml"));

		if( pos != -1 )
		{
			tstring strQuestionFolder = m_strXmlUrl.substr(0, pos);
			CStream* pStream = new CStream(1024);
			pStream->WriteInt(currentModeType);
			pStream->WriteString(GetResource());
			pStream->WriteString(GetTitle());
			if(NDCloudContentServiceManager::GetInstance()->DeleteCloudFile(dwUserID,m_strGuid,m_eContentType,strQuestionFolder,MakeHttpDelegate(this, &CCloudItemUI::OnDeleteResourceCompelete),pStream))
			{
				notify = false;
			}
		}
	}
	if(notify)
	{
		BroadcastEvent(EVT_MENU_DELETE_COMPELETE,(WPARAM)1,(LPARAM)GetResource(),NULL);
	}
}

void CQuestionItemUI::DownloadLocal()
{
	int nType = CGroupExplorerUI::GetInstance()->GetCurrentType();
	if( nType == DBankQuestion )
	{
		// delete question file
		tstring strQuestionFilePath = NDCloudFileManager::GetInstance()->GetNDCloudDirectory();
		strQuestionFilePath += _T("\\Question\\");
		strQuestionFilePath += m_strGuid;
		strQuestionFilePath += _T("_default.zip");

		DeleteFile(strQuestionFilePath.c_str());
	}

	OnDownloadResourceBefore();

	m_nButtonIndex = 0;
	this->ShowProgress(true);
	CStream* pStream = new CStream(1024);
	pStream->WriteString(GetTitle());
	BOOL bRet = NDCloudQuestionManager::GetInstance()->DownloadQuestion(m_strGuid, 0,
		MakeHttpDelegate(this, &CQuestionItemUI::OnDownloadResourceCompleted2),
		MakeHttpDelegate(this, &CCloudItemUI::OnDownloadResourceProgress), pStream);
	if (!bRet)
	{
		this->ShowProgress(false);
	}
}

bool CQuestionItemUI::OnDownloadResourceCompleted2( void* pNotify )
{
	THttpNotify* pHttpNotify = (THttpNotify*)pNotify;
	tstring strTitle = _T("");
	if(pHttpNotify->pUserData)
	{
		CStream* pStream = (CStream*)pHttpNotify->pUserData;
		if(pStream)
		{
			pStream->ResetCursor();
			strTitle = pStream->ReadString();
			delete pStream;
		}
	}
	if(pHttpNotify->strFilePath != _T(""))
	{
		tstring strMainXmlPath =  pHttpNotify->strFilePath;
		strMainXmlPath += _T("\\main.xml");

		tstring strQuestionType = NDCloudQuestionManager::GetInstance()->GetQuestionResType(m_strQuestionName.c_str());

		BOOL res = NDCloudQuestionManager::GetInstance()->ConvertQuestionToCourseFile(m_strGuid, strQuestionType, m_strQuestionName, pHttpNotify->strFilePath);
		if( res )
		{
			//广播下载完成
			vector<tstring> arg;
			arg.push_back(strTitle);
			arg.push_back(strMainXmlPath);
			BroadcastEvent(EVT_MENU_DOWNLOADLOCAL_COMPELETE,(WPARAM)6,(LPARAM)&arg,NULL);
			tstring strToast = _T("“") + strTitle + _T("”下载完成");
			CToast::Toast(strToast);
		}
		else
		{
			tstring strToast = _T("“") + strTitle + _T("”下载失败");
			CToast::Toast(strToast);
		}
	}
	else
	{
		tstring strToast = _T("“") + strTitle + _T("”下载失败");
		CToast::Toast(strToast);
	}
	CControlUI* pCtrl = FindSubControl(_T("empty"));
	if(pCtrl)
		pCtrl->SetToolTip(_T(""));
	this->SetProgress(m_proDownload->GetMaxValue());
	this->ShowProgress(false);
	this->SetProgress(0);
	return true;
}