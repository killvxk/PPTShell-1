#include "StdAfx.h"
#include "ProgressMaskerUI.h"

CProgressMaskerUI::CProgressMaskerUI(void)
{
	m_dwTaskId = 0;
}

CProgressMaskerUI::~CProgressMaskerUI(void)
{

}

DuiLib::CDuiString CProgressMaskerUI::GetSkinFolder()
{
	return CDuiString(_T("skins"));
}

DuiLib::CDuiString CProgressMaskerUI::GetSkinFile()
{
	return _T("ProgressMasker\\view.xml");
}

void CProgressMaskerUI::InitWindow()
{
	__super::InitWindow();



	CRect rtMain;
	AfxGetApp()->m_pMainWnd->GetWindowRect(rtMain);

	m_wndShadow.SetMaskSize(rtMain);
	m_wndShadow.Create(GetHWND());
}

LPCTSTR CProgressMaskerUI::GetWindowClassName( void ) const
{
	return _T("CProgressMaskerUI");
}

void CProgressMaskerUI::OnFinalMessage( HWND hWnd )
{
	//CancelPPTOperation(m_dwTaskId);
	delete this;
}

void CProgressMaskerUI::ShowWindow( bool bShow /*= true*/, bool bTakeFocus /*= true */ )
{
	if (! GetHWND()
		|| !IsWindow(GetHWND()))
	{
		this->Create(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), this->GetWindowClassName(), WS_POPUP, 0);
		this->CenterWindow();
	}

	__super::ShowWindow(bShow, bTakeFocus);
}

void CProgressMaskerUI::SetOperationId( DWORD dwId )
{
	m_dwTaskId = dwId;
}
