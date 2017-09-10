//-----------------------------------------------------------------------
// FileName:				HttpDownload.h
//
// Desc:
//------------------------------------------------------------------------
#ifndef _HTTP_DOWNLOAD_H_
#define _HTTP_DOWNLOAD_H_

#define HIGHEST_DOWNLOAD_PRIORITY		10
#define LOWEST_DOWNLOAD_PRIORITY		0

#include <WinInet.h>
// Structure for notifications to the outside world
typedef struct THttpNotify
{
	DWORD		dwErrorCode;	// success: 0
	DWORD		dwTaskId;
	char*		pData;
	int			nDataSize;	
	int			nTotalSize;
	int			nSpeed;			// in KB
	int			nElapseTime;	// in second
	int			nRemainTime;	// in second
	float		fPercent;
	tstring		strFilePath;
	PVOID		pDetailData;	// more detail response data
	PVOID		pUserData;		// passed parameter

} THttpNotify; 

 
class CHttpDownloadTask
{
public:
	CHttpDownloadTask(LPCTSTR lpcstrHost, 
					  LPCTSTR lpcstrUrl, 
					  LPCTSTR lpcstrHeader,
					  LPCTSTR lpcstrMethod, 
					  LPCSTR  lpcstrPost,
					  int nPort, 
					  CHttpDelegateBase& OnComplete,
					  CHttpDelegateBase& OnProgress,
					  CHttpDelegateBase& OnStart,
					  PVOID pUserData = NULL);

	~CHttpDownloadTask();

	BOOL	Start();
	BOOL	Stop();
	BOOL	Pause( CHttpDelegateBase* pOnPauseDelegate = NULL);
	BOOL	Resume();

	void	SetOwner(HWND hWnd);
	void	SetTaskId(DWORD dwTaskId);
	DWORD	GetTaskId();
	void	AddTaskCallBack(CHttpDelegateBase& OnComplete, CHttpDelegateBase& OnProgress);
	BOOL	IsDownloading();
	void	SetDownloadFile(BOOL bDownloadFile, DWORD dwStartPos = 0);		// 支持断点续传
	BOOL	IsDownloadFile();
	BOOL	IsPaused();

	void	SetPriority(int nPriority);
	int		GetPriority();	

protected:
	void	GetHttpData(tstring host, tstring url, int nPort, tstring method, string post);
	void	GetHttpFile(tstring host, tstring url, int nPort, tstring method, string post);
	LRESULT	CallDelegate(CHttpEventSource* pEventSource, map<DWORD, PVOID>* pMapUserDatas, THttpNotify* pNotify);	

protected:
	DWORD						m_dwTaskId;
	tstring						m_strHost;
	tstring						m_strUrl;
	tstring						m_strHeader;
	tstring						m_strMethod;
	string						m_strPost;
	int							m_nPort;

	BOOL						m_bDownloading;
	BOOL						m_bPaused;
	BOOL						m_bDownloadFile;

	DWORD						m_dwFileStartPos;
	HWND						m_hOwnerWnd;

	CHttpEventSource			OnStart;
	CHttpEventSource			OnComplete;
	CHttpEventSource			OnProgress;
	CHttpEventSource			OnPause;
	CHttpEventSource			OnTempEventSource;

	CRITICAL_SECTION			m_Lock;
	tstring						m_strHttpHeader;

	int							m_nPriority;

	map<DWORD, PVOID>			m_mapOnStartUserDatas;		// (Object|Function)  <--> pUserData
	map<DWORD, PVOID>			m_mapOnProgressUserDatas;	// (Object|Function)  <--> pUserData
	map<DWORD, PVOID>			m_mapOnCompleteUserDatas;	// (Object|Function)  <--> pUserData
};

#endif