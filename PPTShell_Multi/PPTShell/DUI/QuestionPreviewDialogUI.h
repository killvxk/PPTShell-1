#pragma once
#include "Util\Stream.h"
#include "DUICommon.h"

class CCloudResourceHandler;
class CQuestionDownloader;

enum QUESTION_TYPE
{
	QUESTION_CLOUD,				//云端基础习题
	QUESTION_INTERACTIVE_CLOUD,	//云端互动习题
	QUESTION_LOCAL,				//本地基础习题
	QUESTION_INTERACTIVE_LOCAL,	//本地互动习题
};

class CQuestionPreviewDialogUI: public WindowImplBase
{
	UIBEGIN_MSG_MAP
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("btnClose"),OnBtnCloseClick);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("btnThumbnailPreview"),OnBtnThumbnailPreviewClick);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("btnThumbnailNext"),OnBtnThumbnailNextClick);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("btnQuestionPreview"),OnBtnQuestionPreviewClick);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("btnQuestionNext"),OnBtnQuestionNextClick);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("btnFullScreen"),OnBtnFullScreenClick);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("btnInsert"),OnBtnInsertClick);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("btnCancelInsert"),OnBtnCancelInsertClick);
	UIEND_MSG_MAP
public:
	CQuestionPreviewDialogUI(void);
	~CQuestionPreviewDialogUI(void);
	void InitData(CStream* pStream);
	void InitPosition();
	void SetQuestionType(QUESTION_TYPE type);
	QUESTION_TYPE GetQuestionType();
	wstring GetUrl();
	void AddRetryCount();
	int GetRetryCount();
	static bool Initialize( HINSTANCE hInstance );
	static LRESULT CALLBACK WndProc(
		HWND hwnd,      // handle to window
		UINT uMsg,      // message identifier
		WPARAM wParam,  // first message parameter
		LPARAM lParam   // second message parameter
		);
	static DWORD WINAPI SetPosThread(LPARAM lpParam);
	void OnBtnQuestionPreviewClick(TNotifyUI& msg);
	void OnBtnQuestionNextClick(TNotifyUI& msg);
	void HideWindow();
	bool GetIsWebWindowShown();
	void ShowWebWindow();
private:
	virtual LPCTSTR GetWindowClassName() const;	
	virtual void InitWindow();
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void OnShown();
	virtual LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void OnBtnCloseClick(TNotifyUI& msg);
	void OnBtnThumbnailPreviewClick(TNotifyUI& msg);
	void OnBtnThumbnailNextClick(TNotifyUI& msg);
	void OnBtnFullScreenClick(TNotifyUI& msg);
	void OnBtnInsertClick(TNotifyUI& msg);
	void OnBtnCancelInsertClick(TNotifyUI& msg);
	void ShowQuestion(wstring url);
	void HideQuestion();
	bool OnThumbnailClick(void* pObj);
	bool OnDownloadCompeleted( void* pObj );
	int GetDirecotySize(CString strDirectoryPath);//获取文件夹下的文件大小
	static DWORD  WINAPI ScanThread(LPARAM lParam);
	bool OnHandleInsert( void* pObj );
	bool OnInsertCompleted( void* pObj );

	HANDLE m_hThread;
	map<CCloudResourceHandler*,CQuestionDownloader*> m_mDownloaders;
	int m_nInsertCount;
	QUESTION_TYPE m_eType;
	int m_nScrollWidth;
	HANDLE m_phSetPosThread;
	int m_nRetryCount;//重试次数
	wstring m_sCurrentUrl;
	int m_nCurrentIndex;
	bool m_bIsWebShown;
	HWND m_hWebWindow;
	CLabelUI* m_pLblTitle;
	CLabelUI* m_pLblFileSizeDesc;
	CLabelUI* m_pLblFileSize;
	CGifAnimUI* m_pGifLoading;
	CVerticalLayoutUI* m_pLayoutQuestion;
	CHorizontalLayoutUI* m_pLayoutThumbnailList;
	CButtonUI* m_pBtnFullScreen;
	CButtonUI* m_pBtnInsert;
	CButtonUI* m_pBtnCancelInsert;
};

typedef Singleton<CQuestionPreviewDialogUI> QuestionPreviewDialogUI;
