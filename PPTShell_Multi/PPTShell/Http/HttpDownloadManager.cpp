//-----------------------------------------------------------------------
// FileName:				HttpDownloadManager.cpp
//
// Desc:
//------------------------------------------------------------------------
#include "stdafx.h"
#include "HttpDelegate.h"
#include "HttpDownload.h"
#include "HttpDownloadManager.h"
#include "Util/Util.h"
#include <algorithm>
#include <assert.h>

CHttpDownloadManager*	g_pHttpManager = NULL;

CHttpDownloadManager::CHttpDownloadManager()
{
	m_nCurThreadCount	= 0;
	m_nMaxThreadCount	= 0;
	m_phEvents			= NULL;
	m_phThreads			= NULL;
	m_hOwnerWnd			= NULL;

	g_pHttpManager		= this;
}

CHttpDownloadManager::~CHttpDownloadManager()
{

	//EnterCriticalSection(&m_TaskLock);
	for(int i = 0; i < m_nMaxThreadCount; i++)
	{
		// terminate thread
		if( m_phThreads[i] != NULL )
		{
			TerminateThread(m_phThreads[i], 0);
			m_phThreads[i] = NULL;
		}

		if( m_phEvents[i] != NULL )
		{
			CloseHandle(m_phEvents[i]);
			m_phEvents[i] = NULL;
		}
	}

	DeleteCriticalSection(&m_TaskLock);

	delete m_phEvents;
	m_phEvents = NULL;

	delete m_phThreads;
	m_phThreads = NULL;

	delete m_pbThreadBusy;
	m_pbThreadBusy = NULL;

	//LeaveCriticalSection(&m_TaskLock);
	

}

//
// initialize
//
BOOL CHttpDownloadManager::Initialize(int nMaxThreadCount /* = 10 */, int nDataThreadCount /*= 2*/)
{
	m_nMaxThreadCount = nMaxThreadCount;
	m_nDataThreadCount = nDataThreadCount;

	InitializeCriticalSection(&m_TaskLock);
	
	// thread handles
	m_phThreads = new HANDLE[nMaxThreadCount];
	if( m_phThreads == NULL )
		return FALSE;

	memset(m_phThreads, 0, nMaxThreadCount * sizeof(HANDLE));

	// thread events
	m_phEvents = new HANDLE[nMaxThreadCount];
	if( m_phEvents == NULL )
		return FALSE;

	memset(m_phEvents, 0, nMaxThreadCount * sizeof(HANDLE));

	// thread busy states
	m_pbThreadBusy = new BOOL[nMaxThreadCount];
	if( m_pbThreadBusy == NULL )
		return FALSE;

	memset(m_pbThreadBusy, 0, nMaxThreadCount * sizeof(BOOL));

	// create one thread at first
	m_phEvents[m_nCurThreadCount] = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_phThreads[m_nCurThreadCount] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DownloadThread, 0, 0, NULL);
	m_nCurThreadCount ++;

	return TRUE;
}

BOOL CHttpDownloadManager::Destroy()
{
	return TRUE;
}

BOOL CHttpDownloadManager::SetOwner(HWND hWnd)
{
	m_hOwnerWnd = hWnd;
	return TRUE;
}

BOOL CHttpDownloadManager::SetTaskPriority(DWORD dwTaskId, int nPriority)
{
	EnterCriticalSection(&m_TaskLock);
	map<DWORD, CHttpDownloadTask*>::iterator itr = m_mapTasks.find(dwTaskId);
	if( itr != m_mapTasks.end() )
	{
		CHttpDownloadTask* pTask = itr->second;
		pTask->SetPriority(nPriority);
	}

	LeaveCriticalSection(&m_TaskLock);
	return TRUE;
}

DWORD CHttpDownloadManager::AddTask(LPCTSTR lpcstrHost, 
									LPCTSTR lpcstrUrl, 
									LPCTSTR lpcstrHeader,
									LPCTSTR lpcstrMethod, 
									LPCTSTR lpcstrPost, 
									int nPort, 
									CHttpDelegateBase& OnCompleteDelegate,
									CHttpDelegateBase& OnProgressDelegate, 
									CHttpDelegateBase& OnStartDelegate,
									BOOL bCallBackInMainThread /*= TRUE*/,
									BOOL bDownloadFile /*= FALSE*/,
									DWORD dwFileStartPos /*= 0*/,
									PVOID pUserData /*= NULL*/)
{
	CHttpDownloadTask* pTask = new CHttpDownloadTask(lpcstrHost, 
													 lpcstrUrl, 
													 lpcstrHeader, 
													 lpcstrMethod, 
													 lpcstrPost, 
													 nPort, 
													 OnCompleteDelegate, 
													 OnProgressDelegate,
													 OnStartDelegate, 
													 pUserData);
	if( pTask == NULL )
		return 0;

	// set owner
	if( bCallBackInMainThread )
		pTask->SetOwner(m_hOwnerWnd);

	pTask->SetDownloadFile(bDownloadFile, dwFileStartPos);

	// generate a random value to identify this task
	DWORD dwTaskId = (DWORD)pTask ^ (DWORD)GetCycleCount();

	pTask->SetTaskId(dwTaskId);


	// save to deque
	EnterCriticalSection(&m_TaskLock);

	// cannot has same task id
	map<DWORD, CHttpDownloadTask*>::iterator itr = m_mapTasks.find(dwTaskId);
	assert(itr == m_mapTasks.end());

	m_deqTasks.push_back(pTask);
	m_mapTasks[dwTaskId] = pTask;
	LeaveCriticalSection(&m_TaskLock);

	// find idle thread
	int nThreadIdx = FindIdleThread(bDownloadFile);
	if( nThreadIdx != -1 )
	{
		// check thread whether has been created
		if( m_phThreads[nThreadIdx] == NULL )
		{
			m_phEvents[nThreadIdx] = CreateEvent(NULL, FALSE, FALSE, NULL);
			m_phThreads[nThreadIdx] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DownloadThread, (LPVOID)nThreadIdx, 0, NULL);
		}

		SetEvent(m_phEvents[nThreadIdx]);
	}
	else
	{
		// notify caller that there is no more idle thread to download
		CHttpEventSource EvtSource;

		if( OnStartDelegate.GetFn() != NULL )
			EvtSource += OnStartDelegate;

		THttpNotify notify;
		notify.dwErrorCode = -1;
		notify.nDataSize   = 0;
		notify.nTotalSize  = 1;

		EvtSource(&notify); 
	}
	
	return dwTaskId;
}

BOOL CHttpDownloadManager::AddTaskCallBack(DWORD dwTaskId, CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressDelegate)
{
	EnterCriticalSection(&m_TaskLock);
	map<DWORD, CHttpDownloadTask*>::iterator itr = m_mapTasks.find(dwTaskId);
	if( itr != m_mapTasks.end() )
	{
		CHttpDownloadTask* pTask = itr->second;
		pTask->AddTaskCallBack(OnCompleteDelegate, OnProgressDelegate);
	}

	LeaveCriticalSection(&m_TaskLock);

	return TRUE;
}

BOOL CHttpDownloadManager::CancelTask(DWORD dwTaskId)
{
	EnterCriticalSection(&m_TaskLock);

	CHttpDownloadTask* pFoundTask = NULL;
	BOOL bDownloading = FALSE;
	
	// remove from map
	map<DWORD, CHttpDownloadTask*>::iterator itr = m_mapTasks.find(dwTaskId);
	if( itr != m_mapTasks.end() )
	{
		pFoundTask = itr->second;
		bDownloading = pFoundTask->IsDownloading();

		assert(pFoundTask != NULL);
		pFoundTask->Stop();

		m_mapTasks.erase(dwTaskId);
	}

	// remove from deque
	deque<CHttpDownloadTask*>::iterator itr_deq;
	for(itr_deq = m_deqTasks.begin(); itr_deq != m_deqTasks.end(); itr_deq++)
	{
		CHttpDownloadTask* pTask = *itr_deq;
		if( pTask->GetTaskId() == dwTaskId )
		{
			m_deqTasks.erase(itr_deq);
			break;
		}
	}

	if( pFoundTask != NULL && !bDownloading )
	{
		delete pFoundTask;
		pFoundTask = NULL;
	}

	LeaveCriticalSection(&m_TaskLock);
	return TRUE;
}

//
// only used to download file
//
BOOL CHttpDownloadManager::PauseTask(DWORD dwTaskId, CHttpDelegateBase* pOnPauseDelegate /*= NULL*/)
{
	EnterCriticalSection(&m_TaskLock);

	CHttpDownloadTask* pFoundTask = NULL;

	// keep this task
	map<DWORD, CHttpDownloadTask*>::iterator itr = m_mapTasks.find(dwTaskId);
	if( itr != m_mapTasks.end() )
	{
		pFoundTask = itr->second;
		pFoundTask->SetPriority(-1);		// put to last
		pFoundTask->Pause(pOnPauseDelegate);
	}

	LeaveCriticalSection(&m_TaskLock);

	return TRUE;
}

//
// only used to download file
//
BOOL CHttpDownloadManager::ResumeTask(DWORD dwTaskId, BOOL bDownloadFile /*= TRUE*/, DWORD dwFileStartPos /*= 0*/, int nPriority /*= HIGHEST_DOWNLOAD_PRIORITY*/)
{
	EnterCriticalSection(&m_TaskLock);

	CHttpDownloadTask* pFoundTask = NULL;

	map<DWORD, CHttpDownloadTask*>::iterator itr = m_mapTasks.find(dwTaskId);
	if( itr != m_mapTasks.end() )
	{
		pFoundTask = itr->second;
		pFoundTask->SetDownloadFile(bDownloadFile, dwFileStartPos);
		pFoundTask->SetPriority(nPriority);
		pFoundTask->Resume();
	}

	LeaveCriticalSection(&m_TaskLock);

	// find idle thread
	int nThreadIdx = FindIdleThread(TRUE);
	if( nThreadIdx != -1 )
	{
		// check thread whether has been created
		if( m_phThreads[nThreadIdx] == NULL )
		{
			m_phEvents[nThreadIdx] = CreateEvent(NULL, FALSE, FALSE, NULL);
			m_phThreads[nThreadIdx] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DownloadThread, (LPVOID)nThreadIdx, 0, NULL);
		}

		SetEvent(m_phEvents[nThreadIdx]);
	}

	return TRUE;
}

int CHttpDownloadManager::FindIdleThread(BOOL bDownloadFile)
{
	// first n thread will be only be used to download data
	int start = bDownloadFile ? m_nDataThreadCount : 0;	

	for(int i = start; i < m_nMaxThreadCount; i++)
	{
		BOOL bBusy = InterlockedCompareExchange((LONG*)&m_pbThreadBusy[i], TRUE, FALSE);
		if( !bBusy )
			return i;
	}

	return -1;
}

bool CompareHttpTaskPriority(CHttpDownloadTask* pTask1, CHttpDownloadTask* pTask2)  
{      
	int nPriority1 = pTask1->GetPriority();
	int nPriority2 = pTask2->GetPriority();

	if( nPriority1 <= nPriority2)
		return false; 

	return true; 
}
 

CHttpDownloadTask* CHttpDownloadManager::TakeTask(int nThreadIdx)
{
	CHttpDownloadTask* pTask = NULL;

	EnterCriticalSection(&m_TaskLock);

	// sort by task priority
	if( m_deqTasks.size() > 1 )
		sort(m_deqTasks.begin(), m_deqTasks.end(), CompareHttpTaskPriority);


	if( nThreadIdx < m_nDataThreadCount )
	{
		// only take task which is downloading data
		deque<CHttpDownloadTask*>::iterator itr;
		for(itr = m_deqTasks.begin(); itr != m_deqTasks.end(); itr++)
		{
			// cannot be paused
			CHttpDownloadTask* pTemp = *itr;
			if( !pTemp->IsDownloadFile() && !pTemp->IsPaused() )		
			{
				pTask = pTemp;
				m_deqTasks.erase(itr);
				break;
			}
		}
	}
	else
	{
		// take any type of task  <data/file>
		if( m_deqTasks.size() != 0 )
		{
			pTask = m_deqTasks.front();

			// paused tasks are all put to last of deque
			if( pTask->IsPaused() )			
				pTask = NULL;				// no more task to be executed
			else
				m_deqTasks.pop_front();
		}

	}
	
	LeaveCriticalSection(&m_TaskLock);

	return pTask;
}

void CHttpDownloadManager::HttpDownload(int nThreadIdx)
{
	while( TRUE )
	{
		// obtain one task from deque
		DWORD ret = WaitForSingleObject(m_phEvents[nThreadIdx], INFINITE);
		if( ret == WAIT_FAILED )
			break;

		CHttpDownloadTask* pTask = TakeTask(nThreadIdx);
		if( pTask == NULL )
			continue;

		// start to download
		pTask->Start();
		BOOL bPaused = pTask->IsPaused();
		
		EnterCriticalSection(&m_TaskLock);

		if( bPaused )
		{
			// need to add to deque again
			m_deqTasks.push_back(pTask);
		}
		else
		{
			// after done then remove this task
			m_mapTasks.erase(pTask->GetTaskId());

			delete pTask;
			pTask = NULL;
		}


		// check whether got task
		if( m_deqTasks.size() != 0 )
			SetEvent(m_phEvents[nThreadIdx]);

		LeaveCriticalSection(&m_TaskLock);

		
		// set idle state
		InterlockedCompareExchange((LONG*)&m_pbThreadBusy[nThreadIdx], FALSE, TRUE);
	}
}

DWORD WINAPI CHttpDownloadManager::DownloadThread(LPARAM lParam)
{
	g_pHttpManager->HttpDownload(lParam);
	return 0;
}
