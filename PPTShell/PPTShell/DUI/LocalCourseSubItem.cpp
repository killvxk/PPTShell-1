#include "StdAfx.h"
#include "LocalCourseSubItem.h"
#include "ItemExplorer.h"
#include "PreviewDialogUI.h"
#include "NDCloud/NDCloudAPI.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"

CLocalCourseSubItemUI::CLocalCourseSubItemUI()
{
	
}

CLocalCourseSubItemUI::~CLocalCourseSubItemUI()
{

}


void CLocalCourseSubItemUI::Init()
{
	CResourceItemUI::m_bNeedTypeIcon = false;
	CLocalItemUI::m_bEnableChapter	= false;
	__super::Init();
	SetContentHeight(130);
	SetIcon(_T(""));
}


void CLocalCourseSubItemUI::OnButtonClick( int nButtonIndex, TNotifyUI& msg )
{
	CLocalCourseItemUI* m_pCourseItem = (CLocalCourseItemUI*)GetParentItem();
	if(_tcsicmp(m_pCourseItem->GetResource(), _T("")) == 0)
	{
		CToast::Toast(_STR_PREVIEW_DIALOG_NOSUPPORT);
		return;
	}

	if( nButtonIndex == 0 )
	{
		InsertPPTByThread(m_pCourseItem->GetResource(), GetIndex() + 1, GetIndex() + 1);
	}
	else if( nButtonIndex == 1 )
	{
		CStream* pStream = (CStream*)m_pCourseItem->GetTag();
		if( pStream != NULL )
		{
			// append current selected index / ppt file path
			CStream stream(1024);
			stream.WriteDWORD(GetIndex());
			stream.WriteString(m_pCourseItem->GetResource());

			stream += *pStream;

			CRect rect;
			HWND hwnd = AfxGetMainWnd()->GetSafeHwnd();
			::GetWindowRect(hwnd,&rect);
			CPreviewDlgUI * pPreviewDlg = new CPreviewDlgUI();
			pPreviewDlg->Create(hwnd, _T("PreviewWindow"), WS_POPUP , 0, 0, 0, 0, 0);
			pPreviewDlg->Init(rect);
			//pPreviewDlg->Create(hwnd, _T("PreviewWindow"), WS_POPUP | WS_VISIBLE, 0, 0, 0, 0, 0);
			//MoveWindow(pPreviewDlg->GetHWND(), rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
			pPreviewDlg->ShowPreviewWindow(&stream, E_WND_COURSE_LOCAL);
 			pPreviewDlg->ShowModal();
			SetFocus();

		}
	}
}

void CLocalCourseSubItemUI::OnItemClick( TNotifyUI& msg )
{
	if( !IsResourceExist() )
	{
		CToast::Toast(_STR_LOCAL_ITEM_TIP_FILE_NO_EXIST, false, 1000);
		return;
	}

}


bool CLocalCourseSubItemUI::OnEmptyControlEvent( void* pEvent )
{
	return CResourceItemUI::OnEmptyControlEvent(pEvent);
}
