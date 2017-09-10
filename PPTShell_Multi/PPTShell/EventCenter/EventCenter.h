//-----------------------------------------------------------------------
// FileName:				EventCenter.h
//
// Desc:
//------------------------------------------------------------------------
#ifndef _EVENT_CENTER_H_
#define _EVENT_CENTER_H_

#include "Util/Singleton.h"

class CEventListener
{
public:
	CEventListener(int nEventType);
	~CEventListener();

	void AddDelegate(CEventDelegateBase& delegate);
	void DelDelegate(CEventDelegateBase& delegate);
	void Broadcast(WPARAM wParam, LPARAM lParam, void* pSender);

protected:
	// delegate array
	int						m_nEventType;
	CNotifyEventSource		m_EventSource;
	CRITICAL_SECTION		m_Lock;


};

class CEventCenter
{
private:
	CEventCenter();
	~CEventCenter();

public:
	BOOL Initialize();
	BOOL Destroy();
	void SetOwner(HWND hWnd);
	HWND GetOwner();

	// register and unregister event listener
	BOOL RegisterEventListener(int nEventType, CEventDelegateBase& delegate);
	BOOL UnRegisterEventListener(int nEventType, CEventDelegateBase& delegate);

	// broadcast event
	BOOL BroadcastEvent(int nEventType, WPARAM wParam, LPARAM lParam, void* pSender);

	DECLARE_SINGLETON_CLASS(CEventCenter);

protected:
	map<int, CEventListener*>			m_mapEventListeners;		// EventType <--> CEventListener*
	CRITICAL_SECTION					m_Lock;
	HWND								m_hOwnerWnd;
};

typedef Singleton<CEventCenter>  EventCenter;

#endif