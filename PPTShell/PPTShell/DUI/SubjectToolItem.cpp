#include "StdAfx.h"
#include "SubjectToolItem.h"
#include "Util/Util.h"
#include "InstrumentView.h"
#include "ScreenInstrument.h"
#include "PPTControl/PPTController.h"


#define ToolExec _T("CoursePlayer.exe")

CSubjectToolItemUI::CSubjectToolItemUI(void)
{
	CScreenInstrumentUI::GetMainInstrument()->GetPagger()->AddPageListener(this);
	SetProcess(NULL);
}

CSubjectToolItemUI::~CSubjectToolItemUI(void)
{
	KillProcess();
}

void CSubjectToolItemUI::OnClick( CContainerUI* pParent )
{
	if (trim(m_strResource).empty())
	{
		return;
	}
	HWND nPPTWnd = CPPTController::GetSlideShowViewHwnd();
	char		szPath[MAX_PATH*2] = {0};
	tstring		strDir = GetLocalPath();
	sprintf_s(szPath, "%s\\bin\\CoursePlayer\\%s OSR \"%s?sys=pptshell\" %d", strDir.c_str(), ToolExec, m_strResource.c_str(),nPPTWnd);
	
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	memset( &si, 0, sizeof(STARTUPINFO) );
	si.cb = sizeof(STARTUPINFO); 
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	KillProcess();
	if (CreateProcess(NULL, (TCHAR*)szPath, NULL, NULL, FALSE, 0, NULL,strDir.c_str(), &si, &pi ) )
	{ 
		SetProcess(pi.hProcess);
		CloseHandle(pi.hThread);
	} 
}

void CSubjectToolItemUI::SetProcess( HANDLE hProcess )
{
	m_hProcess = hProcess;
}

void CSubjectToolItemUI::KillProcess()
{
	if (m_hProcess)
	{
		TerminateProcess(m_hProcess, 0);
		CloseHandle(m_hProcess);
		m_hProcess = NULL;
	}
}

bool CSubjectToolItemUI::HasProcess()
{
	return m_hProcess == NULL ? false : true;
}

void CSubjectToolItemUI::OnPageChangeBefore()
{
	KillProcess();
}

void CSubjectToolItemUI::OnPageChanged( int nPageIndex )
{
	KillProcess();
}

void CSubjectToolItemUI::OnPageScanneded()
{

}
