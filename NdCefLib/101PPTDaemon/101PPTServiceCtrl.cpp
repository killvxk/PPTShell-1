#include "StdAfx.h"
#include "101PPTServiceCtrl.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "server/server.hpp"
#include "Userenv.h"
#include "PPTDaemonEvent.h"
#include <string>
#pragma comment(lib, "Userenv.lib")

#include "UpdateLib.h"

#pragma comment(lib, "UpdateLib.lib")

SERVICE_STATUS         C101PPTServiceCtrl::MyServiceStatus; 
SERVICE_STATUS_HANDLE    C101PPTServiceCtrl::MyServiceStatusHandle = NULL; 
HANDLE  C101PPTServiceCtrl::hStopEvent = NULL; 
HANDLE  C101PPTServiceCtrl::hStopThreadEvent = NULL; 
bool C101PPTServiceCtrl::g_stop = false;
bool C101PPTServiceCtrl::g_UpdateStop = false;
bool C101PPTServiceCtrl::g_PPTRun = false;
enum PPTEVENT{
	STARTEVENT = 1,
	STOPEVENT
};


enum E_RET
{
	e_newest			//版本为最新
	,	e_download_newest//版本不是最新但是已经下载了最新版本
	,	e_cancel
	,	e_download_success//下载最新版本完成,有管道
	,   e_success//下载最新版本完成，无管道
	,	e_f_dir
	,	e_f_dat
	,	e_f_down
	,	e_f_mov
	,	e_f_size

};
//MakeEventTypeX(STOPEVENT1, int&, int);

//auto conn = eventhub<EV_EVENT1>::on([](int&, int){});

HANDLE FindProcess(std::wstring strExeName) 
{ 
	PROCESSENTRY32 my; 
	bool nFind = false;
	HANDLE l = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	if (((int)l) != -1) {
		my.dwSize = sizeof(my);
		if (Process32First(l, &my)) {
			do 
			{ 
				_wcsupr_s(my.szExeFile);
				if ( _tcsicmp(my.szExeFile, strExeName.c_str()) == 0) { 
					return l;
				}
			}while (Process32Next(l, &my));
		} 
	} 
	return NULL;
}

int KillExeCheckParentPid(std::wstring strExeName, BOOL bCloseAll ) 
{ 
	PROCESSENTRY32 my; 
	HANDLE l = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	if (((int)l) != -1) {
		my.dwSize = sizeof(my);
		if (Process32First(l, &my)) {
			do 
			{ 
				_wcsupr_s(my.szExeFile);
				if (_tcsstr(my.szExeFile, strExeName.c_str()) != NULL
					|| _tcsicmp(my.szExeFile, strExeName.c_str()) == 0) { 
						HANDLE hProcessHandle;
						hProcessHandle = OpenProcess(PROCESS_TERMINATE, true, my.th32ProcessID); 
						if (hProcessHandle != NULL)
						{ 
							if(TerminateProcess(hProcessHandle, 0) != NULL)
							{
								CloseHandle(hProcessHandle);
							}
						}
						if (!bCloseAll)//2015.12.03 cws
						{
							CloseHandle(l);					
							return 0; 
						}
				}
			}while (Process32Next(l, &my));
		} 
		CloseHandle(l); 
	} 
	return 0;
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
DWORD WINAPI DaemonPPTUpdateThreadProc(LPVOID lpParameter)
{
	C101PPTServiceCtrl::g_UpdateStop = true;
	char szPath[MAX_PATH];
	if( !GetModuleFileNameA( NULL, szPath, MAX_PATH ) )
	{
		return false;
	}
	std::string nPath(szPath);
	nPath = nPath.substr(0,nPath.find_last_of('\\'));
	nPath = nPath.substr(0,nPath.find_last_of('\\'));
	nPath = nPath.substr(0,nPath.find_last_of('\\'));
	nPath+= "\\";
	int updateflag = AutoUpdate(NULL,true,nPath.c_str());
	//int updateflag = 1;
	switch(updateflag){
		case e_download_newest:
		case e_download_success:
		case e_success:
			C101PPTServiceCtrl::Log("update");
			if (!C101PPTServiceCtrl::g_PPTRun){
				C101PPTServiceCtrl::Log("Show 101PPTDesktopAlert");
				C101PPTServiceCtrl::ProcessInServiceUpdate(_T(""));
			}
			break;
	}
	C101PPTServiceCtrl::g_UpdateStop = false;
	return 0;
}
DWORD WINAPI DaemonThreadProc(LPVOID lpParameter)
{
	DWORD dwRet = 0;
	DWORD dwRetStop = 0;
	int nIndex = 0;
	HANDLE m_hEvent[2] ={NULL,NULL};  
	HANDLE m_hEventStop[3] ={NULL,NULL,NULL};  
	CPPTDaemonEvent nPPTDaemonEvent;
	
	//创建守护进程的关闭事件
	//
	nPPTDaemonEvent.CreateDaemonEvent(PPTDaemonEvent_Start);
	nPPTDaemonEvent.CreateDaemonEvent(PPTDaemonEvent_Run);
	nPPTDaemonEvent.CreateDaemonEvent(PPTDaemonEvent_Stop);

	m_hEventStop[0] = C101PPTServiceCtrl::hStopThreadEvent;
	m_hEventStop[1] = nPPTDaemonEvent.GetDaemonEvent(PPTDaemonEvent_Run);
	m_hEventStop[2] = nPPTDaemonEvent.GetDaemonEvent(PPTDaemonEvent_Stop);
	m_hEvent[0] = C101PPTServiceCtrl::hStopThreadEvent;
	m_hEvent[1] = nPPTDaemonEvent.GetDaemonEvent(PPTDaemonEvent_Start);

	HWND hWnd = ::FindWindow(L"Shell_TrayWnd",NULL); 
	::ShowWindow(hWnd, SW_SHOWNORMAL);
	
	SvcDebugOut(" [101PPTDaemon] DaemonThreadProc start %d\n", GetLastError()); 
	while(1)
	{
		dwRet = WaitForMultipleObjects(2,m_hEvent,false,60*1000);
		switch(dwRet)
		{
		case WAIT_OBJECT_0:
			if (C101PPTServiceCtrl::g_stop){
				CloseHandle(C101PPTServiceCtrl::hStopThreadEvent);
				SvcDebugOut(" [101PPTDaemon] DaemonThreadProc stop Event  %d\n", GetLastError()); 
				return 1;
			}
			SvcDebugOut(" [101PPTDaemon] DaemonThreadProc WAIT_OBJECT_0  %d\n", GetLastError()); 
			break;
		case WAIT_OBJECT_0+1:
			{
				nPPTDaemonEvent.ReSetDaemonEvent(PPTDaemonEvent_Start);
				bool nEventStop = false;
				C101PPTServiceCtrl::g_PPTRun = true;
				//HANDLE n101PPTHandle = FindProcess(L"101PPT.exe");
				//m_hEventStop[2] = n101PPTHandle;
				SvcDebugOut(" [101PPTDaemon] DaemonThreadProc WAIT_OBJECT_0+1  %d\n", GetLastError()); 
				while(1)
				{
					dwRetStop = WaitForMultipleObjects(3,m_hEventStop,false,3000);
					switch(dwRetStop)
					{
					case WAIT_OBJECT_0:
						if (C101PPTServiceCtrl::g_stop){
							CloseHandle(C101PPTServiceCtrl::hStopThreadEvent);
							SvcDebugOut(" [101PPTDaemon] DaemonThreadProc m_hEventStop stop Event  %d\n", GetLastError()); 
							return 1;
						}
						SvcDebugOut(" [101PPTDaemon] DaemonThreadProc m_hEventStop WAIT_OBJECT_0  %d\n", GetLastError()); 
						break;
					case WAIT_OBJECT_0+1:
						nPPTDaemonEvent.ReSetDaemonEvent(PPTDaemonEvent_Run);
						SvcDebugOut(" [101PPTDaemon] DaemonThreadProc m_hEventStop WAIT_OBJECT_0+1  %d\n", GetLastError()); 
						break;
					case WAIT_OBJECT_0+2:
						nEventStop = true;
						nPPTDaemonEvent.ReSetDaemonEvent(PPTDaemonEvent_Stop);
						SvcDebugOut(" [101PPTDaemon] DaemonThreadProc m_hEventStop WAIT_OBJECT_0+2  %d\n", GetLastError()); 
						break;
					case WAIT_TIMEOUT:
						{
							if (FindProcess(L"101PPT.EXE")==NULL)
							{
								C101PPTServiceCtrl::g_PPTRun = false;
								C101PPTServiceCtrl::Log("KillExeCheckParentPid");
								KillExeCheckParentPid(L"POWERPNT.EXE",TRUE);
								HWND hWnd = ::FindWindow(L"Shell_TrayWnd",NULL); 
								::ShowWindow(hWnd, SW_SHOWNORMAL);
								CreateThread(NULL,0,DaemonPPTCloseThreadProc,NULL,0,NULL); 
								C101PPTServiceCtrl::ProcessInServicePPT(L"trayshow");

								nEventStop = true;
								SvcDebugOut(" [101PPTDaemon] DaemonThreadProc m_hEventStop WAIT_TIMEOUT  %d\n", GetLastError()); 
								if (C101PPTServiceCtrl::g_stop){
									SvcDebugOut(" [101PPTDaemon] DaemonThreadProc m_hEventStop stop timeout  %d\n", GetLastError()); 
									return 1;
								}
							}
						}
						break;
					case WAIT_FAILED:
						nEventStop = true;
						SvcDebugOut(" [101PPTDaemon] DaemonThreadProc m_hEventStop Stop WAIT_FAILED  %d\n", GetLastError()); 
						break;
					}
					if (nEventStop){
						break;
					}
					if (C101PPTServiceCtrl::g_stop){
						return 1;
					}
				}
			}
			break;
		case WAIT_TIMEOUT:
			 SvcDebugOut(" [101PPTDaemon] DaemonThreadProc WAIT_TIMEOUT  %d\n", GetLastError()); 
			 if (C101PPTServiceCtrl::g_stop){
				 SvcDebugOut(" [101PPTDaemon] DaemonThreadProc stop timeout  %d\n", GetLastError()); 
				 return 1;
			 }
			 if (!C101PPTServiceCtrl::g_UpdateStop&& !C101PPTServiceCtrl::g_PPTRun){
				 CreateThread(NULL,0,DaemonPPTUpdateThreadProc,NULL,0,NULL); 
			 }
			 break;
		 case WAIT_FAILED:
			 SvcDebugOut(" [101PPTDaemon] DaemonThreadProc WAIT_FAILED  %d\n", GetLastError()); 
			 return 1;
		}
	}
	return 0;
}
void C101PPTServiceCtrl::MyServiceStart (DWORD argc, LPTSTR *argv) 
{ 
    DWORD status; 
    DWORD specificError; 
 
    MyServiceStatus.dwServiceType        = SERVICE_WIN32; 
    MyServiceStatus.dwCurrentState       = SERVICE_START_PENDING; 
    MyServiceStatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP | 
        SERVICE_ACCEPT_PAUSE_CONTINUE; 
    MyServiceStatus.dwWin32ExitCode      = 0; 
    MyServiceStatus.dwServiceSpecificExitCode = 0; 
    MyServiceStatus.dwCheckPoint         = 0; 
    MyServiceStatus.dwWaitHint           = 0; 

	hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	hStopThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (hStopEvent == NULL)
	{
		SvcDebugOut(" [101PPTDaemon] CreateEvent failed %d\n", GetLastError()); 
        return; 
	}

    MyServiceStatusHandle = RegisterServiceCtrlHandlerEx( 
        L"101PPTDaemon", 
        (LPHANDLER_FUNCTION_EX)MyServiceCtrlHandler,NULL); 
 
    if (MyServiceStatusHandle == (SERVICE_STATUS_HANDLE)0) 
    { 
        SvcDebugOut(" [101PPTDaemon] RegisterServiceCtrlHandler failed %d\n", GetLastError()); 
        return; 
    } 

    // Initialization code goes here. 
    status = MyServiceInitialization(argc,argv, &specificError); 

    // Handle error condition 
    if (status != NO_ERROR) 
    { 
        MyServiceStatus.dwCurrentState       = SERVICE_STOPPED; 
        MyServiceStatus.dwCheckPoint         = 0; 
        MyServiceStatus.dwWaitHint           = 0; 
        MyServiceStatus.dwWin32ExitCode      = status; 
        MyServiceStatus.dwServiceSpecificExitCode = specificError; 
 
        SetServiceStatus (MyServiceStatusHandle, &MyServiceStatus); 
        return; 
    } 
 
    // Initialization complete - report running status. 
    MyServiceStatus.dwCurrentState       = SERVICE_RUNNING; 
    MyServiceStatus.dwCheckPoint         = 0; 
    MyServiceStatus.dwWaitHint           = 0; 
 
    if (!SetServiceStatus (MyServiceStatusHandle, &MyServiceStatus)) 
    { 
        status = GetLastError(); 
        SvcDebugOut(" [101PPTDaemon] SetServiceStatus error %ld\n",status); 
    } 
	 SvcDebugOut(" [101PPTDaemon] http::server::server 30002 \n",0); 

	char szPath[MAX_PATH];
	if( !GetModuleFileNameA( NULL, szPath, MAX_PATH ) )
	{
		return ;
	}
	std::string nPath(szPath);

	g_stop = false;
	CreateThread(NULL,0,DaemonThreadProc,NULL,0,NULL); 

	nPath = nPath.substr(0,nPath.find_last_of('\\'));
	http::server::server s("0.0.0.0","30002", nPath +"\\html");
	// Run the server until stopped.
	s.run();
	
	//ProcessInService(false);
	while(WaitForSingleObject(hStopEvent, 1000) != WAIT_OBJECT_0)
	{
		// Do nothing
	}
	g_stop = true;
    // This is where the service does its work.  
	if (hStopThreadEvent!=NULL){
		if (!SetEvent(hStopThreadEvent))
		{

		}
	}
	if (!CloseHandle(hStopEvent))
		 SvcDebugOut(" [101PPTDaemon] CloseHandle the Main Thread \n",0); 
    return; 
} 

// Stub initialization function. 
DWORD C101PPTServiceCtrl::MyServiceInitialization(DWORD   argc, LPTSTR  *argv, 
    DWORD *specificError) 
{ 
    argv; 
    argc; 
    specificError; 
    return(0); 
}
void C101PPTServiceCtrl::MyServiceCtrlHandler (DWORD dwCtrlCode,  DWORD dwEventType,  LPVOID lpEventData,  LPVOID lpContext) 
{ 
   DWORD status; 
 
   switch(dwCtrlCode) 
   { 
      case SERVICE_CONTROL_PAUSE: 
      // Do whatever it takes to pause here. 
         MyServiceStatus.dwCurrentState = SERVICE_PAUSED; 
         break; 
 
      case SERVICE_CONTROL_CONTINUE: 
      // Do whatever it takes to continue here. 
         MyServiceStatus.dwCurrentState = SERVICE_RUNNING; 
         break; 
 
      case SERVICE_CONTROL_STOP: 
      // Do whatever it takes to stop here. 
         MyServiceStatus.dwWin32ExitCode = 0; 
         MyServiceStatus.dwCurrentState  = SERVICE_STOPPED; 
         MyServiceStatus.dwCheckPoint    = 0; 
         MyServiceStatus.dwWaitHint      = 0; 

         if (!SetServiceStatus (MyServiceStatusHandle, 
           &MyServiceStatus))
         { 
            status = GetLastError(); 
            SvcDebugOut(" [101PPTDaemon] SetServiceStatus error %ld\n", 
               status); 
         } 
         SvcDebugOut(" [101PPTDaemon] Leaving MyService \n",0); 
         return; 
      case SERVICE_CONTROL_INTERROGATE: 
      // Fall through to send current status. 
         break; 
 
      default: 
         SvcDebugOut(" [101PPTDaemon] Unrecognized opcode %ld\n", dwCtrlCode); 
   } 
 
   // Send current status. 
   if (!SetServiceStatus (MyServiceStatusHandle,  &MyServiceStatus)) 
   { 
      status = GetLastError(); 
      SvcDebugOut(" [101PPTDaemon] SetServiceStatus error %ld\n", status); 
   } 

   if ((dwCtrlCode == SERVICE_CONTROL_STOP) || (dwCtrlCode == SERVICE_CONTROL_SHUTDOWN))
	{
		if (!SetEvent(hStopEvent))
			SvcDebugOut("SetEvent",0);
		else
			SvcDebugOut("Signal ServiceMain thread\n",0);
	}
   return; 
}
void C101PPTServiceCtrl::ServiceCtrlDispatcher()
{
	SERVICE_TABLE_ENTRY   DispatchTable[] = 
	{ 
		{ TEXT(""), (LPSERVICE_MAIN_FUNCTIONW)MyServiceStart}, 
		{ NULL,              NULL } 
	};
	SvcDebugOut(" [101PPTDaemon] StartServiceCtrlDispatcher\n", 0); 
	if (!StartServiceCtrlDispatcher( DispatchTable)) 
	{ 
		SvcDebugOut(" [101PPTDaemon] StartServiceCtrlDispatcher (%d)\n", GetLastError()); 
	}
}
bool C101PPTServiceCtrl::Log(LPSTR nLog){
	TCHAR szPath[MAX_PATH];
	if( !GetModuleFileName( NULL, szPath, MAX_PATH ) )
	{
		return false;
	}
	std::wstring nPath(szPath);
	nPath = nPath.substr(0,nPath.find_last_of('\\'));
	nPath+= L"\\Deamon.log";
	SvcDebugOutFile(nPath,nLog,0);
	return ProcessInService(nPath);
}
bool C101PPTServiceCtrl::ProcessInServicePPT(std::wstring nParam){
	TCHAR szPath[MAX_PATH];
	if( !GetModuleFileName( NULL, szPath, MAX_PATH ) )
	{
		return false;
	}
	std::wstring nPath(szPath);
	nPath = nPath.substr(0,nPath.find_last_of('\\'));
	nPath+= L"\\101PPTStart.exe "+nParam;
	return ProcessInService(nPath);
}
bool C101PPTServiceCtrl::ProcessInServiceUpdate(std::wstring nParam){
	TCHAR szPath[MAX_PATH];
	if( !GetModuleFileName( NULL, szPath, MAX_PATH ) )
	{
		return false;
	}
	std::wstring nPath(szPath);
	nPath = nPath.substr(0,nPath.find_last_of('\\'));
	nPath+= L"\\101PPTDesktopAlert.exe ";
	return ProcessInService(nPath);
}
bool C101PPTServiceCtrl::ProcessInServiceAdmin(std::wstring szImage)
{
	HANDLE hToken = NULL;
	HANDLE hTokenDup = NULL;
	bool bSuccess = false;
	do 
	{
		if(OpenProcessToken(GetCurrentProcess(),TOKEN_ALL_ACCESS,&hToken))
		{
			if(DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS,NULL, SecurityIdentification, TokenPrimary, &hTokenDup))
			{
				DWORD dwSessionId = WTSGetActiveConsoleSessionId();
				if(!SetTokenInformation(hTokenDup,TokenSessionId,&dwSessionId,sizeof(DWORD)))
				{
					SvcDebugOut("SetTokenInformation error ！error code：%d\n",GetLastError());
					bSuccess = FALSE;
					break;
				}

				STARTUPINFO si;
				PROCESS_INFORMATION pi;
				ZeroMemory(&si,sizeof(STARTUPINFO));
				ZeroMemory(&pi,sizeof(PROCESS_INFORMATION));
				si.cb = sizeof(STARTUPINFO);
				si.lpDesktop = _T("WinSta0\\Default");
				si.wShowWindow = SW_SHOW;
				si.dwFlags     =   STARTF_USESHOWWINDOW /*|STARTF_USESTDHANDLES*/;

				LPVOID pEnv = NULL;
				DWORD dwCreationFlag = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT;
				if(!CreateEnvironmentBlock(&pEnv,hTokenDup,FALSE))
				{
					SvcDebugOut("CreateEnvironmentBlock error ！error code：%d\n",GetLastError());
					bSuccess = FALSE;
					break;
				}

				if(!CreateProcessAsUser(hTokenDup,NULL,(LPWSTR)szImage.c_str(),NULL,NULL,FALSE,dwCreationFlag,pEnv,NULL,&si,&pi))
				{
					SvcDebugOut("CreateProcessAsUser error ！error code：%d\n",GetLastError());
					bSuccess = FALSE;
					break;
				}

				if(pEnv)
				{
					DestroyEnvironmentBlock(pEnv);
				}
			}
			else
			{
				SvcDebugOut("DuplicateTokenEx error ！error code：%d\n",GetLastError());
				bSuccess = FALSE;
				break;
			}


		}
		else
		{
			SvcDebugOut("cannot get administror！error code：%d\n",GetLastError());
			bSuccess = FALSE;
			break;
		}
	}while(0);

	if(hTokenDup != NULL && hTokenDup != INVALID_HANDLE_VALUE)
		CloseHandle(hTokenDup);
	if(hToken != NULL && hToken != INVALID_HANDLE_VALUE)
		CloseHandle(hToken);
	return true;
}

bool C101PPTServiceCtrl::ProcessInService(std::wstring szImage)
{
	HANDLE hToken; 

	bool nCloseProcessSnap = false;
	HANDLE hProcessSnap = NULL; 
	BOOL bRet = FALSE; 
	PROCESSENTRY32 pe32 = {0}; 
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE) 
	{
		return false;
	}
	pe32.dwSize = sizeof(PROCESSENTRY32); 
	if (Process32First(hProcessSnap, &pe32)) 
	{ 
		do 
		{
			CString szExeFile=pe32.szExeFile;
			if(szExeFile.CompareNoCase(TEXT("EXPLORER.EXE")) == 0)
			{ 
				HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE,pe32.th32ProcessID); 
				bRet = OpenProcessToken(hProcess,TOKEN_ALL_ACCESS,&hToken);
				CloseHandle (hProcessSnap); 
				bRet=true; 
				nCloseProcessSnap = true;
				break;
			} 
		} while (Process32Next(hProcessSnap, &pe32));
		bRet = TRUE;
	} 
	else 
	{
		bRet = FALSE;
	}
	if (!nCloseProcessSnap) CloseHandle (hProcessSnap); 

	STARTUPINFO si; 
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFO)); 
	si.cb= sizeof(STARTUPINFO); 
	si.dwFlags = STARTF_USESHOWWINDOW;	
	si.wShowWindow = SW_HIDE;
	si.lpDesktop = TEXT("winsta0\\default"); 
	
	//si.wShowWindow = SW_SHOWNORMAL;


	BOOL bResult = CreateProcessAsUser(hToken,NULL,(LPWSTR)szImage.c_str(),NULL,NULL, FALSE,NORMAL_PRIORITY_CLASS,NULL,NULL,&si,&pi);

	if (bResult && pi.hProcess != INVALID_HANDLE_VALUE) 
	{ 
		WaitForSingleObject(pi.hProcess, 3000); 
		CloseHandle(pi.hProcess); 
	} 
	if (pi.hThread != INVALID_HANDLE_VALUE)
		CloseHandle(pi.hThread);  
	return true;
}