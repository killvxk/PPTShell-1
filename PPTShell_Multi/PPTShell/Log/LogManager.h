//================================================================================
// FileName:			LogManager.h
//
// Desc:
//================================================================================
#ifndef _LOG_MANAGER_H_
#define _LOG_MANAGER_H_

enum
{
	LOG_FOLDER_LOG_SERVER			= 0,
	LOG_FOLDER_LOG_LOCAL,
	LOG_FOLDER_LOG_TEMP,
	LOG_FOLDER_LOG_DEBUG,
	MAX_LOG_FOLDERS,

};


#include "Log.h"

class CLogManager
{
public:
	CLogManager(void);
	~CLogManager(void);

public:
	bool init();
	bool destroy();
	bool logMsg(int logtype, int logfolder, char *format, va_list va);
	bool logMsg(char *filename, char *format, va_list va);

protected:
	CLog*				m_pLogFiles[MAX_LOG_FOLDERS];
};


#endif