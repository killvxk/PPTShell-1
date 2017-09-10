// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__B76718EC_2B32_47F1_8119_FEB017D23BB0__INCLUDED_)
#define AFX_STDAFX_H__B76718EC_2B32_47F1_8119_FEB017D23BB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

#include "GdiPlusPng.h"
#include <afxdhtml.h>
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__B76718EC_2B32_47F1_8119_FEB017D23BB0__INCLUDED_)


#include <vector>
using namespace std;

#define MD5_LEN				32
typedef struct _UPDATEDATA
{
	char	sFile[MAX_PATH];
	char	sHash[MD5_LEN+1];
	bool	bDecomp;
	bool    bDown;
}UPDATEDATA, *PUPDATEDATA;

extern BYTE*			g_pUpdateBuff;;