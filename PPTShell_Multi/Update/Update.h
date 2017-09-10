// Update.h : main header file for the UPDATE application
//

#if !defined(AFX_UPDATE_H__E2D8A213_D602_43D9_A99F_5DF454B52E8A__INCLUDED_)
#define AFX_UPDATE_H__E2D8A213_D602_43D9_A99F_5DF454B52E8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CUpdateApp:
// See Update.cpp for the implementation of this class
//

class CUpdateApp : public CWinApp
{
public:
	CUpdateApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUpdateApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CUpdateApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UPDATE_H__E2D8A213_D602_43D9_A99F_5DF454B52E8A__INCLUDED_)

extern WCHAR g_FilePath[MAX_PATH + 1];
extern WCHAR g_szNdCloudPath[MAX_PATH + 1];
