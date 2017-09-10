#pragma once
#include "DUI/InstrumentItem.h"
#include "DUI/InteractToolView.h"

class CInteractInstrumentItemUI :
	public CInstrumentItemUI,
	public IDialogBuilderCallback

{
public:
	CInteractInstrumentItemUI(void);
	virtual ~CInteractInstrumentItemUI(void);

private:
	virtual void		Init();
	virtual CControlUI* CreateControl( LPCTSTR pstrClass );


public:
	virtual	bool		HasView();
	virtual	void		CreateInstrumentView(CContainerUI* pParent);
	virtual	void		OnUnSelected();
	virtual	int			GetTabPageWidth();

	virtual CInstrumentView* GetInstrumentView();




private:
	CInteractToolViewUI*	m_pView;
	CDialogBuilder			m_Builder;

};
