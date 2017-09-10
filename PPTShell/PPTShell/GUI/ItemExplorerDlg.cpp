// ItemExplorerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PPTShell.h"
#include "ItemExplorerDlg.h"
#include "DUI/ItemExplorer.h"

// CItemExplorerDlg 对话框

IMPLEMENT_DYNAMIC(CItemExplorerDlg, CPanelInnerDlg)

CItemExplorerDlg::CItemExplorerDlg(CWnd* pParent /*=NULL*/)
	: CPanelInnerDlg(pParent)
{

}

CItemExplorerDlg::~CItemExplorerDlg()
{
}

BEGIN_MESSAGE_MAP(CItemExplorerDlg, CPanelInnerDlg)
END_MESSAGE_MAP()

BOOL CItemExplorerDlg::OnInitDialog()
{
	__super::OnInitDialog();

	CItemExplorerUI*  m_pGroupExplorer = CItemExplorerUI::GetInstance();
	m_pGroupExplorer->Subclass(this->GetSafeHwnd());
	m_pGroupExplorer->SendMessage(WM_CREATE);	// load skin

	return TRUE;
}

int CItemExplorerDlg::GetInnerIndex()
{
	return 	eInner_Item;
}

int CItemExplorerDlg::GetInnerWidth()
{
	return 250;
}

void CItemExplorerDlg::SetInnerWidth( int nWidth )
{

}

CWindowWnd* CItemExplorerDlg::GetWindowWnd()
{
	return dynamic_cast<CWindowWnd*>(CItemExplorerUI::GetInstance());
}


// CItemExplorerDlg 消息处理程序
