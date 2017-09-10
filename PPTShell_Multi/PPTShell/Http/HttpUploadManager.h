//-----------------------------------------------------------------------
// FileName:				HttpUploadManager.h
//
// Desc:
//------------------------------------------------------------------------
#ifndef _HTTP_UOLOAD_MANAGER_H_
#define _HTTP_UOLOAD_MANAGER_H_

#include "Util/Singleton.h"
#include "HttpUpload.h"

class CHttpUploadManager
{
private:
	CHttpUploadManager();
	~CHttpUploadManager();

public:
	BOOL	Initialize(int nMaxThreadCount = 2, int nDataThreadCount = 0);
	BOOL	Destroy();
	BOOL	SetOwner(HWND hWnd);
	BOOL	SetTaskPriority(DWORD dwTaskId, int nPriority);
	BOOL	CancelTask(DWORD dwTaskId);
	void    HttpUpload(int nThreadIdx);

	DWORD	AddUploadTask(LPCTSTR lpcstrHost, 
		LPCTSTR lpcstrUrl, 
		LPCTSTR  lpcstrFilePath,
		LPCTSTR lpcstrServerFilePath, 
		LPCTSTR lpcstrMethod,
		int nPort, 
		CHttpDelegateBase& OnCompleteDelegate,
		CHttpDelegateBase& OnProgressDelegate,
		BOOL bCallBackInMainThread = TRUE,
		BOOL bReplaceFile  = FALSE,
		PVOID pUserData= NULL);		

	BOOL	AddTaskCallBack(DWORD dwTaskId, CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressDelegate);

	static DWORD WINAPI UploadThread(LPARAM lParam);

	DECLARE_SINGLETON_CLASS(CHttpUploadManager);

protected:
	int		FindIdleThread(BOOL bDownloadFile);
	CHttpUploadTask* TakeTask(int nThreadIdx);


protected:
	deque<CHttpUploadTask*>				m_deqTasks;
	map<DWORD, CHttpUploadTask*>		m_mapTasks;		// for cancel use
	CRITICAL_SECTION					m_TaskLock;

	HWND								m_hOwnerWnd;

	HANDLE*								m_phEvents;
	HANDLE*								m_phThreads;
	BOOL*								m_pbThreadBusy;

	int									m_nCurThreadCount;
	int									m_nMaxThreadCount;
	int									m_nDataThreadCount;		// only used to download data not for file
};

typedef Singleton<CHttpUploadManager>	HttpUploadManager;

#endif