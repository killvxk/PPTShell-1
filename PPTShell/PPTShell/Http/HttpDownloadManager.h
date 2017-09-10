//-----------------------------------------------------------------------
// FileName:				HttpDownloadManager.h
//
// Desc:
//------------------------------------------------------------------------
#ifndef _HTTP_DOWNLOAD_MANAGER_H_
#define _HTTP_DOWNLOAD_MANAGER_H_

#include "Util/Singleton.h"

class CHttpDownloadManager
{
private:
	CHttpDownloadManager();
	~CHttpDownloadManager();

public:
	BOOL	Initialize(int nMaxThreadCount = 10, int nDataThreadCount = 2);
	BOOL	Destroy();
	BOOL	SetOwner(HWND hWnd);
	BOOL	SetTaskPriority(DWORD dwTaskId, int nPriority);
	BOOL	CancelTask(DWORD dwTaskId);
	BOOL	PauseTask(DWORD dwTaskId,  CHttpDelegateBase* pOnPauseDelegate = NULL);
	BOOL	ResumeTask(DWORD dwTaskId, BOOL bDownloadFile = TRUE, DWORD dwFileStartPos = 0, int nPriority = HIGHEST_DOWNLOAD_PRIORITY);
	void    HttpDownload(int nThreadIdx);

	DWORD	AddTask(LPCTSTR lpcstrHost, 
					LPCTSTR lpcstrUrl, 
					LPCTSTR  lpcstrHeader,
					LPCTSTR lpcstrMethod, 
					LPCSTR  lpcstrPost,
					int nPort, 
					CHttpDelegateBase& OnCompleteDelegate,
					CHttpDelegateBase& OnProgressDelegate,
					CHttpDelegateBase& OnStartDelegate,
					BOOL bCallBackInMainThread = TRUE,
					BOOL bDownloadFile = FALSE,
					DWORD dwFileStartPos = 0, // ¶ÏµãÐø´«
					PVOID pUserData = NULL);		

	BOOL	AddTaskCallBack(DWORD dwTaskId, CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressDelegate);

	static DWORD WINAPI DownloadThread(LPARAM lParam);

	DECLARE_SINGLETON_CLASS(CHttpDownloadManager);

protected:
	int		FindIdleThread(BOOL bDownloadFile);
	CHttpDownloadTask* TakeTask(int nThreadIdx);
	

protected:
	deque<CHttpDownloadTask*>			m_deqTasks;
	map<DWORD, CHttpDownloadTask*>		m_mapTasks;		// for cancel use
	CRITICAL_SECTION					m_TaskLock;

	HWND								m_hOwnerWnd;

	HANDLE*								m_phEvents;
	HANDLE*								m_phThreads;
	BOOL*								m_pbThreadBusy;

	int									m_nCurThreadCount;
	int									m_nMaxThreadCount;
	int									m_nDataThreadCount;		// only used to download data not for file
};

typedef Singleton<CHttpDownloadManager>	HttpDownloadManager;

#endif