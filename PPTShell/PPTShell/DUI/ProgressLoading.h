#pragma once

#include "Http/HttpDelegate.h"

class CProgressLoadingUI
	: public WindowImplBase
{
public:
	CProgressLoadingUI(void);
	virtual ~CProgressLoadingUI(void);

public:
	UIBEGIN_MSG_MAP
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("close"),			OnBtnClose);
	UIEND_MSG_MAP

protected:
	virtual void		InitWindow();
	virtual CDuiString	GetSkinFolder();
	virtual CDuiString	GetSkinFile();
	virtual LPCTSTR		GetWindowClassName( void ) const;

public:
	BOOL				Start(tstring strCommandLine, CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressDelegate);
	void				Stop();
	void				ShowWindow( bool bShow = true, bool bTakeFocus = true );

protected:
	BOOL				ParseCommandLine(tstring strCommandLine);
	bool				OnUserLogined(void* param);
	bool				OnDownloadPathObtained(void * pParam);
	bool				OnCourseFileDownloaded(void* param);
	bool				OnCourseFileDownloading(void* param);

protected:
	bool				OnEvent(void * pParam);
	void				OnBtnClose(TNotifyUI& msg);

	void				ShowProgress(bool bVisible);
	void				SetProgress(int nPos);
	void				SwitchTo(bool bSuccess);

private:
	//	CWndShadow			m_wndShadow;
	CContainerUI*		m_laySaving;
	CContainerUI*		m_laySuccess;
	CContainerUI*		m_layFail;
	CProgressUI*		m_proDownload;
	CLabelUI*			m_pTitle;
	CEventSource		m_OnClose;

	BOOL				m_bWebSiteStartup;
	tstring				m_strCourseGuid;
	tstring				m_strChapterGuid;
	tstring				m_strMode;
	tstring				m_strAuth;

	tstring				m_strAccessToken;
	tstring				m_strNonce;
	tstring				m_strMac;
};
