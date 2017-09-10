#include "StdAfx.h"
#include "CourseItem.h"
#include "ItemExplorer.h"
#include "NDCloud/NDCloudAPI.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "Statistics/Statistics.h"
#include "DUI/GuideDialog.h"
#include "UpdateDialog.h"
#include "GroupExplorer.h"

CCourseItemUI::CCourseItemUI()
{
	m_pPreviewsStream		= NULL;
	m_eContentType = CONTENT_COURSEWARE;
}

CCourseItemUI::~CCourseItemUI()
{
	if (m_pPreviewsStream)
	{
		delete m_pPreviewsStream;
		m_pPreviewsStream = NULL;
	}
}

void CCourseItemUI::Init()
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
	//SetFixedWidth(120);
	
	DownloadThumbnail();
}

LPCTSTR CCourseItemUI::GetButtonText( int nIndex )
{
	if (nIndex == 0)
	{
		return _T("应用");
	}

	return __super::GetButtonText(nIndex);

}

void CCourseItemUI::OnButtonClick( int nButtonIndex, TNotifyUI& msg )
{
	DownloadResource(nButtonIndex, CloudFileCourse, 0);
}


void CCourseItemUI::OnItemClick( TNotifyUI& msg )
{
	CItemExplorerUI::GetInstance()->ShowWindow(true);
	CItemExplorerUI::GetInstance()->ResetUI();

	if (!m_pPreviewsStream)
	{
		return;
	}

	m_pPreviewsStream->ResetCursor();
	if (m_pPreviewsStream->ReadInt() <= 0)
	{
		CItemExplorerUI::GetInstance()->SetParentItem(this);
		return;
	}

	CItemExplorerUI::GetInstance()->ShowResource(eItemExplorer_Cloud, m_pPreviewsStream, this);
	CItemExplorerUI::GetInstance()->StopMask();

}

void CCourseItemUI::ReadStream( CStream* pStream )
{
	m_strTitle		= pStream->ReadString();
	m_strGuid		= pStream->ReadString().c_str();

	SetTitle(m_strTitle.c_str());
	SetToolTip(m_strTitle.c_str());
	SetResource(pStream->ReadString().c_str());

	if (m_pPreviewsStream)
	{
		delete m_pPreviewsStream;
		m_pPreviewsStream = NULL;
	}

	m_pPreviewsStream	= new CStream(1024);
	int nPreviewsCount	= pStream->ReadInt();

	m_pPreviewsStream->WriteDWORD(nPreviewsCount);

	if(nPreviewsCount)
	{
		m_strPreviewUrl		= pStream->ReadString();
		m_pPreviewsStream->WriteString(m_strPreviewUrl);

		for (int i = 0; i < nPreviewsCount - 1; ++i)
		{
			m_pPreviewsStream->WriteString(pStream->ReadString());
		}

	}

	m_strJsonInfo = pStream->ReadString();
	
	SetGroup(_T("CourseItem"));
}

bool CCourseItemUI::OnDownloadPreviewsNotify( void* pNotify )
{

// 	if (!IsVisible())
// 	{
// 		CItemExplorerUI::GetInstance()->ShowWindow(false);
// 	}
// 
// 	if (!IsSelected())
// 	{
// 		return true;
// 	}
// 
// 	THttpNotify* pHttpNotify = (THttpNotify*)pNotify;
// 	CStream* pStream = new CStream(1024);
// 	if (!NDCloudDecodePPTThumbnailList(pHttpNotify->pData, pHttpNotify->nDataSize, pStream))
// 	{
// 		delete pStream;
// 		pStream = NULL;
// 
// 		// report missing
// 		tstring  strPost = "missing_resource=[Thumbnail List Missing]: ";
// 		strPost += m_strPreviewsUrl;
// 		
//  
// 		CHttpDownloadManager* pHttpManager = HttpDownloadManager::GetInstance();
// 		if( pHttpManager )
// 		{
// 			pHttpManager->AddTask(_T("p.101.com"), _T("/101ppt/resourceMissing.php"), _T(""), _T("POST"), strPost.c_str(), 80, 
// 				MakeHttpDelegate(NULL), 
// 				MakeHttpDelegate(NULL), 
// 				MakeHttpDelegate(NULL), 
// 				FALSE);
// 		}
// 
// 	}
// 	CItemExplorerUI::GetInstance()->StopMask();
// 	this->SetTag((UINT_PTR)pStream);
// 
// 	CItemExplorerUI::GetInstance()->ShowResource(eItemExplorer_Cloud, pStream, this);
	return true;
}

LPCTSTR CCourseItemUI::GetGuid()
{
	return m_strGuid.c_str();
}

LPCTSTR CCourseItemUI::GetTitle()
{
	return m_strTitle.c_str();
}

LPCTSTR CCourseItemUI::GetPPTUrl()
{
	return GetResource();
}

void CCourseItemUI::DownloadResource( int nButtonIndex, int nType, int nThumbnailSize )
{
	if(_tcsicmp(GetResource(), _T("")) == 0)
	{
		CToast::Toast(_STR_PREVIEW_DIALOG_NOSUPPORT);
		return;
	}

	CCloudItemUI::OnDownloadResourceBefore();
	CResourceItemUI::m_nButtonIndex = nButtonIndex;
	this->ShowProgress(true);
	CCloudItemUI::m_dwResourceDownloadId = NDCloudDownloadCourseFile(GetResource(), GetGuid(), GetTitle(), CourseFilePPT, MakeHttpDelegate(this, &CCloudItemUI::OnDownloadResourceCompleted), MakeHttpDelegate(this, &CCloudItemUI::OnDownloadResourceProgress));
	if (m_dwResourceDownloadId == 0)
	{
		this->ShowProgress(false);
		return;
	}
}

void CCourseItemUI::OnDownloadResourceCompleted( int nButtonIndex, LPCTSTR lptcsPath )
{
	if (nButtonIndex == 0)
	{
		InsertPPTByThread(lptcsPath, 1, -1);
		//SetFocus();
		Statistics::GetInstance()->Report(STAT_INSERT_PPT);
	}
	else if (nButtonIndex == 1)
	{
		OpenAsDefaultExec(lptcsPath);

	}
}

void CCourseItemUI::OnItemDragFinish()
{
	DownloadResource(0, 0, 0);	
}

void CCourseItemUI::DownloadThumbnail()
{
	this->StartMask();
	CPhotoItemUI::m_dwDownloadId = NDCloudDownloadCourseFile(m_strPreviewUrl.c_str(), this->GetGuid(), this->GetTitle(),  CourseFileThumb, MakeHttpDelegate(this, &CCourseItemUI::OnDownloadThumbnailCompleted));
	if (CPhotoItemUI::m_dwDownloadId == 0)
	{
		this->StopMask();
		return;
	}
}

bool CCourseItemUI::OnDownloadThumbnailCompleted( void* pNotify )
{
	THttpNotify* pHttpNotify = (THttpNotify*)pNotify;
	if (pHttpNotify->strFilePath == _T(""))
	{
		this->StopMask();
		return true;
	}
	SetImage(pHttpNotify->strFilePath.c_str());
	this->StopMask();
	return true;
}

CStream* CCourseItemUI::GetPreviews()
{	
	return m_pPreviewsStream;
}

bool CCourseItemUI::HasSubitems()
{
	return true;
}


void CCourseItemUI::DownloadLocal()
{
	if(_tcsicmp(GetResource(), _T("")) != 0)
	{
		CCloudItemUI::OnDownloadResourceBefore();
		CResourceItemUI::m_nButtonIndex = 0;
		this->ShowProgress(true);
		CStream* pStream = new CStream(1024);
		pStream->WriteString(GetTitle());
		CCloudItemUI::m_dwResourceDownloadId = NDCloudDownloadCourseFile(GetResource(), GetGuid(), GetTitle(), CourseFilePPT, MakeHttpDelegate(this, &CCourseItemUI::OnDownloadResourceCompleted2), MakeHttpDelegate(this, &CCloudItemUI::OnDownloadResourceProgress),pStream);
		if (m_dwResourceDownloadId == 0)
		{
			this->ShowProgress(false);
			string strToast="“";
			strToast+=GetTitle();
			strToast+=+"”下载失败";
			CToast::Toast(strToast);
		}
	}
}

bool CCourseItemUI::OnDownloadResourceCompleted2( void* pNotify )
{
	THttpNotify* pHttpNotify = (THttpNotify*)pNotify;
	tstring strTitle = "";
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
		vector<tstring> arg;
		arg.push_back(strTitle);
		arg.push_back(pHttpNotify->strFilePath);
		//广播下载完成
		BroadcastEvent(EVT_MENU_DOWNLOADLOCAL_COMPELETE,(WPARAM)1,(LPARAM)&arg,NULL);
		string strToast = "“"+ strTitle +"”下载完成";
		CToast::Toast(strToast);
	}
	else
	{
		string strToast = "“"+ strTitle +"”下载失败";
		CToast::Toast(strToast);
	}
	CControlUI* pCtrl	= FindSubControl(_T("empty"));
	if(pCtrl)
		pCtrl->SetToolTip("");
	this->SetProgress(m_proDownload->GetMaxValue());
	this->ShowProgress(false);
	this->SetProgress(0);
	return true;
}
