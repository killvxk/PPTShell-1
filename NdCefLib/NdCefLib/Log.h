#pragma once

#include <windows.h>
#include <time.h>
#include <string>
using std::wstring;
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
/*    CLog is a multi thread log class                               */
/************************************************************************/

class CLog
{
private:
	//critical section
	CRITICAL_SECTION	m_lockBuffer;							
	CRITICAL_SECTION	m_lockFile;								
	char				m_buffer[LOG_MAX_BUFFER_LENGTH];	
	DWORD				m_dwStartPos;		
	DWORD				m_timeStart;		
	DWORD				m_timeEnd;

private:
	//read only
	wstring				m_appPath;		
	wstring				m_prefixName;		
	wstring				m_relativePath;	

private:
	DWORD				m_st;

public:
	CLog(std::wstring nPath,wstring pRelativePath, wstring pPrefixName);
	virtual ~CLog(void);

public:
	const wstring	getFileName(void);
	const bool		isTimeOut(void);

public:
	void			writeLog(int writeMode, const char *pFormat, va_list va);
	void			writeToBuffer(const char *pFormat, va_list va);			//write data to buffer
	void			writeToFile(const char *pFormat, va_list va);			//write data to file
	void			saveBuffer(void);										//write buffer to file at once

public:
	// thread func before
	static void		writeBufferToFile(void * pObj);							

};
