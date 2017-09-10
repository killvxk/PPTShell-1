// Update.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Update.h"
#include "UpdateDlg.h"
#include "CoverDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUpdateApp

BEGIN_MESSAGE_MAP(CUpdateApp, CWinApp)
	//{{AFX_MSG_MAP(CUpdateApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUpdateApp construction

CUpdateApp::CUpdateApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CUpdateApp object

CUpdateApp theApp;
WCHAR g_FilePath[MAX_PATH + 1] = {0};
WCHAR g_szNdCloudPath[MAX_PATH + 1] = {0};
/////////////////////////////////////////////////////////////////////////////
// CUpdateApp initialization

ULONG_PTR gdiplusToken;
BOOL CUpdateApp::InitInstance()
{
	// Standard initialization

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	LPCWSTR  lpCommand;
	lpCommand = GetCommandLineW();
	int iCommandNum = 0;
	LPWSTR *CommandArray = CommandLineToArgvW(lpCommand, &iCommandNum);

	CString strCommand = _T("");
	if ( iCommandNum == 2)
	{
		strCommand = CommandArray[1];
	}
	else if ( iCommandNum == 3)
	{
		strCommand = CommandArray[1];
		wcscpy_s(g_FilePath, MAX_PATH, CommandArray[2]);
	}
	else if ( iCommandNum == 4)
	{
		strCommand = CommandArray[1];
		wcscpy_s(g_FilePath, MAX_PATH, CommandArray[2]);
		wcscpy_s(g_szNdCloudPath, MAX_PATH, CommandArray[3]);
		if(wcslen(g_szNdCloudPath) > 0 && g_szNdCloudPath[wcslen(g_szNdCloudPath) - 1] != L'\\')
		{
			wcscat_s(g_szNdCloudPath, MAX_PATH, L"\\");
		}
	}

	if(strCommand == _T("cover"))
	{
		CCoverDlg dlg;
		m_pMainWnd = &dlg;
		int nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
		}
		else if (nResponse == IDCANCEL)
		{
		}
	}
	else
	{
		CUpdateDlg dlg;
		m_pMainWnd = &dlg;
		if(strCommand == _T("selfupdate"))
			dlg.SetUpdateType(true);
		int nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
		}
		else if (nResponse == IDCANCEL)
		{
		}
	}


	

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
