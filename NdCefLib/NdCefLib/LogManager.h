//================================================================================
// FileName:			LogManager.h
//
// Desc:
//================================================================================
#ifndef _LOG_MANAGER_H_
#define _LOG_MANAGER_H_

enum
{
	LOG_FOLDER_LOG_UncaughtException			= 0,
	LOG_FOLDER_LOG_Consolelog,
	LOG_FOLDER_LOG_CEF,
	LOG_FOLDER_LOG_JSCall,
	LOG_FOLDER_LOG_InvokeMethod,
	LOG_FOLDER_LOG_ZIP,
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
	void SetPath(std::wstring nPath);
protected:
	std::wstring m_nPath;
	CLog*				m_pLogFiles[MAX_LOG_FOLDERS];
};


#endif