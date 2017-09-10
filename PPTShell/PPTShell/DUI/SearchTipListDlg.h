//===========================================================================
// FileName:				LaserPointerDlg.h
//	
// Desc:					 
//============================================================================
#pragma once

#include "DUICommon.h"

class CSearchTipListDlg : public WindowImplBase
{
public:
	CSearchTipListDlg();
	~CSearchTipListDlg();
	UIBEGIN_MSG_MAP
		SUPER_HANDLER_MSG
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("tipListPPT"),	OnBtnTipListPPT);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("tipListBaidu"),	OnBtnTipListBaidu);
	UIEND_MSG_MAP
public:
	LPCTSTR GetWindowClassName() const;	

	virtual void InitWindow();

	virtual CDuiString GetSkinFile();

	virtual CDuiString GetSkinFolder();

	virtual CControlUI* CreateControl(LPCTSTR pstrClass);


	virtual LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

	void			SetKillFocusCallBack(CDelegateBase& delegate);
	void			SetSearchCallBack(CDelegateBase& delegate);
private:

	CEventSource	m_OnKillFocusCallBack;
	CEventSource	m_OnSearchCallBack;

	CButtonUI*		m_pBtnPPT;
	CButtonUI*		m_pBtnBaidu;

	CLabelUI*		m_pLabelPPT;
	CLabelUI*		m_pLabelBaidu;

protected:
	bool			OnBtnEvent(void* pObj);

	bool			OnBtnTipListPPT(TNotifyUI& msg);
	bool			OnBtnTipListBaidu(TNotifyUI& msg);
};

