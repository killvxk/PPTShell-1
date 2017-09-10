#include "stdafx.h"
#include "NdCefRenderer.h"

#include <sstream>
#include <string>

#include "include/cef_dom.h"
#include "include/wrapper/cef_helpers.h"
#include "include/wrapper/cef_message_router.h"

namespace NdCef {
	namespace NdCefRenderer {
		const char kFocusedNodeChangedMessage[] = "ClientRenderer.FocusedNodeChanged";
		class NdCefRenderDelegate : public CNdCefApp::RenderDelegate {
		public:
			NdCefRenderDelegate()
				: last_node_is_editable_(false) {
			}

			virtual void OnWebKitInitialized(CefRefPtr<CNdCefApp> app) OVERRIDE {
				CefMessageRouterConfig config;
				message_router_ = CefMessageRouterRendererSide::Create(config);
			}

			virtual void OnContextCreated(CefRefPtr<CNdCefApp> app,
				CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefV8Context> context) OVERRIDE {
					message_router_->OnContextCreated(browser,  frame, context);
			}

			virtual void OnContextReleased(CefRefPtr<CNdCefApp> app,
				CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefV8Context> context) OVERRIDE {
					message_router_->OnContextReleased(browser,  frame, context);
			}

			virtual void OnFocusedNodeChanged(CefRefPtr<CNdCefApp> app,
				CefRefPtr<CefBrowser> browser,
				CefRefPtr<CefFrame> frame,
				CefRefPtr<CefDOMNode> node) OVERRIDE {
					bool is_editable = (node.get() && node->IsEditable());
					if (is_editable != last_node_is_editable_) {
						last_node_is_editable_ = is_editable;
						CefRefPtr<CefProcessMessage> message =
							CefProcessMessage::Create(kFocusedNodeChangedMessage);
						message->GetArgumentList()->SetBool(0, is_editable);
						browser->SendProcessMessage(PID_BROWSER, message);
					}
			}

			virtual bool OnProcessMessageReceived(
				CefRefPtr<CNdCefApp> app,
				CefRefPtr<CefBrowser> browser,
				CefProcessId source_process,
				CefRefPtr<CefProcessMessage> message) OVERRIDE {
					return message_router_->OnProcessMessageReceived(
						browser, source_process, message);
			}

		private:
			bool last_node_is_editable_;

			CefRefPtr<CefMessageRouterRendererSide> message_router_;

			IMPLEMENT_REFCOUNTING(NdCefRenderDelegate);
		};



		void CreateRenderDelegates(CNdCefApp::RenderDelegateSet& delegates) {
			delegates.insert(new NdCefRenderDelegate);
		}
	}  
} 
