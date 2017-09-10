#pragma once
#include "stdafx.h"
#include "DUICommon.h" 
#include <afxwin.h>

class CMagicPen_View : public WindowImplBase/*, public IListCallbackUI*/
{
public:
	CMagicPen_View(void);
	~CMagicPen_View(void);

	UIBEGIN_MSG_MAP  
			EVENT_BUTTON_CLICK(_T("btnClose"),OnCloseBtn);
	UIEND_MSG_MAP
 

public:
	LPCTSTR GetWindowClassName() const;	
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder(); 
	void	OnCloseBtn(TNotifyUI& msg);
	virtual LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);  
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void		InitWindow();
	void InitPenZoom(int nL, int nT, int nW, int nH, CBitmap* pBmp , HWND nHandle);    
private: 
	CBitmap*	m_pbmpCopyScreen;
	int m_nLeft;
	int m_nTop;
	int m_nWidth;
	int m_nHeight;
	int m_nCx;
	int m_nCy;
	CButtonUI*		m_pCloseBtn;//ÍË³ö
	CLabelUI*		m_pCopyBmplbl;//½ØÍ¼
	HWND			m_hParentHandle;//¸¸´°¿Ú¾ä±ú 
};

 
