#pragma once
#include "PanelView.h"
#include "Util\Singleton.h"

#define WM_SHOWMOBILEPEN                      0x0211
#define WM_HIDEMOBILEPEN                      0x0212

class CMobilePenViewUI:public WindowImplBase
{
	UIBEGIN_MSG_MAP 
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("btnClose"),OnBtnCloseClick);
	UIEND_MSG_MAP

public:
	void ShowMobilePenBoard(); 
	void CloseMobilePenBoard();  
	void ClearPanel(); 
	void	AddShapes(IShape* pShape); 

	CMobilePenViewUI(void);
	~CMobilePenViewUI(void);  
private: 
	void OnBtnCloseClick(TNotifyUI& msg);
	 
	DWORD m_dwCurrentColor;
	int m_nCurrentPenViewType;
	int m_nScreenWidth;
	int m_nScreenHeight;  
	CVerticalLayoutUI* m_pLayoutBG; 
	CPanelViewUI*		m_pMobilePen; 

protected:
	virtual CDuiString	GetSkinFolder();
	virtual CDuiString	GetSkinFile();
	virtual LPCTSTR		GetWindowClassName( void ) const; 

	virtual void InitWindow(); 
	CControlUI* CreateControl( LPCTSTR pstrClass );
};


typedef Singleton<CMobilePenViewUI>		MobilePenViewUI;