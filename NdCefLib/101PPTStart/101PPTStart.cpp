// 101PPTStart.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include "stdafx.h"
#include <string>
#include "Userenv.h"
#include "NdCefCmdLine.h"
#include <tlhelp32.h>
#pragma comment(lib, "Userenv.lib")

bool ShellExecuteExPPT(std::wstring path)
{
	SHELLEXECUTEINFO sei = {sizeof(SHELLEXECUTEINFO)};
	sei.lpVerb = L"runas";
	sei.lpFile = path.c_str();
	sei.hwnd = NULL;
	sei.nShow = SW_NORMAL;
	if (!ShellExecuteEx(&sei))
	{
	}
	return true;
}
bool ProcessInServicePPT(){
	TCHAR szPath[MAX_PATH];
	if( !GetModuleFileName( NULL, szPath, MAX_PATH ) )
	{
		return false;
	}
	std::wstring nPath(szPath);
	nPath = nPath.substr(0,nPath.find_last_of('\\'));
	nPath+= L"\\..\\..\\101PPT.exe";
	return ShellExecuteExPPT(nPath); 
}

int KillProcessId(DWORD nProcessIdtemp) 
{ 
	PROCESSENTRY32 my; 
	HANDLE l = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	if (((int)l) != -1) {
		my.dwSize = sizeof(my);
		if (Process32First(l, &my)) {
			do 
			{ 
				if (nProcessIdtemp==my.th32ProcessID) { 
					HANDLE hProcessHandle;
					hProcessHandle = OpenProcess(PROCESS_TERMINATE, true, my.th32ProcessID); 
					if (hProcessHandle != NULL)
					{ 
						if(TerminateProcess(hProcessHandle, 0) != NULL)
						{
							CloseHandle(hProcessHandle);
						}

					}
					CloseHandle(l);					
					return 0; 
				}
			}while (Process32Next(l, &my));
		} 
		CloseHandle(l); 
	} 
	return 0;
}
DWORD WINAPI DaemonPPTClose(LPVOID lpParameter)
{
	for(int i=0;i<10;i++){
		HWND hdPrompt = ::FindWindow(_T("#32770"), _T("Microsoft PowerPoint"));
		if (hdPrompt==NULL){
			hdPrompt = ::FindWindow(_T("#32770"), _T("Microsoft Office PowerPoint"));
		}
		if(hdPrompt!=NULL)
		{
			PostMessage(hdPrompt, WM_CLOSE, 0, 0);
			break;
		}
		Sleep(500);
	}
	return 0;
}
DWORD WINAPI DaemonPPTCloseThreadProc(LPVOID lpParameter)
{
	for(int i=0;i<10;i++){
		HWND hdPrompt = ::FindWindow(_T("#32770"), _T("Microsoft PowerPoint"));
		if (hdPrompt==NULL){
			hdPrompt = ::FindWindow(_T("#32770"), _T("Microsoft Office PowerPoint"));
		}
		if(hdPrompt!=NULL)
		{
			if (::IsWindow(hdPrompt))
			{
				DWORD nProcessIdtemp = 0;
				GetWindowThreadProcessId(hdPrompt,&nProcessIdtemp);
				KillProcessId(nProcessIdtemp);
			}

			break;
		}
		Sleep(500);
	}
	return 0;
}
int _tmain(int argc, _TCHAR* argv[])
{
	NdCef::CNdCefCmdLine<wstring> argumentList;
	argumentList.parseCmd(argc,argv);
	if (argumentList.size()==0){
		return false;
	}else {
		std::wstring strType = argumentList[0];
		if( strType == _T("ppt")){
			ProcessInServicePPT();
		}else if ( strType == _T("trayshow")){
			HWND hWnd = ::FindWindow(L"Shell_TrayWnd",NULL); 
			::ShowWindow(hWnd, SW_SHOWNORMAL);
			DaemonPPTClose(NULL);
		}
	}

	return 0;
}

