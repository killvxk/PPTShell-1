#include "stdafx.h"
#include "RegisterWindowUI.h"
#include "NDCloud/NDCloudUser.h"
#include <regex>
#include "Util/Util.h"
#include "DUI/GroupExplorer.h"
#include "EventCenter/EventDefine.h"
CRegisterWindowUI::CRegisterWindowUI()
{
	m_bClickSentPhoneCodeBtn		= false;
	m_pRegisterUserNameEdit			= NULL;
	m_pRegisterPasswordEdit			= NULL;
	m_pRegisterConfirmPasswordEdit	= NULL;
	m_pCheckCodeEdit				= NULL;
	m_bRegistering					= false;
}

CRegisterWindowUI::~CRegisterWindowUI()
{
	NDCloudUser::GetInstance()->CancelRegister();
	NDCloudUser::GetInstance()->CancelObtainCheckCodeImage();
	NDCloudUser::GetInstance()->CancelSendRegisterShortMessage();
	NDCloudUser::GetInstance()->CancelQueryLoginNameExist();
	KillTimer(m_hMainWnd,WM_SENDBTN_DELAY);
}

bool CRegisterWindowUI::OnCheckCodeImageObtained( void * pParam )
{
	THttpNotify*  pNotify = (THttpNotify* )pParam;
	if(pNotify->dwErrorCode == 0)
	{
		tstring strFilePath = pNotify->strFilePath;
		m_pCheckCodeLabel->SetBkImage(strFilePath.c_str());
	}

	return true;
}

void CRegisterWindowUI::Init( HWND hMainWnd )
{
	m_hMainWnd = hMainWnd;

	NDCloudUser::GetInstance()->ObtainCheckCodeImage(MakeHttpDelegate(this, &CRegisterWindowUI::OnCheckCodeImageObtained) );

	m_pCheckCodeLabel = dynamic_cast<CLabelUI*>(FindSubControl(_T("checkcodeimage")));

	m_pErrorTipRegisterUserNameLabel = dynamic_cast<CLabelUI*>(FindSubControl(_T("error_tip_registerusername")));
	m_pErrorTipRegisterPasswordLabel = dynamic_cast<CLabelUI*>(FindSubControl(_T("error_tip_registerpassword")));
	m_pErrorTipRegisterConfirmPasswordLabel = dynamic_cast<CLabelUI*>(FindSubControl(_T("error_tip_registerconfirmpassword")));

	m_pRegisterUserNameEdit = dynamic_cast<CEditClearUI*>(FindSubControl(_T("registerusername")));
	m_pRegisterUserNameEdit->OnEvent += MakeDelegate(this, &CRegisterWindowUI::OnCheckUserName);
	m_pRegisterPasswordEdit = dynamic_cast<CEditClearUI*>(FindSubControl(_T("registerpassword")));
	m_pRegisterConfirmPasswordEdit = dynamic_cast<CEditClearUI*>(FindSubControl(_T("registerconfirmpassword")));
	m_pCheckCodeEdit = dynamic_cast<CEditClearUI*>(FindSubControl(_T("checkcode")));
	m_pErrorRegisterTipLabel = dynamic_cast<CLabelUI*>(FindSubControl(_T("error_tip")));
	m_pErrorRegisterTipNetLabel = dynamic_cast<CLabelUI*>(FindSubControl(_T("error_tip_net")));

	m_pRegisterUserNameEdit->SetHandleSpecialKeydown(true);
	m_pRegisterPasswordEdit->SetHandleSpecialKeydown(true);
	m_pRegisterConfirmPasswordEdit->SetHandleSpecialKeydown(true);
	m_pCheckCodeEdit->SetHandleSpecialKeydown(true);

	m_pRegisterBtn = dynamic_cast<CButtonUI*>(FindSubControl(_T("RegisterBtn")));
	m_pSentPhoneCodeBtn = dynamic_cast<CButtonUI*>(FindSubControl(_T("sentphonecode")));

	m_pCheckCodeLayout = dynamic_cast<CHorizontalLayoutUI*>(FindSubControl(_T("checkcodeLayout")));
	m_pPhoneCodeLayout = dynamic_cast<CHorizontalLayoutUI*>(FindSubControl(_T("phonecodeLayout")));
	m_pRegisterTipLayout = dynamic_cast<CVerticalLayoutUI*>(FindSubControl(_T("RegisterTipLayout")));
}

void CRegisterWindowUI::Register()
{
	HideRegisterTip();

	tstring strRegisterUserName =			m_pRegisterUserNameEdit->GetText();
	tstring strRegisterPassword =			m_pRegisterPasswordEdit->GetText();
	tstring strRegisterConfirmPassword =	m_pRegisterConfirmPasswordEdit->GetText();
	tstring strCheckCode =					m_pCheckCodeEdit->GetText();

	m_strEmail = strRegisterUserName;
	if(!CheckRegisterValid(0))
	{
		m_pSentPhoneCodeBtn->SetEnabled(false);
		return;
	}
	if(!CheckRegisterValid(1))
		return;
	if(!CheckRegisterValid(2))
		return;
	if(!CheckRegisterValid(3))
		return;

	SetRegisterBtnEnable(false);

	m_bRegistering = true;
	NDCloudUser::GetInstance()->Register(strRegisterUserName.c_str(), strRegisterPassword.c_str(), strCheckCode.c_str(), MakeHttpDelegate(this, &CRegisterWindowUI::OnUserRegister));
}

void CRegisterWindowUI::SetRegisterBtnEnable( bool bEnable )
{
	if(bEnable)
	{
		m_pRegisterBtn->SetEnabled(true);
	}
	else
	{
		m_pRegisterBtn->SetEnabled(false);;
	}
}

void CRegisterWindowUI::OnEditTabChangeRegister( CControlUI * pControl )
{
	if(m_pRegisterUserNameEdit && pControl == m_pRegisterUserNameEdit)
	{
		m_pRegisterPasswordEdit->SetFocus();
		m_pRegisterPasswordEdit->SetSelAll();
	}
	else if(m_pRegisterPasswordEdit && pControl == m_pRegisterPasswordEdit)
	{
		m_pRegisterConfirmPasswordEdit->SetFocus();
		m_pRegisterConfirmPasswordEdit->SetSelAll();
	}
	else if(m_pRegisterConfirmPasswordEdit && pControl == m_pRegisterConfirmPasswordEdit)
	{
		m_pCheckCodeEdit->SetFocus();
		m_pCheckCodeEdit->SetSelAll();
	}
	else if(m_pCheckCodeEdit && pControl == m_pCheckCodeEdit)
	{
		m_pRegisterUserNameEdit->SetFocus();
		m_pRegisterUserNameEdit->SetSelAll();
	}
}

void CRegisterWindowUI::ShowRegisterComplete()
{
	m_bRegistering = false;
	m_pRegisterTipLayout->SetVisible(true);
	SetTimer(m_hMainWnd, WM_REGISTER_COMPLETE, 1000, (TIMERPROC)CGroupExplorerUI::TimerProcComplete);
}

void CRegisterWindowUI::ShowRegisterTip( int nType,tstring strTip )
{
	if(nType == 0)
	{
		m_pErrorTipRegisterUserNameLabel->SetText(strTip.c_str());
		m_pErrorTipRegisterUserNameLabel->SetVisible(true);
	}
	else if(nType == 1)
	{
		m_pErrorTipRegisterPasswordLabel->SetText(strTip.c_str());
		m_pErrorTipRegisterPasswordLabel->SetVisible(true);
	}
	else if(nType == 2)
	{
		m_pErrorTipRegisterConfirmPasswordLabel->SetText(strTip.c_str());
		m_pErrorTipRegisterConfirmPasswordLabel->SetVisible(true);
	}
	else if(nType == 3)
	{
		m_pErrorRegisterTipLabel->SetText(strTip.c_str());
		m_pErrorRegisterTipLabel->SetVisible(true);
	}
	else if(nType == 4)
	{
		m_pErrorRegisterTipNetLabel->SetText(strTip.c_str());
		m_pErrorRegisterTipNetLabel->SetVisible(true);
	}

	m_bRegistering = false;
	SetRegisterBtnEnable(true);
}

void CRegisterWindowUI::HideRegisterTip()
{
	if(m_pErrorTipRegisterUserNameLabel->IsVisible())
	{
		m_pErrorTipRegisterUserNameLabel->SetVisible(false);
	}
	if(m_pErrorTipRegisterPasswordLabel->IsVisible())
	{
		m_pErrorTipRegisterPasswordLabel->SetVisible(false);
	}
	if(m_pErrorTipRegisterConfirmPasswordLabel->IsVisible())
	{
		m_pErrorTipRegisterConfirmPasswordLabel->SetVisible(false);
	}
	if(m_pErrorRegisterTipLabel->IsVisible())
	{
		m_pErrorRegisterTipLabel->SetVisible(false);
	}
	if(m_pErrorRegisterTipNetLabel->IsVisible())
	{
		m_pErrorRegisterTipNetLabel->SetVisible(false);
	}
}

bool CRegisterWindowUI::OnUserRegister( void * pParam )
{
	THttpNotify* pNotify = static_cast<THttpNotify*>(pParam);

	if(pNotify->dwErrorCode != 0)
	{
		ShowRegisterTip(4, _T("当前网络不太好，无法登录，请检查网络连接"));
		return false;
	}
	CNDCloudUser* pUser = NDCloudUser::GetInstance();

	if( pUser->IsSuccess() )
	{
		//注册成功
		m_pRegisterUserNameEdit->SetText(_T(""));
		m_pRegisterPasswordEdit->SetText(_T(""));
		m_pRegisterConfirmPasswordEdit->SetText(_T(""));
		m_pCheckCodeEdit->SetText(_T(""));
		NDCloudUser::GetInstance()->ObtainCheckCodeImage(MakeHttpDelegate(this, &CRegisterWindowUI::OnCheckCodeImageObtained) );
		ShowRegisterComplete();
		// 		if(m_bRegisterType == 0) //手机
// 		{
// 			
// 		}
// 		else if(m_bRegisterType == 1) //邮箱
// 		{
// 			ShowRegisterEmailSuccessUI();
// 		}
	}
	else
	{
		tstring strErrorCode = pUser->GetErrorCode();
		tstring strErrorMessage = pUser->GetErrorMessage();

		OnRefrshCheckCode();
		ShowRegisterTip(3, strErrorMessage);
	}

	return true;
}

bool CRegisterWindowUI::OnCheckUserName( void * pParam )
{
	TEventUI* pNotify = (TEventUI*)pParam;
	if(pNotify->Type == UIEVENT_KILLFOCUS)
	{
		tstring strRegisterUserName =			m_pRegisterUserNameEdit->GetText();

		HideRegisterTip();
		if(!CheckRegisterValid(0))
		{
			//m_pSentPhoneCodeBtn->SetEnabled(false);
			return true;
		}

		NDCloudUser::GetInstance()->QueryLoginNameExist(strRegisterUserName.c_str(), MakeHttpDelegate(this, &CRegisterWindowUI::OnQueryLoginNameExist));
	}

	return true;
}

bool CRegisterWindowUI::OnQueryLoginNameExist( void * pParam )
{
	THttpNotify* pNotify = static_cast<THttpNotify*>(pParam);

	if(pNotify->dwErrorCode != 0)
	{
		ShowRegisterTip(4, _T("当前网络不太好，无法登录，请检查网络连接"));
		return false;
	}

	BOOL bExist = NDCloudUser::GetInstance()->IsLoginNameExist();
	if(bExist)
	{
		ShowRegisterTip(0, _T("该账号已存在"));
		m_pSentPhoneCodeBtn->SetEnabled(false);
		return false;
	}

// 	if(m_bRegisterType == 0)//手机
// 	{
// 		m_pCheckCodeLayout->SetVisible(false);
// 		m_pPhoneCodeLayout->SetVisible(true);
// 		m_pSentPhoneCodeBtn->SetEnabled(true);
// 	}
// 	else
	{
		m_pPhoneCodeLayout->SetVisible(false);
		m_pCheckCodeLayout->SetVisible(true);
	}

	return true;
}

bool CRegisterWindowUI::CheckRegisterValid( int nIndex )
{
	tstring strRegisterUserName =			m_pRegisterUserNameEdit->GetText();
	tstring strRegisterPassword =			m_pRegisterPasswordEdit->GetText();
	tstring strRegisterConfirmPassword =	m_pRegisterConfirmPasswordEdit->GetText();
	tstring strCheckCode =					m_pCheckCodeEdit->GetText();

	switch(nIndex)
	{
	case 0:
		{
			if(strRegisterUserName.empty() || strRegisterUserName == m_pRegisterUserNameEdit->GetTipText())
			{
				ShowRegisterTip(0, _T("用户名不能为空"));
				return false;
			}

			//
			std::tr1::regex szUserNameRule("^\\d+$");
			std::tr1::smatch sResult;
			if(std::tr1::regex_search(strRegisterUserName, sResult, szUserNameRule))
			{
				//手机
				std::tr1::regex szUserNameRuleEx("^1[34578]\\d{9}$");

				if(!std::tr1::regex_search(strRegisterUserName, sResult, szUserNameRuleEx))
				{
					ShowRegisterTip(0, _T("您输入的手机号码格式不正确 ,请输入11位有效的手机号码"));
					return false;
				}

				m_bRegisterType = 0;
			}
			else
			{
				//邮箱
				std::tr1::regex szUserNameRuleEx("^[a-zA-Z0-9_-]+@[a-zA-Z0-9_-]+(\\.[a-zA-Z0-9_-]+)+$");
				if(!std::tr1::regex_search(strRegisterUserName, sResult, szUserNameRuleEx))
				{
					ShowRegisterTip(0, _T("您输入的邮箱格式有误 ,请输入正确的邮箱"));
					return false;
				}

				m_bRegisterType = 1;
			}
		}
		break;
	case 1:
		{
			if(strRegisterPassword.empty() || strRegisterPassword == m_pRegisterPasswordEdit->GetTipText())
			{
				ShowRegisterTip(1, _T("密码不能为空"));
				return false;
			}
			else if(strRegisterPassword.length() < 6)
			{
				ShowRegisterTip(1, _T("密码长度不能少于6个"));
				return false;
			}
		}
		break;
	case 2:
		{
			if(strRegisterConfirmPassword.empty() || strRegisterConfirmPassword == m_pRegisterConfirmPasswordEdit->GetTipText())
			{
				ShowRegisterTip(2, _T("重复密码不能为空"));
				return false;
			}
			else if(strRegisterPassword != strRegisterConfirmPassword)
			{
				ShowRegisterTip(1, _T("两次输入密码不相同"));
				ShowRegisterTip(2, _T("两次输入密码不相同"));
				return false;
			}
		}
		break;
	case 3:
		{
			if(strCheckCode.empty() || strCheckCode == m_pCheckCodeEdit->GetTipText())
			{
				ShowRegisterTip(3, _T("请输入验证码"));
				return false;
			}
		}
		break;
	}
	return true;
}

void CRegisterWindowUI::OnSentPhoneCode( TNotifyUI& msg )
{
	if(m_bClickSentPhoneCodeBtn)
		return;
	tstring strRegisterUserName =			m_pRegisterUserNameEdit->GetText();
	if(!CheckRegisterValid(0))
	{
		m_pSentPhoneCodeBtn->SetEnabled(false);
		return;
	}

	m_bClickSentPhoneCodeBtn = true;
	NDCloudUser::GetInstance()->SendRegisterShortMessage(strRegisterUserName.c_str(), MakeHttpDelegate(this, &CRegisterWindowUI::OnSendRegisterShortMessage));
}

bool CRegisterWindowUI::OnSendRegisterShortMessage( void * pParam )
{
	THttpNotify* pNotify = static_cast<THttpNotify*>(pParam);

	m_bClickSentPhoneCodeBtn = false;
	if(pNotify->dwErrorCode != 0)
	{
		ShowRegisterTip(4, _T("当前网络不太好，无法登录，请检查网络连接"));
		return false;
	}

	m_nDelayTimes = 60;
	m_pSentPhoneCodeBtn->SetEnabled(false);
	SetTimer(m_hMainWnd, WM_SENDBTN_DELAY, 1000, (TIMERPROC)CGroupExplorerUI::TimerProcComplete);
	//成功 60秒刷新
	return true;
}

void CRegisterWindowUI::OnReSendEmail()
{
	NDCloudUser::GetInstance()->SendActiveEmail(m_strEmail.c_str(), MakeHttpDelegate(this, &CRegisterWindowUI::OnSendActiveEmail));
}

bool CRegisterWindowUI::OnSendActiveEmail( void * pParam )
{
	THttpNotify* pNotify = static_cast<THttpNotify*>(pParam);

	m_bClickSentPhoneCodeBtn = false;
	if(pNotify->dwErrorCode != 0)
	{
		return false;
	}
	//ShowRegisterEmailSuccessUI();
	return true;
}

void CRegisterWindowUI::NotifyTabToEdit()
{
	CControlUI* pCtrl = GetManager()->FindControl(_T("registerLayout"));
	if (pCtrl)
	{
		if(m_pRegisterUserNameEdit->IsFocused())
		{
			GetManager()->SendNotify(m_pRegisterUserNameEdit, DUI_MSGTYPE_TABSWITCH);
		}
		else if(m_pRegisterPasswordEdit->IsFocused())
		{
			GetManager()->SendNotify(m_pRegisterPasswordEdit, DUI_MSGTYPE_TABSWITCH);
		}
		else if(m_pRegisterConfirmPasswordEdit->IsFocused())
		{
			GetManager()->SendNotify(m_pRegisterConfirmPasswordEdit, DUI_MSGTYPE_TABSWITCH);
		}
		else if(m_pCheckCodeEdit->IsFocused())
		{
			GetManager()->SendNotify(m_pCheckCodeEdit, DUI_MSGTYPE_TABSWITCH);
		}
	}

}

void CRegisterWindowUI::OnRefrshCheckCode()
{
	NDCloudUser::GetInstance()->ObtainCheckCodeImage(MakeHttpDelegate(this, &CRegisterWindowUI::OnCheckCodeImageObtained) );
}	

bool CRegisterWindowUI::GetRegisterStatus()
{
	return m_bRegistering;
}

void CRegisterWindowUI::ClearEditStatus()
{
	m_pRegisterUserNameEdit->SetText(m_pRegisterUserNameEdit->GetTipText().c_str());
	m_pRegisterUserNameEdit->SetClearBtn();
	m_pRegisterPasswordEdit->SetText(m_pRegisterPasswordEdit->GetTipText().c_str());
	m_pRegisterPasswordEdit->SetClearBtn();
	m_pRegisterConfirmPasswordEdit->SetText(m_pRegisterConfirmPasswordEdit->GetTipText().c_str());
	m_pRegisterConfirmPasswordEdit->SetClearBtn();
	m_pCheckCodeEdit->SetText(m_pCheckCodeEdit->GetTipText().c_str());
	m_pCheckCodeEdit->SetClearBtn();

	m_pErrorTipRegisterUserNameLabel->SetVisible(false);
	m_pErrorTipRegisterPasswordLabel->SetVisible(false);
	m_pErrorTipRegisterConfirmPasswordLabel->SetVisible(false);
	m_pErrorRegisterTipLabel->SetVisible(false);
	m_pErrorRegisterTipNetLabel->SetVisible(false);
}
