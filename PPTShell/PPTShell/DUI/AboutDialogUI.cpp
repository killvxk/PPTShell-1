#include "StdAfx.h"
#include "AboutDialogUI.h"
#include "Config.h"
#include "AutoBuildStr.h"

CAboutDialogUI::CAboutDialogUI(void)
{
}

CAboutDialogUI::~CAboutDialogUI(void)
{
}

LPCTSTR CAboutDialogUI::GetWindowClassName( void ) const
{
	return _T("AboutDialog");
}

DuiLib::CDuiString CAboutDialogUI::GetSkinFile()
{
	return _T("About\\About.xml");
}

DuiLib::CDuiString CAboutDialogUI::GetSkinFolder()
{
	return _T("skins");
}

void CAboutDialogUI::OnBtnCloseClick(TNotifyUI& msg)
{
	Close();
}

void CAboutDialogUI::OnFinalMessage( HWND hWnd )
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

void CAboutDialogUI::InitWindow()
{
	//CConfig config;
	CLabelUI* lblVersion = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lblVersion")));
	tstring strVer = _T("°æ±¾ºÅ: ") + g_Config::GetInstance()->GetVersion();
	lblVersion->SetText(strVer.c_str());
	CLabelUI* lblBuild = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lblBuild")));
#ifdef DEVELOP_VERSION
	lblBuild->SetText(_STR_BUILD_TIME);
#else
	lblBuild->SetText(_STR_FINAL_TIME);
#endif
	
}
