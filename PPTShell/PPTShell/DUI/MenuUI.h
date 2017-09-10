#pragma once
#include "DUICommon.h"

class CMenuUI: public WindowImplBase
{
	UIBEGIN_MSG_MAP
		EVENT_ID_HANDLER(DUI_MSGTYPE_ITEMCLICK, _T("menu_open"), OnMenuOpenClick);
		EVENT_ID_HANDLER(DUI_MSGTYPE_ITEMCLICK, _T("menu_import"), OnMenuImportClick);
		EVENT_ID_HANDLER(DUI_MSGTYPE_ITEMCLICK, _T("menu_rename"), OnMenuRenameClick);
		EVENT_ID_HANDLER(DUI_MSGTYPE_ITEMCLICK, _T("menu_clearup"), OnMenuClearupClick);
		EVENT_ID_HANDLER(DUI_MSGTYPE_ITEMCLICK, _T("menu_upload_netdisc"), OnMenuUploadNetdiscClick);
		EVENT_ID_HANDLER(DUI_MSGTYPE_ITEMCLICK, _T("menu_download_local"), OnMenuDownloadLocalClick);
		EVENT_ID_HANDLER(DUI_MSGTYPE_ITEMCLICK, _T("menu_delete"), OnMenuDeleteClick);
		EVENT_ID_HANDLER(DUI_MSGTYPE_ITEMCLICK, _T("menu_edit_exercises"), OnMenuEditExercisesClick);
		EVENT_ID_HANDLER(DUI_MSGTYPE_ITEMCLICK, _T("menu_upload"), OnMenuUploadClick);
	UIEND_MSG_MAP

public:
	CMenuUI(void);
	~CMenuUI(void);
	void SetType(int nType);//设置菜单对应于那个模块
	void SetItemIndex(int nIndex);
	void SetIsCloudResource(bool isCloud);//
	void HideAllMenuItem();//隐藏所有菜单项
	void ShowMenuOpen();//显示打开项
	void ShowMenuImport();//显示导入项
	void ShowMenuRename();//显示重命名项
	void ShowMenuClearup();//显示清空项
	void ShowMenuUploadNetDisc();//显示上传到我的网盘项
	void ShowMenuDownloadLocal();//显示下载到本地项
	void ShowMenuDelete();//显示删除项
	void ShowMenuEditExercises();//显示编辑习题项
	void ShowMenuUpload();//显示上传项
private:
	LPCTSTR GetWindowClassName() const;	
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void InitWindow();
	virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void OnMenuOpenClick(TNotifyUI& msg);
	void OnMenuImportClick(TNotifyUI& msg);
	void OnMenuRenameClick(TNotifyUI& msg);
	void OnMenuClearupClick(TNotifyUI& msg);
	void OnMenuUploadNetdiscClick(TNotifyUI& msg);
	void OnMenuDownloadLocalClick(TNotifyUI& msg);
	void OnMenuDeleteClick(TNotifyUI& msg);
	void OnMenuEditExercisesClick(TNotifyUI& msg);
	void OnMenuUploadClick(TNotifyUI& msg);
	void RefreshMenuItem();
	void RefreshSize();

	int m_nType;
	int m_nItemIndex;
	bool m_bIsCloud;

	CListContainerElementUI* m_pMenuOpen;
	CControlUI* m_pMenuImportSplit;
	CListContainerElementUI* m_pMenuImport;
	CControlUI* m_pMenuRenameSplit;
	CListContainerElementUI* m_pMenuRename;
	CControlUI* m_pMenuClearupSplit;
	CListContainerElementUI* m_pMenuClearup;
	CControlUI* m_pMenuUploadNetdiscSplit;
	CListContainerElementUI* m_pMenuUploadNetdisc;
	CControlUI* m_pMenuDownloadLocalSplit;
	CListContainerElementUI* m_pMenuDownloadLocal;
	CControlUI* m_pMenuDeleteSplit;
	CListContainerElementUI* m_pMenuDelete;
	CControlUI* m_pMenuEditExercisesSplit;
	CListContainerElementUI* m_pMenuEditExercises;
	CControlUI* m_pMenuUploadSplit;
	CListContainerElementUI* m_pMenuUpload;
};
