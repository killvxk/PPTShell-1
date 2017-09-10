// GroupExplorerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PPTShell.h"
#include "GroupExplorerDlg.h"
#include "DUI/GroupExplorer.h"

// CGroupExplorerDlg 对话框

IMPLEMENT_DYNAMIC(CGroupExplorerDlg, CPanelInnerDlg)

CGroupExplorerDlg::CGroupExplorerDlg(CWnd* pParent /*=NULL*/)
	: CPanelInnerDlg(pParent)
{

}

CGroupExplorerDlg::~CGroupExplorerDlg()
{
}


BEGIN_MESSAGE_MAP(CGroupExplorerDlg, CPanelInnerDlg)
END_MESSAGE_MAP()

BOOL CGroupExplorerDlg::OnInitDialog()
{
	__super::OnInitDialog();

	m_nWidth = 150;
	//CGroupExplorerUI*  m_pGroupExplorer = new CGroupExplorerUI();
	CGroupExplorerUI*  m_pGroupExplorer = CGroupExplorerUI::GetInstance();
	m_pGroupExplorer->Subclass(this->GetSafeHwnd());
	m_pGroupExplorer->SendMessage(WM_CREATE);	// load skin


	return TRUE;
}

int CGroupExplorerDlg::GetInnerIndex()
{
	return eInner_Group;
}

int CGroupExplorerDlg::GetInnerWidth()
{
	return m_nWidth;
}

void CGroupExplorerDlg::SetInnerWidth( int nWidth )
{
	m_nWidth = nWidth;

	CPanelInnerDlg::AdjustMainPanel();
}

// BOOL CGroupExplorerDlg::PreTranslateMessage( MSG* pMsg )
// {
// 	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
// 	{
// 		CGroupExplorerUI::GetInstance()->NotifyReturnToEdit();
// 	}
// 	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB)
// 	{
// 		CGroupExplorerUI::GetInstance()->NotifyTabToEdit();
// 		return TRUE;
// 	}
// 	return __super::PreTranslateMessage(pMsg );
// }

CWindowWnd* CGroupExplorerDlg::GetWindowWnd()
{
	return dynamic_cast<CWindowWnd*>(CGroupExplorerUI::GetInstance());
}


// CGroupExplorerDlg 消息处理程序
