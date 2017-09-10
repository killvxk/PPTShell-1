#include "StdAfx.h"
#include "ExternPanelManager.h"
#include "Resource.h"
#include "PanelView.h"
#include "PanelWindow.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"

CExternPanelManager::CExternPanelManager(void)
{
	m_hPenWnd		= NULL;
	m_pPanelWnd		= NULL;

}

CExternPanelManager::~CExternPanelManager(void)
{
}


void CExternPanelManager::ShowPanel(DWORD dwColor )
{
 	CPanelWindowUI*	wndScreenPen = NULL;
 	if (!m_hPenWnd || !::IsWindow(m_hPenWnd))
 	{
 		wndScreenPen = new CPanelWindowUI;
		wndScreenPen->ShowPanel(CPPTController::GetSlideShowViewHwnd());
 
 		m_hPenWnd	= wndScreenPen->GetHWND();
		m_pPanelWnd = (LPVOID)wndScreenPen;
 	}
 	else
 	{
		::SetWindowPos(m_hPenWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
 		wndScreenPen = (CPanelWindowUI*)m_pPanelWnd;
 	}
 
 	if(wndScreenPen)
 	{
		wndScreenPen->SetPointerType(ePoiner_Pen);
 		wndScreenPen->SetPenColor(dwColor);
 	}
}


void CExternPanelManager::SetPointerType( int nPointerType )
{
	CPanelWindowUI*	wndScreenPen = NULL;
	if (!m_hPenWnd || !::IsWindow(m_hPenWnd))
	{
		return;
	}
	else
	{
		wndScreenPen = (CPanelWindowUI*)m_pPanelWnd;
		::SetWindowPos(m_hPenWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

	if(wndScreenPen)
	{
		wndScreenPen->SetPointerType(nPointerType);
	}
}

void CExternPanelManager::ClosePanel()
{
	if(!m_hPenWnd)
	{
		return;
	}

	if (::IsWindow(m_hPenWnd))
	{
		::ShowWindow(m_hPenWnd, SW_HIDE);
		::PostMessage(m_hPenWnd, WM_CLOSE, 0, 0);
	}

	m_hPenWnd = NULL;
}	

void CExternPanelManager::ClearPanel()
{
	CPanelWindowUI*	wndScreenPen = NULL;
	if (!m_hPenWnd || !::IsWindow(m_hPenWnd))
	{
		return;
	}
	else
	{
		wndScreenPen = (CPanelWindowUI*)m_pPanelWnd;
		::SetWindowPos(m_hPenWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

	if(wndScreenPen)
	{
		wndScreenPen->ClearPanel();
	}
}
