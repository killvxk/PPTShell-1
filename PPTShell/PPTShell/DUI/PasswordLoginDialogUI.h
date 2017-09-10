#pragma once
#include "DUICommon.h"

class CPasswordLoginDialogUI: public WindowImplBase
{
public:
	CPasswordLoginDialogUI(void);
	~CPasswordLoginDialogUI(void);

	UIBEGIN_MSG_MAP
		EVENT_BUTTON_CLICK(_T("btnClose"),OnBtnCloseClick);
		EVENT_BUTTON_CLICK(_T("btnCancel"),OnBtnCloseClick);
		EVENT_BUTTON_CLICK(_T("btnOK"),OnBtnOKClick);
		EVENT_ID_HANDLER(DUI_MSGTYPE_KILLFOCUS,_T("password"),OnEditPasswordKillfocus);
	UIEND_MSG_MAP

	virtual CDuiString GetSkinFolder();
	virtual CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName(void) const;
	virtual	void InitWindow();

	void OnBtnCloseClick(TNotifyUI& msg);
	void SetTitleWords(tstring title);

	void OnBtnOKClick(TNotifyUI& msg);
	void OnEditPasswordKillfocus(TNotifyUI& msg);
	
	tstring m_title;
};
