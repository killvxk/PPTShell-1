#pragma once
#include "resource.h"
#include <string> 
#include <comdef.h>  
#include <gdiplus.h>
using namespace Gdiplus;
#include "CGdiPlusBitmap.h"

class CNdPngCloseBtn
{
public:
	CNdPngCloseBtn(void);
	~CNdPngCloseBtn(void);
public:
	ATOM NdBtnRegisterClass(HINSTANCE hInstance);
	BOOL NdBtnInitInstance(HINSTANCE hInstance, HWND hWndParent,int nCmdShow);
	static LRESULT CALLBACK NdBtnCloseProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static void NdDrawImage(HWND hWnd,HDC hdc);
	static CGdiPlusBitmapResource* pbgImage;
	static CGdiPlusBitmapResource* pbgImageClose;
	static CGdiPlusBitmapResource* pbgImageOver;
	static bool m_bIsMouseOver; 
};
