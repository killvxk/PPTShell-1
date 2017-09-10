#include "stdafx.h"
#include "UpdateTipDialog.h"
#include "EventCenter/EventDefine.h"


CUpdateTipDialogUI::CUpdateTipDialogUI()
{

}

CUpdateTipDialogUI::~CUpdateTipDialogUI()
{

}

LPCTSTR CUpdateTipDialogUI::GetWindowClassName() const
{
	return _T("UpdateTipDialog");
}

CDuiString CUpdateTipDialogUI::GetSkinFile()
{
	return _T("TopBar\\updatetip.xml");
}

CDuiString CUpdateTipDialogUI::GetSkinFolder()
{
	return CDuiString(_T("skins"));
}


void CUpdateTipDialogUI::InitWindow()
{
	
}

CControlUI* CUpdateTipDialogUI::CreateControl(LPCTSTR pstrClass)
{
	return NULL;
}

void CUpdateTipDialogUI::OnLaterwBtn( TNotifyUI& msg )
{
	ShowWindow(false);
}

void CUpdateTipDialogUI::OnStartNewAppBtn( TNotifyUI& msg )
{
	ShowWindow(false);
	BroadcastEvent(EVT_ENV_RELEASE, 0, 0, NULL);
}
