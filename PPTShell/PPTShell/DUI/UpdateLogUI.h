#pragma once
#include "DUICommon.h"
#include "ThirdParty/json/json.h"
#include "PraiseInfo.h"

class CUpdateLogUI : public WindowImplBase
{
public:
	CUpdateLogUI(void);
	~CUpdateLogUI(void); 

	UIBEGIN_MSG_MAP
		EVENT_BUTTON_CLICK(_T("btnClose"),OnBtnCloseClick);
		EVENT_BUTTON_CLICK(_T("btnSuggest"),OnBtnSuggestClick);
		EVENT_BUTTON_CLICK(_T("btnRefresh"),OnBtnRefreshClick);
	UIEND_MSG_MAP
protected:
	virtual CDuiString GetSkinFolder();
	virtual CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName(void) const;
	virtual void InitWindow();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	void OnBtnCloseClick(TNotifyUI& msg);
	void OnBtnSuggestClick(TNotifyUI& msg);
	void OnBtnRefreshClick(TNotifyUI& msg);

	void ShowUpdateLog();

	//计算文字高度
	void CalcText( HDC hdc, RECT& rc, LPCTSTR lpszText, int nFontId, UINT format, UITYPE_FONT nFontType, int c = -1);
	void CreatYearDot(CContainerUI* pParent, const char* lpszYear,bool isCurrent=false);
	void CreateItemSytle(CContainerUI* pParent, const char* lpszMothDate, const char* lpszTitle, const char* lpszLogContent,bool isCurrent=false);
	

private:
	DWORD m_dwLogTaskId;//获取更新日志的任务id
	DWORD m_dwPraiseTaskId;//获取点赞列表的任务id
	tstring m_strVersion;
	tstring m_strLogFilePath;
	Json::Value m_LogRootItem;
	tstring m_strPraiseFilePath;
	vector<CPraiseInfo> m_vPraises;
	bool m_bLogLoadCompeleted;
	bool m_bLogLoadSuccess;
	bool m_bPraiseLoadCompeleted;
	bool m_bPraiseLoadSuccess;
	CDialogBuilder m_builderPraise;
	CDialogBuilder m_builderYear;
	CDialogBuilder m_builderLogItem;
	bool m_bHasShowCache;//标示是否已经显示缓存内容
	tstring m_strPraiseDateFilePath;
	map<int,int> m_mapPraiseDate;
	//CRITICAL_SECTION m_Lock;//获取点赞列表和日志列表的互斥锁，避免两个线程同时处理Compelete

	CVerticalLayoutUI* m_pLayoutUpdateLog;
	CVerticalLayoutUI* m_pLayoutLoading;
	CGifAnimUI* m_pGifLoading;
	CVerticalLayoutUI* m_pLayoutLoadingFail;
	CGifAnimUI* m_pGifLoadingFail;
	CVerticalLayoutUI* m_pLayoutPraise;
	CVerticalLayoutUI * m_pLayoutLogList;

	void ShowLoading();
	void HideLoading();
	void ShowLoadingFail();
	void HideLoadingFail();
	void ShowContent();
	void HideContent();
	void StarGetInfoTask();
	void StopGetInfoTask();
	bool OnUpdateLogCompeleted(void* param);
	bool ReadVersionLogFile();
	bool WriteVersionLogFile(char* strJson);
	bool OnPraiseCompeleted(void* param);
	void ReadPraiseFile();
	void WritePraiseFile(char* strJson);
	bool OnPraiseItemClick(void* pNotify);
	void WritePraiseDateFile(int n_id,int n_date);
	void ReadPraiseDateFile();
};


