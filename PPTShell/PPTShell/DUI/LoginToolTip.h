#pragma once
#include "stdafx.h"
#include "DUICommon.h"
#include "Util\Singleton.h"

#define TEACHER_TIMER_ID 90301

class CLoginToolTip : public WindowImplBase/*, public IListCallbackUI*/
{
public:
	CLoginToolTip(void);
	~CLoginToolTip(void);

	UIBEGIN_MSG_MAP  
		EVENT_BUTTON_CLICK(_T("btn_Close"),OnBtnCloseClick);
	UIEND_MSG_MAP
 

public:
	LPCTSTR GetWindowClassName() const;	
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void InitWindow();
   
	void	ShowLocalWindows(POINT ptInfo, tstring sTeacherName, tstring sGradeName);	  
	bool	OnCloseWndEvent(void* pObj);
	void	OnBtnCloseClick( TNotifyUI& msg ); 
protected:   
	CLabelUI*	m_pTeacherName;
	CLabelUI*	m_pTeacherGradeName;
	POINT		m_ptPos;
	long		m_lStartTick;
};


typedef Singleton<CLoginToolTip>		LoginToolTipUI;
