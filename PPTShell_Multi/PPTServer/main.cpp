#include "stdafx.h"
#include "Common.h"
#include "BaseClient.h"
#include "BaseServer.h"
#include "Packet.h"
#include "PacketProcessor.h"
#include "Log.h"
#include "LogManager.h"
#include <dbghelp.h> 
#pragma comment(lib, "dbghelp.lib")

#define SERVER_VERSION				"V1.0"
LONG WINAPI MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS *lpExceptionInfo);

//
// Global Variables
//
CPacketProcessor*	g_pPacketProcessor = NULL;
CLogManager*		g_pLogMgr		   = NULL;

//
// packet process
//
void OnPacketProcess(CBaseClient* pClient, char* pPacket)
{
	if( g_pPacketProcessor != NULL )
		g_pPacketProcessor->OnProcessPacket(pClient, pPacket);
}

//
// socket closed
//
void OnSocketClosed(CBaseClient* pClient, char* pPacket)
{
	if( g_pPacketProcessor != NULL )
		g_pPacketProcessor->OnSocketClosed(pClient);
}

//
// main
//
void main()
{
	system("title PPTServer");

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

	g_pPacketProcessor = new CPacketProcessor;
	if( g_pPacketProcessor == NULL )
		return;

	g_pPacketProcessor->Initialize();

	CBaseServer baseServer;
	BOOL res = baseServer.Initialize(OnPacketProcess, OnSocketClosed);
	if( !res )
	{
		printf("[Error]: initialize failed.\r\n");
		return;
	}

	res = baseServer.StartWork(18888);
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
		Sleep(10000);
	}
}



LONG WINAPI MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS *lpExceptionInfo)
{
	LONG ret = EXCEPTION_EXECUTE_HANDLER;

	char szFileName[64]; 

	SYSTEMTIME st;
	GetLocalTime(&st); 

	sprintf(szFileName, "%d-%d-%d-%d-%d-%d.dmp", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

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