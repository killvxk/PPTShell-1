// MediaPlayerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MediaPlayer.h"
#include "MediaPlayerDlg.h"
#include <cstdlib>
#include <afx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
bool flag=false;
int count =0;
int IndexInFieldList;
int spaceflag=1;

/////////////////////////////////////////////////////////////////////////////
// CMediaPlayerDlg dialog

CMediaPlayerDlg::CMediaPlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMediaPlayerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMediaPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMediaPlayerDlg)
	DDX_Control(pDX, IDC_OCX1, m_player);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMediaPlayerDlg, CDialog)
	//{{AFX_MSG_MAP(CMediaPlayerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMediaPlayerDlg message handlers

BOOL CMediaPlayerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_control=static_cast<CWMPControls>(m_player.GetControls());
	m_setting=m_player.GetSettings();
	m_media=static_cast<CWMPMedia>(m_player.GetCurrentMedia());
	m_setting.SetAutoStart(TRUE);
	m_setting.SetPlayCount(10000);
	m_player.SetUrl(m_strFileName);
	SetWindowText(_T("VRMediaPlayer"));
	SetWindowPos(NULL,0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN),0);
	CRect rect;
	GetClientRect(rect);
	m_player.MoveWindow(rect);
	//SetWindowPos(NULL,0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN),0);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMediaPlayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMediaPlayerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
	
}

void CMediaPlayerDlg::OnFullscreen() 
{
	// TODO: Add your control notification handler code here
	m_control.pause();
	m_player.SetFullScreen(true);
	m_control.play();
}

BOOL CMediaPlayerDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch(pMsg->message){
		case WM_LBUTTONDBLCLK:
				OnFullscreen();
				return TRUE;
				break;
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDBLCLK:
		case WM_MOUSEWHEEL:
		case WM_KEYDOWN:
			return TRUE;
			break;
		case WM_VRMEDIAPLAYCLOSE:
			OnCancel();
			break;
		case WM_TIMER:
			KillTimer(1);
			SendMessage(WM_LBUTTONDBLCLK,0,0);
			break;
		case WM_CLOSE:
			//OnCancel();
			SetTimer(2000,1,NULL);
			break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CMediaPlayerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CRect rect;
	GetClientRect(rect);
	if (::IsWindow(m_player.GetSafeHwnd())){
		m_player.MoveWindow(rect);
	}
	// TODO: Add your message handler code here
}
