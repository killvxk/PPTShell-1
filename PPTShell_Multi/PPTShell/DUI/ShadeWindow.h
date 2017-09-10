#pragma once
#include "DUICommon.h"
#include "GifAnimUI.h"
#include "DUI/WndShadow.h"

class CShadeWindow : public WindowImplBase
{
public:
	CShadeWindow(void);
	~CShadeWindow(void);

public:
	static  CShadeWindow* GetInstance(HWND hParent);
	static  void Show(tstring strTip, bool bLoading = true);
	static  void Show(HWND hParent, tstring strTip, bool bLoading = true);
	static  void Hide();


protected:
	virtual CDuiString	GetSkinFolder();
	virtual CDuiString	GetSkinFile();
	virtual LPCTSTR		GetWindowClassName(void) const;
	virtual void		OnFinalMessage( HWND hWnd );

	virtual void		InitWindow();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);

	void				ShadeShow(tstring strTip, bool bLoading);
	void				ShadeHide();
	void				CalcText( HDC hdc, RECT& rc, LPCTSTR lpszText, int nFontId, UINT format, UITYPE_FONT nFontType, int c = -1);

	static CShadeWindow*m_pInstance;
	HWND				m_hParent;

	CLabelUI*			m_pLabel;
	CGifAnimUI*			m_pAnimation;
//	CWndShadow*			m_pWndShadow;

};

void ShadeWindowShow(tstring strTip, bool bLoading = true);
void ShadeWindowShow(HWND hParent, tstring strTip, bool bLoading = true);
void ShadeWindowHide();
