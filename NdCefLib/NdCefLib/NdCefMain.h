#pragma once
#include "include/base/cef_scoped_ptr.h"
#include "NdCefWindow.h"
#include "NdCefMessageLoop.h"
#include "NdCefContext.h"
#include <map>

namespace NdCef{
	typedef std::map<HWND,LONG> SlideIndex;
	class CNdCefMain 
	{
	public:
		CNdCefMain();
		~CNdCefMain();
	public:
		scoped_ptr<CNdCefMessageLoop> message_loop;
		scoped_ptr<CNdCefContext> context;
		int RunMain(bool bSubprocessPath = true);
		bool ShowMain(HWND hParentWnd);
		bool ShowMainUrl(HWND hParentWnd,std::wstring nUrl,bool nosr = false,bool ntransparent = true,std::wstring nthumbPath = L"");
		bool CreateMainUrl(HWND hParentWnd,std::wstring nUrl,bool nosr,bool ntransparent,std::wstring nthumbPath);
		bool ChangeUrl(HWND hParentWnd,std::wstring nUrl);
		void CloseAllBrowsers();
		bool ExecuteJS(HWND hParent,LPCTSTR nJSCode);
		bool ExecuteJSCallBack(HWND hParent,LPCTSTR eventName,LPCTSTR nParam);
		bool RegisterJS(NdCefIRegisterJs* nJsFunObj);
		bool CreatePlayBrower(HWND hParentWnd,std::wstring nTitle,std::wstring nUrl,bool nClose);
		bool CloseByWnd(HWND hParentWnd);
		void SetFocusToBrowser(HWND hParentWnd);
		void SetFocus(HWND hParent,LONG nIndex);
		void SetSlideIndex(HWND hParent,LONG nIndex);
		int Shutdown(bool force_close=false);
		int ExecuteJavaScript();
		bool SetBrowserSizeChange(HWND hParentWnd);
		int MsgRun();
		static bool m_Shutdown;
		static bool m_ShowDevtool;
		bool m_bSubprocessPath;
		void SetInstance(HINSTANCE hInstance);
		void SetPPTMainWnd(HWND nPPTWnd);
		void SizeMessage(HWND hParentWnd);
		bool ClearCookie(std::wstring nUrl,std::wstring nCookieName);
		SlideIndex m_SlideIndex;
		static bool FindFirstFileExists(LPCTSTR lpPath, DWORD dwFilter);
		static bool GetDevtoolFlag();
		void SetCefLib();
		void SetSubprocessCmdNoReg();
		void SetPopup(HWND hParent,int nPop);
		void SetQuestionType(HWND hParent,int nType);
		void SendEsc2PPT();
	private:
		HINSTANCE GetInstance();
		bool m_SubprocessCmdNoReg;
	};
}

