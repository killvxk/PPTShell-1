#pragma once
#include "DUICommon.h"

class CModifyPasswordDialogUI: public WindowImplBase
{
public:
	CModifyPasswordDialogUI(void);
	~CModifyPasswordDialogUI(void);

	UIBEGIN_MSG_MAP
		EVENT_BUTTON_CLICK(_T("btnClose"),OnBtnCloseClick);
		EVENT_BUTTON_CLICK(_T("btnCancel"),OnBtnCloseClick);
		EVENT_BUTTON_CLICK(_T("btnOK"),OnBtnOkClick);
		EVENT_ID_HANDLER(DUI_MSGTYPE_KILLFOCUS,_T("OldPassword"),OnEditOldPasswordKillfocus);
		EVENT_ID_HANDLER(DUI_MSGTYPE_KILLFOCUS,_T("ComfirmPassword"),OnEditComfirmPasswordKillfocus);
	UIEND_MSG_MAP

	virtual CDuiString GetSkinFolder();
	virtual CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName(void) const;
	virtual	void InitWindow();

	void OnBtnCloseClick(TNotifyUI& msg);

	void OnEditOldPasswordKillfocus(TNotifyUI& msg);
	bool CheckOldPassword();
	void OnBtnOkClick(TNotifyUI& msg);
	void OnEditComfirmPasswordKillfocus(TNotifyUI& msg);
	bool CheckNewPassword();
};
