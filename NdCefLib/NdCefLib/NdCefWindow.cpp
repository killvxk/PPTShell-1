#include "stdafx.h"
#include "NdCefApp.h"
#include "NdCefWindow.h"
#include "NdCefContext.h"
#include "NdCefOSRWindow.h"

namespace NdCef{

	LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);

	std::wstring g_szTitle = L"NdCef";  
	std::wstring g_szWindowClass = L"NdCefWindowsClass";  
	std::wstring g_szOSRWindowClass = L"NdCefOSRWindowsClass";

	CefRefPtr<CNdCefHandler> g_hNdCefHandle;

	class MainBrowserProvider : public OSRBrowserProvider {
		virtual CefRefPtr<CefBrowser> GetBrowser() {
			if (g_hNdCefHandle.get())
				return g_hNdCefHandle->GetBrowser();
			return NULL;
		}
	} g_main_browser_provider;

	CNdCefWindow::CNdCefWindow(std::wstring url) {
		m_Url = url;
		CNdDebug::printfStr("CNdCefWindow");
	}
	CNdCefWindow::~CNdCefWindow() {
		CNdDebug::printfStr("~CNdCefWindow");
	}

	int CNdCefWindow::RegisterMainClass(HINSTANCE hInstance) {
		WNDCLASSEX wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style         = CS_HREDRAW | CS_VREDRAW;
		
		wcex.cbClsExtra    = 0;
		wcex.cbWndExtra    = 0;
		wcex.hInstance     = hInstance;
		wcex.hIcon         = LoadIcon(NULL,IDI_APPLICATION);
		wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName  = NULL;
		wcex.lpfnWndProc   = MainWndProc;
		wcex.lpszClassName = g_szWindowClass.c_str();
		wcex.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

		return RegisterClassEx(&wcex);
	}

	BOOL CNdCefWindow::CreateMainWindow(HINSTANCE hInstance, HWND nParentWnd,int nCmdShow) {
		HWND hWnd;
		m_hInstance = hInstance;
		
		if (m_nOsr){
			CreateMessage(nParentWnd);
		}else{
			RegisterMainClass(hInstance);
			CNdDebug::printf("%s %d","CNdCefWindow::nParentWnd:",nParentWnd);
			int dwStyle = WS_OVERLAPPEDWINDOW;
			RECT nRect ={0,0,800,500};
			if (nParentWnd!=NULL){
				dwStyle = WS_CHILDWINDOW | WS_VISIBLE;
				WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |       
					WS_BORDER | ES_LEFT | ES_MULTILINE |       
					ES_AUTOHSCROLL | ES_AUTOVSCROLL;
				GetClientRect(
					nParentWnd, // ´°¿Ú¾ä±ú
					&nRect
					);
			}

			hWnd = CreateWindow(g_szWindowClass.c_str(), g_szTitle.c_str(),
				dwStyle, 0, 0,
				nRect.right-nRect.left, nRect.bottom-nRect.top, nParentWnd, NULL, hInstance, this);
			if (!hWnd)
				return FALSE;
			ShowWindow(hWnd, nCmdShow);
			UpdateWindow(hWnd);
		}
		return TRUE;
	}

	void CNdCefWindow::CreateMessage(HWND hWnd){
		if (!g_hNdCefHandle.get()){
			CNdDebug::printfStr("g_hNdCefHandle.get() NULL");
			g_hNdCefHandle = new CNdCefHandler();
			g_hNdCefHandle->SetMainWindowHandle(hWnd);
		}else{
			g_hNdCefHandle->SetMainWindowHandle(hWnd);
		}

		CNdDebug::printfStr("CNdCefWindow::CreateMessage");

		RECT rect;
		int x = 0;

		GetClientRect(hWnd, &rect);

		CefWindowInfo info;
		CefBrowserSettings settings;
		settings.file_access_from_file_urls = STATE_ENABLED;
		settings.universal_access_from_file_urls = STATE_ENABLED;
		settings.webgl = STATE_ENABLED;
		settings.web_security = STATE_DISABLED; 
		CNdCefContext::Get()->PopulateBrowserSettings(&settings);
		
		if (m_nOsr){
			CefRefPtr<CNdCefOSRWindow> osr_window =
			CNdCefOSRWindow::Create(&g_main_browser_provider, m_ntransparent,true);
			g_hNdCefHandle->SetOSRHandler(osr_window.get());
			if (m_ntransparent){
				osr_window->CreateWidget(hWnd, rect, m_hInstance,g_szOSRWindowClass.c_str(),m_Url);
				info.SetAsWindowless(osr_window->hwnd(), true);
			}
			else{
				osr_window->SetThumbImgPath(m_nthumbPath);
				osr_window->CreateWidget(hWnd, rect, m_hInstance,g_szOSRWindowClass.c_str(),m_Url);
				info.SetAsWindowless(osr_window->hwnd(), false);
			}
		
			CefBrowserHost::CreateBrowser(info, g_hNdCefHandle.get(),
				m_Url, settings, NULL);
			
		}else{
			if (g_hNdCefHandle.get()){
				g_hNdCefHandle->SetOcxWindow(true);
			}
			
			info.SetAsChild(hWnd, rect);
			CefBrowserHost::CreateBrowser(info, g_hNdCefHandle.get(),
				m_Url, settings, NULL);
		}
		
	}

	void CNdCefWindow::SizeMessage( HWND hszieWnd,WPARAM wParam,LPARAM lParam){
		CNdDebug::printfStr("CNdCefWindow::SizeMessage");
		if (wParam != SIZE_MINIMIZED) 
		{
			CefRefPtr<CefBrowser> browser = g_hNdCefHandle->FindBrowser(hszieWnd);
			if (browser.get()) {
				CefWindowHandle hWnd = browser->GetHost()->GetWindowHandle();
				if (hWnd) 
				{
					RECT rect;
					GetClientRect(hszieWnd, &rect);

					HDWP hdwp = BeginDeferWindowPos(1);

					hdwp = DeferWindowPos(hdwp, hWnd, NULL,
						rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
						SWP_NOZORDER);
					EndDeferWindowPos(hdwp);
				}
			}
		}
	}
	LRESULT CALLBACK CNdCefWindow::MainWndProc(HWND hWnd, UINT message, WPARAM wParam,LPARAM lParam) {
		PAINTSTRUCT ps;
		HDC hdc;
		switch (message) {
			case WM_SYSKEYDOWN:
				if (wParam == VK_F4)
				{
					return false;
				}
				break;
			case WM_CREATE: {
				LPCREATESTRUCT pN = (LPCREATESTRUCT)lParam;
				CNdCefWindow* self = reinterpret_cast<CNdCefWindow*>(pN->lpCreateParams);
				self->CreateMessage(hWnd);
				}
				return 0;
			case WM_PAINT:
				hdc = BeginPaint(hWnd, &ps);
				EndPaint(hWnd, &ps);
				return 0;
			case WM_SETFOCUS:
				if (g_hNdCefHandle.get()) {
					CefRefPtr<CefBrowser> browser = g_hNdCefHandle.get()->FindBrowser(hWnd);
					if (browser){
						g_hNdCefHandle->SetFocusToBrowser(browser);
					}
				}
				return 0;
			case WM_SIZE: 
				if (!g_hNdCefHandle.get()){
					break;
				}
				SizeMessage(hWnd,wParam, lParam);
				return 0;

			case WM_MOVING:
			case WM_MOVE:
				if (g_hNdCefHandle.get() && g_hNdCefHandle->GetBrowser())
					g_hNdCefHandle->GetBrowser()->GetHost()->NotifyMoveOrResizeStarted();
				return 0;

			case WM_ERASEBKGND:
				if (g_hNdCefHandle.get() && g_hNdCefHandle->GetBrowser()) {
					CefWindowHandle hWnd =
						g_hNdCefHandle->GetBrowser()->GetHost()->GetWindowHandle();
					if (hWnd) {
						return 0;
					}
				}
				break;
			case WM_MOUSEWHEEL:
				CNdDebug::printfStr("*****************WM_MOUSEWHEEL************************");
				return 0;
				break;
			case WM_ENTERMENULOOP:
				if (!wParam) {
					CefSetOSModalLoop(true);
				}
				break;

			case WM_EXITMENULOOP:
				CNdDebug::printf("%s %d","CNdCefWindow::WM_EXITMENULOOP",wParam);
				if (!wParam) {
					CefSetOSModalLoop(false);
				}
				break;

			case WM_CLOSE:
				CNdDebug::printfStr("*****************×¢Òâ************************");
				CNdDebug::printfStr("CNdCefWindow::WM_CLOSE");
				if (g_hNdCefHandle.get() && !g_hNdCefHandle->IsClosing()) {
					CefRefPtr<CefBrowser> browser = g_hNdCefHandle->GetBrowser();
					if (browser.get()) {
						browser->GetHost()->CloseBrowser(false);
						return 0;
					}
				}
				break;
			case WM_DESTROY:
				CNdDebug::printfStr("CNdCefWindow::WM_DESTROY");
				return 0;
		}

		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	BOOL CNdCefWindow::WinMain(HINSTANCE hInstance,HWND nParentWnd,bool nosr,bool ntransparent,std::wstring nthumbPath){
		m_nOsr = nosr;
		m_ntransparent = ntransparent;
		m_nthumbPath = nthumbPath;
		if (!CreateMainWindow(hInstance,nParentWnd, SW_SHOW)){
			return FALSE;
		}
		return TRUE;
	}
}

