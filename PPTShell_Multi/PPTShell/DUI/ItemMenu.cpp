#include "StdAfx.h"
#include "DUI/IVisitor.h"
#include "ItemMenu.h"


CItemMenuUI::CItemMenuUI(void)
{
	m_pHolder		= NULL;
	m_pMenuList		= NULL;
	m_nHeight		= 0;
	m_fLeftWeight	= 0.5;
}

CItemMenuUI::~CItemMenuUI(void)
{
}

LPCTSTR CItemMenuUI::GetWindowClassName() const
{
	return _T("ItemMenu");
}

DuiLib::CDuiString CItemMenuUI::GetSkinFile()
{
	return _T("Item\\ItemMenuWindow.xml");
}

DuiLib::CDuiString CItemMenuUI::GetSkinFolder()
{
	return _T("skins");
}

void CItemMenuUI::InitWindow()
{
	m_pMenuList = (CListUI*)m_PaintManager.FindControl(_T("menu_list"));
}


void CItemMenuUI::OnFinalMessage( HWND hWnd )
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

LRESULT CItemMenuUI::OnKillFocus( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	::DestroyWindow(GetHWND());
	return FALSE;
}

void CItemMenuUI::OnMenuItemChanged( TNotifyUI& msg )
{
	CListUI*		pList = (CListUI*)msg.pSender;
	CContainerUI*	pItem = (CContainerUI*)pList->GetItemAt(msg.wParam);
	if (pItem && GetHolder())
	{
		GetHolder()->GetManager()->SendNotify(GetHolder(), _T("menuclick"), (WPARAM)pItem->GetTag(), 0, true);
	}
	Close();
}

CControlUI* CItemMenuUI::GetHolder()
{
	return m_pHolder;
}

void CItemMenuUI::SetHolder( CControlUI* pHolder )
{
	m_pHolder = pHolder;
}

void CItemMenuUI::AddMenuItem( LPCTSTR lptcsMenu, int nMenuId )
{
	if (!m_pMenuList)
	{
		return;
	}

	CContainerUI* pMenuItem = NULL;
	if (!m_MenuItemBuilder.GetMarkup()->IsValid())
	{
		pMenuItem = (CContainerUI*)m_MenuItemBuilder.Create(_T("Item\\ItemMenuStyle.xml"));
	}
	else
	{
		pMenuItem = (CContainerUI*)m_MenuItemBuilder.Create();
	}

	CLabelUI*	pMenuText = (CLabelUI*)pMenuItem->FindSubControl(_T("menu_text"));
	if (pMenuText)
	{
		pMenuText->SetText(lptcsMenu);
	}
	pMenuItem->SetTag((UINT_PTR)nMenuId);

	m_pMenuList->Add(pMenuItem);
	m_nHeight += pMenuItem->GetFixedHeight();

	
}

void CItemMenuUI::AddMenuSplitter()
{
	if (!m_pMenuList)
	{
		return;
	}

	CControlUI* pMenuSplitter = NULL;
	if (!m_MenuSplitterBuilder.GetMarkup()->IsValid())
	{
		pMenuSplitter = (CControlUI*)m_MenuSplitterBuilder.Create(_T("Item\\ItemMenuSplitter.xml"));
	}
	else
	{
		pMenuSplitter = (CControlUI*)m_MenuSplitterBuilder.Create();
	}

	m_pMenuList->Add(pMenuSplitter);
	m_nHeight += pMenuSplitter->GetFixedHeight();

}


void CItemMenuUI::CreateMenu()
{
	if (GetHWND())
	{
		return;
	}
	::GetCursorPos(&m_pt);
	this->Create(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), this->GetWindowClassName(), WS_POPUP, 0);

	RECT rt	= {0};
	::GetWindowRect(GetHWND(), &rt);
	m_nHeight = rt.bottom	- rt.top;
}

void CItemMenuUI::ShowMenu()
{
	//border
	RECT rt	= {0};
	::GetWindowRect(GetHWND(), &rt);
	int nWidth	= rt.right	- rt.left;

	int	nShowX = m_pt.x;
	int	nShowY = m_pt.y;

	if (GetHolder() && GetHolder()->GetManager())
	{
		RECT rtHolder	= GetHolder()->GetPos();
		RECT rtPaint	= {0};

		::GetWindowRect(GetHolder()->GetManager()->GetPaintWindow(), &rtPaint);

		rtHolder.left	+= rtPaint.left;
		rtHolder.right	+= rtPaint.left;
		rtHolder.top	+= rtPaint.top;
		rtHolder.bottom	+= rtPaint.top;

		if (nShowX > ( rtHolder.left + (int)((rtHolder.right - rtHolder.left) * m_fLeftWeight)))
		{
			nShowX = nShowX - nWidth;
		}
	}

	MONITORINFO oMonitor = {0};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromPoint(m_pt, MONITOR_DEFAULTTONEAREST), &oMonitor);

	int nScrWidth	= oMonitor.rcMonitor.right - oMonitor.rcMonitor.left;
	int nScrHeight	= oMonitor.rcMonitor.bottom - oMonitor.rcMonitor.top;
	if (nShowX + nWidth > nScrWidth)
	{
		nShowX = nShowX - nWidth;
	}

	if (nShowY + m_nHeight > nScrHeight)
	{
		nShowY = nShowY - m_nHeight;
	}


	::SetWindowPos(this->GetHWND(), NULL, nShowX, nShowY, nWidth, m_nHeight, SWP_NOZORDER | SWP_SHOWWINDOW);

}

void CItemMenuUI::SetLeftWeight( float fWeight )
{
	if (fWeight < 0.0f || fWeight > 1.0f)
	{
		return;
	}
	m_fLeftWeight = fWeight;
}	
