#pragma once
#include "DUICommon.h"

class CQRCodeDialogUI : public WindowImplBase
{
public:
	CQRCodeDialogUI(void);
	~CQRCodeDialogUI(void);

	UIBEGIN_MSG_MAP
		EVENT_BUTTON_CLICK(_T("CloseBtn"),			OnBtnClose);
	UIEND_MSG_MAP

protected:
	virtual CDuiString GetSkinFolder();
	virtual CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName(void) const;
	virtual	void InitWindow();
	virtual void OnFinalMessage( HWND hWnd );

	void		 OnBtnClose(TNotifyUI& msg);
	bool		 OnBlueToothChanged(void* param);

protected:
	CDuiString		m_strBkImage;
};
