//-----------------------------------------------------------------------
// FileName:				HttpDownload.cpp
//
// Desc:
//------------------------------------------------------------------------
#include "stdafx.h"
#include "HttpDelegate.h"
#include "HttpUpload.h"
#include "Util/MD5Encrypt.h"
#include "json/json.h"
#include "Util/Util.h"

#define POST_HEADER			_T("Host: %s\r\nContent-Type: multipart/form-data, boundary=---HTTP-DATA-BOUNDARY-0-%u%u---\r\nContent-Length: %u\r\nConnection: Keep-Alive\r\n\r\n")
#define POST_BODY_NAME		_T("-----HTTP-DATA-BOUNDARY-0-%u%u---\r\nContent-Disposition: form-data; name=\"name\"\r\n\r\n%s\r\n")
#define POST_BODY_PATH		_T("-----HTTP-DATA-BOUNDARY-0-%u%u---\r\nContent-Disposition: form-data; name=\"Path\"\r\n\r\n%s\r\n")
#define POST_BODY_FILEPATH	_T("-----HTTP-DATA-BOUNDARY-0-%u%u---\r\nContent-Disposition: form-data; name=\"FilePath\"\r\n\r\n%s\r\n")
#define POST_BODY_SCOPE		_T("-----HTTP-DATA-BOUNDARY-0-%u%u---\r\nContent-Disposition: form-data; name=\"scope\"\r\n\r\n1\r\n")
//#define POST_BODY_MD5		_T("-----------------------------%u\r\nContent-Disposition: form-data; name=\"md5\"\r\n\r\n%s\r\n"
#define POST_BODY_SIZE		_T("-----HTTP-DATA-BOUNDARY-0-%u%u---\r\nContent-Disposition: form-data; name=\"size\"\r\n\r\n%u\r\n")
#define POST_BODY_CHUNKS	_T("-----HTTP-DATA-BOUNDARY-0-%u%u---\r\nContent-Disposition: form-data; name=\"chunks\"\r\n\r\n%u\r\n")
#define POST_BODY_CHUNK		_T("-----HTTP-DATA-BOUNDARY-0-%u%u---\r\nContent-Disposition: form-data; name=\"chunk\"\r\n\r\n%u\r\n")
#define POST_BODY_CHUNKSIZE	_T("-----HTTP-DATA-BOUNDARY-0-%u%u---\r\nContent-Disposition: form-data; name=\"chunkSize\"\r\n\r\n%d\r\n")
#define POST_BODY_FILE		_T("-----HTTP-DATA-BOUNDARY-0-%u%u---\r\nContent-Disposition: form-data; name=\"file\"; filename=\"%s\"\r\n\r\n")
#define POST_BODY_FILE_END	_T("\r\n-----HTTP-DATA-BOUNDARY-0-%u%u-----\r\n")

#define ChunkSize			( 50 * 100 * 1024 * 10 )
#pragma comment(lib, "wininet.lib")

//-----------------------------------
//CHttpUploadTask
//
CHttpUploadTask::CHttpUploadTask( LPCTSTR lpcstrHost, 
								 LPCTSTR lpcstrUrl, 
								 LPCTSTR lpcstrFilePath, 
								 LPCTSTR lpcstrServerFilePath, 
								 LPCTSTR lpcstrMethod,
								 int nPort, 
								 CHttpDelegateBase& OnCompleteDelegate,
								 CHttpDelegateBase& OnProgressDelegate,
								 BOOL	bReplaceFile,
								 PVOID pUserData
								 ) 
{
	m_strHost			= lpcstrHost;
	m_strUrl			= lpcstrUrl;
	m_strFilePath		= lpcstrFilePath;
	m_strServerFilePath	= lpcstrServerFilePath;
	m_strMethod			= lpcstrMethod;
	m_nPort				= nPort;
	m_bUploading		= FALSE;
	m_bDownloadFile		= FALSE;
	m_hOwnerWnd			= NULL;
	m_dwFileStartPos	= 0;
	m_nPriority			= HIGHEST_DOWNLOAD_PRIORITY;
	m_bReplaceFile		= bReplaceFile;
	m_pUserData			= pUserData;
	
	InitializeCriticalSection(&m_Lock);

	if( OnCompleteDelegate.GetFn() != NULL )
		OnComplete += OnCompleteDelegate;

	if( OnProgressDelegate.GetFn() != NULL )
		OnProgress += OnProgressDelegate;
}


CHttpUploadTask::~CHttpUploadTask()
{
	DeleteCriticalSection(&m_Lock);
}

void CHttpUploadTask::SetOwner(HWND hWnd)
{
	m_hOwnerWnd = hWnd;
}

void CHttpUploadTask::SetTaskId(DWORD dwTaskId)
{
	m_dwTaskId = dwTaskId;
}

DWORD CHttpUploadTask::GetTaskId()
{
	return	m_dwTaskId;
}

BOOL CHttpUploadTask::IsUploading()
{
	BOOL res = FALSE;

	EnterCriticalSection(&m_Lock);
	res = m_bUploading;
	LeaveCriticalSection(&m_Lock);

	return res;
}

void CHttpUploadTask::SetPriority(int nPriority)
{
	m_nPriority = nPriority;
}

int CHttpUploadTask::GetPriority()
{
	return m_nPriority;
}


BOOL CHttpUploadTask::Start()
{
	EnterCriticalSection(&m_Lock);
	m_bUploading =  TRUE;
	LeaveCriticalSection(&m_Lock);

	SendHttpData();

	return TRUE;
}

BOOL CHttpUploadTask::Stop()
{
	EnterCriticalSection(&m_Lock);
	m_bUploading = FALSE;
	LeaveCriticalSection(&m_Lock);

	OnComplete.clear();
	OnProgress.clear();

	return TRUE;
}


void CHttpUploadTask::SendHttpData()
{
	DWORD				dwErrorCode = 0;
	int					nDataSize = 0;
	char				*pBuffer = NULL;
	DWORD				dwBytesRead;
	string				strResult = "";

	if(!UseHttpSendRequestDataToWeb(TRUE))
	{
		dwErrorCode = 1;
		goto _Out;
	}

	if (m_bUploading)  
	{
		TCHAR   szBuf[2048] = {0};  
		
		int		nAckStatus = 0;
		int		nRecLen,n,m;
		do  
		{  
			//读取上传完成后的返回结果  
			memset(szBuf, 0, sizeof(szBuf));

			nRecLen = recv(m_conn, szBuf, sizeof(szBuf) - 1, 0);
//   		strResult = szBuf;
//   		strResult = Utf8ToAnsi(strResult);
			if (0 >= nRecLen) 
			{
				dwErrorCode = 3;
				goto _Out;
			}
			if (-1 == (n = FindChar_f(szBuf, "HTTP/")))
			{
				dwErrorCode = 4;
				goto _Out;
			}

			char sTmp[64];
			strncpy(sTmp, szBuf + 9, 3);
			sTmp[3] = '\0';
			nAckStatus = atoi(sTmp);

			
			if (nAckStatus < 200 || nAckStatus >= 300)
			{
				dwErrorCode = 6;
				goto _Out;
			}

			if (-1 == (n = FindChar_n(szBuf, "\r\n\r\n", 2048)))
			{
				dwErrorCode = 7;
				goto _Out;
			}
			n += 4;

			strResult += szBuf + n ;
			int nCount;
			if (-1 != (m = FindChar_n(szBuf + n, "\r\n", nRecLen - n )))
			{
				ZeroMemory(sTmp, sizeof(sTmp));
				strncpy(sTmp, szBuf + n, m);
				nCount = strtoul(sTmp, 0 ,16);
				strResult = strResult.substr(m + 2, nCount);
			}

			strResult = Utf8ToAnsi(strResult);
			Json::Reader	reader;
			Json::Value		result;
			if (!reader.parse(strResult.c_str(), result))
			{
				dwErrorCode = 8;
				break;
			}
			break;
		
		}   
		while (m_bUploading);  
// 		int len = strResult.length();
// 		nDataSize = len;
// 		pBuffer =new char[len+1];
// 		strcpy(pBuffer,strResult.c_str());
	}
_Out:
	closesocket(m_conn);
// 	if ( pBuffer == NULL )
// 	{
// 		pBuffer =new char[nDataSize+1];
// 		ZeroMemory(pBuffer, nDataSize+1);
// 	}

	// OnComplete
	THttpNotify CompleteNotify;
	CompleteNotify.dwErrorCode	= dwErrorCode;
	CompleteNotify.dwTaskId		= m_dwTaskId;
	CompleteNotify.pData		= (char *)strResult.c_str();
	CompleteNotify.nDataSize	= strResult.length();
	CompleteNotify.nTotalSize	= strResult.length();
	CompleteNotify.fPercent		= 1.0f;
	CompleteNotify.pUserData	= m_pUserData;

	if( m_hOwnerWnd != NULL && OnComplete)
		SendMessage(m_hOwnerWnd, WM_USER_HTTP_UPLOAD, (WPARAM)&OnComplete, (LPARAM)&CompleteNotify);
	else
	{
		if( OnComplete )
			OnComplete(&CompleteNotify);
	}

//	delete pBuffer;
}

/************************************************************************/  
BOOL CHttpUploadTask::UseHttpSendRequestDataToWeb(BOOL bUseHeader)  
{
	//tstring strMd5 = MD5EncryptFile(m_strFilePath.c_str());

	DWORD dwFileSize = 0;
	HANDLE hFile = CreateFile(m_strFilePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if( hFile == INVALID_HANDLE_VALUE )
	{
		CloseHandle(hFile);
		return FALSE;
	}

	dwFileSize = GetFileSize(hFile, NULL);
	if( dwFileSize == 0 )
	{
		CloseHandle(hFile);
		return FALSE;
	}
  
	//分块上传，默认每块50M
	bool bChunkUpload = false;
	int nChunkCount = (dwFileSize / ChunkSize) + 1;
	if(nChunkCount > 1)
		bChunkUpload = true;

	DWORD dwUploadFullSize = 0;

	CTime tm = CTime::GetCurrentTime();

	TCHAR szHeader[2048]; 
	TCHAR szBodyName[1024]= {0};
	TCHAR szBodyPath[1024]= {0};
	TCHAR szBodyScope[1024]= {0};
	TCHAR szBodyFile[1024]= {0};
	TCHAR szBodyChunk[1024]= {0};
	TCHAR szBodyChunks[1024]= {0};
	TCHAR szBodyChunkSize[1024]= {0};
	TCHAR szBodySize[1024]= {0};
	TCHAR szBodyFileEnd[1024];


	//get file name

	TCHAR szDrive[8]				= {0};
	TCHAR szDir[MAX_PATH * 2]		= {0};
	TCHAR szFile[MAX_PATH * 2]	= {0};
	TCHAR szExt[MAX_PATH * 2]		= {0};
	if (_tsplitpath_s(m_strFilePath.c_str(), szDrive, szDir, szFile, szExt))
	{
		CloseHandle(hFile);
		return FALSE;
		//continue;
	}

	tstring strFileName = szFile;
	strFileName += szExt;
	if(!m_bReplaceFile)
	{
		CTime tm = CTime::GetCurrentTime();
		DWORD dwTime = tm.GetTime();
		TCHAR szTime[128];
		_stprintf(szTime, _T("%d"), dwTime);
		strFileName = szTime;
		strFileName += szExt;
	}
	m_strFileName = strFileName;

	for(int i = 0 ; i < nChunkCount ; i++)
	{
		//auto truncate filename to avoid length limit of server
// 		if (strFileName.length() > 100)
// 		{
// 			//strFormat.length() + 1  .jpg
// 			TCHAR szBuffer[MAX_PATH] = {0};
// 			_tcsncpy_s(szBuffer, strFileName.c_str(), 100 - (_tcslen(szExt) + 1));
// 			strFileName = szBuffer;
// 			strFileName	+= szExt;
// 		}

		if (0 != ConnWithTime(m_conn, TIMEOUT_MIN, RE_CONN_TIME, m_strHost.c_str(), m_nPort, m_bUploading))
		{ 
			CloseHandle(hFile);
			return FALSE;
		}


		DWORD dwTime = tm.GetTime();


		strFileName = AnsiToUtf8(strFileName);
		_stprintf(szBodyName, POST_BODY_NAME, dwTime, dwTime, strFileName.c_str() );
		if(m_bReplaceFile)
		{
			m_strServerFilePath = AnsiToUtf8(m_strServerFilePath);
			_stprintf(szBodyPath, POST_BODY_FILEPATH, dwTime, dwTime, m_strServerFilePath.c_str() );
		}
		else
			_stprintf(szBodyPath, POST_BODY_PATH, dwTime, dwTime, m_strServerFilePath.c_str() );

		_stprintf(szBodyScope,  POST_BODY_SCOPE, dwTime, dwTime );
		_stprintf(szBodyFile, POST_BODY_FILE, dwTime, dwTime, strFileName.c_str());
		_stprintf(szBodyFileEnd, POST_BODY_FILE_END, dwTime, dwTime );
		_stprintf(szBodySize, POST_BODY_SIZE, dwTime, dwTime , dwFileSize);
		_stprintf(szBodyChunks, POST_BODY_CHUNKS, dwTime, dwTime , nChunkCount);
		_stprintf(szBodyChunk, POST_BODY_CHUNK, dwTime, dwTime , i);

		ULONG ulFileSize = dwFileSize >= ChunkSize * (i + 1) ? ChunkSize : dwFileSize - ChunkSize * i;
		_stprintf(szBodyChunkSize, POST_BODY_CHUNKSIZE, dwTime, dwTime , ulFileSize);

		DWORD dwBodyNameLen = _tcslen(szBodyName);  
		DWORD dwBodyPathLen= _tcslen(szBodyPath); 
		DWORD dwBodyScopeLen= _tcslen(szBodyScope); 
		DWORD dwBodyFileLen= _tcslen(szBodyFile); 
		DWORD dwBodyFileEndLen= _tcslen(szBodyFileEnd); 
		DWORD dwBodySizeLen = _tcslen(szBodySize); 
		DWORD dwBodyChunksLen = _tcslen(szBodyChunks); 
		DWORD dwBodyChunkLen = _tcslen(szBodyChunk); 
		DWORD dwBodyChunkSizeLen = _tcslen(szBodyChunkSize); 

		DWORD dwBufferTotal;
		if(m_bReplaceFile)
		{
			if(bChunkUpload)
			{
				dwBufferTotal = ulFileSize + dwBodyPathLen + dwBodyScopeLen + dwBodyFileLen + dwBodyFileEndLen
					+ dwBodySizeLen + dwBodyChunksLen + dwBodyChunkLen + dwBodyChunkSizeLen;  
			}
			else
			{
				dwBufferTotal = ulFileSize + dwBodyPathLen + dwBodyScopeLen + dwBodyFileLen + dwBodyFileEndLen;  
			}
		}
		else
		{
			if(bChunkUpload)
			{
				dwBufferTotal = ulFileSize + dwBodyNameLen + dwBodyPathLen + dwBodyScopeLen + dwBodyFileLen + dwBodyFileEndLen
					+ dwBodySizeLen + dwBodyChunksLen + dwBodyChunkLen + dwBodyChunkSizeLen; 
			}
			else
			{
				dwBufferTotal = ulFileSize + dwBodyNameLen + dwBodyPathLen + dwBodyScopeLen + dwBodyFileLen + dwBodyFileEndLen; 
			}
		}
		
			

		if(m_bReplaceFile)
			_stprintf(szHeader, POST_HEADER, m_strHost.c_str(), dwTime, dwTime, dwBufferTotal);
		else
			_stprintf(szHeader, POST_HEADER, m_strHost.c_str(), dwTime, dwTime, dwBufferTotal);

		char s[10240];
		ZeroMemory(s, sizeof(s));
		sprintf(s, "POST %s HTTP/1.1\r\n"
			"%s", m_strUrl.c_str(), szHeader);
		int nRet = send(m_conn, s, _tcslen(s), 0);
		if(nRet == SOCKET_ERROR || nRet != _tcslen(s))
		{
			CloseHandle(hFile);
			return FALSE;
		}

		DWORD       dwBytesRead = 0;  

		DWORD       dwBytesOutAll = 0;  
		BOOL        bRet = TRUE;  

		DWORD		dwStartTime = GetTickCount();
		DWORD		dwLastTime =  dwStartTime;


		if (m_bUploading && !m_bReplaceFile)//Name
		{  
			memcpy(s, szBodyName, dwBodyNameLen);
			nRet = send(m_conn, s, dwBodyNameLen, 0);
			if (nRet == SOCKET_ERROR || nRet != dwBodyNameLen)  
			{
				CloseHandle(hFile);
				return FALSE;  
			}
			dwBytesOutAll += dwBodyNameLen;  
		}  

		if (m_bUploading)//Path
		{  
			memcpy(s, szBodyPath, dwBodyPathLen);
			nRet = send(m_conn, s, dwBodyPathLen, 0);
			if (nRet == SOCKET_ERROR || nRet != dwBodyPathLen)  
			{
				CloseHandle(hFile);
				return FALSE;  
			}
			//nRecv = recv(m_conn, s, sizeof(s), 0);

			dwBytesOutAll += dwBodyPathLen;  
		}

		if(m_bUploading && bChunkUpload)//Size
		{ 
			memcpy(s, szBodySize, dwBodySizeLen);
			nRet = send(m_conn, s, dwBodySizeLen, 0);
			if (nRet == SOCKET_ERROR || nRet != dwBodySizeLen)  
			{
				CloseHandle(hFile);
				return FALSE;  
			}
			//nRecv = recv(m_conn, s, sizeof(s), 0);

			dwBytesOutAll += dwBodySizeLen;  
		}

		if(m_bUploading && bChunkUpload)//Chunk
		{
			memcpy(s, szBodyChunk, dwBodyChunkLen);
			nRet = send(m_conn, s, dwBodyChunkLen, 0);
			if (nRet == SOCKET_ERROR || nRet != dwBodyChunkLen)  
			{
				CloseHandle(hFile);
				return FALSE;  
			}
			//nRecv = recv(m_conn, s, sizeof(s), 0);

			dwBytesOutAll += dwBodyChunkLen;  
		}

		if(m_bUploading && bChunkUpload)//ChunkSize
		{
			memcpy(s, szBodyChunkSize, dwBodyChunkSizeLen);
			nRet = send(m_conn, s, dwBodyChunkSizeLen, 0);
			if (nRet == SOCKET_ERROR || nRet != dwBodyChunkSizeLen)  
			{
				CloseHandle(hFile);
				return FALSE;  
			}
			//nRecv = recv(m_conn, s, sizeof(s), 0);

			dwBytesOutAll += dwBodyChunkSizeLen;  
		}


		if(m_bUploading && bChunkUpload)//Chunks
		{
			memcpy(s, szBodyChunks, dwBodyChunksLen);
			nRet = send(m_conn, s, dwBodyChunksLen, 0);
			if (nRet == SOCKET_ERROR || nRet != dwBodyChunksLen)  
			{
				CloseHandle(hFile);
				return FALSE;  
			}
			//nRecv = recv(m_conn, s, sizeof(s), 0);

			dwBytesOutAll += dwBodyChunksLen;  
		}

		if (m_bUploading)//Scope
		{   
			memcpy(s, szBodyScope, dwBodyScopeLen);
			nRet = send(m_conn, s, dwBodyScopeLen, 0);
			if (nRet == SOCKET_ERROR || nRet != dwBodyScopeLen)  
			{
				CloseHandle(hFile);
				return FALSE;  
			}
			//nRecv = recv(m_conn, s, sizeof(s), 0);

			dwBytesOutAll += dwBodyScopeLen;  
		}  

		if(m_bUploading)//File
		{  
			memcpy(s, szBodyFile, dwBodyFileLen);
			nRet = send(m_conn, s, dwBodyFileLen, 0);
			if (nRet == SOCKET_ERROR || nRet != dwBodyFileLen)  
			{
				CloseHandle(hFile);
				return FALSE;  
			}
			//nRecv = recv(m_conn, s, sizeof(s), 0);

			dwBytesOutAll += dwBodyFileLen;  
		}

		//读取并发送数据  
		DWORD dwPregressSize = 0;
		while (m_bUploading)  
		{  
			ReadFile(hFile, s, sizeof(s), &dwBytesRead, NULL );  
			if (dwBytesRead <= 0)  
				break;  

			//write数据  
			//bRet = InternetWriteFile(m_hRequest, szBuffData, dwBytesRead, &dwBytesOut);  
			nRet = send(m_conn, s, dwBytesRead, 0);
			if (nRet == SOCKET_ERROR || nRet != dwBytesRead)  
			{
				DWORD dwErrorCode = GetLastError();
				CloseHandle(hFile);
				return FALSE;  
			}

			dwBytesOutAll += dwBytesRead;  
// 			char szBuff[1024];
// 			_stprintf(szBuff, "%u,%u\n", dwBytesOutAll,dwBytesRead);
// 			OutputDebugString(szBuff);
			dwUploadFullSize += dwBytesRead;
			// OnProgress
			if( dwBytesOutAll > 0 && OnProgress )
			{
				dwPregressSize += dwBytesRead;

				if (GetTickCount() - dwLastTime >= 1000)
				{


					THttpNotify ProgressNotify;
					ProgressNotify.dwTaskId		= m_dwTaskId;
					ProgressNotify.pData		= s;
					ProgressNotify.nDataSize	= dwUploadFullSize;
					ProgressNotify.fPercent		= (float)dwUploadFullSize / (float)dwFileSize;
					ProgressNotify.nTotalSize	= dwFileSize;

					// calculate speed
					DWORD dwElapseTime = GetTickCount() - dwLastTime;
					float fSpeed = (float)dwPregressSize/ 1024.0f / ((float)dwElapseTime / 1000.0f);

					dwPregressSize = 0;
					dwLastTime = GetTickCount();

					ProgressNotify.nSpeed =(int)fSpeed;
					if( ProgressNotify.nSpeed < 0 )
						ProgressNotify.nSpeed = 0;

					ProgressNotify.nElapseTime = (GetTickCount()-dwStartTime) / 1000;

					// calculate remain time
					int nRemainSizeInK =  (dwFileSize - dwUploadFullSize) / 1024;

					if( ProgressNotify.nSpeed )
						ProgressNotify.nRemainTime = nRemainSizeInK / ProgressNotify.nSpeed;
					else
						ProgressNotify.nRemainTime = nRemainSizeInK;

					if( m_hOwnerWnd != NULL )
						SendMessage(m_hOwnerWnd, WM_USER_HTTP_UPLOAD, (WPARAM)&OnProgress, (LPARAM)&ProgressNotify);
					else
						OnProgress(&ProgressNotify);
				}

			}

			if(dwBytesOutAll >= dwBufferTotal - dwBodyFileEndLen)
			{
				break;
			}
		}  

		//

		//如果filename不为空，则发送表尾结束符  
		if (m_bUploading)//FileEnd  
		{  
			nRet = send(m_conn, szBodyFileEnd, dwBodyFileEndLen, 0);
			if (nRet == SOCKET_ERROR || nRet != dwBodyFileEndLen)  
			{
				CloseHandle(hFile);
				return FALSE;  
			}

			//nRecv = recv(m_conn, s, sizeof(s), 0);
			dwBytesOutAll += dwBodyFileEndLen;  
		}  

		if(dwBytesOutAll != dwBufferTotal)
		{
			CloseHandle(hFile);
			return FALSE;  
		}

		if(dwUploadFullSize == dwFileSize)
		{
			break;
		}

		closesocket(m_conn);
	}
	

	

	//结束上传请求  
// 	if (m_bUploading && !HttpEndRequest(m_hRequest, NULL, 0, 0))  
// 	{  
// 		DWORD   dwErr = GetLastError();  
// 		CloseHandle(hFile);
// 		return FALSE;  
// 	}  

	CloseHandle(hFile);
	return TRUE;  
}
