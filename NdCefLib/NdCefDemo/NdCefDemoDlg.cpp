
// CefDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NdCefDemo.h"
#include "NdCefDemoDlg.h"
#include <iostream>

#define MSG_URLCHANGE  30001	

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
HINSTANCE g_dllhin;
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CNdCefDemoDlg dialog




CNdCefDemoDlg::CNdCefDemoDlg(CWnd* pParent /*=NULL*/)
: CDialog(CNdCefDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNdCefDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CEF1, cefstatic1);
	DDX_Control(pDX, IDC_CEF2, cefstatic2);
}

BEGIN_MESSAGE_MAP(CNdCefDemoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CNdCefDemoDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CNdCefDemoDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CNdCefDemoDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CNdCefDemoDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON6, &CNdCefDemoDlg::OnBnClickedButton6)
//	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, &CNdCefDemoDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON5, &CNdCefDemoDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON7, &CNdCefDemoDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CNdCefDemoDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CNdCefDemoDlg::OnBnClickedButton9)
	ON_WM_CLOSE()
	ON_WM_COPYDATA()
	ON_BN_CLICKED(IDC_BUTTON10, &CNdCefDemoDlg::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON11, &CNdCefDemoDlg::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON12, &CNdCefDemoDlg::OnBnClickedButton12)
END_MESSAGE_MAP()


// CNdCefDemoDlg message handlers

BOOL CNdCefDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CNdCefDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNdCefDemoDlg::OnPaint()
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
HCURSOR CNdCefDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

typedef int(*NdCefSumFun)(int,int); 
typedef bool(*NdCefInitFun)(); 
typedef bool(*NdCefShowFun)(HWND); 
typedef bool(*NdCefCloseByWndFun)(HWND); 
typedef bool(*NdCefShowbyUrlFun)(HWND,LPCTSTR nUrl); 
typedef bool(*NdCefChangeUrlFun)(HWND,LPCTSTR nUrl); 
typedef bool(*NdCefExecuteJSFun)(HWND,LPCTSTR nJsCode); 
typedef bool(*NdCefClearCookieFun)(LPCTSTR nUrl,LPCTSTR nCookieName);
typedef bool(*NdCefExecuteJSCallBackFun)(HWND,LPCTSTR nJsCode,LPCTSTR nParam); 
typedef void(*NdCefUnInitFun)();
typedef bool(*NdCefUnZipFun)(LPCSTR filezip,LPCSTR filepath);
typedef bool(*NdCefZipFun)(LPCSTR filezip,LPCSTR filepath);

void CNdCefDemoDlg::OnBnClickedButton1()
{
	OutputDebugString(L"OnBnClickedButton1\n"); 
	// TODO: Add your control notification handler code here
	g_dllhin = LoadLibrary(L"NdCefLib.dll"); 
	if (g_dllhin != NULL) 
	{ 

		NdCefSumFun fun = (NdCefSumFun )GetProcAddress(g_dllhin,"NdCefSum"); 

		if (fun != NULL) 
		{ 
			int sum = fun(1,3); 
			OutputDebugString(L"Sum\n"); 
		} 
		//FreeLibrary(g_dllhin); 
	}
}

void CNdCefDemoDlg::OnBnClickedButton2()
{
	OutputDebugString(L"OnBnClickedButton2\n"); 
	if (g_dllhin) {
		NdCefUnInitFun fun = (NdCefUnInitFun )GetProcAddress(g_dllhin,"NdCefUnInit"); 

		if (fun != NULL) 
		{ 
			fun(); 
			
			OutputDebugString(L"NdCefUnInitFun\n"); 
		} 

		FreeLibrary(g_dllhin);
		g_dllhin = NULL;
		OutputDebugString(L"FreeLibrary\n"); 
	}
	// TODO: Add your control notification handler code here
}

void CNdCefDemoDlg::OnBnClickedButton3()
{
	if (g_dllhin != NULL) 
	{ 
		NdCefInitFun fun = (NdCefInitFun )GetProcAddress(g_dllhin,"NdCefInit"); 

		if (fun != NULL) 
		{ 
			bool initSuccess = fun(); 
			if (initSuccess){
				OutputDebugString(L"NdCefInitFun OK\n"); 
			}else{
				OutputDebugString(L"NdCefInitFun Error\n");
			}
		} 
	}
	// TODO: Add your control notification handler code here
}

void CNdCefDemoDlg::OnBnClickedButton4()
{
	if (g_dllhin != NULL) 
	{ 
		NdCefShowFun NdCefShow = (NdCefShowFun )GetProcAddress(g_dllhin,"NdCefShow"); 
		if (NdCefShow != NULL) 
		{ 
			bool nShow = NdCefShow(cefstatic1.GetSafeHwnd()); 
			if (nShow){
				OutputDebugString(L"NdCefShowFun OK\n"); 
			}else{
				OutputDebugString(L"NdCefShowFun Error\n");
			}
		}
	} 
}

void CNdCefDemoDlg::OnBnClickedButton6()
{
	// TODO: Add your control notification handler code here
	if (g_dllhin != NULL) 
	{ 
		NdCefShowbyUrlFun NdCefShow = (NdCefShowbyUrlFun )GetProcAddress(g_dllhin,"NdCefShowbyUrl"); 
		if (NdCefShow != NULL) 
		{ 
			bool nShow = NdCefShow(cefstatic2.GetSafeHwnd(),L"https://openapi.99.com/Oauth2/Authorize?response_type=code&client_id=2852836&redirect_uri=http%3A%2F%2Fesp-sso.edu.web.sdp.101.com%2Fredirect_third%3Fsso_host%3Desp-sso.edu.web.sdp.101.com%26type%3Dnd99%26fromway%3Dhttp%25253A%25252F%25252Fp.101.com"); 
			if (nShow){
				OutputDebugString(L"NdCefShowFun OK\n"); 
			}else{
				OutputDebugString(L"NdCefShowFun Error\n");
			}
		}
	} 
}
//void CNdCefDemoDlg::OnClose()
//{
//	// TODO: Add your message handler code here and/or call default
//	OnBnClickedButton2();
//	CDialog::OnClose();
//}

void CNdCefDemoDlg::OnBnClickedOk()
{
	OnBnClickedButton2();
	// TODO: Add your control notification handler code here
	OnOK();
}

void CNdCefDemoDlg::OnBnClickedButton5()
{
	if (g_dllhin != NULL) 
	{ 
		/*
		NdCefExecuteJSFun NdCefExecuteJS = (NdCefExecuteJSFun )GetProcAddress(g_dllhin,"NdCefExecuteJS"); 
		if (NdCefExecuteJS != NULL) 
		{ 
			bool nShow = NdCefExecuteJS(cefstatic1.GetSafeHwnd(),L"Myalert(\"nihao,ÄãºÃ£¡\")"); 
			if (nShow){
				OutputDebugString(L"NdCefExecuteJS OK\n"); 
			}else{
				OutputDebugString(L"NdCefExecuteJS Error\n");
			}
		}
		*/
		NdCefExecuteJSCallBackFun NdCefExecuteJSCallBack = (NdCefExecuteJSCallBackFun )GetProcAddress(g_dllhin,"NdCefExecuteJSCallBack"); 
		if (NdCefExecuteJSCallBack != NULL) 
		{ 
			bool nShow = NdCefExecuteJSCallBack(cefstatic1.GetSafeHwnd(),L"Myalert",L"\"sfsddsf\""); 
			if (nShow){
				OutputDebugString(L"NdCefExecuteJSCallBack OK\n"); 
			}else{
				OutputDebugString(L"NdCefExecuteJSCallBack Error\n");
			}
		}
	} 
	// TODO: Add your control notification handler code here
}

void CNdCefDemoDlg::OnBnClickedButton7()
{

	// TODO: Add your control notification handler code here
}

 int urlindex = 0;
 wstring url1 = L"www.baidu.com";
  wstring url2 = L"www.163.com";
void CNdCefDemoDlg::OnBnClickedButton8()
{
	if (g_dllhin != NULL) 
	{ 
		NdCefChangeUrlFun NdCefChangeUrl = (NdCefChangeUrlFun )GetProcAddress(g_dllhin,"NdCefChangeUrl"); 
		if (NdCefChangeUrl != NULL) 
		{ 
			urlindex++;
			wstring url =url2;
			if (urlindex%2==0){
				url = url1;
			}
			bool nShow = NdCefChangeUrl(cefstatic2.GetSafeHwnd(),url.c_str()); 
			if (nShow){
				OutputDebugString(L"NdCefShowFun OK\n"); 
			}else{
				OutputDebugString(L"NdCefShowFun Error\n");
			}
		}
	} 
	// TODO: Add your control notification handler code here
}

void CNdCefDemoDlg::OnBnClickedButton9()
{
	// TODO: Add your control notification handler code here
	
		if (g_dllhin != NULL) 
		{ 
			NdCefCloseByWndFun NdCefCloseByWnd = (NdCefCloseByWndFun )GetProcAddress(g_dllhin,"NdCefCloseByWnd"); 
			if (NdCefCloseByWnd != NULL) 
			{ 
				bool nClose = NdCefCloseByWnd(cefstatic2.GetSafeHwnd()); 
				if (nClose){
					OutputDebugString(L"NdCefShowFun OK\n"); 
				}else{
					OutputDebugString(L"NdCefShowFun Error\n");
				}
			}
		} 
}

void CNdCefDemoDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnClose();
}

BOOL CNdCefDemoDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	// TODO: Add your message handler code here and/or call default
	switch (pCopyDataStruct->dwData)
	{ 	
	case MSG_URLCHANGE:
		std::wstring nUrlInfo = (LPCTSTR) pCopyDataStruct->lpData;
		break;
	}
	return CDialog::OnCopyData(pWnd, pCopyDataStruct);
}

void CNdCefDemoDlg::OnBnClickedButton10()
{
	if (g_dllhin != NULL) 
	{ 
		NdCefClearCookieFun NdCefClearCookie = (NdCefClearCookieFun )GetProcAddress(g_dllhin,"NdCefClearCookie"); 
		if (NdCefClearCookie != NULL) 
		{ 
			bool nClose = NdCefClearCookie(L"",L""); 
			if (nClose){
				OutputDebugString(L"NdCefClearCookie OK\n"); 
			}else{
				OutputDebugString(L"NdCefClearCookie Error\n");
			}
		}
	} 
	// TODO: Add your control notification handler code here
}

void CNdCefDemoDlg::OnBnClickedButton11()
{
	// TODO: Add your control notification handler code here
	if (g_dllhin) {
		NdCefZipFun pFnNdCefZip = (NdCefZipFun)GetProcAddress(g_dllhin,"NdCefZip"); 

		if (pFnNdCefZip != NULL) 
		{ 
			 pFnNdCefZip("d:/1.zip","d:/1/"); 
		}
	}
}

void CNdCefDemoDlg::OnBnClickedButton12()
{
	if (g_dllhin) {
		NdCefUnZipFun pFnNdCefUnZip = (NdCefUnZipFun)GetProcAddress(g_dllhin,"NdCefUnZip"); 

		if (pFnNdCefUnZip != NULL) 
		{ 
			 pFnNdCefUnZip("d:/nodejs.zip","d:/2"); 
		}
	}
	// TODO: Add your control notification handler code here
}
