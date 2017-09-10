#pragma once
#include "DUI/InstrumentItem.h"
#include "DUI/SubjectToolView.h"

class CSubjectInstrumentItemUI :
	public CInstrumentItemUI,
	public IDialogBuilderCallback
{
public:
	CSubjectInstrumentItemUI(void);
	virtual ~CSubjectInstrumentItemUI(void);

private:
	virtual void		Init();
	virtual CControlUI* CreateControl( LPCTSTR pstrClass );


public:
	virtual	bool				HasView();
	virtual	void				CreateInstrumentView(CContainerUI* pParent);
	virtual	int					GetTabPageWidth();
	virtual CInstrumentView*	GetInstrumentView();
	virtual void				OnAdjustView(RECT rtTool);


private:
	CSubjectToolViewUI*		m_pView;
	CDialogBuilder			m_Builder;
};
