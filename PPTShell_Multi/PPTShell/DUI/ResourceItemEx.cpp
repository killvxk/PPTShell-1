#include "StdAfx.h"


#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "Util/Util.h"
#include "Util/Stream.h"
#include "DUI/DragDialogUI.h"

#include "DUI/ResourceItemEx.h"
#include "DUI/VRItemEx.h"

#include "DUI/ResourceStyleable.h"
#include "DUI/PhotoStyleable.h"
#include "DUI/VideoStyleable.h"
#include "DUI/FlashStyleable.h"
#include "DUI/VolumeStyleable.h"
#include "DUI/CourseStyleable.h"
#include "DUI/3DResourceStyleable.h"
#include "DUI/VRResourceStyleable.h"
#include "DUI/QuestionStyleable.h"
#include "DUI/BankCourseStyleable.h"
#include "DUI/PPTTemplateStyleable.h"
#include "DUI/NdpStyleable.h"
#include "DUI/NdpxStyleable.h"

#include "DUI/IComponent.h"
#include "DUI/ItemComponent.h"
#include "DUI/CloudComponent.h"
#include "DUI/MaskComponent.h"
#include "DUI/ToolbarComponent.h"

#include "Util/Tween.h"
#include "DUI/DragDialogUI.h"
#include "DUI/ItemExplorer.h"
#include "PPTControl/PPTController.h"
#include "NDCloud/NDCloudAPI.h"

//CloudPhotoHandler
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
#include "DUI/CloudFlashHandler.h"
#include "DUI/CloudVideoHandler.h"
#include "DUI/CloudCourseHandler.h"
#include "DUI/Cloud3DResourceHandler.h"
#include "DUI/CloudQuestionHandler.h"
#include "DUI/CloudInteractQuestionHandler.h"
#include "DUI/CloudNdpHandler.h"
#include "DUI/CloudNdpXHandler.h"
#include "DUI/CloudVRResourceHandler.h"

#include "DUI/Toast.h"
#include "NDCloud/NDCloudContentService.h"


#define EASE_IN		100
#define EASE_OUT	 200
#define TIMER_FRAME	 10


DuiLib::CDialogBuilder CResourceItemExUI::m_ItemToolbarBuilder;
CResourceItemExUI::CResourceItemExUI( IComponent* pComponent )
	: CThumbnailItemUI(pComponent)
{
	m_bHasSetThumbnail		= false;
	m_bHasSubItems			= false;

	//ui
	m_layToolBarStyle		= NULL;
	m_layToolBar			= NULL;
	m_lbTitle				= NULL;
	m_proDownload			= NULL;
	m_pEmptyCtrl			= NULL;

	//slide
	m_fCurrentFrame			= 0;
	m_bEnableToolbar		= true;
	m_bSildableToolbar		= false;

	//drag
	m_bEnableDrag			= true;
	m_pDragDialog			= NULL;
	m_dwDragStartTime		= 0;
	m_ptDragStartPos.x		= -1;
	m_ptDragStartPos.y		= -1;

}

CResourceItemExUI::~CResourceItemExUI()
{
	if (GetHandler())
	{
		GetHandler()->DoDestroy(this);
		SetHandler(NULL);
	}
}


void CResourceItemExUI::DoInit()
{
	__super::DoInit();


	m_proDownload	= static_cast<CProgressUI*>( FindSubControl(_T("progress")));
	m_layToolBar	= static_cast<CHorizontalLayoutUI*>( FindSubControl(_T("toolbar")));
	m_lbTitle		= static_cast<CLabelUI*>( FindSubControl(_T("name")));
	m_pEmptyCtrl	= static_cast<CControlUI*>(FindSubControl(_T("empty")));

	//icon
	// 	if (m_bNeedTypeIcon)
	// 	{
	// 		CControlUI * pIcon = new CControlUI();
	// 		pIcon->SetName(_T("icon"));
	// 		pIcon->SetFixedWidth(44);
	// 		pIcon->SetFixedHeight(20);
	// 		pIcon->OnEvent	+= MakeDelegate(this, &CResourceItemExUI::OnEmptyControlEvent);
	// 
	// 		pEmpty->Add(pIcon);
	// 	}

	if (m_bEnableToolbar)
	{
		if (!m_bSildableToolbar)
		{
			m_layToolBar->SetVisible(true);
		}
	}

	if (GetHandler())
	{
		SetTitle(GetHandler()->GetTitle());
		this->SetName(GetHandler()->GetTitle());
	}

	GetOption()->OnSelect	+= MakeDelegate(this, &CResourceItemExUI::OnItemSelect);
	this->OnNotify			+= MakeDelegate(this, &CResourceItemExUI::OnControlNotify);

	
}

bool CResourceItemExUI::OnItemSelect( void* pObj )
{
	TNotifyUI* pNotify	= (TNotifyUI*)pObj;
	COptionUI* pOption	= (COptionUI*)pNotify->pSender;

	if (pOption->IsSelected())
	{	
		m_layToolBar->SetAttribute(_T("inset"), _T("2,0,2,2"));
	}
	else
	{
		m_layToolBar->SetAttribute(_T("inset"), _T("0,0,0,0"));
	}

	return true;
}

bool CResourceItemExUI::OnControlNotify( void* pObj )
{
	__super::OnControlNotify(pObj);
	TNotifyUI* pNotify = (TNotifyUI*)pObj;

	if (pNotify->pSender 
		&& pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		int			nBtnIndex	= 0;
		TNotifyUI	notify;
		if (_stscanf_s(pNotify->pSender->GetName(), _T("btn%d"), &nBtnIndex) == 1)
		{
			notify.wParam		= (WPARAM)pNotify->pSender->GetTag();
			notify.pSender		= this;
			notify.lParam		= MAKELPARAM(-1, -1);
			notify.ptMouse.x	= -1;
			notify.ptMouse.y	= -1;

			if (GetImageCtrl())
			{
				notify.lParam	= MAKELPARAM(GetImageCtrl()->GetImageSize().cx, GetImageCtrl()->GetImageSize().cy);
				if (GetFileAttributes(GetImageCtrl()->GetBkImage()) !=  INVALID_FILE_ATTRIBUTES)
				{
					notify.sType	= GetImageCtrl()->GetBkImage();
				}
				else
				{
					notify.sType	= CPaintManagerUI::GetResourcePath();
					notify.sType	+= GetImageCtrl()->GetBkImage();
				}
			}

			if (GetHandler())
			{
				GetHandler()->SetTrigger(this);
				GetHandler()->DoButtonClick(&notify);
			}
		}
	}
	else if (pNotify->pSender 
		&& pNotify->sType == _T("menuclick"))
	{
		int			nBtnIndex	= 0;
		if (GetHandler())
		{
			GetHandler()->SetTrigger(this);
			GetHandler()->DoMenuClick(pNotify);
		}
	}

	return true;
}

bool CResourceItemExUI::OnControlEvent( void* pObj )
{
	__super::OnControlEvent(pObj);

	TEventUI* pEvent = (TEventUI*)pObj;

	if (pEvent->pSender
		&& pEvent->pSender->GetName() == _T("empty"))
	{
		GetOption()->Event(*pEvent);
	}

	if (pEvent->Type == UIEVENT_KEYDOWN)
	{
		if (GetHandler())
		{
			GetHandler()->SetTrigger(this);
			GetHandler()->DoKeyDown(pEvent);
		}
	}

	if (m_bEnableDrag && !m_proDownload->IsVisible())
	{
		OnItemEvent(pObj);
	}

	if(pEvent->Type == UIEVENT_RBUTTONUP
		&& pEvent->pSender->GetName() == _T("empty"))
	{
		TNotifyUI notify;
		notify.pSender = this;

		//not progressing
		if (!m_proDownload->IsVisible())
		{
			if (GetHandler())
			{
				GetHandler()->SetTrigger(this);
				GetHandler()->DoRClick(&notify);
			}
		}
		
	}

	if (m_bSildableToolbar)
	{
		if (pEvent->Type == UIEVENT_MOUSEENTER
			&& _tcsicmp(pEvent->pSender->GetName(), _T("empty")) == 0)
		{
			EaseIn();
		}
		else if (pEvent->Type == UIEVENT_MOUSELEAVE)
		{
			CControlUI* pNewCtrl = (CControlUI*)pEvent->wParam;
			if (pNewCtrl 
				&&( _tcsicmp(pNewCtrl->GetName(), _T("btn1")) == 0
				||_tcsicmp(pNewCtrl->GetName(), _T("btn2")) == 0
				||_tcsicmp(pNewCtrl->GetName(), _T("progress")) == 0
				||_tcsicmp(pNewCtrl->GetName(), _T("icon")) == 0))
			{
				return true;
			}
			EaseOut();
		}
	}
	return true;
}

void CResourceItemExUI::EaseIn()
{
	if (!m_bEnableToolbar)
	{
		return;
	}
	m_fCurrentFrame = 0;
	GetManager()->KillTimer( this, EASE_OUT );
	GetManager()->SetTimer( this, EASE_IN, 10 );

}

void CResourceItemExUI::EaseOut()
{
	if (!m_bEnableToolbar)
	{
		return;
	}

	m_fCurrentFrame = 0;
	GetManager()->KillTimer( this, EASE_IN );
	GetManager()->SetTimer( this, EASE_OUT, 10 );
}

void CResourceItemExUI::OnTimer( UINT_PTR idEvent )
{
	if (idEvent == EASE_IN)
	{
		if (!m_layToolBar->IsVisible())
		{
			m_layToolBar->SetFixedHeight(0);
			m_layToolBar->SetVisible(true);

		}
		if (m_layToolBar->GetFixedHeight() >= 30)
		{
			GetManager()->KillTimer( this, EASE_IN );
			return;
		}

		m_fCurrentFrame ++;
		float a = Interpolator::getInterpolator(Interpolator::TWEEN_Quadratic, m_fCurrentFrame, 0, 35, TIMER_FRAME, Interpolator::EASEIN);
		m_layToolBar->SetFixedHeight((int)a == 0 ? 1: (int)a);

	}
	else if (idEvent == EASE_OUT)
	{
		if (m_layToolBar->GetFixedHeight() <= 4)
		{
			GetManager()->KillTimer( this, EASE_OUT );
			m_layToolBar->SetVisible(false);
			return;
		}
		m_fCurrentFrame ++;
		float a = Interpolator::getInterpolator(Interpolator::TWEEN_Quadratic, m_fCurrentFrame, 0, 35, TIMER_FRAME, Interpolator::EASEOUT);
		m_layToolBar->SetFixedHeight(35 - (int)a);
	}

}

void CResourceItemExUI::DoEvent( TEventUI& event )
{
	if (event.Type == UIEVENT_TIMER)
	{
		OnTimer((UINT_PTR)event.wParam );
	}
	__super::DoEvent(event);
}

void CResourceItemExUI::EnableToolbar( bool bEnable )
{
	m_bEnableToolbar = bEnable;
	if (m_layToolBar)
	{
		m_layToolBar->SetVisible(bEnable);
	}
	
}

bool CResourceItemExUI::OnItemEvent(void *param)
{
	TEventUI* pEvent = (TEventUI*)param;

	if (pEvent->Type == UIEVENT_BUTTONDOWN )
	{
		m_dwDragStartTime	= pEvent->dwTimestamp;
		m_ptDragStartPos	= pEvent->ptMouse;
	
	}
	else if( pEvent->Type == UIEVENT_BUTTONUP )
	{
		if( m_pDragDialog != NULL )
		{
			m_pDragDialog->Close();
			m_pDragDialog = NULL;	

			CRect rt;
			HWND hWnd = AfxGetMainWnd()->GetSafeHwnd();
			GetWindowRect(hWnd, &rt);

			rt.top = rt.top+205;

			POINT ptMouse;
			GetCursorPos(&ptMouse);


			CRect rtPPT;
			HWND hPPTWnd = CPPTController::GetPPTHwnd();
			GetWindowRect(hPPTWnd, &rtPPT);

		 
			if( pEvent->ptMouse.x < 0 &&  rt.PtInRect(ptMouse) && rtPPT.PtInRect(ptMouse) )
			{
				OnItemDragFinish(ptMouse);
			}
			
		}

		m_dwDragStartTime = 0;

	}
	else if( pEvent->Type == UIEVENT_MOUSEMOVE )
	{
		POINT ptMouse;
		GetCursorPos(&ptMouse);

		int nDiff = abs(pEvent->ptMouse.x - m_ptDragStartPos.x) + abs(pEvent->ptMouse.y - m_ptDragStartPos.y);

		if( m_dwDragStartTime != 0 && nDiff > 10 )
		{
			int width = GetWidth();
			int height = GetHeight() - GetTitleCtrl()->GetHeight();

			if( m_pDragDialog == NULL )
			{
				HWND hWnd = AfxGetMainWnd()->GetSafeHwnd();

				POINT ptMouse;
				GetCursorPos(&ptMouse);

				//
				LPCTSTR strImage = GetImageCtrl()->GetBkImage();
				tstring strImagePath = strImage;

				Gdiplus::Image* pImage = new Image(AnsiToUnicode(strImage).c_str());
				int nWidth = pImage->GetWidth();
				int nHeight = pImage->GetHeight();
				
				if( nWidth > 1000 || nHeight > 1000 )
				{
					WCHAR wszTempPath[MAX_PATH];
					GetTempPathW(MAX_PATH, wszTempPath);

					WCHAR wszTempFileName[MAX_PATH];
					swprintf_s(wszTempFileName, L"%s\\%u.png", wszTempPath, GetCycleCount());

					GUID png = {0x557CF406, 0x1A04, 0x11D3, 0x9A, 0x73, 0x00, 0x00, 0xF8, 0x1E, 0xF3, 0x2E};

					Gdiplus::Image* pThumbnail = pImage->GetThumbnailImage(width, height);
					pThumbnail->Save(wszTempFileName, &png);

					strImagePath = Un2Str(wszTempFileName);

				}

				delete pImage;


				m_pDragDialog = DragDialogUI::GetInstance();
				m_pDragDialog->SetBkImage(strImagePath.c_str());
				m_pDragDialog->SetUserData(&m_pDragDialog);
				m_pDragDialog->Create(hWnd, _T("DragWindow"), WS_VISIBLE, 0, 0, 0, 0, 0);
			}

			int x = ptMouse.x - width / 2;
			int y = ptMouse.y - height / 2;

			if( m_pDragDialog != NULL && ::IsWindow(m_pDragDialog->GetHWND()) )
				MoveWindow(m_pDragDialog->GetHWND(), x, y, width, height, TRUE);
		}
	}
	else if(pEvent->Type == UIEVENT_RBUTTONUP)
	{
		
	}

	return true;
}

void CResourceItemExUI::OnItemDragFinish( POINT pt )
{
	if (GetHandler())
	{

		TNotifyUI notify;
		notify.pSender  = this;
		notify.wParam	= 0;
		notify.ptMouse	= pt;
		notify.lParam	= MAKELPARAM(-1, -1);

		if (GetImageCtrl())
		{
			notify.lParam	= MAKELPARAM(GetImageCtrl()->GetImageSize().cx, GetImageCtrl()->GetImageSize().cy);
			if (GetFileAttributes(GetImageCtrl()->GetBkImage()) !=  INVALID_FILE_ATTRIBUTES)
			{
				notify.sType	= GetImageCtrl()->GetBkImage();
			}
			else
			{
				notify.sType	= CPaintManagerUI::GetResourcePath();
				notify.sType	+= GetImageCtrl()->GetBkImage();
			}
			
		}
		GetHandler()->SetTrigger(this);
		GetHandler()->DoDropDown(&notify);
	}
}

void CResourceItemExUI::EnableDrag( bool bEnable )
{
	m_bEnableDrag = bEnable;
}

void CResourceItemExUI::SildableToolbar( bool bSildable )
{
	m_bSildableToolbar = bSildable;
}

CLabelUI* CResourceItemExUI::GetTitleCtrl()
{
	return m_lbTitle;
}


void CResourceItemExUI::SetTitle( LPCTSTR lptcsTitle )
{
	if (m_lbTitle && lptcsTitle)
	{
		m_lbTitle->SetText(lptcsTitle);
		m_lbTitle->SetToolTip(lptcsTitle);
	}
}

void CResourceItemExUI::ShowProgress( bool bVisible )
{
	m_proDownload->SetVisible(bVisible);
	for (int i = 0; i < m_layToolBar->GetCount(); ++i)
	{
		CButtonUI* pBtn  = dynamic_cast<CButtonUI* >(m_layToolBar->GetItemAt(i));
		if (pBtn)
		{
			pBtn->SetEnabled(!bVisible);
		}
	}
}

void CResourceItemExUI::SetProgress( int nPos )
{
	m_proDownload->SetValue(nPos);
}


void CResourceItemExUI::OnDownloadBefore( THttpNotify* pHttpNotify )
{
	this->ShowProgress(true);

}

void CResourceItemExUI::OnDownloadProgress( THttpNotify* pHttpNotify )
{
	int nPos = (int)(pHttpNotify->fPercent * m_proDownload->GetMaxValue());

	this->ShowProgress(true);
	this->SetProgress(nPos);

	TCHAR szTip[128]	= {0};
	TCHAR szSpeed[16]	= {0};
	TCHAR szRemain[32]	= {0};
	TCHAR szElapse[32]	= {0};
	TCHAR szSize[32]	= {0};
	int		nTemp		= 0;

	if (pHttpNotify->dwErrorCode == -1)
	{
		_stprintf_s(szTip, _T("<n>队列中...<n>"));
	}
	else
	{
		_stprintf_s(szSpeed, _T("%.1f %s"), 
			pHttpNotify->nSpeed < 1000 ? pHttpNotify->nSpeed : (pHttpNotify->nSpeed  * 1.0f / 1024),
			pHttpNotify->nSpeed < 1000 ? _T("KB/S") : _T("MB/S"));

		nTemp = pHttpNotify->nElapseTime;
		_stprintf_s(szElapse, _T("%02d:%02d:%02d"), 
			pHttpNotify->nElapseTime / 3600,
			(nTemp %= 3600, nTemp / 60),
			pHttpNotify->nElapseTime % 60);

		nTemp = pHttpNotify->nRemainTime;
		_stprintf_s(szRemain, _T("%02d:%02d:%02d"), 
			pHttpNotify->nRemainTime / 3600,
			(nTemp %= 3600, nTemp / 60),
			pHttpNotify->nRemainTime % 60);


		if (pHttpNotify->nTotalSize < 1000 )
		{
			_stprintf_s(szSize, _T("%.2f B"), 
				(pHttpNotify->nTotalSize  * 1.0f));
		}
		else if (pHttpNotify->nTotalSize < 1000 * 1000 )
		{
			_stprintf_s(szSize, _T("%.2f KB"), 
				(pHttpNotify->nTotalSize  * 1.0f / 1024));
		}
		else if (pHttpNotify->nTotalSize < 1000 * 1000 * 1000 )
		{
			_stprintf_s(szSize, _T("%.2f MB"), 
				(pHttpNotify->nTotalSize  * 1.0f / (1024 * 1024)));
		}
		else
		{
			_stprintf_s(szSize, _T("%.2f GB"), 
				(pHttpNotify->nTotalSize  * 1.0f / (1024 * 1024 * 1024)));
		}

		_stprintf_s(szTip, _T("下载速度：%s<n>下载用时：%s<n>剩余时间：%s<n>文件大小：%s<n>"), 
			szSpeed,
			szElapse,
			szRemain,
			szSize);
	}
	m_pEmptyCtrl->SetToolTip(szTip);


}

void CResourceItemExUI::OnDownloadCompleted( THttpNotify* pHttpNotify )
{
	if (!pHttpNotify || pHttpNotify->dwErrorCode != 0)
	{
		this->ShowProgress(false);
	}

	this->SetProgress(m_proDownload->GetMaxValue());
	this->ShowProgress(false);
	this->SetProgress(0);

	m_pEmptyCtrl->SetToolTip(_T(""));

}

void CResourceItemExUI::OnDownloadInterpose( THttpNotify* pHttpNotify )
{
	if (pHttpNotify->dwErrorCode == eInterpose_Cancel)
	{
		this->SetProgress(m_proDownload->GetMaxValue());
		this->ShowProgress(false);
		this->SetProgress(0);

		m_pEmptyCtrl->SetToolTip(_T(""));
	}

}

void CResourceItemExUI::OnDeleteComplete( LPCTSTR lptcsError )
{
	if (lptcsError)
	{
		//toast or ?
		CToast::Toast("删除失败:" + tstring(lptcsError));
		return;
	}

	if (this->HasSubItems())
	{
		CItemExplorerUI::GetInstance()->ShowWindow(false);
		CItemExplorerUI::GetInstance()->ResetUI();
	}

	CContainerUI* pParent = (CContainerUI*)this->GetParent();
	pParent->Remove(this);


}

void CResourceItemExUI::OnRenameComplete( LPCTSTR lptcsError, LPCTSTR lptcsNewName )
{
	if (lptcsError)
	{
		//toast or ?
		CToast::Toast("重命名失败:" + tstring(lptcsError));
		return;
	}

	SetTitle(lptcsNewName);
}

void CResourceItemExUI::OnGetThumbnailCompleted( LPCTSTR lptcsPath )
{
	__super::OnGetThumbnailCompleted(lptcsPath);

	if (!IsResourceExist(lptcsPath))
	{
		if (dynamic_cast<CCloudPhotoHandler*>(GetHandler()))
		{
			SetImage(_T("Item\\item_bg_image_none.png"));
			m_layToolBar->SetVisible(false);
		}
	}
}

CResourceItemExUI* CResourceItemExUI::AllocResourceItem( int nItemType )
{
	CResourceItemExUI* pItem = NULL;
	switch (nItemType)
	{
		case CloudFileCourse:
		case CloudFilePPTTemplate:
		case DBankCourse:
			{
				IComponent* pCompnent = NULL;
				if(nItemType == CloudFileCourse)
				{
					pCompnent = new CToolbarComponent(&CCourseStyleable::Styleable);
				}
				else if(nItemType == CloudFilePPTTemplate)
				{
					pCompnent = new CToolbarComponent(&CPPTTemplateStyleable::Styleable);
				}
				else
				{
					pCompnent = new CToolbarComponent(&CBankCourseStyleable::Styleable);
				}

				pCompnent				= new CMaskComponent(pCompnent);
				pCompnent				= new CItemComponent(pCompnent);
				pCompnent				= new CCloudComponent(pCompnent);
				pItem					= new CResourceItemExUI(pCompnent);

				CCloudCourseHandler* pHandler = new CCloudCourseHandler();
				pHandler->SetIsDbank(nItemType == DBankCourse);
				pHandler->SetContentType(CONTENT_COURSEWARE);

				pItem->SetHandler(pHandler);
				pItem->SetHasSubItems(true);
	
			}
			break;
		case CloudFileVideo:
		case DBankVideo:
			{
				IComponent* pCompnent	= new CToolbarComponent(&CVideoStyleable::Styleable);
				pCompnent				= new CMaskComponent(pCompnent);
				pCompnent				= new CItemComponent(pCompnent);
				pCompnent				= new CCloudComponent(pCompnent);
				pItem					= new CResourceItemExUI(pCompnent);

				CCloudVideoHandler* pHandler = new CCloudVideoHandler();
				pHandler->SetIsDbank(nItemType == DBankVideo);
				pHandler->SetContentType(CONTENT_ASSETS);
				pItem->SetHandler(pHandler);
			}
			break;
		case CloudFileImage:
		case DBankImage:
			{
				IComponent* pCompnent	= new CToolbarComponent(&CPhotoStyleable::Styleable);
				pCompnent				= new CMaskComponent(pCompnent);
				pCompnent				= new CItemComponent(pCompnent);
				pCompnent				= new CCloudComponent(pCompnent);
				pItem					= new CResourceItemExUI(pCompnent);

				CCloudPhotoHandler* pHandler = new CCloudPhotoHandler();
				pHandler->SetIsDbank(nItemType == DBankImage);
				pHandler->SetContentType(CONTENT_ASSETS);
				pItem->SetHandler(pHandler);
			}
			break;
		case CloudFileFlash:
		case DBankFlash:
			{
				IComponent* pCompnent	= new CToolbarComponent(&CFlashStyleable::Styleable);
				pCompnent				= new CMaskComponent(pCompnent);
				pCompnent				= new CItemComponent(pCompnent);
				pCompnent				= new CCloudComponent(pCompnent);
				pItem					= new CResourceItemExUI(pCompnent);

				CCloudFlashHandler* pHandler = new CCloudFlashHandler();
				pHandler->SetIsDbank(nItemType == DBankFlash);
				pHandler->SetContentType(CONTENT_ASSETS);
				pItem->SetHandler(pHandler);
			}
			break;
		case CloudFileVolume:
		case DBankVolume:
			{
				IComponent* pCompnent	= new CToolbarComponent(&CVolumeStyleable::Styleable);
				pCompnent				= new CMaskComponent(pCompnent);
				pCompnent				= new CItemComponent(pCompnent);
				pCompnent				= new CCloudComponent(pCompnent);
				pItem					= new CResourceItemExUI(pCompnent);

				CCloudVideoHandler* pHandler = new CCloudVideoHandler();
				pHandler->SetHasPreviewUrl(false);
				pHandler->SetIsDbank(nItemType == DBankVolume);
				pHandler->SetContentType(CONTENT_ASSETS);
				pItem->SetHandler(pHandler);
			}
			break;
		case CloudFile3DResource:
			{
				IComponent* pCompnent	= new CToolbarComponent(&C3DResourceStyleable::Styleable);
				pCompnent				= new CMaskComponent(pCompnent);
				pCompnent				= new CItemComponent(pCompnent);
				pCompnent				= new CCloudComponent(pCompnent);
				pItem					= new CResourceItemExUI(pCompnent);
				pItem->SetHandler(new CCloud3DResourceHandler());
			}
			break;
		case CloudFileVRResource:
			{
				IComponent* pCompnent	= NULL;
				CToolbarComponent* pToolbarCompnent	= new CToolbarComponent(&CVRResourceStyleable::Styleable);
				pToolbarCompnent->SetNeedInterposeBtn(true);

				pCompnent				= new CMaskComponent(pToolbarCompnent);
				pCompnent				= new CItemComponent(pCompnent);
				pCompnent				= new CCloudComponent(pCompnent);
				pItem					= new CVRItemExUI(pCompnent);
				pItem->SetHandler(new CCloudVRResourceHandler());
			}
			break;
		case CloudFileQuestion:
		case DBankQuestion:
			{
				IComponent* pCompnent	= new CToolbarComponent(&CQuestionStyleable::Styleable);
				pCompnent				= new CMaskComponent(pCompnent);
				pCompnent				= new CItemComponent(pCompnent);
				pCompnent				= new CCloudComponent(pCompnent);
				pItem					= new CResourceItemExUI(pCompnent);
				
				CCloudQuestionHandler* pHandler =  new CCloudQuestionHandler();
				pHandler->SetIsDbank(nItemType == DBankQuestion);
				pHandler->SetContentType(CONTENT_QUESTIONS);

				pItem->SetHandler(pHandler);
			}
			break;
		case CloudFileCoursewareObjects:
		case DBankCoursewareObjects:
			{
				IComponent* pCompnent	= new CToolbarComponent(&CQuestionStyleable::Styleable);
				pCompnent				= new CMaskComponent(pCompnent);
				pCompnent				= new CItemComponent(pCompnent);
				pCompnent				= new CCloudComponent(pCompnent);
				pItem					= new CResourceItemExUI(pCompnent);

				CCloudInteractQuestionHandler* pHandler =  new CCloudInteractQuestionHandler();
				pHandler->SetIsDbank(nItemType == DBankCoursewareObjects);
				pHandler->SetContentType(CONTENT_COUSEWAREOBJECTS);
				pItem->SetHandler(pHandler);
			}
			break;
		case CloudFileNdpCourse:
		case DBankNdpCourse:
			{
				IComponent* pCompnent	= new CToolbarComponent(&CNdpStyleable::Styleable);
				pCompnent				= new CMaskComponent(pCompnent);
				pCompnent				= new CItemComponent(pCompnent);
				pCompnent				= new CCloudComponent(pCompnent);
				pItem					= new CResourceItemExUI(pCompnent);

				CCloudNdpHandler* pHandler =  new CCloudNdpHandler();
				pHandler->SetIsDbank(nItemType == DBankNdpCourse);
				pHandler->SetContentType(CONTENT_COURSEWARE);
				pItem->SetHandler(pHandler);
			}
			break;
		case CloudFileNdpxCourse:
		case DBankNdpxCourse:
			{
				IComponent* pCompnent	= new CToolbarComponent(&CNdpXStyleable::Styleable);
				pCompnent				= new CMaskComponent(pCompnent);
				pCompnent				= new CItemComponent(pCompnent);
				pCompnent				= new CCloudComponent(pCompnent);
				pItem					= new CResourceItemExUI(pCompnent);

				CCloudNdpXHandler* pHandler =  new CCloudNdpXHandler();
				pHandler->SetIsDbank(nItemType == DBankNdpxCourse);
				pHandler->SetContentType(CONTENT_COURSEWARE);
				pItem->SetHandler(pHandler);
			}
			break;
// 		case CloudFileCoursewareObjects:
// 		case DBankCoursewareObjects:
// 			return new CCoursewareObjectsItemUI;
// 		case CloudFileNdpCourse:
// 			return new CNdpCourseItemUI;
	}

	return pItem;
}

bool CResourceItemExUI::HasSubItems()
{
	return m_bHasSubItems;
}

void CResourceItemExUI::SetHasSubItems( bool bHas )
{
	m_bHasSubItems = bHas;
}


