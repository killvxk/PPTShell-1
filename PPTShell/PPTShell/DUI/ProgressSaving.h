#pragma once



class CProgressSavingUI
	: public WindowImplBase
{
public:
	CProgressSavingUI(void);
	virtual ~CProgressSavingUI(void);

public:
	UIBEGIN_MSG_MAP
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("close"),			OnBtnClose);
	UIEND_MSG_MAP

protected:
	virtual void		InitWindow();

	virtual CDuiString	GetSkinFolder();

	virtual CDuiString	GetSkinFile();

	virtual LPCTSTR		GetWindowClassName( void ) const;

	virtual LRESULT		HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);


public:
	bool				Start(LPCTSTR lptcsPath, CDelegateBase* OnCloseDelegate = NULL, int nPresentCount = 0);

	void				Stop();

	void				ShowWindow( bool bShow = true, bool bTakeFocus = true );


private:
	bool				OnTitleQueried(void* param);

	bool				OnSaveProgress(void* pObj);

	bool				OnSaveCompleted(void* pObj);

	bool				OnCommitCompleted(void * pParam);

	bool				OnEvent(void * pParam);

	void				OnBtnClose(TNotifyUI& msg);

protected:
	DWORD				UploadFile(LPCTSTR lptcsUrl, LPCTSTR lptcsSessionId, LPCTSTR lptcsUploadGuid, LPCTSTR lptcsServerFilePath);

	void				ShowProgress(bool bVisible);

	void				SetProgress(int nPos);

	void				SwitchTo(bool bSuccess);

private:
//	CWndShadow			m_wndShadow;
	CContainerUI*		m_laySaving;
	CContainerUI*		m_laySuccess;
	CContainerUI*		m_layFail;
	CProgressUI*		m_proDownload;
	CEventSource		m_OnClose;

	bool				m_bSuccess;
	DWORD				m_dwUploadId;
	DWORD				m_dwDownloadServerPathId;
	DWORD				m_dwCommitId;
	tstring				m_strUploadGuid;
	tstring				m_strServerPath;
	tstring				m_strLocalPath;
	tstring				m_strTitle;
	
	int					m_nPresentCount;
	bool				m_bCover;

};
