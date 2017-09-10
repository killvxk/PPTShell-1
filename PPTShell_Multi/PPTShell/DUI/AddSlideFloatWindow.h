#pragma once

#include "DUICommon.h"
#include "Util/Singleton.h"


class CAddSlideFloatWindow : public WindowImplBase
{
public:
	CAddSlideFloatWindow(void);
	~CAddSlideFloatWindow(void);
	UIBEGIN_MSG_MAP
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("AddSlide"),		OnAddSlideBtn);
	UIEND_MSG_MAP

public:
	LPCTSTR				GetWindowClassName() const;	

	virtual void		InitWindow();

	virtual CDuiString	GetSkinFile();

	virtual CDuiString	GetSkinFolder();

private:
	void				OnAddSlideBtn(TNotifyUI& msg);

};

typedef Singleton<CAddSlideFloatWindow>	AddSlideUI;