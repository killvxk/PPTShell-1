//===========================================================================
// FileName:				PPTPack.h
//	
// Desc:					 
//============================================================================
#ifndef _PPTPACK_H_
#define _PPTPACK_H_

#include "Util/Singleton.h"


enum
{
	PACK_PPT_START = 0,
	PACK_PPT_SLIDER,
	PACK_PPT_ZIP,
	PACK_PPT_END,
	UNPACK_PPT_INIT_FINISH,
};
typedef struct PackNotify
{
	DWORD			dwTaskId;
	int				nPackStep;
	DWORD			dwRet;
	CEventSource	OnSetValue;
	CEventSource	OnProgress;
	CEventSource	OnCompleted;
	tstring			strPath;
	tstring			strFailPath;
	bool			bCancel;
}PackNotify;

class CPPTPack
{
private:
	CPPTPack();
	virtual ~CPPTPack();

public:


	DECLARE_SINGLETON_CLASS(CPPTPack);

//	PackNotify* m_pPackNotify ;
public:

	DWORD Packing(LPCTSTR lptcsPath, CDelegateBase& OnSetValue, CDelegateBase& OnProgress, CDelegateBase& OnCompleted);
	DWORD UnPacking(LPCTSTR lptcsPath, CDelegateBase& OnSetValue, CDelegateBase& OnProgress, CDelegateBase& OnCompleted);

	void CancelPack(DWORD dwTaskId);

	void SetOwner( HWND hWnd );
	HWND GetOwner() {return m_hOwner;}
private:
	static	void	PackingThread(void* param);
	static	void	UnPackingThread(void* param);
	

private:
	
	HWND m_hOwner;

	static HANDLE m_hPackingThread;
	static HANDLE m_hUnPackingThread;
	static	deque<PackNotify*>		m_deqPackTasks;
	static	map<DWORD,PackNotify*>	m_mapPackTasks;
	static	deque<PackNotify*>		m_deqUnPackTasks;
	static	map<DWORD,PackNotify*>	m_mapUnPackTasks;

	static	CRITICAL_SECTION		m_lock;
};

typedef Singleton<CPPTPack> PPTPack;

#endif