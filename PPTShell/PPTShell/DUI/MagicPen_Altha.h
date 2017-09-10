#pragma once
#include "stdafx.h"
#include "DUICommon.h" 

class CMagicPen_Altha : public WindowImplBase/*, public IListCallbackUI*/
{
public:
	CMagicPen_Altha(void);
	~CMagicPen_Altha(void);

	UIBEGIN_MSG_MAP  
	UIEND_MSG_MAP
 

public:
	LPCTSTR GetWindowClassName() const;	
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder(); 
	virtual LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void InitPenZoom( int nL, int nT, int nW, int nH,CBitmap* pBmp );
protected:  
	HWND			m_hWndParent;   
	CBitmap*		m_pbmpTmp;
	HWND			m_hPenZoom;	
	int				m_nCx;
	int				m_nCy;

	int				m_nLeft;
	int				m_nTop;
	int				m_nWidth;
	int				m_nHeight;
};

 
