
// 101PPTDesktopAlert.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMy101PPTDesktopAlertApp:
// See 101PPTDesktopAlert.cpp for the implementation of this class
//

class CMy101PPTDesktopAlertApp : public CWinAppEx
{
public:
	CMy101PPTDesktopAlertApp();

// Overrides
	public:
	ULONG_PTR m_gdiplusToken;
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CMy101PPTDesktopAlertApp theApp;