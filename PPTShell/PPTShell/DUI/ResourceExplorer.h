#pragma once
#include "DUICommon.h"
#include "GifAnimUI.h"
#include "Util/Stream.h"
#include "DUI/CSliderTabLayoutUI.h"

class CResourceExplorerUI: 
	public WindowImplBase
{

public:
	CResourceExplorerUI();
	~CResourceExplorerUI();
	UIBEGIN_MSG_MAP
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("close"),			OnBtnClose);
	UIEND_MSG_MAP

public:
	LPCTSTR				GetWindowClassName() const;
	UINT				GetClassStyle() const;
	virtual void		InitWindow();
	virtual CDuiString	GetSkinFile();
	virtual CDuiString	GetSkinFolder();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	bool				OnListEvent(void* pObj);
	void				OnBtnClose( TNotifyUI& msg );


	//notify
public:
	virtual void		ShowNetlessUI(bool bShow);

	void				SetFixedWidth(int nWidth);
	void				SetBkColor(DWORD dwColor);
	void				SetCountText(int nCount);
	void				SetCountText(LPCTSTR lptcsCount);
	void				SetTitleText(LPCTSTR lptcsName);

	void				StartMask();
	void				StopMask();

	tstring				GetTitleText()	{ return m_lbName->GetText().GetData();	};

	CButtonUI*			GetRefreshBtn();

	virtual void		ShowResource(int nType, CStream* pStream, void* pParams) = 0;

	virtual LRESULT		HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual void		ShowWindow( bool bShow = true, bool bTakeFocus = true );

protected:
	CSliderTabLayoutUI*	m_pSliderTabLayout;

	CVerticalLayoutUI*	m_pList;
	CDialogBuilder		m_builder;
	CLabelUI*			m_lbCount;
	CLabelUI*			m_lbName;
	CGifAnimUI*			m_pLoading;
	CContainerUI*		m_pBody;
	CVerticalLayoutUI*	m_pMask;
	CVerticalLayoutUI*	m_pBackGround;
	CVerticalLayoutUI*	m_layNetless;
	CButtonUI*			m_btnRefresh;
	bool				m_bLastShowNetless;
	bool				m_bLastShowMask;

};

