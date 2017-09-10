#include "stdafx.h"
#include "ResourceExplorer.h"
#include "GUI/PanelInnerDlg.h"

CResourceExplorerUI::CResourceExplorerUI()
{	
	m_bLastShowNetless	= false;
	m_bLastShowMask		= false;
}
CResourceExplorerUI::~CResourceExplorerUI()
{
	
}

LPCTSTR CResourceExplorerUI::GetWindowClassName() const
{
	return _T("CResourceExplorerUI"); 
}

UINT CResourceExplorerUI::GetClassStyle() const
{
	return CS_DBLCLKS;
}

void CResourceExplorerUI::InitWindow()
{
	__super::InitWindow();

	m_lbCount		= dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(_T("type_count")));
	m_lbName		= dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(_T("type_name")));
	m_pList			= dynamic_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("content")));
	m_pLoading		= dynamic_cast<CGifAnimUI*>(m_PaintManager.FindControl(_T("loading")));
	m_pBody			= dynamic_cast<CContainerUI*>(m_PaintManager.FindControl(_T("body")));
	m_pMask			= dynamic_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("mask")));
	m_pBackGround	= dynamic_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("bk")));
	m_layNetless	= dynamic_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("netless")));
	m_btnRefresh	= dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(_T("refresh")));
	m_pSliderTabLayout = dynamic_cast<CSliderTabLayoutUI*>(m_PaintManager.FindControl(_T("sliderTabLayout")));
}

CDuiString CResourceExplorerUI::GetSkinFile()
{
	return _T("RightBar\\Explorer\\ResourceExplorer.xml");
}

CDuiString CResourceExplorerUI::GetSkinFolder()
{
	return CDuiString(_T("skins"));
}

CControlUI* CResourceExplorerUI::CreateControl(LPCTSTR pstrClass)
{
	if( _tcscmp(pstrClass, _T("GifAnim")) == 0 )
		return new CGifAnimUI;
	else if( _tcscmp(pstrClass, _T("SliderTabLayoutUI")) == 0 )
	{
		return new CSliderTabLayoutUI;
	}
	return __super::CreateControl(pstrClass);
}

void CResourceExplorerUI::SetCountText( int nCount )
{
	TCHAR szText[32] = {0};
	_stprintf_s(szText, _T("%d"), nCount);
	if(m_lbCount)
		m_lbCount->SetText(szText);
}

void CResourceExplorerUI::SetCountText( LPCTSTR lptcsCount )
{
	if (lptcsCount != NULL)
	{
		m_lbCount->SetText(lptcsCount);
	}
	else
	{
		m_lbCount->SetText(_T(""));
	}

	
}

void CResourceExplorerUI::SetTitleText( LPCTSTR lptcsName )
{
	if (lptcsName != NULL)
	{
		m_lbName->SetText(lptcsName);
		m_lbName->SetFixedWidth(_tcslen(lptcsName) * 8);
		m_lbName->SetToolTip(lptcsName);
	}
	else
	{
		m_lbName->SetText(_T(""));
	}
}

void CResourceExplorerUI::StartMask()
{
	m_pMask->SetVisible(true);
	m_pLoading->PlayGif();

	m_bLastShowMask = true;
}

void CResourceExplorerUI::StopMask()
{
	m_pMask->SetVisible(false);
	m_pLoading->StopGif();

	m_bLastShowMask = false;
}

void CResourceExplorerUI::SetBkColor( DWORD dwColor )
{
	m_pBackGround->SetBkColor(dwColor);
}

void CResourceExplorerUI::SetFixedWidth( int nWidth )
{
	RECT rc;
	::GetWindowRect(this->GetHWND(), &rc);
	rc.left = rc.right - nWidth;
	::MoveWindow(this->GetHWND(), rc.left, rc.top, (rc.right - rc.left),  rc.bottom - rc.top, TRUE);

}


LRESULT CResourceExplorerUI::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(uMsg == WM_USER_HIDE)
	{
		ShowWindow(false);
	}
	bHandled = FALSE;
	return 0;
}

CButtonUI* CResourceExplorerUI::GetRefreshBtn()
{
	return m_btnRefresh;
}	

void CResourceExplorerUI::ShowNetlessUI( bool bShow )
{
	if (bShow)
	{
		this->SetCountText(0);
		this->ShowWindow(true);
	}
	m_pList->SetVisible(!bShow);
	m_layNetless->SetVisible(bShow);

	m_bLastShowNetless = bShow;
	
}

void CResourceExplorerUI::ShowWindow( bool bShow /*= true*/, bool bTakeFocus /*= true */ )
{
	__super::ShowWindow(bShow, true);

	CPanelInnerDlg::AdjustMainPanel();
}

void CResourceExplorerUI::OnBtnClose( TNotifyUI& msg )
{
	this->ShowWindow(false);
}
