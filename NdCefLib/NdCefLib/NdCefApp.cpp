#include "stdafx.h"
#include "NdCefApp.h"

#include <string>

#include "include/cef_cookie.h"
#include "include/cef_process_message.h"
#include "include/cef_task.h"
#include "include/cef_v8.h"
#include "include/wrapper/cef_helpers.h"
#include "NdCefRenderer.h"
#include "NdCefRegisterJs.h"


namespace NdCef {

	CNdCefApp::CNdCefApp() {
		m_bRegisterJs = true;
		OutputDebugString(L"CNdCefApp::CNdCefApp\n"); 
	}

	void CNdCefApp::OnRegisterCustomSchemes(
		CefRefPtr<CefSchemeRegistrar> registrar) {
			cookieable_schemes_.push_back("http");
			cookieable_schemes_.push_back("https");

			RegisterCustomSchemes(registrar, cookieable_schemes_);
	}

	void CNdCefApp::OnBeforeCommandLineProcessing(
		const CefString& process_type,
		CefRefPtr<CefCommandLine> command_line) {
		//	command_line->AppendArgument("--enable-webgl");
		//	command_line->AppendArgument("--ignore-gpu-blacklist");
		//	command_line->
		//command_line->AppendSwitch();
		//"enable-media-stream", "enable-media-stream"
	}

	void CNdCefApp::OnContextInitialized() {
		OutputDebugString(L"CreateBrowserDelegates\n"); 
		CreateBrowserDelegates(browser_delegates_);

		CefRefPtr<CefCookieManager> manager = CefCookieManager::GetGlobalManager();
		DCHECK(manager.get());
		manager->SetSupportedSchemes(cookieable_schemes_);

		print_handler_ = CreatePrintHandler();

		BrowserDelegateSet::iterator it = browser_delegates_.begin();
		for (; it != browser_delegates_.end(); ++it)
			(*it)->OnContextInitialized(this);
	}

	void CNdCefApp::OnBeforeChildProcessLaunch(
		CefRefPtr<CefCommandLine> command_line) {
			BrowserDelegateSet::iterator it = browser_delegates_.begin();
			for (; it != browser_delegates_.end(); ++it)
				(*it)->OnBeforeChildProcessLaunch(this, command_line);
	}

	void CNdCefApp::OnRenderProcessThreadCreated(
		CefRefPtr<CefListValue> extra_info) {
			BrowserDelegateSet::iterator it = browser_delegates_.begin();
			for (; it != browser_delegates_.end(); ++it)
				(*it)->OnRenderProcessThreadCreated(this, extra_info);
	}

	void CNdCefApp::OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) {
		OutputDebugString(L"CNdCefApp::OnRenderThreadCreated\n"); 
		CreateRenderDelegates(render_delegates_);

		RenderDelegateSet::iterator it = render_delegates_.begin();
		for (; it != render_delegates_.end(); ++it){
			(*it)->OnRenderThreadCreated(this, extra_info);
		}

		OutputDebugString(L"CNdCefApp::OnRenderThreadCreated  OK\n"); 
	}

	void CNdCefApp::OnWebKitInitialized() {
		RenderDelegateSet::iterator it = render_delegates_.begin();
		for (; it != render_delegates_.end(); ++it)
			(*it)->OnWebKitInitialized(this);
	}

	void CNdCefApp::OnBrowserCreated(CefRefPtr<CefBrowser> browser) {
		RenderDelegateSet::iterator it = render_delegates_.begin();
		for (; it != render_delegates_.end(); ++it)
			(*it)->OnBrowserCreated(this, browser);
	}

	void CNdCefApp::OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) {
		RenderDelegateSet::iterator it = render_delegates_.begin();
		for (; it != render_delegates_.end(); ++it)
			(*it)->OnBrowserDestroyed(this, browser);
	}

	CefRefPtr<CefLoadHandler> CNdCefApp::GetLoadHandler() {
		CefRefPtr<CefLoadHandler> load_handler;
		RenderDelegateSet::iterator it = render_delegates_.begin();
		for (; it != render_delegates_.end() && !load_handler.get(); ++it)
			load_handler = (*it)->GetLoadHandler(this);

		return load_handler;
	}

	bool CNdCefApp::OnBeforeNavigation(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		NavigationType navigation_type,
		bool is_redirect) {
			RenderDelegateSet::iterator it = render_delegates_.begin();
			for (; it != render_delegates_.end(); ++it) {
				if ((*it)->OnBeforeNavigation(this, browser, frame, request,
					navigation_type, is_redirect)) {
						return true;
				}
			}

			return false;
	}

	void CNdCefApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context) {
			RenderDelegateSet::iterator it = render_delegates_.begin();
			for (; it != render_delegates_.end(); ++it)
				(*it)->OnContextCreated(this, browser, frame, context);
	}

	void CNdCefApp::OnContextReleased(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context) {
			RenderDelegateSet::iterator it = render_delegates_.begin();
			for (; it != render_delegates_.end(); ++it)
				(*it)->OnContextReleased(this, browser, frame, context);
	}

	void CNdCefApp::OnUncaughtException(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context,
		CefRefPtr<CefV8Exception> exception,
		CefRefPtr<CefV8StackTrace> stackTrace) {
			RenderDelegateSet::iterator it = render_delegates_.begin();
			for (; it != render_delegates_.end(); ++it) {
				(*it)->OnUncaughtException(this, browser, frame, context, exception,
					stackTrace);
			}
	}

	void CNdCefApp::OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefDOMNode> node) {
			RenderDelegateSet::iterator it = render_delegates_.begin();
			for (; it != render_delegates_.end(); ++it)
				(*it)->OnFocusedNodeChanged(this, browser, frame, node);
	}

	bool CNdCefApp::OnProcessMessageReceived(
		CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message) {
			DCHECK_EQ(source_process, PID_BROWSER);

			bool handled = false;

			std::wstring message_name = message->GetName();
			if (message_name==L"NoRegisterJS"){
				SetRegisterJs(false);
			}

			RenderDelegateSet::iterator it = render_delegates_.begin();
			for (; it != render_delegates_.end() && !handled; ++it) {
				handled = (*it)->OnProcessMessageReceived(this, browser, source_process,
					message);
			}

			return handled;
	}

	void CNdCefApp::CreateBrowserDelegates(BrowserDelegateSet& delegates) {
	}

	void CNdCefApp::CreateRenderDelegates(RenderDelegateSet& delegates) {
		OutputDebugString(L"CNdCefApp::CreateRenderDelegates\n"); 
		NdCefRenderer::CreateRenderDelegates(delegates);
		NdCefRegisterJs::CreateRenderDelegates(delegates);
		OutputDebugString(L"CNdCefApp::CreateRenderDelegates  OK\n"); 
	}

	void CNdCefApp::RegisterCustomSchemes(
		CefRefPtr<CefSchemeRegistrar> registrar,
		std::vector<CefString>& cookiable_schemes) {
			//scheme_test::RegisterCustomSchemes(registrar, cookiable_schemes);
	}

	CefRefPtr<CefPrintHandler> CNdCefApp::CreatePrintHandler() {
		return NULL;
	}

	bool CNdCefApp::GetRegisterJs(){
		return m_bRegisterJs;
	}
	void CNdCefApp::SetRegisterJs(bool nReg){
		m_bRegisterJs = nReg;
	}
} 