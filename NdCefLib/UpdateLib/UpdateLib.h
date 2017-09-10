#ifndef _UPDATELIB_H_
#define _UPDATELIB_H_

#ifdef PPT_EXPORTS
#define PPT_API extern "C" _declspec(dllexport)
#else
#define PPT_API extern "C" _declspec(dllimport)
#endif

PPT_API unsigned int AutoUpdate(HWND hwnd, bool bSelfUpdate, const char* strWorkDirectory);

#endif