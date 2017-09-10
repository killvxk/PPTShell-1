#include "stdafx.h"
#include "PPTVRDispEvent.h"
#include "Util/Util.h"

CPPTVRDispEvent::CPPTVRDispEvent()
{
	m_hEventExit = NULL;
	m_hEventStop = NULL;
	m_hEventStart = NULL;
	m_hStopThreadEvent = NULL; 
}

CPPTVRDispEvent::~CPPTVRDispEvent()
{
	if (NULL != m_hEventExit)
	{
		CloseHandle(m_hEventExit);
		m_hEventExit = NULL;
	}
	if (NULL != m_hEventStart)
	{
		CloseHandle(m_hEventStart);
		m_hEventStart = NULL;
	}
	if (NULL != m_hEventStop)
	{
		CloseHandle(m_hEventStop);
		m_hEventStop = NULL;
	}
	if (NULL != m_hStopThreadEvent)
	{
		CloseHandle(m_hStopThreadEvent);
		m_hStopThreadEvent = NULL;
	}
}

bool CPPTVRDispEvent::OpenDispEvent(PPTVRDispEvent_Type nType){
	if (nType==EVENT_VRDISP_EXIT){
		m_hEventExit = OpenEvent(EVENT_ALL_ACCESS,FALSE,GLOBAL_EVENT_DISP_EXIT);
		if (NULL == m_hEventExit)
		{
			return false;
		}
	}else if (nType==EVENT_VRDISP_START){
		m_hEventStart = OpenEvent(EVENT_ALL_ACCESS,FALSE,GLOBAL_EVENT_DISP_START);
		if (NULL == m_hEventStart)
		{
			return false;
		}
	}else if (nType==EVENT_VRDISP_STOP){
		m_hEventStop = OpenEvent(EVENT_ALL_ACCESS,FALSE,GLOBAL_EVENT_DISP_STOP);
		if (NULL == m_hEventStop)
		{
			return false;
		}
	}
	return true;
}
bool CPPTVRDispEvent::CreateDispEvent(PPTVRDispEvent_Type nType){
	SECURITY_DESCRIPTOR secutityDese;
	::InitializeSecurityDescriptor(&secutityDese, SECURITY_DESCRIPTOR_REVISION);
	::SetSecurityDescriptorDacl(&secutityDese,TRUE,NULL,FALSE);
	SECURITY_ATTRIBUTES securityAttr;
	// set SECURITY_ATTRIBUTES
	securityAttr.nLength = sizeof SECURITY_ATTRIBUTES;
	securityAttr.bInheritHandle = FALSE;
	securityAttr.lpSecurityDescriptor = &secutityDese;
	if (nType==EVENT_VRDISP_EXIT){
		m_hEventExit = CreateEvent(&securityAttr, TRUE, FALSE, GLOBAL_EVENT_DISP_EXIT);
		if (NULL == m_hEventExit)
		{
			return false;
		}
	}else if (nType==EVENT_VRDISP_START){
		m_hEventStart = ::CreateEvent(&securityAttr, TRUE, FALSE, GLOBAL_EVENT_DISP_START);
		if (NULL == m_hEventStart)
		{
			return false;
		}
	}else if (nType==EVENT_VRDISP_STOP){
		m_hEventStop = CreateEvent(&securityAttr, TRUE, FALSE, GLOBAL_EVENT_DISP_STOP);
		if (NULL == m_hEventStop)
		{
			return false;
		}
	}
	return true;
}
HANDLE CPPTVRDispEvent::GetDispEvent(PPTVRDispEvent_Type nType){
	if (nType==EVENT_VRDISP_EXIT){
		return m_hEventExit;
	}else if (nType==EVENT_VRDISP_START){
		return m_hEventStart;
	}else if (nType==EVENT_VRDISP_STOP){
		return m_hEventStop;
	}else if (nType==EVENT_101PPT_EXIT){
		return m_hStopThreadEvent;
	}
	return NULL;
}
void CPPTVRDispEvent::SetDispEvent(PPTVRDispEvent_Type nType){
	if (nType==EVENT_VRDISP_EXIT){
		if (m_hEventExit) SetEvent(m_hEventExit);
	}else if (nType==EVENT_VRDISP_START){
		if (m_hEventStart) SetEvent(m_hEventStart);
	}else if (nType==EVENT_VRDISP_STOP){
		if (m_hEventStop) SetEvent(m_hEventStop);
	}else if (nType==EVENT_101PPT_EXIT){
		if (m_hStopThreadEvent) SetEvent(m_hStopThreadEvent);
	}
}
void CPPTVRDispEvent::ReSetDispEvent(PPTVRDispEvent_Type nType){
	if (nType==EVENT_VRDISP_EXIT){
		if (m_hEventExit)  ResetEvent(m_hEventExit);
	}else if (nType==EVENT_VRDISP_START){
		if (m_hEventStart)  ResetEvent(m_hEventStart);
	}else if (nType==EVENT_VRDISP_STOP){
		if (m_hEventStop)  ResetEvent(m_hEventStop);
	}else if (nType==EVENT_101PPT_EXIT){
		if (m_hStopThreadEvent)  ResetEvent(m_hStopThreadEvent);
	}
}
DWORD WINAPI CPPTVRDispEvent::PPTVRDispThreadProc(LPVOID lpParameter)
{
	HANDLE hEvent[3] = {NULL,NULL,NULL};  
	CPPTVRDispEvent* nPPTVRDispEvent = (CPPTVRDispEvent*)(lpParameter);
	hEvent[0] = nPPTVRDispEvent->GetDispEvent(EVENT_VRDISP_START);
	hEvent[1] = nPPTVRDispEvent->GetDispEvent(EVENT_VRDISP_STOP);
	hEvent[2] = nPPTVRDispEvent->GetDispEvent(EVENT_101PPT_EXIT);


	if (nPPTVRDispEvent){
		while (true)
		{
			Sleep(1);
			DWORD dwRet = WaitForMultipleObjects(3, hEvent, FALSE, INFINITE);
			WRITE_LOG_LOCAL(_T("PPTVRDispEvent: %d\r\n"), dwRet);
			switch(dwRet)
			{
			case WAIT_FAILED:
			case WAIT_OBJECT_0:
				nPPTVRDispEvent->ReSetDispEvent(EVENT_VRDISP_START);
				break;
			case WAIT_OBJECT_0+1:
				nPPTVRDispEvent->ReSetDispEvent(EVENT_VRDISP_STOP);
				break;
			case WAIT_OBJECT_0+2:
				nPPTVRDispEvent->SetDispEvent(EVENT_VRDISP_EXIT);
				return 1;
				break;
			case WAIT_TIMEOUT:
				break;
			}
		}
	}
	return 0;
}
void CPPTVRDispEvent::SetExit(){
	SetDispEvent(EVENT_101PPT_EXIT);
}
void CPPTVRDispEvent::CreateThreadSetEvent(){
	m_hStopThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	CreateThread(NULL,0,PPTVRDispThreadProc,this,0,NULL); 
}
void CPPTVRDispEvent::PPTRun(){
	CreateDispEvent(EVENT_VRDISP_START);
	CreateDispEvent(EVENT_VRDISP_STOP);
	CreateDispEvent(EVENT_VRDISP_EXIT);
	CreateThreadSetEvent();
}