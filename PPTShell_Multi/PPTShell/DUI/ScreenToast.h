#pragma once

#include "DUICommon.h"
#include "ScreenToast.h" 
 

class CScreenToast : public WindowImplBase
{
private:
	CScreenToast(void);
	~CScreenToast(void);

public:
	static  CScreenToast* GetInstance(HWND hParent);
	static void		Toast(HWND hParent, tstring strMsg, bool bFront = false, int nTime = 2000);


	virtual void	InitWindow();

	bool			Show(tstring strMsg, bool bFront = false, int nTime = 2000);

private:
	virtual CDuiString	GetSkinFolder();
	virtual CDuiString	GetSkinFile();
	virtual LPCTSTR		GetWindowClassName(void) const;

	virtual void		OnFinalMessage( HWND hWnd );
	virtual LRESULT		ResponseDefaultKeyEvent(WPARAM wParam);

	void  ShowToast();

	void  Reset();

	void  SetToastMsg(tstring strMsg);
	void  CalcText( HDC hdc, RECT& rc, LPCTSTR lpszText, int nFontId, UINT format, UITYPE_FONT nFontType, int c = -1);

	static void OnTimer(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime);


protected:
	static CScreenToast*		m_pInstance;
	HWND						m_hParent;

	list<TOAST_INFO>			m_lstMsg;
	DWORD						m_dwStartTime;
	DWORD						m_dwEndTime;

	int							m_nLastStatus;
	int							m_nCurStatus;
	int							m_nOpacity;
	CTextUI*					m_lblContent;
	CHorizontalLayoutUI*		m_layoutToast;

	CRITICAL_SECTION			m_Lock;

	BOOL						m_bShow;

	CRect						m_MainRect;
};
