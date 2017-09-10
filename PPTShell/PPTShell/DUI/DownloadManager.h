#pragma once


class CResourceDownloader;
class IDownloaderListener;
class CDownloadManagerUI: 
	public WindowImplBase,
	public IDownloaderListener
{

private:
	CDownloadManagerUI();
	~CDownloadManagerUI();
	UIBEGIN_MSG_MAP
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("dm_start_all"),			OnBtnStartAll);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("dm_pause_all"),			OnBtnPauseAll);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("dm_cancel_all"),		OnBtnCancelAll);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("dm_clean_all"),			OnBtnCleanAll);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("dm_tab_header"),		OnBtnTabHeader);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("dm_btn_close"),			OnBtnClose);
		EVENT_HANDLER(_T("itemremove"),									OnDownloadItemRemove);
	UIEND_MSG_MAP

private:
	//notify
	void				OnBtnPauseAll( TNotifyUI& msg );
	void				OnBtnStartAll( TNotifyUI& msg );
	void				OnBtnCancelAll( TNotifyUI& msg );
	void				OnBtnCleanAll( TNotifyUI& msg );
	void				OnBtnTabHeader( TNotifyUI& msg );
	void				OnDownloadItemRemove( TNotifyUI& msg );
	void				OnBtnClose( TNotifyUI& msg );

protected:
	//super
	virtual void		InitWindow();
	virtual CDuiString	GetSkinFolder();
	virtual CDuiString	GetSkinFile();
	virtual LPCTSTR		GetWindowClassName( void ) const;
	virtual LRESULT		OnKillFocus( UINT , WPARAM , LPARAM , BOOL& bHandled );

	//IDownloaderListener
	virtual void		OnDownloaderCreate(CResourceDownloader* pDownloader, CItemHandler* pItemHandler);
	virtual void		OnDownloaderDestroy( CResourceDownloader* pDownloader , int nResult);

	//others
	LPCTSTR				GetItemIcon(int nType);
	void				InitDownloadingList();
	void				SwitchDownloadingTabTo(bool bList);
	void				SwitchDownloadedTabTo(bool bList);
	CControlUI*			GetDownloadingItem(CItemHandler* pHandler);
	CControlUI*			GetDownloadedItem(CResourceDownloader* pDownloader);
	void				UpdateDownloadCount();
	bool				DoTipDialog(LPCTSTR lptcsTip);

public:
	void				UpdatePos();
	void				ShowManagerUI(bool bShow);
	static CDownloadManagerUI*	GetInstance();



private:
	bool				m_bIsTipping;
	//ui
	CControlUI*			m_btnResumeAll;
	CControlUI*			m_btnPauseAll;
	CTabLayoutUI*		m_tabDownloadMgr;
	CContainerUI*		m_layDownloadingList;
	CContainerUI*		m_layDownloadedList;
	CContainerUI*		m_layDownloading;
	CContainerUI*		m_layDownloaded;
	CContainerUI*		m_layDownloadingNone;
	CContainerUI*		m_layDownloadedNone;

	static CDownloadManagerUI*	m_pInstance;
};