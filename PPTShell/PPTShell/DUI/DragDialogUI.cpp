#include "StdAfx.h"
#include "DragDialogUI.h"


CDragDialogUI::CDragDialogUI(void)
{
	m_ppDragDialogUI = NULL;
}

CDragDialogUI::~CDragDialogUI(void)
{
}

LPCTSTR CDragDialogUI::GetWindowClassName( void ) const
{
	return _T("DragDialog");
}

DuiLib::CDuiString CDragDialogUI::GetSkinFile()
{
	return _T("Drag\\Drag.xml");
}

DuiLib::CDuiString CDragDialogUI::GetSkinFolder()
{
	return _T("skins");
}

void CDragDialogUI::InitWindow()
{
	CLabelUI* pImageLabel	= static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("DragImage")));
	pImageLabel->SetBkImage(m_strBkImage);
}

void CDragDialogUI::SetBkImage(LPCTSTR strBkImage)
{
	m_strBkImage = strBkImage;
	CLabelUI* pImageLabel	= static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("DragImage")));
	pImageLabel->SetBkImage(m_strBkImage);
}

void CDragDialogUI::SetUserData(CDragDialogUI** pDragDialog)
{
	m_ppDragDialogUI = pDragDialog;
}

LRESULT CDragDialogUI::OnKillFocus(UINT , WPARAM , LPARAM , BOOL& bHandled)
{
	//Close();
	ShowWindow(SW_HIDE);
	//if( m_ppDragDialogUI != NULL)
	//	*m_ppDragDialogUI = NULL;

	return 0;
}

void CDragDialogUI::BeginShowWindow(HWND hParent /*= NULL*/)
{	
	static bool bCreate=false;
	if(!bCreate){
		Create(hParent, _T("DragWindow"), WS_VISIBLE, 0, 0, 0, 0, 0);
		bCreate=true;
	}
	ShowWindow(SW_SHOW);
}
