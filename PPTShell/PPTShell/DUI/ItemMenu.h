#pragma once

enum	ItemMenuIndex
{
	eMenu_Unkown = 0,
	eMenu_Rename,
	eMenu_Delete,
	eMenu_Downto_Local
};


class IHolderVisitor;
class CItemMenuUI :
	public WindowImplBase,
	public IHolderVisitor
{
public:
	CItemMenuUI(void);
	~CItemMenuUI(void);

	UIBEGIN_MSG_MAP
		EVENT_ID_HANDLER(DUI_MSGTYPE_ITEMSELECT, _T("menu_list"),	OnMenuItemChanged);
	UIEND_MSG_MAP

public:
	CControlUI*			GetHolder();
	void				SetHolder( CControlUI* pHolder );
	void				CreateMenu();
	//
	void				AddMenuItem(LPCTSTR lptcsMenu, int nMenuId);
	void				AddMenuSplitter();
	
	void				SetLeftWeight(float fWeight);
	void				ShowMenu();

	


protected:
	virtual void		InitWindow();
	LPCTSTR				GetWindowClassName() const;	
	virtual CDuiString	GetSkinFile();
	virtual CDuiString	GetSkinFolder();
	virtual void		OnFinalMessage(HWND hWnd);
	virtual LRESULT		OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	void				OnMenuItemChanged(TNotifyUI& msg);

private:
	POINT				m_pt;
	float				m_fLeftWeight;
	int					m_nHeight;
	CControlUI*			m_pHolder;
	CListUI*			m_pMenuList;
	CDialogBuilder		m_MenuSplitterBuilder;
	CDialogBuilder		m_MenuItemBuilder;

};
