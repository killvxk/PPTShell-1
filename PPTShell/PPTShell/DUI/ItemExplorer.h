#pragma once
#include "ResourceExplorer.h"
#include "ResourceItem.h"


enum ItemExplorerType
{
	eItemExplorer_Cloud,
	eItemExplorer_Local,
};
class CItemExplorerUI: 
	public CResourceExplorerUI
{

public:
	CItemExplorerUI();
	~CItemExplorerUI();
	UIBEGIN_MSG_MAP
		SUPER_HANDLER_MSG
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("close"),			OnBtnClose);
	UIEND_MSG_MAP

public:
	static CItemExplorerUI* m_pInstance;
	static CItemExplorerUI* GetInstance();

	LPCTSTR GetWindowClassName() const;
	UINT	GetClassStyle() const;


	virtual void		InitWindow();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	
	CResourceItemUI*	GetParentItem()		{	return m_pParentItem;	}
	void SetParentItem(CResourceItemUI* pParentItem);

	//notify
public:
	void				OnBtnClose(TNotifyUI& msg);

public:
	void				ResetUI();
	CResourceItemUI*	CreateItem();
	virtual void		ShowResource( int nType, CStream* pStream, void* pParams );

private:
	int					m_nItemExplorerType;
	CResourceItemUI*	m_pParentItem;


};

