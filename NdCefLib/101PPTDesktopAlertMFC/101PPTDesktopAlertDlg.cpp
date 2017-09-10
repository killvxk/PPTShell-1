
// 101PPTDesktopAlertDlg.cpp : implementation file
//

#include "stdafx.h"
#include "101PPTDesktopAlert.h"
#include "101PPTDesktopAlertDlg.h"
#include "AlertWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy101PPTDesktopAlertDlg dialog




CMy101PPTDesktopAlertDlg::CMy101PPTDesktopAlertDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMy101PPTDesktopAlertDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_ptPopup = CPoint (-1, -1);
}

void CMy101PPTDesktopAlertDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMy101PPTDesktopAlertDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CMy101PPTDesktopAlertDlg::OnBnClickedButton1)
	ON_REGISTERED_MESSAGE(AFX_WM_ON_CLOSEPOPUPWINDOW, OnClosePopup)
END_MESSAGE_MAP()


// CMy101PPTDesktopAlertDlg message handlers

BOOL CMy101PPTDesktopAlertDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMy101PPTDesktopAlertDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMy101PPTDesktopAlertDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMy101PPTDesktopAlertDlg::OnBnClickedButton1()
{
	CMFCDesktopAlertWnd* pPopup = new CMFCDesktopAlertWnd;

	BYTE nTransparency = 255;
	pPopup->SetAnimationType (CMFCPopupMenu::SLIDE);
	pPopup->SetAnimationSpeed (30);
	pPopup->SetTransparency (nTransparency);
	pPopup->SetSmallCaption ();
	pPopup->SetAutoCloseTime (3000);


	CMFCDesktopAlertWndInfo params;

		params.m_hIcon =m_hIcon;
	params.m_strText =_T(" ");
	params.m_strURL =_T(" ");
	params.m_nURLCmdID = 101;

	pPopup->Create (this, IDD_ALERTDLG, NULL, 
		m_ptPopup, RUNTIME_CLASS (CAlertWnd));
	//pPopup->Create (this, params, NULL,m_ptPopup);
	pPopup->SetIcon(m_hIcon, FALSE);
	pPopup->SetWindowText (_T("Message"));
	// TODO: Add your control notification handler code here
}
LRESULT CMy101PPTDesktopAlertDlg::OnClosePopup(WPARAM,LPARAM lp)
{
	CMFCDesktopAlertWnd* pPopup = (CMFCDesktopAlertWnd*) lp;
	ASSERT_VALID (pPopup);

	m_ptPopup = pPopup->GetLastPos ();

	return 0;
}