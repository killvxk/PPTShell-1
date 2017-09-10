//-----------------------------------------------------------------------
// FileName:				HttpDownload.cpp
//
// Desc:
//------------------------------------------------------------------------
#include "stdafx.h"
#include "HttpDelegate.h"
#include "HttpDownload.h"
#include "Util\Stream.h"

#pragma comment(lib, "wininet.lib")

CHttpDownloadTask::CHttpDownloadTask(LPCTSTR lpcstrHost, 
									 LPCTSTR lpcstrUrl, 
								     LPCTSTR lpcstrHeader,
									 LPCTSTR lpcstrMethod, 
									 LPCTSTR lpcstrPost,
									 int nPort, 
									 CHttpDelegateBase& OnCompleteDelegate,
									 CHttpDelegateBase& OnProgressDelegate,
									 CHttpDelegateBase& OnStartDelegate,
									 PVOID pUserData /*= NULL*/)
{
	m_strHost			= lpcstrHost;
	m_strUrl			= lpcstrUrl;
	m_strHeader			= lpcstrHeader;
	m_strMethod			= lpcstrMethod;
	m_strPost			= lpcstrPost;
	m_nPort				= nPort;
	m_bDownloading		= FALSE;
	m_bPaused			= FALSE;
	m_bDownloadFile		= FALSE;
	m_hOwnerWnd			= NULL;
	m_dwFileStartPos	= 0;
	m_nPriority			= HIGHEST_DOWNLOAD_PRIORITY;

	InitializeCriticalSection(&m_Lock);

	if( OnCompleteDelegate.GetFn() != NULL )
		OnComplete += OnCompleteDelegate;

	if( OnProgressDelegate.GetFn() != NULL )
		OnProgress += OnProgressDelegate;	

	if( OnStartDelegate.GetFn() != NULL )
		OnStart += OnStartDelegate;

	// user data
	DWORD dwKey = (DWORD)OnCompleteDelegate.GetFn() | (DWORD)OnCompleteDelegate.GetObject();
	m_mapOnCompleteUserDatas[dwKey] = pUserData;

	dwKey = (DWORD)OnProgressDelegate.GetFn() | (DWORD)OnProgressDelegate.GetObject();
	m_mapOnProgressUserDatas[dwKey] = pUserData;

	dwKey = (DWORD)OnStartDelegate.GetFn() | (DWORD)OnStartDelegate.GetObject();
	m_mapOnStartUserDatas[dwKey] = pUserData;

}

CHttpDownloadTask::~CHttpDownloadTask()
{
	DeleteCriticalSection(&m_Lock);
}

void CHttpDownloadTask::SetOwner(HWND hWnd)
{
	m_hOwnerWnd = hWnd;
}

void CHttpDownloadTask::SetTaskId(DWORD dwTaskId)
{
	m_dwTaskId = dwTaskId;
}

DWORD CHttpDownloadTask::GetTaskId()
{
	return	m_dwTaskId;
}

BOOL CHttpDownloadTask::IsDownloading()
{
	BOOL res = FALSE;

	EnterCriticalSection(&m_Lock);
	res = m_bDownloading;
	LeaveCriticalSection(&m_Lock);

	return res;
}

void CHttpDownloadTask::SetDownloadFile(BOOL bDownloadFile, DWORD dwFileStartPos /*= 0*/)
{
	m_bDownloadFile = bDownloadFile;
	m_dwFileStartPos = dwFileStartPos;

	// Range: bytes=
	if( dwFileStartPos != 0 )
	{
		TCHAR szHeader[MAX_PATH];
		_stprintf_s(szHeader, _T("Range: bytes=%d"), dwFileStartPos);

		m_strHttpHeader = szHeader;
	}
	else
		m_strHttpHeader = _T("");

}

BOOL CHttpDownloadTask::IsDownloadFile()
{
	return  m_bDownloadFile;
}

BOOL CHttpDownloadTask::IsPaused()
{
	return m_bPaused;
}

void CHttpDownloadTask::SetPriority(int nPriority)
{
	m_nPriority = nPriority;
}

int CHttpDownloadTask::GetPriority()
{
	return m_nPriority;
}

BOOL CHttpDownloadTask::Start()
{
	EnterCriticalSection(&m_Lock);
	m_bDownloading =  TRUE;
	LeaveCriticalSection(&m_Lock);

	if( m_bDownloadFile )
		GetHttpFile(m_strHost, m_strUrl, m_nPort, m_strMethod, m_strPost);
	else
		GetHttpData(m_strHost, m_strUrl, m_nPort, m_strMethod, m_strPost);

	return TRUE;
}

BOOL CHttpDownloadTask::Stop()
{
	EnterCriticalSection(&m_Lock);
	m_bDownloading = FALSE;
	LeaveCriticalSection(&m_Lock);

	OnTempEventSource.clear();
	OnComplete.clear();
	OnProgress.clear();
	OnStart.clear();

	return TRUE;
}

BOOL CHttpDownloadTask::Pause( CHttpDelegateBase* pOnPauseDelegate /*= NULL*/)
{
	EnterCriticalSection(&m_Lock);
	m_bPaused = TRUE;

	if( pOnPauseDelegate != NULL && pOnPauseDelegate->GetFn() != NULL )
		OnPause += *pOnPauseDelegate;

	LeaveCriticalSection(&m_Lock);

	return TRUE;
}

BOOL CHttpDownloadTask::Resume()
{
	EnterCriticalSection(&m_Lock);
	m_bPaused = FALSE;
	LeaveCriticalSection(&m_Lock);

	return TRUE;
}

void CHttpDownloadTask::AddTaskCallBack(CHttpDelegateBase &OnCompleteDelegate, CHttpDelegateBase &OnProgressDelegate)
{
	if( OnCompleteDelegate.GetFn() != NULL )
		OnComplete += OnCompleteDelegate;

	if( OnProgressDelegate.GetFn() != NULL )
		OnProgress += OnProgressDelegate;	

}

//--------------------------------------------------------------------------------------------------
// get http data
// 
void CHttpDownloadTask::GetHttpData(tstring host, tstring url, int nPort, tstring method, tstring post)
{
	HINTERNET hInternet		= NULL;
	HINTERNET hConnection	= NULL;
	HINTERNET hRequest		= NULL;
	DWORD	  dwErrorCode	= 0;

	// allocate buffer
	int nBufferSize = 0x2000;
	int nDataSize	= 0;
	char* pBuffer	= (char*)malloc(nBufferSize);


	//
	tstring header = m_strHeader; 

	TCHAR szAgent[]= _T("Mozilla/4.0 (compatible; MSIE 6.0; MS Web Services Client Protocol 2.0.50727.3655)");
	hInternet = InternetOpen(szAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

	hConnection = InternetConnect(hInternet, host.c_str(), nPort, 0, 0, INTERNET_SERVICE_HTTP, 0, 0); 
	if(!hConnection)
	{
		dwErrorCode = 1;
		goto _OUT;
	}

	// temp solution!
	DWORD dwFlags = INTERNET_FLAG_KEEP_CONNECTION|INTERNET_FLAG_DONT_CACHE;
	if( nPort == INTERNET_DEFAULT_HTTPS_PORT )
		dwFlags |= INTERNET_FLAG_SECURE;

	hRequest = ::HttpOpenRequest(hConnection, method.c_str(), url.c_str(), 0, 0, 0, dwFlags, 0);
	if(!hRequest)
	{
		dwErrorCode = 2;
		goto _OUT;
	}

	if( method == _T("POST") && header == _T("") )
		header += "Content-Type: application/x-www-form-urlencoded; charset=UTF-8";

	BOOL res = HttpSendRequest(hRequest, header.c_str(), header.length(), (LPVOID)post.c_str(), post.length());
	if( !res )
	{
		dwErrorCode = 3;
		goto _OUT;
	}

	int nTotalDataSize = 0;
	DWORD dwSize = 4;
	HttpQueryInfo(hRequest, HTTP_QUERY_CONTENT_LENGTH|HTTP_QUERY_FLAG_NUMBER, (LPDWORD)&nTotalDataSize, &dwSize, NULL);


	DWORD BytesWrite = 0;
	while( m_bDownloading && !m_bPaused )
	{
		BytesWrite = 0;

		if( nDataSize >= nBufferSize )
		{
			nBufferSize *= 2;
			char* pTemBuf = (char*)realloc(pBuffer, nBufferSize);
			if( pTemBuf == NULL )
			{
				free(pBuffer);
				break;
			}

			pBuffer = pTemBuf;
		}

		res = InternetReadFile(hRequest, pBuffer+nDataSize, nBufferSize - nDataSize, &BytesWrite);
		if( !res )
			break;

		if( BytesWrite == 0 )
			break;

		nDataSize += BytesWrite;

		// OnProgress
		if( nTotalDataSize > 0 && OnProgress )
		{
			THttpNotify ProgressNotify;
			ProgressNotify.dwTaskId		= m_dwTaskId;
			ProgressNotify.pData		= pBuffer;
			ProgressNotify.nDataSize	= nDataSize;
			ProgressNotify.fPercent		= (float)nDataSize / (float)nTotalDataSize;
			
			CallDelegate(&OnProgress, &m_mapOnProgressUserDatas, &ProgressNotify);
		}
	}

	
_OUT:
	InternetCloseHandle(hRequest);
	InternetCloseHandle(hConnection);
	InternetCloseHandle(hInternet);

	// OnComplete
	THttpNotify CompleteNotify;
	CompleteNotify.dwErrorCode	= dwErrorCode;
	CompleteNotify.dwTaskId		= m_dwTaskId;
	CompleteNotify.pData		= pBuffer;
	CompleteNotify.nDataSize	= nDataSize;
		
	if( nTotalDataSize > 0 )
	{
		if( nDataSize != nTotalDataSize )
			CompleteNotify.dwErrorCode = 4;

		CompleteNotify.fPercent	= (float)nDataSize / (float)nTotalDataSize;
	}
	else
		CompleteNotify.fPercent		= 1.0f;

	// don't callback if it is paused
	if( !m_bPaused )
		CallDelegate(&OnComplete, &m_mapOnCompleteUserDatas, &CompleteNotify);
	
	free(CompleteNotify.pData);
}


//--------------------------------------------------------------------------------------------------
// get http file
// 
void CHttpDownloadTask::GetHttpFile(tstring host, tstring url, int nPort, tstring method, tstring post)
{
	HINTERNET hInternet		= NULL;
	HINTERNET hConnection	= NULL;
	HINTERNET hRequest		= NULL;
	DWORD	  dwErrorCode	= 0;
	THttpNotify StartNotify;

	//
	string header = ""; 

	char szAgent[]= "Mozilla/4.0 (compatible; MSIE 6.0; MS Web Services Client Protocol 2.0.50727.3655)";
	hInternet = InternetOpen(szAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

	hConnection = InternetConnect(hInternet, host.c_str(), nPort, 0, 0, INTERNET_SERVICE_HTTP, 0, 0); 
	if(!hConnection)
	{
		dwErrorCode = 1;
		goto _OUT;
	}

	hRequest = HttpOpenRequest(hConnection, method.c_str(), url.c_str(), 0, 0, 0, INTERNET_FLAG_KEEP_CONNECTION|INTERNET_FLAG_DONT_CACHE, 0);
	if(!hRequest)
	{
		dwErrorCode = 2;
		goto _OUT;
	}

	// header
	if( method == _T("POST"))
		header += _T("Content-Type: application/x-www-form-urlencoded; charset=UTF-8");
	
	header += m_strHttpHeader;

	BOOL res = HttpSendRequest(hRequest, header.c_str(), header.length(), (LPVOID)post.c_str(), post.length());
	if( !res )
	{
		dwErrorCode = 3;
		goto _OUT;
	}

	int nTotalDataSize = 0;
	DWORD dwSize = 4;
	HttpQueryInfo(hRequest, HTTP_QUERY_CONTENT_LENGTH|HTTP_QUERY_FLAG_NUMBER, (LPDWORD)&nTotalDataSize, &dwSize, NULL);
	if( nTotalDataSize == 0 )
		dwErrorCode = 4;


	// notify file size to check whether we need to download this file
	StartNotify.nTotalSize = nTotalDataSize;
	StartNotify.dwErrorCode = dwErrorCode;

	// don't need to download so just return
	LRESULT ret = CallDelegate(&OnStart, &m_mapOnStartUserDatas, &StartNotify);
	if( ret == 0 )
		goto _OUT;

	if( nTotalDataSize == 0 )
		goto _OUT;
	

	// loop
	DWORD BytesWrite = 0;
	char szBuffer[0x2000];
	while( m_bDownloading && !m_bPaused )
	{
		BytesWrite = 0;
		memset(szBuffer, 0, sizeof(szBuffer));

		res = InternetReadFile(hRequest, szBuffer, sizeof(szBuffer), &BytesWrite);
		if( !res )
		{
			dwErrorCode = GetLastError();
			break;
		}

		if( BytesWrite == 0 )
			break;

		// OnProgress
		if( nTotalDataSize > 0 && OnProgress )
		{
			THttpNotify ProgressNotify;
			ProgressNotify.dwTaskId		= m_dwTaskId;
			ProgressNotify.pData		= szBuffer;
			ProgressNotify.nDataSize	= BytesWrite;
			ProgressNotify.nTotalSize	= nTotalDataSize;

			LRESULT ret = CallDelegate(&OnProgress, &m_mapOnProgressUserDatas, &ProgressNotify);
			if( ret == 0 )
			{
				dwErrorCode = 5;
				break;
			}
				
		}
	}


_OUT:
	InternetCloseHandle(hRequest);
	InternetCloseHandle(hConnection);
	InternetCloseHandle(hInternet);

	// OnComplete
	THttpNotify CompleteNotify;
	CompleteNotify.dwErrorCode	= dwErrorCode;
	CompleteNotify.dwTaskId		= m_dwTaskId;
	CompleteNotify.pData		= NULL;
	CompleteNotify.nDataSize	= 0;
	CompleteNotify.nTotalSize	= nTotalDataSize;
	CompleteNotify.fPercent		= 1.0f;

	// don't callback if it is paused
	if( !m_bPaused )
		CallDelegate(&OnComplete, &m_mapOnCompleteUserDatas, &CompleteNotify);
	else
	{
		THttpNotify PauseNotify;

		if( m_hOwnerWnd != NULL )
			ret = SendMessage(m_hOwnerWnd, WM_USER_HTTP_DOWNLOAD, (WPARAM)&OnPause, (LPARAM)&PauseNotify);
		else
			ret = OnPause(&PauseNotify);
	}
}

LRESULT CHttpDownloadTask::CallDelegate(CHttpEventSource *pEventSource, map<DWORD, PVOID>* pMapUserDatas, THttpNotify* pNotify)
{
	LRESULT ret = 0;

	if( pEventSource == NULL || pNotify == NULL || pMapUserDatas == NULL )
		return 0;

	// traverse all delegates
	CHttpPtrArray* pDelegates = pEventSource->GetDelegates();

	for(int i = 0; i < pDelegates->GetSize(); i++)
	{
		CHttpDelegateBase* pDelegate = (CHttpDelegateBase*)pDelegates->GetAt(i);
		if( pDelegate == NULL )
			continue;

		PVOID pUserData = NULL;

		DWORD dwKey = (DWORD)pDelegate->GetFn() | (DWORD)pDelegate->GetObject();
		map<DWORD, PVOID>::iterator itr = pMapUserDatas->find(dwKey);
		if( itr != pMapUserDatas->end() )
			pUserData = itr->second;

		pNotify->pUserData	= pUserData;

		// construct a new event source which only has one delegate
		OnTempEventSource.clear();
		OnTempEventSource += *pDelegate;

		if( m_hOwnerWnd != NULL )
			ret = SendMessage(m_hOwnerWnd, WM_USER_HTTP_DOWNLOAD, (WPARAM)&OnTempEventSource, (LPARAM)pNotify);
		else
			ret = (*pEventSource)(pNotify);

	}		

	return ret;
}