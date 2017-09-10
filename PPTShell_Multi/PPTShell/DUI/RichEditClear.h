#pragma once
#include "DUICommon.h"


#define CLEAR_BUTTON_NAME _T("ClearButton")
#define SEARCH_BUTTON_NAME _T("searchBtn")
#define CLEAR_RICHEDIT_NAME _T("searchEdit")
#define SEARCH_TIME_NAME _T("searchListItem")


class CRichEditClearUI : public CRichEditUI
{
public:
	CRichEditClearUI();
	~CRichEditClearUI();

	virtual LPCTSTR GetClass() const;
	virtual LPVOID	GetInterface(LPCTSTR pstrName);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	virtual void OnTxNotify(DWORD iNotify, void *pv);
	
private:
	bool	m_bClear;
	tstring	m_clearImagePath;

	CButtonUI*	m_pClearButton;
};
