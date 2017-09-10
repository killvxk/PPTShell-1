#include "stdafx.h"
#include "GetBackPwdWindowUI.h"
#include "NDCloud/NDCloudUser.h"
#include <regex>
#include "Util/Util.h"
#include "DUI/GroupExplorer.h"

CGetBackPwdWindowUI::CGetBackPwdWindowUI()
{
}

CGetBackPwdWindowUI::~CGetBackPwdWindowUI()
{
	m_pManager->KillTimer(this, WM_SENDBTN_DELAY);
}

void CGetBackPwdWindowUI::GetBackPwd(TNotifyUI& msg)
{
	CButtonUI* pButton =  dynamic_cast<CButtonUI*>(msg.pSender);
	if(pButton->GetName() == _T("GetBackPwdStep1Btn"))
	{
		HideGetBackPwdStep1Tip();

		tstring strAccount = m_pAccountEdit->GetText();
		tstring strCheckCode = m_pCheckCodeEdit->GetText();

		if(!CheckGetBackPwdStep1Valid(0))
			return;
		if(!CheckGetBackPwdStep1Valid(1))
			return;

		SetGetBackPwdStep1BtnEnable(false);

		m_ndCloudUser.GetBackPwdConfirmAccount(strAccount, strCheckCode, MakeHttpDelegate(this, &CGetBackPwdWindowUI::OnConfirmAccount));
	}
	else if(pButton->GetName() == _T("GetBackPwdStep2Btn"))
	{
		HideGetBackPwdStep2Tip();

		if(!CheckGetBackPwdStep2Valid(0))
			return;
		if(!CheckGetBackPwdStep2Valid(1))
			return;

		tstring strCheckCode = m_pCheckCodeStep2Edit->GetText();

		SetGetBackPwdStep2BtnEnable(false);

		if(m_pPhoneCheckBox->IsSelected())
		{
			m_ndCloudUser.GetBackPwdCheckPhone(m_strAccount, strCheckCode, MakeHttpDelegate(this, &CGetBackPwdWindowUI::OnResetPwd));
		}
		else if(m_pEmailCheckBox->IsSelected())
		{
			m_ndCloudUser.GetBackPwdCheckEmail(m_strAccount, strCheckCode, MakeHttpDelegate(this, &CGetBackPwdWindowUI::OnResetPwd));
		}
	}
	else if(pButton->GetName() == _T("GetBackPwdStep3Btn"))
	{
		HideGetBackPwdStep3Tip();

		if(!CheckGetBackPwdStep3Valid(0))
			return;
		if(!CheckGetBackPwdStep3Valid(1))
			return;

		tstring strPassword = m_pPasswordEdit->GetText();

		m_ndCloudUser.GetBackPwdResetPassword(m_strAccount, strPassword, m_strTempPassword, MakeHttpDelegate(this, &CGetBackPwdWindowUI::OnModifyPassword));
		SetGetBackPwdStep3BtnEnable(false);
	}
}

void CGetBackPwdWindowUI::Init(HWND hMainWnd)
{
	m_ndCloudUser.ObtainCheckCodeImage(MakeHttpDelegate(this, &CGetBackPwdWindowUI::OnCheckCodeImageObtained) );

	m_pGetBackCheckCode = dynamic_cast<CLabelUI*>(FindSubControl(_T("getBackCheckCode")));

	m_pErrorTipGetBackPwdAccountLabel = dynamic_cast<CLabelUI*>(FindSubControl(_T("error_tip_getbackpwdaccount")));
	m_pErrorTipGetBackPwdCheckcodeLabel = dynamic_cast<CLabelUI*>(FindSubControl(_T("error_tip_getbackpwdcheckcode")));
	m_pErrorTipGetBackPwdStep1 = dynamic_cast<CLabelUI*>(FindSubControl(_T("error_tip_getbackpwdstep1")));

	m_pErrorTipGetBackPwdStep2Select = dynamic_cast<CLabelUI*>(FindSubControl(_T("error_tip_getbackpwdstep2select")));
	m_pErrorTipGetBackPwdStep2CheckCode = dynamic_cast<CLabelUI*>(FindSubControl(_T("error_tip_getbackpwdstep2checkcode")));
	m_pErrorTipGetBackPwdStep2 = dynamic_cast<CLabelUI*>(FindSubControl(_T("error_tip_getbackpwdstep2")));

	m_pErrorTipGetBackPwdStep3Password = dynamic_cast<CLabelUI*>(FindSubControl(_T("error_tip_getbackpwdstep3password")));
	m_pErrorTipGetBackPwdStep3ConfirmPassword = dynamic_cast<CLabelUI*>(FindSubControl(_T("error_tip_getbackpwdstep3confirmpassword")));
	m_pErrorTipGetBackPwdStep3 = dynamic_cast<CLabelUI*>(FindSubControl(_T("error_tip_getbackpwdstep3")));
	

	m_pAccountEdit = dynamic_cast<CEditClearUI*>(FindSubControl(_T("getbackpwdaccount")));
	m_pCheckCodeEdit = dynamic_cast<CEditClearUI*>(FindSubControl(_T("getbackpwdcheckcode")));
	m_pCheckCodeStep2Edit = dynamic_cast<CEditClearUI*>(FindSubControl(_T("getbackpwdstep2checkcode")));

	m_pPasswordEdit = dynamic_cast<CEditClearUI*>(FindSubControl(_T("getbackpwdpassword")));
	m_pConfirmPasswordEdit = dynamic_cast<CEditClearUI*>(FindSubControl(_T("getbackpwdconfirmpassword")));

	m_pPhoneCheckBox = dynamic_cast<CCheckBoxUI*>(FindSubControl(_T("phoneCheckBox")));
	m_pEmailCheckBox = dynamic_cast<CCheckBoxUI*>(FindSubControl(_T("emailCheckBox")));

	m_pGetBackPwdStep1Btn = dynamic_cast<CButtonUI*>(FindSubControl(_T("GetBackPwdStep1Btn")));
	m_pGetBackPwdStep2Btn = dynamic_cast<CButtonUI*>(FindSubControl(_T("GetBackPwdStep2Btn")));
	m_pGetBackPwdStep3Btn = dynamic_cast<CButtonUI*>(FindSubControl(_T("GetBackPwdStep3Btn")));
	m_pSentPhoneCodeBtn = dynamic_cast<CButtonUI*>(FindSubControl(_T("sentphonecode")));
	m_pBackLoginBtn = dynamic_cast<CButtonUI*>(FindSubControl(_T("backlogin_getbackpwd")));
	m_pBackLoginBtn->OnEvent += MakeDelegate(this, &CGetBackPwdWindowUI::OnBackLoginEvent);

	m_pSliderTabLayout = dynamic_cast<CSliderTabLayoutUI*>(FindSubControl(_T("getbackpwdslider")));
	
	m_pPhone = dynamic_cast<CLabelUI*>(FindSubControl(_T("getbackpwdphone")));
	m_pEmail = dynamic_cast<CLabelUI*>(FindSubControl(_T("getbackpwdemail")));

	m_pCheckAccountInfoLayout = dynamic_cast<CVerticalLayoutUI*>(FindSubControl(_T("checkAccountInfo")));
	m_pCheckAccountInfoPhoneLayout = dynamic_cast<CHorizontalLayoutUI*>(FindSubControl(_T("checkAccountInfoPhone")));
	m_pCheckAccountInfoEmailLayout = dynamic_cast<CHorizontalLayoutUI*>(FindSubControl(_T("checkAccountInfoEmail")));

	m_pGetBackPwdTipLayout = dynamic_cast<CVerticalLayoutUI*>(FindSubControl(_T("GetBackPwdTipLayout")));

}

bool CGetBackPwdWindowUI::OnCheckCodeImageObtained( void * pParam )
{
	THttpNotify*  pNotify = (THttpNotify* )pParam;
	if(pNotify->dwErrorCode == 0)
	{
		tstring strFilePath = pNotify->strFilePath;
		m_pGetBackCheckCode->SetBkImage(strFilePath.c_str());
	}

	return true;
}

void CGetBackPwdWindowUI::OnRefrshCheckCode()
{
	m_ndCloudUser.ObtainCheckCodeImage(MakeHttpDelegate(this, &CGetBackPwdWindowUI::OnCheckCodeImageObtained) );
}

void CGetBackPwdWindowUI::SetGetBackPwdStep1BtnEnable( bool bEnable )
{
	if(bEnable)
	{
		m_pGetBackPwdStep1Btn->SetEnabled(true);
	}
	else
	{
		m_pGetBackPwdStep1Btn->SetEnabled(false);;
	}
}

void CGetBackPwdWindowUI::SetGetBackPwdStep2BtnEnable( bool bEnable )
{
	if(bEnable)
	{
		m_pGetBackPwdStep2Btn->SetEnabled(true);
	}
	else
	{
		m_pGetBackPwdStep2Btn->SetEnabled(false);;
	}
}

void CGetBackPwdWindowUI::SetGetBackPwdStep3BtnEnable( bool bEnable )
{
	if(bEnable)
	{
		m_pGetBackPwdStep3Btn->SetEnabled(true);
	}
	else
	{
		m_pGetBackPwdStep3Btn->SetEnabled(false);;
	}
}

void CGetBackPwdWindowUI::HideGetBackPwdStep1Tip()
{
	if(m_pErrorTipGetBackPwdAccountLabel->IsVisible())
	{
		m_pErrorTipGetBackPwdAccountLabel->SetVisible(false);
	}
	if(m_pErrorTipGetBackPwdCheckcodeLabel->IsVisible())
	{
		m_pErrorTipGetBackPwdCheckcodeLabel->SetVisible(false);
	}
	if(m_pErrorTipGetBackPwdStep1->IsVisible())
	{
		m_pErrorTipGetBackPwdStep1->SetVisible(false);
	}
}

void CGetBackPwdWindowUI::HideGetBackPwdStep2Tip()
{
	if(m_pErrorTipGetBackPwdStep2Select->IsVisible())
	{
		m_pErrorTipGetBackPwdStep2Select->SetVisible(false);
	}
	if(m_pErrorTipGetBackPwdStep2CheckCode->IsVisible())
	{
		m_pErrorTipGetBackPwdStep2CheckCode->SetVisible(false);
	}
	if(m_pErrorTipGetBackPwdStep2->IsVisible())
	{
		m_pErrorTipGetBackPwdStep2->SetVisible(false);
	}
}

void CGetBackPwdWindowUI::HideGetBackPwdStep3Tip()
{
	if(m_pErrorTipGetBackPwdStep3Password->IsVisible())
	{
		m_pErrorTipGetBackPwdStep3Password->SetVisible(false);
	}
	if(m_pErrorTipGetBackPwdStep3ConfirmPassword->IsVisible())
	{
		m_pErrorTipGetBackPwdStep3ConfirmPassword->SetVisible(false);
	}
	if(m_pErrorTipGetBackPwdStep3->IsVisible())
	{
		m_pErrorTipGetBackPwdStep3->SetVisible(false);
	}
}


bool CGetBackPwdWindowUI::CheckGetBackPwdStep1Valid( int nIndex )
{
	tstring strAccount = m_pAccountEdit->GetText();
	tstring strCheckCode = m_pCheckCodeEdit->GetText();

	switch(nIndex)
	{
	case 0:
		{
			if(strAccount.empty() || strAccount == m_pAccountEdit->GetTipText())
			{
				ShowGetBackPwdStep1Tip(0, _T("账号不能为空"));
				return false;
			}

			//
			std::tr1::regex szUserNameRule("^\\d+$");
			std::tr1::smatch sResult;
			if(std::tr1::regex_search(strAccount, sResult, szUserNameRule))
			{
				//手机
				std::tr1::regex szUserNameRuleEx("^1[34578]\\d{9}$");

				if(!std::tr1::regex_search(strAccount, sResult, szUserNameRuleEx))
				{
					ShowGetBackPwdStep1Tip(0, _T("输入的手机号码格式不正确 ,请输入11位有效的手机号码"));
					return false;
				}

			}
			else
			{
				//邮箱
				std::tr1::regex szUserNameRuleEx("^[a-zA-Z0-9_-]+@[a-zA-Z0-9_-]+(\\.[a-zA-Z0-9_-]+)+$");
				if(!std::tr1::regex_search(strAccount, sResult, szUserNameRuleEx))
				{
					ShowGetBackPwdStep1Tip(0, _T("您输入的邮箱格式有误 ,请输入正确的邮箱"));
					return false;
				}
			}
		}
		break;
	case 1:
		{
			if(strCheckCode.empty() || strCheckCode == m_pCheckCodeEdit->GetTipText())
			{
				ShowGetBackPwdStep1Tip(1, _T("验证码不能为空"));
				return false;
			}
		}
		break;
	}
	return true;
}

bool CGetBackPwdWindowUI::CheckGetBackPwdStep2Valid( int nIndex )
{
	tstring strCheckCode = m_pCheckCodeStep2Edit->GetText();

	switch(nIndex)
	{
	case 0:
		{
			if(!m_pPhoneCheckBox->IsSelected() && !m_pEmailCheckBox->IsSelected())
			{
				ShowGetBackPwdStep2Tip(0, _T("请选择验证方式"));
				return false;
			}
		}
		break;
	case 1:
		{
			if(strCheckCode.empty() || strCheckCode == m_pCheckCodeEdit->GetTipText())
			{
				ShowGetBackPwdStep2Tip(1, _T("验证码不能为空"));
				return false;
			}
		}
		break;
	}
	return true;
}

bool CGetBackPwdWindowUI::CheckGetBackPwdStep3Valid( int nIndex )
{
	tstring strPassword = m_pPasswordEdit->GetText();
	tstring strConfirmPassword = m_pConfirmPasswordEdit->GetText();

	switch(nIndex)
	{
	case 0:
		{
			if( strPassword.empty() || strPassword == m_pPasswordEdit->GetTipText())
			{
				ShowGetBackPwdStep3Tip(0, _T("密码不能为空"));
				return false;
			}
			if( strPassword.length() < 6)
			{
				ShowGetBackPwdStep3Tip(0, _T("密码长度不能少于6个"));
				return false;
			}
		}
		break;
	case 1:
		{
			if( strConfirmPassword.empty() || strConfirmPassword == m_pConfirmPasswordEdit->GetTipText())
			{
				ShowGetBackPwdStep3Tip(1, _T("重复密码不能为空"));
				return false;
			}
			if( strConfirmPassword.length() < 6)
			{
				ShowGetBackPwdStep3Tip(1, _T("重复密码长度不能少于6个"));
				return false;
			}
			else if(strPassword != strConfirmPassword)
			{
				ShowGetBackPwdStep3Tip(0, _T("两次输入密码不相同"));
				ShowGetBackPwdStep3Tip(1, _T("两次输入密码不相同"));
				return false;
			}
		}
		break;
	}
	return true;
}

void CGetBackPwdWindowUI::ShowGetBackPwdStep1Tip( int nType,tstring strTip )
{
	if(nType == 0)
	{
		m_pErrorTipGetBackPwdAccountLabel->SetText(strTip.c_str());
		m_pErrorTipGetBackPwdAccountLabel->SetVisible(true);
	}
	else if(nType == 1)
	{
		m_pErrorTipGetBackPwdCheckcodeLabel->SetText(strTip.c_str());
		m_pErrorTipGetBackPwdCheckcodeLabel->SetVisible(true);
	}
	else if(nType == 2)
	{
		m_pErrorTipGetBackPwdStep1->SetText(strTip.c_str());
		m_pErrorTipGetBackPwdStep1->SetVisible(true);
	}

	SetGetBackPwdStep1BtnEnable(true);
}

void CGetBackPwdWindowUI::ShowGetBackPwdStep2Tip( int nType,tstring strTip )
{
	if(nType == 0)
	{
		m_pErrorTipGetBackPwdStep2Select->SetText(strTip.c_str());
		m_pErrorTipGetBackPwdStep2Select->SetVisible(true);
	}
	else if(nType == 1)
	{
		m_pErrorTipGetBackPwdStep2CheckCode->SetText(strTip.c_str());
		m_pErrorTipGetBackPwdStep2CheckCode->SetVisible(true);
	}
	else if(nType == 2)
	{
		m_pErrorTipGetBackPwdStep2->SetText(strTip.c_str());
		m_pErrorTipGetBackPwdStep2->SetVisible(true);
	}
	SetGetBackPwdStep2BtnEnable(true);
}

void CGetBackPwdWindowUI::ShowGetBackPwdStep3Tip( int nType,tstring strTip )
{
	if(nType == 0)
	{
		m_pErrorTipGetBackPwdStep3Password->SetText(strTip.c_str());
		m_pErrorTipGetBackPwdStep3Password->SetVisible(true);
	}
	else if(nType == 1)
	{
		m_pErrorTipGetBackPwdStep3ConfirmPassword->SetText(strTip.c_str());
		m_pErrorTipGetBackPwdStep3ConfirmPassword->SetVisible(true);
	}
	else if(nType == 2)
	{
		m_pErrorTipGetBackPwdStep3->SetText(strTip.c_str());
		m_pErrorTipGetBackPwdStep3->SetVisible(true);
	}
	SetGetBackPwdStep3BtnEnable(true);
}

bool CGetBackPwdWindowUI::OnConfirmAccount( void * pParam )
{
	GetBackPwdParam* p = (GetBackPwdParam*)pParam;
	if(p->dwErrorCode != 0)
	{
		ShowGetBackPwdStep1Tip(2, _T("当前网络不太好，请检查网络连接"));
		return false;
	}

	OnRefrshCheckCode();
	
	if(p->strCode != _T(""))
	{

		ShowGetBackPwdStep1Tip(2, p->strMessage);
		return false;
	}

	m_pAccountEdit->SetText(_T(""));
	m_pCheckCodeEdit->SetText(_T(""));
	m_strAccount = p->strAccount;
	m_strEmail = p->strEmail;
	m_strPhone = p->strPhone;

	if(m_strEmail == _T("") && m_strPhone == _T(""))
	{
		ShowGetBackPwdStep1Tip(2, _T("该账号并未申请密保方式"));
		return false;
	}

	if(m_strEmail != _T(""))
	{
		m_pCheckAccountInfoPhoneLayout->SetVisible(false);
		m_pCheckAccountInfoEmailLayout->SetVisible(true);
		m_pEmail->SetText(m_strEmail.c_str());
	}
	if(m_strPhone != _T(""))
	{
		m_pCheckAccountInfoPhoneLayout->SetVisible(true);
		m_pCheckAccountInfoEmailLayout->SetVisible(false);
		m_pPhone->SetText(m_strPhone.c_str());
	}

	m_pPhoneCheckBox->Selected(false);
	m_pEmailCheckBox->Selected(false);

	SetGetBackPwdStep1BtnEnable(true);
	m_pSliderTabLayout->SelectItem(1);
	return true;
}

bool CGetBackPwdWindowUI::OnCheck( void * pParam )
{
	GetBackPwdParam* p = (GetBackPwdParam*)pParam;
	if(p->dwErrorCode != 0)
	{
		ShowGetBackPwdStep2Tip(2, _T("当前网络不太好，请检查网络连接"));
		return false;
	}

	if(p->strCode != _T(""))
	{
		ShowGetBackPwdStep2Tip(2, p->strMessage);
		return false;
	}

	return true;
}

bool CGetBackPwdWindowUI::OnResetPwd( void * pParam )
{
	GetBackPwdParam* p = (GetBackPwdParam*)pParam;
	if(p->dwErrorCode != 0)
	{
		ShowGetBackPwdStep2Tip(2, _T("当前网络不太好，请检查网络连接"));
		return false;
	}

	if(p->strCode != _T(""))
	{
		ShowGetBackPwdStep2Tip(2, p->strMessage);
		return false;
	}

	m_strTempPassword = p->strTempPwd;
	m_pCheckCodeStep2Edit->SetText(_T(""));
	SetGetBackPwdStep2BtnEnable(true);
	m_pSliderTabLayout->SelectItem(2);
	return true;
}

void CGetBackPwdWindowUI::OnSentPhoneCode()
{
	HideGetBackPwdStep2Tip();

	if(!CheckGetBackPwdStep2Valid(0))
		return;


	if(m_pPhoneCheckBox->IsSelected())
	{
		m_ndCloudUser.SendGetBackPwdSMS(m_strAccount, MakeHttpDelegate(this, &CGetBackPwdWindowUI::OnCheck));
	}
	else if(m_pEmailCheckBox->IsSelected())
	{
		m_ndCloudUser.SendGetBackPwdEmail(m_strAccount, MakeHttpDelegate(this, &CGetBackPwdWindowUI::OnCheck));
	}

	m_nDelayTimes = 60;
	m_pSentPhoneCodeBtn->SetEnabled(false);
	m_pManager->SetTimer( this, WM_SENDBTN_DELAY, 1000 );
}

bool CGetBackPwdWindowUI::OnModifyPassword( void * pParam )
{
	GetBackPwdParam* p = (GetBackPwdParam*)pParam;
	if(p->dwErrorCode != 0)
	{
		ShowGetBackPwdStep3Tip(2, _T("当前网络不太好，请检查网络连接"));
		return false;
	}

	if(p->strCode != _T(""))
	{
		ShowGetBackPwdStep3Tip(2, p->strMessage);
		return false;
	}

	SetGetBackPwdStep3BtnEnable(true);
	m_pPasswordEdit->SetText(_T(""));
	m_pConfirmPasswordEdit->SetText(_T(""));
	m_strAccount = _T("");
	m_strEmail = _T("");
	m_strPhone = _T("");
	m_strTempPassword = _T("");

	m_pGetBackPwdTipLayout->SetVisible(true);
	m_pManager->SetTimer( this, WM_RESETPWD_SUCCESS, 1000 );
	return true;
}

void CGetBackPwdWindowUI::DoEvent( TEventUI& event )
{
	if (event.Type == UIEVENT_TIMER )
	{
		if(event.wParam == WM_SENDBTN_DELAY )
		{
			m_nDelayTimes--;
			if(m_nDelayTimes <=0)
			{
				m_pManager->KillTimer(this, WM_SENDBTN_DELAY);
				m_pSentPhoneCodeBtn->SetText(_T("发送验证码"));
				m_pSentPhoneCodeBtn->SetEnabled(true);
			}
			else
			{
				TCHAR szBuff[MAX_PATH];
				_stprintf(szBuff, _T("重新发送 (%d)"), m_nDelayTimes);
				m_pSentPhoneCodeBtn->SetText(szBuff);
			}
		}
		else if(event.wParam == WM_RESETPWD_SUCCESS )
		{
			m_pManager->KillTimer(this, WM_RESETPWD_SUCCESS);
			m_pGetBackPwdTipLayout->SetVisible(false);
			if(CGroupExplorerUI::GetInstance()->getSliderTabLayout()->GetCurSel() == 3 && ::IsWindowVisible(CGroupExplorerUI::GetInstance()->GetHWND()))
			{
				CGroupExplorerUI::GetInstance()->ShowWindow(false);
			}
		}
	}
}

bool CGetBackPwdWindowUI::OnBackLoginEvent( void * pParam )
{
	TEventUI* pNotify = (TEventUI*)pParam;
	if (pNotify->Type == UIEVENT_BUTTONDOWN )
	{

	}
	return true;
}

void CGetBackPwdWindowUI::ClearEditStatus()
{
	m_pConfirmPasswordEdit->SetText(m_pConfirmPasswordEdit->GetTipText().c_str());
	m_pConfirmPasswordEdit->SetClearBtn();
	m_pPasswordEdit->SetText(m_pPasswordEdit->GetTipText().c_str());
	m_pPasswordEdit->SetClearBtn();
	m_pAccountEdit->SetText(m_pAccountEdit->GetTipText().c_str());
	m_pAccountEdit->SetClearBtn();
	m_pCheckCodeEdit->SetText(m_pCheckCodeEdit->GetTipText().c_str());
	m_pCheckCodeEdit->SetClearBtn();
	m_pCheckCodeStep2Edit->SetText(m_pCheckCodeStep2Edit->GetTipText().c_str());
	m_pCheckCodeStep2Edit->SetClearBtn();

	m_pErrorTipGetBackPwdAccountLabel->SetVisible(false);
	m_pErrorTipGetBackPwdCheckcodeLabel->SetVisible(false);
	m_pErrorTipGetBackPwdStep1->SetVisible(false);

	m_pErrorTipGetBackPwdStep2Select->SetVisible(false);
	m_pErrorTipGetBackPwdStep2CheckCode->SetVisible(false);
	m_pErrorTipGetBackPwdStep2->SetVisible(false);

	m_pErrorTipGetBackPwdStep3Password->SetVisible(false);
	m_pErrorTipGetBackPwdStep3ConfirmPassword->SetVisible(false);
	m_pErrorTipGetBackPwdStep3->SetVisible(false);
}
