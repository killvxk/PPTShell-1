#include "StdAfx.h"
#include "CourseSubItem.h"
#include "ItemExplorer.h"
#include "PreviewDialogUI.h"
#include "NDCloud/NDCloudAPI.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "Statistics/Statistics.h"
#include "LocalCourseItem.h"
#include "ItemExplorer.h"
#include "GroupExplorer.h"

CCourseSubItemUI::CCourseSubItemUI()
{
}

CCourseSubItemUI::~CCourseSubItemUI()
{

}


void CCourseSubItemUI::Init()
{
	__super::Init();
	SetContentHeight(130);
	//SetFixedWidth(220);
}


void CCourseSubItemUI::OnButtonClick( int nButtonIndex, TNotifyUI& msg )
{
	CCourseItemUI* pCourseItem = (CCourseItemUI*)GetParentItem();
	SetResource(pCourseItem->GetPPTUrl());
	if(_tcsicmp(GetResource(), _T("")) == 0)
	{
		CToast::Toast(_STR_PREVIEW_DIALOG_NOSUPPORT);
		return;
	}

	if( nButtonIndex == 0 )
		DownloadResource(nButtonIndex, CloudFileCourse, 0);
	else
	{
		CStream* pStream = (CStream*)pCourseItem->GetPreviews();
		if( pStream != NULL )
		{
			// append title/guid/url/selected slide index
			CStream stream(1024);
			stream.WriteString((char*)pCourseItem->GetTitle());
			stream.WriteString((char*)pCourseItem->GetGuid());
			stream.WriteString((char*)pCourseItem->GetPPTUrl());
			stream.WriteDWORD(GetIndex());
			
			stream += *pStream;

			CRect rect;
			HWND hwnd = AfxGetMainWnd()->GetSafeHwnd();
			::GetWindowRect(hwnd,&rect);
			CPreviewDlgUI * pPreviewDlg = new CPreviewDlgUI();

			pPreviewDlg->Create(hwnd, _T("PreviewWindow"), WS_POPUP , 0, 0, 0, 0, 0);
			pPreviewDlg->Init(rect);
			//
			//MoveWindow(pPreviewDlg->GetHWND(), rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
			pPreviewDlg->ShowPreviewWindow(&stream, E_WND_COURSE);
			pPreviewDlg->ShowModal();

			SetFocus();

			// report
			Statistics::GetInstance()->Report(STAT_PREVIEW_THUMBNAIL);
		}
	}
}

void CCourseSubItemUI::OnItemClick( TNotifyUI& msg )
{
	CLocalCourseItemUI* pCourseItem = (CLocalCourseItemUI*)GetParentItem();
}

void CCourseSubItemUI::DownloadResource( int nButtonIndex, int nType, int nThumbnailSize )
{
	CCloudItemUI::OnDownloadResourceBefore();

	CCourseItemUI* pCourseItem = (CCourseItemUI*)GetParentItem();
	CResourceItemUI::m_nButtonIndex = nButtonIndex;
	this->ShowProgress(true);
	CCloudItemUI::m_dwResourceDownloadId = NDCloudDownloadCourseFile(GetResource(), pCourseItem->GetGuid(), pCourseItem->GetTitle(),  CourseFilePPT, MakeHttpDelegate(this, &CCloudItemUI::OnDownloadResourceCompleted), MakeHttpDelegate(this, &CCloudItemUI::OnDownloadResourceProgress));
	if (m_dwResourceDownloadId == 0)
	{
		this->ShowProgress(false);
		return;
	}
}


void CCourseSubItemUI::OnDownloadResourceCompleted( int nButtonIndex, LPCTSTR lptcsPath )
{
	if (nButtonIndex == 0)
	{
		InsertPPTByThread(lptcsPath, GetIndex() + 1, GetIndex() + 1);
		//SetFocus();

		Statistics::GetInstance()->Report(STAT_INSERT_PPT);
	}
	else if (nButtonIndex == 1)
	{
		OpenAsDefaultExec(lptcsPath);
	}
}

void CCourseSubItemUI::OnItemDragFinish()
{
	CCourseItemUI* pCourseItem = (CCourseItemUI*)GetParentItem();
	SetResource(pCourseItem->GetPPTUrl());
	DownloadResource(0, 0, 0);
	
}

void CCourseSubItemUI::DownloadThumbnail()
{
	CCourseItemUI* pCourseItem = (CCourseItemUI*)GetParentItem();
	this->StartMask();
	CPhotoItemUI::m_dwDownloadId = NDCloudDownloadCourseFile(GetResource(), pCourseItem->GetGuid(), pCourseItem->GetTitle(),  CourseFileThumb, MakeHttpDelegate(this, &CPhotoItemUI::OnDownloadThumbnailCompleted));
	if (CPhotoItemUI::m_dwDownloadId == 0)
	{
		SetImage(_T("Rightbar/Item/item_bg_image_none.png"));
		this->StopMask();
		return;
	}
}

void CCourseSubItemUI::ReadStream( CStream* pStream )
{
	SetResource(pStream->ReadString().c_str());
	SetGroup(_T("CourseSubItem"));
}
