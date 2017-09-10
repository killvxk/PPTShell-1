#include "stdafx.h"
#include "SearchTipListDlg.h"

CSearchTipListDlg::CSearchTipListDlg()
{

}

CSearchTipListDlg::~CSearchTipListDlg()
{

}

LPCTSTR CSearchTipListDlg::GetWindowClassName() const
{
	return _T("SearchTipListDlg");
}

DuiLib::CDuiString CSearchTipListDlg::GetSkinFile()
{
	return _T("RightBar\\SearchTipListDlg.xml");
}

DuiLib::CDuiString CSearchTipListDlg::GetSkinFolder()
{
	return CDuiString(_T("skins"));
}

void CSearchTipListDlg::InitWindow()
{
	m_pBtnPPT = dynamic_cast<CButtonUI *>(m_PaintManager.FindControl(_T("tipListPPT")));
	m_pBtnBaidu = dynamic_cast<CButtonUI *>(m_PaintManager.FindControl(_T("tipListBaidu")));

	m_pLabelPPT = dynamic_cast<CLabelUI *>(m_PaintManager.FindControl(_T("tipLabelPPT")));
	m_pLabelBaidu = dynamic_cast<CLabelUI *>(m_PaintManager.FindControl(_T("tipLabelBaidu")));

	m_pBtnPPT->OnEvent += MakeDelegate(this, &CSearchTipListDlg::OnBtnEvent);
	m_pBtnBaidu->OnEvent += MakeDelegate(this, &CSearchTipListDlg::OnBtnEvent);
}

CControlUI* CSearchTipListDlg::CreateControl( LPCTSTR pstrClass )
{
	return NULL;
}

LRESULT CSearchTipListDlg::OnKillFocus( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	LRESULT result = __super::OnKillFocus(uMsg, wParam, lParam, bHandled);
	if(m_OnKillFocusCallBack)
		m_OnKillFocusCallBack(NULL);
	return result;
}

void CSearchTipListDlg::SetKillFocusCallBack( CDelegateBase& delegate )
{
	m_OnKillFocusCallBack.clear();
	m_OnKillFocusCallBack += delegate;
}

void CSearchTipListDlg::SetSearchCallBack( CDelegateBase& delegate )
{
	m_OnSearchCallBack.clear();
	m_OnSearchCallBack += delegate;
}

bool CSearchTipListDlg::OnBtnEvent( void* pObj )
{
	TEventUI* pEvent = (TEventUI*)pObj;
	if(pEvent->Type == UIEVENT_MOUSEENTER)
	{
		CButtonUI* pButton = dynamic_cast<CButtonUI *>(pEvent->pSender);
		if(pButton->GetName() == _T("tipListPPT"))
		{
			m_pLabelPPT->SetBkColor(0xFF11B0B6);
		}
		else if(pButton->GetName() == _T("tipListBaidu"))
		{
			m_pLabelBaidu->SetBkColor(0xFF11B0B6);
		}
	}
	else if(pEvent->Type == UIEVENT_MOUSELEAVE)
	{
		CButtonUI* pButton = dynamic_cast<CButtonUI *>(pEvent->pSender);
		if(pButton->GetName() == _T("tipListPPT"))
		{
			m_pLabelPPT->SetBkColor(0xFFFFFFFF);
		}
		else if(pButton->GetName() == _T("tipListBaidu"))
		{
			m_pLabelBaidu->SetBkColor(0xFFFFFFFF);
		}
	}
	return true;
}

bool CSearchTipListDlg::OnBtnTipListPPT( TNotifyUI& msg )
{
	if(m_OnSearchCallBack)
		m_OnSearchCallBack(&msg);

	return true;
}

bool CSearchTipListDlg::OnBtnTipListBaidu( TNotifyUI& msg )
{
	if(m_OnSearchCallBack)
		m_OnSearchCallBack(&msg);

	return true;
}

