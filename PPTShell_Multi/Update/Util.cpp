#include "StdAfx.h"
#include "Util.h"
#include <direct.h>
#include <shlwapi.h>
#include "MD5Encrypt.h"

#pragma warning(disable:4996)

int FindChar_f(LPSTR t, LPSTR s)
{
	char* p = strstr(t, s);
	if (NULL == p) return -1;
	return (int)(p-t);
}

int FindChar_b(LPSTR t, LPSTR s)
{
	char* p = strstr(t, s);
	if (NULL == p) return -1;
	char* pp = p;
	while (true)
	{
		p = strstr(&p[1], s);
		if (NULL == p) break;
		pp = p;
	}
	return (int)(pp-t);
}

int FindChar_n(LPSTR t, LPSTR s, int n)
{
	int lenS = strlen(s);
	for (int i=0; i<n; i++)
	{
		for (int j=0; j<lenS; j++)
		{
			if (s[j] != t[i+j])
				break;
			if (j == lenS-1)
				return i;
		}
	}
	return -1;
}

int JudgeProtocol(LPCSTR szUrl)
{
	//print("JudgeProtocol\n");

	if (('h'==szUrl[0] || 'H'==szUrl[0]) 
	&&  ('t'==szUrl[1] || 'T'==szUrl[1]) 
	&&  ('t'==szUrl[2] || 'T'==szUrl[2]) 
	&&  ('p'==szUrl[3] || 'P'==szUrl[3]) 
	&& ':'==szUrl[4] && '/'==szUrl[5] && '/'==szUrl[6])
		return DOWN_TYPE_HTTP;
	else if (
		('f'==szUrl[0] || 'F'==szUrl[0]) 
	&&  ('t'==szUrl[1] || 'T'==szUrl[1]) 
	&&  ('p'==szUrl[2] || 'P'==szUrl[2]) 
	&& ':'==szUrl[3] && '/'==szUrl[4] && '/'==szUrl[5])
		return DOWN_TYPE_FTP;
	else
		return DOWN_TYPE_NULL;
}

int ConnWithTime(SOCKET& s_conn, const char* szServ, const int nPort)
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(nPort);

	if (INADDR_NONE == inet_addr(szServ))
	{
		hostent* hp = gethostbyname(szServ);
		if (NULL == hp) return -1;
		addr.sin_addr.s_addr = *((unsigned long*)hp->h_addr);
	}
	else
	{
		addr.sin_addr.s_addr = inet_addr(szServ);
	}

	if (CheckApiInLineHook(connect))
		RemoveHook("ws2_32.dll", "connect");

	int TimeOut = TIMEOUT*1000;
	unsigned long ul = 1;
	for (int i=0; i<RE_CONN_TIME; ++i)
	{
		s_conn = socket(AF_INET, SOCK_STREAM, 0);

		//设置发送超时、接收超时、非阻塞方式连接
		if (SOCKET_ERROR == setsockopt(s_conn, SOL_SOCKET, SO_SNDTIMEO, (char*)&TimeOut, sizeof(TimeOut)) 
		||  SOCKET_ERROR == setsockopt(s_conn, SOL_SOCKET, SO_RCVTIMEO, (char*)&TimeOut, sizeof(TimeOut)) 
		||  SOCKET_ERROR == ioctlsocket(s_conn, FIONBIO, (unsigned long*)&ul))
			return -1;

		connect(s_conn, (sockaddr*)&addr, sizeof(addr));
		timeval timeout;
		fd_set r;

		FD_ZERO(&r);
		FD_SET(s_conn, &r);
		timeout.tv_sec  = TIMEOUT;
		timeout.tv_usec = 0;
		if (select(0, 0, &r, 0, &timeout) <= 0)
			closesocket(s_conn);
		else
			break;
		print("超时：%d次\n", i);
		if (RE_CONN_TIME-1 == i)
		{
			print("超时退出!\n");
			return -1;
		}
	}

	//设置阻塞方式连接
	unsigned long ul1 = 0;
	if(SOCKET_ERROR == ioctlsocket(s_conn, FIONBIO, (unsigned long*)&ul1))
	{
		closesocket(s_conn);
		return -1;
	}

	return 0;
}

int SplitHttpUrl(char* szUrl, char* szServ, int& nPort, char* szPath)
{
	char l_szUrl[URL_LEN_MAX];
	strcpy(l_szUrl, szUrl);
	strcpy(szServ, l_szUrl+7);

	int n = FindChar_f(szServ, "/");
	if (-1 == n) return -1;
	int nSize = strlen(szServ)-n;

	strncpy(szPath, szServ+n, nSize);
	szPath[nSize] = '\0';
	szServ[n] = '\0';

	nPort = 80;
	n = FindChar_f(szServ, ":");
	if (-1 != n)
	{
		char sTmp[8];
		nSize = strlen(szServ)-n-1;
		strncpy(sTmp, szServ+n+1, nSize);
		szServ[n] = '\0';
		sTmp[nSize] = '\0';
		nPort = atoi(sTmp);
	}
	return 0;
}

LONG HttpSize(char* szUrl)
{
	char l_szUrl[URL_LEN_MAX];
	strcpy(l_szUrl, szUrl);
	char szServ[URL_LEN_MAX];
	char szPath[URL_LEN_MAX];
	int nPort;
	while (true)
	{
		if (-1 == SplitHttpUrl(l_szUrl, szServ, nPort, szPath))
		{ print("SplitUrl error\n"); return -1;}

		SOCKET s_conn = NULL;
		if (0 != ConnWithTime(s_conn, szServ, nPort))
		{ print("ConnWithTime error\n"); return -1;}

		HttpSend(s_conn, szServ, szPath);
		char sBuf[512];
		int nRecLen = recv(s_conn, sBuf, 512, 0);
		closesocket(s_conn);

		if (0 >= nRecLen) { print("recv error\n"); return -1;}
		int n = FindChar_f(sBuf, "HTTP/");
		if (-1 == n) { print("no http 1\n"); return -1;}

		char sTmp[64];
		strncpy(sTmp, sBuf+9, 3);
		sTmp[3] = '\0';
		int nAckStatus = atoi(sTmp);

		if (301 == nAckStatus)
		{
			print("Jmp\n");
			n = FindChar_f(sBuf, "Location: ");
			if (-1 == n) { print("no http 2\n"); return -1;}
			int m = FindChar_f(sBuf+n, "\r\n");
			if (-1 == m && m > 10) { print("no http 3\n"); return -1;}
			m-=10;
			strncpy(l_szUrl, sBuf+n+10, m);
			l_szUrl[m] = '\0';
			continue;
		}
		else if (nAckStatus<200 || nAckStatus>=300)
			return -1;

		n = FindChar_f(sBuf, "Content-Length: ");
		if (-1 == n) { print("no http 4\n"); return -1;}
		int m = FindChar_f(sBuf+n, "\r\n");
		if (-1 == m && m > 16) { print("no http 5\n"); return -1;}
		m-=16;
		strncpy(sTmp, sBuf+n+16, m);
		sTmp[m] = '\0';
		return atoi(sTmp);
	}

	print("no http 6\n");
	return -1;
}


bool CodePageConvert(const string& Source, string& Target, int SourceCodePage, int TargetCodePage)
{
	int nUnicodeLen = MultiByteToWideChar(SourceCodePage, 0, Source.c_str(), -1, NULL, 0);
	wchar_t* pBuf = NULL;

	pBuf = new wchar_t[nUnicodeLen+1];
	if (!pBuf)
	{
		return false;
	}

	memset(pBuf, 0, (nUnicodeLen+1)*sizeof(wchar_t));
	MultiByteToWideChar(SourceCodePage, 0, Source.c_str(), -1, pBuf, nUnicodeLen);

	int nTargetlen = WideCharToMultiByte(TargetCodePage, 0, pBuf, -1, NULL, 0, NULL, NULL);
	char* pTarget = NULL;

	pTarget = new char[nTargetlen+1];
	if (!pTarget)
	{
		delete[] pBuf;
		pBuf = NULL;
		return false;
	}

	memset(pTarget, 0, (nTargetlen+1)*sizeof(char));
	WideCharToMultiByte(TargetCodePage, 0, pBuf, -1, pTarget, nTargetlen, NULL, NULL);

	Target = pTarget;
	delete[] pTarget;
	pTarget = NULL;
	delete[] pBuf;
	pBuf = NULL;

	return true;
}

bool CanonicalizedURL(const char* pszUrl, int nLen, string& strOutputUrl)
{
	if (!pszUrl)
	{
		return false;
	}

	const char HEX_MAP[] = 
	{
		'0', '1', '2', '3', '4', '5', '6', '7', 
		'8', '9', 'A', 'B', 'C', 'D', 'E', 'F' 
	};

	for (int i = 0; i < nLen; i++)
	{
		char ch = pszUrl[i];
		if (ch > ' ' && ch != '%')
		{
			strOutputUrl += ch;
		}
		else
		{
			// < 0表示中文,<=0x20 空白字符也编码
			int iVal = ch;
			strOutputUrl += L'%';
			strOutputUrl += HEX_MAP[(iVal >> 4) & 0x0f];
			strOutputUrl += HEX_MAP[iVal & 0xf];
		}
	}

	return true;
}

int HttpSend(SOCKET& s_conn, const char* szServ, const char* szPath, const LONG nLow, const LONG nRange)
{
	char s[10240];
	char sRange[256];
	if (0 == nRange)
		sRange[0] = '\0';
	else
	{
		char sTmp[128];
		strcpy(sRange, "Range: bytes=");
		_i64toa(nLow, sTmp, 10);
		strcat(sRange, sTmp);
		strcat(sRange, "-");
		_i64toa(nLow+nRange-1, sTmp, 10);
		strcat(sRange, sTmp);
		strcat(sRange, "\r\n");
	}

	string strUtf8Url;
	string strCanonicalizedUrl;

	CodePageConvert(szPath, strUtf8Url, CP_ACP, CP_UTF8);
	CanonicalizedURL(strUtf8Url.c_str(), strUtf8Url.length(), strCanonicalizedUrl);

	sprintf(s, "GET %s HTTP/1.1\r\n"
		"Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/xaml+xml, "
		"application/vnd.ms-xpsdocument, application/x-ms-xbap, application/x-ms-application, "
		"application/x-silverlight, application/x-shockwave-flash, application/msword, "
		"application/vnd.ms-excel, application/vnd.ms-powerpoint, */*\r\n"
		"Accept-Language: zh-cn\r\n"
		"Accept-Encoding: gzip, deflate\r\n"
		"%s"
		"User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; Mozilla/4.0 "
		"(compatible; MSIE 6.0; Windows NT 5.1; SV1) ; .NET CLR 2.0.50727; .NET CLR 3.0.04506.30; "
		".NET CLR 3.0.04506.648; .NET CLR 1.1.4322; CIBA)\r\n"
		"Host: %s\r\n"
		"Connection: Keep-Alive\r\n\r\n", 
		strCanonicalizedUrl.c_str(), sRange, szServ);
	send(s_conn, s, strlen(s), 0);
	return 0;
}

BOOL CheckApiInLineHook(PVOID ApiAddress)
{
	BYTE OrigCode[7] = {0};
	RtlCopyMemory(OrigCode, (BYTE*)ApiAddress, 6);
	if(OrigCode[0] == 0xE9 || (OrigCode[0] == 0x68 && OrigCode[4] == 0xC3) || OrigCode[0] == 0xB8)
		return 1;
	return 0;
}

BOOL RemoveHook(LPCSTR szDllPath, LPCSTR szFuncName)
{
	BOOL bRet = FALSE;
	void* lpBase = NULL;
	char szPath[MAX_PATH];
	do 
	{
		lpBase = LoadLibrary(szDllPath);
		if (NULL == lpBase)
			break;
		
		void* lpFunc = GetProcAddress((HMODULE)lpBase, szFuncName);
		if (NULL == lpFunc)
			break;

		GetModuleFileName((HMODULE)lpBase, szPath, MAX_PATH);
		DWORD dwRVA = (DWORD)lpFunc - (DWORD)lpBase;
		HANDLE hFile = CreateFile(szPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (INVALID_HANDLE_VALUE == hFile)
			break;
		
		DWORD dwSize = GetFileSize(hFile, NULL);
		HANDLE hMapFile = CreateFileMapping(hFile, NULL, PAGE_READONLY|SEC_IMAGE, 0, dwSize, NULL);
		void* lpBaseMap = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, dwSize);
		void* lpRealFunc = (void*)((DWORD)lpBaseMap + dwRVA);
		
		//校验
		char* p1 = (char*)lpFunc;
		char* p2 = (char*)lpRealFunc;
		if (p1[7] != p2[7] || p1[8] != p2[8] || p1[9] != p2[9])
		{
			UnmapViewOfFile(lpBaseMap);
			CloseHandle(hMapFile);
			CloseHandle(hFile);
			return FALSE;
		}

		bRet = TRUE;
		DWORD dwOldProtect;
		if (VirtualProtect(lpFunc, 10, PAGE_EXECUTE_READWRITE, &dwOldProtect))
			memcpy(lpFunc, lpRealFunc, 10);
		else
			bRet = FALSE;
		
		UnmapViewOfFile(lpBaseMap);
		CloseHandle(hMapFile);
		CloseHandle(hFile);
		
	} while (false);
	
	if (NULL != lpBase)
		FreeLibrary((HMODULE)lpBase);
	return bRet;
}

bool JudgeDir(char* szPath)
{
	char szPath1[MAX_PATH];
	strcpy(szPath1, szPath);
	UINT nLen = strlen(szPath1);
	if ('\\' == szPath1[nLen-1] ||'/' == szPath1[nLen-1])
		strcat(szPath1, "*.*");
	else
		strcat(szPath1, "\\*.*");
	WIN32_FIND_DATA wfd;
	bool rValue = false;
	HANDLE hFind = FindFirstFile(szPath1, &wfd);
	if (INVALID_HANDLE_VALUE != hFind) rValue = true;
	FindClose(hFind);
	if (rValue)
		return true;
	else
	{
		strcpy(szPath1, szPath);
		return (TRUE == CreateDirectory(szPath1, NULL)) ? true : false;
	}
}

void print(const char *fmt, ...)
{
	char buf[2048];
	char buf1[1024];
	SYSTEMTIME time;
	GetLocalTime(&time);
	sprintf(buf, "%02d-%02d %02d:%02d:%02d\t\t", 
		time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
	va_list args;
	va_start(args, fmt);
	int argno = vsprintf(buf1, fmt, args);
	va_end(args);
	strcat(buf, buf1);
	
	OutputDebugString(buf);
}


// 取父目录
string GetParentPath(const string& strFileName)
{
	static const basic_string <char>::size_type npos = -1;
	basic_string <char>::size_type indexCh = strFileName.rfind('\\');

	if (indexCh == npos)
	{
		indexCh = strFileName.rfind('/');
	}
	if (indexCh == npos)
	{
		return "";
	}

	string strPath = strFileName.substr(0, indexCh);

	return strPath;
}


// 是否是目录
bool IsDir(const string& strFileName)
{
	// 	CPath path(strFileName);
	// 	return boost::filesystem::is_directory(path);

	WIN32_FIND_DATA wfd;
	string strCheckName = strFileName;

	if (strCheckName[strCheckName.length()-1] == '\\' || strCheckName[strCheckName.length()-1] == '\\')
	{
		strCheckName = strCheckName.substr(0, strCheckName.length()-1);
	}

	ZeroMemory(&wfd, sizeof(wfd));
	HANDLE handle = FindFirstFile(strCheckName.c_str(), &wfd);


	if (handle != INVALID_HANDLE_VALUE)
	{
		FindClose(handle);

		if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			return true;
		}
	}

	return false;
}

// 创建目录
bool DirectoryMake(const string& strDirectory)
{
	//	return boost::filesystem::create_directories(strDirectory);
	if (strDirectory.empty())
	{
		return false;
	}

	int nIndex = 0;
	int nRes = 0;
	string strDirectoryFull = strDirectory;

	if (strDirectoryFull[strDirectoryFull.length()-1] != '\\' && strDirectoryFull[strDirectoryFull.length()-1] != '/')
	{
		strDirectoryFull += '\\';
	}

	int nPathLen = strDirectoryFull.length()+1;
	char* pszPathCopy = new char[nPathLen];

	if (!pszPathCopy)
	{
		return false;
	}

	ZeroMemory(pszPathCopy, nPathLen);
	strncpy(pszPathCopy, strDirectoryFull.c_str(), nPathLen-1);

	if (pszPathCopy[0])
	{
		nIndex = 1;

		while (pszPathCopy[nIndex])
		{
			if (pszPathCopy[nIndex] == '/' || pszPathCopy[nIndex] == '\\')
			{
				if (pszPathCopy[nIndex-1] == ':')
				{
					nIndex++;
					continue;
				}

				pszPathCopy[nIndex] = 0;

				nRes = mkdir(pszPathCopy);
				if (nRes < 0 && errno != EEXIST)
				{
					delete[] pszPathCopy;
					return false;
				}

				pszPathCopy[nIndex] = '/';
			}
			nIndex++;
		}
	}

	delete[] pszPathCopy;
	return true;
}

bool MiscMyDeleteFile(const char* pcszFileName)
{
	if(!PathFileExists(pcszFileName))
	{
		return true;
	}

	if(!DeleteFile(pcszFileName))
	{
		BOOL bRet = MoveFileEx(pcszFileName,NULL,MOVEFILE_DELAY_UNTIL_REBOOT);
		if(!bRet)
		{
			return false;
		}
	}
	return true;
}

bool MiscMyMoveFile(const char* pcszSrcFile,const char* pcszDstFile)
{
	MakeDir(pcszDstFile);
// 	string strParentPath = GetParentPath(pcszDstFile);
// 	if(strParentPath.length() > 0)
// 	{
// 		if (!IsDir(strParentPath))
// 		{
// 			DirectoryMake(strParentPath);
// 		}
// 	}
	if(!PathFileExists(pcszDstFile))
	{
		// 目标文件不存在
		BOOL bRet = FALSE;
		bRet = MoveFileEx(pcszSrcFile,pcszDstFile,MOVEFILE_REPLACE_EXISTING|MOVEFILE_COPY_ALLOWED);
		if(!bRet)
		{
			print("MoveFile %s fail error code:%u",pcszDstFile,GetLastError());
			return false;
		}
	}
	else
	{
		// 目标文件存在
		BOOL bRet = FALSE;
		if(!MoveFileEx(pcszSrcFile, pcszDstFile, MOVEFILE_COPY_ALLOWED|MOVEFILE_REPLACE_EXISTING))
		{
			CString strDstDel;
			strDstDel.Format("%s.del",pcszSrcFile);
			MoveFileEx(pcszDstFile,strDstDel,MOVEFILE_COPY_ALLOWED|MOVEFILE_REPLACE_EXISTING);
			if(!MoveFileEx(pcszSrcFile,pcszDstFile,MOVEFILE_COPY_ALLOWED|MOVEFILE_REPLACE_EXISTING))
			{
				MoveFileEx(strDstDel,pcszDstFile,MOVEFILE_COPY_ALLOWED|MOVEFILE_REPLACE_EXISTING);
				return false;
			}
			else
			{
				MoveFileEx(strDstDel,NULL,MOVEFILE_REPLACE_EXISTING|MOVEFILE_DELAY_UNTIL_REBOOT);
			}
		}
	}
	return true;
}


bool ChkDir()
{
	CString strFilePath;
	GetModuleFileName(NULL, strFilePath.GetBuffer(MAX_PATH), MAX_PATH);
	strFilePath.ReleaseBuffer();
	strFilePath = strFilePath.Left(strFilePath.ReverseFind('\\') + 1);
	strFilePath = strFilePath + UPDATE_TEMP_DIR;

	char szPath[MAX_PATH];
	strcpy(szPath, strFilePath);
	UINT nLen = strlen(szPath);
	if ('\\' == szPath[nLen-1] ||'/' == szPath[nLen-1])
		strcat(szPath, "*.*");
	else
		strcat(szPath, "\\*.*");
	WIN32_FIND_DATA wfd;
	bool rValue = false;
	HANDLE hFind = FindFirstFile(szPath, &wfd);
	if (INVALID_HANDLE_VALUE != hFind) rValue = true;
	FindClose(hFind);
	if (rValue)
		return true;
	else
	{
		strcpy(szPath, strFilePath);
		return (TRUE == DirectoryMake(szPath)) ? true : false;
	}
}

bool CheckDat(char* sDatBuff)
{
	int nLen = strlen(sDatBuff);
	if (nLen <= 0)
		return false;
	int i ;
	
	for (i=nLen-2; i>0; --i)
	{
		if ('\r' == sDatBuff[i] && '\n' == sDatBuff[i+1])
			break;
	}
	
	if (i <= 0)
		return false;
	
	i += 6;
	char* p = &sDatBuff[i];
	CString str = sDatBuff;
	str = MD5EncryptString(str.Left(i));
	
	if (0 == str.Compare(p))
		return true;
	else
		return false;
}

bool SafeMove(LPCSTR lpExistingFileName, LPCSTR lpNewFileName)
{
	if (MoveFileEx(lpExistingFileName, lpNewFileName, MOVEFILE_REPLACE_EXISTING))
		return true;
	else
	{
		char szMess[128];
		sprintf(szMess,"getlasterror:%d",GetLastError());
		print(szMess);
		char sBak[MAX_PATH];
		int n = strlen(lpNewFileName);
		strcpy(sBak, lpNewFileName);
		sBak[n-1] = 'k';
		sBak[n-2] = 'a';
		sBak[n-3] = 'b';
		if (MoveFileEx(lpNewFileName, sBak, MOVEFILE_REPLACE_EXISTING))
		{
			if (MoveFileEx(lpExistingFileName, lpNewFileName, MOVEFILE_REPLACE_EXISTING))
				return true;
		}
	}
	return false;
}


bool isFileExsit(LPTSTR strPath)
{
	DWORD ret = GetFileAttributes(strPath);
	if( ret == INVALID_FILE_ATTRIBUTES )
		return FALSE;	

	return TRUE;
}

// 删除目录
/*
bool DirectoryDelete(char* szDirectory)
{
	int nLen = strlen(szDirectory);
	if (nLen == 0)
	{
		return false;
	}


	if (szDirectory[nLen-1] != '\\' && szDirectory[nLen-1] != '/')
	{
		_tcscat(szDirectory , "\\");
	}


	CString strDirectoryFull;
	strDirectoryFull.Format("%s", szDirectory);

	CString strFiles = strDirectoryFull;
	strFiles += "*.*";

	WIN32_FIND_DATA wfd;
	HANDLE hFile = FindFirstFile(strFiles.GetString(), &wfd);
	bool bRet = TRUE;
	TCHAR szTemp[MAX_PATH];

	if (hFile != INVALID_HANDLE_VALUE)
	{
		do 
		{
			if (strcmp(wfd.cFileName , ".")!=0 && strcmp(wfd.cFileName , "..")!=0)
			{
				sprintf(szTemp,"%s%s", strDirectoryFull.GetBuffer() ,wfd.cFileName );

				if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					bRet = bRet && DirectoryDelete(szTemp);
				}
				else
				{
					bRet = bRet && DeleteFile(szTemp);
				}
			}
		} while (FindNextFile(hFile, &wfd));
		FindClose(hFile);
	}

	if (bRet)
	{
		bool bRemoveRet = RemoveDirectory(szDirectory);
		if (!bRemoveRet)
		{
			int nErr = GetLastError();
		}

		return bRemoveRet;
	}

	return false;
}
*/
bool CreateHashFile(char* szTempPath, vector<UPDATEDATA> &list)
{
	strcat(szTempPath, "UpdateHashList.dat");
	HANDLE m_hFile = CreateFile(szTempPath, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS , FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == m_hFile)
	{
		return false;
	}

	char szBuff[MAX_PATH * 2] = {0};

	vector<UPDATEDATA>::iterator ite;
	DWORD dwSize;

	for (ite = list.begin(); ite != list.end(); ++ite)
	{
		//移除update.exe
		if(stricmp((*ite).sFile, _T("update.exe")) == 0)
			continue;
		//
		sprintf(szBuff,"%s|%u\r\n",(*ite).sFile,(*ite).sHash);
		WriteFile(m_hFile, szBuff, strlen(szBuff), &dwSize, NULL);
	}	

	CloseHandle(m_hFile);

	return true;
}

bool CreateHashFile(char* szTempPath, vector<UPDATEDATA> &list, vector<UPDATEDATA> &locallist)
{
	strcat(szTempPath, "UpdateHashList.dat");
	HANDLE m_hFile = CreateFile(szTempPath, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS , FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == m_hFile)
	{
		return false;
	}

	char szBuff[MAX_PATH * 2] = {0};

	vector<UPDATEDATA>::iterator ite;
	DWORD dwSize;
	
	for (ite = list.begin(); ite != list.end(); ++ite)
	{
		//移除update.exe
		if(stricmp((*ite).sFile, _T("update.exe")) == 0)
			continue;
		//
		sprintf(szBuff,"%s|%u\r\n",(*ite).sFile,(*ite).sHash);
		WriteFile(m_hFile, szBuff, strlen(szBuff), &dwSize, NULL);
	}	
	
	for (ite = locallist.begin(); ite != locallist.end(); ++ite)
	{
		//移除update.exe
		if(stricmp((*ite).sFile, _T("update.exe")) == 0)
			continue;
		//
		sprintf(szBuff,"%s|%u\r\n",(*ite).sFile,(*ite).sHash);
		WriteFile(m_hFile, szBuff, strlen(szBuff), &dwSize, NULL);
	}
		
	CloseHandle(m_hFile);

	return true;
	
}

bool CreateHashFile( char* szTempPath, vector<UPDATEDATA> &list, vector<UPDATEDATA> &locallist, vector<string> &vecRedownload, vector<string> &vecCover )
{
	strcat(szTempPath, "UpdateHashList.dat");
	HANDLE m_hFile = CreateFile(szTempPath, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS , FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == m_hFile)
	{
		return false;
	}

	char szBuff[MAX_PATH * 2] = {0};

	vector<UPDATEDATA>::iterator ite;
	vector<string>::iterator ite1;
	DWORD dwSize;

	for (ite = list.begin(); ite != list.end(); ++ite)
	{
		//移除update.exe
		if(stricmp((*ite).sFile, _T("update.exe")) == 0)
			continue;
		//
		sprintf(szBuff,"%s|%u\r\n",(*ite).sFile,(*ite).sHash);
		WriteFile(m_hFile, szBuff, strlen(szBuff), &dwSize, NULL);
	}	

	for (ite = locallist.begin(); ite != locallist.end(); ++ite)
	{
		//移除update.exe
		if(stricmp((*ite).sFile, _T("update.exe")) == 0)
			continue;
		//
		sprintf(szBuff,"%s|%u\r\n",(*ite).sFile,(*ite).sHash);
		WriteFile(m_hFile, szBuff, strlen(szBuff), &dwSize, NULL);
	}

	for (ite1 = vecRedownload.begin(); ite1 != vecRedownload.end(); ++ite1)
	{
		//移除update.exe
		if(stricmp((*ite1).c_str(), _T("update.exe")) == 0)
			continue;
		//
		sprintf(szBuff,"%s|0\r\n",(*ite1).c_str());
		WriteFile(m_hFile, szBuff, strlen(szBuff), &dwSize, NULL);
	}

	for (ite1 = vecCover.begin(); ite1 != vecCover.end(); ++ite1)
	{
		//移除update.exe
		if(stricmp((*ite1).c_str(), _T("update.exe")) == 0)
			continue;
		//
		sprintf(szBuff,"%s|0\r\n",(*ite1).c_str());
		WriteFile(m_hFile, szBuff, strlen(szBuff), &dwSize, NULL);
	}

	CloseHandle(m_hFile);

	return true;
}


void ANSIToUnicode(char* szBuf,WCHAR *wszBuf)
{
	int  unicodeLen = ::MultiByteToWideChar( CP_ACP,
		0,
		szBuf,
		-1,
		NULL,
		0 );  
	wchar_t *  pUnicode;  
	pUnicode = new  wchar_t[unicodeLen+1];  
	memset(pUnicode,0,(unicodeLen+1)*sizeof(wchar_t));  

	::MultiByteToWideChar( CP_ACP,
		0,
		szBuf,
		-1,
		(LPWSTR)pUnicode,
		unicodeLen );  

	wcscpy(wszBuf,pUnicode);
	delete  pUnicode; 

}

void UnicodeToANSI( WCHAR *wszBuf, char* szBuf )
{
	// unicode --> ansi
	int ansiLen = ::WideCharToMultiByte(CP_ACP, NULL, wszBuf, wcslen(wszBuf), NULL, 0, NULL, NULL);
	char* pszAnsi = new char[ansiLen + 1];

	::WideCharToMultiByte(CP_ACP, NULL, wszBuf, wcslen(wszBuf), pszAnsi, ansiLen, NULL, NULL);
	pszAnsi[ansiLen] = '\0';

	strcpy(szBuf,pszAnsi);
	delete  pszAnsi; 
}


BOOL MakeDir(const char* pszDir)
{
	if(PathFileExists(pszDir) || strlen(pszDir) < 3)
	{
		return TRUE;
	}

	int i;
	char* p;
	BOOL bOK;

	p = (char*)malloc(strlen(pszDir)+1);
	strcpy(p,pszDir);
	i=3;
	while(p[i])
	{ 
		if(p[i]=='/'||p[i]=='\\')
		{
			p[i]=0;
			bOK = CreateDirectory(p,NULL);
			if(!bOK && GetLastError()!=ERROR_ALREADY_EXISTS)
			{
				p[i]='\\';
				free(p);
				return FALSE;
			}
			p[i]='\\';
		}
		i++;
	}
	free(p);
	return TRUE;
}

DWORD  CalcFileCRC(LPCTSTR lptcsPath)
{
	FILE* fp = NULL;
	_tfopen_s(&fp, lptcsPath, _T("rb"));
	if (!fp)
	{
		return 0;
	}

	fseek(fp, 0, SEEK_END);
	DWORD dwFileSize= ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char* lpBuffer = new char[dwFileSize];
	fread(lpBuffer, dwFileSize, 1, fp);

	DWORD dwCrc = CalcCRC(lpBuffer, dwFileSize);

	delete lpBuffer;
	fclose(fp);
	return dwCrc;
}

DWORD  CalcCRC(char* ptr, DWORD Size)
{
	static DWORD crcTable[256];
	static bool bTableIniFlag = false;
	if (!bTableIniFlag)
	{
		DWORD crcTmp1;
		for (int i=0; i<256; i++)
		{
			crcTmp1 = i;
			for (int j=8; j>0; j--)
			{
				if (crcTmp1&1) crcTmp1 = (crcTmp1 >> 1) ^ 0xEDB88320L;
				else crcTmp1 >>= 1;
			}

			crcTable[i] = crcTmp1;
		}
		bTableIniFlag = true;
	}


	DWORD dwRet = 0;

	DWORD crcTmp2 = 0xFFFFFFFF;
	while(Size--)
	{
		crcTmp2 = ((crcTmp2>>8) & 0x00FFFFFF) ^ crcTable[ (crcTmp2^(*ptr)) & 0xFF ];
		ptr++;
	}

	dwRet=(crcTmp2^0xFFFFFFFF);
	return dwRet;
}


LONGLONG GetNetFileSize(char* szUrl, PLONG plStop)
{
	LONGLONG lSize = 0;
	char szUrl1[URL_LEN_MAX];
	while (true)
	{
		int n = FindChar_f(szUrl, "\r\n");
		if (-1 == n)
			strcpy(szUrl1, szUrl);
		else
		{
			strncpy(szUrl1, szUrl, n);
			szUrl1[n] = '\0';
		}

		int nProtocol = JudgeProtocol(szUrl1);
		switch (nProtocol)
		{
		case DOWN_TYPE_HTTP:	lSize += HttpSize(szUrl1); break;
		default: print("JudgeProtocol null\n"); break;
		}

		if (-1 == n) break;
		szUrl = szUrl + n + 2;
		if (0 == strlen(szUrl)) break;
	}
	return lSize;
}

// 取完整文件名
string GetFileName(const string& strFileName)
{
	basic_string <char>::size_type indexCh = strFileName.rfind('\\');
	static const basic_string <char>::size_type npos = -1;

	if (indexCh == npos)
	{
		indexCh = strFileName.rfind('/');
	}
	if (indexCh == npos)
	{
		return "";
	}

	return strFileName.substr(indexCh+1, strFileName.length()-1-indexCh);
}

string StrReplace(string s, string oldSub, string newSub)
{
	if (( s == "")||( oldSub == "") ) return s;
	string s2 = "";
	unsigned int n_old = oldSub.size();
	int k1, k0 = 0;
	k1 = s.find( oldSub , k0 );
	while( k1 >=0 )
	{
		s2 += s.substr( k0,k1-k0);
		s2 += newSub ;
		k0 = k1 + n_old;
		k1 = s.find( oldSub , k0 );

	}
	s2 += s.substr( k0,s.size() - k0 );
	return s2 ;
}

bool DirectoryDelete(const string& strDirectory)
{
	//	return (boost::filesystem::remove_all(strDirectory)==0?false:true);

	if (strDirectory.empty())
	{
		return false;
	}

	int nLen = strDirectory.length();
	string strDirectoryFull = strDirectory;

	if (strDirectoryFull[strDirectoryFull.length()-1] != '\\' && strDirectoryFull[strDirectoryFull.length()-1] != '/')
	{
		strDirectoryFull += '\\';
	}

	string strFiles = strDirectoryFull + "*.*";
	WIN32_FIND_DATA wfd;
	HANDLE hFile = FindFirstFile(strFiles.c_str(), &wfd);
	bool bRet = TRUE;
	string strTemp;

	if (hFile != INVALID_HANDLE_VALUE)
	{
		do 
		{
			if (strcmp(wfd.cFileName , ".")!=0 && strcmp(wfd.cFileName , "..")!=0)
			{
				strTemp = strDirectoryFull + wfd.cFileName;

				if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					bRet = bRet && DirectoryDelete(strTemp);
				}
				else
				{
					SetFileAttributes(strTemp.c_str(), FILE_ATTRIBUTE_NORMAL);
					bRet = bRet && DeleteFile(strTemp.c_str());
				}
			}
		} while (FindNextFile(hFile, &wfd));
		FindClose(hFile);
	}

	if (bRet)
	{
		bool bRemoveRet = RemoveDirectory(strDirectory.c_str());
		if (!bRemoveRet)
		{
			int nErr = GetLastError();
		}

		return bRemoveRet;
	}

	return false;
}

// 取文件名
string GetFileTitle(const string& strFileName)
{
	basic_string <char>::size_type indexCh = strFileName.rfind('\\');
	static const basic_string <char>::size_type npos = -1;

	if (indexCh == npos)
	{
		indexCh = strFileName.rfind('/');
	}
	if (indexCh == npos)
	{
		return "";
	}

	string strName = strFileName.substr(indexCh+1, strFileName.length()-1-indexCh);

	indexCh = strName.rfind('.');
	if (indexCh != npos)
	{
		strName = strName.substr(0, indexCh);
	}

	return strName;
}

bool IsFileExist(const string& strFileName)
{
	HANDLE hFile = CreateFile(strFileName.c_str(),0,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		return true;
	}
	else
	{
		return false;
	}
}

// 判断是否可以读该文件
bool FileReadAllow(const string& strFileName)
{
	FILE* fp = fopen(strFileName.c_str(),"rb");
	if(NULL == fp)
	{
		return false;
	}
	fclose(fp);
	return true;
}


// 获取文件大小
unsigned __int64 GetFileSize(const string& strFileName)
{
	if (!FileReadAllow(strFileName) || IsDir(strFileName))
	{
		return 0;
	}

	WIN32_FIND_DATA wfd;

	ZeroMemory(&wfd, sizeof(wfd));
	HANDLE hFile = FindFirstFile(strFileName.c_str(), &wfd);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	DWORD64 dw64SizeH = (DWORD64) wfd.nFileSizeHigh;
	DWORD64 dw64Size = dw64SizeH << 32;
	dw64Size += (DWORD64)wfd.nFileSizeLow;

	FindClose(hFile);

	return dw64Size;
}