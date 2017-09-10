#pragma once
#include "DUI/InstrumentItem.h"
#include "DUI/EncourageView.h"

class CEncourageInstrumentItemUI :
	public CInstrumentItemUI, 
	public IDialogBuilderCallback
{
public:
	CEncourageInstrumentItemUI(void);
	virtual ~CEncourageInstrumentItemUI(void);

private:
	virtual void		Init();
	virtual CControlUI* CreateControl( LPCTSTR pstrClass );


public:
	virtual	bool		HasView();
	virtual	void		CreateInstrumentView(CContainerUI* pParent);
	virtual	int			GetTabPageWidth();
	virtual	CInstrumentView*	GetInstrumentView();

	

private:
	CEncourageViewUI*	m_pView;
	CDialogBuilder		m_Builder;

};
