#include "StdAfx.h"
#include "NdpCourseItem.h"
#include "ItemExplorer.h"
#include "NDCloud/NDCloudAPI.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "Statistics/Statistics.h"
#include "DUI/GuideDialog.h"
#include "UpdateDialog.h"
#include "GroupExplorer.h"
#include "json/json.h"
#include "ThirdParty/zip/ZipWrapper.h"
#include "CoursePlayUI.h"

CNdpCourseItemUI::CNdpCourseItemUI()
{
	m_dwPreviewsDownloadId	= 0;
	m_eContentType			= CONTENT_COURSEWARE;
}

CNdpCourseItemUI::~CNdpCourseItemUI()
{
	CStream* pStream = (CStream*)this->GetTag();
	if (pStream)
	{
		delete pStream;
		this->SetTag(NULL);
	}
	NDCloudDownloadCancel(m_dwPreviewsDownloadId);
}

void CNdpCourseItemUI::Init()
{
	__super::Init();
	CControlUI* pCtrl = FindSubControl(_T("btn2"));
	if (pCtrl)
	{
		pCtrl->SetVisible(false);
	}

	pCtrl = FindSubControl(_T("toolbar"));
	if (pCtrl)
	{
		pCtrl->SetFixedHeight(30);
	}

	SetImage(_T("RightBar\\Item\\item_bg_course.png"));


	SetIcon(_T("RightBar\\Item\\bg_tit_class.png"));
	SetContentHeight(80);
	//SetFixedWidth(130);
	
}

LPCTSTR CNdpCourseItemUI::GetButtonText( int nIndex )
{
	if (nIndex == 0)
	{
		return _T("播放");
	}

	return __super::GetButtonText(nIndex);

}

void CNdpCourseItemUI::OnButtonClick( int nButtonIndex, TNotifyUI& msg )
{
	DownloadResource(nButtonIndex, CloudFileCourse, 0);
}


void CNdpCourseItemUI::OnItemClick( TNotifyUI& msg )
{
	CItemExplorerUI::GetInstance()->ShowWindow(false);
}

void CNdpCourseItemUI::ReadStream( CStream* pStream )
{
	m_strTitle		= pStream->ReadString();
	m_strGuid		= pStream->ReadString().c_str();

	SetTitle(m_strTitle.c_str());
	SetToolTip(m_strTitle.c_str());
	SetResource(pStream->ReadString().c_str());


	int nPreviewsCount	= pStream->ReadInt();

	for (int i = 0; i < nPreviewsCount ; ++i)
	{
		pStream->ReadString();
	}

	m_strJsonInfo = pStream->ReadString();

	SetGroup(_T("CourseItem"));
}

bool CNdpCourseItemUI::OnDownloadPreviewsNotify( void* pNotify )
{
	return true;
}

LPCTSTR CNdpCourseItemUI::GetGuid()
{
	return m_strGuid.c_str();
}

LPCTSTR CNdpCourseItemUI::GetTitle()
{
	return m_strTitle.c_str();
}

LPCTSTR CNdpCourseItemUI::GetPPTUrl()
{
	return GetResource();
}

void CNdpCourseItemUI::DownloadResource( int nButtonIndex, int nType, int nThumbnailSize )
{
	CCloudItemUI::OnDownloadResourceBefore();
	CResourceItemUI::m_nButtonIndex = nButtonIndex;
	this->ShowProgress(true);

// 	if( _tcslen(GetResource() ) > 0)
// 	{
// 		m_dwResourceDownloadId = NDCloudDownloadFile(GetResource(), _T(""), _T(""), CloudFileNdpCourse, 0, MakeHttpDelegate(this, &CNdpCourseItemUI::OnNdpCourseDownloaded), MakeHttpDelegate(this, &CCloudItemUI::OnDownloadResourceProgress));	
// 	}
// 	else
	{
		CHttpDownloadManager* pHttpDownloadManager = HttpDownloadManager::GetInstance();

		TCHAR szPost[1024];
		_stprintf(szPost, _T("/v1.1/coursewares/%s/actions/download"),m_strGuid.c_str());
		if(pHttpDownloadManager)
		{
			CCloudItemUI::m_dwResourceDownloadId = pHttpDownloadManager->AddTask(_T("mooc-lms.web.sdp.101.com"),szPost, _T(""), _T("GET"),"", 80,MakeHttpDelegate(this, &CNdpCourseItemUI::OnGetNdpDownloadUrl) ,MakeHttpDelegate(NULL), MakeHttpDelegate(NULL) );
		}

		if (m_dwResourceDownloadId == 0)
		{
			this->ShowProgress(false);
			return;
		}
	}
	
}

void CNdpCourseItemUI::OnDownloadResourceCompleted( int nButtonIndex, LPCTSTR lptcsPath )
{
	if (nButtonIndex == 0)
	{
		Statistics::GetInstance()->Report(STAT_INSERT_PPT);
	}
}

void CNdpCourseItemUI::OnItemDragFinish()
{
	DownloadResource(0, 0, 0);	
}

bool CNdpCourseItemUI::OnGetNdpDownloadUrl( void * pParam )
{
	THttpNotify* pNotify = static_cast<THttpNotify*>(pParam);

	if(pNotify->dwErrorCode != 0)
	{
		__super::OnDownloadResourceCompleted(pNotify);
	}

	pNotify->pData[pNotify->nDataSize] = 0;
	string str = pNotify->pData;
	Json::Reader	reader;
	Json::Value		result;
	if (reader.parse(str.c_str(), result))
	{
		if (!result.get("md5", Json::Value()).isNull() && !result.get("url", Json::Value()).isNull())
		{
			m_strMd5 = Utf82Str(result["md5"].asCString());
			m_strDownloadUrl = Utf82Str(result["url"].asCString());

			tstring strNdpFilePath = NDCloudFileManager::GetInstance()->GetNDCloudDirectory();
			strNdpFilePath += _T("\\NdpCourse\\");
			strNdpFilePath += m_strGuid;
			strNdpFilePath += _T("_default_webp.zip");

			tstring strMD5 = CalcFileMD5(strNdpFilePath);
			if(strMD5 != m_strMd5)
				DeleteFile(strNdpFilePath.c_str());
			CCloudItemUI::m_dwResourceDownloadId = NDCloudDownloadFile(m_strDownloadUrl, _T(""), _T(""), CloudFileNdpCourse, 0, MakeHttpDelegate(this, &CNdpCourseItemUI::OnNdpCourseDownloaded), MakeHttpDelegate(this, &CCloudItemUI::OnDownloadResourceProgress));	
			return true;
		}

		if (!result.get("code", Json::Value()).isNull())
		{
			tstring strCode = Utf82Str(result["code"].asCString());
			if(strCode == _T("LC/WAIT") )
			{
				CHttpDownloadManager* pHttpDownloadManager = HttpDownloadManager::GetInstance();

				TCHAR szPost[1024];
				_stprintf(szPost, _T("/v1.1/coursewares/%s/actions/download"),m_strGuid.c_str());
				if(pHttpDownloadManager)
				{
					CCloudItemUI::m_dwResourceDownloadId = pHttpDownloadManager->AddTask(_T("mooc-lms.web.sdp.101.com"),szPost, _T(""), _T("GET"), "", 80,MakeHttpDelegate(this, &CNdpCourseItemUI::OnGetNdpDownloadUrl) ,MakeHttpDelegate(NULL), MakeHttpDelegate(NULL) );
				}

				if (m_dwResourceDownloadId == 0)
				{
					__super::OnDownloadResourceCompleted(pNotify);
					return false;
				}

				return true;
			}
		}
	}

	__super::OnDownloadResourceCompleted(pNotify);

	return false;
}

bool CNdpCourseItemUI::OnNdpCourseDownloaded( void * pParam )
{
	THttpNotify* pNotify = (THttpNotify*)pParam;

	// unzip
	if( pNotify->strFilePath != _T("") )
	{
		// folder
		tstring strFolder = pNotify->strFilePath;
		int pos = strFolder.rfind('.');
		if( pos != -1 )
			strFolder = strFolder.substr(0, pos);
	
		CUnZipper UnZipper;
		UnZipper.UnZip(Str2Ansi(pNotify->strFilePath).c_str(), Str2Ansi(strFolder).c_str());

		//用播放器打开
		tstring strXml = strFolder;
		strXml += _T("\\main.xml");
		if(GetFileAttributes(strXml.c_str()) != INVALID_FILE_ATTRIBUTES)
		{
			//tstring strLocalPath = GetLocalPath();

			//TCHAR szPlayerPath[MAX_PATH] = {0};
			tstring strPlayerPath=GetHtmlPlayerPath();
			
	 
			//TCHAR szExePath[MAX_PATH*2]={0};
			tstring strExePath=GetCoursePlayerExePath();
			//_stprintf_s(szExePath, _T("%s\\bin\\CoursePlayer\\CoursePlayer.exe"), strLocalPath.c_str());

			tstring strWorkDirectory = GetCoursePlayerPath();


			PROCESS_INFORMATION pi;
			STARTUPINFO si;    
			ZeroMemory( &pi, sizeof(pi) );
			ZeroMemory( &si, sizeof(si) );
			si.cb = sizeof(si);

			TCHAR szParam[MAX_PATH*2]={0};
			strXml = UrlEncode(Str2Utf8(strXml));
			strPlayerPath = UrlEncode(Str2Utf8(strPlayerPath));

			for(int i = 0; i < strXml.length(); i++)
			{
				if( strXml.at(i) == _T('\\') )
					strXml.replace(i, 1, _T("/"));
			}


			//_stprintf_s(szParam, _T("player file:///%s?main=/%s 播放"), strPlayerPath.c_str(),  strXml.c_str());

			_stprintf_s(szParam, _T("file:///%s?main=/%s&hidePage=toolbar"), strPlayerPath.c_str(),  strXml.c_str());

			CCoursePlayUI * pCoursePlayUI = CoursePlayUI::GetInstance();
			pCoursePlayUI->Init((WCHAR *)Str2Unicode(szParam).c_str(),COURSEPLAY_PLAYER);

			//ShellExecute(NULL, "open", strExePath.c_str(), szParam, strWorkDirectory.c_str(), SW_SHOWMAXIMIZED);
		}
		//
	}
	
	__super::OnDownloadResourceCompleted(pNotify);

	return true;
}
