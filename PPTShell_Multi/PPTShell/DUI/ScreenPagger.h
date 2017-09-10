#pragma once

#include "DUI/InstrumentView.h"
#include "PageActiveXScanner.h"

class CScreenPaggerUI: 
	public CContainerUI,
	public INotifyUI,
	public IDialogBuilderCallback,
	public IPageListener
{
public:
	CScreenPaggerUI(void);
	virtual ~CScreenPaggerUI(void);

	UIBEGIN_MSG_MAP
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK, _T("thumbnail_pickup"),	OnBtnPickup);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK, _T("thumbnail_expand"),	OnBtnExpand);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK, _T("pre_page"),			OnBtnPrePage);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK, _T("next_page"),		OnBtnNextPage);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK, _T("pre_action"),		OnBtnPreAction);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK, _T("next_action"),		OnBtnNextAction);
	UIEND_MSG_MAP
protected:
	virtual void		Init();
	virtual CControlUI* CreateControl( LPCTSTR pstrClass );

public:
	void				CreateThumbnailList();
	void				AddPageListener(IPageListener* pPageListener);
	void				RemovePageListener(IPageListener* pPageListener);

	void				SetPageCount(int nCount);
	int					GetPageCount();
	void				SetCurrentPage(int nCurrentPage);
	void				SetPageActiveX(CStream* pStream);
	void				SetInsideMode(bool bInsideMode);

	CPageActiveXScanner*	GetScanner();

private:
	void				OnBtnExpand(TNotifyUI& msg);
	void				OnBtnPickup(TNotifyUI& msg);
	void				OnBtnPrePage(TNotifyUI& msg);
	void				OnBtnNextPage(TNotifyUI& msg);
	void				OnBtnPreAction(TNotifyUI& msg);
	void				OnBtnNextAction(TNotifyUI& msg);

	bool				OnListEvent(void* pObj);
	bool				OnItemClick(void* pObj);

	void				UpdateItemListScrollPos();
	void				SwitchPage(int nPage);


	virtual void		OnPageChangeBefore();
	virtual void		OnPageChanged( int nPageIndex );
	virtual void		OnPageScanneded();
	void				ShowSelectIndex();



private:
	CContainerUI*			m_pItemList;
	int						m_nTotalLeafCount;
	int						m_nCurrentLeaf;
	int						m_nTotalPageCount;
	int						m_nCurrentPage;
	int                     m_nCurrentPPTPage;    //PPT当前页包括了最后一页 
	static	CDialogBuilder	m_ItemBuilder;
	CPageActiveXScanner		m_PageScaner;
	bool					m_bInsideMode;
	DWORD					m_dwLastClickTickCount;
	vector<IPageListener*>	m_vctrPageListener;


};
