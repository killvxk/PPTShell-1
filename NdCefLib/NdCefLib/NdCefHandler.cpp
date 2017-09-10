#include "stdafx.h"
#include <shellapi.h>
#include "NdCefHandler.h"

#include <stdio.h>
#include <algorithm>
#include <iomanip>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "include/base/cef_bind.h"
#include "include/cef_browser.h"
#include "include/cef_frame.h"
#include "include/cef_path_util.h"
#include "include/cef_process_util.h"
#include "include/cef_trace.h"
#include "include/cef_url.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_stream_resource_handler.h"
#include "include/cef_url.h"
#include "NdCefRenderer.h"
#include "NdCefContext.h"
#include "NdCefMessageLoop.h"
#include "NdCefMain.h"
#include "json/json.h"
#include <iphlpapi.h>
#pragma comment(lib,"Iphlpapi.lib") 

#define NEWLINE "\r\n"

enum NdCefMenuids {
	NDCEF_ID_SHOW_DEVTOOLS   = MENU_ID_USER_FIRST,
	NDCEF_ID_CLOSE_DEVTOOLS
};

namespace NdCef {
	std::string GetDataURI(const std::string& data,
		const std::string& mime_type) {
			return "data:" + mime_type + ";base64," +
				CefURIEncode(CefBase64Encode(data.data(), data.size()), false).ToString();
	}

	std::string GetTimeString(const CefTime& value) {
		if (value.GetTimeT() == 0)
			return "Unspecified";

		static const char* kMonths[] = {
			"January", "February", "March", "April", "May", "June", "July", "August",
			"September", "October", "November", "December"
		};
		std::string month;
		if (value.month >= 1 && value.month <= 12)
			month = kMonths[value.month - 1];
		else
			month = "Invalid";

		std::stringstream ss;
		ss << month << " " << value.day_of_month << ", " << value.year << " " <<
			std::setfill('0') << std::setw(2) << value.hour << ":" <<
			std::setfill('0') << std::setw(2) << value.minute << ":" <<
			std::setfill('0') << std::setw(2) << value.second;
		return ss.str();
	}

	std::string GetBinaryString(CefRefPtr<CefBinaryValue> value) {
		if (!value.get())
			return "&nbsp;";

		const size_t size = value->GetSize();
		std::string src;
		src.resize(size);
		value->GetData(const_cast<char*>(src.data()), size, 0);

		return CefBase64Encode(src.data(), src.size());
	}


	void LoadErrorPage(CefRefPtr<CefFrame> frame,
		const std::string& failed_url,
		cef_errorcode_t error_code,
		const std::string& other_info) {
			std::stringstream ss;
			ss << "<html><head><title>Page failed to load</title></head>"
				"<body bgcolor=\"white\">"
				"<h3>Page failed to load.</h3>"
				"URL: <a href=\"" << failed_url << "\">"<< failed_url << "</a>"
				"<br/>Error: " << "UNKNOWN" <<
				" (" << error_code << ")";

			if (!other_info.empty())
				ss << "<br/>" << other_info;

			ss << "</body></html>";
			frame->LoadURL(GetDataURI(ss.str(), "text/html"));
	}


	int CNdCefHandler::browser_count_ = 0;
	HWND CNdCefHandler::m_hPPTMain = NULL;
	bool CNdCefHandler::m_bRegisterJs = true;

	NdCefIRegisterJs* CNdCefHandler::m_NdCefIRegisterJs = NULL;

	CNdCefHandler::CNdCefHandler()
		: is_closing_(false),
		console_log_file_("E://console.log"),//CNdCefContext::Get()->GetConsoleLogPath()
		first_console_message_(true)
	{
		m_bBasicQuestion	= true;
		m_IsZoomed = false;
		m_bPlayWindowClose = false;
		m_ocxWindow = false;
		m_bCefLib = false;
		m_bPopup = 0;
		m_bQestionType = 0;
		CNdDebug::printfStr("CNdCefHandler");
		DCHECK(!console_log_file_.empty());
	}

	CNdCefHandler::~CNdCefHandler() {
		CNdDebug::printfStr("~CNdCefHandler");
	}
	void CNdCefHandler::SetPPTMainWnd(HWND hPPTMain){
		m_hPPTMain = hPPTMain;
	}
	HWND CNdCefHandler::GetPPTMainWnd(){
		return m_hPPTMain;
	}
	void CNdCefHandler::SendEsc2PPT(){
		SendMsgToMainProcessByWND(m_hPPTMain,MSG_ESC,0,NULL);
	}
	bool CNdCefHandler::GetRegisterJs(){
		return m_bRegisterJs;
	}
	void CNdCefHandler::SetRegisterJs(bool nReg){
		m_bRegisterJs = nReg;
	}
	void CNdCefHandler::SetPopup(HWND hParent,int nPopup){
		m_bPopup = nPopup;
	}
	void CNdCefHandler::SetQuestionType(HWND hParent,int nType){
		m_bQestionType = nType;
	}
	void CNdCefHandler::SendMsgToMainProcessByWND(HWND hwMain,int dwdata,DWORD size,LPVOID ndata)
	{
		if(IsWindow(hwMain))
		{
			COPYDATASTRUCT ps;
			ps.dwData	= dwdata;
			ps.cbData	= size;
			ps.lpData	= ndata;
			::SendMessage(hwMain, WM_COPYDATA, 0,(LPARAM)&ps);
		}
	}

	void CNdCefHandler::registerListener(int browserid,std::string eventName, std::string callBack)
	{
		CallbackKey nKey;
		nKey.key = eventName;
		nKey.browerid = browserid;
		CallbackMap::iterator iter = m_CallbackMap.find(nKey);
		if (iter == m_CallbackMap.end()){	//从未注册该事件
			CallbackSet *pCallbackSet = new CallbackSet();
			pCallbackSet->insert(callBack);
			m_CallbackMap.insert(std::make_pair(nKey, pCallbackSet));
		}
		else{		//有注册过该事件
			CallbackSet *pCallbackSet = iter->second;
			pCallbackSet->insert(callBack);
		}
	}

	void CNdCefHandler::unRegisterListener(int browserid,std::string eventName, std::string callBack)
	{
		CallbackKey nKey;
		nKey.key = eventName;
		nKey.browerid = browserid;
		CallbackMap::iterator iter = m_CallbackMap.find(nKey);
		if (iter == m_CallbackMap.end()){
			return;	//没有找到
		}
		CallbackSet *pCallbackSet = iter->second;
		CallbackSet::iterator iter1 = pCallbackSet->find(callBack);

		if (iter1 != pCallbackSet->end()){
			pCallbackSet->erase(iter1);
		}

		return;
	}
	bool CNdCefHandler::OnProcessMessageReceived(
		CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message) {
			CEF_REQUIRE_UI_THREAD();

			if (message_router_->OnProcessMessageReceived(browser, source_process,
				message)) {
					return true;
			}

			std::wstring message_name = message->GetName();
			if (message_name==L"close"){
				browser->GetHost()->CloseBrowser(true);
			}else if (message_name==L"drawMethod"){
				CefRefPtr<CefListValue> args = message->GetArgumentList();
				if (args->GetSize()==2){
					std::string eventName = args->GetString(0).ToString();
					std::string eventData = args->GetString(1).ToString();
					std::string nInfo = eventName+"//*CEF*"+eventData;
					SendMsgToMainProcessByWND(m_hPPTMain,MSG_DRAWMETHOD,nInfo.length(),(LPVOID)nInfo.c_str());
					CNdDebug::printfStr("CNdCefHandler::OnProcessMessageReceived drawMethod");
				}
			}else if (message_name==L"sendEvent"||message_name==L"stopAcceptExam"){
				CefRefPtr<CefListValue> args = message->GetArgumentList();
				if (args->GetSize()==2){
					std::string eventName = args->GetString(0).ToString();
					std::string eventData = args->GetString(1).ToString();
					Json::Value root;
					Json::Reader reader;
					Json::FastWriter writer;
					if (reader.parse(eventData,root)){
						Json::Value sendEventName = root["eventName"];
						Json::Value sendEventData = root["eventData"];
						std::string nInfo = sendEventName.asString()+"//*CEF*"+writer.write(sendEventData);
						CNdDebug::printfStr("---------------");
						if (m_bCefLib){
							CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
							HWND  hCefParent = GetParent(hwnd);
							HWND  hCefTopParent = GetParent(hCefParent);
							SendMsgToMainProcessByWND(hCefTopParent,MSG_CEFINFO,nInfo.length(),(LPVOID)nInfo.c_str());
						}else{
							SendMsgToMainProcessByWND(m_hPPTMain,MSG_CEFINFO,nInfo.length(),(LPVOID)nInfo.c_str());
						}
					
						CNdDebug::printfStr("CNdCefHandler::OnProcessMessageReceived");
					}
				}
			}else if (message_name==L"InvokeNativeMethod"){
				CefRefPtr<CefListValue> args = message->GetArgumentList();
				if (args->GetSize()==1){
					std::string nInfo = args->GetString(0).ToString();
					
					if (m_bCefLib){
						CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
						HWND  hCefParent = GetParent(hwnd);
						HWND  hCefTopParent = GetParent(hCefParent);
						SendMsgToMainProcessByWND(hCefTopParent,MSG_CEF_ICRINVOKENATIVE,nInfo.length(),(LPVOID)nInfo.c_str());
					}else{
						SendMsgToMainProcessByWND(m_hPPTMain,MSG_CEF_ICRINVOKENATIVE,nInfo.length(),(LPVOID)nInfo.c_str());
					}
					CNdDebug::printfStr("CNdCefHandler::OnProcessMessageReceived InvokeNativeMethod");
				}
			}else if (message_name==L"getCurrentStudents"){
				/*
				if (m_bCefLib){
					CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
					HWND  hCefParent = GetParent(hwnd);
					HWND  hCefTopParent = GetParent(hCefParent);
					SendMsgToMainProcessByWND(hCefParent,MSG_OBTAIN_ONLINE_STUDENTS,0,NULL);
				}else{
					SendMsgToMainProcessByWND(m_hPPTMain,MSG_OBTAIN_ONLINE_STUDENTS,0,NULL);
				}*/
				SendMsgToMainProcessByWND(m_hPPTMain,MSG_OBTAIN_ONLINE_STUDENTS,0,NULL);
			}else if (message_name==L"showMessageBox"){
				CefRefPtr<CefListValue> args = message->GetArgumentList();
				if (args->GetSize()==2){
					std::string eventName = args->GetString(0).ToString();
					std::string eventData = args->GetString(1).ToString();

					Json::Value root;
					std::string error;
					Json::Reader reader;
					Json::FastWriter writer;
					if (reader.parse(eventData,root)){
						Json::Value value = root["value"];
						Json::Value buttons = value["button"];
						if (buttons.isArray()){
							Json::Value::iterator iter = buttons.begin();
							for (; iter != buttons.end(); iter++)
							{
								if ((*iter)["style"] == "primary")
								{
									Json::Value callback = (*iter)["callback"];
									std::string eventName = callback["eventName"].asString();
									std::string eventData = writer.write(callback["eventData"]); 
									CefString nEventNameTemp(eventName);
									CefString njsonTemp(eventData);
									ExecuteJavaScript(browser,nEventNameTemp.ToWString().c_str(), njsonTemp.ToWString().c_str());
									break;
								}
							}
						}
					}
				}
			}else if (message_name==L"registerListener"){
				CefRefPtr<CefListValue> args = message->GetArgumentList();
				if (args->GetSize()==2){
					std::string eventName = args->GetString(0).ToString();
					std::string eventData = args->GetString(1).ToString();
					registerListener(browser->GetIdentifier(),eventName,eventData);
				}

			}else if (message_name==L"unRegisterListener"){
				CefRefPtr<CefListValue> args = message->GetArgumentList();
				if (args->GetSize()==2){
					std::string eventName = args->GetString(0).ToString();
					std::string eventData = args->GetString(1).ToString();
					unRegisterListener(browser->GetIdentifier(),eventName,eventData);
				}

			}
			return false;
	}

	void CNdCefHandler::OnBeforeContextMenu(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefContextMenuParams> params,
		CefRefPtr<CefMenuModel> model) {
			CEF_REQUIRE_UI_THREAD();
			
			if ((params->GetTypeFlags() & (CM_TYPEFLAG_PAGE | CM_TYPEFLAG_FRAME)) != 0) {
				if (model->GetCount() > 0)
					model->AddSeparator();

				model->AddItem(NDCEF_ID_SHOW_DEVTOOLS, "&Show DevTools");
				model->AddItem(NDCEF_ID_SHOW_DEVTOOLS, "Close DevTools");
			}
			if (!CNdCefMain::GetDevtoolFlag()){
				model->Clear();
			}
	}

	bool CNdCefHandler::OnContextMenuCommand(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefContextMenuParams> params,
		int command_id,
		EventFlags event_flags) {
			CEF_REQUIRE_UI_THREAD();
			switch (command_id) {
				case NDCEF_ID_SHOW_DEVTOOLS:
					ShowDevTools(browser, CefPoint());
					return true;
				case NDCEF_ID_CLOSE_DEVTOOLS:
					CloseDevTools(browser);
					return true;
			}
			return false;
	}
	void CNdCefHandler::SetPreloadUrl(CefString strUrl)
	{
		m_strPreloadUrl = strUrl;
	}
	
	CefString CNdCefHandler::GetPreloadUrl()
	{
		return m_strPreloadUrl;
	}
	bool CNdCefHandler::GetPlayWindowClose(){
		return m_bPlayWindowClose;
	}
	void CNdCefHandler::SetPlayWindowClose(bool nClose){
		m_bPlayWindowClose = nClose;
	}
	bool CNdCefHandler::GetOcxWindow(){
		return m_ocxWindow;
	}
	void CNdCefHandler::SetOcxWindow(bool nocx){
		m_ocxWindow = nocx;
	}
	bool CNdCefHandler::OnConsoleMessage(CefRefPtr<CefBrowser> browser,
		const CefString& message,
		const CefString& source,
		int line) {
			CEF_REQUIRE_UI_THREAD();
			CNdDebug::logMsg(LOG_FOLDER_LOG_Consolelog,message.ToString().c_str());
			if (osr_handler_.get()){
				return osr_handler_.get()->OnConsoleMessage(browser,message,source,line);
			}else{
				if( m_hPPTMain == NULL )
					return false;

				if ( message=="playerloaded")
				{
					if (m_bCefLib){
						CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
						HWND  hCefParent = GetParent(hwnd);
						HWND  hCefTopParent = GetParent(hCefParent);
						SendMsgToMainProcessByWND(hCefTopParent,MSG_PLAYERLOADED,0,NULL);
					}else{
						SendMsgToMainProcessByWND(m_hPPTMain,MSG_PLAYERLOADED,0,NULL);
					}
					return false;
				}

				int nQuestionSavedType = MSG_QUESTION_ADD;
				std::string str = message.ToString();
				int pos = str.find("QuestionAdd");

				if( pos == -1 )
				{
					if (m_bQestionType==0){
						CNdDebug::printfStr("MSG_QUESTION_ADDSAVED");
						nQuestionSavedType = MSG_QUESTION_ADDSAVED;
					}else{
						CNdDebug::printfStr("MSG_QUESTION_SAVED");
						nQuestionSavedType = MSG_QUESTION_SAVED;
					}
					
					pos = str.find("QuestionSaved");
					if( pos == -1 )
					{
						
						return false;
					}
				}
				
				pos = str.find("id\":\"");
				if( pos == -1 )
					return false;

				pos += strlen("id\":\"");

				int pos1 = str.find('"', pos);
				if( pos1 == -1 )
					return false;

				std::string strGuid = str.substr(pos, pos1-pos);

				char szBuffer[1024] = {0};
				sprintf(szBuffer, "%d|%s", m_bBasicQuestion, strGuid.c_str() );

				COPYDATASTRUCT ps;

				ps.dwData	= nQuestionSavedType;
				ps.cbData	= sizeof(szBuffer);
				ps.lpData	= (PVOID)szBuffer;

				::SendMessage(m_hPPTMain, WM_COPYDATA, 0,(LPARAM)&ps);

				// exit process
				//CefQuitMessageLoop();
				//if (nQuestionSavedType != MSG_QUESTION_SAVED){
					//std::wstring strUrl = GetPreloadUrl().ToWString();
					//if (strUrl!=L""){
					//	browser->GetMainFrame()->LoadURL(strUrl);
					//}
					//CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
					//HWND  hCefParent = GetParent(hwnd);
					//ShowWindow(hCefParent, SW_HIDE);
				//}
			}
			return false;
	}

	void CNdCefHandler::OnBeforeDownload(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefDownloadItem> download_item,
		const CefString& suggested_name,
		CefRefPtr<CefBeforeDownloadCallback> callback) {
			CEF_REQUIRE_UI_THREAD();
			//CNdCefContext::Get()->GetDownloadPath("suggested_name");
			//callback->Continue(CNdCefContext::Get()->GetDownloadPath(suggested_name), true);
	}

	void CNdCefHandler::OnDownloadUpdated(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefDownloadItem> download_item,
		CefRefPtr<CefDownloadItemCallback> callback) {
			CEF_REQUIRE_UI_THREAD();
	}

	bool CNdCefHandler::OnDragEnter(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefDragData> dragData,
		CefDragHandler::DragOperationsMask mask) {
			CEF_REQUIRE_UI_THREAD();

			if (mask & DRAG_OPERATION_LINK)
				return true;

			return false;
	}

	bool CNdCefHandler::OnRequestGeolocationPermission(
		CefRefPtr<CefBrowser> browser,
		const CefString& requesting_url,
		int request_id,
		CefRefPtr<CefGeolocationCallback> callback) {
			CEF_REQUIRE_UI_THREAD();

			callback->Continue(true);
			return true;
	}



	bool CNdCefHandler::OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
		const CefKeyEvent& event,
		CefEventHandle os_event,
		bool* is_keyboard_shortcut) {
			CEF_REQUIRE_UI_THREAD();

			/*
			Const vbKeyLeft = 37 'LEFT   'ARROW 键←
			Const vbKeyUp = 38  'UP 'ARROW 键↑
			Const vbKeyRight = 39 'RIGHT  'ARROW 键→
			Const vbKeyDown = 40 'DOWN   'ARROW 键↓
			Const vbKeyEscape = 27  'ESC 键
			*/
			if (!m_bCefLib){
				if( event.type == KEYEVENT_RAWKEYDOWN )
				{
					if (os_event){

						if (os_event->wParam == VK_ESCAPE){
							SendMsgToMainProcessByWND(m_hPPTMain,MSG_ESC,0,NULL);
							return true;
						}
						if (os_event->wParam == VK_LEFT ||os_event->wParam == VK_UP ){
							SendMsgToMainProcessByWND(m_hPPTMain,MSG_CEFPREV,0,NULL);
							return true;
						}else if (os_event->wParam == VK_RIGHT||os_event->wParam == VK_DOWN){
							SendMsgToMainProcessByWND(m_hPPTMain,MSG_CEFNEXT,0,NULL);
							return true;
						}
					}
				}
			}else{
				if( event.type == KEYEVENT_RAWKEYDOWN )
				{
					if (os_event){
						if (os_event->wParam == VK_F4){
							return true;
						}
						if (os_event->wParam == VK_ESCAPE){
							CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
							HWND  hCefParent = GetParent(hwnd);
							HWND  hCefTopParent = GetParent(hCefParent);
							SendMsgToMainProcessByWND(hCefTopParent,MSG_ESC,0,NULL);
							return false;
						}
					}
				}
			}
			return false;
	}

	bool CNdCefHandler::OnBeforePopup(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		const CefString& target_url,
		const CefString& target_frame_name,
		const CefPopupFeatures& popupFeatures,
		CefWindowInfo& windowInfo,
		CefRefPtr<CefClient>& client,
		CefBrowserSettings& settings,
		bool* no_javascript_access) {
			CEF_REQUIRE_IO_THREAD();
			
			if (m_bPopup==0){
				ShellExecute(NULL, L"open", target_url.ToWString().c_str(), NULL, NULL, SW_SHOWNORMAL);
				return true;
			}else if (m_bPopup==1){
				CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
				HWND  hCefParent = GetParent(hwnd);
				HWND  hCefTopParent = GetParent(hCefParent);
				CNdDebug::wprintf(L"%s%d%d%d",L"OnBeforePopup HWnd",hwnd,hCefParent,hCefTopParent);
				CNdDebug::wprintf(L"%s%s",L"@@@",target_url.ToWString().c_str());
				SendMsgToMainProcessByWND(hCefTopParent, MSG_URLPOPUP, target_url.length(),(LPVOID)target_url.ToString().c_str());
				return true;
			}

			if (osr_handler_.get())
				return true;

			if (browser->GetHost()->IsWindowRenderingDisabled()) {
				return true;
			}
			return false;
	}

	void CreateMessageHandlers(MessageHandlerSet& handlers) {
		handlers.insert(new NdCef::Handler());
	}
	void CNdCefHandler::SetZoomed(){
		m_IsZoomed = true;
	}
	void CNdCefHandler::SetCefLib(){
		m_bCefLib = true;
	}
	void CNdCefHandler::BrowserSizeChange(CefRefPtr<CefBrowser> browser){
		if (browser.get()) {
			CefWindowHandle hWnd = browser->GetHost()->GetWindowHandle();
			CNdDebug::printf("%s%d","BrowserSizeChange:",hWnd);
			if (hWnd) 
			{
				HWND  hCefParent = GetParent(hWnd);
				HWND  hOcxParent = hCefParent;
				if (m_ocxWindow){
					hWnd = hOcxParent;
					hCefParent = GetParent(hCefParent);
				}
				if (IsWindow(hCefParent)){
					RECT rect;
					GetClientRect(hCefParent, &rect);
					RECT rectWnd;
					GetWindowRect(hCefParent, &rectWnd);

					int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
					int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

					int nleft = rect.left;
					int nWidth = rect.right - rect.left;
					if (rect.right - rect.left>nScreenWidth){
						nWidth = nScreenWidth;
						if (rectWnd.left<0){
							nleft = -rectWnd.left;
						}
					}
					int ntop = rect.top;
					int nHeight = rect.bottom - rect.top;
					if (rect.bottom - rect.top>nScreenHeight){
						nHeight = nScreenHeight;
						if (rectWnd.top<0){
							ntop = -rectWnd.top;
						}
					}
					HDWP hdwp = BeginDeferWindowPos(1);

					hdwp = DeferWindowPos(hdwp, hWnd, NULL,
						nleft, ntop, nWidth, nHeight,
						SWP_NOZORDER);
					EndDeferWindowPos(hdwp);
				}
			}
		}
	}
	void CNdCefHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
		CEF_REQUIRE_UI_THREAD();

		if (!message_router_) {
			CefMessageRouterConfig config;
			message_router_ = CefMessageRouterBrowserSide::Create(config);

			CreateMessageHandlers(message_handler_set_);
			MessageHandlerSet::const_iterator it = message_handler_set_.begin();
			for (; it != message_handler_set_.end(); ++it)
				message_router_->AddHandler(*(it), false);
		}
		
		if (mouse_cursor_change_disabled_)
			browser->GetHost()->SetMouseCursorChangeDisabled(true);
		
		if (browser->IsPopup()) {
			popup_browsers_.push_back(browser);

			CefPostTask(TID_UI,base::Bind(&CefBrowserHost::SetFocus, browser->GetHost().get(), true));
		}else{
			//ShowDevTools(browser, CefPoint());
			browserslist_.push_back(browser);	
		}

		browser_count_++;
	}

	bool CNdCefHandler::DoClose(CefRefPtr<CefBrowser> browser) {
		CEF_REQUIRE_UI_THREAD();
		CNdDebug::printfStr("CNdCefHandler::DoClose");
		
		if (browserslist_.size()==0){
			is_closing_ = true;
		}
		/*
		if (GetBrowserId() == browser->GetIdentifier()) {
		base::AutoLock lock_scope(lock_);
		is_closing_ = true;
		}
		*/
		return false;
	}

	void CNdCefHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) throw() {

		try
		{
			CEF_REQUIRE_UI_THREAD();
			CNdDebug::printfStr("CNdCefHandler::OnBeforeClose");
			message_router_->OnBeforeClose(browser);

			if (osr_handler_.get()) {
				osr_handler_->OnBeforeClose(browser);
				osr_handler_ = NULL;
			}

			if (browser->IsPopup()) {
				// Remove from the browser popup list.
				BrowserList::iterator bit = popup_browsers_.begin();
				for (; bit != popup_browsers_.end(); ++bit) {
					if ((*bit)->IsSame(browser)) {
						popup_browsers_.erase(bit);
						break;
					}
				}
			}else{
				BrowserList::iterator bititem = browserslist_.begin();
				for (; bititem != browserslist_.end(); ++bititem) {
					if ((*bititem)->IsSame(browser)) {
						browserslist_.erase(bititem);
						break;
					}
				}

			}

			if (--browser_count_ == 0) {
				MessageHandlerSet::const_iterator it = message_handler_set_.begin();
				for (; it != message_handler_set_.end(); ++it) {
					message_router_->RemoveHandler(*(it));
					delete *(it);
				}
				message_handler_set_.clear();
				message_router_ = NULL;

				if (CNdCefMessageLoop::Get()) CNdCefMessageLoop::Get()->Quit();
			}
		}catch(...)
		{
				;
		}
	}

	void CNdCefHandler::OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
		bool isLoading,
		bool canGoBack,
		bool canGoForward) {
			CEF_REQUIRE_UI_THREAD();

			SetLoading(isLoading);
			SetNavState(canGoBack, canGoForward);
			//if (m_IsZoomed){
				CNdDebug::printfStr("BrowserSizeChange");
				BrowserSizeChange(browser);
			//}
	}

	void CNdCefHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		ErrorCode errorCode,
		const CefString& errorText,
		const CefString& failedUrl) {
			CEF_REQUIRE_UI_THREAD();

			// Don't display an error for downloaded files.
			if (errorCode == ERR_ABORTED)
				return;

			// Don't display an error for external protocols that we allow the OS to
			// handle. See OnProtocolExecution().
			if (errorCode == ERR_UNKNOWN_URL_SCHEME) {
				std::string urlStr = frame->GetURL();
				if (urlStr.find("spotify:") == 0)
					return;
			}

			LoadErrorPage(frame, failedUrl, errorCode, errorText);

			CNdDebug::wprintf(L"%s",L"OnLoadError");
			CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
			HWND  hCefParent = GetParent(hwnd);
			HWND  hCefTopParent = GetParent(hCefParent);
			CNdDebug::wprintf(L"%s%d%d%d",L"OnLoadError HWnd",hwnd,hCefParent,hCefTopParent);
			CNdDebug::wprintf(L"%s%s",L"@@@",failedUrl.ToWString().c_str());
			SendMsgToMainProcessByWND(hCefTopParent, MSG_URLFAILED, failedUrl.length(),(LPVOID)failedUrl.ToString().c_str());

			// Load the error page.
			
	}

	bool CNdCefHandler::OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		bool is_redirect) {
			CEF_REQUIRE_UI_THREAD();

			if (!GetRegisterJs()){
				CefRefPtr<CefProcessMessage> msg= CefProcessMessage::Create("NoRegisterJS");
				if (browser.get()){
					browser->SendProcessMessage(PID_RENDERER, msg);
				}
			}

			message_router_->OnBeforeBrowse(browser, frame);
			return false;
	}
	const char kTestOrigin[] = "http://tests/";

	// Retrieve the file name and mime type based on the specified url.
	bool ParseTestUrl(const std::string& url,
		std::string* file_name,
		std::string* mime_type) {
			// Retrieve the path component.
			CefURLParts parts;
			CefParseURL(url, parts);
			std::string file = CefString(&parts.path);
			if (file.size() < 2)
				return false;

			// Remove the leading slash.
			file = file.substr(1);

			// Verify that the file name is valid.
			for(size_t i = 0; i < file.size(); ++i) {
				const char c = file[i];
				if (!isalpha(c) && !isdigit(c) && c != '_' && c != '.')
					return false;
			}

			// Determine the mime type based on the file extension, if any.
			size_t pos = file.rfind(".");
			if (pos != std::string::npos) {
				std::string ext = file.substr(pos + 1);
				if (ext == "html")
					*mime_type = "text/html";
				else if (ext == "png")
					*mime_type = "image/png";
				else
					return false;
			} else {
				// Default to an html extension if none is specified.
				*mime_type = "text/html";
				file += ".html";
			}

			*file_name = file;
			return true;
	}


	std::string DumpRequestContents(CefRefPtr<CefRequest> request) {
		std::stringstream ss;

		ss << "URL: " << std::string(request->GetURL());
		ss << "\nMethod: " << std::string(request->GetMethod());

		CefRequest::HeaderMap headerMap;
		request->GetHeaderMap(headerMap);
		if (headerMap.size() > 0) {
			ss << "\nHeaders:";
			CefRequest::HeaderMap::const_iterator it = headerMap.begin();
			for (; it != headerMap.end(); ++it) {
				ss << "\n\t" << std::string((*it).first) << ": " <<
					std::string((*it).second);
			}
		}

		CefRefPtr<CefPostData> postData = request->GetPostData();
		if (postData.get()) {
			CefPostData::ElementVector elements;
			postData->GetElements(elements);
			if (elements.size() > 0) {
				ss << "\nPost Data:";
				CefRefPtr<CefPostDataElement> element;
				CefPostData::ElementVector::const_iterator it = elements.begin();
				for (; it != elements.end(); ++it) {
					element = (*it);
					if (element->GetType() == PDE_TYPE_BYTES) {
						// the element is composed of bytes
						ss << "\n\tBytes: ";
						if (element->GetBytesCount() == 0) {
							ss << "(empty)";
						} else {
							// retrieve the data.
							size_t size = element->GetBytesCount();
							char* bytes = new char[size];
							element->GetBytes(size, bytes);
							ss << std::string(bytes, size);
							delete [] bytes;
						}
					} else if (element->GetType() == PDE_TYPE_FILE) {
						ss << "\n\tFile: " << std::string(element->GetFile());
					}
				}
			}
		}

		return ss.str();
	}

	CefRefPtr<CefResourceHandler> GetMyResourceHandler(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request) {
			std::string url = request->GetURL();
			if (url.find(kTestOrigin) == 0) {
				// Handle URLs in the test origin.
				std::string file_name, mime_type;
				if (ParseTestUrl(url, &file_name, &mime_type)) {
					//if (file_name == "request.html") {
					// Show the request contents.
					const std::string& dump = DumpRequestContents(request);
					std::string str = "<html><body bgcolor=\"white\"><pre>" + dump +
						"</pre></body></html>";
					CefRefPtr<CefStreamReader> stream =
						CefStreamReader::CreateForData(
						static_cast<void*>(const_cast<char*>(str.c_str())),
						str.size());
					DCHECK(stream.get());
					return new CefStreamResourceHandler("text/html", stream);
					//} else {
					// Load the resource from file.
					//	CefRefPtr<CefStreamReader> stream =
					//		GetBinaryResourceReader(file_name.c_str());
					//	if (stream.get())
					//		return new CefStreamResourceHandler(mime_type, stream);
					//}
				}
			}

			return NULL;
	}

	CefRefPtr<CefResourceHandler> CNdCefHandler::GetResourceHandler(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request) {
			CEF_REQUIRE_IO_THREAD();

			GetMyResourceHandler(browser, frame, request);
			//message_router_->OnBeforeBrowse(browser, frame);
			return false;
	}
	bool CNdCefHandler::OnQuotaRequest(CefRefPtr<CefBrowser> browser,
		const CefString& origin_url,
		int64 new_size,
		CefRefPtr<CefRequestCallback> callback) {
			CEF_REQUIRE_IO_THREAD();

			static const int64 max_size = 1024 * 1024 * 20;  // 20mb.

			// Grant the quota request if the size is reasonable.
			callback->Continue(new_size <= max_size);
			return true;
	}


	void CNdCefHandler::OnAddressChange(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		const CefString& url) {
			CEF_REQUIRE_UI_THREAD();
			CNdDebug::wprintf(L"%s",L"OnAddressChange");
			CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
			HWND  hCefParent = GetParent(hwnd);
			HWND  hCefTopParent = GetParent(hCefParent);
			CNdDebug::wprintf(L"%s%d%d%d",L"OnAddressChange HWnd",hwnd,hCefParent,hCefTopParent);
			CNdDebug::wprintf(L"%s%s",L"@@@",url.ToWString().c_str());
			SendMsgToMainProcessByWND(hCefTopParent, MSG_URLCHANGE, url.length(),(LPVOID)url.ToString().c_str());
			std::string strUrl = url.ToString();

			if( strUrl.find("interaction") != -1 )
				m_bBasicQuestion = false;
			else
				m_bBasicQuestion = true;
			if (strUrl.find("nd_handwritequestion")!=-1){
				m_bBasicQuestion = false;
			}

	}

	void CNdCefHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
		const CefString& title) {
			CEF_REQUIRE_UI_THREAD();
			/*
			CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
			if (GetBrowserId() == browser->GetIdentifier()) {
			hwnd = GetParent(hwnd);
			}
			SetWindowText(hwnd, std::wstring(title).c_str());
			*/
	}

	void CNdCefHandler::SetLoading(bool isLoading) {

	}

	void CNdCefHandler::SetNavState(bool canGoBack, bool canGoForward) {

	}
	void CNdCefHandler::OnProtocolExecution(CefRefPtr<CefBrowser> browser,
		const CefString& url,
		bool& allow_os_execution) {
			CEF_REQUIRE_UI_THREAD();

			std::string urlStr = url;

			// Allow OS execution of Spotify URIs.
			if (urlStr.find("spotify:") == 0)
				allow_os_execution = true;
	}

	bool CNdCefHandler::OnCertificateError(
		CefRefPtr<CefBrowser> browser,
		ErrorCode cert_error,
		const CefString& request_url,
		CefRefPtr<CefSSLInfo> ssl_info,
		CefRefPtr<CefRequestCallback> callback) {
			CEF_REQUIRE_UI_THREAD();

			CefRefPtr<CefSSLCertPrincipal> subject = ssl_info->GetSubject();
			CefRefPtr<CefSSLCertPrincipal> issuer = ssl_info->GetIssuer();

			// Build a table showing certificate information.
			std::stringstream ss;
			ss << "X.509 Certificate Information:"
				"<table border=1><tr><th>Field</th><th>Value</th></tr>" <<
				"<tr><td>Subject</td><td>" <<
				(subject.get() ? subject->GetDisplayName().ToString() : "&nbsp;") <<
				"</td></tr>"
				"<tr><td>Issuer</td><td>" <<
				(issuer.get() ? issuer->GetDisplayName().ToString() : "&nbsp;") <<
				"</td></tr>"
				"<tr><td>Serial #*</td><td>" <<
				GetBinaryString(ssl_info->GetSerialNumber()) << "</td></tr>"
				"<tr><td>Valid Start</td><td>" <<
				GetTimeString(ssl_info->GetValidStart()) << "</td></tr>"
				"<tr><td>Valid Expiry</td><td>" <<
				GetTimeString(ssl_info->GetValidExpiry()) << "</td></tr>"
				"<tr><td>DER Encoded*</td>"
				"<td style=\"max-width:800px;overflow:scroll;\">" <<
				GetBinaryString(ssl_info->GetDEREncoded()) << "</td></tr>"
				"<tr><td>PEM Encoded*</td>"
				"<td style=\"max-width:800px;overflow:scroll;\">" <<
				GetBinaryString(ssl_info->GetPEMEncoded()) << "</td></tr>"
				"</table> * Displayed value is base64 encoded.";

			// Load the error page.
			LoadErrorPage(browser->GetMainFrame(), request_url, cert_error, ss.str());

			return false;  // Cancel the request.
	}

	void CNdCefHandler::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
		TerminationStatus status) {
			CEF_REQUIRE_UI_THREAD();

			message_router_->OnRenderProcessTerminated(browser);

			// Load the startup URL if that's not the website that we terminated on.
			CefRefPtr<CefFrame> frame = browser->GetMainFrame();
			std::wstring url = frame->GetURL();
			std::transform(url.begin(), url.end(), url.begin(), tolower);
	}
	void CNdCefHandler::CloseDevTools(CefRefPtr<CefBrowser> browser) {
		browser->GetHost()->CloseDevTools();
	}
	void CNdCefHandler::ShowDevTools(CefRefPtr<CefBrowser> browser,
                                 const CefPoint& inspect_element_at) {
		  CefWindowInfo windowInfo;
		  CefBrowserSettings settings;

		#if defined(OS_WIN)
		  windowInfo.SetAsPopup(browser->GetHost()->GetWindowHandle(), "DevTools");
		#endif

		  browser->GetHost()->ShowDevTools(windowInfo, this, settings,
										   inspect_element_at);
		}

	bool CNdCefHandler::GetRootScreenRect(CefRefPtr<CefBrowser> browser,
		CefRect& rect) {
			CEF_REQUIRE_UI_THREAD();
			if (!osr_handler_.get())
				return false;
			return osr_handler_->GetRootScreenRect(browser, rect);
	}

	bool CNdCefHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) {
		CEF_REQUIRE_UI_THREAD();
		if (!osr_handler_.get())
			return false;
		return osr_handler_->GetViewRect(browser, rect);
	}

	bool CNdCefHandler::GetScreenPoint(CefRefPtr<CefBrowser> browser,
		int viewX,
		int viewY,
		int& screenX,
		int& screenY) {
			CEF_REQUIRE_UI_THREAD();
			if (!osr_handler_.get())
				return false;
			return osr_handler_->GetScreenPoint(browser, viewX, viewY, screenX, screenY);
	}

	bool CNdCefHandler::GetScreenInfo(CefRefPtr<CefBrowser> browser,
		CefScreenInfo& screen_info) {
			CEF_REQUIRE_UI_THREAD();
			if (!osr_handler_.get())
				return false;
			return osr_handler_->GetScreenInfo(browser, screen_info);
	}

	void CNdCefHandler::OnPopupShow(CefRefPtr<CefBrowser> browser,
		bool show) {
			CEF_REQUIRE_UI_THREAD();
			if (!osr_handler_.get())
				return;
			return osr_handler_->OnPopupShow(browser, show);
	}

	void CNdCefHandler::OnPopupSize(CefRefPtr<CefBrowser> browser,
		const CefRect& rect) {
			CEF_REQUIRE_UI_THREAD();
			if (!osr_handler_.get())
				return;
			return osr_handler_->OnPopupSize(browser, rect);
	}

	void CNdCefHandler::OnPaint(CefRefPtr<CefBrowser> browser,
		PaintElementType type,
		const RectList& dirtyRects,
		const void* buffer,
		int width,
		int height) {
			CEF_REQUIRE_UI_THREAD();
			if (!osr_handler_.get())
				return;
			osr_handler_->OnPaint(browser, type, dirtyRects, buffer, width, height);
	}

	void CNdCefHandler::OnCursorChange(CefRefPtr<CefBrowser> browser,
		CefCursorHandle cursor,
		CursorType type,
		const CefCursorInfo& custom_cursor_info) {
			CEF_REQUIRE_UI_THREAD();
			if (!osr_handler_.get())
				return;
			osr_handler_->OnCursorChange(browser, cursor, type, custom_cursor_info);
	}

	bool CNdCefHandler::StartDragging(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefDragData> drag_data,
		CefRenderHandler::DragOperationsMask allowed_ops,
		int x, int y) {
			CEF_REQUIRE_UI_THREAD();
			if (!osr_handler_.get())
				return false;
			return osr_handler_->StartDragging(browser, drag_data, allowed_ops, x, y);
	}

	void CNdCefHandler::UpdateDragCursor(CefRefPtr<CefBrowser> browser,
		CefRenderHandler::DragOperation operation) {
			CEF_REQUIRE_UI_THREAD();
			if (!osr_handler_.get())
				return;
			osr_handler_->UpdateDragCursor(browser, operation);
	}

	void CNdCefHandler::SetMainWindowHandle(CNdCefWindowHandle handle) {
		if (!CefCurrentlyOn(TID_UI)) {
			// Execute on the UI thread.
			CefPostTask(TID_UI,
				base::Bind(&CNdCefHandler::SetMainWindowHandle, this, handle));
			return;
		}

		main_handle_ = handle;
	}

	CNdCefWindowHandle CNdCefHandler::GetMainWindowHandle() const {
		CEF_REQUIRE_UI_THREAD();
		return main_handle_;
	}

	void CNdCefHandler::SetOSRHandler(CefRefPtr<RenderHandler> handler) {
		if (!CefCurrentlyOn(TID_UI)) {
			// Execute on the UI thread.
			CefPostTask(TID_UI,
				base::Bind(&CNdCefHandler::SetOSRHandler, this, handler));
			return;
		}

		osr_handler_ = handler;
	}

	CefRefPtr<CNdCefHandler::RenderHandler> CNdCefHandler::GetOSRHandler() const {
		return osr_handler_; 
	}

	void CNdCefHandler::CloseAllBrowsers(bool force_close) {
		if (!CefCurrentlyOn(TID_UI)) {
			// Execute on the UI thread.
			CefPostTask(TID_UI,
				base::Bind(&CNdCefHandler::CloseAllBrowsers, this, force_close));
			return;
		}

		if (!popup_browsers_.empty()) {
			// Request that any popup browsers close.
			BrowserList::const_iterator it = popup_browsers_.begin();
			for (; it != popup_browsers_.end(); ++it)
				(*it)->GetHost()->CloseBrowser(force_close);
		}
		if (!browserslist_.empty()) {
			BrowserList::const_iterator it = browserslist_.begin();
			for (; it != browserslist_.end(); ++it)
				(*it)->GetHost()->CloseBrowser(force_close);
		}
	}

	bool CNdCefHandler::IsClosing() const {
		base::AutoLock lock_scope(lock_);
		return is_closing_;
	}
	bool CNdCefHandler::SetBrowserSizeChange(HWND hParentWnd){
		bool nresult = false;
		CefRefPtr<CefBrowser> nBrowser = FindBrowser(hParentWnd);
		if (nBrowser.get()){
			BrowserSizeChange(nBrowser);
			nresult = true;
		}
		return nresult;
	}
	void CNdCefHandler::ClearCookie(std::wstring nUrl,std::wstring nCookieName){
			if (!CefCurrentlyOn(TID_IO)) {
				// Execute on the UI thread.
				CefPostTask(TID_IO,
					base::Bind(&CNdCefHandler::ClearCookie, this, nUrl, nCookieName));
				return;
			}
			CefRefPtr<CefCookieManager> manager = CefCookieManager::GetGlobalManager();
			if (manager.get()){
				manager->DeleteCookies(CefString(nUrl.c_str()), CefString(nCookieName.c_str()));
			}
	}
	bool CNdCefHandler::ChangeUrl(HWND hParentWnd,std::wstring nUrl){
		bool nresult = false;
		CefRefPtr<CefBrowser> nBrowser = FindBrowser(hParentWnd);
		if (nBrowser.get()){
			nBrowser->GetMainFrame()->LoadURL(nUrl);
			nresult = true;
		}
		return nresult;
	}
	bool CNdCefHandler::CloseByWnd(HWND hParentWnd){
		bool nresult = false;
		CefRefPtr<CefBrowser> nBrowser = FindBrowser(hParentWnd);
		if (nBrowser.get()){
			//nBrowser->get
			//if (IsWindow(hWnd_))
			//	DestroyWindow(hWnd_);
			HWND hWnd = nBrowser->GetHost()->GetWindowHandle();
			if (IsWindow(hWnd)){
				DestroyWindow(hWnd);
				nresult = true;
			}
		}
		return nresult;
	}
	bool CNdCefHandler::FindBrowserWnd(HWND hParent){
		bool nresult = false;
		CefRefPtr<CefBrowser> nBrowser = FindBrowser(hParent);
		if (nBrowser.get()){
			nresult = true;
		}
		return nresult = true;
	}
	std::wstring CNdCefHandler::FindBrowserUrl(HWND hParent){
		std::wstring nUrl= L"";
		CefRefPtr<CefBrowser> nBrowser = FindBrowser(hParent);
		if (nBrowser.get()){
			nUrl = nBrowser->GetMainFrame()->GetURL().ToWString();
		}
		return nUrl;
	}
 
	int CNdCefHandler::GetBrowserSize(){
		return browserslist_.size();
	}
	CefRefPtr<CefBrowser> CNdCefHandler::FindBrowser(HWND hParent){
		CefRefPtr<CefBrowser> nResult;
		if (!browserslist_.empty()) {
			BrowserList::iterator bititem = browserslist_.begin();
			for (; bititem != browserslist_.end(); ++bititem) {
				CefWindowHandle hwnd = (*bititem)->GetHost()->GetWindowHandle();
				HWND  hCefParent = GetParent(hwnd);
				HWND  hCefTopParent = GetParent(hCefParent);
				if (hCefTopParent == hParent||hCefParent==hParent) {
					nResult =  (*bititem);
				}
			}
		}
		return nResult;
	}
	void CNdCefHandler::SetBrowserBlank(HWND hParent){
		if (!browserslist_.empty()) {
			BrowserList::iterator bititem = browserslist_.begin();
			for (; bititem != browserslist_.end(); ++bititem) {
				CefWindowHandle hwnd = (*bititem)->GetHost()->GetWindowHandle();
				HWND  hCefParent = GetParent(hwnd);
				HWND  hCefTopParent = GetParent(hCefParent);
				if (hCefTopParent == hParent||hCefParent==hParent) {
					;
				}else{
					if ((*bititem)){
						(*bititem)->GetMainFrame()->LoadURL(L"about:blank");
					}
				}
			}
		}
	}
	bool CNdCefHandler::ExecuteJS(HWND hParent,LPCTSTR nJSCode){
		bool nResult = false;
		if (!browserslist_.empty()) {
			BrowserList::iterator bititem = browserslist_.begin();
			for (; bititem != browserslist_.end(); ++bititem) {

				CefWindowHandle hwnd = (*bititem)->GetHost()->GetWindowHandle();
				HWND  hCefParent = GetParent(hwnd);
				HWND  hCefTopParent = GetParent(hCefParent);
				if (hCefTopParent == hParent) {
					CNdDebug::printfStr("GetParent(hwnd) == hParent");
					CefRefPtr<CefFrame> frame = (*bititem)->GetMainFrame(); 
					frame->ExecuteJavaScript(nJSCode,frame->GetURL(), 0);
					nResult = true;
				}

			}

		}
		return nResult;
	}
	std::string&   stringreplace(std::string&   str, const   std::string&   old_value, const   std::string&   new_value)
	{
		std::string::size_type   pos(0);
		while (true)   {
			if ((pos = str.find(old_value)) != std::string::npos){
				str.replace(pos, old_value.length(), new_value);
				pos += new_value.length();
			}
			else
				break;
		}
		return   str;
	}
	bool CNdCefHandler::ExecuteJavaScript(CefRefPtr<CefBrowser> nBrowser,LPCTSTR eventName,LPCTSTR json){
		bool nResult = false;
		CefRefPtr<CefFrame> frame = nBrowser->GetMainFrame(); 
		CefString nEventNameTemp(eventName);
		CefString njsonTemp(json);
		CallbackKey nKey;
		nKey.key = nEventNameTemp.ToString();
		nKey.browerid = nBrowser->GetIdentifier();
		CallbackMap::iterator iter = m_CallbackMap.find(nKey);
		CNdDebug::printf("%s%d","m_CallbackMap.size",m_CallbackMap.size());
		if (iter != m_CallbackMap.end()){
			CallbackSet callbacks(*(iter->second));	//取出Listener集，复制一份
			CallbackSet::iterator iter1 = callbacks.begin();
			for (; iter1 != callbacks.end(); iter1++)
			{
				std::string callback = *iter1;
				stringreplace(callback, "==param==", njsonTemp.ToString());

				CNdDebug::logMsg(LOG_FOLDER_LOG_JSCall,"---------frame->ExecuteJavaScript-----------");
				CNdDebug::logMsg(LOG_FOLDER_LOG_JSCall,(char*)callback.c_str());
				frame->ExecuteJavaScript(callback.c_str(),frame->GetURL(), 0);
				CNdDebug::logMsg(LOG_FOLDER_LOG_JSCall,"---------frame->ExecuteJavaScript  OK-----------");
			}
			nResult = true;
		}
		return nResult;
	}
	bool CNdCefHandler::ExecuteJSCallBack(HWND hParent,LPCTSTR eventName,LPCTSTR json){
		bool nResult = false;
		CefRefPtr<CefBrowser> nBrowser = FindBrowser(hParent);
		if (nBrowser.get()){
			nResult = ExecuteJavaScript(nBrowser,eventName,json);
		}
		return nResult;
	}
	bool CNdCefHandler::RegisterJS(NdCefIRegisterJs* nJsFunObj){
		m_NdCefIRegisterJs = nJsFunObj;
		return true;
	}
	CefRefPtr<CefBrowser> CNdCefHandler::GetBrowser(){
		CefRefPtr<CefBrowser> nResult;
		if (!browserslist_.empty()) {
			BrowserList::iterator bititem = browserslist_.begin();
			if (bititem != browserslist_.end()){
				nResult =  (*bititem);
			}
			
		}
		return nResult;
	}
	void CNdCefHandler::SetFocusToBrowser(CefRefPtr<CefBrowser> browser) {
		if (!CefCurrentlyOn(TID_UI)) {
			// Execute on the UI thread.
			CefPostTask(TID_UI,base::Bind(&CefBrowserHost::SetFocus, browser->GetHost().get(), true));
			//CefPostTask(TID_UI, base::Bind(&CNdCefHandler::SetFocusToBrowser, browser));
			return;
		}

		browser->GetHost()->SetFocus(true);
	}
}
