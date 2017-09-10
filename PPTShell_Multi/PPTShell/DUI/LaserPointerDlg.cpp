#include "stdafx.h"
#include "LaserPointerDlg.h"
#include "EventCenter/EventDefine.h"


CLaserPointerUI::CLaserPointerUI()
{
	m_nImageWidth = 20;
	m_nImageHeight = 20;
}

CLaserPointerUI::~CLaserPointerUI()
{

}

LPCTSTR CLaserPointerUI::GetWindowClassName() const
{
	return _T("LaserPointerUI");
}

CDuiString CLaserPointerUI::GetSkinFile()
{
	return _T("LaserPointer\\LaserPointer.xml");
}

CDuiString CLaserPointerUI::GetSkinFolder()
{
	return CDuiString(_T("skins"));
}


void CLaserPointerUI::InitWindow()
{

}

CControlUI* CLaserPointerUI::CreateControl(LPCTSTR pstrClass)
{
	return NULL;
}

void CLaserPointerUI::Show( int nPosX, int nPoxY )
{
//	HWND hwnd = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
	//::PostMessage(hwnd, WM);
	m_nScreenLeft = nPosX - (m_nImageWidth/2 );
	m_nScreenTop = nPoxY - (m_nImageHeight/2 );

	::SetWindowPos(GetHWND(), HWND_TOPMOST, m_nScreenLeft, m_nScreenTop, m_nImageWidth, m_nImageHeight,  SWP_SHOWWINDOW | SWP_NOACTIVATE);
}

void CLaserPointerUI::Hide()
{
	ShowWindow(false);
}
