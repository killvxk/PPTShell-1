#include "NdCefOSRWindow.h"

#include <windowsx.h>

#include "include/base/cef_bind.h"
#include "include/base/cef_build.h"
#include "include/wrapper/cef_closure_task.h"
#include "NdCefThumbSave.h"

namespace NdCef {
	namespace {
		bool IsKeyDown(WPARAM wparam) {
			return (GetKeyState(wparam) & 0x8000) != 0;
		}

		int GetCefMouseModifiers(WPARAM wparam) {
			int modifiers = 0;
			if (wparam & MK_CONTROL)
				modifiers |= EVENTFLAG_CONTROL_DOWN;
			if (wparam & MK_SHIFT)
				modifiers |= EVENTFLAG_SHIFT_DOWN;
			if (IsKeyDown(VK_MENU))
				modifiers |= EVENTFLAG_ALT_DOWN;
			if (wparam & MK_LBUTTON)
				modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
			if (wparam & MK_MBUTTON)
				modifiers |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;
			if (wparam & MK_RBUTTON)
				modifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;

			// Low bit set from GetKeyState indicates "toggled".
			if (::GetKeyState(VK_NUMLOCK) & 1)
				modifiers |= EVENTFLAG_NUM_LOCK_ON;
			if (::GetKeyState(VK_CAPITAL) & 1)
				modifiers |= EVENTFLAG_CAPS_LOCK_ON;
			return modifiers;
		}

		int GetCefKeyboardModifiers(WPARAM wparam, LPARAM lparam) {
			int modifiers = 0;
			if (IsKeyDown(VK_SHIFT))
				modifiers |= EVENTFLAG_SHIFT_DOWN;
			if (IsKeyDown(VK_CONTROL))
				modifiers |= EVENTFLAG_CONTROL_DOWN;
			if (IsKeyDown(VK_MENU))
				modifiers |= EVENTFLAG_ALT_DOWN;

			// Low bit set from GetKeyState indicates "toggled".
			if (::GetKeyState(VK_NUMLOCK) & 1)
				modifiers |= EVENTFLAG_NUM_LOCK_ON;
			if (::GetKeyState(VK_CAPITAL) & 1)
				modifiers |= EVENTFLAG_CAPS_LOCK_ON;

			switch (wparam) 
			{
			case VK_RETURN:
				if ((lparam >> 16) & KF_EXTENDED)
					modifiers |= EVENTFLAG_IS_KEY_PAD;
				break;
			case VK_INSERT:
			case VK_DELETE:
			case VK_HOME:
			case VK_END:
			case VK_PRIOR:
			case VK_NEXT:
			case VK_UP:
			case VK_DOWN:
			case VK_LEFT:
			case VK_RIGHT:
				if (!((lparam >> 16) & KF_EXTENDED))
					modifiers |= EVENTFLAG_IS_KEY_PAD;
				break;
			case VK_NUMLOCK:
			case VK_NUMPAD0:
			case VK_NUMPAD1:
			case VK_NUMPAD2:
			case VK_NUMPAD3:
			case VK_NUMPAD4:
			case VK_NUMPAD5:
			case VK_NUMPAD6:
			case VK_NUMPAD7:
			case VK_NUMPAD8:
			case VK_NUMPAD9:
			case VK_DIVIDE:
			case VK_MULTIPLY:
			case VK_SUBTRACT:
			case VK_ADD:
			case VK_DECIMAL:
			case VK_CLEAR:
				modifiers |= EVENTFLAG_IS_KEY_PAD;
				break;
			case VK_SHIFT:
				if (IsKeyDown(VK_LSHIFT))
					modifiers |= EVENTFLAG_IS_LEFT;
				else if (IsKeyDown(VK_RSHIFT))
					modifiers |= EVENTFLAG_IS_RIGHT;
				break;
			case VK_CONTROL:
				if (IsKeyDown(VK_LCONTROL))
					modifiers |= EVENTFLAG_IS_LEFT;
				else if (IsKeyDown(VK_RCONTROL))
					modifiers |= EVENTFLAG_IS_RIGHT;
				break;
			case VK_MENU:
				if (IsKeyDown(VK_LMENU))
					modifiers |= EVENTFLAG_IS_LEFT;
				else if (IsKeyDown(VK_RMENU))
					modifiers |= EVENTFLAG_IS_RIGHT;
				break;
			case VK_LWIN:
				modifiers |= EVENTFLAG_IS_LEFT;
				break;
			case VK_RWIN:
				modifiers |= EVENTFLAG_IS_RIGHT;
				break;
			}
			return modifiers;
		}
	}  // namespace


	extern CefRefPtr<CNdCefHandler> g_hNdCefHandle;

	// static
	CefRefPtr<CNdCefOSRWindow> CNdCefOSRWindow::Create(
		OSRBrowserProvider* browser_provider,
		bool transparent,
		bool show_update_rect) {
			DCHECK(browser_provider);
			if (!browser_provider)
				return NULL;

			return new CNdCefOSRWindow(browser_provider, transparent, show_update_rect);
	}

	// static
	CefRefPtr<CNdCefOSRWindow> CNdCefOSRWindow::From(
		CefRefPtr<CNdCefHandler::RenderHandler> renderHandler) {
			return static_cast<CNdCefOSRWindow*>(renderHandler.get());
	}

	bool CNdCefOSRWindow::CreateWidget(HWND hWndParent, const RECT& rect,
		HINSTANCE hInst, LPCTSTR className,std::wstring nUrl) {
			DCHECK(hWnd_ == NULL && hDC_ == NULL && hRC_ == NULL);

			RegisterOSRClass(hInst, className);
			if (m_transparent){
				POINT pt;
				::GetCursorPos(&pt);

				HMONITOR hMonitor = ::MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
				MONITORINFO monitorInfo;
				monitorInfo.cbSize = sizeof(MONITORINFO);
				::GetMonitorInfo(hMonitor, &monitorInfo);

				int offsetx=0;
				int offsety=0;

				int m_nWidth = monitorInfo.rcMonitor.right-monitorInfo.rcMonitor.left-offsetx;
				int m_nHeight =monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top-offsety;
	
				std::wstring str[]={L"天平.html",L"摸球.html",L"掷骰子.html",L"植树工具.html",L"图形切割.html", L"方块塔.html", L"模拟时钟.html"};  
				std::vector<std::wstring> strArray(str, str+7);  
				bool nFind = false;
				for (size_t i =0;i<strArray.size();i++){
					if (nUrl.find(strArray[i])!=std::wstring::npos){
						nFind = true;
						break;
					}
				}
				nFind = true;
				//设置为ppt的子窗
				if (IsWindow(hWndParent)&&nFind){
					hWnd_ = ::CreateWindow(className, NULL,
						WS_POPUP | WS_SYSMENU   ,
						0, 0, m_nWidth, m_nHeight,hWndParent, NULL, hInst, this);	

					if (!hWnd_)
						return false;

					SetWindowLongPtr(hWnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
					::ShowWindow(hWnd_, SW_SHOWNORMAL);
					::UpdateWindow(hWnd_);
					::SetWindowPos(hWnd_, HWND_TOP, offsetx/2 ,offsety/2 ,m_nWidth ,m_nHeight , SWP_SHOWWINDOW);
				}else{
					hWnd_ = ::CreateWindow(className, NULL,
						WS_POPUP | WS_SYSMENU   ,
						0, 0, m_nWidth, m_nHeight,NULL, NULL, hInst, this);	
			
					if (!hWnd_)
						return false;

					SetWindowLongPtr(hWnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
					::ShowWindow(hWnd_, SW_SHOWNORMAL);
					::UpdateWindow(hWnd_);
					::SetWindowPos(hWnd_, HWND_TOPMOST, offsetx/2 ,offsety/2 ,m_nWidth ,m_nHeight , SWP_SHOWWINDOW);
				} 
				HWND hWnd = ::FindWindow((L"CEFOCX"), NULL);  //向CEFOCX的窗口发送页面变动消息
				if (::IsWindow(hWnd)){
					::SendMessage(hWnd, MSG_CEFOSR_ESC, NULL, NULL);
				}

			}else{
				hWnd_ = ::CreateWindow(className, NULL, WS_POPUP  | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX,
					0, 0, 1680, 1050, NULL, NULL, hInst, this);

				::ShowWindow(hWnd_, SW_HIDE);//WS_OVERLAPPEDWINDOW| WS_CLIPCHILDREN
				::UpdateWindow(hWnd_);
			}
			AddRef();

			return true;
	}

	void CNdCefOSRWindow::DestroyWidget() {
		if (IsWindow(hWnd_))
			DestroyWindow(hWnd_);
	}
	bool CNdCefOSRWindow::IsClosed() {
		return m_close;
	}
	void CNdCefOSRWindow::SetClosed() {
		m_close = true;
	}
   bool CNdCefOSRWindow::OnConsoleMessage(CefRefPtr<CefBrowser> browser,
		const CefString& message,
		const CefString& source,
		int line){
		if ( message=="playerloaded")
		{
			if (!m_transparent&&m_thumbImgPath!=L""){
				CNdCefThumbSave::Initialize();
				CNdCefThumbSave::SaveBmpToJpeg(m_bitmap_buffer,m_thumbImgPath.c_str(),100);
				CNdCefThumbSave::UnInitialize();
			}

			browser->GetHost()->CloseBrowser(true);
		}
		return true;
   }
	void CNdCefOSRWindow::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
		::DestroyWindow(hWnd_);
	}

	bool CNdCefOSRWindow::GetRootScreenRect(CefRefPtr<CefBrowser> browser,
		CefRect& rect) {
			RECT window_rect = {0};
			HWND root_window = GetAncestor(hWnd_, GA_ROOT);
			if (::GetWindowRect(root_window, &window_rect)) {
				rect = CefRect(window_rect.left,
					window_rect.top,
					window_rect.right - window_rect.left,
					window_rect.bottom - window_rect.top);
				return true;
			}
			return false;
	}

	bool CNdCefOSRWindow::GetViewRect(CefRefPtr<CefBrowser> browser,
		CefRect& rect) {
			RECT clientRect;
			if (!::GetClientRect(hWnd_, &clientRect))
				return false;
			rect.x = rect.y = 0;
			rect.width = clientRect.right;
			rect.height = clientRect.bottom;
			return true;
	}

	bool CNdCefOSRWindow::GetScreenPoint(CefRefPtr<CefBrowser> browser,
		int viewX,
		int viewY,
		int& screenX,
		int& screenY) {
			if (!::IsWindow(hWnd_))
				return false;

			// Convert the point from view coordinates to actual screen coordinates.
			POINT screen_pt = {viewX, viewY};
			ClientToScreen(hWnd_, &screen_pt);
			screenX = screen_pt.x;
			screenY = screen_pt.y;
			return true;
	}
	void CNdCefOSRWindow::SetThumbImgPath(std::wstring nPath){
		m_thumbImgPath = nPath; 
	}
	void CNdCefOSRWindow::OnPopupShow(CefRefPtr<CefBrowser> browser,
		bool show) {
			if (!show) {
				browser->GetHost()->Invalidate(PET_VIEW);
			}
	}

	void CNdCefOSRWindow::OnPopupSize(CefRefPtr<CefBrowser> browser,
		const CefRect& rect) {
	}
	void CNdCefOSRWindow::OnPaint(CefRefPtr<CefBrowser> browser,
		PaintElementType type,
		const RectList& dirtyRects,
		const void* buffer,
		int width, int height) {
		
			if (this->m_dc_buffer == NULL 
				|| m_pre_width != width 
				|| m_pre_height != height) {
					if (this->m_dc_buffer != NULL){
						DeleteDC(m_dc_buffer);
					}

					if (this->m_bitmap_buffer){
						DeleteObject(m_bitmap_buffer);
						m_bitmap_buffer = NULL;
					}
					this->m_dc_buffer = this->CreateBitmapFromData(buffer, width, height, m_bitmap_buffer);
					m_pre_width = width;
					m_pre_height = height;
			}
			else {	// bitmap created,copy the response memory to the bitmap buffer,so that the image can update 
				for (size_t i = 0; i < dirtyRects.size(); i++)
				{
					CefRect rect = dirtyRects[i];

					// every pixel takes 4 bytes 
					int offset = rect.y*width * 4 + rect.x * 4;
					char* startPos = (char*)m_ptr_bitmap_buffer + offset;
					char* bufferStartPos = (char*)buffer + offset;

					// copy every line of current dirty Rect
					for (int j = 0; j < rect.height; j++)
					{
						int lineOffset = j*width * 4;
						memcpy(startPos + lineOffset, bufferStartPos + lineOffset, rect.width * 4);
					}
				}

				//SaveBmp(m_bitmap_buffer, g_szSavePath);
			}
			if (m_transparent){
				DrawTransparent(false);
			}else{
				//DrawNormalPartial(dirtyRects, false);
				DrawTransparent(false);
			}
	}

	void CNdCefOSRWindow::OnCursorChange(CefRefPtr<CefBrowser> browser,
		CefCursorHandle cursor,
		CursorType type,
		const CefCursorInfo& custom_cursor_info) {
			if (!::IsWindow(hWnd_))
				return;

			// Change the plugin window's cursor.
			//SetClassLongPtr(hWnd_, GCLP_HCURSOR,
			//	static_cast<LONG>(reinterpret_cast<LONG_PTR>(cursor)));
			//SetCursor(cursor);
			SetCursor (LoadCursor (NULL, IDC_ARROW));
	}

	bool CNdCefOSRWindow::StartDragging(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefDragData> drag_data,
		CefRenderHandler::DragOperationsMask allowed_ops,
		int x, int y) {
			return false;
	}

	void CNdCefOSRWindow::UpdateDragCursor(CefRefPtr<CefBrowser> browser,
		CefRenderHandler::DragOperation operation) {
	}

	void CNdCefOSRWindow::Invalidate() {
		if (!CefCurrentlyOn(TID_UI)) {
			CefPostTask(TID_UI, base::Bind(&CNdCefOSRWindow::Invalidate, this));
			return;
		}
	}

	void CNdCefOSRWindow::WasHidden(bool hidden) {
		if (hidden == hidden_)
			return;
		CefRefPtr<CefBrowser> browser = browser_provider_->GetBrowser();
		if (!browser)
			return;
		browser->GetHost()->WasHidden(hidden);
		hidden_ = hidden;
	}

	CNdCefOSRWindow::CNdCefOSRWindow(OSRBrowserProvider* browser_provider,
		bool transparent,
		bool show_update_rect)
		:browser_provider_(browser_provider),
		hWnd_(NULL),
		hDC_(NULL),
		hRC_(NULL),
		painting_popup_(false),
		hidden_(false) {
			m_dc_buffer = NULL;
			m_bitmap_buffer = NULL;
			m_transparent = transparent;
			m_close = false;
	}

	CNdCefOSRWindow::~CNdCefOSRWindow() {
		DestroyWidget();
	}

	void CNdCefOSRWindow::OnDestroyed() {
		SetWindowLongPtr(hWnd_, GWLP_USERDATA, 0L);
		hWnd_ = NULL;
		Release();
	}

	ATOM CNdCefOSRWindow::RegisterOSRClass(HINSTANCE hInstance, LPCTSTR className) {
		WNDCLASSEX wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style         = CS_OWNDC;
		wcex.lpfnWndProc   = &CNdCefOSRWindow::WndProc;
		wcex.cbClsExtra    = 0;
		wcex.cbWndExtra    = 0;
		wcex.hInstance     = hInstance;
		wcex.hIcon         = NULL;
		wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName  = NULL;
		wcex.lpszClassName = className;
		wcex.hIconSm       = LoadIcon(NULL,IDI_APPLICATION);
		return RegisterClassEx(&wcex);
	}
	
	void CNdCefOSRWindow::PaintWindow(HWND hWnd)
	{
		if (m_transparent){
			this->DrawTransparent(true);
		}else{
			this->DrawTransparent(true);
			//this->DrawNormal(true);
		}
	}
	void CNdCefOSRWindow::DrawNormal(bool bWmPaint) {
		RECT r;
		GetWindowRect(this->hWnd_, &r);
		RectList rectlist;
		CefRect rect(0, 0, r.right - r.left, r.bottom - r.top);
		rectlist.push_back(rect);
		DrawNormalPartial(rectlist, true);
	}
	void CNdCefOSRWindow::DrawTransparent(bool bWmPaint)
	{
		PAINTSTRUCT ps;
		HDC dc;
		BLENDFUNCTION blend;

		if (bWmPaint){
			dc = BeginPaint(hWnd_, &ps);
		}
		else{
			dc = GetDC(hWnd_);
		}

		blend.BlendOp = 0; //theonlyBlendOpdefinedinWindows2000
		blend.BlendFlags = 0; //nothingelseisspecial...
		blend.AlphaFormat = AC_SRC_ALPHA;//AC_SRC_ALPHA;//AC_SRC_ALPHA; //...
		blend.SourceConstantAlpha = 255; // :该选项控制着整个窗口的透明度. 1为完全透明, 255为完全不透明

		SIZE size_window;
		POINT pt_dest;
		RECT rect;
		GetWindowRect(hWnd_, &rect);
		pt_dest.x = rect.left;
		pt_dest.y = rect.top;

		size_window.cx = rect.right - rect.left;
		size_window.cy = rect.bottom - rect.top;
		POINT pt_src;
		pt_src.x = 0;
		pt_src.y = 0;

		UpdateLayeredWindow(this->hWnd_, dc, &pt_dest, &size_window, this->m_dc_buffer, &pt_src, 0, &blend, 2);

		if (bWmPaint) {
			EndPaint(hWnd_, &ps);
		}
		else{
			ReleaseDC(hWnd_, dc);
		}
	}

	void CNdCefOSRWindow::DrawNormalPartial(const RectList& list, bool is_wm_paint){
		PAINTSTRUCT ps;
		HDC dc;
		SIZE sizeWindow;
		POINT ptDest;
		RECT rect;

		if (is_wm_paint){
			dc = BeginPaint(hWnd_, &ps);
		}
		else{
			dc = GetDC(hWnd_);
		}
		GetWindowRect(hWnd_, &rect);
		for (size_t i = 0; i < list.size(); i++) {
			CefRect cefrect = list[i];
			ptDest.x = cefrect.x;
			ptDest.y = cefrect.y;

			sizeWindow.cx = cefrect.width;
			sizeWindow.cy = cefrect.height;
			POINT ptSrc;
			ptSrc.x = cefrect.x;
			ptSrc.y = cefrect.y;

			BitBlt(dc, ptDest.x, ptDest.y, sizeWindow.cx, sizeWindow.cy, m_dc_buffer, ptDest.x, ptDest.y, SRCCOPY);
		}
		if (is_wm_paint) {
			EndPaint(hWnd_, &ps);
		}
		else{
			ReleaseDC(hWnd_, dc);
		}
	}
	HDC CNdCefOSRWindow::CreateBitmapFromData(const void* pData, int width, int height, HBITMAP& bitmap)
	{
		BITMAPFILEHEADER bfh;
		memset(&bfh, 0, sizeof(bfh));
		bfh.bfType = 0x4D42;
		bfh.bfSize = sizeof(bfh)+4 * width*height + sizeof(BITMAPFILEHEADER);
		bfh.bfOffBits = sizeof(BITMAPINFOHEADER)+sizeof(BITMAPFILEHEADER);
		DWORD dwWritten = 0;
		BITMAPINFOHEADER bih;
		memset(&bih, 0, sizeof(bih));
		bih.biSize = sizeof(bih);
		bih.biWidth = width;
		bih.biHeight = -height;
		bih.biPlanes = 1;
		bih.biBitCount = 32;

		BITMAPINFO bitmapInfo;
		memset((void *)&bitmapInfo, 0, sizeof(BITMAPINFO));
		bitmapInfo.bmiHeader = bih;
		void* pDest;
		HDC dcRes = CreateCompatibleDC(GetDC(hWnd_));
		bitmap = CreateDIBSection(dcRes, &bitmapInfo, DIB_PAL_COLORS, (void **)&pDest,
			NULL, 0);
		if (pDest == NULL)
		{
			dcRes = NULL;
			goto Exit;
		}
		memcpy(pDest, pData, width*height * 4);
		m_hOldBitmap = (HBITMAP)SelectObject(dcRes, bitmap);
		this->m_ptr_bitmap_buffer = pDest;
Exit:
		return dcRes;
	}
	// Plugin window procedure.
	// static
	LRESULT CALLBACK CNdCefOSRWindow::WndProc(HWND hWnd, UINT message,
		WPARAM wParam, LPARAM lParam) {
			static POINT lastMousePos, curMousePos;
			static bool mouseRotation = false;
			static bool mouseTracking = false;

			static int lastClickX = 0;
			static int lastClickY = 0;
			static CefBrowserHost::MouseButtonType lastClickButton = MBT_LEFT;
			static int gLastClickCount = 0;
			static double gLastClickTime = 0;

			static bool gLastMouseDownOnView = false;

			CNdCefOSRWindow* window =
				reinterpret_cast<CNdCefOSRWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
			CefRefPtr<CefBrowserHost> browser;
			if (window && window->browser_provider_->GetBrowser().get())
				browser = window->browser_provider_->GetBrowser()->GetHost();

			LONG currentTime = 0;
			bool cancelPreviousClick = false;

			if (message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN ||
				message == WM_MBUTTONDOWN || message == WM_MOUSEMOVE ||
				message == WM_MOUSELEAVE) {
					currentTime = GetMessageTime();
					int x = GET_X_LPARAM(lParam);
					int y = GET_Y_LPARAM(lParam);
					cancelPreviousClick =
						(abs(lastClickX - x) > (GetSystemMetrics(SM_CXDOUBLECLK) / 2))
						|| (abs(lastClickY - y) > (GetSystemMetrics(SM_CYDOUBLECLK) / 2))
						|| ((currentTime - gLastClickTime) > GetDoubleClickTime());
					if (cancelPreviousClick &&
						(message == WM_MOUSEMOVE || message == WM_MOUSELEAVE)) {
							gLastClickCount = 0;
							lastClickX = 0;
							lastClickY = 0;
							gLastClickTime = 0;
					}
			}

			switch (message)
			{
			case WM_CREATE:
				{
					LPCREATESTRUCT pN = (LPCREATESTRUCT)lParam;
					CNdCefOSRWindow* self = reinterpret_cast<CNdCefOSRWindow*>(pN->lpCreateParams);
					DWORD dwExStyle = ::GetWindowLong(hWnd, GWL_EXSTYLE);
					if ((dwExStyle & 0x80000) != 0x80000)
					{
						if (self->m_transparent){
							::SetWindowLong(hWnd, GWL_EXSTYLE, dwExStyle ^ 0x80000 );
							::SetWindowLong(hWnd, GWL_EXSTYLE, dwExStyle ^ 0x80000 |WS_EX_TOOLWINDOW);
						}
					}
				}
				break;

			case WM_DESTROY:
				if (window)
					window->OnDestroyed();
				return 0;
	
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_MBUTTONDOWN: {
				SetCapture(hWnd);
				SetFocus(hWnd);
				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);
				if (wParam & MK_SHIFT) {
					// Start rotation effect.
					lastMousePos.x = curMousePos.x = x;
					lastMousePos.y = curMousePos.y = y;
					mouseRotation = true;
				} else {
					CefBrowserHost::MouseButtonType btnType =
						(message == WM_LBUTTONDOWN ? MBT_LEFT : (
						message == WM_RBUTTONDOWN ? MBT_RIGHT : MBT_MIDDLE));
					if (!cancelPreviousClick && (btnType == lastClickButton)) {
						++gLastClickCount;
					} else {
						gLastClickCount = 1;
						lastClickX = x;
						lastClickY = y;
					}
					gLastClickTime = currentTime;
					lastClickButton = btnType;

					if (browser.get()) {
						CefMouseEvent mouse_event;
						mouse_event.x = x;
						mouse_event.y = y;
						mouse_event.modifiers = GetCefMouseModifiers(wParam);
						browser->SendMouseClickEvent(mouse_event, btnType, false,
							gLastClickCount);
					}
				}
				break;
								 }

			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
			case WM_MBUTTONUP:
				if (GetCapture() == hWnd)
					ReleaseCapture();
				if (mouseRotation) {
					// End rotation effect.
					mouseRotation = false;
					window->Invalidate();
				} else {
					int x = GET_X_LPARAM(lParam);
					int y = GET_Y_LPARAM(lParam);
					CefBrowserHost::MouseButtonType btnType =
						(message == WM_LBUTTONUP ? MBT_LEFT : (
						message == WM_RBUTTONUP ? MBT_RIGHT : MBT_MIDDLE));
					if (browser.get()) {
						CefMouseEvent mouse_event;
						mouse_event.x = x;
						mouse_event.y = y;
						mouse_event.modifiers = GetCefMouseModifiers(wParam);
						browser->SendMouseClickEvent(mouse_event, btnType, true,
							gLastClickCount);
					}
				}
				break;

			case WM_MOUSEMOVE: {
				SetCursor (LoadCursor (NULL, IDC_ARROW));
				ShowCursor(true);
				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);
				if (mouseRotation) {
					// Apply rotation effect.
					curMousePos.x = x;
					curMousePos.y = y;
					lastMousePos.x = curMousePos.x;
					lastMousePos.y = curMousePos.y;
					window->Invalidate();
				} else {
					if (!mouseTracking) {
						// Start tracking mouse leave. Required for the WM_MOUSELEAVE event to
						// be generated.
						TRACKMOUSEEVENT tme;
						tme.cbSize = sizeof(TRACKMOUSEEVENT);
						tme.dwFlags = TME_LEAVE;
						tme.hwndTrack = hWnd;
						TrackMouseEvent(&tme);
						mouseTracking = true;
					}
					if (browser.get()) {
						CefMouseEvent mouse_event;
						mouse_event.x = x;
						mouse_event.y = y;
						mouse_event.modifiers = GetCefMouseModifiers(wParam);
						browser->SendMouseMoveEvent(mouse_event, false);
					}
				}
				break;
							   }

			case WM_MOUSELEAVE:
				if (mouseTracking) {
					// Stop tracking mouse leave.
					TRACKMOUSEEVENT tme;
					tme.cbSize = sizeof(TRACKMOUSEEVENT);
					tme.dwFlags = TME_LEAVE & TME_CANCEL;
					tme.hwndTrack = hWnd;
					TrackMouseEvent(&tme);
					mouseTracking = false;
				}
				if (browser.get()) {
					// Determine the cursor position in screen coordinates.
					POINT p;
					::GetCursorPos(&p);
					::ScreenToClient(hWnd, &p);

					CefMouseEvent mouse_event;
					mouse_event.x = p.x;
					mouse_event.y = p.y;
					mouse_event.modifiers = GetCefMouseModifiers(wParam);
					browser->SendMouseMoveEvent(mouse_event, true);
				}
				break;

			case WM_MOUSEWHEEL:
				if (browser.get()) {
					POINT screen_point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
					HWND scrolled_wnd = ::WindowFromPoint(screen_point);
					if (scrolled_wnd != hWnd) {
						break;
					}
					ScreenToClient(hWnd, &screen_point);
					int delta = GET_WHEEL_DELTA_WPARAM(wParam);

					CefMouseEvent mouse_event;
					mouse_event.x = screen_point.x;
					mouse_event.y = screen_point.y;
					mouse_event.modifiers = GetCefMouseModifiers(wParam);

					browser->SendMouseWheelEvent(mouse_event,
						IsKeyDown(VK_SHIFT) ? delta : 0,
						!IsKeyDown(VK_SHIFT) ? delta : 0);
				}
				break;

			case WM_SIZE:
				if (browser.get())
					browser->WasResized();
				break;

			case WM_SETFOCUS:
			case WM_KILLFOCUS:
				if (browser.get())
					browser->SendFocusEvent(message == WM_SETFOCUS);
				break;

			case WM_CAPTURECHANGED:
			case WM_CANCELMODE:
				if (!mouseRotation) {
					if (browser.get())
						browser->SendCaptureLostEvent();
				}
				break;
			case WM_SYSCHAR:
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
			case WM_KEYDOWN:
			case WM_KEYUP:
			case WM_CHAR: 
				{
					CefKeyEvent event;
					event.windows_key_code = wParam;
					event.native_key_code = lParam;
					event.is_system_key = message == WM_SYSCHAR ||
						message == WM_SYSKEYDOWN ||
						message == WM_SYSKEYUP;

					if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN)
						event.type = KEYEVENT_RAWKEYDOWN;
					else if (message == WM_KEYUP || message == WM_SYSKEYUP)
						event.type = KEYEVENT_KEYUP;
					else
						event.type = KEYEVENT_CHAR;
					event.modifiers = GetCefKeyboardModifiers(wParam, lParam);
					if (wParam==VK_ESCAPE){
						if (window&&browser.get()){
							if (!window->IsClosed()) {
								SetCursor (LoadCursor (NULL, IDC_ARROW));
								g_hNdCefHandle->SendMsgToMainProcessByWND(CNdCefHandler::GetPPTMainWnd(),MSG_CEFFOREGROUDWND,0,NULL);
								window->SetClosed();
								browser->CloseBrowser(false);
								HANDLE hself = GetCurrentProcess();   
								TerminateProcess(hself, 0); 
							}
						}
					}else{
						if( event.type == KEYEVENT_RAWKEYDOWN )
						{
							if (g_hNdCefHandle.get()) {
								if (wParam == VK_LEFT ||wParam == VK_UP ){
									g_hNdCefHandle->SendMsgToMainProcessByWND(CNdCefHandler::GetPPTMainWnd(),MSG_PREV,0,NULL);
									if (window&&browser.get()){
										if (!window->IsClosed()) {
											g_hNdCefHandle->SendMsgToMainProcessByWND(CNdCefHandler::GetPPTMainWnd(),MSG_CEFFOREGROUDWND,0,NULL);
											window->SetClosed();
											browser->CloseBrowser(false);
											HANDLE hself = GetCurrentProcess();   
											TerminateProcess(hself, 0); 
										}
									}
									return true;
								}else if (wParam== VK_RIGHT||wParam == VK_DOWN){
									g_hNdCefHandle->SendMsgToMainProcessByWND(CNdCefHandler::GetPPTMainWnd(),MSG_NEXT,0,NULL);
									if (window&&browser.get()){
										if (!window->IsClosed()) {
											g_hNdCefHandle->SendMsgToMainProcessByWND(CNdCefHandler::GetPPTMainWnd(),MSG_CEFFOREGROUDWND,0,NULL);
											//g_hNdCefHandle->SendMsgToMainProcessByWND(CNdCefHandler::GetPPTMainWnd(),MSG_NEXT,0,NULL);
											window->SetClosed();
											browser->CloseBrowser(false);
											HANDLE hself = GetCurrentProcess();   
											TerminateProcess(hself, 0); 
										}
									}
									return true;
								}else{
									if (browser.get())
										browser->SendKeyEvent(event);
								}
							}else{
							if (browser.get())
								browser->SendKeyEvent(event);
							}
						}else{
							if (browser.get())
								browser->SendKeyEvent(event);
						}
					}
					break;
				}

			case WM_PAINT: 
				{
					if (window)
						window->PaintWindow(hWnd);
					return 0;
				}

			case WM_ERASEBKGND:
				return 0;
			}

			return DefWindowProc(hWnd, message, wParam, lParam);
	}

}  

