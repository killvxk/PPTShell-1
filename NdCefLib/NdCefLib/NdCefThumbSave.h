#pragma once
#ifndef _NDCEF_THUMBSAVE_H_
#define _NDCEF_THUMBSAVE_H_

#include <windows.h>
#include <Gdiplus.h>
#include <GdiPlusEnums.h>
using namespace Gdiplus;

class CNdCefThumbSave 	
{
public:
	static void Initialize();
	static void UnInitialize();
	static bool GetEncoderClsid(const WCHAR* pszFormat, CLSID* pClsid);  
	static BOOL GetEncoderClsName(LPCTSTR lpszFileName, LPTSTR lpszBuf, DWORD dwBufSize);
	static Image* FixedSize(Image *imgSrc, int Width, int Height);
	static BOOL SaveBmpToJpeg(HBITMAP hBmp, LPCTSTR lpszJpegFileName, ULONG quality = 100);
};

#endif