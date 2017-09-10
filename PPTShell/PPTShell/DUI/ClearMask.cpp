#include "StdAfx.h"
#include "ClearMask.h"

CClearMask::CClearMask(void)
{
}

CClearMask::~CClearMask(void)
{
}

LPCTSTR CClearMask::GetWindowClassName( void ) const
{
	return _T("ClearMask");
}

DuiLib::CDuiString CClearMask::GetSkinFile()
{
	return _T("Options\\ClearMask.xml");
}

DuiLib::CDuiString CClearMask::GetSkinFolder()
{
	return _T("skins");
}

void CClearMask::InitWindow()
{
	pMask = dynamic_cast<CGifAnimUI*>(m_PaintManager.FindControl(_T("gifLoading")));
	pMask->PlayGif();
}

CControlUI* CClearMask::CreateControl(LPCTSTR pstrClass)
{
	if( _tcscmp(pstrClass, _T("GifAnim")) == 0 )
		return new CGifAnimUI;

	return __super::CreateControl(pstrClass);
}

void CClearMask::CloseMask()
{
	pMask->StopGif();
	Close();
}
