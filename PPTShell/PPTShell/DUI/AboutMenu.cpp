#include "StdAfx.h"
#include "AboutMenu.h"
#include "UpdateDialog.h"
#include "AboutDialogUI.h"
#include "Feedback.h"
#include "QRCodeDialogUI.h"
#include "Common.h"
#include "UpdateLogUI.h"
#include "DUI/OptionsDialogUI.h"


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
	m_pDotContainer->SetVisible(m_bUpdate);;
	//CListUI* list=(CListUI*)m_PaintManager.FindControl(_T("menu_List"));
	//if(list)
	//{
	//	list->SelectItem(-1, true);
	//}
#ifndef DEVELOP_VERSION
	CContainerUI* pOptionContainer = static_cast<CContainerUI*>(m_PaintManager.FindControl(_T("menu_Options")));
	CListUI* pList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("menu_List")));
	pList->Remove(pOptionContainer);
	CVerticalLayoutUI* pMainContainer = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("mainContainer")));
	pMainContainer->SetBkImage(_T("AboutMenu\\bg2.png"));
	::SetWindowPos(*this, NULL, ptPos.x, ptPos.y, 160, 210, SWP_NOZORDER | SWP_NOACTIVATE);
#endif
}

void CAboutMenu::OnFinalMessage( HWND hWnd )
{
	__super::OnFinalMessage(hWnd);
	delete this;
}


LRESULT CAboutMenu::OnKillFocus( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	this->Close();
	return __super::OnKillFocus(uMsg, wParam, lParam, bHandled);; 
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
	pFeedbackDlg->Create(m_hWndParent, _T("FeedbackWindow"), WS_VISIBLE, 0, 0, 0, 0, 0);
	MoveWindow(pFeedbackDlg->GetHWND(), rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
}

void CAboutMenu::OnBtnAbout( TNotifyUI& msg )
{
	CRect rect;
	::GetWindowRect(m_hWndParent,&rect);
	CAboutDialogUI * pUpdateDlg = new CAboutDialogUI();
	pUpdateDlg->Create(m_hWndParent, _T("AboutWindow"), WS_POPUP | WS_VISIBLE, 0, 0, 0, 0, 0);
	MoveWindow(pUpdateDlg->GetHWND(), rect.left, rect.top, rect.Width(), rect.Height(), TRUE);

}

void CAboutMenu::OnBtnOptions(TNotifyUI& msg)
{
	CRect rect;
	::GetWindowRect(m_hWndParent, &rect);
	COptionsDialogUI * pOptionsDlg = new COptionsDialogUI();
	pOptionsDlg->Create(m_hWndParent, _T("OptionsWindow"), WS_POPUP | WS_VISIBLE, 0, 0, 0, 0, 0);
	MoveWindow(pOptionsDlg->GetHWND(), rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
}




