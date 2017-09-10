#pragma once
#include "stdafx.h"
#include "DUICommon.h"
#include "Util\Singleton.h"

#define FAIL_TIMER_ID 90101

class CShowIcrFailInfo : public WindowImplBase/*, public IListCallbackUI*/
{
public:
	CShowIcrFailInfo(void);
	~CShowIcrFailInfo(void);

	UIBEGIN_MSG_MAP   
		EVENT_BUTTON_CLICK(_T("Loading_CloseBtn"),OnBtnCloseClick); 
	UIEND_MSG_MAP
 

public:
	LPCTSTR GetWindowClassName() const;	
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void InitWindow();
   
	void	ShowLocalWindows(HWND hParent = NULL);	  
	bool	OnCloseWndEvent(void* pObj);
	void	OnBtnCloseClick(TNotifyUI& msg); 
protected:    
	CLabelUI*	m_pFailInfo; 

	int m_nCx;
	int m_nCy;
	DWORD m_nStartTime;
};


typedef Singleton<CShowIcrFailInfo>		ShowIcrFailInfoUI;
