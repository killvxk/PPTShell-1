#include "stdafx.h"
#include "PPTDaemonEvent.h"
#include "Util/Util.h"

CPPTDaemonEvent::CPPTDaemonEvent()
{
	m_hEventStop = NULL;
	m_hEventRun = NULL;
	m_hEventStart = NULL;
	m_hStopThreadEvent = NULL; 
}

CPPTDaemonEvent::~CPPTDaemonEvent()
{
	if (NULL != m_hEventStop)
	{
		CloseHandle(m_hEventStop);
		m_hEventStop = NULL;
	}
	if (NULL != m_hEventStart)
	{
		CloseHandle(m_hEventStart);
		m_hEventStart = NULL;
	}
	if (NULL != m_hEventRun)
	{
		CloseHandle(m_hEventRun);
		m_hEventRun = NULL;
	}
	if (NULL != m_hStopThreadEvent)
	{
		CloseHandle(m_hStopThreadEvent);
		m_hStopThreadEvent = NULL;
	}
}

bool CPPTDaemonEvent::OpenDaemonEvent(PPTDaemonEvent_Type nType){
	if (nType==PPTDaemonEvent_Start){
		m_hEventStart = OpenEvent(EVENT_MODIFY_STATE,TRUE,GLOBAL_EVENT_DAEMON_START);
		if (NULL == m_hEventStart)
		{
			return false;
		}
	}else if (nType==PPTDaemonEvent_Run){
		m_hEventRun = OpenEvent(EVENT_ALL_ACCESS,FALSE,GLOBAL_EVENT_DAEMON_RUN);
		if (NULL == m_hEventRun)
		{
			return false;
		}
	}else if (nType==PPTDaemonEvent_Stop){
		m_hEventStop = OpenEvent(EVENT_ALL_ACCESS,FALSE,GLOBAL_EVENT_DAEMON_STOP);
		if (NULL == m_hEventStop)
		{
			return false;
		}
	}
	return true;
}
bool CPPTDaemonEvent::CreateDaemonEvent(PPTDaemonEvent_Type nType){
	SECURITY_DESCRIPTOR secutityDese;
	::InitializeSecurityDescriptor(&secutityDese, SECURITY_DESCRIPTOR_REVISION);
	::SetSecurityDescriptorDacl(&secutityDese,TRUE,NULL,FALSE);
	SECURITY_ATTRIBUTES securityAttr;
	// set SECURITY_ATTRIBUTES
	securityAttr.nLength = sizeof SECURITY_ATTRIBUTES;
	securityAttr.bInheritHandle = FALSE;
	securityAttr.lpSecurityDescriptor = &secutityDese;
	if (nType==PPTDaemonEvent_Start){
		m_hEventStart = ::CreateEvent(&securityAttr, TRUE, FALSE, GLOBAL_EVENT_DAEMON_START);
		if (NULL == m_hEventStart)
		{
			return false;
		}
	}else if (nType==PPTDaemonEvent_Run){
		m_hEventRun = CreateEvent(&securityAttr, TRUE, FALSE, GLOBAL_EVENT_DAEMON_RUN);
		if (NULL == m_hEventRun)
		{
			return false;
		}
	}else if (nType==PPTDaemonEvent_Stop){
		m_hEventStop = CreateEvent(&securityAttr, TRUE, FALSE, GLOBAL_EVENT_DAEMON_STOP);
		if (NULL == m_hEventStop)
		{
			return false;
		}
	}
	return true;
}
HANDLE CPPTDaemonEvent::GetDaemonEvent(PPTDaemonEvent_Type nType){
	if (nType==PPTDaemonEvent_Start){
		return m_hEventStart;
	}else if (nType==PPTDaemonEvent_Stop){
		return m_hEventStop;
	}else if (nType==PPTDaemonEvent_Run){
		return m_hEventRun;
	}else if (nType==PPTDaemonEvent_StopThread){
		return m_hStopThreadEvent;
	}
	return NULL;
}
void CPPTDaemonEvent::SetDaemonEvent(PPTDaemonEvent_Type nType){
	if (nType==PPTDaemonEvent_Start){
		if (m_hEventStart) SetEvent(m_hEventStart);
	}else if (nType==PPTDaemonEvent_Stop){
		if (m_hEventStop) SetEvent(m_hEventStop);
	}else if (nType==PPTDaemonEvent_Run){
		if (m_hEventRun) SetEvent(m_hEventRun);
	}else if (nType==PPTDaemonEvent_StopThread){
		if (m_hStopThreadEvent) SetEvent(m_hStopThreadEvent);
	}
}
void CPPTDaemonEvent::ReSetDaemonEvent(PPTDaemonEvent_Type nType){
	if (nType==PPTDaemonEvent_Start){
		if (m_hEventStart)  ResetEvent(m_hEventStart);
	}else if (nType==PPTDaemonEvent_Stop){
		if (m_hEventStop)  ResetEvent(m_hEventStop);
	}else if (nType==PPTDaemonEvent_Run){
		if (m_hEventRun)  ResetEvent(m_hEventRun);
	}else if (nType==PPTDaemonEvent_StopThread){
		if (m_hStopThreadEvent)  ResetEvent(m_hStopThreadEvent);
	}
}

DWORD WINAPI CPPTDaemonEvent::PPTDaemonThreadProc(LPVOID lpParameter)
{
	CPPTDaemonEvent* nPPTDaemonEvent = (CPPTDaemonEvent*)(lpParameter);
	if (nPPTDaemonEvent){
		while (true)
		{
			Sleep(1);
			DWORD dwRet = WaitForSingleObject(nPPTDaemonEvent->GetDaemonEvent(PPTDaemonEvent_StopThread), 2000);
			switch(dwRet)
			{
			case WAIT_FAILED:
			case WAIT_OBJECT_0:
				nPPTDaemonEvent->SetDaemonEvent(PPTDaemonEvent_Stop);
				return 1;
				break;
			case WAIT_TIMEOUT:
				nPPTDaemonEvent->SetDaemonEvent(PPTDaemonEvent_Run);
				break;
			}
		}
	}
	return 0;
}
void CPPTDaemonEvent::SetExit(){
	SetDaemonEvent(PPTDaemonEvent_StopThread);
}
void CPPTDaemonEvent::CreateThreadSetEvent(){
	m_hStopThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	CreateThread(NULL,0,PPTDaemonThreadProc,this,0,NULL); 
}
void CPPTDaemonEvent::PPTRun(){
	OpenDaemonEvent(PPTDaemonEvent_Start);
	if (m_hEventStart==NULL){
		CPPTDeamonStart();
		OpenDaemonEvent(PPTDaemonEvent_Start);
	}
	SetDaemonEvent(PPTDaemonEvent_Start);
	OpenDaemonEvent(PPTDaemonEvent_Run);
	SetDaemonEvent(PPTDaemonEvent_Run);
	OpenDaemonEvent(PPTDaemonEvent_Stop);
	if (m_hEventStop){
		CreateThreadSetEvent();
	}
}
void CPPTDaemonEvent::CPPTDeamonStart(){
	PROCESS_INFORMATION pi;
	STARTUPINFO si;      //Òþ²Ø½ø³Ì´°¿Ú
	si.cb = sizeof(STARTUPINFO);
	si.lpReserved = NULL;
	si.lpDesktop = NULL;
	si.lpTitle = NULL;
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	si.cbReserved2 = NULL;
	si.lpReserved2 = NULL;
	tstring strSrc = GetCoursePlayerPath();
	tstring strRegName = strSrc+_TEXT("\\101PPTDaemon.exe -i");
	BOOL ret = CreateProcess(NULL,(LPTSTR)strRegName.c_str(),NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
	if(ret)
	{

	}
}
