//===========================================================================
// FileName:				PPTInfoSender.cpp
//	
// Desc:					 
//============================================================================
#include "stdafx.h"
#include "PPTICRPlayControl.h"
#include "GUI/MainFrm.h"
#include "PPTImagesExporter.h"
#include "NDCloud/NDCloudUser.h"

CPPTICRPlayControl::CPPTICRPlayControl()
{

}

CPPTICRPlayControl::~CPPTICRPlayControl()
{

}


bool CPPTICRPlayControl::OnExportImagesCompleted( void* pObj )
{
	ExportNotify* pNotify = (ExportNotify*)pObj;

	if (pNotify->dwErrorCode != 0)
	{
		return false;
	}

	m_strDirPath = pNotify->strDir.c_str();
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)NDICRPlayThread, (LPVOID)this, 0, NULL);

	return true;
}

bool CPPTICRPlayControl::OnExportImagesCompleted(  )
{

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)NDICRPlayThread, (LPVOID)this, 0, NULL);

	return true;
}

DWORD WINAPI CPPTICRPlayControl::NDICRPlayThread( LPARAM lParam )
{
	CPPTICRPlayControl* pControl = (CPPTICRPlayControl*)lParam;

// 	tstring strDir =_T(" thumb=");
// 	strDir += pControl->m_strDirPath;

	tstring strPath = GetLocalPath();
	strPath += _T("\\icrplayer\\ND_ICR_WPF_PLAYER.exe");

	CMainFrame* pMainFrame = (CMainFrame* )AfxGetApp()->m_pMainWnd;

 	tstring strCmd = _T("");
// 	strCmd += strDir;

	

	if(!pMainFrame->m_bPreView)
	{
		tstring strUserName = NDCloudUser::GetInstance()->GetUserName();
		tstring strPassword = NDCloudUser::GetInstance()->GetPassword();
		strCmd += " user=";
		strCmd += strUserName;
		strCmd += "&pwd=";
		strCmd += NDCloudUser::GetInstance()->MD5EncryptString(strPassword);
		strCmd += _T("&inclass=1");
	}

	wstring wstrPath = AnsiToUnicode(strPath);
	wstring wstrCmd = AnsiToUnicode(strCmd);
	wstring wstrDir = AnsiToUnicode(GetLocalPath());
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.wShowWindow=SW_SHOW;
	ZeroMemory(&pi, sizeof(pi));

	tstring strNDICRPlayPath = GetLocalPath();
	strNDICRPlayPath += _T("\\icrplayer\\");
	if (CreateProcess(strPath.c_str(),(LPTSTR)strCmd.c_str(),NULL,NULL,FALSE,0,NULL,strNDICRPlayPath.c_str(),&si,&pi))
	{
		WaitForSingleObject(pi.hProcess, INFINITE);	
	}


	ShadeWindowHide();

	return 0;
}
