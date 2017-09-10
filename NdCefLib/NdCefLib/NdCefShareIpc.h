#pragma once
#include <windows.h>


#define MEMORY_SIZE 0x20000
#define MEMORY_URL_SIZE 0x1000
#define GLOBAL_MEMORY_NAME TEXT("Global\\NdCefShareMemory")
#define GLOBAL_EVENT_NAME TEXT("Global\\NdCefShareMemoryEvent")
#define GLOBAL_MEMORY_URL_NAME TEXT("Global\\NdCefShareMemoryURL")
#define GLOBAL_EVENT_URL_NAME TEXT("Global\\NdCefShareMemoryEventURL")

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
private:
	HANDLE m_hEvent;
	HANDLE m_hFileMapping;
};
