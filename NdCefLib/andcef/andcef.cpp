// andcef.cpp : Implementation of CandcefApp and DLL registration.

#include "stdafx.h"
#include "andcef.h"

#include "include/cef_app.h"
#include "include/base/cef_scoped_ptr.h"
#include "NdCefMain.h"
#include <dbghelp.h> 
#pragma comment(lib, "dbghelp.lib")

using namespace NdCef;

CandcefApp theApp;

scoped_ptr<CNdCefMain> nCefMain;

const GUID CDECL BASED_CODE _tlid =
		{ 0xBC25CF1D, 0xD20F, 0x4160, { 0xAC, 0x3B, 0x90, 0x8F, 0x83, 0x83, 0x85, 0x3A } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;

LONG WINAPI MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS *lpExceptionInfo);

// CandcefApp::InitInstance - DLL initialization

BOOL CandcefApp::InitInstance()
{
	SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
	
	BOOL bInit = COleControlModule::InitInstance();


	if (bInit)
	{
		// TODO: Add your own module initialization code here.
	}

	return bInit;
}



// CandcefApp::ExitInstance - DLL termination

int CandcefApp::ExitInstance()
{
	CNdDebug::printfStr("ExitInstance");
	// TODO: Add your own module termination code here.
	if (nCefMain.get()){
		nCefMain.get()->Shutdown(true);
		nCefMain.release();
		nCefMain.reset();
	}
	CNdDebug::printfStr("ExitInstance OK");
	return COleControlModule::ExitInstance();
}



// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}



// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
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