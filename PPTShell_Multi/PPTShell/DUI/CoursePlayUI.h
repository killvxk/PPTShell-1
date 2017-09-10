#pragma once

#include "DUICommon.h"
#include "Util/Util.h"
#include "Util/Singleton.h"
#include "ThirdParty/cef/cefobj.h"
#include "NDCloud/NDCloudQuestion.h"

enum CoursePlay_Type{
	COURSEPLAY_ADD = 1,
	COURSEPLAY_EDITOR,
	COURSEPLAY_PREVIEW,
	COURSEPLAY_PLAYER,
};

class CCoursePlayUI : public WindowImplBase
{
public:
	CCoursePlayUI();
	~CCoursePlayUI();

	UIBEGIN_MSG_MAP

		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("close"),	OnCloseBtn);

	UIEND_MSG_MAP

private:
	void				OnCloseBtn(TNotifyUI& msg);
	void				OnZoomDBClick();
	
public:
	LPCTSTR GetWindowClassName() const;	

	virtual void InitWindow();

	virtual CDuiString GetSkinFile();

	virtual CDuiString GetSkinFolder();

	virtual CControlUI* CreateControl(LPCTSTR pstrClass);

	bool	Initialize( HINSTANCE hInstance );

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK WndProc(
		HWND hwnd,      // handle to window
		UINT uMsg,      // message identifier
		WPARAM wParam,  // first message parameter
		LPARAM lParam   // second message parameter
		);

	void Init(WCHAR* nUrl ,CoursePlay_Type nType,BOOL nShow = TRUE);
	void Hide();
	void SizeMessage();
	void SetClientRect();
	void SetTitle(tstring nTitle);
	TCHAR* GetQuestionResType( tstring szQuestionResDescription );
	BOOL HideAddQuestionWindow();
	BOOL ShowAddQuestionWindow();
	BOOL ShowCoursePlayerShadow();
	BOOL HideCoursePlayerShadow();
	BOOL SetQuestionInfo( tstring szPath, tstring szQuestionType, tstring szGuid);
	tstring GetQuestionPath();
	void SetQuestionPath(tstring strQuestionPath);
private:
	CButtonUI*		m_pCloseBtn;
	CLabelUI*		m_pTopTitleLabel;
	CLabelUI*		m_pTitleLabel;
	CLabelUI*		m_pSubTitleLabel;
	
private:
	//CWebBrowserUI*	m_pWebBrowser;
	HWND			m_hWebWindow;

	HINSTANCE		m_hInstance;

	tstring			m_strWndClassName;

	int				m_nType;
	DWORD           m_dwStartTickTime;
	DWORD           m_dwDBClickTime;
	DWORD           m_dwDBSizeTime;
	DWORD           m_dwDBClickCount;
	BOOL            m_dwDBClickFlag;
	BOOL			m_bSubTitle;
	tagQuestionOptType m_nQuestType;
	std::wstring	m_nUrl;
	tstring			m_strQuestionPath;
	int m_nErrorCount;
	CRect m_MainRect;
	bool m_nHiddenFlag;
};

typedef Singleton<CCoursePlayUI> CoursePlayUI;