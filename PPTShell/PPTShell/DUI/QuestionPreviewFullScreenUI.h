#pragma once
#include "DUICommon.h"
#include "QuestionPreviewDialogUI.h"

class CQuestionPreviewFullScreenUI: public WindowImplBase
{
	UIBEGIN_MSG_MAP
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("btnClose"),OnBtnCloseClick);
	UIEND_MSG_MAP
public:
	CQuestionPreviewFullScreenUI(void);
	~CQuestionPreviewFullScreenUI(void);
	static CQuestionPreviewFullScreenUI* GetInstance();
	void SetPreviewDlg(CQuestionPreviewDialogUI* pDlg);
	void InitData(CStream* pStream);
	tstring GetUrl();
	void AddRetryCount();
	int GetRetryCount();
	static bool Initialize( HINSTANCE hInstance );
	static LRESULT CALLBACK WndProc(
		HWND hwnd,      // handle to window
		UINT uMsg,      // message identifier
		WPARAM wParam,  // first message parameter
		LPARAM lParam   // second message parameter
		);
	void ShowQuestion(tstring url);
	void HideWindow();
private:
	virtual LPCTSTR GetWindowClassName() const;	
	virtual void InitWindow();
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	void ShowWebWindow();
	virtual void OnShown();
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	static DWORD  WINAPI ScanThread(LPARAM lParam);

	void OnBtnCloseClick(TNotifyUI& msg);
	void HideQuestion();

	CQuestionPreviewDialogUI* m_pPreviewDlg;
	tstring m_sCurrentUrl;
	int m_nCurrentIndex;
	int m_nRetryCount;//÷ÿ ‘¥Œ ˝
	HWND m_hWebWindow;
	HANDLE m_hThread;
	CVerticalLayoutUI* m_pLayoutQuestion;
	CGifAnimUI* m_pGifLoading;
	CButtonUI* m_pBtnClose;
};
