#pragma once

class C101PPTServiceCtrl
{
public:
    static SERVICE_STATUS          MyServiceStatus; 
    static SERVICE_STATUS_HANDLE   MyServiceStatusHandle; 
    static HANDLE hStopEvent;
    static HANDLE hStopThreadEvent;
    static HANDLE  hStopPPTThreadEvent;
    static  HANDLE hDevNotifyModem;
	static bool g_stop;
	static bool g_UpdateStop;
	static bool g_PPTRun;
public:
	static void ServiceCtrlDispatcher();
    static void MyServiceCtrlHandler (DWORD dwCtrlCode,  DWORD dwEventType,  LPVOID lpEventData,  LPVOID lpContext);
    static void MyServiceStart (DWORD argc, LPTSTR *argv); 
    static DWORD MyServiceInitialization (DWORD argc, LPTSTR *argv, DWORD *specificError); 
	static bool ProcessInService(std::wstring szImage);
	static bool ProcessInServicePPT(std::wstring nParam);
	static bool ProcessInServiceUpdate(std::wstring nParam);
	static bool ProcessInServiceAdmin(std::wstring szImage);
	static bool Log(LPSTR nLog);
};
