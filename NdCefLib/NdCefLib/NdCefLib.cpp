#include "stdafx.h"

#include "include/cef_app.h"
#include "include/base/cef_scoped_ptr.h"
#include "NdCefMain.h"

using namespace NdCef;

scoped_ptr<CNdCefMain> nCefMain;

bool NdCefInit(){
	CNdDebug::printfStr("NdCefInit");
	if (!nCefMain.get()){
		nCefMain.reset(new CNdCefMain());
		nCefMain.get()->RunMain();
		CNdDebug::printfStr("RunMain");
	}
	return true;
}
bool NdCefPop(){
	if (nCefMain.get()){
		nCefMain.get()->ShowMain(NULL);
		CNdDebug::printfStr("NdCefPop");
	}
	return true;
}
bool NdCefShow(HWND hParent){
	
	if (nCefMain.get()){
		nCefMain.get()->ShowMain(hParent);
		CNdDebug::printfStr("NdCefShow");
	}
	return true;
}
bool NdCefClearCookie(LPCTSTR nUrl,LPCTSTR nCookieName){
	if (nCefMain.get()){
		nCefMain.get()->ClearCookie(nUrl,nCookieName);
		CNdDebug::printfStr("NdCefShowbyUrl");
	}
	return true;
}
bool NdCefShowbyUrl(HWND hParent,LPCTSTR nUrl){
	if (nCefMain.get()){
		nCefMain.get()->ShowMainUrl(hParent,nUrl);
		nCefMain.get()->SetCefLib();
		CNdDebug::printfStr("NdCefShowbyUrl");
	}
	return true;
}
//设置Pop 0默认浏览器打开  1.消息通知  2.CEF弹出
bool NdCefPopup(HWND hParent,int nPop){

	if (nCefMain.get()){
		nCefMain.get()->SetPopup(hParent,nPop);
		CNdDebug::printfStr("NdCefPop");
	}
	return true;
}
//设置编辑器类型  0 添加 1 修改
bool NdCefQuestionType(HWND hParent,int nType){

	if (nCefMain.get()){
		nCefMain.get()->SetQuestionType(hParent,nType);
		CNdDebug::printfStr("NdCefQuestionType");
	}
	return true;
}
bool NdCefSizeChange(HWND hParent){
	if (nCefMain.get()){
		nCefMain.get()->SizeMessage(hParent);
		CNdDebug::printfStr("NdCefSizeChange");
	}
	return true;
}
bool NdCefChangeUrl(HWND hParent,LPCTSTR nUrl){
	if (nCefMain.get()){
		nCefMain.get()->ChangeUrl(hParent,nUrl);
		nCefMain.get()->SetCefLib();
		CNdDebug::printfStr("NdCefChangeUrl");
	}
	return true;
}
bool NdCefCloseByWnd(HWND hParent){
	if (nCefMain.get()){
		nCefMain.get()->CloseByWnd(hParent);
		CNdDebug::printfStr("CloseByWnd");
	}
	return true;
}

bool NdCefExecuteJS(HWND hParent,LPCTSTR nJSCode){
	bool nResult = false;
	if (nCefMain.get()){
		nResult = nCefMain.get()->ExecuteJS(hParent,nJSCode);
		CNdDebug::printfStr("NdCefExecuteJS");
	}
	return nResult;
}

bool NdCefExecuteJSCallBack(HWND hParent,LPCTSTR eventName,LPCTSTR nParam){
	bool nResult = false;
	if (nCefMain.get()){
		nResult = nCefMain.get()->ExecuteJSCallBack(hParent,eventName,nParam);
		CNdDebug::printfStr("NdCefExecuteJS");
	}
	return nResult;
}

void NdCefUnInit(){
	CNdDebug::printfStr("NdCefUnInit");
	
	if (nCefMain.get()){
		nCefMain.get()->CloseAllBrowsers();
		nCefMain.get()->Shutdown();
		CNdDebug::printfStr("CefShutdown");
	}
}

int NdCefSum(int a, int b) 
{ 
	CNdDebug::printf("%s%d","NdCefSum:",a+b);
	return a+b; 
} 

int unzipmain(int argc,char * argv[]);
int zipmain(int argc,char *argv[]);

bool NdCefUnZip(LPCSTR filezip,LPCSTR filepath){

	char* argv[4];
	argv[0] = "miniunz";
	argv[1] = (char *)filezip;
	argv[2] = "-d";
	argv[3] = (char *)filepath;
	unzipmain(4,argv);
	return true;
}
bool NdCefZip(LPCSTR filezip,LPCSTR filepath){
	char* argv[4];
	argv[0] = "minizip";
	argv[1] = "-J";
	argv[2] = (char *)filezip;
	argv[3] = (char *)filepath;
	zipmain(4,argv);
	//zipmain(filezip,filepath);
	return true;
}