#pragma once
#include "DUICommon.h"
#include "DUI/EditClear.h"
#include "NDCloud/NDCloudAPI.h"

class CRegisterWindowUI : public CContainerUI
{
public:
	CRegisterWindowUI();
	~CRegisterWindowUI();

private:
	CButtonUI*			m_pRegisterBtn;
	CLabelUI*			m_pErrorTipRegisterUserNameLabel;
	CLabelUI*			m_pErrorTipRegisterPasswordLabel;
	CLabelUI*			m_pErrorTipRegisterConfirmPasswordLabel;
	CLabelUI*			m_pErrorRegisterTipLabel;
	CLabelUI*			m_pErrorRegisterTipNetLabel;
	CEditClearUI*		m_pRegisterUserNameEdit;
	CEditClearUI*		m_pRegisterPasswordEdit;
	CEditClearUI*		m_pRegisterConfirmPasswordEdit;
	CEditClearUI*		m_pCheckCodeEdit;
	CLabelUI*			m_pCheckCodeLabel;
	int					m_bRegisterType; // ÷ª˙ = 0 or ” œ‰ = 1
	CHorizontalLayoutUI*m_pCheckCodeLayout;
	CHorizontalLayoutUI*m_pPhoneCodeLayout;
	bool				m_bClickSentPhoneCodeBtn;
	CChapterTree*		m_pChapterTree; 
	tstring				m_strEmail;
	bool				m_bRegistering;

private:
	bool				OnCheckCodeImageObtained(void * pParam);

	void				OnBackLogin(TNotifyUI& msg);
	void				OnSentPhoneCode(TNotifyUI& msg);
	void				ShowRegisterTip(int nType,tstring strTip);
	void				HideRegisterTip();

	bool				OnUserRegister(void * pParam);
	bool				OnCheckUserName(void * pParam);
	bool				OnQueryLoginNameExist(void * pParam);
	bool				OnSendRegisterShortMessage(void * pParam);
	bool				CheckRegisterValid(int nIndex);

	void				ShowRegisterComplete();
	void				OnReSendEmail();
	bool				OnSendActiveEmail(void * pParam);
public:
	void				Init(HWND hMainWnd);
	void				Register();
	void				NotifyTabToEdit();
	void				OnEditTabChangeRegister(CControlUI * pControl);
	void				OnRefrshCheckCode();
	void				SetRegisterBtnEnable( bool bEnable);
	bool				GetRegisterStatus();
	void				ClearEditStatus();
public:
	HWND				m_hMainWnd;

	CVerticalLayoutUI*	m_pRegisterTipLayout;
	int					m_nDelayTimes;
	CButtonUI*			m_pSentPhoneCodeBtn;
};