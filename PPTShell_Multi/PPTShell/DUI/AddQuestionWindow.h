#pragma once

#include "DUICommon.h"
#include "Util/Singleton.h"


class CAddQuestionWindow : public WindowImplBase
{
public:
	CAddQuestionWindow(void);
	~CAddQuestionWindow(void);
	UIBEGIN_MSG_MAP
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("AddQuestion"),		OnAddQuestionBtn);
	UIEND_MSG_MAP

public:
	LPCTSTR				GetWindowClassName() const;	

	virtual void		InitWindow();

	virtual CDuiString	GetSkinFile();

	virtual CDuiString	GetSkinFolder();
	BOOL SetQuestionInfo( tstring szPath, tstring szQuestionType, tstring szGuid);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void Hide();
private:
	void				OnAddQuestionBtn(TNotifyUI& msg);
	tstring m_szPath;
	tstring m_szQuestionType;
	tstring m_szGuid;

};

typedef Singleton<CAddQuestionWindow>	AddQuestionUI;