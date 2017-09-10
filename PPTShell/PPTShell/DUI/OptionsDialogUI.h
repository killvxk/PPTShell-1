#pragma once
#include "DUICommon.h"

class COptionsDialogUI : public WindowImplBase
{
public:
	COptionsDialogUI(void);
	~COptionsDialogUI(void);

	UIBEGIN_MSG_MAP
		EVENT_BUTTON_CLICK(_T("btnClose"),OnBtnCloseClick);
		EVENT_BUTTON_CLICK(_T("btnOK"),OnBtnOKClick);
		EVENT_BUTTON_CLICK(_T("btnCancel"),OnBtnCloseClick);
		EVENT_ID_HANDLER(DUI_MSGTYPE_SELECTCHANGED,_T("setting_Advanced"),OnOptionSelectChanged);
		EVENT_ID_HANDLER(DUI_MSGTYPE_SELECTCHANGED,_T("setting_SavePath"),OnOptionSelectChanged);
		EVENT_ID_HANDLER(DUI_MSGTYPE_SELECTCHANGED,_T("setting_Individuation"),OnOptionSelectChanged);
		EVENT_ID_HANDLER(DUI_MSGTYPE_SELECTCHANGED,_T("setting_Convention"),OnOptionSelectChanged);
		EVENT_BUTTON_CLICK(_T("ChangePath"),OnBtnChangePathClick);
		EVENT_BUTTON_CLICK(_T("OpenFolder"),OnBtnOpenFolderClick);
		EVENT_ID_HANDLER(DUI_MSGTYPE_KILLFOCUS,_T("editPath"),OnEditPathKillfocus);
		EVENT_BUTTON_CLICK(_T("btnClearTemp"),OnBtnClearTempClick);
		EVENT_ID_HANDLER(DUI_MSGTYPE_SETFOCUS,_T("password"),OnEditPasswordSetfocus);
		EVENT_ID_HANDLER(DUI_MSGTYPE_KILLFOCUS,_T("password"),OnEditPasswordKillfocus);
		EVENT_ID_HANDLER(DUI_MSGTYPE_SETFOCUS,_T("retypePassword"),OnEditRetypePasswordSetfocus);
		EVENT_ID_HANDLER(DUI_MSGTYPE_KILLFOCUS,_T("retypePassword"),OnEditRetypePasswordKillfocus);
		EVENT_BUTTON_CLICK(_T("passwordOk"),OnBtnPasswordOkClick);
		EVENT_ID_HANDLER(DUI_MSGTYPE_SELECTCHANGED,_T("chkMakePassword"),OnChkMakePasswordChanged);
		EVENT_BUTTON_CLICK(_T("passwordModify"),OnBtnPasswordModifyClick);
		EVENT_BUTTON_CLICK(_T("passwordTip"),OnBtnPasswordTipClick);
		EVENT_BUTTON_CLICK(_T("retypePasswordTip"),OnBtnRetypePasswordTipClick);
	UIEND_MSG_MAP

	static BOOL isICROpen;		   //互动课堂是否已经初始化

private:
	virtual CDuiString GetSkinFolder();
	virtual CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName(void) const;
	virtual	void InitWindow();

	void OnBtnCloseClick(TNotifyUI& msg);
	void OnBtnOKClick(TNotifyUI& msg);
	void OnOptionSelectChanged(TNotifyUI& msg);
	void OptionColorChanged(CDuiString strName);
	
	void DoSetting(); 

	CDuiString m_strOptionSelected;
	CDuiString m_strDotSelected;

	void RegisterOpenMode();
	BOOL CheckSelfRelation(const TCHAR *strAppKey);
	BOOL CheckFileRelation(const TCHAR *strExt, const TCHAR *strAppKey);
	void RegisterURLProtocol(TCHAR *pszProtocal, TCHAR *pszExePath);
	void RegisterFileRelation(TCHAR *strExt, TCHAR *strAppName, TCHAR *strAppKey, TCHAR *strDefaultIcon, TCHAR *strDescribe);
	void ResetOpenMode();

	void OnBtnChangePathClick(TNotifyUI& msg);
	void OnBtnOpenFolderClick(TNotifyUI& msg);
	void OnEditPathKillfocus(TNotifyUI& msg);
	bool IsDirectoryExists(CString const& path);
	bool CheckDirectory(CString& path);
	void OnBtnClearTempClick(TNotifyUI& msg);
	BOOL DeleteDirectory(TCHAR* psDirName);

	void OnEditPasswordSetfocus(TNotifyUI& msg);
	void OnEditPasswordKillfocus(TNotifyUI& msg);
	void OnEditRetypePasswordSetfocus(TNotifyUI& msg);
	void OnEditRetypePasswordKillfocus(TNotifyUI& msg);
	void OnBtnPasswordOkClick(TNotifyUI& msg);
	void OnChkMakePasswordChanged(TNotifyUI& msg);
	void SwitchView(int status, LPTSTR tip = NULL);
	void OnBtnPasswordModifyClick(TNotifyUI& msg);
	void OnBtnPasswordTipClick(TNotifyUI& msg);
	void OnBtnRetypePasswordTipClick(TNotifyUI& msg);

	bool ChkPasswordFormat(LPTSTR password);
	CString GetPassword();
	void SetPassword(LPTSTR password);
	void CheckXmlExist();

	tstring m_SavePath;			   //下载地址
	tstring GetDefaultDirectory(); //得到下载地址的默认目录
	

};

