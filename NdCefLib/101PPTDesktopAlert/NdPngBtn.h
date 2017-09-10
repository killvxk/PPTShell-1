#pragma once
#include "resource.h"
#include <string> 
#include <comdef.h>  
#include <gdiplus.h>
using namespace Gdiplus;
#include "CGdiPlusBitmap.h"

class CNdPngBtn
{
public:
	CNdPngBtn(void);
	~CNdPngBtn(void);
public:
	ATOM NdBtnRegisterClass(HINSTANCE hInstance);
	BOOL NdBtnInitInstance(HINSTANCE hInstance, HWND hWndParent,int nCmdShow);
	static LRESULT CALLBACK NdBtnProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static void NdDrawImage(HWND hWnd,HDC hdc);
	static CGdiPlusBitmapResource* pbgImage;
	static bool m_bIsMouseOver; 
};
