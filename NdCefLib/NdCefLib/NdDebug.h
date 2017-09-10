#pragma once
#include "Log.h"
#include "LogManager.h"

class CNdDebug
{
public:
	static void CNdDebug::wprintf(LPCWSTR fmt,...);
	static void CNdDebug::printf(LPCSTR fmt,...);
	static void CNdDebug::printfStr(LPCSTR fmt);
	static void CNdDebug::printfOut(LPCSTR logpath,LPCSTR fmt,...);
	static void CNdDebug::logMsg(int logfolder,const char *format, ...);
	static CLogManager* m_pLogMgr; 
};
