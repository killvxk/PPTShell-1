#pragma once

#include <windows.h>
#include <time.h>
#include <string>
using std::string;

#define LOG_MAX_LINE_LENGTH		MAX_PATH*2		
#define LOG_MAX_BUFFER_LENGTH	512*1024
#define LOG_TIME_OUT			10*60*1000


//
// Log Types and Folders
//
enum
{
	AT_ONCE							= 1,
	BUFFER,
};


/************************************************************************/
/*    Gobal func						                               */
/************************************************************************/

const tstring	getAppPath(void);
void			createLogPath(const TCHAR * pRelativePath);

/************************************************************************/
/*    CLog is a multi thread log class                               */
/************************************************************************/

class CLog
{
private:
	//critical section
	CRITICAL_SECTION	m_lockBuffer;							
	CRITICAL_SECTION	m_lockFile;								
	TCHAR				m_buffer[LOG_MAX_BUFFER_LENGTH];	
	DWORD				m_dwStartPos;		
	DWORD				m_timeStart;		
	DWORD				m_timeEnd;

private:
	//read only
	tstring				m_appPath;		
	tstring				m_prefixName;		
	tstring				m_relativePath;	

private:
	DWORD				m_st;

public:
	CLog(const TCHAR *pRelativePath, const TCHAR *pPrefixName);
	virtual ~CLog(void);

public:
	const tstring	getFileName(void);
	const bool		isTimeOut(void);

public:
	void			writeLog(int writeMode, const TCHAR *pFormat, va_list va);
	void			writeToBuffer(const TCHAR *pFormat, va_list va);			//write data to buffer
	void			writeToFile(const TCHAR *pFormat, va_list va);			//write data to file
	void			saveBuffer(void);										//write buffer to file at once

public:
	// thread func before
	static void		writeBufferToFile(void * pObj);							

};
