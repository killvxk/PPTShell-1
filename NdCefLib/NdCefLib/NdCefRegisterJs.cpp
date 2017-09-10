#include "stdafx.h"
#include "NdCefRegisterJs.h"

#include <algorithm>
#include <string>

#include "include/wrapper/cef_stream_resource_handler.h"
#include "include/wrapper/cef_closure_task.h"
#include "NdCefV8Value2Json.h"
#include "time.h"
#include "NdCefShareIpc.h"

using namespace std;

namespace NdCef {
	namespace NdCefRegisterJs {

		namespace {

			const char kPCInterface[] = "PCInterface";
			const char kCoursePlayer[] = "CoursePlayer";
			const char kclose[] = "close";
			const char kdrawMethod[] = "drawMethod";
			
			
			const char kinvokeMethod[] = "invokeMethod";
			const char kprintLog[] = "printLog";
			const char kinvokeMethodAsync[] = "invokeMethodAsync";
			const char kregisterListener[] = "registerListener";
			const char kunRegisterListener[] = "unRegisterListener";
			const char kprepareSwitchPage[] = "prepareSwitchPage";
			const char konPageLoaded[] = "onPageLoaded";

			class Accessor : public CefV8Accessor {
			public:
				Accessor() {}
				virtual bool Get(const CefString& name,
					const CefRefPtr<CefV8Value> object,
					CefRefPtr<CefV8Value>& retval,
					CefString& exception) OVERRIDE { 
						retval = val_;
						return false; 
				}
				virtual bool Set(const CefString& name,
					const CefRefPtr<CefV8Value> object,
					const CefRefPtr<CefV8Value> value,
					CefString& exception) OVERRIDE {
						val_ = value;
						return true;
				}
				CefRefPtr<CefV8Value> val_;
				IMPLEMENT_REFCOUNTING(Accessor);
			};
			class V8Handler : public CefV8Handler {
			public:
				CefRefPtr<CefBrowser> m_browser;
				V8Handler() {
				}
				virtual bool Execute(const CefString& name,
					CefRefPtr<CefV8Value> object,
					const CefV8ValueList& arguments,
					CefRefPtr<CefV8Value>& retval,
					CefString& exception) {
						CNdDebug::wprintf(L"%s",name.c_str());
						if (name == kclose){
							CefRefPtr<CefProcessMessage> msg= CefProcessMessage::Create("close");
							if (m_browser.get()){
								m_browser->SendProcessMessage(PID_RENDERER, msg);
							}
							retval = CefV8Value::CreateBool(true);
						}else if (name == kdrawMethod ||  name == kinvokeMethod || name == kinvokeMethodAsync) {
							if (arguments.size() >=1 && arguments[0]->IsString() ) {
								std::wstring CommandName = arguments[0]->GetStringValue();
								std::wstring eventData = L"{}";
								if (name == kdrawMethod || name == kinvokeMethod){
									if (arguments.size()==2&&arguments[1]->IsString()){
										eventData = arguments[1]->GetStringValue();
									}
								}else if (name == kinvokeMethodAsync){
									if (arguments.size()==3&&arguments[2]->IsString()){
										eventData = arguments[2]->GetStringValue();
									}
								}
								std::wstring methodName = CommandName.substr(CommandName.find_last_of('.') + 1);
								//std::wstring eventParams = arguments[2]->GetStringValue();
								CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,"--------------kinvokeMethod----------------------");
								CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,"%s%s",L"CommandName:",CefString(CommandName).ToString().c_str());
								CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,"%s%s",L"eventData:",CefString(eventData).ToString().c_str());
								CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,"%s%s",L"methodName:",CefString(methodName).ToString().c_str());
								CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,"---------------kinvokeMethod---------------------");
								if (name == kdrawMethod){
									CefRefPtr<CefProcessMessage> msg= CefProcessMessage::Create("drawMethod");
									Json::FastWriter writer;
									CefRefPtr<CefListValue> args = msg->GetArgumentList();
									Json::Value vEvent;
									vEvent["method_name"] = CefString(methodName).ToString();
									vEvent["param"] =  CefString(eventData).ToString();

									args->SetString(0, writer.write(vEvent));

									if (m_browser.get()){
										m_browser->SendProcessMessage(PID_RENDERER, msg);
										Sleep(100);
									}
									CNdCefShareMemory nipc;
									CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,"**********drawMethod OpenFileMapping****************");
									if (nipc.OpenFileMemoryMapping(GLOBAL_MEMORY_NAME,GLOBAL_EVENT_NAME)){
										char pszData[MEMORY_SIZE];
										DWORD nSize = MEMORY_SIZE;
										CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,"************drawMethod ReadMemory**************");
										if (nipc.ReadMemory(pszData,nSize,MEMORY_SIZE)){
											pszData[nSize] = '\0';
											Json::Reader reader;  
											Json::Value root;
											if (reader.parse(pszData,root)){
												std::string njsonstr(pszData); 
												CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,pszData);
												retval = CefV8Value::CreateString(njsonstr);
											}else{
												retval = CefV8Value::CreateString("{}");
												CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,"*************drawMethod parse false*****************");
											}
										}else{
											retval = CefV8Value::CreateString("{}");
											CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,"*************drawMethod ReadMemory false**********");
										}
									}else{
										CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,"**********drawMethod OpenFileMapping false*****************");
										retval = CefV8Value::CreateString("{}");
									}
									CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,"*************drawMethod OpenFileMapping****************");
								}else if (methodName==L"sendEvent"){
									//CefRefPtr<CefV8Value> eventName = arguments[2]->GetValue("eventName");
									//CefRefPtr<CefV8Value> eventData = arguments[2]->GetValue("eventData");
									//std::string eventParam = "";
									//if (eventData->IsObject()){
									//	eventParam = CNdCefV8Value2Json::V8Value2ObjStr(eventData);
									//}
									CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,"sendEvent:%s",eventData.c_str());
									CefRefPtr<CefProcessMessage> msg= CefProcessMessage::Create("sendEvent");
									CefRefPtr<CefListValue> args = msg->GetArgumentList();
									args->SetString(0, methodName);
									args->SetString(1, eventData);
									if (m_browser.get()){
										m_browser->SendProcessMessage(PID_RENDERER, msg);
									}
									retval = CefV8Value::CreateBool(true);
								}else if (methodName==L"stopAcceptExam"){
									CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,"stopAcceptExam:%s",eventData.c_str());
									Json::FastWriter writer;
									CefRefPtr<CefProcessMessage> msg= CefProcessMessage::Create("stopAcceptExam");
									CefRefPtr<CefListValue> args = msg->GetArgumentList();
									Json::Value vEvent;
									Json::Value eventDatajs;
									Json::Value eventValue;
									Json::Value userarray(Json::arrayValue);
									eventValue["questionId"] = "";
									eventValue["url"] = "";
									eventValue["questionType"] = "";
									eventValue["isForce"] = false;
									eventValue["dispatchOnly"] = true;
									eventValue["correctUserIds"] = userarray;
									eventValue["examId"] = "";

									eventDatajs["source"] = "";
									eventDatajs["item"] = "";
									eventDatajs["type"] = "forcestop";
									eventDatajs["value"] = eventValue;
									vEvent["eventName"] = "Exam";
									vEvent["eventData"] = eventDatajs;
									args->SetString(0, methodName);
									args->SetString(1, writer.write(vEvent));
									if (m_browser.get()){
										m_browser->SendProcessMessage(PID_RENDERER, msg);
									}
									retval = CefV8Value::CreateString("{}");
								}else if (methodName==L"showMessageBox"){
									CefRefPtr<CefProcessMessage> msg= CefProcessMessage::Create("showMessageBox");
									CefRefPtr<CefListValue> args = msg->GetArgumentList();
									args->SetString(0, methodName);
									args->SetString(1, eventData);
									if (m_browser.get()){
										m_browser->SendProcessMessage(PID_RENDERER, msg);
									}
									retval = CefV8Value::CreateBool(true);
								}else if (methodName==L"getPlayMode"){
									std::wstring playMode = L"{\"isPreview\":false}";
									retval = CefV8Value::CreateString(playMode);
								}else if (methodName==L"getCurrentUserInfo"){
									std::wstring currentUserInfo = L"{\"isProjector\":false,\"userType\":\"TEACHER\",\"isPC\":true,\"name\":\"张老师\",\"number\":\"10000\"}";
									retval = CefV8Value::CreateString(currentUserInfo);
								}else if (methodName==L"GetCurrentGroups"){
									std::wstring currentGroups = L"{\"groups\": [{\"groupId\": \"0\", \"groupName\": \"全班\", \"groupOrder\": \"0\" }]}";
									retval = CefV8Value::CreateString(currentGroups);
								}else if (methodName==L"getCurrentStudents"){
									CefRefPtr<CefProcessMessage> msg= CefProcessMessage::Create("getCurrentStudents");
									if (m_browser.get()){
										m_browser->SendProcessMessage(PID_RENDERER, msg);
										Sleep(100);
									}
									CNdCefShareMemory nipc;
									CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,"***********getCurrentStudents OpenFileMapping************");
									if (nipc.OpenFileMemoryMapping(GLOBAL_MEMORY_NAME,GLOBAL_EVENT_NAME)){
										char pszData[MEMORY_SIZE];
										DWORD nSize = MEMORY_SIZE;
										CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,"***********getCurrentStudents ReadMemory*******");
										if (nipc.ReadMemory(pszData,nSize,MEMORY_SIZE)){
											pszData[nSize] = '\0';
											Json::Reader reader; 
											Json::Value root;
											if (reader.parse(pszData,root)){
												std::string njsonstr(pszData); 
												CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,pszData);
												retval = CefV8Value::CreateString(njsonstr);
											}else{
												retval = CefV8Value::CreateString("{}");
												CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,"*************getCurrentStudents parse false*****************");
											}
										}else{
											retval = CefV8Value::CreateString("{}");
											CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,"*************getCurrentStudents ReadMemory false*****************");
										}
									}else{
										CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,"*******getCurrentStudents OpenFileMapping false*****************");
										retval = CefV8Value::CreateString("{}");
									}
									CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,"********getCurrentStudents OpenFileMapping***************");
								}else if (methodName==L"printLog"){
									CNdDebug::logMsg(LOG_FOLDER_LOG_Consolelog,CefString(eventData).ToString().c_str());
									retval = CefV8Value::CreateString("{}");
								}else if (methodName==L"log"){
									CNdDebug::logMsg(LOG_FOLDER_LOG_Consolelog,CefString(eventData).ToString().c_str());
									retval = CefV8Value::CreateString("{}");
								}else{
									CefRefPtr<CefProcessMessage> msg= CefProcessMessage::Create("InvokeNativeMethod");
									/*接口格式
									{
										"method_name":"dohandwrite",
										"param":"string-value"
									}
									*/
									Json::FastWriter writer;
									CefRefPtr<CefListValue> args = msg->GetArgumentList();
									Json::Value vEvent;
									vEvent["method_name"] = CefString(methodName).ToString();
									vEvent["param"] =  CefString(eventData).ToString();

									args->SetString(0, writer.write(vEvent));

									if (m_browser.get()){
										m_browser->SendProcessMessage(PID_RENDERER, msg);
										Sleep(100);
									}
									CNdCefShareMemory nipc;
									CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,"**********InvokeNativeMethod OpenFileMapping****************");
									if (nipc.OpenFileMemoryMapping(GLOBAL_MEMORY_NAME,GLOBAL_EVENT_NAME)){
										char pszData[MEMORY_SIZE];
										DWORD nSize = MEMORY_SIZE;
										CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,"************InvokeNativeMethod ReadMemory**************");
										if (nipc.ReadMemory(pszData,nSize,MEMORY_SIZE)){
											pszData[nSize] = '\0';
											Json::Reader reader;  
											Json::Value root;
											if (reader.parse(pszData,root)){
												std::string njsonstr(pszData); 
												CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,pszData);
												retval = CefV8Value::CreateString(njsonstr);
											}else{
												retval = CefV8Value::CreateString("{}");
												CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,"*************InvokeNativeMethod parse false*****************");
											}
										}else{
											retval = CefV8Value::CreateString("{}");
											CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,"*************InvokeNativeMethod ReadMemory false**********");
										}
									}else{
										CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,"**********InvokeNativeMethod OpenFileMapping false*****************");
										retval = CefV8Value::CreateString("{}");
									}
									CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,"*************InvokeNativeMethod OpenFileMapping****************");
								}
							}
						} else if (name == kregisterListener) {
							CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,"**************registerListener2");
							if (arguments.size() == 2 && arguments[0]->IsString()&& arguments[1]->IsString()){
								std::wstring eventKey = arguments[0]->GetStringValue();
								std::wstring eventCallBack = arguments[1]->GetStringValue();
								CefRefPtr<CefProcessMessage> msg= CefProcessMessage::Create("registerListener");
								CefRefPtr<CefListValue> args = msg->GetArgumentList();
								args->SetString(0, eventKey);
								args->SetString(1, eventCallBack);
								if (m_browser.get()){
									m_browser->SendProcessMessage(PID_RENDERER, msg);
								}
								CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,"**************registerListener");
								CNdDebug::wprintf(L"%s%s",eventKey.c_str(),eventCallBack.c_str());
							}
							retval = CefV8Value::CreateBool(true);
						} else if (name == kunRegisterListener) {
							if (arguments.size() == 2 && arguments[0]->IsString()&& arguments[1]->IsString()){
								std::wstring eventKey = arguments[0]->GetStringValue();
								std::wstring eventCallBack = arguments[1]->GetStringValue();
								CefRefPtr<CefProcessMessage> msg= CefProcessMessage::Create("unRegisterListener");
								CefRefPtr<CefListValue> args = msg->GetArgumentList();
								args->SetString(0, eventKey);
								args->SetString(1, eventCallBack);
								if (m_browser.get()){
									m_browser->SendProcessMessage(PID_RENDERER, msg);
								}
								CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,"**************unRegisterListener");
								CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,"kunRegisterListener:%s,%s",arguments[0]->GetStringValue().ToString().c_str(),arguments[1]->GetStringValue().ToString().c_str());
							}
							retval = CefV8Value::CreateBool(true);
						} else if (name == kprintLog) {
							if (arguments.size()>0 && arguments[0]->IsString()) {
								CNdDebug::logMsg(LOG_FOLDER_LOG_Consolelog,arguments[0]->GetStringValue().ToString().c_str());
							}
						}else if (name == kprepareSwitchPage){
							retval = CefV8Value::CreateString("{}");
						}else if (name == konPageLoaded){
							
						}
						return true;
				}

			private:
				IMPLEMENT_REFCOUNTING(V8Handler);
			};

			class RenderDelegate : public CNdCefApp::RenderDelegate {
			public:
				RenderDelegate() {
				}

				virtual void OnContextCreated(CefRefPtr<CNdCefApp> app,
					CefRefPtr<CefBrowser> browser,
					CefRefPtr<CefFrame> frame,
					CefRefPtr<CefV8Context> context) OVERRIDE {
						CefRefPtr<CefV8Value> object = context->GetGlobal();

						CefRefPtr<CefV8Handler> handler = new V8Handler();
						//CefRefPtr<CefV8Accessor> accessor = new Accessor();

						CefRefPtr<CefV8Value> CoursePlayerobj =  CefV8Value::CreateObject(NULL);
						CefRefPtr<CefV8Value> closefun =  CefV8Value::CreateFunction(kclose, handler);
						CefRefPtr<CefV8Value> drawMethodfun =  CefV8Value::CreateFunction(kdrawMethod, handler);
						
						CefRefPtr<CefV8Value> PCInterfaceobj =  CefV8Value::CreateObject(NULL);
						CefRefPtr<CefV8Value> invokeMethodfun =  CefV8Value::CreateFunction(kinvokeMethod, handler);
						CefRefPtr<CefV8Value> invokeMethodAsyncFun =  CefV8Value::CreateFunction(kinvokeMethodAsync, handler);
						CefRefPtr<CefV8Value> printLogfun =  CefV8Value::CreateFunction(kprintLog, handler);
						
						CefRefPtr<CefV8Value> registerListenerfun =  CefV8Value::CreateFunction(kregisterListener, handler);
						CefRefPtr<CefV8Value> unRegisterListenerfunc =  CefV8Value::CreateFunction(kunRegisterListener, handler);
						CefRefPtr<CefV8Value> prepareSwitchPagefun =  CefV8Value::CreateFunction(kprepareSwitchPage, handler);
						CefRefPtr<CefV8Value> onPageLoaded =  CefV8Value::CreateFunction(konPageLoaded, handler);
						((V8Handler*)(handler.get()))->m_browser = browser;
						PCInterfaceobj->SetValue(kinvokeMethod,invokeMethodfun,
							V8_PROPERTY_ATTRIBUTE_READONLY);
						PCInterfaceobj->SetValue(kinvokeMethodAsync,invokeMethodAsyncFun,
							V8_PROPERTY_ATTRIBUTE_READONLY);
						PCInterfaceobj->SetValue(kprintLog,printLogfun,
							V8_PROPERTY_ATTRIBUTE_READONLY);
						PCInterfaceobj->SetValue(konPageLoaded,onPageLoaded,
							V8_PROPERTY_ATTRIBUTE_READONLY);
						PCInterfaceobj->SetValue(kprepareSwitchPage,prepareSwitchPagefun,
							V8_PROPERTY_ATTRIBUTE_READONLY);
						PCInterfaceobj->SetValue(kregisterListener,registerListenerfun,
							V8_PROPERTY_ATTRIBUTE_READONLY);
						PCInterfaceobj->SetValue(kunRegisterListener,unRegisterListenerfunc,
							V8_PROPERTY_ATTRIBUTE_READONLY);

						
						if (app.get()){
							if (app->GetRegisterJs()){
								CNdDebug::logMsg(LOG_FOLDER_LOG_InvokeMethod,"%s%d","-----------GetRegisterJs:",app->GetRegisterJs());
								object->SetValue(kPCInterface,PCInterfaceobj,
									V8_PROPERTY_ATTRIBUTE_READONLY);
							}
						}

						CoursePlayerobj->SetValue(kclose,closefun,
							V8_PROPERTY_ATTRIBUTE_READONLY);
						CoursePlayerobj->SetValue(kdrawMethod,drawMethodfun,
							V8_PROPERTY_ATTRIBUTE_READONLY);
						

						object->SetValue(kCoursePlayer,CoursePlayerobj,
							V8_PROPERTY_ATTRIBUTE_READONLY);
						
				}
				void OnUncaughtException(CefRefPtr<CefBrowser> browser,
					CefRefPtr<CefFrame> frame,
					CefRefPtr<CefV8Context> context,
					CefRefPtr<CefV8Exception> exception,
					CefRefPtr<CefV8StackTrace> stackTrace) {
						std::string strStackTrace;
						for (int i = 0; i < stackTrace->GetFrameCount(); i++)
						{
							CefRefPtr<CefV8StackFrame> frame = stackTrace->GetFrame(i);
							char szStack[1001] = { '\0' };
							_snprintf(szStack, 1000, "%s: %s(%d)\n", frame->GetScriptNameOrSourceURL().ToString().c_str(),
								frame->GetFunctionName().ToString().c_str(), frame->GetLineNumber());
							strStackTrace += szStack;
						}
						if (strStackTrace.size() > 32 * 1024)
						{
							strStackTrace.substr(0, 30 * 1024);
						}
						CNdDebug::logMsg(LOG_FOLDER_LOG_UncaughtException, "Uncaught JavaScript Exception: BrowserId(%d), Msg: %s---------------%s(%d)\n%s",
							browser->GetIdentifier(), exception->GetMessage().ToString().c_str(), 
							exception->GetScriptResourceName().ToString().c_str(), 
							exception->GetLineNumber(), strStackTrace.c_str());
				}
			private:
				IMPLEMENT_REFCOUNTING(RenderDelegate);
			};

		}  
		bool FindFirstFileExists(LPCTSTR lpPath, DWORD dwFilter)
		{
			WIN32_FIND_DATA fd;
			HANDLE hFind = FindFirstFile(lpPath, &fd);
			bool bFilter = (FALSE == dwFilter) ? TRUE : fd.dwFileAttributes & dwFilter;
			bool RetValue = ((hFind != INVALID_HANDLE_VALUE) && bFilter) ? TRUE : FALSE;
			FindClose(hFind);
			return RetValue;
		}
		void CreateRenderDelegates(CNdCefApp::RenderDelegateSet& delegates) {
			TCHAR szPath[MAX_PATH];
			if( GetModuleFileName( NULL, szPath, MAX_PATH ) )
			{
				std::wstring nPath(szPath);
				nPath = nPath.substr(0,nPath.find_last_of('\\'));
				std::wstring nLogPath = nPath +L"\\log"; 
				CNdDebug::printfStr("---------------nLogPath------------");
				if (FindFirstFileExists(nLogPath.c_str(),false)&&CNdDebug::m_pLogMgr==NULL){
					CNdDebug::printfStr("---------------CLogManager------------");
					CNdDebug::m_pLogMgr = new CLogManager();
					if (CNdDebug::m_pLogMgr) {
						CNdDebug::printfStr("---------------m_pLogMgr------------");
						CNdDebug::m_pLogMgr->SetPath(nLogPath);
						CNdDebug::m_pLogMgr->init();
					}
				}
			}
	
			delegates.insert(new RenderDelegate);
		}
	}
}  
