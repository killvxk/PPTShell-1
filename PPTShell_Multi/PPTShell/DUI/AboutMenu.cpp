#include "StdAfx.h"
#include "AboutMenu.h"
#include "UpdateDialog.h"
#include "AboutDialogUI.h"
#include "Feedback.h"
#include "QRCodeDialogUI.h"
#include "Common.h"
#include "UpdateLogUI.h"

CAboutMenu::CAboutMenu(void)
{
}

CAboutMenu::~CAboutMenu(void)
{
}

LPCTSTR CAboutMenu::GetWindowClassName() const
{
	return _T("AboutMenu");
}

DuiLib::CDuiString CAboutMenu::GetSkinFile()
{
	return _T("AboutMenu\\menu.xml");
}

DuiLib::CDuiString CAboutMenu::GetSkinFolder()
{
	return _T("skins");
}


void CAboutMenu::Init( HWND hWndParent, POINT ptPos, bool bUpdate, tstring strVer, tstring strContent )
{
	::SetWindowPos(*this, NULL, ptPos.x, ptPos.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
	
	m_hWndParent	= hWndParent;
	m_bUpdate		= bUpdate;
	m_strVer		= strVer;
	m_strContent	= strContent;

//	CControlUI* m_pMenuUpdate = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("menu_Update")));
// 	if(m_pMenuUpdate && !m_bUpdate && m_pMenuUpdate->IsVisible())
// 	{	
// 		CControlUI* m_pMenuUpdateLine = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("menu_Update_line")));
// 		m_pMenuUpdate->SetVisible(false);
// 		m_pMenuUpdateLine->SetVisible(false);
// 		CRect rect;
// 		GetClientRect(GetHWND(), &rect);
// 		ResizeClient(rect.right- rect.left, rect.bottom - rect.top - 52);
// 	}
// 	
	m_pDotContainer = static_cast<CContainerUI*>(m_PaintManager.FindControl(_T("new_Version")));

	m_pDotContainer->SetVisible(m_bUpdate);
}

void CAboutMenu::OnFinalMessage( HWND /*hWnd*/ )
{
	//m_hWnd = NULL;
	//delete this;
}


LRESULT CAboutMenu::OnKillFocus( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	Close();
	//bHandled = FALSE;

	//CListUI* pList	= static_cast<CListUI*>(m_PaintManager.FindControl(_T("menu_List")));
	//CListContainerElementUI* pItem = static_cast<CListContainerElementUI*>(pList->GetItemAt(0));
	//pItem->Select(false);
	//ShowWindow(SW_HIDE);
	return __super::OnKillFocus(uMsg, wParam, lParam, bHandled); 
}

LPCTSTR CAboutMenu::GetItemText(CControlUI* pList, int iItem, int iSubItem)
{
	return pList->GetUserData();
}

void CAboutMenu::OnBtnUpdate(TNotifyUI& msg)
{
	//CRect rect;
	//::GetWindowRect(m_hWndParent,&rect);
	//CUpdateDialogUI * pUpdateDlg = new CUpdateDialogUI(rect, m_bUpdate);
	//pUpdateDlg->Create(m_hWndParent, _T("UpdateWindow"), WS_VISIBLE, 0, 0, 0, 0, 0);

	////pUpdateDlg->setDlgHwnd(m_hWndParent);
	//pUpdateDlg->setUpdate(m_b)
	//pUpdateDlg->setUpdateVersion(m_strVer.c_str());
	//pUpdateDlg->setUpdateLog(m_strContent.c_str());
	//pUpdateDlg->ShowWindow();

	//Close();

	::PostMessage(m_hWndParent, WM_NOTIFY_UPDATE, NOTIFY_FORM_MENU, 0);
}

void CAboutMenu::OnBtnLog(TNotifyUI& msg)
{
	CRect rect;
	GetWindowRect(m_hWndParent,&rect);
	CUpdateLogUI * pUpdateLogDlg = new CUpdateLogUI();
	pUpdateLogDlg->Create(m_hWndParent, _T("UpadteLogWindow"), WS_POPUP | WS_VISIBLE, 0, 0, 0, 0, 0);
	MoveWindow(pUpdateLogDlg->GetHWND(), rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
}

void CAboutMenu::OnBtnFeedBack( TNotifyUI& msg )
{
	CRect rect;
	::GetWindowRect(m_hWndParent,&rect);
	CFeedbackDialogUI * pFeedbackDlg = new CFeedbackDialogUI();
	pFeedbackDlg->Create(m_hWndParent, _T("FeedbackWindow"), WS_OVERLAPPED | WS_VISIBLE, 0, 0, 0, 0, 0);
	int x = rect.left;
	int width = rect.Width();
	if(rect.left<0)
	{
		x = 0;
		width = rect.Width() - rect.left;
	}
	int y = rect.top;
	int height = rect.Height();
	if(rect.top<0)
	{
		y = 0;
		height = rect.Height() - rect.top;
	}
	MoveWindow(pFeedbackDlg->GetHWND(), x, y, width, height, TRUE);
}

void CAboutMenu::OnBtnAbout( TNotifyUI& msg )
{
	CRect rect;
	::GetWindowRect(m_hWndParent,&rect);
	CAboutDialogUI * pUpdateDlg = new CAboutDialogUI();
	pUpdateDlg->Create(m_hWndParent, _T("AboutWindow"), WS_POPUP | WS_VISIBLE, 0, 0, 0, 0, 0);
	MoveWindow(pUpdateDlg->GetHWND(), rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
}