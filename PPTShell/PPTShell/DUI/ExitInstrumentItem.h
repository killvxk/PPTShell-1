#pragma once
#include "DUI/InstrumentItem.h"

class CExitInstrumentItemUI :public CInstrumentItemUI
{
public:
	CExitInstrumentItemUI();
	virtual ~CExitInstrumentItemUI(void);

private:
	virtual void		Init();


public:
	virtual	bool		HasView();
	virtual	void		CreateInstrumentView(CContainerUI* pParent);
	virtual	void		OnSelected();
	virtual	int			GetTabPageWidth();
	virtual	CInstrumentView*	GetInstrumentView();

};
