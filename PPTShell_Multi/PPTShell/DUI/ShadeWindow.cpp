#include "StdAfx.h"
#include "ShadeWindow.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"

CShadeWindow::CShadeWindow(void)
{
//	m_pWndShadow = NULL;
}

CShadeWindow::~CShadeWindow(void)
{
	//m_pInstance = NULL;
}

CShadeWindow* CShadeWindow::m_pInstance = NULL;

LPCTSTR CShadeWindow::GetWindowClassName( void ) const
{
	return _T("ShadeWindow");
}

DuiLib::CDuiString CShadeWindow::GetSkinFile()
{
	return _T("ShadeWindow\\ShadeWindow.xml");
}

DuiLib::CDuiString CShadeWindow::GetSkinFolder()
{
	return _T("skins");
}

void CShadeWindow::InitWindow()
{
	m_pLabel	= dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(_T("loading_tip")));
	m_pAnimation= dynamic_cast<CGifAnimUI*>(m_PaintManager.FindControl(_T("loading")));

	__super::InitWindow();
}

CControlUI* CShadeWindow::CreateControl(LPCTSTR pstrClass)
{
	if( _tcscmp(pstrClass, _T("GifAnim")) == 0 )
		return new CGifAnimUI;
	return __super::CreateControl(pstrClass);
}

void CShadeWindow::OnFinalMessage( HWND hWnd )
{
	if ( m_pInstance != NULL)
		delete m_pInstance;

	m_pInstance = NULL;
	//delete this;
}


CShadeWindow* CShadeWindow::GetInstance(HWND hParent)
{
	if ( m_pInstance == NULL)
	{
		m_pInstance = new CShadeWindow;
		m_pInstance->m_hParent	= hParent;
		m_pInstance->Create(m_pInstance->m_hParent, _T("ShadeWindow"), WS_POPUP , 0 ,0 ,0, 0, 0);

	}

	if (m_pInstance == NULL)
		return NULL;

	return m_pInstance;
}

void CShadeWindow::Show( tstring strTip, bool bLoading )
{
	if ( CShadeWindow::GetInstance(AfxGetApp()->m_pMainWnd->m_hWnd) != NULL )
		CShadeWindow::GetInstance(AfxGetApp()->m_pMainWnd->m_hWnd)->ShadeShow(strTip, bLoading);
}

void CShadeWindow::Show( HWND hParent, tstring strTip, bool bLoading )
{
	if ( CShadeWindow::GetInstance(hParent) != NULL )
		CShadeWindow::GetInstance(hParent)->ShadeShow(strTip, bLoading);
}

void CShadeWindow::Hide()
{
	if ( CShadeWindow::GetInstance(AfxGetApp()->m_pMainWnd->m_hWnd) != NULL )
		CShadeWindow::GetInstance(AfxGetApp()->m_pMainWnd->m_hWnd)->ShadeHide();
}

void CShadeWindow::ShadeShow( tstring strTip, bool bLoading )
{
	if ( strTip.empty() )
	{
		MoveWindow(GetHWND(), 0, 0, 0, 0, TRUE);
		m_PaintManager.SetTransparent(255);
		ShowWindow(true);
	}
	
	if ( m_pLabel != NULL )
	{
		m_pLabel->SetText(strTip.c_str());

		RECT rcCalc		= {0};
		rcCalc.right	= 600;
		rcCalc.top		= 60;
		CalcText(m_PaintManager.GetPaintDC(), rcCalc, strTip.c_str(), m_pLabel->GetFont(), DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT|DT_NOPREFIX, UIFONT_GDI);

		RECT rcPadding = m_pLabel->GetPadding();

		m_pLabel->SetFixedWidth(rcCalc.right - rcCalc.left);
		m_pLabel->SetFixedHeight(rcCalc.bottom - rcCalc.top);

 	//	CRect mainRect;
 	//	::GetWindowRect(m_hParent, mainRect);

		//ShowWindow(true);
 
 	//	int nLeft	= mainRect.left + (mainRect.Width() - m_pLabel->GetMaxWidth() - rcPadding.left - rcPadding.right)/2;
 	//	int nTop	= mainRect.top + (mainRect.Height() - m_pLabel->GetMaxHeight() - rcPadding.top - rcPadding.bottom)/2;
 	//	int nWidth	= rcCalc.right - rcCalc.left + (rcCalc.left + rcPadding.left + rcPadding.right);
 	//	int nHight	= rcCalc.bottom - rcCalc.top + (rcPadding.top + rcPadding.bottom);
 
 	//	if (bLoading && m_pAnimation != NULL ) // gif show
 	//	{
 	//		int nAnimationWidth = m_pAnimation->GetFixedWidth() + m_pAnimation->GetPadding().left + m_pAnimation->GetPadding().right;
 
 	//		nLeft -= nAnimationWidth/2;
 	//		nWidth += nAnimationWidth;
 	//	}
	//	::SetWindowPos(this->GetHWND(), HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
 		
	}
	if ( m_pAnimation != NULL )
	{
		if ( bLoading )
			m_pAnimation->PlayGif();
		else
			m_pAnimation->SetVisible(false);
	}
	
//	CenterWindow();


	CRect rect;
	HWND hwnd = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
	::GetWindowRect(hwnd, &rect);
	MoveWindow(GetHWND(), rect.left, rect.top, rect.Width(), rect.Height(), TRUE);

	ShowWindow(true);
// 	if ( m_pWndShadow == NULL )
// 	{
// 		m_pWndShadow = new CWndShadow;
// 		RECT rtMain;
// 		GetWindowRect(m_hParent, &rtMain);
// 		m_pWndShadow->SetMaskSize(rtMain);
// 		m_pWndShadow->Create(m_hWnd, true);
// 	}
//  	else
//  	{
//  		RECT rtMain;
//  		GetWindowRect(m_hParent, &rtMain);
//  		m_pWndShadow->SetMaskSize(rtMain);
// 		::ShowWindow(m_pWndShadow->m_hWnd, SW_SHOW);
//  	}

	
}

void  CShadeWindow::CalcText( HDC hdc, RECT& rc, LPCTSTR lpszText, int nFontId, UINT format, UITYPE_FONT nFontType, int c)
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

void CShadeWindow::ShadeHide()
{
	//ShowWindow(false);
	Close();
}


//================================================================

void ShadeWindowShow( tstring strTip, bool bLoading )
{
	CShadeWindow::Show(strTip, bLoading);
}

void ShadeWindowShow( HWND hParent, tstring strTip, bool bLoading )
{
	CShadeWindow::Show(hParent, strTip, bLoading);
}

void ShadeWindowHide()
{
	CShadeWindow::Hide();
}

