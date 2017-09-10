#pragma once


#include "ScreenPagger.h"
#include "DUI/SpotLightWindow.h"

class IPageListener;

class CScreenInstrumentUI:
	public WindowImplBase,
	public IPageListener
{
public:
	CScreenInstrumentUI(void);
	virtual ~CScreenInstrumentUI(void);

public:
	UIBEGIN_MSG_MAP
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("vr_play"),			OnBtnPlayVR);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("vr_stop"),			OnBtnStopVR);

		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("left_hover"),		OnBtnLeftHover);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("right_hover"),		OnBtnRightHover);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("left_packup"),		OnBtnLeftPackup);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("right_packup"),		OnBtnRightPackup);
		EVENT_ITEM_HANDLER_WITH_LIST_ID(DUI_MSGTYPE_ITEMCLICKED, _T("tools"),	OnToolItemChanged);
		EVENT_ID_HANDLER(DUI_MSGTYPE_ITEMSELECT,	_T("tools"),		OnToolItemSelect);
	UIEND_MSG_MAP

protected:
	//super
	virtual void		InitWindow();
	virtual CDuiString	GetSkinFolder();
	virtual CDuiString	GetSkinFile();
	virtual LPCTSTR		GetWindowClassName( void ) const;
	virtual void		OnFinalMessage( HWND hWnd );
	virtual CControlUI*	CreateControl(LPCTSTR pstrClass);
	virtual LRESULT		OnClose( UINT , WPARAM , LPARAM , BOOL& bHandled );
	virtual LRESULT		HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	//IPageListener
	virtual void		OnPageChangeBefore();
	virtual void		OnPageChanged(int nPageIndex);
	virtual void		OnPageScanneded();

	void				CreateInstrumentItems(CListUI* pList, CContainerUI* pViewParent, CListUI* pReleatedList, bool bLeft);
	CControlUI*			CreateItem();	
	void				UpdatePageUI(CLabelUI* pLabel, int nIndex);	
	
private:
	void				OnBtnPlayVR(TNotifyUI& msg);
	void				OnBtnStopVR(TNotifyUI& msg);
	bool				OnToolTabEvent(void* pObj);
	void				OnBtnPrePage(TNotifyUI& msg);
	void				OnBtnNextPage(TNotifyUI& msg);
	void				OnBtnLeftHover(TNotifyUI& msg);
	void				OnBtnRightHover(TNotifyUI& msg);
	void				OnBtnLeftPackup(TNotifyUI& msg);
	void				OnBtnRightPackup(TNotifyUI& msg);
	void				OnToolItemChanged(TNotifyUI& msg);
	void				OnToolItemSelect(TNotifyUI& msg);
	bool				OnFilterEvent(void* pEvent);
	void				TraversalControls(CControlUI * pControl);
	void SetPreNextButtonEnable(bool enable);
	bool OnScreenPreNextEnable( void* pObj );
	void				UpdateInstrumentPos();
public:
	void				ShowInstrument(HWND hParent, int nPageCount);
	void				CloseInstrument();
	CInstrumentItemUI*	GetLeftInstrumentItem(int nIndex);
	CInstrumentItemUI*	GetRightInstrumentItem(int nIndex);
	CContainerUI*		GetLeftActionButtons();
	CContainerUI*		GetRigthActionButtons();

	CSpotLightWindowUI*	GetSpotLightWindow();
	CScreenPaggerUI*	GetPagger();
	void				HandleDrawMessage(tstring lpEventName, tstring lptcsEventData, tstring& strResult);
		
	static	CScreenInstrumentUI* GetMainInstrument();
	void				StopVR(BOOL nPlay);
	void				StopTopMostTimer();//关闭置顶计时器2016.03.17 cws
	void				StartTopMostTimer();//打开置顶计时器2016.03.17 cws
	void				TopMostView();//置顶显示2016.03.17 cws
private:
	CScreenPaggerUI*	m_pScreenThumbnails;
	CContainerUI*		m_layLeftHover;
	CContainerUI*		m_layLeftTools;
	CTabLayoutUI*		m_layToolTab;
	CListUI*			m_pLeftTools;
	CLabelUI*			m_pLeftPageLabel;
	CLabelUI*			m_pRightPageLabel;

	CContainerUI*		m_layRightHover;
	CContainerUI*		m_layRightTools;
	CListUI*			m_pRightTools;

	CControlUI*			m_pTabLeftPadding;
	CControlUI*			m_pTabRightPadding;

	CContainerUI*		m_pLeftActionButtons;
	CContainerUI*		m_pRightActionButtons;

	int					m_nCurrentType;
	static CDialogBuilder	m_ItemBuilder;
	static CDialogBuilder	m_ScreenThumbnailBuilder;
	static CDialogBuilder	m_SpotLightBuilder;
	static CScreenInstrumentUI* m_pThis;
	int					m_nPageCount;

	CSpotLightWindowUI*	m_pSpotLightWindow;
	CVerticalLayoutUI*	m_pSpotLightLayout;

	//count down ui
	CContainerUI*		m_layCountDown;
	CControlUI*			m_ctrlSeconds;
	int					m_nSeconds;

};
