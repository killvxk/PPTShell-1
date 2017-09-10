#include "unhook.h"

BOOL CheckApiInLineHook(PVOID ApiAddress)
{
	BYTE OrigCode[7] = {0};
	RtlCopyMemory(OrigCode,(BYTE*)ApiAddress, 6);
	if(OrigCode[0] == 0xE9 || (OrigCode[0] == 0x68 && OrigCode[4] == 0xC3) || OrigCode[0] == 0xB8)
	{
		return 1;
	}
	return 0;
}

BOOL RemoveHook(LPCSTR szDllPath, LPCSTR szFuncName)
{
	BOOL bRet = FALSE;
	void* lpBase = NULL;
	do 
	{
		lpBase = LoadLibrary(szDllPath);
		if (NULL == lpBase)
			break;
		
		void* lpFunc = GetProcAddress((HMODULE)lpBase, szFuncName);
		if (NULL == lpFunc)
			break;
		
		DWORD dwRVA = (DWORD)lpFunc - (DWORD)lpBase;
		HANDLE hFile = CreateFile(szDllPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (INVALID_HANDLE_VALUE == hFile)
			break;
		
		DWORD dwSize = GetFileSize(hFile, NULL);
		HANDLE hMapFile = CreateFileMapping(hFile, NULL, PAGE_READONLY|SEC_IMAGE, 0, dwSize, NULL);
		void* lpBaseMap = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, dwSize);
		void* lpRealFunc = (void*)((DWORD)lpBaseMap + dwRVA);
		bRet = TRUE;
		
		DWORD dwOldProtect;
		if (VirtualProtect(lpFunc, 10, PAGE_EXECUTE_READWRITE, &dwOldProtect))
			memcpy(lpFunc, lpRealFunc, 10);
		else
			bRet = FALSE;
		
		UnmapViewOfFile(lpBaseMap);
		CloseHandle(hMapFile);
		CloseHandle(hFile);
		
	} while (false);
	
	if (NULL != lpBase)
		FreeLibrary((HMODULE)lpBase);
	return bRet;
}
