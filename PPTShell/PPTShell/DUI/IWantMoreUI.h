#pragma once
#include "DUICommon.h"

#define IWant_host		_T("p.101.com")
#define IWant_PostUrl	_T("101ppt/iWant.php")

class CIWantMoreUI: public WindowImplBase
{
	UIBEGIN_MSG_MAP
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,_T("btnClose"),OnBtnCloseClick);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,_T("btnSubmit"),OnBtnSubmitClick);
		EVENT_ID_HANDLER(DUI_MSGTYPE_TEXTCHANGED,_T("editContent"),OnEditContentTextChanged);
	UIEND_MSG_MAP
public:
	CIWantMoreUI(void);
	~CIWantMoreUI(void);
	void Init(CRect rect);
private:
	virtual CDuiString GetSkinFolder();
	virtual CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName(void) const;
	virtual void InitWindow();

	void OnBtnCloseClick(TNotifyUI& msg);
	void OnBtnSubmitClick(TNotifyUI& msg);
	void OnEditContentTextChanged(TNotifyUI& msg);
	bool OnPostIWantCompeleted(void* param);

	DWORD m_dwPostIWantTaskId;

	CRichEditUI* m_pEditContent;
	CButtonUI* m_pBtnSubmit;
};
