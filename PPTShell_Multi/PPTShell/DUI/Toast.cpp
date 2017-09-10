#include "StdAfx.h"
#include "Toast.h"

#define  TAOST_DEAFULT_OPACITY	(0)
#define  TAOST_TOTAL_OPACITY	(160)
#define	 OPACITY_STEP			(4)

CToast* CToast::m_pInstance = NULL;

CToast::CToast(void)
{
	InitializeCriticalSection(&m_Lock);

	m_lstMsg.clear();
	m_dwStartTime	= 0;
	m_dwEndTime		= 0;
	m_nOpacity		= TAOST_DEAFULT_OPACITY;
	m_nCurStatus	= TOAST_NONE;
	m_nLastStatus	= TOAST_NONE;
	m_hParent		= NULL;
	m_bShow			= TRUE;

	memset(m_MainRect, 0 , sizeof(CRect));
}

CToast::~CToast(void)
{
	EnterCriticalSection(&m_Lock);
	m_lstMsg.clear();
	LeaveCriticalSection(&m_Lock);

	DeleteCriticalSection(&m_Lock);
	KillTimer(this->GetHWND(), TOAST_TIMER_ID);
}

LPCTSTR CToast::GetWindowClassName() const
{
	return _T("Toast");
}

CDuiString CToast::GetSkinFile()
{
	return _T("Toast\\Toast.xml");
}

CDuiString CToast::GetSkinFolder()
{
	return CDuiString(_T("skins"));
}

void CToast::InitWindow()
{
	m_lblContent = dynamic_cast<CTextUI*>(m_PaintManager.FindControl(_T("toastContent")));
	m_layoutToast = dynamic_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("toastLayout")));

	__super::InitWindow();
}

void CToast::Toast(tstring strMsg, bool bFront, int nTime)
{
	CToast::GetInstance(AfxGetApp()->m_pMainWnd->m_hWnd)->Show(strMsg, bFront, nTime);
}


void CToast::Toast(HWND hParent, tstring strMsg, bool bFront, int nTime)
{
	CToast::GetInstance(AfxGetApp()->m_pMainWnd->m_hWnd)->Show(strMsg, bFront, nTime);
}

void CToast::SetShow(BOOL bShow)
{
	CToast::GetInstance(NULL)->m_bShow = bShow;

	if ( !bShow )
	{
		CToast::GetInstance(NULL)->Reset();
	}
}

CToast* CToast::GetInstance(HWND hParent)
{
	if ( m_pInstance == NULL)
	{
		m_pInstance = new CToast;
		m_pInstance->m_hParent	= hParent;

		m_pInstance->Create(hParent, _T("Toast"), WS_OVERLAPPEDWINDOW, 0 ,0 ,0, 0, 0);
	}

	if (m_pInstance == NULL)
		return NULL;

	return m_pInstance;
}

bool CToast::Show( tstring strMsg, bool bFront /*= false*/, int nTime /*= 2000*/ )
{
	TOAST_INFO toast;
	toast.strMsg	= strMsg;
	toast.dwTime	= nTime;

	if ( !m_bShow )
		return false;

	bool bNeedPush = false;
	tstring strDisplayText = m_lblContent->GetText();

	EnterCriticalSection(&m_Lock);
	if ( bFront )
	{
		if (m_lstMsg.empty())
		{
			if (strDisplayText != strMsg)
				bNeedPush = true;
		}
		else
		{
			const TOAST_INFO& infoExist = m_lstMsg.front();
			if (infoExist.strMsg != strMsg)
				bNeedPush = true;
		}
		
		if (bNeedPush)
			m_lstMsg.push_front(toast);
	}
	else
	{
		if (m_lstMsg.empty())
		{
			if (strDisplayText != strMsg)
				bNeedPush = true;
		}
		else
		{
			const TOAST_INFO& infoExist = m_lstMsg.back();
			if (infoExist.strMsg != strMsg)
				bNeedPush = true;
		}

		if (bNeedPush)
			m_lstMsg.push_back(toast);
	}
	LeaveCriticalSection(&m_Lock);

	if (!bNeedPush)
		return true;
	 
	if ( m_nCurStatus == TOAST_NONE || m_nCurStatus == TOAST_END )
	{
		SetTimer(this->GetHWND(), TOAST_TIMER_ID, 10, (TIMERPROC)&CToast::OnTimer);

		m_nLastStatus	= m_nCurStatus;
		m_nCurStatus	= TOAST_BEGIN;

		TOAST_INFO toast = m_lstMsg.front();
		SetToastMsg(toast.strMsg);
	}
	
	return true;
}

void CToast::OnTimer( HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime)
{
	if ( idEvent == TOAST_TIMER_ID )
	{
		m_pInstance->ShowToast();
	}
}

void CToast::ShowToast()
{
	m_dwStartTime = GetTickCount();

	CRect rt;
	::GetWindowRect(m_hParent , rt);

	CRect rtToast;
	::GetWindowRect(this->GetHWND(), rtToast);

	if ( rt.left != m_MainRect.left )
	{
		m_MainRect  = rt;
		::MoveWindow(this->GetHWND(),
			rt.left + (rt.Width() - rtToast.Width())/2,
			rt.top + (rt.Height() - rtToast.Height())/2,
			rtToast.Width(),
			rtToast.Height(),
			true);
	}

	if ( m_nCurStatus == TOAST_BEGIN )
	{
		bool bParentVisible = IsWindowVisible(m_hParent);
		ShowWindow(bParentVisible);
		m_PaintManager.SetTransparent(m_nOpacity);
		m_nOpacity += OPACITY_STEP;

		if ( m_nOpacity >= TAOST_TOTAL_OPACITY )
			m_nCurStatus = TOAST_SHOW_MSG;
	}
	else if ( m_nCurStatus == TOAST_SHOW_MSG && (m_dwEndTime < m_dwStartTime) )
	{
		EnterCriticalSection(&m_Lock);
		if ( !m_lstMsg.empty() )
		{
			TOAST_INFO toast = m_lstMsg.front();
			m_lstMsg.pop_front();
			SetToastMsg(toast.strMsg);
			m_dwEndTime = m_dwStartTime + toast.dwTime;

			if ( m_nLastStatus == TOAST_SHOW_MSG )
			{
				m_nCurStatus	= TOAST_BEGIN;
				m_nOpacity		= TAOST_DEAFULT_OPACITY;
				m_dwEndTime		+= TAOST_TOTAL_OPACITY/OPACITY_STEP;
			}

			m_nLastStatus = TOAST_SHOW_MSG;
		}
		else
		{
			m_nCurStatus = TOAST_END;
		}
		LeaveCriticalSection(&m_Lock);
	}
	else if ( m_nCurStatus == TOAST_END )
	{
		m_PaintManager.SetTransparent(m_nOpacity);
		m_nOpacity -= OPACITY_STEP;

		if ( m_nOpacity <= TAOST_DEAFULT_OPACITY )
		{
			m_nCurStatus = TOAST_NONE;
			KillTimer(this->GetHWND(), TOAST_TIMER_ID);
			m_lblContent->SetText(_T(""));
			ShowWindow(false);
		}
	}

}

void CToast::SetToastMsg(tstring strMsg)
{
	RECT rcCalc = {0};
	int nContainerWidth = 600;
	rcCalc.right = nContainerWidth;

	m_lblContent->SetText(strMsg.c_str());

	this->CalcText(m_PaintManager.GetPaintDC(), rcCalc, strMsg.c_str(), m_lblContent->GetFont(), DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT | DT_NOPREFIX, UIFONT_GDI);

	RECT rcPadding = m_lblContent->GetPadding();

	m_lblContent->SetMaxWidth(rcCalc.right - rcCalc.left);
	m_lblContent->SetMaxHeight(rcCalc.bottom - rcCalc.top);
	
	::GetWindowRect(m_hParent, m_MainRect);

	//::MoveWindow(this->GetHWND(),
	//	m_MainRect.left + (m_MainRect.Width() - m_lblContent->GetFixedWidth())/2,
	//	m_MainRect.top + (m_MainRect.Height() - m_lblContent->GetFixedHeight())/2,
	//	rcCalc.right - rcCalc.left + (rcCalc.left + rcPadding.left + rcPadding.right), 
	//	rcCalc.bottom - rcCalc.top + (rcPadding.top + rcPadding.bottom),
	//	true);
	ShowWindow(true);
	::SetWindowPos(this->GetHWND(), HWND_TOPMOST,
		m_MainRect.left + (m_MainRect.Width() - m_lblContent->GetMaxWidth() - rcPadding.left - rcPadding.right)/2,
		m_MainRect.top + (m_MainRect.Height() - m_lblContent->GetMaxHeight() - rcPadding.top - rcPadding.bottom)/2,
		rcCalc.right - rcCalc.left + (rcCalc.left + rcPadding.left + rcPadding.right), 
		rcCalc.bottom - rcCalc.top + (rcPadding.top + rcPadding.bottom),
		SWP_SHOWWINDOW);
	//::SetWindowPos(this->GetHWND(), HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
}


void  CToast::CalcText( HDC hdc, RECT& rc, LPCTSTR lpszText, int nFontId, UINT format, UITYPE_FONT nFontType, int c)
{
	if (nFontType == UIFONT_GDI)
	{
		HFONT hFont = m_PaintManager.GetFont(nFontId);
		HFONT hOldFont = (HFONT)::SelectObject(hdc, hFont);

		if ((DT_SINGLELINE & format))
		{
			SIZE size = {0};
			::GetTextExtentExPoint(hdc, lpszText, c == -1 ? _tcslen(lpszText) : c, 0, NULL, NULL, &size);
			rc.right = rc.left + size.cx;
			rc.bottom = rc.top + size.cy;
		}
		else
		{
			format &= ~DT_END_ELLIPSIS;
			format &= ~DT_PATH_ELLIPSIS;
			if (!(DT_SINGLELINE & format)) format |= DT_WORDBREAK | DT_EDITCONTROL;
			::DrawText(hdc, lpszText, c, &rc, format | DT_CALCRECT);
		}
		::SelectObject(hdc, hOldFont);
	}

}

void CToast::Reset()
{
	KillTimer(GetHWND(), TOAST_TIMER_ID);
	if (m_lblContent)
		m_lblContent->SetText(_T(""));
	EnterCriticalSection(&m_Lock);
	m_lstMsg.clear();
	LeaveCriticalSection(&m_Lock);
	m_nCurStatus	= TOAST_NONE;
	ShowWindow(false);
}

void CToast::OnFinalMessage( HWND hWnd )
{
	delete m_pInstance;
	m_pInstance = NULL;
}

LRESULT CToast::ResponseDefaultKeyEvent( WPARAM wParam )
{
	if ( wParam == VK_ESCAPE)
		return FALSE;
	return S_OK;
}
