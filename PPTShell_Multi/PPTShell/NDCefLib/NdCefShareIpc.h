#pragma once
#include <windows.h>
#include "Util/Singleton.h"
#include <queue>

#define MEMORY_SIZE 0x20000
#define MEMORY_URL_SIZE 0x1000
#define GLOBAL_MEMORY_NAME TEXT("Global\\NdCefShareMemory")
#define GLOBAL_EVENT_NAME TEXT("Global\\NdCefShareMemoryEvent")
#define GLOBAL_MEMORY_URL_NAME TEXT("Global\\NdCefShareMemoryURL")
#define GLOBAL_EVENT_URL_NAME TEXT("Global\\NdCefShareMemoryEventURL")

class CICRInfo{
public:
	int GetICRType(){
		return m_nICRType;
	}
	tstring GetICRInfo(){
		return m_nICRInfo;
	}
	void SetICRType(int nICRType){
		 m_nICRType = nICRType;
	}
	 void SetICRInfo(tstring nICRInfo){
		 m_nICRInfo = nICRInfo;
	}
private:
	int m_nICRType;
	tstring m_nICRInfo;
};
class CNdCefShareMemory
{
public:
	CNdCefShareMemory();
	~CNdCefShareMemory();

public:
	bool OpenFileMemoryMapping(LPCTSTR memoryName,LPCTSTR evnetName);
	bool CreateFileMemoryMapping(LPCTSTR memoryName,LPCTSTR evnetName);
    bool WriteMemory(void* pData, DWORD dwSize,DWORD memorySize);
	bool ReadMemory(void* pOutBuffer, DWORD& dwBufferSize,DWORD memorySize);  

	void SetExit();
	void SetIcrEvernt(int nICRType,tstring nICRInfo);
	static DWORD WINAPI PPTMemoryThreadProc(LPVOID lpParameter);
	void CreateThreadSetEvent();
	DECLARE_SINGLETON_CLASS(CNdCefShareMemory);
private:
	HANDLE m_hEvent;
	HANDLE m_hFileMapping;
	HANDLE m_hEventStart;
	HANDLE m_hEventStop;
	queue<CICRInfo> m_qICRInfo;
};
typedef Singleton<CNdCefShareMemory> NdCefShareMemory;