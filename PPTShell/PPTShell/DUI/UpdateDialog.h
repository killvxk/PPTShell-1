#pragma once

#include "DUICommon.h"
#include "Util/Util.h"
#include "Update/UpdateOperation.h"
#include "DUI/WndShadow.h"
#include "DUI/LabelMulti.h"
#include "Util/Singleton.h"

#define UPDATE_TEMP_DIR		_T("temp\\")
#define UPDATE_EXE_NAME		_T("\\update.exe")

#define Update_window_height 364
#define Update_window_width  540

class CUpdateDialogUI : public WindowImplBase
{
public:
	CUpdateDialogUI();
	~CUpdateDialogUI();

	UIBEGIN_MSG_MAP
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("confirmBtn"),	OnConfirmBtn);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("closeBtn"),		OnCloseBtn);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("playnewBtn"),	OnPlayNewBtn);
	UIEND_MSG_MAP

public:
	LPCTSTR GetWindowClassName() const;	

	virtual void InitWindow();

	virtual CDuiString GetSkinFile();

	virtual CDuiString GetSkinFolder();

	virtual CControlUI* CreateControl(LPCTSTR pstrClass);

	void	setUpdateLog(LPCTSTR szUpdateLog);
	void	setUpdateVersion(LPCTSTR szUpdateVersion);
	void	setUpdate(BOOL bUnUpdate);
	void	setAllFilesSize(LPCTSTR szFilesSize);
	tstring	getLocalVersion();
	void	setDlgHwnd(HWND Hwnd);

	void	TimingUpdate();//定时更新

	static void	TimerProcComplete(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

	int		GetTimingUpdateCount();
	void	SetTimingUpdateCount(int nCount);

	void	Init(CRect rect);

	void	ShowUpdateInfo(UpdateInfo_Param * pParam);

	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
private:

	void				OnCloseBtn(TNotifyUI& msg);
	void				OnConfirmBtn(TNotifyUI& msg);
	void				OnPlayNewBtn(TNotifyUI& msg);
	
	BOOL	m_bUpdate;

	tstring	m_strUpdateLog;
	tstring m_strUpdateVersion;
	tstring m_strAllFilesSize;

	CVerticalLayoutUI*		m_pCheckUpdateLayout;
	CVerticalLayoutUI*		m_pUpdateLayout;
	CVerticalLayoutUI*		m_pNewestLayout;
	CVerticalLayoutUI*		m_pFailedLayout;
	CVerticalLayoutUI*		m_pUpdateInfoLayout; 
	CVerticalLayoutUI*		m_pUpdateTop;

	CVerticalLayoutUI*		m_pHasUpdateLayout;
	CVerticalLayoutUI*		m_pUpdateProcessingLayout;

	CLabelMultiUI*			m_pUpdateInfoLabel;

	CLabelUI*				m_pAllFilesSizeLabel;
	CLabelUI*				m_pUpdateInfoTitle;
	CLabelUI*				m_pLocalVersion;

	HANDLE hThread;

	CRect					m_mainRect;
	CVerticalLayoutUI*		m_pMainLayoutUI;
	CVerticalLayoutUI*		m_pMainWindowLayoutUI;

	HWND					m_mainHwnd;


//	CUpdate					m_update;
	tstring					m_strNewestVersion;

	int						m_nTimingUpdateCount;

	CButtonUI*				m_pPlaynewBtn;

	CProgressUI*			m_pCurrentProgress;
	CProgressUI*			m_pTotalProgress;
	CLabelUI*				m_pUpdateInfoParamLabel;
	CLabelUI*				m_pUpdateProcessingLabel;
public:
	CWndShadow				m_WndShadow;
};


typedef Singleton<CUpdateDialogUI>	UpdateDialogUI;
