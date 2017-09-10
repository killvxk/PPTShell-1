//-----------------------------------------------------------------------
// FileName:				EventCenter.cpp
//
// Desc:
//------------------------------------------------------------------------
#include "stdafx.h"
#include "EventDefine.h"
#include "EventCenter.h"


//------------------------------------------------------------
// event listener
//
CEventListener::CEventListener(int nEventType)
{
	m_nEventType = nEventType;
	InitializeCriticalSection(&m_Lock);

}

CEventListener::~CEventListener()
{
	DeleteCriticalSection(&m_Lock);
}

void CEventListener::AddDelegate(CEventDelegateBase& delegate)
{
	EnterCriticalSection(&m_Lock);
	m_EventSource += delegate;
	LeaveCriticalSection(&m_Lock);
}

void CEventListener::DelDelegate(CEventDelegateBase& delegate)
{
	EnterCriticalSection(&m_Lock);
	m_EventSource -= delegate;
	LeaveCriticalSection(&m_Lock);
}

void CEventListener::Broadcast(WPARAM wParam, LPARAM lParam, void* pSender)
{
	EnterCriticalSection(&m_Lock);
	TEventNotify notify;
	notify.pSender		= pSender;
	notify.nEventType	= m_nEventType;
	notify.wParam		= wParam;
	notify.lParam		= lParam;

	m_EventSource(&notify);
	LeaveCriticalSection(&m_Lock);
}


//------------------------------------------------------------
// event center
//
CEventCenter::CEventCenter()
{
	InitializeCriticalSection(&m_Lock);
	m_hOwnerWnd		= NULL;
}

CEventCenter::~CEventCenter()
{
	DeleteCriticalSection(&m_Lock);
}

//
// message WM_USER_EVENT will send to this window
//
BOOL CEventCenter::Initialize()
{
	return TRUE;
}

BOOL CEventCenter::Destroy()
{
	EnterCriticalSection(&m_Lock);
	map<int, CEventListener*>::iterator itr;
	for(itr = m_mapEventListeners.begin(); itr != m_mapEventListeners.end(); itr++)
	{
		CEventListener* pListener = itr->second;
		delete pListener;
		pListener = NULL;
	}

	m_mapEventListeners.clear();
	LeaveCriticalSection(&m_Lock);
	
	
	return TRUE;
}

BOOL CEventCenter::RegisterEventListener(int nEventType, CEventDelegateBase& delegate)
{
	EnterCriticalSection(&m_Lock);
	map<int, CEventListener*>::iterator itr = m_mapEventListeners.find(nEventType);
	if( itr != m_mapEventListeners.end() )
	{
		CEventListener* pListener = itr->second;
		pListener->AddDelegate(delegate);
	}
	else
	{
		CEventListener* pListener = new CEventListener(nEventType);
		pListener->AddDelegate(delegate);
		m_mapEventListeners[nEventType] = pListener;
		
	}
	 
	LeaveCriticalSection(&m_Lock);
	return TRUE;
}

BOOL CEventCenter::UnRegisterEventListener(int nEventType, CEventDelegateBase &delegate)
{
	EnterCriticalSection(&m_Lock);
	map<int, CEventListener*>::iterator itr = m_mapEventListeners.find(nEventType);
	if( itr != m_mapEventListeners.end() )
	{
		CEventListener* pListener = itr->second;
		pListener->DelDelegate(delegate);
	}

	LeaveCriticalSection(&m_Lock);
	return TRUE;
}

BOOL CEventCenter::BroadcastEvent(int nEventType, WPARAM wParam, LPARAM lParam, void* pSender)
{
	EnterCriticalSection(&m_Lock);
	map<int, CEventListener*>::iterator itr = m_mapEventListeners.find(nEventType);
	if( itr != m_mapEventListeners.end() )
	{
		CEventListener* pListener = itr->second;
		pListener->Broadcast(wParam, lParam, pSender);
	}

	LeaveCriticalSection(&m_Lock);

	return TRUE;
}

void CEventCenter::SetOwner(HWND hWnd)
{
	m_hOwnerWnd = hWnd;
}

HWND CEventCenter::GetOwner()
{
	return m_hOwnerWnd;
}

//--------------------------------------------------------------------------
// interfaces
//
void OnEvent(int nEventType, CEventDelegateBase& delegate)
{
	EventCenter::GetInstance()->RegisterEventListener(nEventType, delegate);
}

void CancelEvent(int nEventType, CEventDelegateBase& delegate)
{
	EventCenter::GetInstance()->UnRegisterEventListener(nEventType, delegate);
}

void BroadcastEvent(int nEventType, WPARAM wParam, LPARAM lParam, void* pSender)
{
	CStream stream(256);
	stream.WriteDWORD(nEventType);
	stream.WriteDWORD(wParam);
	stream.WriteDWORD(lParam);
	stream.WriteDWORD((DWORD)pSender);

	SendMessage(EventCenter::GetInstance()->GetOwner(), WM_USER_SEND_EVENT, (WPARAM)&stream, 0);
}

void BroadcastPostedEvent(int nEventType, WPARAM wParam, LPARAM lParam, void* pSender)
{
	CStream* pStream = new CStream(256);
	pStream->WriteDWORD(nEventType);
	pStream->WriteDWORD(wParam);
	pStream->WriteDWORD(lParam);
	pStream->WriteDWORD((DWORD)pSender);

	PostMessage(EventCenter::GetInstance()->GetOwner(), WM_USER_POST_EVENT, (WPARAM)pStream, 1);
}


void BroadcastEvent(int nEventType, CStream* pStream, void* pSender)
{
	pStream->ResetCursor();

	CStream stream(256);
	stream.WriteDWORD(nEventType);
	stream.WriteDWORD((DWORD)pStream);
	stream.WriteDWORD(0);
	stream.WriteDWORD((DWORD)pSender);

	SendMessage(EventCenter::GetInstance()->GetOwner(), WM_USER_SEND_EVENT, (WPARAM)&stream, 0);
}


void BroadcastEventInThread(int nEventType, WPARAM wParam, LPARAM lParam, void* pSender)
{
	EventCenter::GetInstance()->BroadcastEvent(nEventType, wParam, lParam, pSender);
}

void BroadcastEventInThread(int nEventType, CStream* pStream, void* pSender)
{
	pStream->ResetCursor();
	EventCenter::GetInstance()->BroadcastEvent(nEventType, (WPARAM)pStream, 0, pSender);
}
