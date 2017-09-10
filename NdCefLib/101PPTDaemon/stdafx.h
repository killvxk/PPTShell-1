// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#ifndef _WIN32_WINNT		// 允许使用特定于 Windows XP 或更高版本的功能。
#define _WIN32_WINNT 0x0501	// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif	


#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <stdio.h>
#include <tchar.h>
#include <string>
#include <iostream>
#include <Tlhelp32.h>
#include <SetupAPI.h>
#include <atlstr.h>
#include "dbt.h"
#include "guiddef.h"

#include <memory>	
#include <vector>	
using namespace std;

void SvcDebugOut(LPSTR String, DWORD Status);
void SvcDebugOutFile(std::wstring nFilePath,LPSTR String, DWORD Status);
// TODO: 在此处引用程序需要的其他头文件
