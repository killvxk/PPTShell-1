#pragma once
//
// ListItem.h
// 
#include "ListBarButton.h"

class CListBarExpandButton : public CListBarButton

{
public:
	CListBarExpandButton();
	~CListBarExpandButton();

	void				Expand(bool bExpand = true);
	bool				GetExpandStaus();

	virtual void		PaintStatusImage(HDC hDC);

	virtual void		SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	//override control
	virtual	void		DoInit();

	void				AddToVecControls(CControlUI* pControl);


protected:
	vector<CControlUI *>		m_vecControls;			//保存可扩展按钮下的全部控件，方便统一管理
protected:
	

	CDuiString	m_strNormalIcon;
	CDuiString	m_strHotIcon;
	CDuiString	m_strExpandIcon;
	CDuiString	m_strTipIcon;
	int			m_nLogoWidth;
	int			m_nLogoHeight;
	CRect		m_rcLogoPadding;
	CRect		m_rcTipPos;

	bool		m_bExpand;
	bool		m_bClickDown;

	DWORD		m_dwNormalColor;
	DWORD		m_dwHoverColor;
	DWORD		m_dwExpandColor;
};