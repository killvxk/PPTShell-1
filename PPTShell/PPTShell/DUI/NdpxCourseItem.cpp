#include "StdAfx.h"
#include "NdpxCourseItem.h"
#include "ItemExplorer.h"
#include "NDCloud/NDCloudAPI.h"



CNdpxCourseItemUI::CNdpxCourseItemUI()
{
	m_dwPreviewsDownloadId	= 0;
	m_eContentType			= CONTENT_COURSEWARE;
}

CNdpxCourseItemUI::~CNdpxCourseItemUI()
{
	CStream* pStream = (CStream*)this->GetTag();
	if (pStream)
	{
		delete pStream;
		this->SetTag(NULL);
	}
	NDCloudDownloadCancel(m_dwPreviewsDownloadId);
}

void CNdpxCourseItemUI::Init()
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

LPCTSTR CNdpxCourseItemUI::GetButtonText( int nIndex )
{
	if (nIndex == 0)
	{
		return _T("´ò¿ª");
	}

	return __super::GetButtonText(nIndex);

}

void CNdpxCourseItemUI::OnButtonClick( int nButtonIndex, TNotifyUI& msg )
{
	DownloadResource(nButtonIndex, CloudFileCourse, 0);
}


void CNdpxCourseItemUI::OnItemClick( TNotifyUI& msg )
{
	CItemExplorerUI::GetInstance()->ShowWindow(false);
}

void CNdpxCourseItemUI::ReadStream( CStream* pStream )
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

bool CNdpxCourseItemUI::OnDownloadPreviewsNotify( void* pNotify )
{
	return true;
}

LPCTSTR CNdpxCourseItemUI::GetGuid()
{
	return m_strGuid.c_str();
}

LPCTSTR CNdpxCourseItemUI::GetTitle()
{
	return m_strTitle.c_str();
}

LPCTSTR CNdpxCourseItemUI::GetPPTUrl()
{
	return GetResource();
}

void CNdpxCourseItemUI::DownloadResource( int nButtonIndex, int nType, int nThumbnailSize )
{
	CCloudItemUI::OnDownloadResourceBefore();
	CResourceItemUI::m_nButtonIndex = nButtonIndex;
	this->ShowProgress(true);

	if( _tcslen(GetResource() ) > 0)
		m_dwResourceDownloadId = NDCloudDownloadFile(GetResource(), _T(""), _T(""), CloudFileNdpCourse, 0, MakeHttpDelegate(this, &CNdpxCourseItemUI::OnNdpxCourseDownloaded), MakeHttpDelegate(this, &CCloudItemUI::OnDownloadResourceProgress));	
	
}

void CNdpxCourseItemUI::OnItemDragFinish()
{
	DownloadResource(0, 0, 0);	
}

bool CNdpxCourseItemUI::OnNdpxCourseDownloaded( void * pParam )
{
	THttpNotify* pNotify = (THttpNotify*)pParam;

	tstring strFilePath = pNotify->strFilePath;
	// open 
	if( pNotify->strFilePath != _T("") )
	{
		COPYDATASTRUCT copyData = {0};
		copyData.dwData = MSG_OPEN_FILE;
		copyData.cbData = strFilePath.length() + 1;
		copyData.lpData = (LPVOID)strFilePath.c_str();

		SendMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), WM_COPYDATA, MSG_OPEN_FILE, (LPARAM)&copyData);
	}
	
	__super::OnDownloadResourceCompleted(pNotify);

	return true;
}
