#include "stdafx.h"
#include "PackPPTTip.h"

CPackPPTTipUI::CPackPPTTipUI()
{
}


CPackPPTTipUI::~CPackPPTTipUI()
{

}

LPCTSTR CPackPPTTipUI::GetWindowClassName() const
{
	return _T("PackPPTTip");
}

CDuiString CPackPPTTipUI::GetSkinFile()
{
	return _T("TopBar\\PackPPTTipUI.xml");
}

CDuiString CPackPPTTipUI::GetSkinFolder()
{
	return CDuiString(_T("skins"));
}

void CPackPPTTipUI::InitWindow()
{
	
}
