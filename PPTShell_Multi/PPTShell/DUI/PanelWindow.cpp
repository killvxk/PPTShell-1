#include "StdAfx.h"
#include "PanelView.h"
#include "PanelWindow.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"


CPanelWindowUI::CPanelWindowUI(void)
{
	m_pPanelView = NULL;
}

CPanelWindowUI::~CPanelWindowUI(void)
{
}

DuiLib::CDuiString CPanelWindowUI::GetSkinFolder()
{
	return _T("skins");
}

DuiLib::CDuiString CPanelWindowUI::GetSkinFile()
{
	return  _T("ScreenTool\\PanelWindow.xml");
}

LPCTSTR CPanelWindowUI::GetWindowClassName( void ) const
{
	return _T("CPanelWindowUI");
}

CControlUI* CPanelWindowUI::CreateControl( LPCTSTR pstrClass )
{
	if (_tcsicmp(_T("PanelView"), pstrClass) == 0)
	{
		return new CPanelViewUI;
	}

	return NULL;
}

void CPanelWindowUI::OnFinalMessage( HWND hWnd )
{
	delete this;
}

void CPanelWindowUI::InitWindow()
{
	__super::InitWindow();
	m_pPanelView = (CPanelViewUI*)m_PaintManager.FindControl(_T("MainPanel"));
}


void CPanelWindowUI::ShowPanel( HWND hParent )
{
	HWND hWnd = GetHWND();
	if (!hWnd|| !IsWindow(hWnd))
	{
		Create(hParent, GetWindowClassName(), WS_POPUP | WS_VISIBLE, 0);

		MONITORINFO oMonitor = {0};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(this->GetHWND(), MONITOR_DEFAULTTONEAREST), &oMonitor);

		int nWidth	= oMonitor.rcMonitor.right - oMonitor.rcMonitor.left;
		int nHeight	= oMonitor.rcMonitor.bottom - oMonitor.rcMonitor.top;
		::MoveWindow(m_hWnd, 0,0, nWidth, nHeight, false);
	}
	::SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
}

void CPanelWindowUI::ClosePanel()
{
	HWND hWnd = GetHWND();
	if (!hWnd|| !IsWindow(hWnd))
	{
		return;
	}
	::ShowWindow(hWnd, SW_HIDE);
	::DestroyWindow(hWnd);

}

void CPanelWindowUI::ClearPanel()
{
	m_pPanelView->ClearShapes();
}

void CPanelWindowUI::SetPenColor( DWORD dwColor )
{
	dwColor  &= 0x00FFFFFF;
	if(dwColor == 0)
	{
		dwColor = 0x1;
	}

	dwColor = RGB(GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
	m_pPanelView->SetShapeColor(dwColor);
}

LRESULT CPanelWindowUI::OnKeyDown( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
 	if (wParam == VK_ESCAPE)
 	{
 		ClosePanel();
 	}
	return FALSE;
}

void CPanelWindowUI::SetPointerType( int nPointerType )
{
	m_pPanelView->SetPointer(nPointerType);
}
