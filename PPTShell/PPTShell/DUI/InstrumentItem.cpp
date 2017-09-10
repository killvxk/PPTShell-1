#include "StdAfx.h"
#include "InstrumentItem.h"

CInstrumentItemUI::CInstrumentItemUI(void)
{
	m_pPageContainer	= NULL;
	m_pTab				= NULL;
	m_bLeft				= true;

}

CInstrumentItemUI::~CInstrumentItemUI(void)
{
}

void CInstrumentItemUI::Init()
{
	__super::Init();

}

void CInstrumentItemUI::SetTitle( LPCTSTR lptcsTitle )
{
	CControlUI*		pToolName	= this->FindSubControl(_T("tool_name"));
	if (pToolName && lptcsTitle)
	{
		pToolName->SetText(lptcsTitle);
	}
}

void CInstrumentItemUI::SetIcon( LPCTSTR lptcsIcon )
{
	CControlUI*		pToolImg	= this->FindSubControl(_T("tool_image"));
	if (pToolImg && lptcsIcon)
	{
		pToolImg->SetBkImage(lptcsIcon);
	}
}

void CInstrumentItemUI::SetTabContainer( CContainerUI* pPage )
{
	m_pPageContainer = pPage;
}

void CInstrumentItemUI::SetTab( CTabLayoutUI* pPage )
{
	m_pTab = pPage;
}

void CInstrumentItemUI::ShowView()
{
	if (m_pPageContainer && m_pTab)
	{
		m_pTab->SetFixedWidth(GetTabPageWidth());
		m_pTab->SelectItem(m_pPageContainer);
	}
}

void CInstrumentItemUI::SetReleatedInstruments( CListUI* pList )
{
	m_pRelatedInstruments = pList;
}


void CInstrumentItemUI::TriggerReleatedSelect()
{
	CListUI* pParent = (CListUI*)this->GetParent();
	int nIndex = pParent->GetItemIndex(this);
	if (m_pRelatedInstruments)
	{		
		int nRelatedIndex = (int)m_pRelatedInstruments->GetCurSel();
		if( nIndex == nRelatedIndex ) return;

		int iOldSel = nRelatedIndex;
		// We should first unselect the currently selected item
		if( nRelatedIndex >= 0 ) {
			CControlUI* pControl = m_pRelatedInstruments->GetItemAt(nRelatedIndex);
			if( pControl != NULL) {
				IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
				if( pListItem != NULL ) pListItem->Select(false);
			}

			m_pRelatedInstruments->m_iCurSel = -1;

		}
		if( nIndex < 0 ) return;

		CControlUI* pControl = m_pRelatedInstruments->GetItemAt(nIndex);
		if( pControl == NULL ) return;

		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
		if( pListItem == NULL ) return;
		m_pRelatedInstruments->m_iCurSel = nIndex;
		if( !pListItem->Select(true) ) {
			m_pRelatedInstruments->m_iCurSel = -1;
			return;
		}
	}
}

void CInstrumentItemUI::OnSelected()
{
	CInstrumentView* pView = GetInstrumentView();
	if (pView)
	{
		pView->SetVisible(!pView->IsVisible());
		pView->SetHoldInstrumentItem(this);
		pView->OnSelected();
	}

	TriggerReleatedSelect();
}

void CInstrumentItemUI::SetKeepSide( bool bLeft )
{
	m_bLeft = bLeft;
}

bool CInstrumentItemUI::IsLeftSide()
{
	CContainerUI* pView = GetInstrumentView();
	if (!pView)
	{
		return m_bLeft;
	}

	if (pView->IsVisible())
	{
		return m_bLeft;
	}

	return !m_bLeft;

}

void CInstrumentItemUI::OnUnSelected()
{
	CInstrumentView* pView = GetInstrumentView();
	if (pView)
	{
		pView->SetVisible(false);
		pView->OnUnSelected();
	}
}

void CInstrumentItemUI::OnAdjustView( RECT rtTools )
{
	CInstrumentView* pView = GetInstrumentView();
	if (pView)
	{
		CControlUI* pCtrl = (CControlUI*)pView->FindSubControl(_T("top_padding"));
		if (pCtrl)
		{
			RECT rt = this->GetPos();
			pCtrl->SetFixedHeight(rt.top);
		}
	}
}
