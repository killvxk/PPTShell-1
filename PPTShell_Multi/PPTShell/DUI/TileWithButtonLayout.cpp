#include "stdafx.h"
#include "DUI/TileWithButtonLayout.h"

DuiLib::CTileWithButtonLayoutUI::CTileWithButtonLayoutUI()
{
	m_nDynamicMoreContainer = NULL;
}

DuiLib::CTileWithButtonLayoutUI::~CTileWithButtonLayoutUI()
{
	GetManager()->KillTimer(this, Refresh_ScrollBar );
}

void DuiLib::CTileWithButtonLayoutUI::SetBtnMoreCallBack( CDelegateBase& delegate )
{
	m_OnMoreButtonCallBack.clear();
	m_OnMoreButtonCallBack += delegate;
}

bool DuiLib::CTileWithButtonLayoutUI::OnBtnMoreNotify( void * pObj )
{
	TNotifyUI* pNotify = (TNotifyUI*)pObj;
	if(pNotify->sType == DUI_MSGTYPE_CLICK && m_OnMoreButtonCallBack)
	{
		m_OnMoreButtonCallBack(NULL);
	}

	return true;
}

bool DuiLib::CTileWithButtonLayoutUI::OnScrollBarEvent( void * pObj )
{

	TEventUI* pEvent = (TEventUI*)pObj;
	if(pEvent->Type ==  UIEVENT_TIMER && pEvent->wParam == Refresh_ScrollBar )
	{
		SIZE sizePos	= this->GetScrollPos();
		SIZE sizeRange	= this->GetScrollRange();
		if(sizePos.cy == sizeRange.cy && sizePos.cy != 0 && m_OnMoreButtonCallBack)
		{
			GetManager()->KillTimer(this, Refresh_ScrollBar );
 			m_OnMoreButtonCallBack(NULL);
		}
	}

	return true;
}

void DuiLib::CTileWithButtonLayoutUI::ShowMoreButton()
{
	//初始化加载更新按钮
	m_nDynamicMoreContainer = new CContainerUI;
	m_nDynamicMoreContainer->SetName(_T("DynamicMoreContainer"));
	m_nDynamicMoreContainer->SetFixedHeight(45);
	m_nDynamicMoreContainer->SetFixedWidth(385);

	m_nDynamicMoreLayout = new CVerticalLayoutUI;
	m_nDynamicMoreLayout->SetInset(CDuiRect(6,10,9,10));
	m_pDynamicMoreBtn		= new CButtonUI;
	m_pDynamicMoreBtn->SetTextColor(0xFFFFFFFF);
	m_pDynamicMoreBtn->SetFont(140100);
	m_pDynamicMoreBtn->SetText(_T("加载更多"));
	m_pDynamicMoreBtn->SetBkColor(0xFF272727);
	m_pDynamicMoreBtn->SetHotBkColor(0xFF577071);
	m_pDynamicMoreBtn->SetPushedBkColor(0xFF10B0B6);
	m_pDynamicMoreBtn->OnNotify += MakeDelegate(this, &CTileWithButtonLayoutUI::OnBtnMoreNotify);
	m_nDynamicMoreLayout->Add(m_pDynamicMoreBtn);

	m_nDynamicMoreLoadLayout = new CVerticalLayoutUI;
	m_nDynamicMoreLoadLayout->SetInset(CDuiRect(6,10,9,10));
	CVerticalLayoutUI* pLayout = new CVerticalLayoutUI;
	pLayout->SetBkColor(0xFF272727);
	m_nDynamicMoreLoadGif = new CGifAnimUI;
	m_nDynamicMoreLoadGif->SetBkImage(_T("Guide\\loading.gif"));
	m_nDynamicMoreLoadGif->SetFixedWidth(24);
	m_nDynamicMoreLoadGif->SetFixedHeight(24);
	m_nDynamicMoreLoadGif->SetPadding(CDuiRect(174,1,0,0));
	pLayout->Add(m_nDynamicMoreLoadGif);
	m_nDynamicMoreLoadLayout->Add(pLayout);
	m_nDynamicMoreLoadLayout->SetVisible(false);

	m_nDynamicMoreContainer->Add(m_nDynamicMoreLayout);
	m_nDynamicMoreContainer->Add(m_nDynamicMoreLoadLayout);
// 	this->OnEvent += MakeDelegate(this, &CTileWithButtonLayoutUI::OnScrollBarEvent);
// 	GetManager()->KillTimer(this, Refresh_ScrollBar);
// 	GetManager()->SetTimer(this, Refresh_ScrollBar, 200);
	this->Add(m_nDynamicMoreContainer);
}

void DuiLib::CTileWithButtonLayoutUI::HideMoreButton()
{
	if(m_nDynamicMoreContainer)
	{
		this->Remove(m_nDynamicMoreContainer);
		m_nDynamicMoreContainer = NULL;
	}
}

void DuiLib::CTileWithButtonLayoutUI::SetPos( RECT rc, bool bNeedInvalidate /*= true*/ )
{
	CControlUI::SetPos(rc, bNeedInvalidate);
	rc = m_rcItem;

	// Adjust for inset
	rc.left += m_rcInset.left;
	rc.top += m_rcInset.top;
	rc.right -= m_rcInset.right;
	rc.bottom -= m_rcInset.bottom;

	if( m_items.GetSize() == 0) {
		ProcessScrollBar(rc, 0, 0);
		return;
	}

	if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

	// Position the elements
	if( m_szItem.cx > 0 ) m_nColumns = (rc.right - rc.left) / m_szItem.cx;
	if( m_nColumns == 0 ) m_nColumns = 1;

	int cyNeeded = 0;
	int cxWidth = (rc.right - rc.left) / m_nColumns;
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
		cxWidth = (rc.right - rc.left + m_pHorizontalScrollBar->GetScrollRange() ) / m_nColumns; ;

	int cyHeight = 0;
	int iCount = 0;
	POINT ptTile = { rc.left, rc.top };
	if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
		ptTile.y -= m_pVerticalScrollBar->GetScrollPos();
	}
	int iPosX = rc.left;
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
		iPosX -= m_pHorizontalScrollBar->GetScrollPos();
		ptTile.x = iPosX;
	}
	for( int it1 = 0; it1 < m_items.GetSize(); it1++ ) {
		CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
		if( !pControl->IsVisible() ) continue;
		if( pControl->IsFloat() ) {
			SetFloatPos(it1);
			continue;
		}

		// Determine size
		RECT rcTile ;
		if(pControl == m_nDynamicMoreContainer)
		{
			if( (it1 % m_nColumns) == 0 )
			{
				rcTile = CDuiRect(ptTile.x, ptTile.y, ptTile.x + pControl->GetFixedWidth(), ptTile.y);
			}
			else
			{
				if(it1 - 1 >= 0)
				{
					CControlUI* pControl1 = static_cast<CControlUI*>(m_items[it1 - 1]);

					cyHeight = pControl->GetFixedHeight();
					ptTile.x = 0;
					ptTile.y += pControl1->GetFixedHeight() + pControl->GetFixedHeight();
					rcTile = CDuiRect(ptTile.x, ptTile.y, ptTile.x + pControl->GetFixedWidth(), ptTile.y);
				}
				else
				{
					rcTile = CDuiRect(ptTile.x, ptTile.y, ptTile.x + pControl->GetFixedWidth(), ptTile.y);
				}
				
			}
		}
		else
		{
			rcTile = CDuiRect(ptTile.x, ptTile.y, ptTile.x + cxWidth, ptTile.y);
		}

		if( (iCount % m_nColumns) == 0 )
		{
			int iIndex = iCount;
			for( int it2 = it1; it2 < m_items.GetSize(); it2++ ) {
				CControlUI* pLineControl = static_cast<CControlUI*>(m_items[it2]);
				if( !pLineControl->IsVisible() ) continue;
				if( pLineControl->IsFloat() ) continue;

				RECT rcPadding = pLineControl->GetPadding();
				SIZE szAvailable = { rcTile.right - rcTile.left - rcPadding.left - rcPadding.right, 9999 };
				if( iIndex == iCount || (iIndex + 1) % m_nColumns == 0 ) {
					szAvailable.cx -= m_iChildPadding / 2;
				}
				else {
					szAvailable.cx -= m_iChildPadding;
				}

				if( szAvailable.cx < pControl->GetMinWidth() ) szAvailable.cx = pControl->GetMinWidth();
				if( szAvailable.cx > pControl->GetMaxWidth() ) szAvailable.cx = pControl->GetMaxWidth();

				SIZE szTile = pLineControl->EstimateSize(szAvailable);
				if( szTile.cx < pControl->GetMinWidth() ) szTile.cx = pControl->GetMinWidth();
				if( szTile.cx > pControl->GetMaxWidth() ) szTile.cx = pControl->GetMaxWidth();
				if( szTile.cy < pControl->GetMinHeight() ) szTile.cy = pControl->GetMinHeight();
				if( szTile.cy > pControl->GetMaxHeight() ) szTile.cy = pControl->GetMaxHeight();

				cyHeight = max(cyHeight, szTile.cy + rcPadding.top + rcPadding.bottom);
				if( (++iIndex % m_nColumns) == 0) break;
			}
		}

		RECT rcPadding = pControl->GetPadding();

		rcTile.left += rcPadding.left + m_iChildPadding / 2;
		rcTile.right -= rcPadding.right + m_iChildPadding / 2;
		if( (iCount % m_nColumns) == 0 ) {
			rcTile.left -= m_iChildPadding / 2;
		}

		if( ( (iCount + 1) % m_nColumns) == 0 ) {
			rcTile.right += m_iChildPadding / 2;
		}

		// Set position
		rcTile.top = ptTile.y + rcPadding.top;
		rcTile.bottom = ptTile.y + cyHeight;

		SIZE szAvailable = { rcTile.right - rcTile.left, rcTile.bottom - rcTile.top };
		SIZE szTile = pControl->EstimateSize(szAvailable);
		if( szTile.cx == 0 ) szTile.cx = szAvailable.cx;
		if( szTile.cy == 0 ) szTile.cy = szAvailable.cy;
		if( szTile.cx < pControl->GetMinWidth() ) szTile.cx = pControl->GetMinWidth();
		if( szTile.cx > pControl->GetMaxWidth() ) szTile.cx = pControl->GetMaxWidth();
		if( szTile.cy < pControl->GetMinHeight() ) szTile.cy = pControl->GetMinHeight();
		if( szTile.cy > pControl->GetMaxHeight() ) szTile.cy = pControl->GetMaxHeight();
		RECT rcPos = {(rcTile.left + rcTile.right - szTile.cx) / 2, (rcTile.top + rcTile.bottom - szTile.cy) / 2,
			(rcTile.left + rcTile.right - szTile.cx) / 2 + szTile.cx, (rcTile.top + rcTile.bottom - szTile.cy) / 2 + szTile.cy};
		pControl->SetPos(rcPos, false);

		if( (++iCount % m_nColumns) == 0 ) {
			ptTile.x = iPosX;
			ptTile.y += cyHeight + m_iChildPadding;
			cyHeight = 0;
		}
		else {
			ptTile.x += cxWidth;
		}
		cyNeeded = rcTile.bottom - rc.top;
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) cyNeeded += m_pVerticalScrollBar->GetScrollPos();
	}

	// Process the scrollbar
	ProcessScrollBar(rc, 0, cyNeeded);
}

void DuiLib::CTileWithButtonLayoutUI::StartMoreBtnLoading()
{
	m_nDynamicMoreLoadGif->PlayGif();
	m_nDynamicMoreLoadLayout->SetVisible(true);
}

void DuiLib::CTileWithButtonLayoutUI::StopMoreBtnLoading()
{
	m_nDynamicMoreLoadGif->StopGif();
	m_nDynamicMoreLoadLayout->SetVisible(false);
}

