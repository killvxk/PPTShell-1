// Net.cpp: implementation of the CNet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Net.h"
#include "AutoUpdate.h"
#include "UpdateDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNet::CNet()
{
	WSADATA ws;
	if (0 == WSAStartup(MAKEWORD(1,1), &ws))
		m_bInit = true;
	else
		m_bInit = false;

	m_hTempFile = NULL;
	m_hConfFile = NULL;
}

CNet::~CNet()
{
	if (m_bInit)
		WSACleanup();
}


void CNet::SetDownloadSize( HANDLE hFile, LONGLONG& lSize, LONGLONG& lDownloadSize)
{
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return;
	}

	DWORD dwWriteSize = 0;

	LONGLONG lDownloadInfo[2];
	lDownloadInfo[0] = lSize;
	lDownloadInfo[1] = lDownloadSize;
	SetFilePointer(hFile, 0, 0, FILE_BEGIN);
	WriteFile(hFile, &lDownloadInfo, sizeof(lDownloadInfo), &dwWriteSize, NULL);
}


BOOL CNet::DownData( char* szUrl, char* szPath, char* szName, char** ppBuff/*=NULL*/, void* pWnd/*=NULL*/ )
{
	SOCKET conn = NULL;

	bool bComplete = false;

	if (!m_bInit) { goto _Out;}
	if (NULL == szUrl || NULL == szPath || 0 == strlen(szPath)) { goto _Out;}
	if (DOWN_TYPE_HTTP != JudgeProtocol(szUrl)) { goto _Out;}

	LONG lSize = HttpSize(szUrl);
	if (-1 == lSize) { goto _Out;}

	LONGLONG lDownloadSize = 0;

	char szServ[URL_LEN_MAX];
	char szPath1[URL_LEN_MAX];
	int  nPort;
	if (-1 == SplitHttpUrl(szUrl, szServ, nPort, szPath1)) { goto _Out;}

	if (NULL == ppBuff)
		if (!JudgeDir(szPath)) { goto _Out;}

		char szFile[MAX_PATH];
		strcpy(szFile, szPath);
		strcat(szFile, szName);

		int nRecLen = 0;
		char sTmp[1024];
		char l_szUrl[URL_LEN_MAX];

		if (0 != ConnWithTime(conn, szServ, nPort)) goto _Out;

		int nAckStatus ;
		while (true)
		{
			nAckStatus = 0;
			HttpSend(conn, szServ, szPath1);
			nRecLen = recv(conn, sTmp, 512, 0);

			if (0 >= nRecLen) { goto _Out;}
			int n = FindChar_f(sTmp, "HTTP/");
			if (-1 == n) { goto _Out;}

			char sTmp1[64];
			strncpy(sTmp1, sTmp+9, 3);
			sTmp1[3] = '\0';
			nAckStatus = atoi(sTmp1);

			if (301 == nAckStatus)
			{
				closesocket(conn);
				conn = NULL;
				n = FindChar_f(sTmp, "Location: ");
				if (-1 == n) goto _Out;
				int m = FindChar_f(sTmp+n, "\r\n");
				if (-1 == m && m > 10) goto _Out;;
				m-=10;
				strncpy(l_szUrl, sTmp+n+10, m);
				l_szUrl[m] = '\0';
				if (-1 == SplitHttpUrl(l_szUrl, szServ, nPort, szPath1)) goto _Out;;
				if (0 != ConnWithTime(conn, szServ, nPort)) goto _Out;;
				continue;
			}
			else if (nAckStatus<200 || nAckStatus>=300)
			{ goto _Out;}

			break;
		}

		int n = FindChar_n(sTmp, "\r\n\r\n", 512);
		if (-1 == n)
		{
			nRecLen += recv(conn, sTmp+512, 512, 0);
			n = FindChar_n(sTmp, "\r\n\r\n", 1024);
			if (-1 == n) { goto _Out;}
		}
		n += 4;

		char* szBuf = new char[lSize+1];
		if (NULL == szBuf) { goto _Out;}

		
		DWORD dwWriteSize;

		nRecLen = nRecLen-n;
		if(nRecLen > 0)
		{
			memcpy(szBuf, &sTmp[n], nRecLen);
			lDownloadSize += nRecLen;
		}

		DWORD dwLastTime = GetTickCount();
		DWORD dwElapseTime;
		DWORD dwPregressSize = 0;
		while (lDownloadSize < lSize && 0 < (nRecLen = recv(conn, &szBuf[lDownloadSize], lSize - lDownloadSize, 0)))
		{
			lDownloadSize += nRecLen;
		}

		if (lDownloadSize == lSize)
		{
			bComplete = true;
			szBuf[lSize] = '\0';
			(*ppBuff) = szBuf;
		}

_Out:
		if(conn)
			closesocket(conn); 

		return (bComplete) ? TRUE : FALSE;
}

#define SIZE_200K 200*1024
BOOL CNet::DownFile(char* szUrl, char* szPath, char* szName, char** ppBuff, void* pWnd) 
{
	SOCKET conn = NULL;
	char	szBuf[SIZE_200K];
	bool bComplete = false;

	char szSrcFileName[MAX_PATH];
	_snprintf(szSrcFileName, sizeof(szSrcFileName)-1, "%s%s", szPath, szName);

	char szTempFileName[MAX_PATH];
	_snprintf(szTempFileName, sizeof(szTempFileName)-1, "%s%s.nd!", szPath, szName);//.tq! --temp file

	MakeDir(szTempFileName);
	m_hTempFile = CreateFile(szTempFileName, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == m_hTempFile)
	{
		goto _Out;
	}

	char szConfFileName[MAX_PATH];
	_snprintf(szConfFileName, sizeof(szConfFileName)-1, "%s%s.nd$", szPath, szName);//.tq$ --config

	m_hConfFile = CreateFile(szConfFileName, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == m_hConfFile)
	{
		goto _Out;
	}

	//////////////////////////////////////////////////////////////////////////
	CUpdateDlg* pDlg = (CUpdateDlg*)pWnd;
	if (!m_bInit) { goto _Out;}
	if (NULL == szUrl || NULL == szPath || 0 == strlen(szPath)) { goto _Out;}
	if (DOWN_TYPE_HTTP != JudgeProtocol(szUrl)) { goto _Out;}

	LONGLONG lSize = HttpSize(szUrl);
	if (-1 == lSize) { goto _Out;}

	if(pDlg)
		pDlg->m_updateParam.dwDownloadFileSize = lSize;

//////////////////////////////////////////////////////////////////////////
	LONGLONG lConfFileSize = 0;
	LONGLONG lConfDownloadSize = 0;
	DWORD dwReadSize = 0;

	ReadFile(m_hConfFile, &lConfFileSize, sizeof(LONGLONG), &dwReadSize, NULL);//打开配置文件
	if (dwReadSize == sizeof(LONGLONG) && lConfFileSize != lSize)//获取大小
	{
		CloseHandle(m_hConfFile);
		m_hConfFile = CreateFile(szConfFileName, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == m_hConfFile)
		{
			m_hConfFile = NULL;
			goto _Out;
		}
	}
	else if(dwReadSize > 0)
	{
		ReadFile(m_hConfFile, &lConfDownloadSize, sizeof(LONGLONG), &dwReadSize, NULL);//打开配置文件
	}
//////////////////////////////////////////////////////////////////////////
	char szServ[URL_LEN_MAX];
	char szPath1[URL_LEN_MAX];
	int  nPort;
	if (-1 == SplitHttpUrl(szUrl, szServ, nPort, szPath1)) { goto _Out;}

	if (NULL == ppBuff)
		if (!JudgeDir(szPath)) { goto _Out;}

	char szFile[MAX_PATH];
	strcpy(szFile, szPath);
	strcat(szFile, szName);
	
	int nRecLen = 0;
	char sTmp[1024];
	char l_szUrl[URL_LEN_MAX];

	if (0 != ConnWithTime(conn, szServ, nPort)) goto _Out;

	int nAckStatus ;
	while (true)
	{
		nAckStatus = 0;
		HttpSend(conn, szServ, szPath1 , lConfDownloadSize, lSize-lConfDownloadSize);
		nRecLen = recv(conn, sTmp, 512, 0);

		if (0 >= nRecLen) { goto _Out;}
		int n = FindChar_f(sTmp, "HTTP/");
		if (-1 == n) { goto _Out;}

		char sTmp1[64];
		strncpy(sTmp1, sTmp+9, 3);
		sTmp1[3] = '\0';
		nAckStatus = atoi(sTmp1);

		if (301 == nAckStatus)
		{
			closesocket(conn);
			conn = NULL;
			n = FindChar_f(sTmp, "Location: ");
			if (-1 == n) goto _Out;
			int m = FindChar_f(sTmp+n, "\r\n");
			if (-1 == m && m > 10) goto _Out;;
			m-=10;
			strncpy(l_szUrl, sTmp+n+10, m);
			l_szUrl[m] = '\0';
			if (-1 == SplitHttpUrl(l_szUrl, szServ, nPort, szPath1)) goto _Out;;
			if (0 != ConnWithTime(conn, szServ, nPort)) goto _Out;;
			continue;
		}
		else if (nAckStatus<200 || nAckStatus>=300)
		{ goto _Out;}

		break;
	}

	if(nAckStatus != 206)//不支持
	{
		lConfDownloadSize = 0;
	}

	int n = FindChar_n(sTmp, "\r\n\r\n", 512);
	if (-1 == n)
	{
		nRecLen += recv(conn, sTmp+512, 512, 0);
		n = FindChar_n(sTmp, "\r\n\r\n", 1024);
		if (-1 == n) { goto _Out;}
	}
	n += 4;

// 	char* szBuf = new char[lSize+1];
// 	if (NULL == szBuf) { goto _Out;}
// 	memcpy(szBuf, &sTmp[n], nRecLen-n);
	DWORD dwWriteSize;

 	nRecLen = nRecLen-n;
	if(nRecLen > 0)
	{
		HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		OVERLAPPED ov = {0, 0, (DWORD)lConfDownloadSize, 0, hEvent};

		if (!WriteFile(m_hTempFile, sTmp + n, nRecLen, &dwWriteSize, &ov))
			GetOverlappedResult(m_hTempFile, &ov, &dwWriteSize, TRUE);
		CloseHandle(hEvent);

		if (nRecLen != dwWriteSize)
		{
			goto _Out;
		}

		lConfDownloadSize += nRecLen;
		FlushFileBuffers(m_hTempFile);
		SetDownloadSize(m_hConfFile, lSize, lConfDownloadSize);
	}

	DWORD dwLastTime = GetTickCount();
	DWORD dwElapseTime;
	DWORD dwPregressSize = 0;
	while (lConfDownloadSize < lSize && 0 < (nRecLen = recv(conn, szBuf, sizeof(szBuf), 0)))
	{
		HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		OVERLAPPED ov = {0, 0, (DWORD)lConfDownloadSize, 0, hEvent};

		if (!WriteFile(m_hTempFile, szBuf, nRecLen, &dwWriteSize, &ov))
			GetOverlappedResult(m_hTempFile, &ov, &dwWriteSize, TRUE);
		CloseHandle(hEvent);

		if (nRecLen != dwWriteSize)
		{
			goto _Out;
		}

		lConfDownloadSize += nRecLen;
		FlushFileBuffers(m_hTempFile);
		SetDownloadSize(m_hConfFile, lSize, lConfDownloadSize);

		dwPregressSize += nRecLen;
		if(pDlg)
		{
			if (GetTickCount() - dwLastTime >= 1000)
			{
				pDlg->m_updateParam.dwDownloadingSize = lConfDownloadSize;
				pDlg->m_updateParam.updateInfoParam.dwCurrentProgress = ((float)pDlg->m_updateParam.dwDownloadingSize / pDlg->m_updateParam.dwDownloadFileSize) * 100;
				pDlg->m_updateParam.updateInfoParam.dwTotalProgress = pDlg->m_updateParam.dwPreTotalProgress + 
					((float)pDlg->m_updateParam.dwDownloadingSize / pDlg->m_updateParam.dwDownloadFileSize) *
					( ((float)1 / pDlg->m_updateParam.dwDownloadCount ) * 100);

				dwElapseTime = GetTickCount() - dwLastTime;
				pDlg->m_updateParam.updateInfoParam.fDownloadSpeed = (float)dwPregressSize / (dwElapseTime / 1000.0f);
				dwLastTime = GetTickCount();
				dwPregressSize = 0;
			}
			if(g_pUpdateBuff)
				memcpy(g_pUpdateBuff,&pDlg->m_updateParam.updateInfoParam,sizeof(UpdateInfo_Param));
		}
	}

	if (lConfDownloadSize == lSize)
	{
		bComplete = true;
	}
	
_Out:
	if(conn)
		closesocket(conn); 

	if(m_hConfFile)
	{
		CloseHandle(m_hConfFile);
		m_hConfFile = NULL;
	}
	if(m_hTempFile)
	{
		CloseHandle(m_hTempFile);
		m_hTempFile = NULL;
	}
	if(bComplete)
	{		
		DeleteFile(szConfFileName);
		MakeDir(szSrcFileName);
		MoveFileEx(szTempFileName, szSrcFileName, MOVEFILE_COPY_ALLOWED|MOVEFILE_REPLACE_EXISTING);
	}
	return (bComplete) ? TRUE : FALSE;
}

BOOL CNet::DownFile( char* szUrl, char* szPath, void* pWnd/*=NULL*/ )
{
	SOCKET conn = NULL;
	char	szBuf[SIZE_200K];


	char szSrcFileName[MAX_PATH];
	_snprintf(szSrcFileName, sizeof(szSrcFileName)-1, "%s", szPath);

	char szTempFileName[MAX_PATH];
	_snprintf(szTempFileName, sizeof(szTempFileName)-1, "%s.nd!", szPath);//.tq! --temp file

	MakeDir(szTempFileName);
	m_hTempFile = CreateFile(szTempFileName, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == m_hTempFile)
	{
		goto _Out;
	}

	char szConfFileName[MAX_PATH];
	_snprintf(szConfFileName, sizeof(szConfFileName)-1, "%s.nd$", szPath);//.tq$ --config

	m_hConfFile = CreateFile(szConfFileName, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == m_hConfFile)
	{
		goto _Out;
	}

	//////////////////////////////////////////////////////////////////////////
	CUpdateDlg* pDlg = (CUpdateDlg*)pWnd;
	if (!m_bInit) { goto _Out;}
	if (NULL == szUrl || NULL == szPath || 0 == strlen(szPath)) { goto _Out;}
	if (DOWN_TYPE_HTTP != JudgeProtocol(szUrl)) { goto _Out;}

	LONGLONG lSize = HttpSize(szUrl);
	if (-1 == lSize) { goto _Out;}

	if(pDlg)
		pDlg->m_updateParam.dwDownloadFileSize = lSize;

	//////////////////////////////////////////////////////////////////////////
	LONGLONG lConfFileSize = 0;
	LONGLONG lConfDownloadSize = 0;
	DWORD dwReadSize = 0;

	ReadFile(m_hConfFile, &lConfFileSize, sizeof(LONGLONG), &dwReadSize, NULL);//打开配置文件
	if (dwReadSize == sizeof(LONGLONG) && lConfFileSize != lSize)//获取大小
	{
		CloseHandle(m_hConfFile);
		m_hConfFile = CreateFile(szConfFileName, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == m_hConfFile)
		{
			m_hConfFile = NULL;
			goto _Out;
		}
	}
	else if(dwReadSize > 0)
	{
		ReadFile(m_hConfFile, &lConfDownloadSize, sizeof(LONGLONG), &dwReadSize, NULL);//打开配置文件
	}
	//////////////////////////////////////////////////////////////////////////
	char szServ[URL_LEN_MAX];
	char szPath1[URL_LEN_MAX];
	int  nPort;
	if (-1 == SplitHttpUrl(szUrl, szServ, nPort, szPath1)) { goto _Out;}

	char szFile[MAX_PATH];
	strcpy(szFile, szPath);

	int nRecLen = 0;
	char sTmp[1024];
	char l_szUrl[URL_LEN_MAX];

	if (0 != ConnWithTime(conn, szServ, nPort)) goto _Out;

	int nAckStatus ;
	while (true)
	{
		nAckStatus = 0;
		HttpSend(conn, szServ, szPath1 , lConfDownloadSize, lSize-lConfDownloadSize);
		nRecLen = recv(conn, sTmp, 512, 0);

		if (0 >= nRecLen) { goto _Out;}
		int n = FindChar_f(sTmp, "HTTP/");
		if (-1 == n) { goto _Out;}

		char sTmp1[64];
		strncpy(sTmp1, sTmp+9, 3);
		sTmp1[3] = '\0';
		nAckStatus = atoi(sTmp1);

		if (301 == nAckStatus)
		{
			closesocket(conn);
			conn = NULL;
			n = FindChar_f(sTmp, "Location: ");
			if (-1 == n) goto _Out;
			int m = FindChar_f(sTmp+n, "\r\n");
			if (-1 == m && m > 10) goto _Out;;
			m-=10;
			strncpy(l_szUrl, sTmp+n+10, m);
			l_szUrl[m] = '\0';
			if (-1 == SplitHttpUrl(l_szUrl, szServ, nPort, szPath1)) goto _Out;;
			if (0 != ConnWithTime(conn, szServ, nPort)) goto _Out;;
			continue;
		}
		else if (nAckStatus<200 || nAckStatus>=300)
		{ goto _Out;}

		break;
	}

	if(nAckStatus != 206)//不支持
	{
		lConfDownloadSize = 0;
	}

	int n = FindChar_n(sTmp, "\r\n\r\n", 512);
	if (-1 == n)
	{
		nRecLen += recv(conn, sTmp+512, 512, 0);
		n = FindChar_n(sTmp, "\r\n\r\n", 1024);
		if (-1 == n) { goto _Out;}
	}
	n += 4;

	// 	char* szBuf = new char[lSize+1];
	// 	if (NULL == szBuf) { goto _Out;}
	// 	memcpy(szBuf, &sTmp[n], nRecLen-n);
	bool bComplete = false;
	DWORD dwWriteSize;

	nRecLen = nRecLen-n;
	if(nRecLen > 0)
	{
		HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		OVERLAPPED ov = {0, 0, (DWORD)lConfDownloadSize, 0, hEvent};

		if (!WriteFile(m_hTempFile, sTmp + n, nRecLen, &dwWriteSize, &ov))
			GetOverlappedResult(m_hTempFile, &ov, &dwWriteSize, TRUE);
		CloseHandle(hEvent);

		if (nRecLen != dwWriteSize)
		{
			goto _Out;
		}

		lConfDownloadSize += nRecLen;
		FlushFileBuffers(m_hTempFile);
		SetDownloadSize(m_hConfFile, lSize, lConfDownloadSize);
	}

	DWORD dwLastTime = GetTickCount();
	DWORD dwElapseTime;
	DWORD dwPregressSize = 0;
	while (lConfDownloadSize < lSize && 0 < (nRecLen = recv(conn, szBuf, sizeof(szBuf), 0)))
	{
		HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		OVERLAPPED ov = {0, 0, (DWORD)lConfDownloadSize, 0, hEvent};

		if (!WriteFile(m_hTempFile, szBuf, nRecLen, &dwWriteSize, &ov))
			GetOverlappedResult(m_hTempFile, &ov, &dwWriteSize, TRUE);
		CloseHandle(hEvent);

		if (nRecLen != dwWriteSize)
		{
			goto _Out;
		}

		lConfDownloadSize += nRecLen;
		FlushFileBuffers(m_hTempFile);
		SetDownloadSize(m_hConfFile, lSize, lConfDownloadSize);

		dwPregressSize += nRecLen;
		if(pDlg)
		{
			if (GetTickCount() - dwLastTime >= 1000)
			{
				pDlg->m_updateParam.dwDownloadingSize = lConfDownloadSize;
				pDlg->m_updateParam.updateInfoParam.dwCurrentProgress = ((float)pDlg->m_updateParam.dwDownloadingSize / pDlg->m_updateParam.dwDownloadFileSize) * 100;
				pDlg->m_updateParam.updateInfoParam.dwTotalProgress = pDlg->m_updateParam.dwPreTotalProgress + 
					((float)pDlg->m_updateParam.dwDownloadingSize / pDlg->m_updateParam.dwDownloadFileSize) *
					( ((float)1 / pDlg->m_updateParam.dwDownloadCount ) * 100);

				dwElapseTime = GetTickCount() - dwLastTime;
				pDlg->m_updateParam.updateInfoParam.fDownloadSpeed = (float)dwPregressSize / (dwElapseTime / 1000.0f);
				dwLastTime = GetTickCount();
				dwPregressSize = 0;
			}
			if(g_pUpdateBuff)
				memcpy(g_pUpdateBuff,&pDlg->m_updateParam.updateInfoParam,sizeof(UpdateInfo_Param));
		}
	}

	if (lConfDownloadSize == lSize)
	{
		bComplete = true;
	}

_Out:
	if(conn)
		closesocket(conn); 

	if(m_hConfFile)
	{
		CloseHandle(m_hConfFile);
		m_hConfFile = NULL;
	}
	if(m_hTempFile)
	{
		CloseHandle(m_hTempFile);
		m_hTempFile = NULL;
	}
	if(bComplete)
	{		
		DeleteFile(szConfFileName);
		MakeDir(szSrcFileName);
		MoveFileEx(szTempFileName, szSrcFileName, MOVEFILE_COPY_ALLOWED|MOVEFILE_REPLACE_EXISTING);
	}
	return (bComplete) ? TRUE : FALSE;
}

LONG CNet::GetNetFileSize(char* szUrl) 
{
	if (!m_bInit) { print("no Init\n"); return -1;}
	LONG lSize = 0;
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
		
		if (DOWN_TYPE_HTTP != JudgeProtocol(szUrl1))
			break;
		lSize += HttpSize(szUrl1);
		
		if (-1 == n) break;
		szUrl = szUrl + n + 2;
		if (0 == strlen(szUrl)) break;
	}
	return lSize;
}
