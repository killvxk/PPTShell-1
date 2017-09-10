#include "stdafx.h"
#include "Log.h"


const tstring getAppPath()
{
	TCHAR appPath[MAX_PATH] = {0};
	memset(appPath, 0, MAX_PATH);
	GetModuleFileName(NULL, appPath, MAX_PATH);

	TCHAR* lpPos=_tcsrchr(appPath, _T('\\'));
	if(lpPos)
	{
		*(lpPos+sizeof(TCHAR)) = '\0';
	}
	return tstring(appPath);
}

void createLogPath( const TCHAR * pRelativePath )
{
	tstring strRelativePath(pRelativePath);
	int nPos	= 1;
	tstring strTmpPath;
	tstring strCompletePath = getAppPath();
	while(strRelativePath.length() > 0)
	{
		nPos = strRelativePath.find_first_of(_T('\\'));
		if (nPos != -1)
		{
			strTmpPath		= strRelativePath.substr(0 , nPos);
			strRelativePath = strRelativePath.substr(nPos + 1, strRelativePath.length() - (nPos + 1));
		}
		else
		{
			strTmpPath		= strRelativePath.substr(0, strRelativePath.length());
			strRelativePath = _T("");
		}

		strCompletePath		+= strTmpPath;
		CreateDirectory(strCompletePath.c_str(), NULL);
		strCompletePath		+= _T('\\');
	}
}


CLog::CLog(const TCHAR *pRelativePath, const TCHAR *pPrefixName)
{
	::InitializeCriticalSection(&m_lockFile);
	::InitializeCriticalSection(&m_lockBuffer);

	m_appPath		= getAppPath(); 
	m_dwStartPos	= 0;
	m_timeStart		= GetTickCount();
	m_relativePath	= tstring(pRelativePath);
	m_prefixName	= tstring(pPrefixName);

	createLogPath(pRelativePath);
	memset(m_buffer, 0, LOG_MAX_BUFFER_LENGTH);
}

CLog::~CLog(void)
{
	::DeleteCriticalSection(&m_lockFile);
	::DeleteCriticalSection(&m_lockBuffer);
}


void CLog::writeToBuffer( const TCHAR *pFormat, va_list va )
{
	tm local		= {0};
	TCHAR szTime[32] = {0};
	__time64_t long_time;
	_time64( &long_time );

	localtime_s( &local, &long_time );
	wsprintf(szTime, _T("%02d:%02d:%02d ") ,local.tm_hour, local.tm_min, local.tm_sec);

	TCHAR szTmpBuffer[LOG_MAX_LINE_LENGTH*50]={0};

	_vsntprintf_s(szTmpBuffer, _countof(szTmpBuffer)-1, pFormat, va);

	tstring strBuffer = _T("");
	strBuffer += szTime;
	strBuffer += szTmpBuffer;
	strBuffer += _T("\n");

	int nBufferLen = strBuffer.length();
	EnterCriticalSection(&m_lockBuffer);

	if (nBufferLen + m_dwStartPos > LOG_MAX_BUFFER_LENGTH || isTimeOut())
	{
		writeBufferToFile(this);

		// clear buffer
		memset(m_buffer, 0, LOG_MAX_BUFFER_LENGTH);
		m_dwStartPos = 0;
	}
	memcpy(m_buffer + m_dwStartPos, strBuffer.c_str(), nBufferLen);
	m_dwStartPos += nBufferLen;

	LeaveCriticalSection(&m_lockBuffer);

}

void CLog::writeToFile( const TCHAR *pFormat, va_list va )
{
	__time64_t long_time;
	_time64( &long_time );
	tm local = {0}; 
	localtime_s( &local, &long_time );

	TCHAR szTime[32] = {0};

	SYSTEMTIME wtm;
	::GetLocalTime(&wtm);
	int iMilliTime = wtm.wMilliseconds;

	wsprintf(szTime, _T("%02d/%02d %02d:%02d:%02d::%02d "), local.tm_mon+1, local.tm_mday, local.tm_hour, local.tm_min, local.tm_sec, iMilliTime);

	TCHAR szTmpBuffer[LOG_MAX_LINE_LENGTH*50] = {0};

	_vsntprintf(szTmpBuffer, _countof(szTmpBuffer)-1, pFormat, va);

	tstring strBuffer = _T("");
	strBuffer += szTime;
	strBuffer += szTmpBuffer;
	strBuffer += _T("\n");

	tstring strFileName = getFileName();

	::EnterCriticalSection(&m_lockFile);
	FILE* file = NULL;
	_tfopen_s(&file, strFileName.c_str(), _T("a"));
	if (file == NULL)
	{
		::LeaveCriticalSection(&m_lockFile);
		return;
	}
	//fprintf(file, strBuffer.c_str());
	fwrite(strBuffer.c_str(), strBuffer.size(), 1, file);
	fclose(file);
	::LeaveCriticalSection(&m_lockFile);
}


void CLog::writeBufferToFile( void *pObj )
{
	CLog *pThis	= (CLog *)pObj;

	tstring strAllBuffer = _T("");
	strAllBuffer += pThis->m_buffer;
	//memset(pThis->m_buffer, 0, LOG_MAX_BUFFER_LENGTH);

	tstring strFileName = pThis->getFileName();
	EnterCriticalSection(&pThis->m_lockFile);
	{
		FILE* file = NULL;
		_tfopen_s(&file, strFileName.c_str(), _T("a"));
		if (file == NULL)
		{
			LeaveCriticalSection(&pThis->m_lockFile);
			return;
		}
		//fprintf(file, strAllBuffer.c_str());
		fwrite(strAllBuffer.c_str(), strAllBuffer.size(), 1, file);
		fclose(file);
	}
	LeaveCriticalSection(&pThis->m_lockFile);

}

const tstring CLog::getFileName(void)
{

	__time64_t long_time;
	_time64( &long_time );
	tm local = {0}; 
	localtime_s( &local, &long_time );

	TCHAR szFileName[MAX_PATH] = {0};
	wsprintf(szFileName, _T("%s%s\\%s%d-%.2d-%.2d.log"), m_appPath.c_str(), m_relativePath.c_str(), m_prefixName.c_str(), local.tm_year + 1900, local.tm_mon + 1,local.tm_mday);

	return tstring(szFileName);
	
}

const bool CLog::isTimeOut(void)
{
	DWORD dwDuration;

	m_timeEnd=GetTickCount();

	if (m_timeEnd < m_timeStart)
	{
		dwDuration = ( 0xFFFFFFFF - m_timeStart ) + m_timeEnd;
	}
	else
	{
		dwDuration = m_timeEnd - m_timeStart;
	}

	if(dwDuration >= LOG_TIME_OUT)
	{
		m_timeStart = m_timeEnd;
		return true;
	}
	else
	{
		return false;
	}
}
void CLog::saveBuffer(void)
{
	EnterCriticalSection(&m_lockBuffer);
	{
		writeBufferToFile(this);
	}
	LeaveCriticalSection(&m_lockBuffer);
	
}

void CLog::writeLog(int writeMode, const TCHAR *pFormat, va_list va )
{
	switch(writeMode)
	{
		case AT_ONCE: 
			this->writeToFile(pFormat, va);
			break;

		case BUFFER:
			this->writeToBuffer(pFormat, va);
			break;

	}
}
