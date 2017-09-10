#pragma once
#include "DUI/InstrumentItem.h"
#include "PencelView.h"

class CPencelInstrumentItemUI :
	public CInstrumentItemUI,
	public IDialogBuilderCallback
{
public:
	CPencelInstrumentItemUI(void);
	virtual ~CPencelInstrumentItemUI(void);

private:
	virtual void		Init();
	virtual CControlUI* CreateControl( LPCTSTR pstrClass );


public:
	virtual	bool		HasView();
	virtual	void		CreateInstrumentView(CContainerUI* pParent);
	virtual	int			GetTabPageWidth();
	virtual void		OnUnSelected();
	virtual	void		OnSelected();
	virtual void		OnAdjustView(RECT rtTools);

	virtual CInstrumentView* GetInstrumentView();

	



	
private:
	CPencelViewUI*		m_pView;
	CDialogBuilder		m_Builder;
	bool				m_bShow;

};
