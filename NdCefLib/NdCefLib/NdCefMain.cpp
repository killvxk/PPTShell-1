#include "stdafx.h"
#include "NdCefMain.h"
#include "NdCefApp.h"
#include "NdCefHandler.h"
#include "NdCefPlayWindow.h"

#define MAX_URL_LENGTH  255

HINSTANCE g_dllhInstance = NULL;

namespace NdCef{
	bool CNdCefMain::m_Shutdown = false;
	bool CNdCefMain::m_ShowDevtool = false;
	
	CNdCefMain::CNdCefMain() 
	{
		m_SubprocessCmdNoReg = false;
		m_bSubprocessPath = false;
		CNdDebug::printfStr("CNdCefMain");
	}
	CNdCefMain::~CNdCefMain() 
	{
		if (!m_Shutdown){
			CNdDebug::printfStr("~CNdCefMain Shutdown");
			if (m_bSubprocessPath){
				Shutdown(true);
			}
		}
		CNdDebug::printfStr("~CNdCefMain");
	}
	extern CefRefPtr<CNdCefHandler> g_hNdCefHandle;

	int CNdCefMain::RunMain(bool bSubprocessPath){
		void* sandbox_info = NULL;

		CefMainArgs main_args(::GetModuleHandle(NULL));
		CefRefPtr<NdCef::CNdCefApp> app(new NdCef::CNdCefApp);
	
		int exit_code = CefExecuteProcess(main_args, app.get(), NULL);
		if (exit_code >= 0){
			CNdDebug::printfStr("CefExecuteProcess");
			return exit_code;
		}
		m_bSubprocessPath = bSubprocessPath; 
		TCHAR szPath[MAX_PATH];
		if( !GetModuleFileName( GetInstance(), szPath, MAX_PATH ) )
		{
			return 0;
		}
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
		if (bSubprocessPath){
			context.reset(new CNdCefContext(0, NULL));
			CefSettings settings;
			settings.no_sandbox = false;
			settings.multi_threaded_message_loop = true;    //使用主程序消息循环
			settings.single_process = false;                 //使用多进程模式
			settings.ignore_certificate_errors = true;      //忽略掉ssl证书验证错误
			settings.command_line_args_disabled = true;
			//context->PopulateSettings(&settings);

			message_loop.reset(new CNdCefMessageLoop());

			std::wstring configPath = nPath;
			std::wstring devtoolPath = nPath;
			nPath += TEXT("\\NdCefClient.exe");
	
			devtoolPath += TEXT("\\devtools_resources.pak");
			configPath += TEXT("\\..\\..\\setting\\Config.ini");
		
			HWND hwMain = (HWND)GetPrivateProfileInt(TEXT("config"), TEXT("MainHwnd"), 0, configPath.c_str());
			CNdCefHandler::SetPPTMainWnd(hwMain);
			CNdDebug::wprintf(L"%s",nPath.c_str());
			CNdDebug::wprintf(L"%d%s",hwMain,configPath.c_str());
			CNdDebug::logMsg(LOG_FOLDER_LOG_CEF,"********bSubprocessPath***************");
			m_ShowDevtool = FindFirstFileExists(devtoolPath.c_str(),false);

			settings.log_severity = LOGSEVERITY_DISABLE;

			if (m_SubprocessCmdNoReg){
				CNdCefHandler::SetRegisterJs(false);
			}

			if (m_ShowDevtool){
				settings.remote_debugging_port = 8088;
			}
		
			CefString(&settings.browser_subprocess_path).FromWString(nPath);
		
			CNdDebug::printfStr("---------------CefInitialize Start------------");
			CefInitialize(main_args, settings, app.get(), NULL);
			CNdDebug::printfStr("---------------CefInitialize Ok------------");
		}
		return 0;
	}
	bool CNdCefMain::ShowMain(HWND hParentWnd){
		std::wstring nUrl = TEXT("F:/NDCloud/Package/player/index.html?main=/F:/NDCloud/Question/4b8cf3ef-f597-4704-a7c5-2c8ad32d6dbc_default/main.xml");
			////F:/NDCloud/Package/tools/平面几何/等边三角形.html");
		return ShowMainUrl(hParentWnd,nUrl);
	}
	void CNdCefMain::SetPPTMainWnd(HWND nPPTWnd){
		CNdCefHandler::SetPPTMainWnd(nPPTWnd);
	}
	void CNdCefMain::SendEsc2PPT(){
		if (g_hNdCefHandle.get()){
			g_hNdCefHandle->SendEsc2PPT();
		}
	}
	void CNdCefMain::SetSubprocessCmdNoReg(){
		m_SubprocessCmdNoReg = true;
	}
	bool CNdCefMain::ChangeUrl(HWND hParentWnd,std::wstring nUrl){
		bool nResult = false;
		if (g_hNdCefHandle.get()){
			nResult = g_hNdCefHandle->ChangeUrl(hParentWnd,nUrl);
		}
		return nResult;
	}
	bool CNdCefMain::GetDevtoolFlag(){
		return m_ShowDevtool;
	}
	bool CNdCefMain::FindFirstFileExists(LPCTSTR lpPath, DWORD dwFilter)
	{
		WIN32_FIND_DATA fd;
		HANDLE hFind = FindFirstFile(lpPath, &fd);
		bool bFilter = (FALSE == dwFilter) ? TRUE : fd.dwFileAttributes & dwFilter;
		bool RetValue = ((hFind != INVALID_HANDLE_VALUE) && bFilter) ? TRUE : FALSE;
		FindClose(hFind);
		return RetValue;
	}
	bool CNdCefMain::ClearCookie(std::wstring nUrl,std::wstring nCookieName){
		bool nResult = true;
		if (g_hNdCefHandle.get()){
			g_hNdCefHandle->ClearCookie(nUrl,nCookieName);
		}
		return nResult;
	}
	void CNdCefMain::SetPopup(HWND hParent,int nPop){
		if (g_hNdCefHandle.get()){
			g_hNdCefHandle->SetPopup(hParent,nPop);
		}
	}
	void CNdCefMain::SetQuestionType(HWND hParent,int nType){
		if (g_hNdCefHandle.get()){
			g_hNdCefHandle->SetQuestionType(hParent,nType);
		}
	}
	void CNdCefMain::SizeMessage(HWND hParentWnd){
		CefRefPtr<CefBrowser> browser = g_hNdCefHandle->FindBrowser(hParentWnd);
		if (browser.get()) {
			CefWindowHandle hWnd = browser->GetHost()->GetWindowHandle();

			if (hWnd) 
			{
				HWND  hCefParent = GetParent(hWnd);
				if (IsWindow(hCefParent)){
					RECT rect;
					GetClientRect(hParentWnd, &rect);
					browser->GetHost()->NotifyMoveOrResizeStarted();
					HDWP hdwp = BeginDeferWindowPos(1);

					hdwp = DeferWindowPos(hdwp, hCefParent, NULL,
						rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
						SWP_NOZORDER);
					EndDeferWindowPos(hdwp);
				
				}
			}
		}
	}
	bool CNdCefMain::SetBrowserSizeChange(HWND hParentWnd){
		bool nResult = false;
		if (g_hNdCefHandle.get()){
			nResult = g_hNdCefHandle->SetBrowserSizeChange(hParentWnd);
		}
		return nResult;
	}
	bool CNdCefMain::CloseByWnd(HWND hParentWnd){
		bool nResult = false;
		if (g_hNdCefHandle.get()){
			nResult = g_hNdCefHandle->CloseByWnd(hParentWnd);
		}
		return nResult;
	}
	void  CNdCefMain::SetCefLib(){
		if (g_hNdCefHandle.get()){
			g_hNdCefHandle->SetCefLib();
		}
	}
	bool CNdCefMain::ShowMainUrl(HWND hParentWnd,std::wstring nUrl,bool nosr,bool ntransparent,std::wstring nthumbPath){
		CNdDebug::printf("%s %d","ShowMainUrl:",hParentWnd);
		bool nNeedCreate = true;//判断是否需要创建窗体
		bool nFindWnd = false;
		if (g_hNdCefHandle.get()){
			CNdDebug::wprintf(L"%d%s%s",g_hNdCefHandle->GetBrowserSize(),L"----",nUrl.c_str());
			nFindWnd = g_hNdCefHandle->FindBrowserWnd(hParentWnd);
			if (nFindWnd){
				std::wstring nBrowerUrl = g_hNdCefHandle->FindBrowserUrl(hParentWnd);
				CNdDebug::wprintf(L"%s",L"FindBrowserUrl:",nBrowerUrl.c_str());
				//URL相同不需要重新加载
				if (nBrowerUrl==nUrl){
					;//暂时无任何需要处理
					//nNeedCreate = false;
					//g_hNdCefHandle->SetMainWindowHandle(hParentWnd);
				}else{
					//切换URL
					//g_hNdCefHandle->ChangeUrl(hParentWnd,nUrl);
				}
			}
		}
		if (nNeedCreate){
			CreateMainUrl(hParentWnd,nUrl,nosr,ntransparent,nthumbPath);
		}
		return true;
	}
	void CNdCefMain::SetFocusToBrowser(HWND hParentWnd){
		if (g_hNdCefHandle.get()){
			g_hNdCefHandle->SetBrowserBlank(hParentWnd);
			CefRefPtr<CefBrowser> browser = g_hNdCefHandle.get()->FindBrowser(hParentWnd);
			if (browser){
				g_hNdCefHandle->SetFocusToBrowser(browser);
			}
		}
	}
	bool CNdCefMain::CreateMainUrl(HWND hParentWnd,std::wstring nUrl,bool nosr,bool ntransparent,std::wstring nthumbPath){
		scoped_ptr<CNdCefWindow> window(new CNdCefWindow(nUrl));
		//window.reset();
		if (window.get()){
			if (window->WinMain(GetInstance(),hParentWnd,nosr,ntransparent,nthumbPath)){
				CNdDebug::printfStr("RunMain WinMain");

			}else{
				CNdDebug::printfStr("RunMain WinMain Error");
				return false;
			}
		}
		return true;
	}
	bool CNdCefMain::CreatePlayBrower(HWND hParentWnd,std::wstring nTitle,std::wstring nUrl,bool nClose){
		scoped_ptr<CNdCefPlayWindow> window(new CNdCefPlayWindow());
		//window.reset();
		if (window.get()){
			window->InitWindowInfo(nTitle,nUrl,nClose);
			if (window->WinMain(GetInstance(),NULL)){
				CNdDebug::printfStr("RunMain WinMain");

			}else{
				CNdDebug::printfStr("RunMain WinMain Error");
				return false;
			}
		}
		return true;
	}
	int CNdCefMain::MsgRun(){
		return  message_loop->Run(GetInstance());
	}
	void CNdCefMain::CloseAllBrowsers(){
		/*
		if (g_hNdCefHandle.get()){
			CNdDebug::printfStr("CloseAllBrowsers");
			g_hNdCefHandle->RegisterJS(NULL);
			g_hNdCefHandle->CloseAllBrowsers(true);
		}
		CefQuitMessageLoop();
		*/
	}
	void CNdCefMain::SetSlideIndex(HWND hParent,LONG nIndex){
		CNdDebug::printf("%s %d %s %d","SlideWnd",hParent,"SlideIndex",nIndex);
		m_SlideIndex[hParent] = nIndex;
	}
	void CNdCefMain::SetFocus(HWND hParent,LONG nIndex){

	}
	bool CNdCefMain::ExecuteJS(HWND hParent,LPCTSTR nJSCode){
		bool nResult = false;
		if (g_hNdCefHandle.get()){
			nResult = g_hNdCefHandle->ExecuteJS(hParent,nJSCode);
		}
		return nResult;
	}
	bool CNdCefMain:: ExecuteJSCallBack(HWND hParent,LPCTSTR eventName,LPCTSTR nParam){
		bool nResult = false;
		if (g_hNdCefHandle.get()){
			nResult = g_hNdCefHandle->ExecuteJSCallBack(hParent,eventName,nParam);
		}
		return nResult;
	}
	bool CNdCefMain::RegisterJS(NdCefIRegisterJs* nJsFunObj){
		bool nResult = false;
		if (g_hNdCefHandle.get()){
			nResult = g_hNdCefHandle->RegisterJS(nJsFunObj);
		}
		return nResult;
	}
	
	int CNdCefMain::Shutdown(bool force_close){
		if (CNdDebug::m_pLogMgr){
			CNdDebug::m_pLogMgr->destroy();
			delete CNdDebug::m_pLogMgr;
			CNdDebug::m_pLogMgr = NULL;
		}
		if (!m_Shutdown){
			m_Shutdown = true;
			if (g_hNdCefHandle.get()){
				CNdDebug::printfStr("CloseAllBrowsers");
				g_hNdCefHandle->RegisterJS(NULL);
				g_hNdCefHandle->CloseAllBrowsers(true);
			}
		
			CNdDebug::printfStr("CNdCefMain::CefShutdown");
			if (force_close){
				message_loop.reset();
				context.reset();
				exit(0);
			}else{
				CefShutdown();
				CNdDebug::printfStr("CNdCefMain::CefShutdown OK");
				message_loop.reset();
				context.reset();
				CNdDebug::printfStr("CNdCefMain::CefShutdown OK Exit");
			}
			/*
			//if (message_loop.get()){
			//	message_loop->Quit();
			//}
			//CefQuitMessageLoop();
			//CloseAllBrowsers();
			if (g_hNdCefHandle.get()){
				g_hNdCefHandle->Release();
			}*/
		}
		return 0;
	}
	void CNdCefMain::SetInstance(HINSTANCE hInstance){
		g_dllhInstance = hInstance;
	}
	HINSTANCE CNdCefMain::GetInstance(){
		if (g_dllhInstance==NULL){
			g_dllhInstance = ::GetModuleHandle(NULL);
		}
		return g_dllhInstance;
	}
}

