#pragma once
#include "DUI/InstrumentView.h"

class CInstrumentItemUI :
	public CListContainerElementUI,
	public ISelectListener
{
public:
	CInstrumentItemUI(void);
	virtual ~CInstrumentItemUI(void);

protected:
	virtual void		Init();
	void				TriggerReleatedSelect();


public:
	void		ShowView();

	void		SetTitle(LPCTSTR lptcsTitle);
	void		SetIcon(LPCTSTR lptcsIcon);
	void		SetTabContainer(CContainerUI* pPage);
	void		SetTab(CTabLayoutUI* pPage);
	void		SetReleatedInstruments(CListUI*	pList);
	void		SetKeepSide(bool bLeft);
	bool		IsLeftSide();

	
	virtual	void				OnSelected();
	virtual	void				OnUnSelected();

	virtual	int					GetTabPageWidth()							= 0;
	virtual	bool				HasView()									= 0;
	virtual	void				CreateInstrumentView(CContainerUI* pParent)	= 0;
	virtual	CInstrumentView*	GetInstrumentView()							= 0;
	virtual void				OnAdjustView(RECT rtTools);

	

	


protected:
	CContainerUI*		m_pPageContainer;
	CTabLayoutUI*		m_pTab;
	CListUI*			m_pRelatedInstruments;
	bool				m_bLeft;


};
