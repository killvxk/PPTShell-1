#include "StdAfx.h"
#include "UntitledWindow.h"
#include "EventCenter/EventDefine.h"
#include "Util/Stream.h"

CUntitledWindow::CUntitledWindow(void)
{

}

CUntitledWindow::~CUntitledWindow(void)
{
}


LPCTSTR CUntitledWindow::GetWindowClassName( void ) const
{
	return _T("UntitledWindow");
}

DuiLib::CDuiString CUntitledWindow::GetSkinFile()
{
	return _T("Untitled\\Untitled.xml");
}

DuiLib::CDuiString CUntitledWindow::GetSkinFolder()
{
	return _T("skins");
}

void CUntitledWindow::InitWindow()
{
	m_pBtnNew = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("NewBtn")));
}

void CUntitledWindow::OnOpenBtn(TNotifyUI& msg)
{
	BroadcastEvent(EVT_PPT_FILE_OPERATION, OPEN_FILE, 0, 0);
}

void CUntitledWindow::OnNewBtn(TNotifyUI& msg)
{
	SetNewButtonEnabled(false);
	BroadcastEvent(EVT_PPT_FILE_OPERATION, NEW_FILE, 0, 0);
	ShowWindow(false);
}

void CUntitledWindow::SetNewButtonEnabled( bool bEnabled )
{
	if (m_pBtnNew != NULL )
		m_pBtnNew->SetEnabled(bEnabled);
}
