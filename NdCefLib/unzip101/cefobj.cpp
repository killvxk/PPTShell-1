#include "stdafx.h"
#include "cefobj.h"
#include <windows.h>
#include <string>
#include "Userenv.h"
#include <tlhelp32.h>
#pragma comment(lib, "Userenv.lib")

typedef bool(*NdCefUnZipFun)(const char* filezip,const char* filepath);

CCefObject::CCefObject()
{
}

CCefObject::~CCefObject()
{
}

int KillExeCheckParentPid(std::wstring strExeName, BOOL bCloseAll ) 
{ 
	PROCESSENTRY32 my; 
	int nCount = 0;
	HANDLE l = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	if (((int)l) != -1) {
		my.dwSize = sizeof(my);
		if (Process32First(l, &my)) {
			do 
			{ 
				_tcsupr(my.szExeFile);
				if (_tcsstr(my.szExeFile, strExeName.c_str()) != NULL
					|| _tcsicmp(my.szExeFile, strExeName.c_str()) == 0) { 
						HANDLE hProcessHandle;
						hProcessHandle = OpenProcess(PROCESS_TERMINATE, true, my.th32ProcessID); 
						if (hProcessHandle != NULL)
						{ 
							if(TerminateProcess(hProcessHandle, 0) != NULL)
							{
								CloseHandle(hProcessHandle);
								nCount++;
							}
						}
						if (!bCloseAll)//2015.12.03 cws
						{
							CloseHandle(l);					
							return 1; 
						}
				}
			}while (Process32Next(l, &my));
		} 
		CloseHandle(l); 
	} 
	return nCount;
}
bool CCefObject::UnZip(const char* filezip,const char* filepath)
{	
	HMODULE hCefObject = LoadLibrary(L"NdCefLib.dll"); 
	NdCefUnZipFun pFnNdCefUnZip = (NdCefUnZipFun)GetProcAddress(hCefObject,"NdCefUnZip"); 

	if (pFnNdCefUnZip != NULL) 
	{ 
		KillExeCheckParentPid(L"node.exe",TRUE);
		return pFnNdCefUnZip(filezip,filepath); 
	}
	return false;
}
