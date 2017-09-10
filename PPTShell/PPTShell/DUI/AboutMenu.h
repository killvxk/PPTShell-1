#pragma once
#include "DUICommon.h"

class CAboutMenu : public WindowImplBase/*, public IListCallbackUI*/
{
public:
	CAboutMenu(void);

	UIBEGIN_MSG_MAP
		EVENT_ITEM_CLICK(_T("menu_Update"),			OnBtnUpdate);
		EVENT_ITEM_CLICK(_T("menu_Log"),			OnBtnLog);
		EVENT_ITEM_CLICK(_T("menu_FeedBack"),		OnBtnFeedBack);
		EVENT_ITEM_CLICK(_T("menu_About"),			OnBtnAbout);
		EVENT_ITEM_CLICK(_T("menu_Options"),		OnBtnOptions); 
	UIEND_MSG_MAP

protected:
	~CAboutMenu(void);

public:
	LPCTSTR GetWindowClassName() const;	
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();

	void			Init(HWND hWndParent, POINT ptPos, bool bUpdate, tstring strVer, tstring strContent);
	virtual void    OnFinalMessage(HWND hWnd);
	//virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LPCTSTR	GetItemText(CControlUI* pList, int iItem, int iSubItem);

private:
	void		OnBtnUpdate(TNotifyUI& msg);
	void		OnBtnLog(TNotifyUI& msg);
	void		OnBtnFeedBack(TNotifyUI& msg);
	void		OnBtnAbout(TNotifyUI& msg); 
	void		OnBtnOptions(TNotifyUI& msg); 

protected:
	CContainerUI*	m_pDotContainer;
	BOOL			m_bUpdate;
	HWND			m_hWndParent;
	tstring			m_strVer;
	tstring			m_strContent;

	CControlUI*		m_pUCLoginLine;
	CListContainerElementUI*	m_pUCLoginElement;
};
