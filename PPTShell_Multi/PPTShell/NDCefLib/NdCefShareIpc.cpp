#include "stdafx.h"
#include "NdCefShareIpc.h"
#include "Plugins/Icr/IcrPlayer.h"

CNdCefShareMemory::CNdCefShareMemory()
{
	m_hEvent = NULL;
	m_hFileMapping = NULL;
}

CNdCefShareMemory::~CNdCefShareMemory()
{
	if (NULL != m_hFileMapping)
	{
		CloseHandle(m_hFileMapping);
		m_hFileMapping = NULL;
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}
}

bool CNdCefShareMemory::OpenFileMemoryMapping(LPCTSTR memoryName,LPCTSTR evnetName)
{
	m_hFileMapping = ::OpenFileMapping(
		FILE_MAP_WRITE|FILE_MAP_READ,
		FALSE,
		memoryName);
	if (NULL == m_hFileMapping)
	{
		return false;
	}

	m_hEvent = OpenEvent(EVENT_MODIFY_STATE,TRUE,evnetName);
	if (NULL == m_hEvent)
	{
		return false;
	}

	return true;
}

bool CNdCefShareMemory::CreateFileMemoryMapping(LPCTSTR memoryName,LPCTSTR evnetName)
{
	if (m_hFileMapping==NULL){
		m_hFileMapping = ::CreateFileMapping(
			INVALID_HANDLE_VALUE,
			NULL,
			PAGE_READWRITE,
			0,
			MEMORY_SIZE,
			memoryName);
		if (NULL == m_hFileMapping)
		{
			return false;
		}
	}
	if (m_hEvent==NULL){
		m_hEvent = CreateEvent(NULL, FALSE, FALSE, evnetName);
		if (NULL == m_hEvent)
		{
			return false;
		}
	}
	return true;
}

bool CNdCefShareMemory::ReadMemory(void* pOutBuffer, DWORD& dwBufferSize,DWORD memorySize)
{
	WaitForSingleObject(m_hEvent, 2000);

	if( pOutBuffer == NULL || dwBufferSize == 0 || dwBufferSize < MEMORY_SIZE )
		return false;

	if (NULL == m_hFileMapping)
		return false;
	

	char* pBuffer = NULL;

	pBuffer = (LPSTR)MapViewOfFile(
		m_hFileMapping,
		FILE_MAP_READ,
		0,
		0,
		memorySize);
	if (NULL == pBuffer)
		return false;
	

	__try
	{
		CopyMemory((void*)pOutBuffer, pBuffer, memorySize);
		dwBufferSize = strlen(pBuffer);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		UnmapViewOfFile(pBuffer);
		return false;
	}

	UnmapViewOfFile(pBuffer);

	PulseEvent(m_hEvent);


	return true;
}

bool CNdCefShareMemory::WriteMemory(void* pData, DWORD dwSize,DWORD memorySize)
{
	if( pData == NULL || dwSize == 0 )
	{
		return false;
	}

	char* pBuffer = NULL;

	if (NULL == m_hFileMapping)
	{
		return false;
	}

	pBuffer = (char*)MapViewOfFile(
		m_hFileMapping,
		FILE_MAP_WRITE,
		0,
		0,
		memorySize);
	if (NULL == pBuffer)
	{
		return false;
	}

	__try
	{
		ZeroMemory(pBuffer, strlen(pBuffer));
		CopyMemory((void*)pBuffer, pData, dwSize);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		UnmapViewOfFile(pBuffer);
		return false;
	}

    UnmapViewOfFile(pBuffer);
	SetEvent(m_hEvent);

	return true;
}

DWORD WINAPI CNdCefShareMemory::PPTMemoryThreadProc(LPVOID lpParameter)
{
	CNdCefShareMemory* nNdCefShareMemory = NdCefShareMemory::GetInstance();
	HANDLE m_hEvent[2] ={NULL,NULL}; 
	if (nNdCefShareMemory){
		HANDLE hEvent[2] ={NULL,NULL};  
		hEvent[0] = nNdCefShareMemory->m_hEventStop;
		hEvent[1] = nNdCefShareMemory->m_hEventStart;
		while (true)
		{
			Sleep(1);
			DWORD dwRet = WaitForMultipleObjects(2,hEvent,false,INFINITE);
			switch(dwRet)
			{
			case WAIT_FAILED:
			case WAIT_OBJECT_0:
				return 1;
				break;
			case WAIT_OBJECT_0+1:
				{			
					ResetEvent(nNdCefShareMemory->m_hEventStart);
					while(!nNdCefShareMemory->m_qICRInfo.empty()){
						CICRInfo nInfo = nNdCefShareMemory->m_qICRInfo.front();
						nNdCefShareMemory->m_qICRInfo.pop();
						switch (nInfo.GetICRType())
						{
						case MSG_DRAWMETHOD:
						case MSG_CEF_ICRINVOKENATIVE:
						
							{
								nNdCefShareMemory->CreateFileMemoryMapping(GLOBAL_MEMORY_NAME,GLOBAL_EVENT_NAME);

								char* pResult = NULL;

								pResult = IcrPlayer::GetInstance()->IcrInvokeNativeMethod((char *)nInfo.GetICRInfo().c_str());

								// write student infos
								if( pResult != NULL )
								{
									nNdCefShareMemory->WriteMemory(pResult, strlen(pResult),MEMORY_SIZE);
									IcrPlayer::GetInstance()->IcrReleaseMemory(pResult);
								}else{
									tstring nInfo = "{}";
									nNdCefShareMemory->WriteMemory((char *)nInfo.c_str(), nInfo.length(),MEMORY_SIZE);
								}
							}
							break;
						}
					}
				}
				break;
			case WAIT_TIMEOUT:
				break;
			}
		}
	}
	return 0;
}
void CNdCefShareMemory::SetExit(){
	if (m_hEventStop) SetEvent(m_hEventStop);
}
void CNdCefShareMemory::SetIcrEvernt(int nICRType,tstring nICRInfo){
	CICRInfo nInfo;
	nInfo.SetICRType(nICRType);
	nInfo.SetICRInfo(nICRInfo);
	m_qICRInfo.push(nInfo);
	if (m_hEventStart) SetEvent(m_hEventStart);
}
void CNdCefShareMemory::CreateThreadSetEvent(){
	m_hEventStop = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEventStart = CreateEvent(NULL, TRUE, FALSE, NULL);
	CreateThread(NULL,0,PPTMemoryThreadProc,NULL,0,NULL); 
}