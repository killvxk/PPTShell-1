#pragma once

class CInstrumentItemUI;


class ISelectListener
{
public:
	virtual	void		OnUnSelected()	= 0;
	virtual	void		OnSelected()	= 0;

};

class IPageListener
{
public:
	virtual	void		OnPageChangeBefore()	= 0;
	virtual	void		OnPageChanged()			= 0;

};


class CInstrumentView:
	public CVerticalLayoutUI,
	public ISelectListener,
	public IPageListener
{
public:
	CInstrumentView()
	{
		m_pItem	= NULL;
	}

public:
	void	SetHoldInstrumentItem(CInstrumentItemUI* pItem)
	{
		m_pItem = pItem;
	};

	CInstrumentItemUI*	GetHoldInstrumentItem()
	{
		return m_pItem;
	}

	// be called when HoldInstrumentItem select or unselect
	virtual	void		OnUnSelected(){};

	virtual	void		OnSelected(){};

	//be called when page changed
	virtual	void		OnPageChanged(){};

	virtual	void		OnPageChangeBefore(){};

private:
	CInstrumentItemUI* m_pItem;


};