#pragma once

#include "DUICommon.h"
#include "DUI/EditClear.h"
#include "NDCloud/NDCloudAPI.h"

class CChangePasswordWindowUI : public CContainerUI
{
public:
	CChangePasswordWindowUI();
	~CChangePasswordWindowUI();

private:
	CEditClearUI*		m_pOldPasswordEdit;
	CEditClearUI*		m_pNewPasswordEdit;
	CEditClearUI*		m_pNewConfirmPasswordEdit;
	CLabelUI*			m_pErrorTipOldPasswordLabel;
	CLabelUI*			m_pErrorTipNewPasswordLabel;
	CLabelUI*			m_pErrorTipNewConfirmPasswordLabel;
	CLabelUI*			m_pErrorTipLabel;
	CButtonUI*			m_pChangePasswordBtn;

	bool				m_bChangePasswording;
public:
	HWND				m_hMainWnd;

	CVerticalLayoutUI*	m_pChangePasswordTipLayout;

private:
	

 	void				HideChangePasswordTip();
	bool				CheckChangePasswordValid(int nIndex);
 	void				ShowChangePasswordTip(int nType,tstring strTip);
 	void				ShowChangePasswordComplete();
// 
 	bool				OnUserChangePassword(void * pParam);
// 
// 	void				CalcText( HDC hdc, RECT& rc, LPCTSTR lpszText, int nFontId, UINT format, UITYPE_FONT nFontType, int  c = -1);
public:
	void				Init(HWND hMainWnd);
	void				ChangePassword();
	void				NotifyTabToEdit();
	void				OnEditTabChange(CControlUI * pControl);

	void				SetChangePasswordBtnEnable( bool bEnable);

	bool				GetChangePasswordStatus();
	void				ClearEditStatus();
};