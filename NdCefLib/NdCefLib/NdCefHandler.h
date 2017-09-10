#pragma once

#include <list>
#include <map>
#include <set>
#include <unordered_set>
#include <string>

#include "include/base/cef_lock.h"
#include "include/cef_client.h"
#include "include/wrapper/cef_helpers.h"
#include "include/wrapper/cef_message_router.h"
#include "NdCefIRegisterJs.h"
#include "NdDebug.h"

#define  CNdCefWindowHandle CefWindowHandle
#define MSG_NEXT							5
#define MSG_PREV							6
#define MSG_ESC                             7
#define MSG_CEFINFO                         8
#define MSG_OBTAIN_ONLINE_STUDENTS          9
#define MSG_CEF_ICRINVOKENATIVE             10
#define MSG_DRAWMETHOD						11
#define MSG_CEFNEXT							12
#define MSG_CEFPREV							13
#define MSG_CEFFOREGROUDWND			    	14
#define MSG_URLCHANGE                       30001
#define MSG_URLFAILED                       30002
#define MSG_URLPOPUP                        30003
#define MSG_PLAYERLOADED                    30004
#define MSG_QUESTION_ADD					1002
#define MSG_QUESTION_SAVED					1003
#define MSG_QUESTION_ADDSAVED				1005
#define	MSG_CEFOSR_ESC						1008

namespace NdCef {

	static void SetFocusToBrowser(CefRefPtr<CefBrowser> browser);

	typedef std::set<CefMessageRouterBrowserSide::Handler*> MessageHandlerSet;
	typedef std::tr1::unordered_set<std::string> CallbackSet;
	class CallbackKey {
	public:
		std::string key;
		int browerid;
		bool operator < ( const  CallbackKey& nCallbackKey) const  {
			if (browerid<nCallbackKey.browerid){
				return true;
			}else if (browerid==nCallbackKey.browerid){
				if (key<nCallbackKey.key){
					return true;
				}
			}
			return false;
		}
	};
	typedef std::map<CallbackKey, CallbackSet *> CallbackMap;

	class Handler : public CefMessageRouterBrowserSide::Handler {
	public:
		Handler() {}
		virtual bool OnQuery(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			int64 query_id,
			const CefString& request,
			bool persistent,
			CefRefPtr<Callback> callback) OVERRIDE {
				const std::string& url = frame->GetURL();
				CNdDebug::printf("%s%s","OnQuery:",url);
				return false;
		}
	};

	class CNdCefHandler : public CefClient,
		public CefContextMenuHandler,
		public CefDisplayHandler,
		public CefDownloadHandler,
		public CefDragHandler,
		public CefGeolocationHandler,
		public CefKeyboardHandler,
		public CefLifeSpanHandler,
		public CefLoadHandler,
		public CefRenderHandler,
		public CefRequestHandler {
	public:
		class RenderHandler : public CefRenderHandler 
		{
		public:
			virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) =0;
			virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> browser,
				const CefString& message,
				const CefString& source,
				int line) = 0; 
		};


		CNdCefHandler();
		~CNdCefHandler();

		CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() OVERRIDE {
			return this;
		}
		CefRefPtr<CefDialogHandler> GetDialogHandler() OVERRIDE {
			return dialog_handler_;
		}
		CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE {
			return this;
		}
		CefRefPtr<CefDownloadHandler> GetDownloadHandler() OVERRIDE {
			return this;
		}
		CefRefPtr<CefDragHandler> GetDragHandler() OVERRIDE {
			return this;
		}
		CefRefPtr<CefGeolocationHandler> GetGeolocationHandler() OVERRIDE {
			return this;
		}
		CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() OVERRIDE {
			return jsdialog_handler_;
		}
		CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() OVERRIDE {
			return this;
		}
		CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE {
			return this;
		}
		CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE {
			return this;
		}
		CefRefPtr<CefRenderHandler> GetRenderHandler() OVERRIDE {
			return this;
		}
		CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE {
			return this;
		}
		bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
			CefProcessId source_process,
			CefRefPtr<CefProcessMessage> message) OVERRIDE;

		void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefContextMenuParams> params,
			CefRefPtr<CefMenuModel> model) OVERRIDE;
		bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefContextMenuParams> params,
			int command_id,
			EventFlags event_flags) OVERRIDE;

		void OnAddressChange(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			const CefString& url) OVERRIDE;
		void OnTitleChange(CefRefPtr<CefBrowser> browser,
			const CefString& title) OVERRIDE;
		bool OnConsoleMessage(CefRefPtr<CefBrowser> browser,
			const CefString& message,
			const CefString& source,
			int line) OVERRIDE;

		void OnBeforeDownload(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefDownloadItem> download_item,
			const CefString& suggested_name,
			CefRefPtr<CefBeforeDownloadCallback> callback) OVERRIDE;
		void OnDownloadUpdated(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefDownloadItem> download_item,
			CefRefPtr<CefDownloadItemCallback> callback) OVERRIDE;

		bool OnDragEnter(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefDragData> dragData,
			CefDragHandler::DragOperationsMask mask) OVERRIDE;

		bool OnRequestGeolocationPermission(
			CefRefPtr<CefBrowser> browser,
			const CefString& requesting_url,
			int request_id,
			CefRefPtr<CefGeolocationCallback> callback) OVERRIDE;

		bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
			const CefKeyEvent& event,
			CefEventHandle os_event,
			bool* is_keyboard_shortcut) OVERRIDE;

		bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			const CefString& target_url,
			const CefString& target_frame_name,
			const CefPopupFeatures& popupFeatures,
			CefWindowInfo& windowInfo,
			CefRefPtr<CefClient>& client,
			CefBrowserSettings& settings,
			bool* no_javascript_access) OVERRIDE;
		void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
		bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
		void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

		void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
			bool isLoading,
			bool canGoBack,
			bool canGoForward) OVERRIDE;
		void OnLoadError(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			ErrorCode errorCode,
			const CefString& errorText,
			const CefString& failedUrl) OVERRIDE;

		bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefRequest> request,
			bool is_redirect) OVERRIDE;
		CefRefPtr<CefResourceHandler> GetResourceHandler(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefRequest> request) OVERRIDE;
		bool OnQuotaRequest(CefRefPtr<CefBrowser> browser,
			const CefString& origin_url,
			int64 new_size,
			CefRefPtr<CefRequestCallback> callback) OVERRIDE;
		void OnProtocolExecution(CefRefPtr<CefBrowser> browser,
			const CefString& url,
			bool& allow_os_execution) OVERRIDE;
		bool OnCertificateError(
			CefRefPtr<CefBrowser> browser,
			ErrorCode cert_error,
			const CefString& request_url,
			CefRefPtr<CefSSLInfo> ssl_info,
			CefRefPtr<CefRequestCallback> callback) OVERRIDE;
		void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
			TerminationStatus status) OVERRIDE;

		bool GetRootScreenRect(CefRefPtr<CefBrowser> browser,
			CefRect& rect) OVERRIDE;
		bool GetViewRect(CefRefPtr<CefBrowser> browser,
			CefRect& rect) OVERRIDE;
		bool GetScreenPoint(CefRefPtr<CefBrowser> browser,
			int viewX,
			int viewY,
			int& screenX,
			int& screenY) OVERRIDE;
		bool GetScreenInfo(CefRefPtr<CefBrowser> browser,
			CefScreenInfo& screen_info) OVERRIDE;
		void OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) OVERRIDE;
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
		void UpdateDragCursor(CefRefPtr<CefBrowser> browser,
			CefRenderHandler::DragOperation operation) OVERRIDE;

		void SetMainWindowHandle(CNdCefWindowHandle handle);
		CNdCefWindowHandle GetMainWindowHandle() const;

		void SetOSRHandler(CefRefPtr<RenderHandler> handler);
		CefRefPtr<RenderHandler> GetOSRHandler() const;

		void CloseAllBrowsers(bool force_close);

		bool IsClosing() const;

		void ShowDevTools(CefRefPtr<CefBrowser> browser,
			const CefPoint& inspect_element_at);
		void CloseDevTools(CefRefPtr<CefBrowser> browser);

		void SendMsgToMainProcessByWND(HWND hwMain,int dwdata,DWORD size,LPVOID ndata);
		bool Save(const std::string& path, const std::string& data);
		bool ExecuteJS(HWND hParent,LPCTSTR nJSCode);
		bool ExecuteJSCallBack(HWND hParent,LPCTSTR eventName,LPCTSTR nParam);
		bool ExecuteJavaScript(CefRefPtr<CefBrowser> nBrowser,LPCTSTR eventName,LPCTSTR json);
		bool RegisterJS(NdCefIRegisterJs*);
		bool ChangeUrl(HWND hParentWnd,std::wstring nUrl);
		bool CloseByWnd(HWND hParentWnd);
		CefRefPtr<CefBrowser> FindBrowser(HWND hParent);
		bool FindBrowserWnd(HWND hParent);
		std::wstring FindBrowserUrl(HWND hParent);
		int GetBrowserSize();
		void SetFocusToBrowser(CefRefPtr<CefBrowser> browser);
		void SetBrowserBlank(HWND hParent);
		CefRefPtr<CefBrowser> GetBrowser();
		static void SetPPTMainWnd(HWND hPPTMain);
		static HWND GetPPTMainWnd();
		void SetPreloadUrl(CefString strUrl);
		CefString GetPreloadUrl();
		void SetPlayWindowClose(bool nClose);
		bool GetPlayWindowClose();
		bool GetOcxWindow();
		void SetOcxWindow(bool nocx);
		void BrowserSizeChange(CefRefPtr<CefBrowser> browser);
		bool SetBrowserSizeChange(HWND hParentWnd);
		void ClearCookie(std::wstring nUrl,std::wstring nCookieName);
		void SetZoomed();
		void SetCefLib();
		static bool GetRegisterJs();
		static void SetRegisterJs(bool nReg);
		void SetPopup(HWND hParent,int nPop);
		void SetQuestionType(HWND hParent,int nType);
		void SendEsc2PPT();
	private:
		void SetLoading(bool isLoading);
		void SetNavState(bool canGoBack, bool canGoForward);
		
		void registerListener(int browserid,std::string eventName, std::string callBack);
		void unRegisterListener(int browserid,std::string eventName, std::string callBack);

		void BuildTestMenu(CefRefPtr<CefMenuModel> model);
		bool ExecuteTestMenu(int command_id);
		struct TestMenuState {
			TestMenuState() : check_item(true), radio_item(0) {}
			bool check_item;
			int radio_item;
		} test_menu_state_;

		bool mouse_cursor_change_disabled_;

		CefRefPtr<CefDialogHandler> dialog_handler_;
		CefRefPtr<CefJSDialogHandler> jsdialog_handler_;

		mutable base::Lock lock_;

		bool is_closing_;

		typedef std::list<CefRefPtr<CefBrowser> > BrowserList;
		BrowserList popup_browsers_;
		BrowserList browserslist_;

		CNdCefWindowHandle main_handle_;

		CefRefPtr<RenderHandler> osr_handler_;

		const std::string console_log_file_;
		bool first_console_message_;

		bool focus_on_editable_field_;

		CallbackMap m_CallbackMap;

		CefRefPtr<CefMessageRouterBrowserSide> message_router_;

		MessageHandlerSet message_handler_set_;
		static NdCefIRegisterJs* m_NdCefIRegisterJs; 

		static int browser_count_;

		static HWND m_hPPTMain;
		bool m_bBasicQuestion;
		CefString m_strPreloadUrl;
		bool m_bPlayWindowClose;
		bool m_IsZoomed;
		bool m_ocxWindow;
		bool m_bCefLib;
		static bool m_bRegisterJs;
		bool m_bPopup;
		bool m_bQestionType;
		IMPLEMENT_REFCOUNTING(CNdCefHandler);
	};

} 
