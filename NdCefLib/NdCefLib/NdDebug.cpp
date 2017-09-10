#include "stdafx.h"
#include <sstream>
#include <fstream>

void CNdDebug::wprintf(LPCWSTR fmt,...)
{
	std::wstringstream stream;
	va_list ap;
	int d;
	wchar_t c, *s;
	va_start(ap, fmt);
	while (*fmt)
		  switch (*fmt++) {
	 case 's': /* wstring */
		 s = va_arg(ap, wchar_t *);
		 stream<<s;
		 break;
	 case 'd': /* int */
		 d = va_arg(ap, int);
		 stream<<d;
		 break;
	 case 'c': /* wchar_t */
		 c = (wchar_t) va_arg(ap, wchar_t);
		  stream<<c;
		 break;
	  }
	  va_end(ap);
	  stream<<std::endl;
	  OutputDebugString(stream.str().c_str());
}

void CNdDebug::printf(LPCSTR fmt,...)
{
	std::stringstream stream;
	va_list ap;
	int d;
	char c, *s;
	va_start(ap, fmt);
	while (*fmt)
		  switch (*fmt++) {
	 case 's': /* string */
		 s = va_arg(ap,char *);
		 stream<<s;
		 break;
	 case 'd': /* int */
		 d = va_arg(ap, int);
		 stream<<d;
		 break;
	 case 'c': /* char */
		 c = (char) va_arg(ap, int);
		  stream<<c;
		 break;
	  }
	  va_end(ap);
	  stream<<std::endl;
	  OutputDebugStringA(stream.str().c_str());
}
void CNdDebug::printfOut(LPCSTR logpath,LPCSTR fmt,...)
{
	std::stringstream stream;
	va_list ap;
	int d;
	char c, *s;
	va_start(ap, fmt);
	while (*fmt)
		switch (*fmt++) {
	 case 's': /* string */
		 s = va_arg(ap,char *);
		 stream<<s;
		 break;
	 case 'd': /* int */
		 d = va_arg(ap, int);
		 stream<<d;
		 break;
	 case 'c': /* char */
		 c = (char) va_arg(ap, int);
		 stream<<c;
		 break;
	}
	va_end(ap);
	stream<<std::endl;
	std::ofstream log(logpath,std::ios::app); 
	log<<stream.str().c_str();
	log.close();
	 OutputDebugStringA(stream.str().c_str());
}
void CNdDebug::printfStr(LPCSTR fmt)
{
	 OutputDebugStringA(fmt);
	 OutputDebugStringA("\n");
}

CLogManager* CNdDebug::m_pLogMgr = NULL;
bool FindFirstFileExists(LPCTSTR lpPath, DWORD dwFilter)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(lpPath, &fd);
	bool bFilter = (FALSE == dwFilter) ? TRUE : fd.dwFileAttributes & dwFilter;
	bool RetValue = ((hFind != INVALID_HANDLE_VALUE) && bFilter) ? TRUE : FALSE;
	FindClose(hFind);
	return RetValue;
}

void CNdDebug::logMsg( int logfolder, const char *format, ...)
{
	/*
	if( m_pLogMgr == NULL ){
		TCHAR szPath[MAX_PATH];
		if( GetModuleFileName( NULL, szPath, MAX_PATH ) )
		{
			std::wstring nPath(szPath);
			nPath = nPath.substr(0,nPath.find_last_of('\\'));
			std::wstring nLogPath = nPath +L"\\log"; 
			CNdDebug::printfStr("---------------nLogPath------------");
			if (FindFirstFileExists(nLogPath.c_str(),false)&&CNdDebug::m_pLogMgr==NULL){
				CNdDebug::printfStr("---------------CLogManager------------");
				CNdDebug::m_pLogMgr = new CLogManager();
				if (CNdDebug::m_pLogMgr) {
					CNdDebug::printfStr("---------------m_pLogMgr------------");
					CNdDebug::m_pLogMgr->SetPath(nLogPath);
					CNdDebug::m_pLogMgr->init();
				}
			}
		}
	}
	*/
	if( m_pLogMgr == NULL ){
		return ;
	}
	va_list va;
	va_start(va, format);
	m_pLogMgr->logMsg(AT_ONCE, logfolder, (char*)format, va);
	va_end(va);

}