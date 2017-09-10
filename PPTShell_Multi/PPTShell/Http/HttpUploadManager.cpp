//-----------------------------------------------------------------------
// FileName:				HttpDownloadManager.cpp
//
// Desc:
//------------------------------------------------------------------------
#include "stdafx.h"
#include "HttpDelegate.h"
#include "HttpUploadManager.h"
#include "Util/Util.h"
#include <algorithm>
#include <assert.h>

CHttpUploadManager*	g_pHttpManager = NULL;

CHttpUploadManager::CHttpUploadManager()
{
	m_nCurThreadCount	= 0;
	m_nMaxThreadCount	= 0;
	m_phEvents			= NULL;
	m_phThreads			= NULL;
	m_hOwnerWnd			= NULL;

	g_pHttpManager		= this;
}

CHttpUploadManager::~CHttpUploadManager()
{

}

//
// initialize
//
BOOL CHttpUploadManager::Initialize(int nMaxThreadCount /* = 10 */, int nDataThreadCount /*= 2*/)
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
	m_phThreads[m_nCurThreadCount] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)UploadThread, 0, 0, NULL);
	m_nCurThreadCount ++;

	return TRUE;
}

BOOL CHttpUploadManager::Destroy()
{
	EnterCriticalSection(&m_TaskLock);
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

	delete m_phEvents;
	m_phEvents = NULL;

	delete m_phThreads;
	m_phThreads = NULL;

	delete m_pbThreadBusy;
	m_pbThreadBusy = NULL;

	LeaveCriticalSection(&m_TaskLock);
	DeleteCriticalSection(&m_TaskLock);
	return TRUE;
}

BOOL CHttpUploadManager::SetOwner(HWND hWnd)
{
	m_hOwnerWnd = hWnd;
	return TRUE;
}

BOOL CHttpUploadManager::SetTaskPriority(DWORD dwTaskId, int nPriority)
{
	EnterCriticalSection(&m_TaskLock);
	map<DWORD, CHttpUploadTask*>::iterator itr = m_mapTasks.find(dwTaskId);
	if( itr != m_mapTasks.end() )
	{
		CHttpUploadTask* pTask = itr->second;
		pTask->SetPriority(nPriority);
	}

	LeaveCriticalSection(&m_TaskLock);
	return TRUE;
}

DWORD CHttpUploadManager::AddUploadTask( LPCTSTR lpcstrHost, 
										  LPCTSTR lpcstrUrl, 
										  LPCTSTR  lpcstrFilePath,
										  LPCTSTR lpcstrServerFilePath, 
										  LPCTSTR lpcstrMethod,
										  int nPort, 
										  CHttpDelegateBase& OnCompleteDelegate, 
										  CHttpDelegateBase& OnProgressDelegate, 
										  BOOL bCallBackInMainThread /*= TRUE*/, 
										  BOOL  bReplaceFile,
										  PVOID pUserData)
{
	CHttpUploadTask* pTask = new CHttpUploadTask(
		lpcstrHost, 
		lpcstrUrl, 
		lpcstrFilePath, 
		lpcstrServerFilePath, 
		lpcstrMethod, 
		nPort, 
		OnCompleteDelegate,
		OnProgressDelegate,
		bReplaceFile,
		pUserData);

	if( pTask == NULL )
		return 0;

	// set owner
	if( bCallBackInMainThread )
		pTask->SetOwner(m_hOwnerWnd);

	// generate a random value to identify this task
	DWORD dwTaskId = (DWORD)pTask ^ (DWORD)GetCycleCount();

	pTask->SetTaskId(dwTaskId);


	// save to deque
	EnterCriticalSection(&m_TaskLock);

	// cannot has same task id
	map<DWORD, CHttpUploadTask*>::iterator itr = m_mapTasks.find(dwTaskId);
	assert(itr == m_mapTasks.end());

	m_deqTasks.push_back(pTask);
	m_mapTasks[dwTaskId] = pTask;
	LeaveCriticalSection(&m_TaskLock);

	// find idle thread
	int nThreadIdx = FindIdleThread(FALSE);
	if( nThreadIdx != -1 )
	{
		// check thread whether has been created
		if( m_phThreads[nThreadIdx] == NULL )
		{
			m_phEvents[nThreadIdx] = CreateEvent(NULL, FALSE, FALSE, NULL);
			m_phThreads[nThreadIdx] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)UploadThread, (LPVOID)nThreadIdx, 0, NULL);
		}

		SetEvent(m_phEvents[nThreadIdx]);
	}

	return dwTaskId;
}

// BOOL CHttpUploadManager::AddTaskCallBack(DWORD dwTaskId, CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressDelegate)
// {
// 	EnterCriticalSection(&m_TaskLock);
// 	map<DWORD, CHttpUploadTask*>::iterator itr = m_mapTasks.find(dwTaskId);
// 	if( itr != m_mapTasks.end() )
// 	{
// 		CHttpUploadTask* pTask = itr->second;
// 		pTask->AddTaskCallBack(OnCompleteDelegate, OnProgressDelegate);
// 	}
// 
// 	LeaveCriticalSection(&m_TaskLock);
// 
// 	return TRUE;
// }

BOOL CHttpUploadManager::CancelTask(DWORD dwTaskId)
{
	EnterCriticalSection(&m_TaskLock);

	CHttpUploadTask* pFoundTask = NULL;
	BOOL bUploading = FALSE;

	// remove from map
	map<DWORD, CHttpUploadTask*>::iterator itr = m_mapTasks.find(dwTaskId);
	if( itr != m_mapTasks.end() )
	{
		pFoundTask = itr->second;
		bUploading = pFoundTask->IsUploading();

		assert(pFoundTask != NULL);
		pFoundTask->Stop();

		m_mapTasks.erase(dwTaskId);
	}

	// remove from deque
	deque<CHttpUploadTask*>::iterator itr_deq;
	for(itr_deq = m_deqTasks.begin(); itr_deq != m_deqTasks.end(); itr_deq++)
	{
		CHttpUploadTask* pTask = *itr_deq;
		if( pTask->GetTaskId() == dwTaskId )
		{
			m_deqTasks.erase(itr_deq);
			break;
		}
	}

	if( pFoundTask != NULL && !bUploading )
	{
		delete pFoundTask;
		pFoundTask = NULL;
	}

	LeaveCriticalSection(&m_TaskLock);
	return TRUE;
}

int CHttpUploadManager::FindIdleThread(BOOL bDownloadFile)
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

bool CompareHttpTaskPriority(CHttpUploadTask* pTask1, CHttpUploadTask* pTask2)  
{      
	int nPriority1 = pTask1->GetPriority();
	int nPriority2 = pTask2->GetPriority();

	if( nPriority1 <= nPriority2)
		return false; 

	return true; 
}


CHttpUploadTask* CHttpUploadManager::TakeTask(int nThreadIdx)
{
	CHttpUploadTask* pTask = NULL;

	EnterCriticalSection(&m_TaskLock);

	// sort by task priority
	if( m_deqTasks.size() > 1 )
		sort(m_deqTasks.begin(), m_deqTasks.end(), CompareHttpTaskPriority);


// 	if( nThreadIdx < m_nDataThreadCount )
// 	{
// 		// only take task which is downloading data
// 		deque<CHttpUploadTask*>::iterator itr;
// 		for(itr = m_deqTasks.begin(); itr != m_deqTasks.end(); itr++)
// 		{
// 			CHttpUploadTask* pTemp = *itr;
// 			if( !pTemp->IsDownloadFile() )
// 			{
// 				pTask = pTemp;
// 				m_deqTasks.erase(itr);
// 				break;
// 			}
// 		}
// 	}
// 	else
// 	{
		// take any type of task  <data/file>
		if( m_deqTasks.size() != 0 )
		{
			pTask = m_deqTasks.front();
			m_deqTasks.pop_front();
		}

//	}

	LeaveCriticalSection(&m_TaskLock);

	return pTask;
}

void CHttpUploadManager::HttpUpload(int nThreadIdx)
{
	while( TRUE )
	{
		// obtain one task from deque
		DWORD ret = WaitForSingleObject(m_phEvents[nThreadIdx], INFINITE);
		if( ret == WAIT_FAILED )
			break;

		CHttpUploadTask* pTask = TakeTask(nThreadIdx);
		if( pTask == NULL )
			continue;

		// start to download
		pTask->Start();

		// after done then remove this task
		EnterCriticalSection(&m_TaskLock);
		m_mapTasks.erase(pTask->GetTaskId());

		delete pTask;
		pTask = NULL;

		// check whether got task
		if( m_deqTasks.size() != 0 )
			SetEvent(m_phEvents[nThreadIdx]);

		LeaveCriticalSection(&m_TaskLock);


		// set idle state
		InterlockedCompareExchange((LONG*)&m_pbThreadBusy[nThreadIdx], FALSE, TRUE);
	}
}

DWORD WINAPI CHttpUploadManager::UploadThread(LPARAM lParam)
{
	g_pHttpManager->HttpUpload(lParam);
	return 0;
}
