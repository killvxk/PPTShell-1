
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#pragma warning(disable : 4099)

#define _ATL_APARTMENT_THREADED 
#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions




#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars


#include <afxsock.h>            // MFC socket extensions

#include <vector>
#include <list>
#include <map>
#include <string>
#include <deque>
#include <memory>
#include <sstream>

using namespace std;

#include <GdiPlus.h>
#pragma comment(lib,"GdiPlus.lib")
using namespace Gdiplus;

#include <ShellAPI.h>
#pragma comment(lib,"Shell32.lib")

#include <regex>
using namespace std::tr1;


#ifdef UNICODE
	typedef wstring	tstring;
	typedef wregex tregex;
	typedef wsmatch tsmatch;
#else
	typedef string	tstring;
	typedef regex tregex;
	typedef smatch tsmatch;
#endif

#include "DUI/Toast.h"
#include "DUI/MessageBoxUI.h"
#include "DUI/ShadeWindow.h"
#include "Common.h"

#pragma warning(disable:4099)
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <afxdisp.h>
#include <afxdisp.h>

#ifdef _DEBUG
#ifndef _STR_BUILD_TIME
	#define _STR_BUILD_TIME _T("build.debug")
#endif
#ifndef _STR_FINAL_TIME
	#define _STR_FINAL_TIME _T("final.debug")
#endif
#endif

#ifndef MAX_URL_LEN 
#define MAX_URL_LEN 2048
#endif

//#ifdef DEVELOP_VERSION
//#ifndef MULTI_VERSION 
//#define MULTI_VERSION
//#endif
//#endif

