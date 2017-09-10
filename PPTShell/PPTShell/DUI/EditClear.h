#pragma once
#include "DUICommon.h"


#define CLEAR_BUTTON_NAME _T("ClearButton")
#define SEARCH_BUTTON_NAME _T("searchBtn")
#define CLEAR_RICHEDIT_NAME _T("searchEdit")
#define SEARCH_TIME_NAME _T("searchListItem")


class CEditClearUI : public CEditUI
{
public:
	CEditClearUI();
	~CEditClearUI();

	virtual LPCTSTR GetClass() const;
	virtual LPVOID	GetInterface(LPCTSTR pstrName);

	virtual void	DoInit();

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	void SetText(LPCTSTR pstrText);
	void SetClearBtn();
	void HideClearBtn();
	
	void DoEvent(TEventUI& event);

	void PaintText(HDC hDC);

	tstring GetTipText();
private:
	bool	m_bClear;
	bool	m_bClearStatus;
	tstring	m_clearImagePath;
	//tstring m_tiptext;

	CButtonUI*	m_pClearButton;

	//DWORD	m_dwTipColor;
	//DWORD	m_dwNormalTextColor;

	int		m_nType;
};
