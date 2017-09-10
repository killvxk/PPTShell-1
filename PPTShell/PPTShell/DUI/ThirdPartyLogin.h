#pragma once

#include "DUICommon.h"
#include "Util/Util.h"
#include "Util/Singleton.h"
#include "ThirdParty/cef/cefobj.h"

class CThirdPartyLoginUI : public WindowImplBase
{
public:
	CThirdPartyLoginUI();
	~CThirdPartyLoginUI();

	UIBEGIN_MSG_MAP
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("close"),	OnCloseBtn);

	UIEND_MSG_MAP

private:
	void				OnCloseBtn(TNotifyUI& msg);
public:
	LPCTSTR GetWindowClassName() const;	

	virtual void InitWindow();

	virtual CDuiString GetSkinFile();

	virtual CDuiString GetSkinFolder();

	virtual CControlUI* CreateControl(LPCTSTR pstrClass);

	void	Init( int nType);
	void    ResetUrl();

	bool	Initialize( HINSTANCE hInstance );

	static LRESULT CALLBACK WndProc(
		HWND hwnd,      // handle to window
		UINT uMsg,      // message identifier
		WPARAM wParam,  // first message parameter
		LPARAM lParam   // second message parameter
		);

	bool OnThirdLogin(void* pParam);
private:
	CButtonUI*		m_pCloseBtn;
	CLabelUI*		m_pTitleLabel;
private:
	//CWebBrowserUI*	m_pWebBrowser;
	HWND			m_hWebWindow;

	HINSTANCE		m_hInstance;

	tstring			m_strWndClassName;

	int				m_nType;
};

typedef Singleton<CThirdPartyLoginUI> ThirdPartyLoginUI;