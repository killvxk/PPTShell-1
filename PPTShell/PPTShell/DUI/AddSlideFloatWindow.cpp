#include "StdAfx.h"
#include "AddSlideFloatWindow.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"

CAddSlideFloatWindow::CAddSlideFloatWindow(void)
{
}

CAddSlideFloatWindow::~CAddSlideFloatWindow(void)
{
}


LPCTSTR CAddSlideFloatWindow::GetWindowClassName() const
{
	return _T("AddSlide");
}

CDuiString CAddSlideFloatWindow::GetSkinFile()
{
	return _T("AddSlide\\AddSlideFloatWindow.xml");
}

CDuiString CAddSlideFloatWindow::GetSkinFolder()
{
	return CDuiString(_T("skins"));
}


void CAddSlideFloatWindow::InitWindow()
{

}

void CAddSlideFloatWindow::OnAddSlideBtn( TNotifyUI& msg )
{
	AddSlideByThread();
}
