// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "NdCefApp.h"

extern HINSTANCE g_dllhInstance;

BOOL APIENTRY DllMain( HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					  )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_dllhInstance = (HINSTANCE)(hModule);
		CNdDebug::printfStr("DLL_PROCESS_ATTACH");
		break;
	case DLL_THREAD_ATTACH:
		//CNdDebug::printfStr("DLL_THREAD_ATTACH");
		break;
	case DLL_THREAD_DETACH:
		//CNdDebug::printfStr("DLL_THREAD_DETACH");
		break;
	case DLL_PROCESS_DETACH:
		CNdDebug::printfStr("DLL_PROCESS_DETACH");
		break;
	}
	return TRUE;
}

