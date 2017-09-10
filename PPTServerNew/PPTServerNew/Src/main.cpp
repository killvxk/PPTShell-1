#include "stdafx.h"
#include "Common.h"
#include "BaseClient.h"
#include "BaseServer.h"
#include "Packet.h"
#include "Log.h"
#include "LogManager.h"
#include <dbghelp.h> 
#include <iostream>

#pragma comment(lib, "dbghelp.lib")

#define SERVER_VERSION				"V1.0"
LONG WINAPI MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS *lpExceptionInfo);

//
// Global Variables
//
CLogManager*		g_pLogMgr		   = NULL;

void DeleteOverlapEx(WSAOVERLAPPED_EX* pOverlap)
{
	if (pOverlap->buffer != NULL)
	{
		delete pOverlap->buffer;
		pOverlap->buffer = NULL;
	}

	delete pOverlap;
	pOverlap = NULL;
}

//
// main
//
void main()
{
	system("title PPTServerNew");

	// verison
	SET_CONSOLE_RED;
	printf("Version: %s\r\n", SERVER_VERSION);
	RESET_CONSOLE;

	// set unhandled exception
	SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);

	WSAData wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);

	g_pLogMgr = new CLogManager();
	if( g_pLogMgr == NULL )
	{
		printf("[Error]: create log manager failed.\r\n");
		return;
	}

	int ret = g_pLogMgr->init();
	if( !ret )
	{	
		printf("[Error]: cLog initialize failed.\r\n");
		return;
	}

	WRITE_LOG_SERVER("---------------------PPTServer  START-----------------------------");

	CBaseServer *pBaseServer = CBaseServer::CreateInstance();
	if (pBaseServer == nullptr)
	{
		return;
	}
	BOOL res = pBaseServer->Initialize();
	if( !res )
	{
		printf("[Error]: initialize failed.\r\n");
		return;
	}

	res = pBaseServer->StartWork(18888);
	if( !res )
	{
		printf("[Error]: start work failed.\r\n");
		return;
	}
	else
	{
		printf("[Success]: listen on port: 18888.\r\n");
	}

	while( TRUE )
	{
// 		std::cout << "Press any key to show server current info:";
// 		system("pause");
		
		pBaseServer->ShowCurrInfo();
		Sleep(120000);
	}
	pBaseServer->DestroyInstance();
}



LONG WINAPI MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS *lpExceptionInfo)
{
	LONG ret = EXCEPTION_EXECUTE_HANDLER;

	char szFileName[64]; 

	SYSTEMTIME st;
	GetLocalTime(&st); 

	sprintf_s(szFileName, "%d-%d-%d-%d-%d-%d.dmp", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

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

//-----------------------------------------------------------------------------------------------------------
// Log
//
void logMsg(char* filename, int logtype, int logfolder, char *format, ...)
{
	if( g_pLogMgr == NULL )
		return;

	va_list va;
	va_start(va, format);

	if( filename == NULL )
		g_pLogMgr->logMsg(logtype, logfolder, format, va);

	else
		g_pLogMgr->logMsg(filename, format, va);

	va_end(va);

}