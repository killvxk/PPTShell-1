#include "StdAfx.h"
#include "AddQuestionWindow.h"
#include "CoursePlayUI.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"

CAddQuestionWindow::CAddQuestionWindow(void)
{
}

CAddQuestionWindow::~CAddQuestionWindow(void)
{
}


LPCTSTR CAddQuestionWindow::GetWindowClassName() const
{
	return _T("AddQuestionWindow");
}

CDuiString CAddQuestionWindow::GetSkinFile()
{
	return _T("CoursePlayer\\AddQuestionWindow.xml");
}

CDuiString CAddQuestionWindow::GetSkinFolder()
{
	return CDuiString(_T("skins"));
}


void CAddQuestionWindow::InitWindow()
{

}
LRESULT CAddQuestionWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam){
	switch(uMsg)
	{
	case WM_SHOWWINDOW:
		if ((BOOL) wParam == false){
			Hide();
		}
		break;
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}
void CAddQuestionWindow::OnAddQuestionBtn( TNotifyUI& msg )
{
	InsertQuestionByThread(m_szPath.c_str(),m_szQuestionType.c_str(),m_szGuid.c_str());
	CCoursePlayUI * pCoursePlayUI = CoursePlayUI::GetInstance();
	pCoursePlayUI->Hide();
}
void CAddQuestionWindow::Hide(){
	ShowWindow(false);
} 
BOOL CAddQuestionWindow::SetQuestionInfo( tstring szPath, tstring szQuestionType, tstring szGuid){
	m_szPath = szPath;
	m_szQuestionType = szQuestionType;
	m_szGuid =	szGuid;
	return TRUE;
}