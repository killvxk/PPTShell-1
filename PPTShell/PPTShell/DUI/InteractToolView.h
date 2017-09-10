#pragma once
#include "InstrumentView.h"
class CInteractToolViewUI: 
	public CInstrumentView,
	public IDialogBuilderCallback,
	public INotifyUI
{
public:
	CInteractToolViewUI(void);
	virtual ~CInteractToolViewUI(void);

	UIBEGIN_MSG_MAP
		EVENT_ID_HANDLER(DUI_MSGTYPE_SETFOCUS, _T("interact_item"),	OnInteractItemClick);
	UIEND_MSG_MAP
public:
	virtual void		Init();
	virtual CControlUI* CreateControl( LPCTSTR pstrClass );


protected:
	void			CreateInteractionsWithConfig();
	CControlUI*		CreateItemUI(int nToolType);
	void			CreateListFromValue(LPCTSTR lptcsGroup, LPVOID lpGroup);

	void			OnInteractItemClick( TNotifyUI& msg );

public:
	virtual	void	OnSelected();
	virtual	void	OnUnSelected();
	CControlUI*		GetItem(int nIndex);
	CControlUI*		GetItem(LPCTSTR szName);

	

private:
	CTileLayoutUI*				m_pList;
	static CDialogBuilder		m_ItemBuilder;
	static CDialogBuilder		m_GroupBuilder; 
};
