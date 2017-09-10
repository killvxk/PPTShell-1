#pragma once
#include "DUICommon.h"
#include "Update/UpdateOperation.h"

#define UPDATE_REFRESH_TEXT 1

class CDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public:
	CControlUI* CreateControl(LPCTSTR pstrClass)
	{
		return NULL;
	}
};

class CForceUpdate : public WindowImplBase
{
public:

	CForceUpdate(void);
	~CForceUpdate(void);

	LPCTSTR GetWindowClassName() const { return _T("ForceUpdate"); };
	UINT GetClassStyle() const { return CS_DBLCLKS; };
	bool	OnCheckUpdate( void* pObj );

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
//	void LoadSkin();

//	void Notify(TNotifyUI& msg);

	static void  TimerProcComplete(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime);

	virtual CDuiString GetSkinFile()
	{
		return _T("ForceUpdate\\ForceUpdate.xml");
	}

	virtual CDuiString GetSkinFolder()
	{
		return CDuiString(_T("skins"));
	}

	void UpdateConfig();
public:
	int m_nTimer;
	CLabelUI*	m_pLabel;

	CUpdate m_update;

	virtual void InitWindow();
};
