// AlertWnd.cpp : implementation file
//

#include "stdafx.h"
#include "101PPTDesktopAlert.h"
#include "AlertWnd.h"


// CAlertWnd dialog

IMPLEMENT_DYNCREATE(CAlertWnd, CMFCDesktopAlertDialog)

CAlertWnd::CAlertWnd()
{

}

CAlertWnd::~CAlertWnd()
{
}

void CAlertWnd::DoDataExchange(CDataExchange* pDX)
{
	CMFCDesktopAlertDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAlertWnd, CMFCDesktopAlertDialog)
END_MESSAGE_MAP()


// CAlertWnd message handlers

BOOL CAlertWnd::OnInitDialog() 
{
	CMFCDesktopAlertDialog::OnInitDialog();
	m_cClose.Create(0, 0, this, 1234, IDB_CLOSE_PRESS, IDB_CLOSE_HOVER, IDB_CLOSE_PRESS);
	return TRUE;
}

bool ImageFromIDResource(UINT nID, LPCTSTR sTR,Image * &pImg);

void CAlertWnd::OnDraw (CDC* pDC)
{
	CMFCDesktopAlertDialog::OnDraw (pDC);
	Gdiplus::Image* m_bg;
	//ImageFromIDResource(IDB_BG,_T("PNG"),m_bg);
	Graphics g(pDC->GetSafeHdc());      
	//g.DrawImage(m_bg,0,0);    
	g.ReleaseHDC(pDC->GetSafeHdc());  
}