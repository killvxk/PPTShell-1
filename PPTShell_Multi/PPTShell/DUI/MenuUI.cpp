#include "StdAfx.h"
#include "MenuUI.h"
#include "EventCenter\EventDefine.h"

CMenuUI::CMenuUI(void)
{
	m_pMenuOpen = NULL;
	m_pMenuImportSplit = NULL;
	m_pMenuImport = NULL;
	m_pMenuRenameSplit = NULL;
	m_pMenuRename = NULL;
	m_pMenuClearupSplit = NULL;
	m_pMenuClearup = NULL;
	m_pMenuUploadNetdiscSplit = NULL;
	m_pMenuUploadNetdisc = NULL;
	m_pMenuDownloadLocalSplit = NULL;
	m_pMenuDownloadLocal = NULL;
	m_pMenuDeleteSplit = NULL;
	m_pMenuDelete = NULL;
	m_pMenuEditExercisesSplit = NULL;
	m_pMenuEditExercises = NULL;
	m_pMenuUploadSplit = NULL;
	m_pMenuUpload = NULL;
	m_nType=-1;
	m_nItemIndex=-1;
	m_bIsCloud = false;
}

CMenuUI::~CMenuUI(void)
{
}

LPCTSTR CMenuUI::GetWindowClassName() const
{
	return _T("MenuUI");
}

DuiLib::CDuiString CMenuUI::GetSkinFile()
{
	return _T("RightBar\\Menu.xml");
}

DuiLib::CDuiString CMenuUI::GetSkinFolder()
{
	return _T("skins");
}

void CMenuUI::InitWindow()
{
	m_pMenuOpen = static_cast<CListContainerElementUI*>(m_PaintManager.FindControl(_T("menu_open")));
	m_pMenuImportSplit = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("menu_import_split")));
	m_pMenuImport = static_cast<CListContainerElementUI*>(m_PaintManager.FindControl(_T("menu_import")));
	m_pMenuRenameSplit = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("menu_rename_split")));
	m_pMenuRename = static_cast<CListContainerElementUI*>(m_PaintManager.FindControl(_T("menu_rename")));
	m_pMenuClearupSplit = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("menu_clearup_split")));
	m_pMenuClearup = static_cast<CListContainerElementUI*>(m_PaintManager.FindControl(_T("menu_clearup")));
	m_pMenuUploadNetdiscSplit = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("menu_upload_netdisc_split")));
	m_pMenuUploadNetdisc = static_cast<CListContainerElementUI*>(m_PaintManager.FindControl(_T("menu_upload_netdisc")));
	m_pMenuDownloadLocalSplit = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("menu_download_local_split")));
	m_pMenuDownloadLocal = static_cast<CListContainerElementUI*>(m_PaintManager.FindControl(_T("menu_download_local")));
	m_pMenuDeleteSplit = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("menu_delete_split")));
	m_pMenuDelete = static_cast<CListContainerElementUI*>(m_PaintManager.FindControl(_T("menu_delete")));
	m_pMenuEditExercisesSplit = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("menu_edit_exercises_split")));
	m_pMenuEditExercises = static_cast<CListContainerElementUI*>(m_PaintManager.FindControl(_T("menu_edit_exercises")));
	m_pMenuUploadSplit = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("menu_upload_split")));
	m_pMenuUpload = static_cast<CListContainerElementUI*>(m_PaintManager.FindControl(_T("menu_upload")));
}


LRESULT CMenuUI::OnKillFocus( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	ShowWindow(false);
	return __super::OnKillFocus(uMsg, wParam, lParam, bHandled); 
}

void CMenuUI::OnMenuOpenClick( TNotifyUI& msg )
{
	ShowWindow(false);
}

void CMenuUI::OnMenuImportClick( TNotifyUI& msg )
{
	ShowWindow(false);
	BroadcastEvent(EVT_MENU_IMPORT, 0, 0, NULL);
}

void CMenuUI::OnMenuRenameClick( TNotifyUI& msg )
{
	ShowWindow(false);
	if(m_nItemIndex>=0)
	{
		BroadcastEvent(EVT_MENU_RENAME,m_nItemIndex,(LPARAM)m_bIsCloud,NULL);
	}
}

void CMenuUI::OnMenuClearupClick( TNotifyUI& msg )
{
	ShowWindow(false);
	BroadcastEvent(EVT_MENU_CLEARUP, 0, 0, NULL);
}

void CMenuUI::OnMenuUploadNetdiscClick( TNotifyUI& msg )
{
	ShowWindow(false);
	if(m_nItemIndex>=0)
	{
		BroadcastEvent(EVT_MENU_UPLOADNETDISC,m_nItemIndex,(LPARAM)m_bIsCloud,NULL);
	}
}

void CMenuUI::OnMenuDownloadLocalClick( TNotifyUI& msg )
{
	ShowWindow(false);
	if(m_nType>=0)
	{
		BroadcastEvent(EVT_MENU_DOWNLOADLOCAL,true,m_nType,NULL);
	}
	else if(m_nItemIndex>=0)
	{
		BroadcastEvent(EVT_MENU_DOWNLOADLOCAL,false,m_nItemIndex,NULL);
	}
}

void CMenuUI::OnMenuDeleteClick( TNotifyUI& msg )
{
	ShowWindow(false);
	if(m_nItemIndex>=0)
	{
		BroadcastEvent(EVT_MENU_DELETE,m_nItemIndex,(LPARAM)m_bIsCloud,NULL);
	}
}

void CMenuUI::OnMenuEditExercisesClick( TNotifyUI& msg )
{
	ShowWindow(false);
	if(m_nItemIndex>=0)
	{
		BroadcastEvent(EVT_MENU_EDITEXERCISES,m_nItemIndex,(LPARAM)m_bIsCloud,NULL);
	}
}

void CMenuUI::OnMenuUploadClick( TNotifyUI& msg )
{
	ShowWindow(false);
	BroadcastEvent(EVT_MENU_UPLOAD, 0, 0, NULL);
}

void CMenuUI::HideAllMenuItem()
{
	if(m_pMenuOpen)
	{
		m_pMenuOpen->SetVisible(false);
	}
	if(m_pMenuImportSplit)
	{
		m_pMenuImportSplit->SetVisible(false);
	}
	if(m_pMenuImport)
	{
		m_pMenuImport->SetVisible(false);
	}
	if(m_pMenuRenameSplit)
	{
		m_pMenuRenameSplit->SetVisible(false);
	}
	if(m_pMenuRename)
	{
		m_pMenuRename->SetVisible(false);
	}
	if(m_pMenuClearupSplit)
	{
		m_pMenuClearupSplit->SetVisible(false);
	}
	if(m_pMenuClearup)
	{
		m_pMenuClearup->SetVisible(false);
	}
	if(m_pMenuUploadNetdiscSplit)
	{
		m_pMenuUploadNetdiscSplit->SetVisible(false);
	}
	if(m_pMenuUploadNetdisc)
	{
		m_pMenuUploadNetdisc->SetVisible(false);
	}
	if(m_pMenuDownloadLocalSplit)
	{
		m_pMenuDownloadLocalSplit->SetVisible(false);
	}
	if(m_pMenuDownloadLocal)
	{
		m_pMenuDownloadLocal->SetVisible(false);
	}
	if(m_pMenuDeleteSplit)
	{
		m_pMenuDeleteSplit->SetVisible(false);
	}
	if(m_pMenuDelete)
	{
		m_pMenuDelete->SetVisible(false);
	}
	if(m_pMenuEditExercisesSplit)
	{
		m_pMenuEditExercisesSplit->SetVisible(false);
	}
	if(m_pMenuEditExercises)
	{
		m_pMenuEditExercises->SetVisible(false);
	}
	if(m_pMenuUploadSplit)
	{
		m_pMenuUploadSplit->SetVisible(false);
	}
	if(m_pMenuUpload)
	{
		m_pMenuUpload->SetVisible(false);
	}
	RefreshSize();
}

void CMenuUI::ShowMenuOpen()
{
	if(m_pMenuOpen)
	{
		m_pMenuOpen->SetVisible(true);
	}
	RefreshMenuItem();
	RefreshSize();
}

void CMenuUI::ShowMenuImport()
{
	if(m_pMenuImport)
	{
		m_pMenuImport->SetVisible(true);
	}
	RefreshMenuItem();
	RefreshSize();
}

void CMenuUI::ShowMenuRename()
{
	if(m_pMenuRename)
	{
		m_pMenuRename->SetVisible(true);
	}
	RefreshMenuItem();
	RefreshSize();
}

void CMenuUI::ShowMenuClearup()
{
	if(m_pMenuClearup)
	{
		m_pMenuClearup->SetVisible(true);
	}
	RefreshMenuItem();
	RefreshSize();
}

void CMenuUI::ShowMenuUploadNetDisc()
{
	if(m_pMenuUploadNetdisc)
	{
		m_pMenuUploadNetdisc->SetVisible(true);
	}
	RefreshMenuItem();
	RefreshSize();
}

void CMenuUI::ShowMenuDownloadLocal()
{
	if(m_pMenuDownloadLocal)
	{
		m_pMenuDownloadLocal->SetVisible(true);
	}
	RefreshMenuItem();
	RefreshSize();
}

void CMenuUI::ShowMenuDelete()
{
	if(m_pMenuDelete)
	{
		m_pMenuDelete->SetVisible(true);
	}
	RefreshMenuItem();
	RefreshSize();
}

void CMenuUI::ShowMenuEditExercises()
{
	if(m_pMenuEditExercises)
	{
		m_pMenuEditExercises->SetVisible(true);
	}
	RefreshMenuItem();
	RefreshSize();
}

void CMenuUI::ShowMenuUpload()
{
	if(m_pMenuUpload)
	{
		m_pMenuUpload->SetVisible(true);
	}
	RefreshMenuItem();
	RefreshSize();
}

void CMenuUI::RefreshMenuItem()
{
	if(m_pMenuImportSplit)
	{
		bool isMenuImportSplitShow=false;
		if(m_pMenuImport&&m_pMenuImport->IsVisible())
		{
			if(m_pMenuOpen&&m_pMenuOpen->IsVisible())
				isMenuImportSplitShow=true;
		}
		m_pMenuImportSplit->SetVisible(isMenuImportSplitShow);
	}
	if(m_pMenuClearupSplit)
	{
		bool isMenuClearupSplitShow=false;
		if(m_pMenuClearup&&m_pMenuClearup->IsVisible())
		{
			if((m_pMenuOpen&&m_pMenuOpen->IsVisible())||(m_pMenuImport&&m_pMenuImport->IsVisible()))
			{
				isMenuClearupSplitShow=true;
			}
		}
		m_pMenuClearupSplit->SetVisible(isMenuClearupSplitShow);
	}
	if(m_pMenuUploadSplit)
	{
		bool isMenuUploadSplitShow=false;
		if(m_pMenuUpload&&m_pMenuUpload->IsVisible())
		{
			if((m_pMenuOpen&&m_pMenuOpen->IsVisible())||(m_pMenuImport&&m_pMenuImport->IsVisible())||(m_pMenuClearup&&m_pMenuClearup->IsVisible()))
			{
				isMenuUploadSplitShow=true;
			}
		}
		m_pMenuUploadSplit->SetVisible(isMenuUploadSplitShow);
	}
	if(m_pMenuUploadNetdiscSplit)
	{
		bool isMenuUploadNetdiscSplitShow=false;
		if(m_pMenuUploadNetdisc&&m_pMenuUploadNetdisc->IsVisible())
		{
			if((m_pMenuOpen&&m_pMenuOpen->IsVisible())||(m_pMenuImport&&m_pMenuImport->IsVisible())||
				(m_pMenuClearup&&m_pMenuClearup->IsVisible())||(m_pMenuUpload&&m_pMenuUpload->IsVisible()))
			{
				isMenuUploadNetdiscSplitShow=true;
			}
		}
		m_pMenuUploadNetdiscSplit->SetVisible(isMenuUploadNetdiscSplitShow);
	}
	if(m_pMenuDownloadLocalSplit)
	{
		bool isMenuDownloadLocalSplitShow=false;
		if(m_pMenuDownloadLocal&&m_pMenuDownloadLocal->IsVisible())
		{
			if((m_pMenuOpen&&m_pMenuOpen->IsVisible())||(m_pMenuImport&&m_pMenuImport->IsVisible())||
				(m_pMenuClearup&&m_pMenuClearup->IsVisible())||(m_pMenuUpload&&m_pMenuUpload->IsVisible())||(m_pMenuUploadNetdisc&&m_pMenuUploadNetdisc->IsVisible()))
			{
				isMenuDownloadLocalSplitShow=true;
			}
		}
		m_pMenuDownloadLocalSplit->SetVisible(isMenuDownloadLocalSplitShow);
	}
	if(m_pMenuDeleteSplit)
	{
		bool isMenuDeleteSplitShow=false;
		if(m_pMenuDelete&&m_pMenuDelete->IsVisible())
		{
			if((m_pMenuOpen&&m_pMenuOpen->IsVisible())||(m_pMenuImport&&m_pMenuImport->IsVisible())||
				(m_pMenuClearup&&m_pMenuClearup->IsVisible())||(m_pMenuUpload&&m_pMenuUpload->IsVisible())||
				(m_pMenuUploadNetdisc&&m_pMenuUploadNetdisc->IsVisible())||(m_pMenuDownloadLocal&&m_pMenuDownloadLocal->IsVisible()))
			{
				isMenuDeleteSplitShow=true;
			}
		}
		m_pMenuDeleteSplit->SetVisible(isMenuDeleteSplitShow);
	}
	if(m_pMenuRenameSplit)
	{
		bool isMenuRenameSplitShow=false;
		if(m_pMenuRename&&m_pMenuRename->IsVisible())
		{
			if((m_pMenuOpen&&m_pMenuOpen->IsVisible())||(m_pMenuImport&&m_pMenuImport->IsVisible())||
				(m_pMenuClearup&&m_pMenuClearup->IsVisible())||(m_pMenuUpload&&m_pMenuUpload->IsVisible())||
				(m_pMenuUploadNetdisc&&m_pMenuUploadNetdisc->IsVisible())||(m_pMenuDownloadLocal&&m_pMenuDownloadLocal->IsVisible())||(m_pMenuDelete&&m_pMenuDelete->IsVisible()))
			{
				isMenuRenameSplitShow=true;
			}
		}
		m_pMenuRenameSplit->SetVisible(isMenuRenameSplitShow);
	}
	if(m_pMenuEditExercisesSplit)
	{
		bool isMenuEditExercisesSplitShow=false;
		if(m_pMenuEditExercises&&m_pMenuEditExercises->IsVisible())
		{
			if((m_pMenuOpen&&m_pMenuOpen->IsVisible())||(m_pMenuImport&&m_pMenuImport->IsVisible())||
				(m_pMenuClearup&&m_pMenuClearup->IsVisible())||(m_pMenuUpload&&m_pMenuUpload->IsVisible())||
				(m_pMenuUploadNetdisc&&m_pMenuUploadNetdisc->IsVisible())||(m_pMenuDownloadLocal&&m_pMenuDownloadLocal->IsVisible())||
				(m_pMenuDelete&&m_pMenuDelete->IsVisible())||(m_pMenuRename&&m_pMenuRename->IsVisible()))
			{
				isMenuEditExercisesSplitShow=true;
			}
		}
		m_pMenuEditExercisesSplit->SetVisible(isMenuEditExercisesSplitShow);
	}
}

void CMenuUI::RefreshSize()
{
	int width = 140;
	int height=0;
	if(m_pMenuOpen&&m_pMenuOpen->IsVisible())
	{
		height+=m_pMenuOpen->GetFixedHeight();
	}
	if(m_pMenuImportSplit&&m_pMenuImportSplit->IsVisible())
	{
		height+=m_pMenuImportSplit->GetFixedHeight();
	}
	if(m_pMenuImport&&m_pMenuImport->IsVisible())
	{
		height+=m_pMenuImport->GetFixedHeight();
	}
	if(m_pMenuClearupSplit&&m_pMenuClearupSplit->IsVisible())
	{
		height+=m_pMenuClearupSplit->GetFixedHeight();
	}
	if(m_pMenuClearup&&m_pMenuClearup->IsVisible())
	{
		height+=m_pMenuClearup->GetFixedHeight();
	}
	if(m_pMenuUploadSplit&&m_pMenuUploadSplit->IsVisible())
	{
		height+=m_pMenuUploadSplit->GetFixedHeight();
	}
	if(m_pMenuUpload&&m_pMenuUpload->IsVisible())
	{
		height+=m_pMenuUpload->GetFixedHeight();
	}
	if(m_pMenuUploadNetdiscSplit&&m_pMenuUploadNetdiscSplit->IsVisible())
	{
		height+=m_pMenuUploadNetdiscSplit->GetFixedHeight();
	}
	if(m_pMenuUploadNetdisc&&m_pMenuUploadNetdisc->IsVisible())
	{
		height+=m_pMenuUploadNetdisc->GetFixedHeight();
	}
	if(m_pMenuDownloadLocalSplit&&m_pMenuDownloadLocalSplit->IsVisible())
	{
		height+=m_pMenuDownloadLocalSplit->GetFixedHeight();
	}
	if(m_pMenuDownloadLocal&&m_pMenuDownloadLocal->IsVisible())
	{
		height+=m_pMenuDownloadLocal->GetFixedHeight();
	}
	if(m_pMenuDeleteSplit&&m_pMenuDeleteSplit->IsVisible())
	{
		height+=m_pMenuDeleteSplit->GetFixedHeight();
	}
	if(m_pMenuDelete&&m_pMenuDelete->IsVisible())
	{
		height+=m_pMenuDelete->GetFixedHeight();
	}
	if(m_pMenuRenameSplit&&m_pMenuRenameSplit->IsVisible())
	{
		height+=m_pMenuRenameSplit->GetFixedHeight();
	}
	if(m_pMenuRename&&m_pMenuRename->IsVisible())
	{
		height+=m_pMenuRename->GetFixedHeight();
	}
	if(m_pMenuEditExercisesSplit&&m_pMenuEditExercisesSplit->IsVisible())
	{
		height+=m_pMenuEditExercisesSplit->GetFixedHeight();
	}
	if(m_pMenuEditExercises&&m_pMenuEditExercises->IsVisible())
	{
		height+=m_pMenuEditExercises->GetFixedHeight();
	}
	CPoint pt;
	GetCursorPos(&pt);
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);
	int x = pt.x;
	int y = pt.y;
	if(pt.x+width>=cx)
	{
		x = pt.x-width;
	}
	if(pt.y+height>=cy)
	{
		y = pt.y - height;
	}
	::SetWindowPos(*this, NULL, x, y, width,height, SWP_NOZORDER );
}

void CMenuUI::SetType(int nType)
{
	m_nType=nType;
	m_nItemIndex=-1;
}

void CMenuUI::SetItemIndex( int nIndex )
{
	m_nType=-1;;
	m_nItemIndex=nIndex;
}

void CMenuUI::SetIsCloudResource( bool isCloud )
{
	m_bIsCloud = isCloud;
}
