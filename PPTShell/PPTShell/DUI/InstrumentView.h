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
	virtual	void		OnPageChangeBefore()				= 0;
	virtual void		OnPageChanged(int nPageIndex)		= 0;
	virtual void		OnPageScanneded()					= 0;

};


class CInstrumentView:
	public CVerticalLayoutUI,
	public ISelectListener,
	public IPageListener
{
public:
	CInstrumentView();
	~CInstrumentView();

public:
	void				SetHoldInstrumentItem(CInstrumentItemUI* pItem);
	CInstrumentItemUI*	GetHoldInstrumentItem();
	
	// be called when HoldInstrumentItem select or unselect
	virtual	void		OnUnSelected();
	virtual	void		OnSelected();

	//be called when page changed
	virtual	void		OnPageChanged(int nPageIndex);
	virtual	void		OnPageChangeBefore();
	virtual void		OnPageScanneded();

private:
	CInstrumentItemUI* m_pItem;


};