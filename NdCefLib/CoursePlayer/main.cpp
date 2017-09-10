// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <windows.h>
#include <shellapi.h>
#include <string>
#include <tchar.h>
#include "NdCefCmdLine.h"
#include "include/cef_app.h"
#include "include/base/cef_scoped_ptr.h"
#include "NdCefMain.h"

#include <dbghelp.h> 
#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "shell32.lib")

using namespace NdCef;
scoped_ptr<CNdCefMain> nCefMain;

LONG WINAPI MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS *lpExceptionInfo);
// Entry point function for all processes.
int APIENTRY wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPTSTR    lpCmdLine,
                      int       nCmdShow) 
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);

	CNdDebug::printfStr("--------CoursePlayer Main---------");
	CNdDebug::wprintf(L"%s",lpCmdLine);
	CNdCefCmdLine<wstring> argumentList;
	argumentList.parseCmd();
	if (argumentList.size()==0){
		return false;
	}else {
		std::wstring strType = argumentList[0];
		if (strType == _T("player")){
			// player D:\player\index.html?main=/E:/NDCloud/NdpCourse/b273d9b0-5ab0-4046-bec4-adfdea5b5ac2_default_webp/main.xml screenClass 1 1
			if (argumentList.size()<2){
				return false;
			}
			std::wstring strUrl = argumentList[1];
			std::wstring strParentWndName;
			if( argumentList.size() >= 3 )
				strParentWndName = argumentList[2];
			std::wstring strCurPage;
			std::wstring strTotalPage;
			bool bLast = false;

			if( argumentList.size() >= 5 )
			{
				strCurPage		= argumentList[3];
				strTotalPage	= argumentList[4];

				if( strCurPage == strTotalPage )
					bLast = true;
			}	
			HWND hParentWnd = NULL;
			if( !strParentWndName.empty() )
				hParentWnd = FindWindowW(strParentWndName.c_str(), NULL);

			scoped_ptr<CNdCefMain> nCefMain(new CNdCefMain());
			nCefMain.get()->SetInstance(hInstance);
			nCefMain.get()->RunMain();
			LoadKeyboardLayout( L"0x0409", KLF_ACTIVATE|KLF_SETFORPROCESS );  //win8.1进行恢复到默认输入法
			nCefMain.get()->CreatePlayBrower(NULL,strParentWndName,strUrl,true);
			//nCefMain.get()->ShowMainUrl(hParentWnd,strUrl);
			nCefMain.get()->MsgRun();
			nCefMain.get()->CloseAllBrowsers();
			nCefMain.get()->Shutdown();
			return 0;
		}else if (strType == _T("editor")){
			HWND hWnd = NULL;
			if (argumentList.size()<2){
				return false;
			}
			bool nClose = false;
			std::wstring strUrl = argumentList[1];
			std::wstring strQuestionType;

			if( argumentList.size() >= 3 )
			{
				CefString strWndHandle = argumentList[2];
				std::string str = strWndHandle.ToString();
				hWnd = (HWND)atoi(str.c_str());
			}

			if( argumentList.size() >= 4 ){
				if (argumentList[3]== _T("close")){
					nClose = true;
				}
			}
			
			if( argumentList.size() >= 5 )
				strQuestionType = argumentList[3];
			else
				strQuestionType = _T("习题编辑器2.0");

			nCefMain.reset(new CNdCefMain());
			nCefMain.get()->SetInstance(hInstance);
			nCefMain.get()->RunMain();
			if (hWnd!=NULL) nCefMain.get()->SetPPTMainWnd(hWnd);
			LoadKeyboardLayout( L"0x0409", KLF_ACTIVATE|KLF_SETFORPROCESS );  //win8.1进行恢复到默认输入法
			nCefMain.get()->CreatePlayBrower(NULL,strQuestionType,strUrl,nClose);
			nCefMain.get()->MsgRun();
			nCefMain.get()->CloseAllBrowsers();
			nCefMain.get()->Shutdown();
		}else if( strType == _T("thumbnail")){
			// thumbnail D:\player\index.html?main=/E:/NDCloud/NdpCourse/b273d9b0-5ab0-4046-bec4-adfdea5b5ac2_default_webp/main.xml d:\\1.jpg
			if (argumentList.size()>=3){
				CefString strUrl = argumentList[1];
				CefString strPath = argumentList[2];
				DWORD dwStart = GetTickCount();
				nCefMain.reset(new CNdCefMain());
				nCefMain.get()->SetInstance(hInstance);
				nCefMain.get()->SetSubprocessCmdNoReg();
				nCefMain.get()->RunMain();
				nCefMain.get()->ShowMainUrl(NULL,strUrl,true,false,strPath);
				nCefMain.get()->MsgRun();
				DWORD dwEnd = GetTickCount();
				DWORD time = dwEnd-dwStart;
				if (argumentList.size()==4){
					CefString strLogPath = argumentList[3];
					CNdDebug::printfOut(strLogPath.ToString().c_str(),"%s%s%d",strUrl.ToString().c_str()," time:",time);
				}else{
					CNdDebug::printf("%s%s%s%d","-----------thumbnail:",strUrl.ToString().c_str()," time:",time);
				}
				nCefMain.get()->CloseAllBrowsers();
				nCefMain.get()->Shutdown();
				
			}
		}else if (strType == L"OSR"){
			if (argumentList.size()<2){
				return false;
			}
			HWND hWnd = NULL;
			if( argumentList.size() >= 3 )
			{
				CefString strWndHandle = argumentList[2];
				std::string str = strWndHandle.ToString();
				hWnd = (HWND)atoi(str.c_str());
			}
			
			std::wstring strUrl = argumentList[1];
			scoped_ptr<CNdCefMain> nCefMain(new CNdCefMain());
			nCefMain.get()->SetInstance(hInstance);
			nCefMain.get()->RunMain();
			nCefMain.get()->ShowMainUrl(hWnd,strUrl,true,true);
			nCefMain.get()->MsgRun();
			nCefMain.get()->CloseAllBrowsers();
			nCefMain.get()->Shutdown();
			return 0;
		}
	}
	return 0;
}

std::wstring GetLocalPath()
{
	TCHAR szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, MAX_PATH);

	TCHAR* p = _tcsrchr(szPath, '\\');
	*p = '\0';

	return szPath;
}
LONG WINAPI MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS *lpExceptionInfo)
{
	LONG ret = EXCEPTION_EXECUTE_HANDLER;

	wchar_t szFileName[MAX_PATH]; 

	SYSTEMTIME st;
	GetLocalTime(&st); 

	wstring strPath = GetLocalPath();
	strPath += _T("\\Dump\\");
	CreateDirectory(strPath.c_str(), NULL);

	wsprintf(szFileName, _T("%s%d-%d%d-%d%d-%d-%s.dmp"), strPath.c_str(), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,L"CoursePlayer");

	HANDLE hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION ExInfo;

		ExInfo.ThreadId				= GetCurrentThreadId();
		ExInfo.ExceptionPointers	= lpExceptionInfo;
		ExInfo.ClientPointers		= false;

		// write the dump
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL ); 
		CloseHandle(hFile);
	}

	return EXCEPTION_EXECUTE_HANDLER;
}