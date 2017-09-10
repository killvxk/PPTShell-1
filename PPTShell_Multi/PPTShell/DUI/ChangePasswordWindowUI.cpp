#include "stdafx.h"
#include "ChangePasswordWindowUI.h"
#include "NDCloud/NDCloudUser.h"
#include <regex>
#include "Util/Util.h"
#include "DUI/GroupExplorer.h"

CChangePasswordWindowUI::CChangePasswordWindowUI()
{
	m_pOldPasswordEdit			= NULL;
	m_pNewPasswordEdit			= NULL;
	m_pNewConfirmPasswordEdit	= NULL;
	m_bChangePasswording		= false;
}

CChangePasswordWindowUI::~CChangePasswordWindowUI()
{
	
}


void CChangePasswordWindowUI::Init(HWND hMainWnd)
{
	m_hMainWnd = hMainWnd;
	m_pOldPasswordEdit = dynamic_cast<CEditClearUI*>(FindSubControl(_T("oldpassword")));
	m_pNewPasswordEdit = dynamic_cast<CEditClearUI*>(FindSubControl(_T("newpassword")));
	m_pNewConfirmPasswordEdit = dynamic_cast<CEditClearUI*>(FindSubControl(_T("newconfirmpassword")));
	m_pOldPasswordEdit->SetHandleSpecialKeydown(true);
	m_pNewPasswordEdit->SetHandleSpecialKeydown(true);
	m_pNewConfirmPasswordEdit->SetHandleSpecialKeydown(true);

	m_pChangePasswordTipLayout = dynamic_cast<CVerticalLayoutUI*>(FindSubControl(_T("ChangePasswordTipLayout")));


	m_pErrorTipOldPasswordLabel = dynamic_cast<CLabelUI*>(FindSubControl(_T("error_tip_oldpassword")));
	m_pErrorTipNewPasswordLabel = dynamic_cast<CLabelUI*>(FindSubControl(_T("error_tip_newpassword")));
	m_pErrorTipNewConfirmPasswordLabel = dynamic_cast<CLabelUI*>(FindSubControl(_T("error_tip_newconfirmpassword")));
	m_pErrorTipLabel = dynamic_cast<CLabelUI*>(FindSubControl(_T("error_tip")));

	m_pChangePasswordBtn = dynamic_cast<CButtonUI*>(FindSubControl(_T("ChangePasswordBtn")));
}

void CChangePasswordWindowUI::ChangePassword()
{
	HideChangePasswordTip();

	tstring strOldPassword				=	m_pOldPasswordEdit->GetText();
	tstring strNewPassword				=	m_pNewPasswordEdit->GetText();
	tstring strNewConfirmPassword		=	m_pNewConfirmPasswordEdit->GetText();

	if(!CheckChangePasswordValid(0))
		return;
	if(!CheckChangePasswordValid(1))
		return;
	if(!CheckChangePasswordValid(2))
		return;

	SetChangePasswordBtnEnable(false);

	m_bChangePasswording = true;
	NDCloudUser::GetInstance()->ChangePassword(strOldPassword, strNewPassword, MakeHttpDelegate(this, &CChangePasswordWindowUI::OnUserChangePassword));
}

void CChangePasswordWindowUI::NotifyTabToEdit()
{

}

void CChangePasswordWindowUI::OnEditTabChange( CControlUI * pControl )
{
	if(m_pNewConfirmPasswordEdit && pControl == m_pOldPasswordEdit)
	{
		m_pNewPasswordEdit->SetFocus();
		m_pNewPasswordEdit->SetSelAll();
	}
	else if(m_pNewPasswordEdit && pControl == m_pNewPasswordEdit)
	{
		m_pNewConfirmPasswordEdit->SetFocus();
		m_pNewConfirmPasswordEdit->SetSelAll();
	}
	else if(m_pNewConfirmPasswordEdit && pControl == m_pNewConfirmPasswordEdit)
	{
		m_pOldPasswordEdit->SetFocus();
		m_pOldPasswordEdit->SetSelAll();
	}
}

void CChangePasswordWindowUI::SetChangePasswordBtnEnable( bool bEnable )
{
	if(bEnable)
	{
		m_pChangePasswordBtn->SetEnabled(true);
	}
	else
	{
		m_pChangePasswordBtn->SetEnabled(false);;
	}
}

void CChangePasswordWindowUI::HideChangePasswordTip()
{
	if(m_pErrorTipOldPasswordLabel->IsVisible())
	{
		m_pErrorTipOldPasswordLabel->SetVisible(false);
	}
	if(m_pErrorTipNewPasswordLabel->IsVisible())
	{
		m_pErrorTipNewPasswordLabel->SetVisible(false);
	}
	if(m_pErrorTipNewConfirmPasswordLabel->IsVisible())
	{
		m_pErrorTipNewConfirmPasswordLabel->SetVisible(false);
	}
	if(m_pErrorTipLabel->IsVisible())
	{
		m_pErrorTipLabel->SetVisible(false);
	}
}

bool CChangePasswordWindowUI::CheckChangePasswordValid( int nIndex )
{
	tstring strOldPassword =			m_pOldPasswordEdit->GetText();
	tstring strNewPassword =			m_pNewPasswordEdit->GetText();
	tstring strNewConfirmPassword =		m_pNewConfirmPasswordEdit->GetText();
	switch(nIndex)
	{
	case 0:
		{
			if(strOldPassword.empty() || strOldPassword == m_pOldPasswordEdit->GetTipText())
			{
				ShowChangePasswordTip(0, _T("旧密码不能为空"));
				return false;
			}
			else if(strOldPassword.length() < 6)
			{
				ShowChangePasswordTip(0, _T("旧密码长度不能少于6个"));
				return false;
			}
		}
		break;
	case 1:
		{
			if(strNewPassword.empty() || strNewPassword == m_pNewPasswordEdit->GetTipText())
			{
				ShowChangePasswordTip(1, _T("新密码不能为空"));
				return false;
			}
			else if(strNewPassword.length() < 6)
			{
				ShowChangePasswordTip(1, _T("新密码长度不能少于6个"));
				return false;
			}
		}
		break;
	case 2:
		{
			if(strNewConfirmPassword.empty() || strNewConfirmPassword == m_pNewConfirmPasswordEdit->GetTipText())
			{
				ShowChangePasswordTip(2, _T("重复密码不能为空"));
				return false;
			}
			else if(strNewConfirmPassword.length() < 6)
			{
				ShowChangePasswordTip(2, _T("重复密码长度不能少于6个"));
				return false;
			}
			else if(strNewPassword != strNewConfirmPassword)
			{
				ShowChangePasswordTip(1, _T("两次输入密码不相同"));
				ShowChangePasswordTip(2, _T("两次输入密码不相同"));
				return false;
			}
		}
		break;
	}
	return true;
}

void CChangePasswordWindowUI::ShowChangePasswordTip( int nType,tstring strTip )
{
	if(nType == 0)
	{
		m_pErrorTipOldPasswordLabel->SetText(strTip.c_str());
		m_pErrorTipOldPasswordLabel->SetVisible(true);
	}
	else if(nType == 1)
	{
		m_pErrorTipNewPasswordLabel->SetText(strTip.c_str());
		m_pErrorTipNewPasswordLabel->SetVisible(true);
	}
	else if(nType == 2)
	{
		m_pErrorTipNewConfirmPasswordLabel->SetText(strTip.c_str());
		m_pErrorTipNewConfirmPasswordLabel->SetVisible(true);
	}
	else if(nType == 3)
	{
		m_pErrorTipLabel->SetText(strTip.c_str());
		m_pErrorTipLabel->SetVisible(true);
	}

	m_bChangePasswording = false;
	SetChangePasswordBtnEnable(true);
}

bool CChangePasswordWindowUI::OnUserChangePassword( void * pParam )
{
	THttpNotify* pNotify = static_cast<THttpNotify*>(pParam);

	if(pNotify->dwErrorCode != 0)
	{
		ShowChangePasswordTip(3, _T("当前网络不太好，请检查网络连接"));
		return false;
	}
	CNDCloudUser* pUser = NDCloudUser::GetInstance();

	if( pUser->IsSuccess() )
	{
		//修改成功
		m_pOldPasswordEdit->SetText(_T(""));
		m_pNewPasswordEdit->SetText(_T(""));
		m_pNewConfirmPasswordEdit->SetText(_T(""));
		ShowChangePasswordComplete();
	}
	else
	{
		tstring strErrorCode = pUser->GetErrorCode();
		tstring strErrorMessage = pUser->GetErrorMessage();

		ShowChangePasswordTip(3, strErrorMessage);
	}

	m_bChangePasswording = false;
	return true;
}

void CChangePasswordWindowUI::ShowChangePasswordComplete()
{
	m_pChangePasswordTipLayout->SetVisible(true);
	SetTimer(m_hMainWnd, WM_CHANGEPASS_COMPLETE, 1000, (TIMERPROC)CGroupExplorerUI::TimerProcComplete);
}

bool CChangePasswordWindowUI::GetChangePasswordStatus()
{
	return m_bChangePasswording;
}

void CChangePasswordWindowUI::ClearEditStatus()
{
	m_pOldPasswordEdit->SetText(m_pOldPasswordEdit->GetTipText().c_str());
	m_pOldPasswordEdit->SetClearBtn();
	m_pNewPasswordEdit->SetText(m_pNewPasswordEdit->GetTipText().c_str());
	m_pNewPasswordEdit->SetClearBtn();
	m_pNewConfirmPasswordEdit->SetText(m_pNewConfirmPasswordEdit->GetTipText().c_str());
	m_pNewConfirmPasswordEdit->SetClearBtn();

	m_pErrorTipOldPasswordLabel->SetVisible(false);
	m_pErrorTipNewPasswordLabel->SetVisible(false);
	m_pErrorTipNewConfirmPasswordLabel->SetVisible(false);
	m_pErrorTipLabel->SetVisible(false);
}
