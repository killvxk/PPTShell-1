#pragma once
#include "resource.h"
#include <string> 
#include <comdef.h>  
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

#include "CGdiPlusBitmap.h"

#define MAX_LOADSTRING 100

namespace NdUI{
	class CNdUI
	{
	public:
		CNdUI(void);
		~CNdUI(void);
	public:
		ATOM NdRegisterClass(HINSTANCE hInstance);
		BOOL NdInitInstance(HINSTANCE hInstance, int nCmdShow);
		BOOL NdMain(HINSTANCE hInstance,int nCmdShow);
		static void ClearCloseTime();
		static void AddCloseTime();
		static BOOL IsCloseTime();
		static void NdDrawImage(HDC hdc);
		static LRESULT CALLBACK NdWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		static HINSTANCE m_hInst;	
		static CGdiPlusBitmapResource* pbgImage;
		static bool m_bIsMouseOver; 
	private:
		static std::wstring szFile;
		static int m_iTimeClose;
									// current instance
		TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
		TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
	};
}