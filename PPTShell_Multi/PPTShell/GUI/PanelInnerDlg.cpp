// ItemExplorerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PPTShell.h"
#include "PanelInnerDlg.h"
#include "MainPane.h"

// CPanelInnerDlg 对话框

IMPLEMENT_DYNAMIC(CPanelInnerDlg, CDialog)

CPanelInnerDlg::CPanelInnerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPanelInnerDlg::IDD, pParent)
{

}

CPanelInnerDlg::~CPanelInnerDlg()
{
}

void CPanelInnerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPanelInnerDlg, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CPanelInnerDlg::OnInitDialog()
{
	return CDialog::OnInitDialog();
}

void CPanelInnerDlg::OnSize( UINT nType, int cx, int cy )
{
	CDialog::OnSize(nType, cx, cy);
}

void CPanelInnerDlg::AdjustMainPanel()
{
	CMainPane* pMainPanel = (CMainPane*)m_MainPanel;
	pMainPanel->AdjustPanel();
}

BOOL CPanelInnerDlg::PreTranslateMessage( MSG* pMsg )
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE
			|| pMsg->wParam == VK_RETURN
			|| pMsg->wParam == VK_TAB)
		{
			this->GetWindowWnd()->HandleMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			return TRUE;
		}
	}

	else if (pMsg->message == WM_SYSKEYDOWN)
	{
		if (pMsg->wParam == VK_F4)
		{
			this->GetWindowWnd()->HandleMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			return TRUE;
		}
	}
	return __super::PreTranslateMessage(pMsg);
}

CDockablePane* CPanelInnerDlg::m_MainPanel = NULL;


// CPanelInnerDlg 消息处理程序
