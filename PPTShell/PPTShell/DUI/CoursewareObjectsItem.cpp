#include "StdAfx.h"
#include "CoursewareObjectsItem.h"
#include "DUI/ItemExplorer.h"
#include "NDCloud/NDCloudUser.h"
#include "NDCloud/NDCloudCoursewareObjects.h"
#include "NDCloud/NDCloudAPI.h"
#include "GUI/MainFrm.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "DUI/CoursePlayUI.h"

CCoursewareObjectsItemUI::CCoursewareObjectsItemUI()
{
	m_eContentType	= CONTENT_COUSEWAREOBJECTS;
}

CCoursewareObjectsItemUI::~CCoursewareObjectsItemUI()
{
	NDCloudCoursewareObjectsManager::GetInstance()->CancelDownload(m_strGuid);
}


void CCoursewareObjectsItemUI::Init()
{
	__super::Init();
	SetContentHeight(110);

	CControlUI* pCtrl = FindSubControl(_T("toolbar"));
	if (pCtrl)
	{
		pCtrl->SetFixedHeight(30);
	}

	m_lbImage->SetBkImage(_T("RightBar\\Item\\item_bg_course.png"));
	SetIcon(_T("RightBar\\Item\\bg_tit_flash.png"));
	DownloadThumbnail();

}

void CCoursewareObjectsItemUI::OnButtonClick( int nButtonIndex, TNotifyUI& msg )
{
	DownloadResource(nButtonIndex, CloudFileCoursewareObjects, 0);
}

void CCoursewareObjectsItemUI::OnItemClick( TNotifyUI& msg )
{

	CItemExplorerUI::GetInstance()->ShowWindow(false);
}

void CCoursewareObjectsItemUI::ReadStream( CStream* pStream )
{
	m_strTitle					= pStream->ReadString();
	m_strGuid					= pStream->ReadString();
	m_strDesc					= pStream->ReadString();
	m_strCoursewareObjectName	= pStream->ReadString();
	m_strPreviewUrl				= pStream->ReadString();
	m_strXmlUrl					= pStream->ReadString();
	m_strJsonInfo				= pStream->ReadString();

	SetTitle(m_strTitle.c_str());
	SetToolTip(m_strTitle.c_str());
	SetResource(m_strPreviewUrl.c_str());
	SetGroup(_T("123"));
}


void CCoursewareObjectsItemUI::DownloadResource( int nButtonIndex, int nType, int nThumbnailSize )
{
	CCloudItemUI::OnDownloadResourceBefore();
	CResourceItemUI::m_nButtonIndex = nButtonIndex;
	this->ShowProgress(true);

	BOOL bRet = NDCloudCoursewareObjectsManager::GetInstance()->DownloadCoursewareObjects(m_strGuid, 0, 
		MakeHttpDelegate(this, &CCloudItemUI::OnDownloadResourceCompleted),
		MakeHttpDelegate(this, &CCloudItemUI::OnDownloadResourceProgress));
	if (!bRet)
	{
		this->ShowProgress(false);
		return;
	}
}

 void CCoursewareObjectsItemUI::OnDownloadResourceCompleted( int nButtonIndex, LPCTSTR lptcsPath )
 {
	 tstring strPath = lptcsPath; 
	 tstring strMainXmlPath = strPath;
	 strMainXmlPath += _T("\\main.xml");

 	 if( nButtonIndex == 0 )
	 {
		 // add reportable = true attribute
		 TiXmlDocument doc;
		 bool res = doc.LoadFile(Str2Utf8(strMainXmlPath).c_str());
		 if( !res )
			 return;

		 TiXmlElement* pRootElement = doc.FirstChildElement();
		 TiXmlElement* pPagesElement = GetElementsByTagName(pRootElement, "pages");
		 if( pPagesElement == NULL )
			 return;

		 TiXmlElement* pPageElement = pPagesElement->FirstChildElement();
		 while( pPageElement != NULL )
		 {
			 pPageElement->SetAttribute("reportable", "true");
			 pPageElement = pPageElement->NextSiblingElement();
		 }

		 doc.SaveFile(Str2Utf8(strMainXmlPath).c_str()); 

		 //
		 InsertQuestionByThread(strMainXmlPath.c_str(),m_strTitle.c_str(),m_strGuid.c_str());
	 }
	 else
	 {
		 tstring strPath = lptcsPath; 
		 tstring strMainXmlPath = strPath;
		 strMainXmlPath += _T("\\main.xml");

		 // translate slash
		 tstring strQuestionPath = strMainXmlPath;
		 for(int i = 0; i < strQuestionPath.length(); i++)
		 {
			 if( strQuestionPath.at(i) == _T('\\') )
				 strQuestionPath.replace(i, 1, _T("/"));
		 }

		 // js player path
		 tstring strLocalPath = GetLocalPath();
		 tstring strPlayerPath = GetHtmlPlayerPath();
		 TCHAR szParam[MAX_PATH*2]={0};

 
		 _stprintf_s(szParam, _T("%s?main=/%s&sys=pptshell"), 
			 UrlEncode(Str2Utf8(strPlayerPath)).c_str(), 
			 UrlEncode(Str2Utf8(strQuestionPath)).c_str());


		 CCoursePlayUI * pCoursePlayUI = CoursePlayUI::GetInstance();
		 pCoursePlayUI->SetQuestionInfo(strMainXmlPath, m_strTitle, m_strGuid);
		 pCoursePlayUI->Init((WCHAR *)Str2Unicode(szParam).c_str(),COURSEPLAY_PREVIEW);
		 //tstring strExePath=GetCoursePlayerExePath();
	 
		 //tstring strWorkDirectory = GetCoursePlayerPath();

		 //ShellExecute(NULL, "open", strExePath.c_str(), szParam, strWorkDirectory.c_str(), SW_SHOWNORMAL);

	 }
 }

void CCoursewareObjectsItemUI::OnItemDragFinish()
{
	DownloadResource(0,CloudFileCoursewareObjects,0);
}

void CCoursewareObjectsItemUI::OnDownloadThumbnailFailed()
{
	
}

void CCoursewareObjectsItemUI::Delete(int currentModeType)
{
	bool notify = true;
	DWORD dwUserID = NDCloudUser::GetInstance()->GetUserId();
	if( dwUserID != 0 )
	{
		int pos = m_strXmlUrl.rfind(_T("/main.xml"));
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

void CCoursewareObjectsItemUI::DownloadLocal()
{
	OnDownloadResourceBefore();
	tstring strTitle	= GetTitle();
	m_nButtonIndex		= 0;
	this->ShowProgress(true);
	CStream* pStream = new CStream(1024);
	pStream->WriteString(strTitle);
	BOOL bRet = NDCloudCoursewareObjectsManager::GetInstance()->DownloadCoursewareObjects(m_strGuid, 0, 
		MakeHttpDelegate(this, &CCoursewareObjectsItemUI::OnDownloadResourceCompleted2),
		MakeHttpDelegate(this, &CCloudItemUI::OnDownloadResourceProgress),pStream);
	if (!bRet)
	{
		this->ShowProgress(false);
		tstring strToast = _T("“") + strTitle + _T("” 下载失败");
		CToast::Toast(strToast);
	}
}

bool CCoursewareObjectsItemUI::OnDownloadResourceCompleted2( void* pNotify )
{
	bool isSuccess=false;
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
		tstring strPath = pHttpNotify->strFilePath; 
		tstring strMainXmlPath = strPath;
		strMainXmlPath += _T("\\main.xml");

		// add reportable = true attribute
		TiXmlDocument doc;
		bool res = doc.LoadFile(Str2Utf8(strMainXmlPath).c_str());
		if( res )
		{
			TiXmlElement* pRootElement = doc.FirstChildElement();
			TiXmlElement* pPagesElement = GetElementsByTagName(pRootElement, "pages");
			if( pPagesElement != NULL )
			{
				TiXmlElement* pPageElement = pPagesElement->FirstChildElement();
				while( pPageElement != NULL )
				{
					pPageElement->SetAttribute("reportable", "true");
					pPageElement = pPageElement->NextSiblingElement();
				}
				if(doc.SaveFile(Str2Utf8(strMainXmlPath).c_str()))
				{
					isSuccess = true;
					//广播下载完成
					vector<tstring> arg;
					arg.push_back(strTitle);
					arg.push_back(strMainXmlPath);
					BroadcastEvent(EVT_MENU_DOWNLOADLOCAL_COMPELETE,(WPARAM)7,(LPARAM)&arg,NULL);
					tstring strToast = _T("“") + strTitle + _T("”下载完成");
					CToast::Toast(strToast);
				}
			}
		}
	}
	if(!isSuccess)
	{
		tstring strToast = _T("“") + strTitle + _T("”下载失败");
		CToast::Toast(strToast);
	}
	CControlUI* pCtrl	= FindSubControl(_T("empty"));
	if(pCtrl)
		pCtrl->SetToolTip(_T(""));
	this->SetProgress(m_proDownload->GetMaxValue());
	this->ShowProgress(false);
	this->SetProgress(0);
	return true;
}