// RightPanel.cpp : implementation file
//

#include "stdafx.h"
#include "PPTShell.h"
#include "MainFrm.h"
#include "MainPane.h"

#include "MainControlDlg.h"
#include "GroupExplorerDlg.h"
#include "ItemExplorerDlg.h"




// CRightPanel

IMPLEMENT_DYNAMIC(CMainPane, CDockablePane)

CMainPane::CMainPane()
{

}

CMainPane::~CMainPane()
{
}


BEGIN_MESSAGE_MAP(CMainPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

int CMainPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CPanelInnerDlg::m_MainPanel = this;
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(!AddDialog(new CItemExplorerDlg()))
		return -1;

	if(!AddDialog(new CGroupExplorerDlg()))
		return -1;

	if(!AddDialog(new CMainControlDlg()))
		return -1;

	GetDialogAt(eInner_Navgate)->ShowWindow(SW_SHOW);

	return 0;
}
void CMainPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	if (!GetSafeHwnd())
	{
		return;
	}

	int nCurrentX = cx;
	for (int i = 0; i < this->GetDialogCount(); ++i)
	{
		CPanelInnerDlg* pDialog = (CPanelInnerDlg*)this->GetDialogAt(i);
		if (pDialog && pDialog->IsWindowVisible())
		{
			nCurrentX -= pDialog->GetInnerWidth();
			pDialog->SetWindowPos(NULL, nCurrentX, 0, pDialog->GetInnerWidth(), cy, SWP_NOACTIVATE | SWP_NOZORDER);

		}
	}

}

CDialog* CMainPane::GetMainControlDlg()
{
	return GetDialogAt(eInner_Navgate);
}

void CMainPane::AdjustPanel()
{
	//calc minimum width for dock panel
	int nWidth = 0;
	for (int i = 0; i < this->GetDialogCount(); ++i)
	{
		CPanelInnerDlg* pDialog = (CPanelInnerDlg*)this->GetDialogAt(i);
		if (pDialog && pDialog->IsWindowVisible())
		{
			nWidth += pDialog->GetInnerWidth();
		}
	}

	RECT rtPanel;
	this->GetWindowRect(&rtPanel);
	this->SetMinSize(CSize(nWidth, rtPanel.bottom - rtPanel.top));
	this->SetWindowPos(NULL, rtPanel.right - nWidth, rtPanel.right, nWidth, rtPanel.bottom - rtPanel.top, SWP_NOZORDER | SWP_NOACTIVATE);
	
	//resize main wnd
	HWND	hMain = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
	RECT	rtFrameClient;
	::GetClientRect(hMain, &rtFrameClient);
	::PostMessage(hMain, WM_SIZE, 0, MAKELONG(rtFrameClient.right - rtFrameClient.left, rtFrameClient.bottom - rtFrameClient.top));
}

bool CMainPane::AddDialog( CPanelInnerDlg* pDialog )
{
	if (!pDialog || !pDialog->Create(IDD_DIALOG_MAIN, this))
	{
		return false;
	}

	m_mapInnerDlgs.insert(make_pair(pDialog->GetInnerIndex(), pDialog));

	return true;
}

CDialog* CMainPane::GetDialogAt( int nIndex )
{
	map<int, CPanelInnerDlg*>::iterator  itor = m_mapInnerDlgs.find(nIndex);
	if (itor != m_mapInnerDlgs.end())
	{
		return itor->second;
	}

	return NULL;
}

int CMainPane::GetDialogCount()
{
	return m_mapInnerDlgs.size();
}



