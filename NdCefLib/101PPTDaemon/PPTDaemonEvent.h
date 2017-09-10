#pragma once
#include <windows.h>

#define GLOBAL_EVENT_DAEMON_STOP TEXT("Global\\PPTDAEMONSTOP")
#define GLOBAL_EVENT_DAEMON_RUN TEXT("Global\\PPTDAEMONRUN")
#define GLOBAL_EVENT_DAEMON_START TEXT("Global\\PPTDAEMONSTART")

enum PPTDaemonEvent_Type{
	PPTDaemonEvent_Start = 1,
	PPTDaemonEvent_Run,
	PPTDaemonEvent_Stop,
	PPTDaemonEvent_StopThread,
};

class CPPTDaemonEvent
{
public:
	CPPTDaemonEvent();
	~CPPTDaemonEvent();

public:
	bool OpenDaemonEvent(PPTDaemonEvent_Type nType);
	bool CreateDaemonEvent(PPTDaemonEvent_Type nType);
	HANDLE GetDaemonEvent(PPTDaemonEvent_Type nType);
	void SetDaemonEvent(PPTDaemonEvent_Type nType);
	void ReSetDaemonEvent(PPTDaemonEvent_Type nType);
	void SetExit();
	void CreateThreadSetEvent();
	void PPTRun();
private:
	HANDLE m_hEventStop;
	HANDLE m_hEventRun;
	HANDLE m_hEventStart;
	HANDLE m_hStopThreadEvent; 
};
