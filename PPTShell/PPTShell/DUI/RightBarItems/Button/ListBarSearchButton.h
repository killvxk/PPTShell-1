#pragma once
//
// ListItem.h
// 
#include "../ListBarExpandButton.h"

class CListBarSearchButton : public CListBarExpandButton
{
public:
	CListBarSearchButton();
	~CListBarSearchButton();

protected:
	//override control
	virtual	void			DoInit();

	virtual void			PaintStatusImage(HDC hDC);
	virtual void			SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	
	

	virtual void			DoClick(TNotifyUI* pNotify);
private:
	bool					m_bSelected;// «∑Ò—°÷–

	DWORD					m_dwSelectedTextColor;
	CDuiString				m_strSelectedIcon;

public:	
	bool					GetSelected();
	void					Selected(bool bSelected);

	bool					OnBtnCloseResponse(void * pObj);
};