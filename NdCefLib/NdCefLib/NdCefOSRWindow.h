#pragma once

#include "include/cef_render_handler.h"
#include "NdCefHandler.h"
#include "NdCefRenderer.h"

namespace NdCef {

	class OSRBrowserProvider {
	public:
		virtual CefRefPtr<CefBrowser> GetBrowser() =0;

	protected:
		virtual ~OSRBrowserProvider() {}
	};

	class CNdCefOSRWindow : public CNdCefHandler::RenderHandler
	{
	public:
		static CefRefPtr<CNdCefOSRWindow> Create(OSRBrowserProvider* browser_provider,
			bool transparent,
			bool show_update_rect);

		static CefRefPtr<CNdCefOSRWindow> From(
			CefRefPtr<CNdCefHandler::RenderHandler> renderHandler);

		bool CreateWidget(HWND hWndParent, const RECT& rect,
			HINSTANCE hInst, LPCTSTR className,std::wstring nUrl);

		// Destroy the underlying window.
		void DestroyWidget();

		HWND hwnd() const {
			return hWnd_;
		}

		virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> browser,
			const CefString& message,
			const CefString& source,
			int line) OVERRIDE; 
		void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

		// CefRenderHandler methods
		bool GetRootScreenRect(CefRefPtr<CefBrowser> browser,
			CefRect& rect) OVERRIDE;
		bool GetViewRect(CefRefPtr<CefBrowser> browser,
			CefRect& rect) OVERRIDE;
		bool GetScreenPoint(CefRefPtr<CefBrowser> browser,
			int viewX,
			int viewY,
			int& screenX,
			int& screenY) OVERRIDE;
		void OnPopupShow(CefRefPtr<CefBrowser> browser,
			bool show) OVERRIDE;
		void OnPopupSize(CefRefPtr<CefBrowser> browser,
			const CefRect& rect) OVERRIDE;
		void OnPaint(CefRefPtr<CefBrowser> browser,
			PaintElementType type,
			const RectList& dirtyRects,
			const void* buffer,
			int width,
			int height) OVERRIDE;
		void OnCursorChange(CefRefPtr<CefBrowser> browser,
			CefCursorHandle cursor,
			CursorType type,
			const CefCursorInfo& custom_cursor_info) OVERRIDE;
		bool StartDragging(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefDragData> drag_data,
			CefRenderHandler::DragOperationsMask allowed_ops,
			int x, int y) OVERRIDE;
		void UpdateDragCursor(
			CefRefPtr<CefBrowser> browser,
			CefRenderHandler::DragOperation operation) OVERRIDE;

		void Invalidate();
		void WasHidden(bool hidden);
		void SetThumbImgPath(std::wstring nPath);
		bool IsClosed();
		void SetClosed();
	private:
		HDC CreateBitmapFromData(const void* pData, int width, int height, HBITMAP& bitmap);
		void PaintWindow(HWND hWnd);	
		void DrawTransparent( bool bWmPaint);
		void DrawNormal(bool bWmPaint);
		void DrawNormalPartial(const RectList& list, bool bWmPaint);
		HDC m_dc_buffer;
		
		std:: wstring m_thumbImgPath;
		HBITMAP m_hOldBitmap;
		HBITMAP m_bitmap_buffer;
		void* m_ptr_bitmap_buffer;
		int m_pre_width;
		int m_pre_height;
		bool m_transparent;
		bool m_close;
	private:
		CNdCefOSRWindow(OSRBrowserProvider* browser_provider,
			bool transparent,
			bool show_update_rect);
		~CNdCefOSRWindow();

		void OnDestroyed();
		static ATOM RegisterOSRClass(HINSTANCE hInstance, LPCTSTR className);
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam,
			LPARAM lParam);

		OSRBrowserProvider* browser_provider_;
		HWND hWnd_;
		HDC hDC_;
		HGLRC hRC_;
		bool painting_popup_;
		bool hidden_;

		IMPLEMENT_REFCOUNTING(CNdCefOSRWindow);
	};

}
