#pragma once

#include "DUICommon.h"
#include "Util/Singleton.h"


class CUpdateTipDialogUI : public WindowImplBase
{
public:
	CUpdateTipDialogUI();
	~CUpdateTipDialogUI();
	UIBEGIN_MSG_MAP
	EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("startNewApp"),		OnStartNewAppBtn);
	EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("onLater"),	OnLaterwBtn);
	UIEND_MSG_MAP

public:
	LPCTSTR GetWindowClassName() const;	

	virtual void InitWindow();

	virtual CDuiString GetSkinFile();

	virtual CDuiString GetSkinFolder();

	virtual CControlUI* CreateControl(LPCTSTR pstrClass);

private:
	void				OnStartNewAppBtn(TNotifyUI& msg);
	void				OnLaterwBtn(TNotifyUI& msg);
	

};


typedef Singleton<CUpdateTipDialogUI>	UpdateTipDialogUI;
