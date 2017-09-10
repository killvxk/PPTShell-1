//===========================================================================
// FileName:				PPTPack.cpp
//	
// Desc:					 
//============================================================================
#include "stdafx.h"
#include "PPTPack.h"
#include "PPTController.h"
#include "PPTControllerManager.h"

deque<PackNotify*>		CPPTPack::m_deqPackTasks;
map<DWORD,PackNotify*>	CPPTPack::m_mapPackTasks;
CRITICAL_SECTION		CPPTPack::m_lock;
HANDLE					CPPTPack::m_hPackingThread;
deque<PackNotify*>		CPPTPack::m_deqUnPackTasks;
map<DWORD,PackNotify*>	CPPTPack::m_mapUnPackTasks;
HANDLE					CPPTPack::m_hUnPackingThread;
CPPTPack::CPPTPack()
{
	InitializeCriticalSection(&m_lock);
	m_hPackingThread = NULL;
	m_hUnPackingThread = NULL;
}

CPPTPack::~CPPTPack()
{
	
}

DWORD CPPTPack::Packing(LPCTSTR lptcsPath, CDelegateBase& OnSetValue, CDelegateBase& OnProgress, CDelegateBase& OnCompleted )
{
	PackNotify* pPackNotify = new PackNotify;
	memset(pPackNotify, 0x00, sizeof(PackNotify));
	pPackNotify->strPath = lptcsPath;
	pPackNotify->OnProgress += OnProgress;
	pPackNotify->OnCompleted += OnCompleted;
	pPackNotify->OnSetValue += OnSetValue;
	pPackNotify->dwTaskId = GetTickCount();
	EnterCriticalSection(&m_lock);
	if (!m_hPackingThread)
	{
		m_hPackingThread = (HANDLE)_beginthread(PackingThread, 0, this);
	}
	m_deqPackTasks.push_back(pPackNotify);
	m_mapPackTasks.insert(make_pair(pPackNotify->dwTaskId, pPackNotify));
	LeaveCriticalSection(&m_lock);

	return pPackNotify->dwTaskId;
}

DWORD CPPTPack::UnPacking( LPCTSTR lptcsPath, CDelegateBase& OnSetValue, CDelegateBase& OnProgress, CDelegateBase& OnCompleted )
{
	PackNotify* pPackNotify = new PackNotify;
	memset(pPackNotify, 0x00, sizeof(PackNotify));
	pPackNotify->strPath = lptcsPath;
	pPackNotify->OnProgress += OnProgress;
	pPackNotify->OnCompleted += OnCompleted;
	pPackNotify->OnSetValue += OnSetValue;
	pPackNotify->dwTaskId = GetTickCount();
	EnterCriticalSection(&m_lock);
	if (!m_hUnPackingThread)
	{
		m_hUnPackingThread = (HANDLE)_beginthread(UnPackingThread, 0, this);
	}
	m_deqUnPackTasks.push_back(pPackNotify);
	m_mapUnPackTasks.insert(make_pair(pPackNotify->dwTaskId, pPackNotify));
	LeaveCriticalSection(&m_lock);

	return pPackNotify->dwTaskId;
}

void CPPTPack::PackingThread(void* param)
{
	CPPTPack* pPPTPack = (CPPTPack*)param;
	PackNotify* pPackNotify			= NULL;
	while(true)
	{	
		EnterCriticalSection(&m_lock);
		if (pPackNotify)
		{
			map<DWORD,PackNotify*>::iterator itor = m_mapPackTasks.find(pPackNotify->dwTaskId);
			if (itor != m_mapPackTasks.end())
			{
				m_mapPackTasks.erase(itor);
			}
			delete pPackNotify;
			pPackNotify = NULL;
		}
		if (m_deqPackTasks.empty())
		{
			LeaveCriticalSection(&m_lock);
			break;
		}
		pPackNotify = m_deqPackTasks.front();
		m_deqPackTasks.pop_front();
		LeaveCriticalSection(&m_lock);

		tstring strPath = pPackNotify->strPath;

		DWORD dwRet = -1;
		if(GetFileAttributes(strPath.c_str()) != INVALID_FILE_ATTRIBUTES)
		{
			CPPTController* pControl = GetPPTController();

			if ( pControl->IsInit() )
			{
				dwRet = pControl->PackAllResource(pPPTPack->GetOwner(), pPackNotify, (TCHAR *)strPath.c_str());
			}
		}
		pPackNotify->nPackStep = PACK_PPT_END;
		pPackNotify->dwRet = dwRet;
		::SendMessage(pPPTPack->GetOwner(), WM_USER_PACK_PPT, (WPARAM)pPackNotify, (LPARAM)NULL);
	}

	EnterCriticalSection(&m_lock);
	m_hPackingThread = NULL;
	LeaveCriticalSection(&m_lock);
}

void CPPTPack::UnPackingThread(void* param)
{
	CPPTPack* pPPTPack = (CPPTPack*)param;
	PackNotify* pPackNotify			= NULL;
	while(true)
	{	
		EnterCriticalSection(&m_lock);
		if (pPackNotify)
		{
			map<DWORD,PackNotify*>::iterator itor = m_mapUnPackTasks.find(pPackNotify->dwTaskId);
			if (itor != m_mapUnPackTasks.end())
			{
				m_mapUnPackTasks.erase(itor);
			}
			delete pPackNotify;
			pPackNotify = NULL;
		}
		if (m_mapUnPackTasks.empty())
		{
			LeaveCriticalSection(&m_lock);
			break;
		}
		pPackNotify = m_deqUnPackTasks.front();
		m_deqUnPackTasks.pop_front();
		LeaveCriticalSection(&m_lock);

		tstring strPath = pPackNotify->strPath;

		DWORD dwRet = -1;
		if(GetFileAttributes(strPath.c_str()) != INVALID_FILE_ATTRIBUTES)
		{
			CPPTController* pControl = GetPPTController();

			if ( pControl->IsInit() )
			{
				dwRet = pControl->UnPackAllResource(pPPTPack->GetOwner(), pPackNotify, (TCHAR *)strPath.c_str());
			}
		}
		pPackNotify->nPackStep = UNPACK_PPT_INIT_FINISH;
		pPackNotify->dwRet = dwRet;
		::SendMessage(pPPTPack->GetOwner(), WM_USER_PACK_PPT, (WPARAM)pPackNotify, (LPARAM)NULL);
	}

	EnterCriticalSection(&m_lock);
	m_hUnPackingThread = NULL;
	LeaveCriticalSection(&m_lock);
}

void CPPTPack::SetOwner( HWND hWnd )
{
	m_hOwner = hWnd;
}

void CPPTPack::CancelPack( DWORD dwTaskId )
{
	if (!dwTaskId)
	{
		return;
	}

	EnterCriticalSection(&m_lock);
	map<DWORD,PackNotify*>::iterator itor = m_mapPackTasks.find(dwTaskId);
	if (itor != m_mapPackTasks.end())
	{
		itor->second->OnProgress.clear();
		itor->second->OnCompleted.clear();
		itor->second->bCancel = true;
	}

	itor = m_mapUnPackTasks.find(dwTaskId);
	if (itor != m_mapUnPackTasks.end())
	{
		itor->second->OnProgress.clear();
		itor->second->OnCompleted.clear();
		itor->second->bCancel = true;
	}
	LeaveCriticalSection(&m_lock);
}
