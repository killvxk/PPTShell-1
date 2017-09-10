#pragma once

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include "include/cef_app.h"

namespace NdCef {

	class CNdCefApp : public CefApp,
		public CefBrowserProcessHandler,
		public CefRenderProcessHandler {
	public:
		class BrowserDelegate : public virtual CefBase {
		public:
			virtual void OnContextInitialized(CefRefPtr<CNdCefApp> app) {}

			virtual void OnBeforeChildProcessLaunch(
				CefRefPtr<CNdCefApp> app,
				CefRefPtr<CefCommandLine> command_line) {}

			virtual void OnRenderProcessThreadCreated(
				CefRefPtr<CNdCefApp> app,
				CefRefPtr<CefListValue> extra_info) {}
		};

		typedef std::set<CefRefPtr<BrowserDelegate> > BrowserDelegateSet;

		class RenderDelegate : public virtual CefBase {
		public:
			virtual void OnRenderThreadCreated(CefRefPtr<CNdCefApp> app,
				CefRefPtr<CefListValue> extra_info) {}

			virtual void OnWebKitInitialized(CefRefPtr<CNdCefApp> app) {}

			virtual void OnBrowserCreated(CefRefPtr<CNdCefApp> app,
				CefRefPtr<CefBrowser> browser) {}

			virtual void OnBrowserDestroyed(CefRefPtr<CNdCefApp> app,
				CefRefPtr<CefBrowser> browser) {}

			virtual CefRefPtr<CefLoadHandler> GetLoadHandler(CefRefPtr<CNdCefApp> app) {
				return NULL;
			}

			virtual bool OnBeforeNavigation(CefRefPtr<CNdCefApp> app,
				CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefRequest> request,
				cef_navigation_type_t navigation_type,
				bool is_redirect) {
					return false;
			}

			virtual void OnContextCreated(CefRefPtr<CNdCefApp> app,
				CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefV8Context> context) {}

			virtual void OnContextReleased(CefRefPtr<CNdCefApp> app,
				CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefV8Context> context) {}

			virtual void OnUncaughtException(CefRefPtr<CNdCefApp> app,
				CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefV8Context> context,
				CefRefPtr<CefV8Exception> exception,
				CefRefPtr<CefV8StackTrace> stackTrace) {}

			virtual void OnFocusedNodeChanged(CefRefPtr<CNdCefApp> app,
				CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefDOMNode> node) {}

			virtual bool OnProcessMessageReceived(
				CefRefPtr<CNdCefApp> app,
				CefRefPtr<CefBrowser> browser,
				CefProcessId source_process,
				CefRefPtr<CefProcessMessage> message) {
					return false;
			}
		};

		typedef std::set<CefRefPtr<RenderDelegate> > RenderDelegateSet;

		CNdCefApp();
		bool GetRegisterJs();
		void SetRegisterJs(bool nReg);
	private:
		static void CreateBrowserDelegates(BrowserDelegateSet& delegates);

		static void CreateRenderDelegates(RenderDelegateSet& delegates);

		static void RegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar,
			std::vector<CefString>& cookiable_schemes);

		static CefRefPtr<CefPrintHandler> CreatePrintHandler();

		void OnRegisterCustomSchemes(
			CefRefPtr<CefSchemeRegistrar> registrar) OVERRIDE;
		CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE {
			return this;
		}
		CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE {
			return this;
		}

		void OnContextInitialized() OVERRIDE;
		void OnBeforeChildProcessLaunch(
			CefRefPtr<CefCommandLine> command_line) OVERRIDE;
		void OnRenderProcessThreadCreated(
			CefRefPtr<CefListValue> extra_info) OVERRIDE;
		CefRefPtr<CefPrintHandler> GetPrintHandler() OVERRIDE {
			return print_handler_;
		}
		void OnBeforeCommandLineProcessing(const CefString& process_type,
			CefRefPtr<CefCommandLine> command_line) OVERRIDE;
		void OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) OVERRIDE;
		void OnWebKitInitialized() OVERRIDE;
		void OnBrowserCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
		void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) OVERRIDE;
		CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE;
		bool OnBeforeNavigation(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefRequest> request,
			NavigationType navigation_type,
			bool is_redirect) OVERRIDE;
		void OnContextCreated(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefV8Context> context) OVERRIDE;
		void OnContextReleased(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefV8Context> context) OVERRIDE;
		void OnUncaughtException(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefV8Context> context,
			CefRefPtr<CefV8Exception> exception,
			CefRefPtr<CefV8StackTrace> stackTrace) OVERRIDE;
		void OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefDOMNode> node) OVERRIDE;
		bool OnProcessMessageReceived(
			CefRefPtr<CefBrowser> browser,
			CefProcessId source_process,
			CefRefPtr<CefProcessMessage> message) OVERRIDE;

		BrowserDelegateSet browser_delegates_;

		RenderDelegateSet render_delegates_;

		std::vector<CefString> cookieable_schemes_;

		CefRefPtr<CefPrintHandler> print_handler_;

		bool m_bRegisterJs;

		IMPLEMENT_REFCOUNTING(CNdCefApp);
	};
} 
