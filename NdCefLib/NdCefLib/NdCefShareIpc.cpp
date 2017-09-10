#include "stdafx.h"
#include "NdCefShareIpc.h"

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

	m_hEvent = CreateEvent(NULL, FALSE, FALSE, evnetName);
	if (NULL == m_hEvent)
	{
		return false;
	}

	return true;
}

bool CNdCefShareMemory::ReadMemory(void* pOutBuffer, DWORD& dwBufferSize,DWORD memorySize)
{
	WaitForSingleObject(m_hEvent, 2000);

	if( pOutBuffer == NULL || dwBufferSize == 0 || dwBufferSize < memorySize ){
		dwBufferSize = 0;
		return false;
	}
	if (NULL == m_hFileMapping){
		dwBufferSize = 0;
		return false;
	}

	char* pBuffer = NULL;

	pBuffer = (LPSTR)MapViewOfFile(
		m_hFileMapping,
		FILE_MAP_READ,
		0,
		0,
		memorySize);
	if (NULL == pBuffer){
		dwBufferSize = 0;
		return false;
	}
	

	__try
	{
		CopyMemory((void*)pOutBuffer, pBuffer, memorySize);
		dwBufferSize = strlen(pBuffer);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		UnmapViewOfFile(pBuffer);
		dwBufferSize = 0;
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
 