#include "stdafx.h"
#include "NdCefApp.h"
#include "NdCefPlayWindow.h"
#include "NdCefContext.h"
#include "NdCefShareIpc.h"

#define IDI_PLAYWINDOW_ICON                       102
#define WM_USER_CPLAYER_NAVI_URL			WM_USER+8899
#define WM_USER_CPLAYER_PPT_PAGE			WM_USER+8898
#define WM_USER_CPLAYER_SET_MESSAGE_WND		WM_USER+8897

#define PLAYER_CLASS_NAME					L"CoursePlayer"
#define PLAYER_WINDOW_NAME					L"CoursePlayer"
#define CEF_CLASS_NAME						L"CefPlayer"
#define CEF_WINDOW_NAME						L"CefPlayer"
#define SHARED_MEMORY_NAME					L"CoursePlayerSharedMemory"

namespace NdCef{

	LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);

	extern CefRefPtr<CNdCefHandler> g_hNdCefHandle;

	CNdCefPlayWindow::CNdCefPlayWindow() {
		m_bPlayWindowClose = true;
		CNdDebug::printfStr("CNdCefPlayWindow");
	}
	CNdCefPlayWindow::~CNdCefPlayWindow() {
		CNdDebug::printfStr("~CNdCefPlayWindow");
	}
	void CNdCefPlayWindow::InitWindowInfo(std::wstring nTitle,std::wstring nUrl,bool nClose){
		m_Title = nTitle;
		m_Url = nUrl;
		m_bPlayWindowClose = nClose;
	}
	int CNdCefPlayWindow::RegisterMainClass(HINSTANCE hInstance) {
		WNDCLASSEX wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style         = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc   = MainWndProc;
		wcex.cbClsExtra    = 0;
		wcex.cbWndExtra    = 0;
		wcex.hInstance     = hInstance;
		wcex.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PLAYWINDOW_ICON));
		wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName  = NULL;
		if (m_bPlayWindowClose){
			wcex.lpszClassName = CEF_CLASS_NAME;
		}else{
			wcex.lpszClassName = PLAYER_CLASS_NAME;
		}
		wcex.hIconSm       = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PLAYWINDOW_ICON));
		return RegisterClassEx(&wcex);
	}

	BOOL CNdCefPlayWindow::CreateMainWindow(HINSTANCE hInstance, HWND nParentWnd,int nCmdShow) {
		HWND hWnd;
		CNdDebug::printf("%s %d","CNdCefPlayWindow::nParentWnd:",nParentWnd);
		int dwStyle = WS_OVERLAPPEDWINDOW| WS_CLIPCHILDREN;
		RECT nRect ={0,0,800,500};
		std::wstring className = PLAYER_CLASS_NAME;
		if (m_bPlayWindowClose){
			className = CEF_CLASS_NAME;
		}
		if (nParentWnd!=NULL){
			dwStyle = WS_CHILD;
			GetClientRect(
				nParentWnd, // ´°¿Ú¾ä±ú
				&nRect
				);
			hWnd = CreateWindow(className.c_str(), m_Title.c_str(),
				dwStyle, 0, 0,
				nRect.right-nRect.left, nRect.bottom-nRect.top, nParentWnd, NULL, hInstance, this);

		}else{
			hWnd = CreateWindow(className.c_str(), m_Title.c_str(),
				dwStyle, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nParentWnd,NULL, hInstance, this);
		}

		//
		if (!hWnd)
			return FALSE;


		int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

		RECT rt;
		GetClientRect(hWnd, &rt);

		int nWidth = rt.right - rt.left;
		int nHeight = rt.bottom - rt.top;

		int x = (nScreenWidth - nWidth) / 2;
		int y = (nScreenHeight - nHeight) / 2;

		::SetWindowPos(hWnd, NULL, x, y, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);


		if( nParentWnd != NULL )
			ShowWindow(hWnd, SW_SHOWMAXIMIZED);
		else
			ShowWindow(hWnd, SW_SHOWNORMAL);

		//ShowWindow(hWnd, nCmdShow);
		//UpdateWindow(hWnd);
		return TRUE;
	}

	void CNdCefPlayWindow::CreateMessage(HWND hWnd){
		if (!g_hNdCefHandle.get()){
			CNdDebug::printfStr("g_hNdCefHandle.get() NULL");
			g_hNdCefHandle = new CNdCefHandler();
		}
		if (g_hNdCefHandle.get()){
			g_hNdCefHandle->SetMainWindowHandle(hWnd);
			g_hNdCefHandle->SetPlayWindowClose(m_bPlayWindowClose);
		}

		CNdDebug::printfStr("CNdCefPlayWindow::CreateMessage");

		RECT rect;
		int x = 0;

		GetClientRect(hWnd, &rect);

		CefWindowInfo info;
		CefBrowserSettings settings;
		settings.file_access_from_file_urls = STATE_ENABLED;

		CNdCefContext::Get()->PopulateBrowserSettings(&settings);
		info.SetAsChild(hWnd, rect);
		CefBrowserHost::CreateBrowser(info, g_hNdCefHandle.get(),
			m_Url, settings, NULL);
	}

	void CNdCefPlayWindow::SizeMessage( HWND hszieWnd,WPARAM wParam,LPARAM lParam){
		CNdDebug::printfStr("CNdCefPlayWindow::SizeMessage");
		CNdDebug::printf("%d",wParam);
		if (wParam != SIZE_MINIMIZED) 
		{
			if (g_hNdCefHandle.get()){
				g_hNdCefHandle->SetZoomed();
			}
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

	LRESULT CALLBACK CNdCefPlayWindow::MainWndProc(HWND hWnd, UINT message, WPARAM wParam,LPARAM lParam) {
		//int wmId, wmEvent;
		CNdDebug::printf("%s %d","----------------MainWndProc",hWnd,message);
		PAINTSTRUCT ps;
		HDC hdc;
		switch (message) {
			case WM_CREATE: {
				LPCREATESTRUCT pN = (LPCREATESTRUCT)lParam;
				CNdCefPlayWindow* self = reinterpret_cast<CNdCefPlayWindow*>(pN->lpCreateParams);
				self->CreateMessage(hWnd);
							}
							return 0;
			case WM_USER_CPLAYER_NAVI_URL:
				if (g_hNdCefHandle.get() && !g_hNdCefHandle->IsClosing()) {
					CefRefPtr<CefBrowser> browser = g_hNdCefHandle->FindBrowser(hWnd);
					if (browser.get()) {
						HANDLE hFileMapping = OpenFileMapping(PAGE_READWRITE, FALSE, SHARED_MEMORY_NAME);
						if( hFileMapping != NULL )
						{
							char* szUrl = (char*)MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0x1000);
							if( szUrl != NULL )
								browser->GetMainFrame()->LoadURL(szUrl);

						} 

						ShowWindow(hWnd, SW_SHOWMAXIMIZED);
						return 0;
					}
				}
				break;

			case WM_USER_CPLAYER_PPT_PAGE:
				{/*
				 if( g_pPlayerWindow.get() && g_pPlayerWindow->GetBrowser() )
					{
					bool bLast = (wParam == lParam);	 
					g_pPlayerWindow->SetLastPPTPage(bLast);
					} 
					*/
				}
				break;
			case WM_USER_CPLAYER_SET_MESSAGE_WND:
				{
					if (g_hNdCefHandle.get() && !g_hNdCefHandle->IsClosing()) {
						CefRefPtr<CefBrowser> browser = g_hNdCefHandle->FindBrowser(hWnd);
						if (browser.get()) {

							char OutBuffer[MEMORY_URL_SIZE];
							memset(OutBuffer, 0, MEMORY_URL_SIZE);

							CNdCefShareMemory CefShareMemory;
							bool res = CefShareMemory.OpenFileMemoryMapping(GLOBAL_MEMORY_URL_NAME,GLOBAL_EVENT_URL_NAME);
							DWORD nSize = MEMORY_URL_SIZE;
							res = CefShareMemory.ReadMemory(OutBuffer, nSize,MEMORY_URL_SIZE);

							char* p = OutBuffer;

							std::string strUtfUrl = p;
							p += strUtfUrl.length() + 1;

							std::string strUtf8ChapterGuid = p;
							p += strUtf8ChapterGuid.length()+1;

							std::string strUtf8ChapterName = p;
							p += strUtf8ChapterName.length()+1;

							std::string strUtf8UserId = p;

							//
							char szUrl[1024];
							sprintf_s(szUrl, strUtfUrl.c_str(), strUtf8ChapterGuid.c_str(), strUtf8ChapterName.c_str(), strUtf8UserId.c_str());

							g_hNdCefHandle->SetPreloadUrl(szUrl);

							BOOL bVisible = IsWindowVisible(hWnd);
							if( !bVisible ){
								std::wstring strUrl = g_hNdCefHandle->GetPreloadUrl().ToWString();
								if (strUrl!=L""){
									browser->GetMainFrame()->LoadURL(strUrl);
								}
							}else{		
								::SetWindowPos(hWnd,HWND_TOPMOST,-1,-1,-1,-1,SWP_NOMOVE|SWP_NOSIZE); 
								::SetWindowPos(hWnd,HWND_NOTOPMOST,-1,-1,-1,-1,SWP_NOMOVE|SWP_NOSIZE); 
								::SetForegroundWindow(hWnd);
							}
						}
					}
				}
				break;
			case WM_PAINT:
				hdc = BeginPaint(hWnd, &ps);
				EndPaint(hWnd, &ps);
				return 0;
			case WM_SETFOCUS:
				if (g_hNdCefHandle.get()) {
					CefRefPtr<CefBrowser> browser = g_hNdCefHandle->FindBrowser(hWnd);
					if (browser.get()) {
						CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
						if (hwnd)
							PostMessage(hwnd, WM_SETFOCUS, wParam, NULL);
					}
				}
				return 0;
			case WM_SIZE: 
				if (!g_hNdCefHandle.get()){
					break;
				}
				SizeMessage(hWnd,wParam, lParam);
				break;

			case WM_MOVING:
			case WM_MOVE:
				//if (g_hNdCefHandle.get() && g_hNdCefHandle->GetBrowser())
				//	g_hNdCefHandle->GetBrowser()->GetHost()->NotifyMoveOrResizeStarted();
				return 0;

			case WM_ERASEBKGND:
				/*
				if (g_hNdCefHandle.get() && g_hNdCefHandle->GetBrowser()) {
				CefWindowHandle hWnd =
				g_hNdCefHandle->GetBrowser()->GetHost()->GetWindowHandle();
				if (hWnd) {
				return 0;
				}
				}*/
				break;

			case WM_ENTERMENULOOP:
				if (!wParam) {
					CefSetOSModalLoop(true);
				}
				break;

			case WM_EXITMENULOOP:
				CNdDebug::printf("%s %d","CNdCefPlayWindow::WM_EXITMENULOOP",wParam);
				if (!wParam) {
					CefSetOSModalLoop(false);
				}
				break;

			case WM_CLOSE:
				CNdDebug::printfStr("*****************×¢Òâ************************");
				CNdDebug::printfStr("CNdCefPlayWindow::WM_CLOSE");
				if (g_hNdCefHandle.get() && !g_hNdCefHandle->IsClosing()) {
					CefRefPtr<CefBrowser> browser = g_hNdCefHandle->FindBrowser(hWnd);
					if (browser.get()) {
						std::wstring strUrl = g_hNdCefHandle->GetPreloadUrl().ToWString();
						if (strUrl!=L""){
							browser->GetMainFrame()->LoadURL(strUrl);
						}
						CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
						HWND  hCefParent = GetParent(hwnd);
						ShowWindow(hCefParent, SW_HIDE);
						if (!g_hNdCefHandle->GetPlayWindowClose()){
							return 0;
						}
					}
				}
				break;

			case WM_DESTROY:
				CNdDebug::printfStr("CNdCefPlayWindow::WM_DESTROY");
				return 0;
		}

		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	BOOL CNdCefPlayWindow::WinMain(HINSTANCE hInstance,HWND nParentWnd){
		RegisterMainClass(hInstance);
		if (!CreateMainWindow(hInstance,nParentWnd, SW_SHOW)){
			return FALSE;
		}
		return TRUE;
	}
}

