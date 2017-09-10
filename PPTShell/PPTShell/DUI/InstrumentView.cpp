#include "stdafx.h"
#include "InstrumentItem.h"
#include "InstrumentView.h"

CInstrumentView::CInstrumentView()
{
	m_pItem	= NULL;
}

CInstrumentView::~CInstrumentView()
{

}

void	CInstrumentView::SetHoldInstrumentItem(CInstrumentItemUI* pItem)
{
	m_pItem = pItem;
}

CInstrumentItemUI*	CInstrumentView::GetHoldInstrumentItem()
{
	return m_pItem;
}

void CInstrumentView::OnSelected()
{

}

void CInstrumentView::OnUnSelected()
{

}

void CInstrumentView::OnPageChanged( int nPageIndex )
{

}

void CInstrumentView::OnPageChangeBefore()
{

}

void CInstrumentView::OnPageScanneded()
{

}

