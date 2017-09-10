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
	m_pLogFiles[LOG_FOLDER_LOG_SERVER]							= new CLog("log\\server", "S");
	m_pLogFiles[LOG_FOLDER_LOG_LOCAL]							= new CLog("log\\local", "L");
	m_pLogFiles[LOG_FOLDER_LOG_TEMP]							= new CLog("log\\temp", "T");
	m_pLogFiles[LOG_FOLDER_LOG_DEBUG]							= new CLog("log\\debug", "T");

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

	char szTmpBuffer[MAX_PATH*10+50]={0};
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