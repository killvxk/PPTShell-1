#pragma once

#include "DUICommon.h"
#include "DUI/EditClear.h"
#include "NDCloud/NDCloudAPI.h"
#include "NDCloud/NDCloudUser.h"
#include "DUI/CSliderTabLayoutUI.h"


class CGetBackPwdWindowUI : public CContainerUI
{
public:
	CGetBackPwdWindowUI();
	~CGetBackPwdWindowUI();

private:
	CLabelUI*			m_pGetBackCheckCode;

	CNDCloudUser		m_ndCloudUser;
	
	CLabelUI*			m_pErrorTipGetBackPwdAccountLabel;
	CLabelUI*			m_pErrorTipGetBackPwdCheckcodeLabel;
	CLabelUI*			m_pErrorTipGetBackPwdStep1;

	CLabelUI*			m_pErrorTipGetBackPwdStep2Select;
	CLabelUI*			m_pErrorTipGetBackPwdStep2CheckCode;
	CLabelUI*			m_pErrorTipGetBackPwdStep2;

	CLabelUI*			m_pErrorTipGetBackPwdStep3Password;
	CLabelUI*			m_pErrorTipGetBackPwdStep3ConfirmPassword;
	CLabelUI*			m_pErrorTipGetBackPwdStep3;

	CLabelUI*			m_pEmail;
	CLabelUI*			m_pPhone;

	CEditClearUI*		m_pAccountEdit;
	CEditClearUI*		m_pCheckCodeEdit;

	CEditClearUI*		m_pCheckCodeStep2Edit;

	CEditClearUI*		m_pPasswordEdit;
	CEditClearUI*		m_pConfirmPasswordEdit;

	CCheckBoxUI*		m_pEmailCheckBox;
	CCheckBoxUI*		m_pPhoneCheckBox;

	CButtonUI*			m_pGetBackPwdStep1Btn;
	CButtonUI*			m_pGetBackPwdStep2Btn;
	CButtonUI*			m_pGetBackPwdStep3Btn;

	CButtonUI*			m_pSentPhoneCodeBtn;
	CButtonUI*			m_pBackLoginBtn;

	CVerticalLayoutUI*	m_pCheckAccountInfoLayout;
	CHorizontalLayoutUI*	m_pCheckAccountInfoPhoneLayout;
	CHorizontalLayoutUI*	m_pCheckAccountInfoEmailLayout;

	CVerticalLayoutUI*	m_pGetBackPwdTipLayout;

	tstring				m_strAccount;
	tstring				m_strEmail;
	tstring				m_strPhone;
	tstring				m_strTempPassword;

	int					m_nDelayTimes;
public:
	CSliderTabLayoutUI* m_pSliderTabLayout;
private:
	bool				OnCheckCodeImageObtained(void * pParam);
	void				HideGetBackPwdStep1Tip();
	void				HideGetBackPwdStep2Tip();
	void				HideGetBackPwdStep3Tip();
	
	bool				CheckGetBackPwdStep1Valid(int nIndex);
	bool				CheckGetBackPwdStep2Valid(int nIndex);
	bool				CheckGetBackPwdStep3Valid(int nIndex);

	void				ShowGetBackPwdStep1Tip(int nType,tstring strTip);
	void				ShowGetBackPwdStep2Tip(int nType,tstring strTip);
	void				ShowGetBackPwdStep3Tip(int nType,tstring strTip);

	bool				OnConfirmAccount(void * pParam);
	bool				OnCheck(void * pParam);
	bool				OnResetPwd(void * pParam);
	bool				OnModifyPassword(void * pParam);
	
	void				DoEvent(TEventUI& event);

	bool				OnBackLoginEvent(void * pParam);
public:
	void				Init(HWND hMainWnd);
	void				GetBackPwd(TNotifyUI& msg);
	void				OnRefrshCheckCode();
	void				OnSentPhoneCode();

	void				SetGetBackPwdStep1BtnEnable( bool bEnable);
	void				SetGetBackPwdStep2BtnEnable( bool bEnable);
	void				SetGetBackPwdStep3BtnEnable( bool bEnable);

	void				ClearEditStatus();
};