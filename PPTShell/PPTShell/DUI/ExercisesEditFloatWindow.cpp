#include "StdAfx.h"
#include "ExercisesEditFloatWindow.h"
#include "Http/HttpDelegate.h"
#include "NDCloud/NDCloudLocalQuestion.h"

CExercisesEditFloatWindow::CExercisesEditFloatWindow(void)
{
	m_strExercisesPath = _T("");
	m_strExercisesGuid = _T("");
	m_strExercisesType = _T("");
}

CExercisesEditFloatWindow::~CExercisesEditFloatWindow(void)
{
}

LPCTSTR CExercisesEditFloatWindow::GetWindowClassName() const
{
	return _T("ExercisesEdit");
}

CDuiString CExercisesEditFloatWindow::GetSkinFile()
{
	return _T("ExercisesEdit\\ExercisesEditFloatWindow.xml");
}

CDuiString CExercisesEditFloatWindow::GetSkinFolder()
{
	return CDuiString(_T("skins"));
}


void CExercisesEditFloatWindow::InitWindow()
{

}

bool CExercisesEditFloatWindow::GetExercisesInfo( tstring strPath )
{
	int nPos = strPath.rfind(_T("\\"));
	if ( nPos != tstring::npos )
	{
		m_strExercisesGuid	= _T("");
		m_strExercisesType	= _T("");

		m_strExercisesPath	= strPath.substr(0, nPos);
		tstring strJsonPath = m_strExercisesPath;
		strJsonPath			+= _T("\\metadata.json");
		NDCloudLocalQuestionManager::GetInstance()->GetQuestionInfo(strJsonPath, m_strExercisesGuid, m_strExercisesType);
	}

	return true;
}

void CExercisesEditFloatWindow::OnExerEditBtn( TNotifyUI& msg )
{
	
	if ( m_strExercisesType.empty() ) 
	{
		CToast::Toast(_STR_QUESTION_TYPE_EMPTY, true);
		//NDCloudLocalQuestionManager::GetInstance()->EditQuestion(m_strExercisesPath);
	}
	else
	{
		NDCloudLocalQuestionManager::GetInstance()->EditQuestion(m_strExercisesGuid, m_strExercisesType, m_strExercisesPath);
	}
}
