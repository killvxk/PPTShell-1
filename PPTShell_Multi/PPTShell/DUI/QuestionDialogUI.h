#pragma once
#include "DUICommon.h"
#include "WndShadow.h"

class CQuestionDialogUI : public WindowImplBase
{
public:
	CQuestionDialogUI(void);
	~CQuestionDialogUI(void);

	UIBEGIN_MSG_MAP
		EVENT_BUTTON_CLICK(_T("CloseBtn"),				OnBtnClose);
		EVENT_BUTTON_CLICK(_T("item"),					OnItemClick);
	UIEND_MSG_MAP

protected:
	virtual CDuiString	GetSkinFolder();
	virtual CDuiString	GetSkinFile();
	virtual LPCTSTR		GetWindowClassName(void) const;
	virtual void		InitWindow();

	void				CreateBaseList();
	void				CreateInteractionList();

private:
	void				OnBtnClose(TNotifyUI& msg);
	void				OnItemClick(TNotifyUI& msg);

public:
	void				CreateDlg();
	LPCTSTR				GetQuestionType();
	bool				IsBasicQuestion();
	

private:
//	CWndShadow			m_WndShadow;
	tstring				m_strQuestionType;
	bool				m_bBasicQuestion;

	CTileLayoutUI*		m_layBaseList;
	CTileLayoutUI*		m_layInteractionList;
	
};
