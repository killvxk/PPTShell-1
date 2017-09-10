#pragma once

#include "NdCefHandler.h"

namespace NdCef{
	class CNdCefPlayWindow
	{
	public:
		CNdCefPlayWindow();
		~CNdCefPlayWindow();
	public:
		BOOL WinMain(HINSTANCE hInstance,HWND nParentWnd);
		void InitWindowInfo(std::wstring nTitle,std::wstring nUrl,bool nClose);
	private:
		std::wstring m_Title;
		std::wstring m_Url;
		bool m_bPlayWindowClose;
	private:
		static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam,LPARAM lParam);
		int RegisterMainClass(HINSTANCE hInstance);
		BOOL CreateMainWindow(HINSTANCE hInstance,HWND nParentWnd, int nCmdShow);
		void CreateMessage(HWND hWnd);
		static void SizeMessage(HWND hWnd,WPARAM wParam,LPARAM lParam);
	};
}

