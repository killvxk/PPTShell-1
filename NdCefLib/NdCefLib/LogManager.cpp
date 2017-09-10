//================================================================================
// FileName:			LogManager.h
//
// Desc:
//================================================================================
#include "stdafx.h"
#include "LogManager.h"


CLogManager::CLogManager(void)
{
}

CLogManager::~CLogManager(void)
{
}

//
// init
//
bool CLogManager::init()
{
	m_pLogFiles[LOG_FOLDER_LOG_UncaughtException]			= new CLog(m_nPath,L"UncaughtException", L"");
	m_pLogFiles[LOG_FOLDER_LOG_Consolelog]					= new CLog(m_nPath,L"console", L"");
	m_pLogFiles[LOG_FOLDER_LOG_CEF]							= new CLog(m_nPath,L"cef", L"");
	m_pLogFiles[LOG_FOLDER_LOG_JSCall]						= new CLog(m_nPath,L"JSCall", L"");
	m_pLogFiles[LOG_FOLDER_LOG_InvokeMethod]				= new CLog(m_nPath,L"InvokeMethod", L"");
	m_pLogFiles[LOG_FOLDER_LOG_ZIP]							= new CLog(m_nPath,L"zip", L"");
	

	return true;
}
void CLogManager::SetPath(std::wstring nPath){
	m_nPath = nPath;
}
//
// destroy
//
bool CLogManager::destroy()
{
	for(int i = 0; i < MAX_LOG_FOLDERS; i++)
	{
		if( m_pLogFiles[i] != NULL )
		{
			m_pLogFiles[i]->saveBuffer();
			delete m_pLogFiles[i];
			m_pLogFiles[i] = NULL;
		}
	}

	return true;
}

//
// log message
//
bool CLogManager::logMsg(int logtype, int logfolder, char *format, va_list va)
{
	if( m_pLogFiles[logfolder] == NULL )
		return false;

	m_pLogFiles[logfolder]->writeLog(logtype, format, va);
	return true;
}

bool CLogManager::logMsg(char *filename, char *format, va_list va)
{
	__time64_t long_time;
	_time64( &long_time );
	tm local = {0}; 
	localtime_s( &local, &long_time );

	char szTime[32] = {0};
	sprintf_s(szTime, "%d/%d %02d:%02d," , local.tm_mon+1, local.tm_mday, local.tm_hour, local.tm_min);

	char szTmpBuffer[MAX_PATH+50]={0};
	_vsnprintf(szTmpBuffer, sizeof(szTmpBuffer)-1, format, va);

	string strBuffer = "";
	strBuffer += szTime;
	strBuffer += szTmpBuffer;
	strBuffer += "\n";

	FILE* file = fopen(filename, "a");
	if (file == NULL)
		return false;

	fprintf(file, strBuffer.c_str()); 
	
	fclose(file);
	return true;
}