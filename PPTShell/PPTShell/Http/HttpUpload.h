//-----------------------------------------------------------------------
// FileName:				HttpDownload.h
//
// Desc:
//------------------------------------------------------------------------
#ifndef _HTTP_UPLOAD_H_
#define _HTTP_UPLOAD_H_
#include "HttpDownload.h"
#define  TIMEOUT_MIN	30
#define	 RE_CONN_TIME	3

class CHttpUploadTask 
{
public:
	CHttpUploadTask(LPCTSTR lpcstrHost, 
		LPCTSTR lpcstrUrl, 
		LPCTSTR lpcstrFilePath, 
		LPCTSTR lpcstrServerFilePath, 
		LPCTSTR lpcstrMethod,
		int nPort, 
		CHttpDelegateBase& OnComplete,
		CHttpDelegateBase& OnProgress,
		BOOL  bReplaceFile = FALSE,
		PVOID pUserData = NULL);

	~CHttpUploadTask();


	BOOL	Start();
	BOOL	Stop();
	void	SetOwner(HWND hWnd);
	void	SetTaskId(DWORD dwTaskId);
	DWORD	GetTaskId();

	BOOL	IsUploading();


	void	SetPriority(int nPriority);
	int		GetPriority();	

	int		GetType() {return m_nType;}

	BOOL	UseHttpSendRequestDataToWeb(BOOL bUseHeader);
public:
	HINTERNET					m_hInternet;
	HINTERNET					m_hConnection;
	HINTERNET					m_hRequest;

protected:

	int							m_nType;

	DWORD						m_dwTaskId;
	tstring						m_strHost;
	tstring						m_strUrl;
	tstring						m_strFilePath;
	tstring						m_strMethod;
	tstring						m_strFileName;

	tstring						m_strServerFilePath;
	int							m_nPort;

	BOOL						m_bUploading;
	BOOL						m_bReplaceFile;

	BOOL						m_bDownloadFile;
	DWORD						m_dwFileStartPos;

	HWND						m_hOwnerWnd;

	CHttpEventSource			OnComplete;
	CHttpEventSource			OnProgress;

	CRITICAL_SECTION			m_Lock;
	tstring						m_strHttpHeader;

	int							m_nPriority;
	PVOID						m_pUserData;

	SOCKET						m_conn;
protected:

	static void __stdcall Callback(HINTERNET hInternet,
		DWORD dwContext,
		DWORD dwInternetStatus,
		LPVOID lpStatusInfo,
		DWORD dwStatusInfoLen);

	void	SendHttpData();

	
};
#endif