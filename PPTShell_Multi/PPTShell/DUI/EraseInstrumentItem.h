#pragma once
#include "DUI/InstrumentItem.h"

class CEraseInstrumentItemUI :public CInstrumentItemUI
{
public:
	CEraseInstrumentItemUI(void);
	virtual ~CEraseInstrumentItemUI(void);

private:
	virtual void		Init();


public:
	virtual	bool		HasView();
	virtual	void		CreateInstrumentView(CContainerUI* pParent);
	virtual	void		OnSelected();
	virtual	int			GetTabPageWidth();
	virtual	CInstrumentView*	GetInstrumentView();

};
