#pragma once
#include "DUI/InstrumentItem.h"

class CCleanInkInstrumentItemUI: public CInstrumentItemUI
{
public:
	CCleanInkInstrumentItemUI(void);
	virtual ~CCleanInkInstrumentItemUI(void);

private:
	virtual void		Init();


public:
	virtual	bool		HasView();
	virtual	void		CreateInstrumentView(CContainerUI* pParent);
	virtual	void		OnSelected();
	virtual	int			GetTabPageWidth();
	virtual	CInstrumentView*	GetInstrumentView();

};
