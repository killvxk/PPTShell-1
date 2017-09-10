#pragma once

#include "NdCefHandler.h"

namespace NdCef{
	class CNdCefWindow
	{
	public:
		CNdCefWindow(std::wstring url);
		~CNdCefWindow();
	public:
		BOOL CNdCefWindow::WinMain(HINSTANCE hInstance,HWND nParentWnd,bool nosr,bool ntransparent,std::wstring nthumbPath);
		std::wstring m_Url;
		std::wstring m_nthumbPath;
		HINSTANCE m_hInstance;
		bool m_nOsr;
		bool m_ntransparent;
	private:
		static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam,LPARAM lParam);
		int RegisterMainClass(HINSTANCE hInstance);
		BOOL CreateMainWindow(HINSTANCE hInstance,HWND nParentWnd, int nCmdShow);
		void CreateMessage(HWND hWnd);
		static void SizeMessage(HWND hWnd,WPARAM wParam,LPARAM lParam);
	};
}

