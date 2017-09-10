#include "StdAfx.h"
#include "ResourceItem.h"
#include "Util/Tween.h"
#include "DragDialogUI.h"
#include "Util/Util.h"
#include "ItemExplorer.h"
#include "PPTControl/PPTController.h"

#define EASE_IN		100
#define EASE_OUT	 200
#define TIMER_FRAME	 10

CDialogBuilder	CResourceItemUI::m_builder;
CResourceItemUI::CResourceItemUI()
{
	m_nButtonIndex			= -1;
	m_bAnimation			= false;
	m_fCurrentFrame			= 0;
	m_bEnableToolbar		= true;
	m_bSildableToolbar		= false;
	m_bNeedTypeIcon			= false;
	m_bEnableDrag			= true;

	m_layToolBar			= NULL;
	m_pDragDialog			= NULL;
	m_dwDragStartTime		= 0;
	m_ptDragStartPos.x		= -1;
	m_ptDragStartPos.y		= -1;
	m_ptDragEndPos.x		= -1;
	m_ptDragEndPos.y		= -1;
}


CResourceItemUI::~CResourceItemUI()
{
	
}

void CResourceItemUI::Init()
{
	__super::Init();


	if( !m_builder.GetMarkup()->IsValid() ) {
		m_layToolBarStyle = dynamic_cast<CVerticalLayoutUI*>(m_builder.Create(_T("RightBar\\Item\\ResourceToolbarSytle.xml"), (UINT)0, NULL, this->GetManager()));
	}
	else {
		m_layToolBarStyle = dynamic_cast<CVerticalLayoutUI*>(m_builder.Create(NULL, this->GetManager()));
	}


	CContainerUI* pContent = GetContent();
	assert(pContent);
	pContent->Add(m_layToolBarStyle);


	m_layToolBar	= dynamic_cast<CHorizontalLayoutUI*>( FindSubControl(_T("toolbar")));


	CContainerUI*pEmpty= dynamic_cast<CContainerUI*>(FindSubControl(_T("empty")));
	pEmpty->OnEvent	+= MakeDelegate(this, &CResourceItemUI::OnEmptyControlEvent);

	if (m_bNeedTypeIcon)
	{
		CControlUI * pIcon = new CControlUI();
		pIcon->SetName(_T("icon"));
		pIcon->SetFixedWidth(44);
		pIcon->SetFixedHeight(20);
		pIcon->OnEvent	+= MakeDelegate(this, &CResourceItemUI::OnEmptyControlEvent);

		pEmpty->Add(pIcon);
	}


	CButtonUI* pBtn = dynamic_cast<CButtonUI*>(FindSubControl(_T("btn1")));
	assert(pBtn);
	pBtn->SetHotBkColor(0xFF11B0B6);
	pBtn->SetText(GetButtonText(0));
	pBtn->OnNotify	+= MakeDelegate(this, &CResourceItemUI::OnBtnNotify);
	pBtn->OnEvent	+= MakeDelegate(this, &CResourceItemUI::OnToolbarEvent);
	

	pBtn			= dynamic_cast<CButtonUI*>(FindSubControl(_T("btn2")));
	assert(pBtn);
	pBtn->SetHotBkColor(0xFF11B0B6);
	pBtn->SetText(GetButtonText(1));
	pBtn->OnNotify	+= MakeDelegate(this, &CResourceItemUI::OnBtnNotify);
	pBtn->OnEvent	+= MakeDelegate(this, &CResourceItemUI::OnToolbarEvent);


	assert(m_optBorder);
	m_optBorder->OnNotify	+= MakeDelegate(this, &CResourceItemUI::OnBtnNotify);
	m_optBorder->OnEvent	+= MakeDelegate(this, &CResourceItemUI::OnToolbarEvent);

	if (m_bEnableToolbar)
	{
		if (!m_bSildableToolbar)
		{
			m_layToolBar->SetVisible(true);
		}
	}
	

}

bool CResourceItemUI::OnBtnNotify( void* pNotify )
{
	TNotifyUI* pNotifyUI = (TNotifyUI*)pNotify;

	if ( _tcsicmp(pNotifyUI->pSender->GetName(),_T("item")) == 0)
	{
		if (pNotifyUI->sType == _T("click"))
		{
			CControlUI *pControl = this->GetParent();
			if (this->GetParent()->GetTag() != NULL)
			{
				CControlUI* pLastToolBar = (CControlUI*)this->GetParent()->GetTag();
				pLastToolBar->SetAttribute(_T("inset"), _T("0,0,0,0"));
			}
			m_layToolBar->SetAttribute(_T("inset"), _T("2,0,2,2"));
			this->GetParent()->SetTag((UINT_PTR) m_layToolBar);			
		}

	}

	if (pNotifyUI->sType == _T("click"))
	{
		if ( _tcsicmp(pNotifyUI->pSender->GetName(),_T("btn1")) == 0)
		{
			OnButtonClick(0, *pNotifyUI);
		}
		else if ( _tcsicmp(pNotifyUI->pSender->GetName(),_T("btn2")) == 0)
		{
			OnButtonClick(1, *pNotifyUI);
		}
		else if ( _tcsicmp(pNotifyUI->pSender->GetName(),_T("item")) == 0
			&& pNotifyUI->wParam == 0)
		{
			OnItemClick(*pNotifyUI);
		}
	}

	return true;
}


bool CResourceItemUI::OnEmptyControlEvent( void* pEvent )
{
	TEventUI event = *(TEventUI*)pEvent;
	m_optBorder->Event(event);

	return true;
}

LPCTSTR CResourceItemUI::GetButtonText( int nIndex )
{
	if (nIndex == 0)
	{
		return _T("²åÈë");
	}
	else if (nIndex == 1)
	{
		return _T("Ô¤ÀÀ");
	}

	return NULL;
}


void CResourceItemUI::OnButtonClick( int nButtonIndex, TNotifyUI& msg )
{

}

bool CResourceItemUI::OnToolbarEvent( void* pObj )
{
	TEventUI* pEvent = (TEventUI*)pObj;

	if (m_bEnableDrag)
	{
		OnItemEvent(pObj);
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

void CResourceItemUI::EaseIn()
{
	if (!m_bEnableToolbar)
	{
		return;
	}
	m_fCurrentFrame = 0;
	m_pManager->KillTimer( this, EASE_OUT );
	m_pManager->SetTimer( this, EASE_IN, 10 );

}

void CResourceItemUI::EaseOut()
{
	if (!m_bEnableToolbar)
	{
		return;
	}

	m_fCurrentFrame = 0;
	m_pManager->KillTimer( this, EASE_IN );
	m_pManager->SetTimer( this, EASE_OUT, 10 );
}

void CResourceItemUI::OnTimer( UINT_PTR idEvent )
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
			m_pManager->KillTimer( this, EASE_IN );
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
			m_pManager->KillTimer( this, EASE_OUT );
			m_layToolBar->SetVisible(false);
			return;
		}
		m_fCurrentFrame ++;
		float a = Interpolator::getInterpolator(Interpolator::TWEEN_Quadratic, m_fCurrentFrame, 0, 35, TIMER_FRAME, Interpolator::EASEOUT);
		m_layToolBar->SetFixedHeight(35 - (int)a);
	}
	
}

void CResourceItemUI::DoEvent( TEventUI& event )
{
	if (event.Type == UIEVENT_TIMER)
	{
		OnTimer((UINT_PTR)event.wParam );
	}
	__super::DoEvent(event);
}

void CResourceItemUI::EnableToolbar( bool bEnable )
{
	m_bEnableToolbar = bEnable;
	if (m_layToolBar)
	{
		m_layToolBar->SetVisible(bEnable);
	}
	
}

bool CResourceItemUI::OnItemEvent(void *param)
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

			m_ptDragEndPos.x = -1;
			m_ptDragEndPos.y = -1;

			CRect rtPPT;
			HWND hPPTWnd = CPPTController::GetPPTHwnd();
			GetWindowRect(hPPTWnd, &rtPPT);

		 
			if( pEvent->ptMouse.x < 0 &&  rt.PtInRect(ptMouse) && rtPPT.PtInRect(ptMouse) )
			{
				m_ptDragEndPos = ptMouse;
				OnItemDragFinish();
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
			int height = GetHeight() - m_lbName->GetHeight();

			if( m_pDragDialog == NULL )
			{
				HWND hWnd = AfxGetMainWnd()->GetSafeHwnd();

				POINT ptMouse;
				GetCursorPos(&ptMouse);

				//
				LPCTSTR strImage = m_lbImage->GetBkImage();
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

void CResourceItemUI::OnItemDragFinish()
{

}

void CResourceItemUI::NeedTypeIcon( bool bNeed )
{
	m_bNeedTypeIcon = bNeed;
}

void CResourceItemUI::SetIcon( LPCTSTR lptcsIcon )
{
	if (!m_bNeedTypeIcon)
	{
		return;
	}
	CControlUI* pCtrl = FindSubControl(_T("icon"));
	if (pCtrl)
	{
		pCtrl->SetBkImage(lptcsIcon);
	}
}

void CResourceItemUI::ReadStream( CStream* pStream )
{
	SetTitle(pStream->ReadString().c_str());
	SetResource(pStream->ReadString().c_str());
	SetGroup(_T("123"));
}

void CResourceItemUI::SetOnItemClickDelegate( CDelegateBase& OnItemClick )
{
	m_OnItemClick += OnItemClick;
}

bool CResourceItemUI::HasSubitems()
{
	return false;
}

void CResourceItemUI::EnableDrag( bool bEnable )
{
	m_bEnableDrag = bEnable;
}

void CResourceItemUI::SildableToolbar( bool bSildable )
{
	m_bSildableToolbar = bSildable;
}

bool CResourceItemUI::IsResourceExist()
{
	DWORD ret = GetFileAttributes(GetResource());
	if( ret == INVALID_FILE_ATTRIBUTES )
		return false;	
	return true;
}

void CResourceItemUI::UploadNetdisc(int currentModeType)
{
	
}

void CResourceItemUI::Delete(int currentModeType)
{

}

void CResourceItemUI::Rename( string strNewName )
{

}

void CResourceItemUI::EditExercises()
{

}

