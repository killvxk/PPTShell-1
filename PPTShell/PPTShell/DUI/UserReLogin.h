#pragma once
#include "stdafx.h"
#include "DUICommon.h"
#include "Util\Singleton.h"

#define DESKTOP_TIMER_ID 90001

class CUserReLogin : public WindowImplBase/*, public IListCallbackUI*/
{
public:
	CUserReLogin(void);
	~CUserReLogin(void);

	UIBEGIN_MSG_MAP 
		EVENT_ITEM_CLICK(_T("LogOutBtn"),	OnLogOutBtn);
		EVENT_ITEM_CLICK(_T("ChangeUserBtn"),OnChangeUserBtn);
		EVENT_ITEM_CLICK(_T("ModifyPwdBtn"),OnModifyPwdBtn);

		EVENT_ID_HANDLER(DUI_MSGTYPE_ITEMSELECT,	_T("UserDept"),		OnSelectEvent);
	UIEND_MSG_MAP
 

public:
	LPCTSTR GetWindowClassName() const;	
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();

	void	InitWindows(HWND hWndParent, POINT ptPos ,tstring pUserPhoto,  tstring pNewUserName, int nLoginType = 0);   
	void	ShowLocalWindows( );
	void	OnLogOutBtn(TNotifyUI& msg);
	void	OnChangeUserBtn(TNotifyUI& msg);
	void	OnModifyPwdBtn(TNotifyUI& msg);
	void	SetUserName(tstring pNewUserName);
	void	SetUserDept();
	bool	OnCloseWndEvent(void* pObj);
	void	OnSelectEvent(TNotifyUI& msg);
	//virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
protected:  
	HWND			m_hWndParent;  
	POINT			m_ptPos;
	CLabelUI*		m_pUserName;//教师名称
	CComboUI*		m_pUserDept;//所在年级
	CButtonUI*		m_pLogOutBtn;//退出
	CButtonUI*		m_pChangeUserBtn;//切换登陆
	CButtonUI*		m_pModifyPwdBtn;//修改密码
	CControlUI*		m_pUserImage;//用户背景图
	CContainerUI*	m_pFreeTeachCtn;//自由授课选中状态背景2016.02.14

	CListContainerElementUI*	m_pUCLoginElement;
private:
	tstring			m_sTeacherGradeName;//老师最后选择的班级
	tstring			m_sTeacherName;//老师名称
	bool			m_bFreeTeach;//自由授课模式
	bool			m_bInit;
};


typedef Singleton<CUserReLogin>		UserReLoginUI;
