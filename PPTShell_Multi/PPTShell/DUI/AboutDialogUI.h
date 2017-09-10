#pragma once
#include "DUICommon.h"

class CAboutDialogUI : public WindowImplBase
{
public:
	CAboutDialogUI(void);
	~CAboutDialogUI(void);

	UIBEGIN_MSG_MAP
		EVENT_BUTTON_CLICK(_T("btnClose"),OnBtnCloseClick);
	UIEND_MSG_MAP

private:
	virtual CDuiString GetSkinFolder();
	virtual CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName(void) const;
	virtual	void InitWindow();

	void OnBtnCloseClick(TNotifyUI& msg);
};
