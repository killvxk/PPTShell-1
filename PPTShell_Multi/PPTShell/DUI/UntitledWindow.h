#pragma once
#include "DUICommon.h"

class CUntitledWindow : public WindowImplBase
{
public:
	CUntitledWindow(void);
	~CUntitledWindow(void);

	UIBEGIN_MSG_MAP
		EVENT_BUTTON_CLICK(_T("OpenBtn"),	OnOpenBtn);
		EVENT_BUTTON_CLICK(	_T("NewBtn"),	OnNewBtn);
	UIEND_MSG_MAP

public:
	void SetNewButtonEnabled(bool bEnabled);

protected:
	virtual CDuiString GetSkinFolder();
	virtual CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName(void) const;

	virtual void InitWindow();


	void OnOpenBtn(TNotifyUI& msg);
	void OnNewBtn(TNotifyUI& msg);

	CButtonUI* m_pBtnNew;
};
