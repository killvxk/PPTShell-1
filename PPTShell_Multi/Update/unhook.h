#include <windows.h>

BOOL CheckApiInLineHook(PVOID ApiAddress);
BOOL RemoveHook(LPCSTR szDllPath, LPCSTR szFuncName);
