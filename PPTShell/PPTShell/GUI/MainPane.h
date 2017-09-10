#pragma once

#include "PanelInnerDlg.h"

class CMainPane : public CDockablePane
{
	DECLARE_DYNAMIC(CMainPane)

public:
	CMainPane();
	virtual ~CMainPane();

	void		AdjustPanel();

	bool		AddDialog(CPanelInnerDlg* pDialog);
	CDialog*	GetDialogAt(int nIndex);
	int			GetDialogCount();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()

protected:
	map<int, CPanelInnerDlg*> m_mapInnerDlgs;

public:
	CDialog*	GetMainControlDlg();
};


