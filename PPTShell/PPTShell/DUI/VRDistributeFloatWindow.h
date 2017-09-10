#pragma once

#include "DUICommon.h"
#include "Util/Singleton.h"


class CVRDistributeFloatWindow : public WindowImplBase
{
public:
	CVRDistributeFloatWindow(void);
	~CVRDistributeFloatWindow(void);
	UIBEGIN_MSG_MAP
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("VRDistribute"),		OnVRDistributeBtn);
	UIEND_MSG_MAP

public:
	LPCTSTR				GetWindowClassName() const;	

	virtual void		InitWindow();

	virtual CDuiString	GetSkinFile();

	virtual CDuiString	GetSkinFolder();

	bool				GetVRPackageInfo(tstring strPath);

	tstring				GetVRApkPath()	{ return m_strApkPath;	}

private:
	void				OnVRDistributeBtn(TNotifyUI& msg);

	tstring				m_strVRName;
	tstring				m_strApkPath;
	tstring				m_strApkPackageName;
	tstring				m_strApkMainActivity;
	tstring				m_strPackageVer;

};

typedef Singleton<CVRDistributeFloatWindow>	VRDistributeUI;