//================================================================================
// FileName:			LogManager.h
//
// Desc:
//================================================================================
#include "stdafx.h"
#include "common.h"
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
	m_pLogFiles[LOG_FOLDER_LOG_SERVER]							= new CLog(_T("log\\server"), _T("S"));
	m_pLogFiles[LOG_FOLDER_LOG_LOCAL]							= new CLog(_T("log\\local"), _T("L"));
	m_pLogFiles[LOG_FOLDER_LOG_TEMP]							= new CLog(_T("log\\temp"), _T("T"));
	m_pLogFiles[LOG_FOLDER_LOG_DEBUG]							= new CLog(_T("log\\debug"), _T("T"));

	return true;
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
bool CLogManager::logMsg(int logtype, int logfolder, TCHAR *format, va_list va)
{
	if( m_pLogFiles[logfolder] == NULL )
		return false;

	m_pLogFiles[logfolder]->writeLog(logtype, format, va);
	return true;
}

bool CLogManager::logMsg(TCHAR *filename, TCHAR *format, va_list va)
{
	__time64_t long_time;
	_time64( &long_time );
	tm local = {0}; 
	localtime_s( &local, &long_time );

	TCHAR szTime[32] = {0};
	wsprintf(szTime, _T("%d/%d %02d:%02d,") , local.tm_mon+1, local.tm_mday, local.tm_hour, local.tm_min);

	TCHAR szTmpBuffer[MAX_PATH*100+50]={0};
	_vsntprintf(szTmpBuffer, _countof(szTmpBuffer)-1, format, va);

	tstring strBuffer = _T("");
	strBuffer += szTime;
	strBuffer += szTmpBuffer;
	strBuffer += _T("\n");

	FILE* file = _tfopen(filename, _T("a"));
	if (file == NULL)
		return false;

	_ftprintf(file, strBuffer.c_str()); 
	
	fclose(file);
	return true;
}