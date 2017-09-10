#include "StdAfx.h"
#include "PreviewDialogUI.h"
#include "PreviewItem.h"
#include "DUI/LocalPhotoItem.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "Util/Util.h"
#include "NDCloud/NDCloudAPI.h"
#include "Statistics/Statistics.h"
#include "EventCenter/EventDefine.h"

#include "DUI/IButtonTag.h"
#include "DUI/IComponent.h"
#include "DUI/ItemComponent.h"
#include "DUI/MaskComponent.h"
#include "DUI/ThumbnailItem.h"
#include "DUI/ResourceStyleable.h"
#include "DUI/PhotoStyleable.h"


#include "DUI/IItemHandler.h"
#include "DUI/INotifyHandler.h"
#include "NDCloud/NDCloudAPI.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IDownloadListener.h"
#include "Util/Stream.h"
#include "DUI/IStreamReader.h"
#include "DUI/IVisitor.h"
#include "DUI/ItemHandler.h"
#include "DUI/CloudResourceHandler.h"
#include "DUI/IThumbnailListener.h"
#include "DUI/CloudPhotoHandler.h"
#include "DUI/CloudThumbnailHandler.h"
#include "DUI/CloudCourseHandler.h"
#include "DUI/CloudCourseThumbnailHandler.h"

#include "Http/HttpDownloadManager.h"

#define VERIFY_LPSZ(lpszVal, ret)  do { if ( (NULL == (lpszVal)) || (NULL == lpszVal[0]) ) return ret; } while ( 0 );
#define VERIFY_LPSZ_NOT_RET(lpszVal)  do { if ( (NULL == (lpszVal)) || (NULL == lpszVal[0]) ) return; } while ( 0 );
#define VERIFY_PTR(lpszVal, ret)  do { if ( (NULL == (lpszVal)) ) return ret; } while ( 0 );
#define VERIFY_PTR_NOT_RET(lpszVal)  do { if ( (NULL == (lpszVal)) ) return; } while ( 0 );

static bool g_bIsDestoryed = false;
CPreviewDlgUI::CPreviewDlgUI(void)
{
	m_pVRItemHandler	= NULL;
	m_phThread			= NULL;
	m_phSetPosThread	= NULL;
	m_nItemCount		= 0;
	m_nInsertCount		= 0;
	g_bIsDestoryed      = false;
	POINT pt;
	::GetCursorPos(&pt);
	::ClientToScreen(GetHWND(), &pt);
	HMONITOR hMonitor = ::MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
	MONITORINFO monitorInfo;
	monitorInfo.cbSize = sizeof(MONITORINFO);
	::GetMonitorInfo(hMonitor, &monitorInfo);

	m_nWidth = monitorInfo.rcMonitor.right-monitorInfo.rcMonitor.left;
	m_nHeight =  monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

}

CPreviewDlgUI::~CPreviewDlgUI(void)
{
	g_bIsDestoryed = true;
	if( m_phSetPosThread  != NULL )
	{
		TerminateThread(m_phSetPosThread, 0);
		CloseHandle(m_phSetPosThread);
		m_phSetPosThread = NULL;
	}

	// terminate thread
	if( m_phThread  != NULL )
	{
		TerminateThread(m_phThread, 0);
		CloseHandle(m_phThread);
		m_phThread = NULL;
	}

	if (m_pVRItemHandler)
	{
		m_pVRItemHandler->RemoveHolder((CControlUI*)this);
	}

	NDCloudDownloadCancel(m_dwPPTDownloadId,
		&MakeHttpDelegate(this, &CPreviewDlgUI::OnPPTFileDownloaded),
		&MakeHttpDelegate(this, &CPreviewDlgUI::OnPPTFileDownloading));

	NDCloudDownloadCancel(m_dwBigPictureDownloadId,
		&MakeHttpDelegate(this, &CPreviewDlgUI::OnPictureDownloaded));

	NDCloudDownloadCancel(m_dwFullScreenBigPictureDownloadId,
		&MakeHttpDelegate(this, &CPreviewDlgUI::OnPreviewFullPictureDownloaded));
}

LPCTSTR CPreviewDlgUI::GetWindowClassName( void ) const
{
	return _T("PreviewDialog");	
}

DuiLib::CDuiString CPreviewDlgUI::GetSkinFile()
{
	return _T("Preview\\Preview.xml");
}

DuiLib::CDuiString CPreviewDlgUI::GetSkinFolder()
{
	return _T("skins");
}

void CPreviewDlgUI::OnBtnClose(TNotifyUI& msg)
{
	Close();
}

void CPreviewDlgUI::OnBtnScrollLeftBtn(TNotifyUI& msg)
{

	SIZE sz = m_pBottomScrollLayout->GetScrollPos();
	sz.cx -= 136;
	m_pBottomScrollLayout->SetScrollPos(sz);

}

void CPreviewDlgUI::OnBtnScrollRightBtn(TNotifyUI& msg)
{
	SIZE sz = m_pBottomScrollLayout->GetScrollPos();
	sz.cx += 136;

	SIZE szRange =  m_pBottomScrollLayout->GetScrollRange();
// 	if (sz.cx >= szRange.cx)
// 	{
// 		m_pBottomScrollLayout->SetScrollPos(szRange);
// 	}
// 	else
// 	{
		m_pBottomScrollLayout->SetScrollPos(sz);
//	}	
	
}

CControlUI* CPreviewDlgUI::CreateControl(LPCTSTR pstrClass)
{
	if( _tcscmp(pstrClass, _T("GifAnim")) == 0 )
		return new CGifAnimUI;

	if( _tcscmp(pstrClass, _T("ResourceItem")) == 0 )
	{
		switch (m_eCurrentType)
		{
		case E_WND_FLASH:
			return new CPreviewItemUI;
		case E_WND_COURSE:
			return new CPreviewItemUI;
		case E_WND_PICTURE:
			return new CPhotoItemUI;
		case E_WND_PICTURE_LOCAL:
			return new CLocalPhotoItemUI;
		case E_WND_COURSE_LOCAL:
			return new CPreviewItemUI;
		}
	}

	return __super::CreateControl(pstrClass);
}

void CPreviewDlgUI::Init(CRect &rect)
{
//	Create(hWndParent, _T("PreviewWindow"), WS_POPUP | WS_VISIBLE, WS_EX_WINDOWEDGE);
	MoveWindow(GetHWND(), rect.left, rect.top, rect.Width(), rect.Height(), TRUE);

//	m_WndShadow.Create(m_hWnd, hWndParent);
}


void CPreviewDlgUI::InitWindow()
{
	m_pTopLayout			= static_cast<CHorizontalLayoutUI *>(m_PaintManager.FindControl(_T("PreivieTopContainer")));
	m_pMiddleLayout			= static_cast<CHorizontalLayoutUI *>(m_PaintManager.FindControl(_T("PreivieMiddleContainer")));
	m_pBottomLayout			= static_cast<CHorizontalLayoutUI *>(m_PaintManager.FindControl(_T("PreiviewBottomContainer")));
	m_pBottomPicLayout		= static_cast<CHorizontalLayoutUI *>(m_PaintManager.FindControl(_T("PreiviewPicBottomContainer")));
	m_pPreviewBigLayout		= static_cast<CHorizontalLayoutUI *>(m_PaintManager.FindControl(_T("PreviewBigLayout")));
	m_pPreviewBigLayout2	= static_cast<CVerticalLayoutUI *>(m_PaintManager.FindControl(_T("PreviewBigLayout2")));
	
	m_pBottomScrollLayout   = static_cast<CHorizontalLayoutUI *>(m_pBottomLayout->FindSubControl(_T("previewScrollCtn")));
	m_pWindowTitleLabel		= static_cast<CLabelUI *>(m_pTopLayout->FindSubControl(_T("PreviewName")));
	m_pFileSizeLabel		= static_cast<CLabelUI *>(m_pTopLayout->FindSubControl(_T("filesize")));
	m_pDownloadNumsLabel    = static_cast<CLabelUI *>(m_pTopLayout->FindSubControl(_T("downloadnum")));
	m_pPreviewBig			= static_cast<CControlUI *>(m_pMiddleLayout->FindSubControl(_T("PreviewBig")));
	m_pPreviewBigError		= static_cast<CControlUI *>(m_pMiddleLayout->FindSubControl(_T("PreviewBigError")));
	m_pPlayBtn				= static_cast<CButtonUI*>(m_pMiddleLayout->FindSubControl(_T("playBtn")));
	m_pAllApplyBtn			= static_cast<CButtonUI*>(m_pMiddleLayout->FindSubControl(_T("allApplyBtn")));
	m_pPageLeftBtn			= static_cast<CButtonUI*>(m_pMiddleLayout->FindSubControl(_T("pageLeftBtn")));
	m_pPageRightBtn			= static_cast<CButtonUI*>(m_pMiddleLayout->FindSubControl(_T("pageRightBtn")));	
	m_pFullScreenBtn		= static_cast<CButtonUI*>(m_pMiddleLayout->FindSubControl(_T("fullScreenBtn")));
	m_pInsertPageBtn		= static_cast<CButtonUI*>(m_pMiddleLayout->FindSubControl(_T("insertPageBtn")));
	m_pCancelInsertBtn		= static_cast<CButtonUI*>(m_pMiddleLayout->FindSubControl(_T("cancelInsertBtn")));
	m_pAnimation			= dynamic_cast<CGifAnimUI*>(m_PaintManager.FindControl(_T("loading11")));
	m_pProgress				= dynamic_cast<CProgressUI*>(m_PaintManager.FindControl(_T("progress")));
	m_pMiddlePicLayout		= static_cast<CVerticalLayoutUI *>(m_pMiddleLayout->FindSubControl(_T("midContainer")));

	//decription and duration
	m_pDecriptionLay		= static_cast<CContainerUI*>(m_PaintManager.FindControl(_T("decription_lay")));
	m_pDecription			= static_cast<CLabelUI *>(m_pDecriptionLay->FindSubControl(_T("decription")));
	m_pDurationLay			= static_cast<CContainerUI*>(m_PaintManager.FindControl(_T("duration_lay")));
	m_pDuration				= static_cast<CLabelUI *>(m_pDurationLay->FindSubControl(_T("duration")));
	m_pInsertVRBtn			= static_cast<CControlUI *>(m_PaintManager.FindControl(_T("btn_insert_vr")));
	m_pDownloadVRBtn		= static_cast<CControlUI *>(m_PaintManager.FindControl(_T("btn_download_vr")));
	m_pPauseVRBtn			= static_cast<CControlUI *>(m_PaintManager.FindControl(_T("btn_pause_vr")));

	m_pCancelInsertBtn->SetVisible(false);//2015.10.21 cws
	m_nSourceType = 0;//2015.10.27 cws
}

void CPreviewDlgUI::ShowPreviewWindow(CStream* pStream, EPLUS_WND_TYPE eType)
{
	m_eCurrentType = eType;
	m_eOriginalType= eType;

	switch(eType)
	{
	case E_WND_FLASH:
		{
			//InitFlashWnd(lpszFileName);
		}
		break;
	case E_WND_COURSE:
	case E_WND_VR:
		{
			InitCourseWnd(pStream);
		}
		break;

	case E_WND_PICTURE:
		{
			InitPictureWnd(pStream);
		}
		break;
	case E_WND_PICTURE_LOCAL:
		{
			InitPictureWnd(pStream);
		}
		break;

	case E_WND_COURSE_LOCAL:
		{
			InitCourseWndLocal(pStream);
		}
		break;

	default:
		break;
	}

}

DWORD WINAPI CPreviewDlgUI::ShowPicThread(LPVOID lpVoid)
{
	CPreviewDlgUI* pThis = (CPreviewDlgUI*)lpVoid;
	if (pThis)
	{
		Sleep(50);
		pThis->ShowPicByThread();
		CloseHandle(pThis->m_phThread);
		pThis->m_phThread = NULL;
	}
	return 1;
}

void CPreviewDlgUI::OnPictureHttpSucess(LPCTSTR lpszPath, LPCTSTR lpszFileSize, LPCTSTR lpszDownLoadNums)
{
	m_strPath = lpszPath;
	m_strPicSize = lpszFileSize;
	m_strDownloadNums = lpszDownLoadNums;
	m_pFullScreenBtn->SetEnabled(false);	
	m_pInsertPageBtn->SetEnabled(false);	
	//m_pCancelInsertBtn->SetEnabled(false);

	//m_phThread = CreateThread(NULL, 0, ShowPicThread, this, 0, NULL);
	ShowPicByThread();//forbid ui operation on thread

}

void CPreviewDlgUI::OnHttpReturnError()
{
	if (!CheckPtr()) return;

	m_pAnimation->StopGif();
	m_pAnimation->SetVisible(false);
	m_pPreviewBigLayout->SetVisible(false);
	{
		m_pPreviewBigError->SetFloat();
		m_pPreviewBigError->SetPos(CDuiRect(275,145,365,215));
	}
	m_pPreviewBigError->SetVisible(true);

}

void CPreviewDlgUI::OnBtnPlay(TNotifyUI& msg)
{

}

void CPreviewDlgUI::OnBtnApply(TNotifyUI& msg)
{
	m_nStartSlideIndex	= 1;
	m_nEndSlideIndex	= -1;

	
	if( m_eCurrentType == E_WND_COURSE_LOCAL )
	{
		if( m_strPPTUrl.empty() )
		{
			CToast::Toast(_STR_PREVIEW_DIALOG_NOSUPPORT);
			return;
		}

		m_pProgress->SetVisible(true);
		m_pInsertPageBtn->SetEnabled(false);
		m_pAllApplyBtn->SetEnabled(false);
		m_pFullScreenBtn->SetEnabled(false);
		m_pCancelInsertBtn->SetEnabled(false);

		THttpNotify notify;
		notify.strFilePath = m_strPPTUrl;

		OnPPTFileDownloaded(&notify);
	}
	else
	{
		m_pProgress->SetVisible(true);
		m_pInsertPageBtn->SetEnabled(false);
		m_pAllApplyBtn->SetEnabled(false);
		m_pFullScreenBtn->SetEnabled(false);
		m_pCancelInsertBtn->SetEnabled(false);

		CThumbnailItemUI*	pItem	= (CThumbnailItemUI*)m_pBottomScrollLayout->GetItemAt(0);
		CItemHandler*	pHandler	= pItem->GetHandler();

		m_pInsertPageBtn->SetEnabled(false);
		m_pAllApplyBtn->SetEnabled(false);

		m_pProgress->SetVisible(true);
		m_dwPPTDownloadId = NDCloudDownloadCourseFile(pHandler->GetResource(),
			pHandler->GetGuid(),
			pHandler->GetTitle(),
			CourseFilePPT,
			MakeHttpDelegate(this, &CPreviewDlgUI::OnPPTFileDownloaded),
			MakeHttpDelegate(this, &CPreviewDlgUI::OnPPTFileDownloading) );

	}

}

void CPreviewDlgUI::OnBtnPageLeft(TNotifyUI& msg)
{
	m_nCurItemIndex --;
	if( m_nCurItemIndex < 0 )
	{
		m_nCurItemIndex = 0;
		return;
	}

	SIZE sz = m_pBottomScrollLayout->GetScrollPos();
	sz.cx -= 136;
	m_pBottomScrollLayout->SetScrollPos(sz);


	if (m_eCurrentType == E_WND_PICTURE
		|| m_eCurrentType == E_WND_COURSE)
	{
		CThumbnailItemUI* pItem = dynamic_cast<CThumbnailItemUI*>(m_pBottomScrollLayout->GetItemAt(m_nCurItemIndex));
		if( pItem == NULL )
			return;

		if (pItem->GetOption())
		{
			pItem->GetOption()->Selected(true);
			m_PaintManager.SendNotify(pItem->GetOption(), DUI_MSGTYPE_CLICK, 0, 0, true);
		}
	}
	else
	{
		CPreviewItemUI* pItem = static_cast<CPreviewItemUI*>(m_pBottomScrollLayout->GetItemAt(m_nCurItemIndex));
		if( pItem == NULL )
			return;

		if (pItem->GetOption())
		{
			pItem->GetOption()->Selected(true);
			m_PaintManager.SendNotify(pItem->GetOption(), DUI_MSGTYPE_CLICK, 0, 0, true);
		}
	}

}

void CPreviewDlgUI::OnBtnPageRight(TNotifyUI& msg)
{
	int count = m_pBottomScrollLayout->GetCount();

	m_nCurItemIndex ++;
	if( m_nCurItemIndex >= count )
	{
		m_nCurItemIndex = count-1;
		return;
	}

	SIZE sz = m_pBottomScrollLayout->GetScrollPos();
	sz.cx += 136;

	SIZE szRange =  m_pBottomScrollLayout->GetScrollRange();

	m_pBottomScrollLayout->SetScrollPos(sz);
	

	if (m_eCurrentType == E_WND_PICTURE
		|| m_eCurrentType == E_WND_COURSE)
	{
		CThumbnailItemUI* pItem = dynamic_cast<CThumbnailItemUI*>(m_pBottomScrollLayout->GetItemAt(m_nCurItemIndex));
		if( pItem == NULL )
			return;

		if (pItem->GetOption())
		{
			pItem->GetOption()->Selected(true);
			m_PaintManager.SendNotify(pItem->GetOption(), DUI_MSGTYPE_CLICK, 0, 0, true);
		}
	}
	else
	{
		CPreviewItemUI* pItem = static_cast<CPreviewItemUI*>(m_pBottomScrollLayout->GetItemAt(m_nCurItemIndex));
		if( pItem == NULL )
			return;

		if (pItem->GetOption())
		{
			pItem->GetOption()->Selected(true);
			m_PaintManager.SendNotify(pItem->GetOption(), DUI_MSGTYPE_CLICK, 0, 0, true);
		}
	}
	
}

void CPreviewDlgUI::OnBtnFullScreen(TNotifyUI& msg)
{
	if (!IsWindow(this->GetHWND()))
	{
		return;
	}

	tstring strImg;
	if (m_pPreviewBig->IsVisible())
	{
		strImg = m_pPreviewBig->GetBkImage();
	}
	CPreviewFullUI::GetInstance(this, this->GetHWND(), m_eCurrentType, strImg.c_str(), true);
	Statistics::GetInstance()->Report(STAT_FULLSCREEN_PREVIEW_PICTURE);
}


void CPreviewDlgUI::OnBtnInsertPage(TNotifyUI& msg)
{
	
	if( m_eCurrentType == E_WND_PICTURE || m_eCurrentType == E_WND_PICTURE_LOCAL )
	{
		if( m_pPreviewBig != NULL )
		{
			if (m_strPicSourceUrl.empty())
			{
				CToast::Toast(_STR_PREVIEW_DIALOG_INSERT_FAIL);
				Close();
				return;
			}
			LPCTSTR strBkImagePath = m_strPicSourceUrl.c_str();
			// get image size
			SIZE size = CRenderEngine::GetImageSize(strBkImagePath);

			BOOL bInsert = InsertPictureByThread(strBkImagePath, -1, -1, size.cx, size.cy);
			if(!bInsert)
			{
				CToast::Toast(_STR_PREVIEW_DIALOG_INSERT_FAIL);
			}	 
			Close();
		}
	}
	else if(m_eCurrentType == E_WND_COURSE_LOCAL )
	{
		m_nStartSlideIndex	= m_nCurItemIndex+1;
		m_nEndSlideIndex	= m_nCurItemIndex+1;

		if( m_strPPTUrl.empty() )
		{
			CToast::Toast(_STR_PREVIEW_DIALOG_NOSUPPORT);
			return;
		}

		m_pInsertPageBtn->SetEnabled(false);
		m_pAllApplyBtn->SetEnabled(false);
// 		m_pFullScreenBtn->SetEnabled(false);
// 		m_pCancelInsertBtn->SetEnabled(false);

		THttpNotify notify;
		notify.strFilePath = m_strPPTUrl;

		OnPPTFileDownloaded(&notify);	
	
	}
	else if( m_eCurrentType == E_WND_COURSE)
	{
		m_nStartSlideIndex	= m_nCurItemIndex+1;
		m_nEndSlideIndex	= m_nCurItemIndex+1;

		CThumbnailItemUI*	pItem	= (CThumbnailItemUI*)m_pBottomScrollLayout->GetItemAt(m_nCurItemIndex);
		CItemHandler*	pHandler	= pItem->GetHandler();

		m_pInsertPageBtn->SetEnabled(false);
		m_pAllApplyBtn->SetEnabled(false);

		m_pProgress->SetVisible(true);
		m_dwPPTDownloadId = NDCloudDownloadCourseFile(pHandler->GetResource(),
			pHandler->GetGuid(),
			pHandler->GetTitle(),
			CourseFilePPT,
			MakeHttpDelegate(this, &CPreviewDlgUI::OnPPTFileDownloaded),
			MakeHttpDelegate(this, &CPreviewDlgUI::OnPPTFileDownloading) );

	}
}


void CPreviewDlgUI::OnBtnCancelInsert(TNotifyUI& msg)
{
	if (--m_nInsertCount <= 0)
	{
		m_pCancelInsertBtn->SetEnabled(false);
	}
	
	UnDoByThread(1);

	//m_nInsertCount = 0;
}

void CPreviewDlgUI::InitFlashWnd(const char* lpszFileName)
{
	if (!CheckPtr()) return;
	SetPreviewTitle(lpszFileName);
	m_pPlayBtn->SetVisible(true);
	m_pAllApplyBtn->SetVisible(false);
	m_pInsertPageBtn->SetText(_T("插入"));
	m_nSourceType = 0;//2015.10.27 cws

}

DWORD WINAPI CPreviewDlgUI::SetPosThread(LPARAM lpParam)
{
	CPreviewDlgUI* pThis  = (CPreviewDlgUI*)lpParam;
	while(true)
	{
		Sleep(50);
		RECT rt = pThis->m_pBottomScrollLayout->GetPos();
		if (!(rt.bottom == 0 
			&& rt.top == 0
			&& rt.left == 0
			&& rt.right == 0))
		{
			SIZE sz = pThis->m_pBottomScrollLayout->GetScrollPos();
			sz.cx = 136 * pThis->m_nCurItemIndex;
			pThis->m_pBottomScrollLayout->SetScrollPos(sz);
			break;
		}
	}
	
	CloseHandle(pThis->m_phSetPosThread);
	pThis->m_phSetPosThread = NULL;

	return 1;
}


void CPreviewDlgUI::InitCourseWnd( CStream* pStream )
{
	// PPT Title/PPT GUID/PPT url/selected thumbnail url
	pStream->ResetCursor();

	m_nCurItemIndex			= pStream->ReadDWORD();
	

	if (!CheckPtr()) return;


	m_pAnimation->SetVisible(true);
	m_pAnimation->PlayGif();
	m_pPlayBtn->SetVisible(false);
	m_pAllApplyBtn->SetVisible(true);
	m_pCancelInsertBtn->SetVisible(true);
	m_pCancelInsertBtn->SetEnabled(false);
	m_pInsertPageBtn->SetText(_T("插入该页"));
	m_nSourceType = 1;//2015.10.21 cws 

	if(m_eCurrentType == E_WND_VR)
	{
		m_pVRItemHandler	= (CItemHandler*)pStream->ReadDWORD();
		tstring strDecr		= pStream->ReadString();
		DWORD	dwFileSize	= pStream->ReadDWORD();
		BOOL	bExists		= pStream->ReadBOOL();

		m_pVRItemHandler->AddHolder((CControlUI*)this);
		
		m_pAllApplyBtn->SetVisible(false);
		m_pCancelInsertBtn->SetVisible(false);
		m_pInsertPageBtn->SetVisible(false);
		m_pFullScreenBtn->SetVisible(false);

		//m_pDurationLay->SetVisible(true);
		if (bExists)
		{
			m_pInsertVRBtn->SetVisible(true);
			m_pDownloadVRBtn->SetVisible(false);
			m_pPauseVRBtn->SetVisible(false);
		}
		else
		{
			m_pInsertVRBtn->SetVisible(false);
			m_pDownloadVRBtn->SetVisible(true);
			m_pPauseVRBtn->SetVisible(false);
		}
		m_pDecriptionLay->SetVisible(true);
	
		if (!strDecr.empty())
		{
			m_pDecription->SetText(strDecr.c_str());
			m_pDecription->SetToolTip(strDecr.c_str());
		}

		SetPreviewSize(dwFileSize);
		CControlUI* pPadding = m_PaintManager.FindControl(_T("btn_padding"));
		if (pPadding)
		{
			pPadding->SetVisible(true);
			pPadding->SetFixedHeight(58);
		}
		m_eCurrentType = E_WND_COURSE;
	}
	//
	int nCount = pStream->ReadDWORD();
	m_pBottomScrollLayout->SetVisible(false);
	for (int i = 0; i < nCount; i++)
	{
		IComponent*		pComponent	= new CMaskComponent(&CPhotoStyleable::Styleable);
		pComponent					= new CItemComponent(pComponent);
		CThumbnailItemUI*	pNewItem= new CThumbnailItemUI(pComponent);
		CCloudCourseThumbnailHandler* pHandler= new CCloudCourseThumbnailHandler();

		pHandler->OnClick += MakeDelegate(this, &CPreviewDlgUI::OnCourseItemClick);
		pNewItem->SetHandler(pHandler);
		if (pNewItem)
		{
			pNewItem->SetPadding(CDuiRect(8, 15, 0, 0));

			if (i == 0)
				pNewItem->SetPadding(CDuiRect(0, 15, 0, 0));

			pNewItem->ReadStream(pStream);
			m_pBottomScrollLayout->Add(pNewItem);

			pNewItem->SetItemHeight(72);
			pNewItem->SetItemWidth(128);
		}
	}

	if (nCount > 0)
	{
		CThumbnailItemUI* pItem = static_cast<CThumbnailItemUI*>(m_pBottomScrollLayout->GetItemAt(m_nCurItemIndex));
		if( pItem == NULL )
			return;
		pItem->GetOption()->Selected(true);

		m_PaintManager.SendNotify(pItem->GetOption(), DUI_MSGTYPE_CLICK, 0, 0, true);
	}

	m_pBottomScrollLayout->GetHorizontalScrollBar()->SetFixedHeight(1);
	m_pBottomScrollLayout->SetVisible(true);
	m_pBottomScrollLayout->Invalidate();

	m_phSetPosThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SetPosThread, this, 0, NULL);
}

void CPreviewDlgUI::InitCourseWndLocal(CStream* pStream)
{
	pStream->ResetCursor();
	m_nCurItemIndex		= pStream->ReadInt();
	m_strPPTUrl			= pStream->ReadString();

	// ppt name
	int pos = m_strPPTUrl.rfind('\\');
	if( pos != -1)
		m_strPPTTitle = m_strPPTUrl.substr(pos+1);
	
	if (!CheckPtr()) 
		return;

	SetPreviewTitle(m_strPPTTitle.c_str());
	m_pAnimation->SetVisible(true);
	m_pAnimation->PlayGif();
	m_pPlayBtn->SetVisible(false);
	m_pAllApplyBtn->SetVisible(true);
	m_pCancelInsertBtn->SetVisible(true);
	m_pCancelInsertBtn->SetEnabled(false);
	m_pInsertPageBtn->SetText(_T("插入该页"));


	tstring strBigPicturePath;

	int nCount = pStream->ReadInt();

	m_nItemCount = nCount;
	m_pBottomScrollLayout->SetVisible(false);

	for (int i = 0; i < nCount; i++)
	{
		CPreviewItemUI * pItem = (CPreviewItemUI*) CreateItem();
		if (pItem)
		{
			pItem->SetPadding(CDuiRect(8, 15, 0, 0));

			if (i == 0)
				pItem->SetPadding(CDuiRect(0, 15, 0, 0));

			tstring strTitle = pStream->ReadString();
			tstring strUrl = pStream->ReadString();

			if (strUrl.empty())
				continue;

			if( i == m_nCurItemIndex )
				strBigPicturePath = strUrl;

			pItem->SetIndex(i);
			pItem->SetPreviewDlgPtr(this);
			pItem->SetResourceUrl((char*)strTitle.c_str(), (char*)strUrl.c_str(), m_strPPTGuid.c_str());
			pItem->SetFixedHeight(72);
			pItem->SetFixedWidth(128);
			pItem->SetGroup("local");
			pItem->SetLocal(true);

			m_pBottomScrollLayout->Add(pItem);
		}
	}

	if (nCount > 0)
	{
		CPreviewItemUI* pItem = static_cast<CPreviewItemUI*>(m_pBottomScrollLayout->GetItemAt(m_nCurItemIndex));
		if( pItem == NULL )
			return;
		pItem->GetOption()->Selected(true);
	}

	int nWidth = 136 * nCount + 8 * (nCount + 1);

	m_pBottomScrollLayout->GetHorizontalScrollBar()->SetFixedHeight(1);
	m_pBottomScrollLayout->SetVisible(true);
	m_pBottomScrollLayout->Invalidate();

	m_phSetPosThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SetPosThread, this, 0, NULL);

	// download main picture
	THttpNotify notify;
	notify.strFilePath = strBigPicturePath;

	OnPictureDownloaded(&notify);
}

bool CPreviewDlgUI::OnPhotoItemClick( void* param )
{
	m_pInsertPageBtn->SetEnabled(false);
	m_pFullScreenBtn->SetEnabled(false);
	if (!param)
	{
		return true;
	}
	// download main picture
	//m_dwBigPictureDownloadId = NDCloudDownloadCourseFile(strBigPictureUrl, m_strPPTGuid, _T(""),  CourseFileThumb, MakeHttpDelegate(this, &CPreviewDlgUI::OnPictureDownloaded));

	CResourceItemUI* pItem = (CResourceItemUI*)param;


	SetPreviewTitle(pItem->GetTitle());
	CContainerUI* pContainer = (CContainerUI*)pItem->GetParent();
	if (pContainer)
	{
		m_nCurItemIndex = pContainer->GetItemIndex(pItem);
	}
	
	m_nSourceType = 0;//2015.10.27 cws
	m_pPreviewBigLayout->SetVisible(false);
	m_pAnimation->SetVisible(true);
	m_pAnimation->PlayGif();
	//TO-DO
	if( m_eCurrentType == E_WND_PICTURE )
	{
		NDCloudDownloadCancel(m_dwBigPictureDownloadId);
		//NDCloudDownloadCancel(m_dwBigPictureDownloadId, &MakeHttpDelegate(this, &CPreviewDlgUI::OnPictureDownloaded));
		m_dwBigPictureDownloadId = NDCloudDownloadFile(pItem->GetResource(), _T(""), pItem->GetTitle(), CloudFileImage, 0, MakeHttpDelegate(this, &CPreviewDlgUI::OnPictureDownloaded));
	}
	else
	{
		THttpNotify httpNotify;
		httpNotify.strFilePath = pItem->GetResource();

		OnPictureDownloaded(&httpNotify);
	}

	return true;
}


void CPreviewDlgUI::InitPictureWnd(CStream* pStream)
{

	if (m_eCurrentType == E_WND_PICTURE)
	{
		pStream->ResetCursor();

		m_nCurItemIndex					= pStream->ReadDWORD();

		if (!CheckPtr()) return;

		m_pAnimation->SetVisible(true);
		m_pAnimation->PlayGif();

		m_pPlayBtn->SetVisible(false);
		m_pAllApplyBtn->SetVisible(false);
		m_pCancelInsertBtn->SetVisible(false);
		m_pInsertPageBtn->SetText(_T("插入该图"));
		m_pInsertPageBtn->SetEnabled(false);
		m_pFullScreenBtn->SetEnabled(false);

		m_nSourceType = 0;//2015.10.21 cws 

		int nCount = pStream->ReadDWORD();
		m_pBottomScrollLayout->SetVisible(false);
		for (int i = 0; i < nCount; i++)
		{
			IComponent*		pComponent	= new CMaskComponent(&CPhotoStyleable::Styleable);
			pComponent					= new CItemComponent(pComponent);
			CThumbnailItemUI*	pNewItem	= new CThumbnailItemUI(pComponent);
			CCloudThumbnailHandler* pHandler= new CCloudThumbnailHandler();

			pHandler->OnClick += MakeDelegate(this, &CPreviewDlgUI::OnPictrueItemClick);
			pNewItem->SetHandler(pHandler);
			if (pNewItem)
			{
				pNewItem->SetPadding(CDuiRect(8, 15, 0, 0));

				if (i == 0)
					pNewItem->SetPadding(CDuiRect(0, 15, 0, 0));

				pNewItem->ReadStream(pStream);
				m_pBottomScrollLayout->Add(pNewItem);

				pNewItem->SetItemHeight(72);
				pNewItem->SetItemWidth(128);
			}
		}

		if (nCount > 0)
		{
			CThumbnailItemUI* pItem = static_cast<CThumbnailItemUI*>(m_pBottomScrollLayout->GetItemAt(m_nCurItemIndex));
			if( pItem == NULL )
				return;
			pItem->GetOption()->Selected(true);

			m_PaintManager.SendNotify(pItem->GetOption(), DUI_MSGTYPE_CLICK, 0, 0, true);
		}

		m_pBottomScrollLayout->GetHorizontalScrollBar()->SetFixedHeight(1);
		m_pBottomScrollLayout->SetVisible(true);
		m_pBottomScrollLayout->Invalidate();

		m_phSetPosThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SetPosThread, this, 0, NULL);
	}
	else
	{
		pStream->ResetCursor();

		CContainerUI*	pItemContainer	= (CContainerUI*)pStream->ReadDWORD();
		m_nCurItemIndex					= pStream->ReadDWORD();
		CResourceItemUI* pCurrentItem	= (CResourceItemUI*)pItemContainer->GetItemAt(m_nCurItemIndex);

		if (!CheckPtr()) return;

		SetPreviewTitle(pCurrentItem->GetTitle());
		m_pAnimation->SetVisible(true);
		m_pAnimation->PlayGif();

		m_pPlayBtn->SetVisible(false);
		m_pAllApplyBtn->SetVisible(false);
		m_pCancelInsertBtn->SetVisible(false);
		m_pInsertPageBtn->SetText(_T("插入该图"));
		m_pInsertPageBtn->SetEnabled(false);
		m_pFullScreenBtn->SetEnabled(false);

		m_nSourceType = 0;//2015.10.21 cws 

		//
		tstring strBigPictureUrl;

		int nCount = pItemContainer->GetCount();
		m_pBottomScrollLayout->SetVisible(false);
		for (int i = 0; i < nCount; i++)
		{
			CResourceItemUI*	pItem	= (CResourceItemUI*)pItemContainer->GetItemAt(i);
			CResourceItemUI*	pNewItem= CreateItem();
			if (pNewItem)
			{
				pNewItem->SetVisible(pItem->IsVisible());
				pNewItem->SetPadding(CDuiRect(8, 15, 0, 0));
				if (i == 0)
					pNewItem->SetPadding(CDuiRect(0, 15, 0, 0));

				if( i == m_nCurItemIndex )
					strBigPictureUrl = pItem->GetResource();

				pNewItem->SetTitle(pItem->GetTitle());
				pNewItem->SetResource(pItem->GetResource());
				pNewItem->SetOnItemClickDelegate(MakeDelegate(this, &CPreviewDlgUI::OnPhotoItemClick));
				pNewItem->SetGroup(_T("photogroup"));
				pNewItem->EnableToolbar(false);
				pNewItem->EnableDrag(false);
				pNewItem->NeedTypeIcon(false);


				if (m_eCurrentType == E_WND_PICTURE_LOCAL)
				{
					CLocalPhotoItemUI* pLocalItem = (CLocalPhotoItemUI*)pNewItem;
					pLocalItem->EnableChapter(false);
				}

				m_pBottomScrollLayout->Add(pNewItem);

				pNewItem->GetTitleControl()->SetVisible(false);
				pNewItem->SetContentHeight(72);
				pNewItem->SetFixedWidth(128);
			}
		}

		if (nCount > 0)
		{
			CResourceItemUI* pItem = static_cast<CResourceItemUI*>(m_pBottomScrollLayout->GetItemAt(m_nCurItemIndex));
			if( pItem == NULL )
				return;
			pItem->GetOption()->Selected(true);

			m_PaintManager.SendNotify(pItem->GetOption(), DUI_MSGTYPE_CLICK, 0, 0, true);
		}

		m_pBottomScrollLayout->GetHorizontalScrollBar()->SetFixedHeight(1);
		m_pBottomScrollLayout->SetVisible(true);
		m_pBottomScrollLayout->Invalidate();

		m_phSetPosThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SetPosThread, this, 0, NULL);
	}

}

bool CPreviewDlgUI::OnPictrueItemClick( void* pObj )
{
	TNotifyUI*		pNotify			= (TNotifyUI*)pObj;
	CStyleItemUI*	pItem			= (CStyleItemUI*)pNotify->pSender;
	CItemHandler*	pHandler		= pItem->GetHandler();

	CContainerUI* pContainer = (CContainerUI*)pItem->GetParent();
	if (pContainer)
	{
		m_nCurItemIndex = pContainer->GetItemIndex(pItem);
	}

	m_nSourceType = 0;//2015.10.27 cws
	m_pPreviewBigLayout->SetVisible(false);
	m_pAnimation->SetVisible(true);
	m_pAnimation->PlayGif();
	m_pInsertPageBtn->SetEnabled(false);
	m_pFullScreenBtn->SetEnabled(false);

	SetPreviewTitle(pHandler->GetTitle());

	//TO-DO
	if( m_eCurrentType == E_WND_PICTURE )
	{
		//CancelPrevieFullDownLoad();
		NDCloudDownloadCancel(m_dwBigPictureDownloadId);

		tstring strUrl = pHandler->GetResource();
		_tcslwr((char *)strUrl.c_str());
		if(_tcsstr(strUrl.c_str(), _T("http://")))
		{
			m_dwBigPictureDownloadId  = NDCloudFileManager::GetInstance()->DownloadFileSearchPlatform(pHandler->GetResource(),
				pHandler->GetGuid(),
				pHandler->GetTitle(),
				CloudFileImage,
				MakeHttpDelegate(this, &CPreviewDlgUI::OnPictureDownloaded),
				MakeHttpDelegate(NULL)
				);
		}
		else
		{
			//NDCloudDownloadCancel(m_dwBigPictureDownloadId, &MakeHttpDelegate(this, &CPreviewDlgUI::OnPictureDownloaded));
			m_dwBigPictureDownloadId = NDCloudDownloadFile(pHandler->GetResource(),
				pHandler->GetGuid(),
				pHandler->GetTitle(),
				CloudFileImage,
				0,
				MakeHttpDelegate(this, &CPreviewDlgUI::OnPictureDownloaded));
		}
		
	}
	else
	{
		THttpNotify httpNotify;
		httpNotify.strFilePath = pHandler->GetResource();

		OnPictureDownloaded(&httpNotify);
	}

	return true;

}


bool CPreviewDlgUI::OnCourseItemClick( void* pObj )
{
	TNotifyUI*		pNotify			= (TNotifyUI*)pObj;
	CStyleItemUI*	pItem			= (CStyleItemUI*)pNotify->pSender;
	CCloudCourseThumbnailHandler*	pHandler		= (CCloudCourseThumbnailHandler*)pItem->GetHandler();

	CContainerUI* pContainer = (CContainerUI*)pItem->GetParent();
	if (pContainer)
	{
		m_nCurItemIndex = pContainer->GetItemIndex(pItem);
	}

	m_nSourceType = 0;//2015.10.27 cws
	m_pPreviewBigLayout->SetVisible(false);
	m_pAnimation->SetVisible(true);
	m_pAnimation->PlayGif();

	SetPreviewTitle(pHandler->GetTitle());

	//TO-DO
	if( m_eCurrentType == E_WND_COURSE )
	{
		//CancelPrevieFullDownLoad();
		NDCloudDownloadCancel(m_dwBigPictureDownloadId, &MakeHttpDelegate(this, &CPreviewDlgUI::OnPictureDownloaded));
		m_dwBigPictureDownloadId = NDCloudDownloadCourseFile(pHandler->GetPreviewUrl(),
			pHandler->GetGuid(),
			pHandler->GetTitle(),
			CourseFileThumb,
			MakeHttpDelegate(this, &CPreviewDlgUI::OnPictureDownloaded));
	}
	else
	{
		THttpNotify httpNotify;
		httpNotify.strFilePath = pHandler->GetResource();

		OnPictureDownloaded(&httpNotify);
	}

	return true;
}

tstring CPreviewDlgUI::GeneratePngPic(LPCTSTR lpszSourceUrl)
{
	tstring strImagePath = lpszSourceUrl;

	WCHAR wszTempPath[MAX_PATH];
	GetTempPathW(MAX_PATH, wszTempPath);

	DWORD dwCrc = CalcFileCRC(lpszSourceUrl);

	WCHAR wszTempFileName[MAX_PATH];
	swprintf_s(wszTempFileName, L"%s\\screen_%x.jpg", wszTempPath, dwCrc);

	DWORD dwRet = GetFileAttributesW(wszTempFileName);

	if (dwRet == INVALID_FILE_ATTRIBUTES)
	{
		CompressImagePixel(AnsiToUnicode(strImagePath).c_str(), wszTempFileName, m_nHeight-50, m_nWidth); 
	}

	strImagePath = Un2Str(wszTempFileName);
	return strImagePath;
}

bool CPreviewDlgUI::CheckPtr()
{
	VERIFY_PTR(m_pWindowTitleLabel, false);
	VERIFY_PTR(m_pPreviewBigError, false);
	VERIFY_PTR(m_pPreviewBig, false);
	VERIFY_PTR(m_pPlayBtn, false);
	VERIFY_PTR(m_pAllApplyBtn, false);
	VERIFY_PTR(m_pPageLeftBtn, false);
	VERIFY_PTR(m_pPageRightBtn, false);
	VERIFY_PTR(m_pFullScreenBtn, false);
	VERIFY_PTR(m_pInsertPageBtn, false);
	VERIFY_PTR(m_pCancelInsertBtn, false);
	VERIFY_PTR(m_pAnimation, false);
	return true;
}

bool CPreviewDlgUI::OnPictureDownloaded(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;

	TCHAR szSize[MAX_PATH] = {0};
	FILE* fp = NULL;
	int err = _tfopen_s(&fp, pNotify->strFilePath.c_str(), "rb");
	if( err == 0 )
	{
		fseek(fp, 0, SEEK_END);
		int nSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		fclose(fp);

		_stprintf_s(szSize, _T("%.1fKB"), (float)nSize/1024.0f);
	}

	OnPictureHttpSucess(pNotify->strFilePath.c_str(), szSize, _T("0"));

	return true;
}


bool CPreviewDlgUI::OnPPTFileDownloading( void* param )
{
	THttpNotify* pNotify = (THttpNotify*)param;
	int nPos = (int)(pNotify->fPercent * m_pProgress->GetMaxValue());
	m_pProgress->SetValue(nPos);

	return true;
}

bool CPreviewDlgUI::OnPreviewFullPictureDownloaded(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;

	CPreviewFullUI::GetInstance(this, HWND_DESKTOP, m_eCurrentType, pNotify->strFilePath.c_str(), pNotify->dwErrorCode == 0);

	return true;
}

bool CPreviewDlgUI::OnPreviewInsertComplete(void* pParam)
{
	if( g_bIsDestoryed )
		return false;

	TEventNotify* pNotify = (TEventNotify*)pParam;

	CStream* pStream = (CStream*)pNotify->wParam;

	int nCount = pStream->ReadBOOL();

	if ( nCount > 0 )
	{
		m_nInsertCount += nCount;
		m_pCancelInsertBtn->SetEnabled(true);
	}

	m_pInsertPageBtn->SetEnabled(true);
	m_pAllApplyBtn->SetEnabled(true);
	m_pFullScreenBtn->SetEnabled(true);

	return true;
}

bool CPreviewDlgUI::OnPPTFileDownloaded(void* param)
{	
	THttpNotify* pNotify = (THttpNotify*)param;
	m_pProgress->SetVisible(false);
	//m_pCancelInsertBtn->SetEnabled(false);

	BOOL bRet = InsertPPTByThread(pNotify->strFilePath.c_str(), m_nStartSlideIndex, m_nEndSlideIndex, 0, 0, 0, NULL, &MakeEventDelegate(this, &CPreviewDlgUI::OnPreviewInsertComplete));

	//if ( bRet && (m_nEndSlideIndex != -1) ) // insert one page enabled cancel button
	//	m_pCancelInsertBtn->SetEnabled(true);

	//Close();

	Statistics::GetInstance()->Report(STAT_INSERT_PPT);
	return true;

}

CResourceItemUI* CPreviewDlgUI::CreateItem()
{
	CResourceItemUI * pItem = NULL;

	if( !m_builder.GetMarkup()->IsValid() ) 
	{
		pItem = (CResourceItemUI*)(m_builder.Create(_T("RightBar\\Item\\ResourceItem.xml"), (UINT)0, this, &m_PaintManager));
	}
	else
	{
		pItem = (CResourceItemUI*)(m_builder.Create(this, &m_PaintManager));
	}
	if (pItem == NULL)
		return NULL;
	return pItem;
}


void CPreviewDlgUI::CreateResourceItem()
{
	m_pBottomScrollLayout->SetVisible(false);
	int nCount = 8;
	for (int i = 0; i < nCount; i++)
	{
		CPreviewItemUI * pItem = static_cast<CPreviewItemUI *>(CreateItem());
		if (pItem)
		{
	
			pItem->SetPadding(CDuiRect(8, 15, 0, 0));
			if (i == 0)
			{
				pItem->SetPadding(CDuiRect(0, 15, 0, 0));
			}

			m_pBottomScrollLayout->Add(pItem);


		}
	}
	m_pBottomScrollLayout->GetHorizontalScrollBar()->SetFixedHeight(1);
	m_pBottomScrollLayout->SetVisible(true);
	m_pBottomScrollLayout->Invalidate();
}

void CPreviewDlgUI::SetCurItemIndex(int idx)
{
	m_nCurItemIndex = idx;
}

 
void CPreviewDlgUI::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);	
	delete this;
}


void CPreviewDlgUI::DownloadItemImage( )
{
	if (m_eCurrentType == E_WND_PICTURE)
	{
		CThumbnailItemUI* pItem = static_cast<CThumbnailItemUI*>(m_pBottomScrollLayout->GetItemAt(m_nCurItemIndex));
		if( pItem == NULL )
			return;

		m_dwFullScreenBigPictureDownloadId = NDCloudDownloadFile(pItem->GetHandler()->GetResource(),
			pItem->GetHandler()->GetGuid(),
			pItem->GetHandler()->GetTitle(),
			CloudFileImage,
			0,
			MakeHttpDelegate(this, &CPreviewDlgUI::OnPreviewFullPictureDownloaded));
	}
	else if (m_eCurrentType == E_WND_COURSE)
	{
		CThumbnailItemUI* pItem = static_cast<CThumbnailItemUI*>(m_pBottomScrollLayout->GetItemAt(m_nCurItemIndex));
		CCloudCourseThumbnailHandler* pHandler = (CCloudCourseThumbnailHandler*)pItem->GetHandler();
		if( pItem == NULL )
			return;

		m_dwFullScreenBigPictureDownloadId = NDCloudDownloadCourseFile(pHandler->GetPreviewUrl(),
			pHandler->GetGuid(),
			pHandler->GetTitle(),
			CourseFileThumb,
			MakeHttpDelegate(this, &CPreviewDlgUI::OnPreviewFullPictureDownloaded));

	}
	else if (m_eCurrentType == E_WND_PICTURE_LOCAL)
	{
		CResourceItemUI* pItem = static_cast<CResourceItemUI*>(m_pBottomScrollLayout->GetItemAt(m_nCurItemIndex));
		if( pItem == NULL )
			return;

		THttpNotify notify;
		notify.strFilePath = pItem->GetResource();
		OnPreviewFullPictureDownloaded(&notify);

	}
	else
	{
		CPreviewItemUI* pItem = static_cast<CPreviewItemUI*>(m_pBottomScrollLayout->GetItemAt(m_nCurItemIndex));
		if( pItem == NULL )
			return;

		if( m_eCurrentType == E_WND_COURSE_LOCAL )
		{
			THttpNotify notify;
			notify.strFilePath = pItem->GetUrl();
			OnPreviewFullPictureDownloaded(&notify);
		}
		else
			m_dwFullScreenBigPictureDownloadId = NDCloudDownloadCourseFile(pItem->GetUrl(), m_strPPTGuid, _T(""),  CourseFileThumb, MakeHttpDelegate(this, &CPreviewDlgUI::OnPreviewFullPictureDownloaded));

	}
}

void CPreviewDlgUI::CancelPrevieFullDownLoad()
{
	NDCloudDownloadCancel(m_dwFullScreenBigPictureDownloadId, &MakeHttpDelegate(this, &CPreviewDlgUI::OnPreviewFullPictureDownloaded));
}


void CPreviewDlgUI::ShowPicByThread()
{
	if (!CheckPtr()) return;
	
	m_strPicSourceUrl = m_strPath;
	m_pPreviewBig->SetFixedHeight(360);
	//bool bSucess = m_pPreviewBig->SetBkImage(m_strPath.c_str());

	// get image size
	SIZE size = CRenderEngine::GetImageSize(m_strPath.c_str());

	int nWidth	=	size.cx;
	int nHeight =	size.cy;
	
	if( nWidth > m_nWidth || nHeight > m_nHeight )
	{
		m_strTempThumbImage = GeneratePngPic(m_strPath.c_str());
		m_pPreviewBig->SetBkImage(m_strTempThumbImage.c_str());
	}
	else
	{
		m_pPreviewBig->SetBkImage(m_strPath.c_str());
	}

	m_pFullScreenBtn->SetEnabled(true);	
	m_pInsertPageBtn->SetEnabled(true);	

	m_pAnimation->StopGif();
	m_pAnimation->SetVisible(false);
	m_pPreviewBigLayout->SetVisible(true);

	SIZE sz = m_pPreviewBig->GetImageSize();
	int fixWidth = sz.cx;
	int fixHeight = sz.cy;
	if(sz.cy > 360 || sz.cx > 640)
	{
		bool scaleByWidth = false;
		bool scaleByHeight = false;
		if (sz.cy > 360 )
		{
			int nFixWidth = (sz.cx*360) / sz.cy;
			if(nFixWidth<=640)
			{
				scaleByWidth  = true;
			}
		}
		if (sz.cx > 640)
		{
			int nFixHeight = (640*sz.cy) / sz.cx;
			if(nFixHeight<=360)
			{
				scaleByHeight = true;
			}
		}
		if(scaleByWidth)
		{
			fixWidth = (sz.cx*360) / sz.cy;
			fixHeight = 360;
		}
		else if(scaleByHeight)
		{
			fixHeight = (640*sz.cy) / sz.cx;
			fixWidth = 640;
		}
		else
		{
			if(sz.cx>=sz.cy)
			{
				fixWidth = 640;
				fixHeight = fixWidth*sz.cy/sz.cx;
			}
			else
			{
				fixHeight = 360;
				fixWidth = fixHeight*sz.cx/sz.cy;
			}
		}
	}
	m_pPreviewBig->SetFixedHeight(fixHeight);
	m_pPreviewBig->SetFixedWidth(fixWidth);
	m_pPreviewBigLayout2->SetFixedWidth(fixWidth);

	if (m_eOriginalType != E_WND_VR)
	{
		m_pFileSizeLabel->SetText(m_strPicSize.c_str());
	}
	
	m_pDownloadNumsLabel->SetText(m_strDownloadNums.c_str());
}

void CPreviewDlgUI::OnDownloadBefore( THttpNotify* pHttpNotify )
{
	
}

void CPreviewDlgUI::OnDownloadProgress( THttpNotify* pHttpNotify )
{
	int nPos = (int)(pHttpNotify->fPercent * m_pProgress->GetMaxValue());
	m_pProgress->SetVisible(true);
	m_pProgress->SetValue(nPos);
	m_pInsertVRBtn->SetVisible(false);
	m_pDownloadVRBtn->SetVisible(false);
	m_pPauseVRBtn->SetVisible(true);
}

void CPreviewDlgUI::OnDownloadCompleted( THttpNotify* pHttpNotify )
{
	m_pProgress->SetVisible(false);
	if (pHttpNotify->dwErrorCode != 0)
	{
		m_pInsertVRBtn->SetVisible(false);
		m_pDownloadVRBtn->SetVisible(true);
		m_pPauseVRBtn->SetVisible(false);
	}
	else
	{
		m_pInsertVRBtn->SetVisible(true);
		m_pDownloadVRBtn->SetVisible(false);
		m_pPauseVRBtn->SetVisible(false);
	}
}

void CPreviewDlgUI::OnDownloadInterpose( THttpNotify* pHttpNotify )
{
	if (pHttpNotify->dwErrorCode == eInterpose_Resume)
	{
		m_pInsertVRBtn->SetVisible(false);
		m_pDownloadVRBtn->SetVisible(false);
		m_pPauseVRBtn->SetVisible(true);
	}
	if (pHttpNotify->dwErrorCode == eInterpose_Pause)
	{
		m_pProgress->SetVisible(false);
		m_pInsertVRBtn->SetVisible(false);
		m_pDownloadVRBtn->SetVisible(true);
		m_pPauseVRBtn->SetVisible(false);
	}
	if (pHttpNotify->dwErrorCode == eInterpose_Cancel)
	{
		m_pProgress->SetVisible(false);
		m_pInsertVRBtn->SetVisible(true);
		m_pDownloadVRBtn->SetVisible(false);
		m_pPauseVRBtn->SetVisible(false);
	}
}

void CPreviewDlgUI::OnBtnInsertVR( TNotifyUI& msg )
{
	if (m_pVRItemHandler)
	{
		CThumbnailItemUI* pItem = (CThumbnailItemUI*)m_pBottomScrollLayout->GetItemAt(0);
		TNotifyUI notify;
		notify.wParam		= (WPARAM)eClickFor_Insert;
		notify.pSender		= pItem;
		notify.lParam		= MAKELPARAM(-1, -1);
		notify.ptMouse.x	= -1;
		notify.ptMouse.y	= -1;

		CControlUI* pImageCtrl = pItem->GetImageCtrl();
		if (pItem->GetImageCtrl())
		{
			notify.lParam	= MAKELPARAM(pImageCtrl->GetImageSize().cx, pImageCtrl->GetImageSize().cy);
			if (GetFileAttributes(pImageCtrl->GetBkImage()) !=  INVALID_FILE_ATTRIBUTES)
			{
				notify.sType	= pImageCtrl->GetBkImage();
			}
			else
			{
				notify.sType	= CPaintManagerUI::GetResourcePath();
				notify.sType	+= pImageCtrl->GetBkImage();
			}
		}

		m_pVRItemHandler->SetTrigger(pItem);
		m_pVRItemHandler->DoButtonClick(&notify);
	}
}

void CPreviewDlgUI::OnBtnDownloadVR( TNotifyUI& msg )
{
	if (m_pVRItemHandler)
	{
		CThumbnailItemUI* pItem = (CThumbnailItemUI*)m_pBottomScrollLayout->GetItemAt(0);
		TNotifyUI notify;
		notify.wParam		= (WPARAM)eClickFor_Start;
		notify.pSender		= pItem;
		notify.lParam		= MAKELPARAM(-1, -1);
		notify.ptMouse.x	= -1;
		notify.ptMouse.y	= -1;

		CControlUI* pImageCtrl = pItem->GetImageCtrl();
		if (pItem->GetImageCtrl())
		{
			notify.lParam	= MAKELPARAM(pImageCtrl->GetImageSize().cx, pImageCtrl->GetImageSize().cy);
			if (GetFileAttributes(pImageCtrl->GetBkImage()) !=  INVALID_FILE_ATTRIBUTES)
			{
				notify.sType	= pImageCtrl->GetBkImage();
			}
			else
			{
				notify.sType	= CPaintManagerUI::GetResourcePath();
				notify.sType	+= pImageCtrl->GetBkImage();
			}
		}

		m_pVRItemHandler->SetTrigger(pItem);
		m_pVRItemHandler->DoButtonClick(&notify);
	}
}

void CPreviewDlgUI::OnBtnPauseVR( TNotifyUI& msg )
{
	if (m_pVRItemHandler)
	{
		CThumbnailItemUI* pItem = (CThumbnailItemUI*)m_pBottomScrollLayout->GetItemAt(0);
		TNotifyUI notify;
		notify.wParam		= (WPARAM)eClickFor_Pause;
		notify.pSender		= pItem;
		notify.lParam		= MAKELPARAM(-1, -1);
		notify.ptMouse.x	= -1;
		notify.ptMouse.y	= -1;

		CControlUI* pImageCtrl = pItem->GetImageCtrl();
		if (pItem->GetImageCtrl())
		{
			notify.lParam	= MAKELPARAM(pImageCtrl->GetImageSize().cx, pImageCtrl->GetImageSize().cy);
			if (GetFileAttributes(pImageCtrl->GetBkImage()) !=  INVALID_FILE_ATTRIBUTES)
			{
				notify.sType	= pImageCtrl->GetBkImage();
			}
			else
			{
				notify.sType	= CPaintManagerUI::GetResourcePath();
				notify.sType	+= pImageCtrl->GetBkImage();
			}
		}

		m_pVRItemHandler->SetTrigger(pItem);
		m_pVRItemHandler->DoButtonClick(&notify);
	}
}

void CPreviewDlgUI::SetPreviewTitle( LPCTSTR lpcsTitle )
{
	if (!m_pWindowTitleLabel
		|| !lpcsTitle)
	{
		return;
	}

	m_pWindowTitleLabel->SetText(lpcsTitle);
	m_pWindowTitleLabel->SetToolTip(lpcsTitle);

	SIZE textSize = CRenderEngine::GetTextSize(m_PaintManager.GetPaintDC(), &m_PaintManager, m_pWindowTitleLabel->GetText(), m_pWindowTitleLabel->GetFont(), DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT| DT_NOPREFIX);
	
	if (textSize.cx < 590)
	{
		m_pWindowTitleLabel->SetFixedWidth(textSize.cx);
	}
	
}

void CPreviewDlgUI::SetPreviewSize( DWORD dwFileSize )
{
	TCHAR szSize[32]	= {0};

	if (dwFileSize < 1000 )
	{
		_stprintf_s(szSize, _T("%.2f B"), 
			(dwFileSize  * 1.0f));
	}
	else if (dwFileSize < 1000 * 1000 )
	{
		_stprintf_s(szSize, _T("%.2f KB"), 
			(dwFileSize  * 1.0f / 1024));
	}
	else if (dwFileSize < 1000 * 1000 * 1000 )
	{
		_stprintf_s(szSize, _T("%.2f MB"), 
			(dwFileSize  * 1.0f / (1024 * 1024)));
	}
	else
	{
		_stprintf_s(szSize, _T("%.2f GB"), 
			(dwFileSize  * 1.0f / (1024 * 1024 * 1024)));
	}

	m_pFileSizeLabel->SetText(szSize);
}

CPreviewFullUI* CPreviewFullUI::m_pInstance = NULL;
CPreviewDlgUI*	CPreviewFullUI::m_pPreViewDlg = NULL;

CPreviewFullUI::CPreviewFullUI(void)
{
	m_nHeight = 0;
	m_nWidth = 0;
	m_rImgeSize.cx = 0; 
	m_rImgeSize.cy = 0;

}

CPreviewFullUI::~CPreviewFullUI(void)
{
	OutputDebugString(_T("~CPreviewFullUI\n"));
	if (m_pPreViewDlg)
	{
		m_pPreViewDlg->CancelPrevieFullDownLoad();
	}
}

LPCTSTR CPreviewFullUI::GetWindowClassName( void ) const
{
	return _T("AboutDialog");
}

DuiLib::CDuiString CPreviewFullUI::GetSkinFile()
{
	return _T("Preview\\PreviewFullScreen.xml");
}

DuiLib::CDuiString CPreviewFullUI::GetSkinFolder()
{
	return _T("skins");
}

void CPreviewFullUI::InitFullWnd( LPCTSTR lpszImgFile, bool bSuccess, EPLUS_WND_TYPE eWndType )
{
	m_eCurrentType = eWndType;
	m_pPicCtnLayout->SetVisible(true);
	m_layLoading->SetVisible(false);
	m_pLoading->StopGif();
 
	if (E_WND_COURSE == m_eCurrentType || m_eCurrentType == E_WND_COURSE_LOCAL )
	{
		m_pPicCtnLayout->EnableScrollBar(false, false);
		m_pPicCtnLayout->SetFixedHeight(m_nHeight - 50);
		m_pPicCtnLayout->SetFixedWidth(m_nWidth);

		m_pVerticalLayout->SetVisible(false);
		m_pPreviewBig->SetVisible(false);
		int nPaddingX = (m_nWidth / 2) - (PIC_PREVIEW_WIDTH / 2);
		int nPaddingY = (m_nHeight / 2) - (PIC_PREVIEW_HEIGHT /2 );
		if (nPaddingX <= 0 )
		{
			nPaddingX = 0;
			m_pPicCtnLayout->EnableScrollBar(true, true);
		}
		if (nPaddingY <= 0)
		{
			nPaddingY = 0;
		}

		m_pPreviewBig->SetPadding(CDuiRect(nPaddingX, nPaddingY, 0, 0));
		m_pPreviewBig->SetFixedWidth(PIC_PREVIEW_WIDTH);
		m_pPreviewBig->SetFixedHeight(PIC_PREVIEW_HEIGHT);
		m_pPreviewBig->SetBkImage(lpszImgFile);
		m_pVerticalLayout->SetVisible(true);  
		m_pPreviewBig->SetVisible(true);
		m_pPreviewBig->Invalidate();
		m_pPicCtnLayout->Invalidate();

	}
	else if (E_WND_PICTURE == m_eCurrentType
			 || E_WND_PICTURE_LOCAL == m_eCurrentType)
	{

		if (_tcslen(lpszImgFile) == 0 && bSuccess)
		{
			m_layLoading->SetVisible(true);
			m_pPicCtnLayout->SetVisible(false);
			m_pLoading->PlayGif();
			m_pPreViewDlg->DownloadItemImage();
			return;
		}

		ShowBigPicThumb(lpszImgFile);
		
	}
	else if (E_WND_FLASH == m_eCurrentType)
	{
	}
	
	ShowWindow(true);
}

void CPreviewFullUI::InitWindow()
{
	m_pVerticalLayout	= static_cast<CVerticalLayoutUI *>(m_PaintManager.FindControl(_T("PreviewFullDlg")));
	m_pPreviewBig		= static_cast<CControlUI*>(m_pVerticalLayout->FindSubControl(_T("PreviewBig")));
	m_pPreviewError		= static_cast<CControlUI*>(m_pVerticalLayout->FindSubControl(_T("PreviewBigError")));
	m_pPicCtnLayout		= static_cast<CVerticalLayoutUI *>(m_pVerticalLayout->FindSubControl(_T("PreivieMidContainer")));
	m_layLoading		= static_cast<CContainerUI *>(m_pVerticalLayout->FindSubControl(_T("loading_layout")));
	m_pLoading			= static_cast<CGifAnimUI *>(m_pVerticalLayout->FindSubControl(_T("loading")));	
	POINT pt;
	::GetCursorPos(&pt);
	::ClientToScreen(GetHWND(), &pt);
	HMONITOR hMonitor = ::MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
	MONITORINFO monitorInfo;
	monitorInfo.cbSize = sizeof(MONITORINFO);
	::GetMonitorInfo(hMonitor, &monitorInfo);

	m_nWidth = monitorInfo.rcMonitor.right-monitorInfo.rcMonitor.left;
	m_nHeight =  monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

	::MoveWindow(m_hWnd, monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top, m_nWidth, m_nHeight, false);

}

void CPreviewFullUI::OnBtnClose(TNotifyUI& msg)
{
	Close();
}

CPreviewFullUI* CPreviewFullUI::GetInstance( CPreviewDlgUI* pDlg, HWND hwnd, EPLUS_WND_TYPE eWndType, LPCTSTR lpszImgFile, bool bSuccess )
{
	if( m_pInstance == NULL )
	{
		m_pInstance = new CPreviewFullUI;
	}

	if (!m_pInstance->GetHWND() || !IsWindow(m_pInstance->GetHWND()))
	{
		m_pInstance->Create(hwnd, _T("PreviewFull"), WS_POPUP, 0, 0, 0, 0, 0);

	}
	m_pInstance->m_pPreViewDlg = pDlg;
	m_pInstance->InitFullWnd(lpszImgFile, bSuccess, eWndType);
	m_pInstance->ShowWindow(SW_SHOWNORMAL);

	return m_pInstance;
}



void CPreviewFullUI::ReleaseInstance()
{
	if (m_pInstance != NULL)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

SIZE CPreviewFullUI::GetImageFileSize(LPCTSTR lpszFile)
{
	const TImageInfo* pImageInfo = NULL;

	pImageInfo = m_PaintManager.GetImageEx((LPCTSTR)lpszFile);

	if( pImageInfo ) 
	{
		m_rImgeSize.cx = pImageInfo->nX;
		m_rImgeSize.cy = pImageInfo->nY;
	}
	return m_rImgeSize;




}

LRESULT CPreviewFullUI::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if ( (m_eCurrentType != E_WND_COURSE 
			&& m_eCurrentType != E_WND_COURSE_LOCAL
			&& m_eCurrentType != E_WND_PICTURE_LOCAL
			&& m_eCurrentType != E_WND_PICTURE) || NULL == m_pPreViewDlg)
	{
		return TRUE;
	}

	if( uMsg == WM_KEYDOWN )
	{
		TNotifyUI msg;
		if ((wParam == VK_UP || wParam == VK_LEFT))
		{
			m_pPreViewDlg->OnBtnPageLeft(msg);
			m_pPreViewDlg->CancelPrevieFullDownLoad();

			m_layLoading->SetVisible(true);
			m_pPicCtnLayout->SetVisible(false);
			m_pLoading->PlayGif();
			m_pPreViewDlg->DownloadItemImage();
			
		}
		else if (wParam == VK_DOWN || wParam == VK_RIGHT)
		{
			m_pPreViewDlg->OnBtnPageRight(msg);
			m_pPreViewDlg->CancelPrevieFullDownLoad();

			m_layLoading->SetVisible(true);
			m_pPicCtnLayout->SetVisible(false);
			m_pLoading->PlayGif();
			m_pPreViewDlg->DownloadItemImage();

		}
	}

	return TRUE;
}

void CPreviewFullUI::ShowBigPicNormal(tstring strImage)
{
	m_pPicCtnLayout->EnableScrollBar(true, false);

	m_pPicCtnLayout->SetFixedHeight(m_nHeight - 50);
	m_pPicCtnLayout->SetFixedWidth(m_nWidth);

	SIZE szImg = GetImageFileSize(strImage.c_str());

	int nPaddingX = (m_nWidth / 2) - (szImg.cx / 2);
	int nPaddingY = (m_nHeight / 2) - (szImg.cy /2 );
	
	if (nPaddingX <= 0 )
	{
		nPaddingX = 0;
		m_pPicCtnLayout->EnableScrollBar(true, true);
	}
	if (nPaddingY <= 0)
	{
		nPaddingY = 0;
	}

	m_pPreviewBig->SetPadding(CDuiRect(nPaddingX, nPaddingY, 0, 0));

	if (szImg.cx == 0 || szImg.cy == 0)
	{

		m_pPreviewBig->SetVisible(false);
		m_pPreviewError->SetVisible(true);
		m_pVerticalLayout->SetVisible(false);
		int nPaddingX = (m_nWidth / 2) -  (90 / 2);
		int nPaddingY = (m_nHeight / 2) - (70 /2 );
		m_pPreviewError->SetFixedWidth(90);
		m_pPreviewError->SetFixedHeight(70);
		m_pPreviewError->SetPadding(CDuiRect(nPaddingX, nPaddingY, 0, 0));
		m_pVerticalLayout->SetVisible(true);
		m_pPreviewError->Invalidate();
		m_pVerticalLayout->Invalidate();
		ShowWindow(SW_SHOW);
		return;

	}



	m_pVerticalLayout->SetVisible(false);
	

	m_pPreviewBig->SetFixedHeight(szImg.cy);
	m_pPreviewBig->SetFixedWidth(szImg.cx);

	m_pPreviewBig->SetBkImage(strImage.c_str());
	
	m_pVerticalLayout->SetVisible(true);
	m_pPreviewBig->Invalidate();
	m_pPicCtnLayout->SetNeedCalHorizenWidth(true);
	m_pPicCtnLayout->Invalidate();
}

void CPreviewFullUI::ShowBigPicThumb( tstring strImage )
{
	SIZE size = CRenderEngine::GetImageSize(strImage.c_str());

	int nWidth	=	size.cx;
	int nHeight =	size.cy;

	if( nWidth > m_nWidth || nHeight > m_nHeight )
	{
		m_pPreViewDlg->m_strTempThumbImage = m_pPreViewDlg->GeneratePngPic(strImage.c_str());
		m_pPreviewBig->SetBkImage(m_pPreViewDlg->m_strTempThumbImage.c_str());
	}
	else
		m_pPreviewBig->SetBkImage(strImage.c_str());
	//bool bSucess = m_pPreviewBig->SetBkImageWithResult(strImage.c_str());
	//if(!bSucess)
	//	m_pPreviewBig->SetBkImage(m_pPreViewDlg->m_strTempThumbImage.c_str());
	SIZE sz = m_pPreviewBig->GetImageSize();
	int nScreenHeight= m_nHeight - 50;
	int nScreenWidth = m_nWidth;
	m_pPicCtnLayout->SetFixedHeight(m_nHeight - 50);
	m_pPicCtnLayout->SetFixedWidth(m_nWidth);

	if (sz.cx == 0 || sz.cy == 0)
	{

		m_pPreviewBig->SetVisible(false);
		m_pPreviewError->SetVisible(true);
		m_pVerticalLayout->SetVisible(false);
		int nPaddingX = (m_nWidth / 2) -  (90 / 2);
		int nPaddingY = (m_nHeight / 2) - (70 /2 );
		m_pPreviewError->SetFixedWidth(90);
		m_pPreviewError->SetFixedHeight(70);
		m_pPreviewError->SetPadding(CDuiRect(nPaddingX, nPaddingY, 0, 0));
		m_pVerticalLayout->SetVisible(true);
		m_pPreviewError->Invalidate();
		m_pVerticalLayout->Invalidate();
		ShowWindow(SW_SHOW);
		return;

	}

	if (sz.cy > nScreenHeight )
	{
		double nFixWidth = (sz.cx*nScreenHeight*1.00000) / sz.cy;
		m_pPreviewBig->SetFixedHeight(nScreenHeight);
		m_pPreviewBig->SetFixedWidth(int(nFixWidth));
		double nPaddingX = (nScreenWidth*1.00000 / 2) - (nFixWidth*1.00000 / 2);
		if (nPaddingX >= 0 )
		{
			m_pPreviewBig->SetPadding(CDuiRect(int(nPaddingX), 0, 0, 0));
		}

	}
	else if (sz.cx > nScreenWidth)
	{


		double nFixHeight = (nScreenWidth*sz.cy*1.00000) / sz.cx;
		m_pPreviewBig->SetFixedHeight(nFixHeight);
		m_pPreviewBig->SetFixedWidth(int(nScreenWidth));

		double nPaddingY = (nScreenHeight *1.00000/ 2) - (nFixHeight*1.00000 / 2);
		if (nPaddingY >= 0 )
		{
			m_pPreviewBig->SetPadding(CDuiRect(0, int(nPaddingY), 0, 0));
		}

	}
	else
	{
		double nPaddingX = (nScreenWidth *1.00000/ 2) - (sz.cx*1.00000 / 2);
		double nPaddingY = (nScreenHeight*1.00000 / 2) - (sz.cy*1.00000 / 2);
		if (nPaddingX >= 0 && nPaddingY >= 0)
		{
			m_pPreviewBig->SetPadding(CDuiRect(int(nPaddingX), int(nPaddingY), 0, 0));
		}
		m_pPreviewBig->SetFixedHeight(sz.cy);
		m_pPreviewBig->SetFixedWidth(sz.cx);
		int nX = m_pPreviewBig->GetFixedWidth();
		int nY = m_pPreviewBig->GetFixedHeight();
	}

	m_pPreviewBig->SetVisible(true);
	m_pPreviewError->SetVisible(false);
	m_pVerticalLayout->SetVisible(false);

	m_pVerticalLayout->SetVisible(true);
	m_pPreviewBig->Invalidate();
	m_pPicCtnLayout->Invalidate();
}

CControlUI* CPreviewFullUI::CreateControl( LPCTSTR pstrClass )
{
	if( _tcscmp(pstrClass, _T("GifAnim")) == 0 )
		return new CGifAnimUI;
	return __super::CreateControl(pstrClass);
}

void CPreviewFullUI::OnFinalMessage( HWND hWnd )
{
	__super::OnFinalMessage(hWnd);
	delete this;
	m_pInstance = NULL;
}
