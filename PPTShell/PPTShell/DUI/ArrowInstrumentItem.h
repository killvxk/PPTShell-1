#pragma once
#include "DUI/InstrumentItem.h"

class CArrowInstrumentItemUI :public CInstrumentItemUI
{
public:
	CArrowInstrumentItemUI(void);
	virtual ~CArrowInstrumentItemUI(void);

private:
	virtual void		Init();


public:
	virtual	bool		HasView();
	virtual	void		CreateInstrumentView(CContainerUI* pParent);
	virtual	void		OnSelected();
	virtual	int			GetTabPageWidth();
	virtual	CInstrumentView*	GetInstrumentView();

};
