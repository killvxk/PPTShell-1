#include "StdAfx.h"
#include "ModifyPasswordDialogUI.h"
#include "Util/Util.h"
#include "Encode.h"

CModifyPasswordDialogUI::CModifyPasswordDialogUI(void)
{
}

CModifyPasswordDialogUI::~CModifyPasswordDialogUI(void)
{
}

LPCTSTR CModifyPasswordDialogUI::GetWindowClassName( void ) const
{
	return _T("CModifyPasswordDialog");
}

DuiLib::CDuiString CModifyPasswordDialogUI::GetSkinFile()
{
	return _T("Options\\ModifyPassword.xml");
}

DuiLib::CDuiString CModifyPasswordDialogUI::GetSkinFolder()
{
	return _T("skins");
}

void CModifyPasswordDialogUI::InitWindow()
{
	;
}

void CModifyPasswordDialogUI::OnBtnCloseClick(TNotifyUI& msg)
{
	Close();
}

void CModifyPasswordDialogUI::OnEditOldPasswordKillfocus(TNotifyUI& msg)
{
	CheckOldPassword();
}

bool CModifyPasswordDialogUI::CheckOldPassword()
{
	tstring strConfinFile = GetLocalPath();
	strConfinFile += _T("\\Setting\\Config.ini");
	TCHAR password[MAX_PATH + 1] = {0};
	GetPrivateProfileString(_T("Option"), _T("password"), _T(""), password, MAX_PATH, strConfinFile.c_str());
	
	CString strPassword = Decrypt(password, (WORD)100);

	CRichEditUI* pEditOldPassword = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("OldPassword")));
	CString OldPassword = pEditOldPassword->GetText().GetData();
	if (!OldPassword.IsEmpty())
	{
		CLabelUI* pLabelOKPic = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("OKPic")));
		CLabelUI* pLabelErrorMsg = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("errorMsg")));

		if (_tcscmp(strPassword.GetBuffer(), OldPassword) == 0)
		{
			pLabelOKPic->SetVisible(true);
			pLabelErrorMsg->SetVisible(false);

			return true;
		}
		else
		{
			pLabelOKPic->SetVisible(false);
			pLabelErrorMsg->SetVisible(true);

			return false;
		}
	}

	return false;

}

void CModifyPasswordDialogUI::OnEditComfirmPasswordKillfocus(TNotifyUI& msg)
{
	CheckNewPassword();
}

bool CModifyPasswordDialogUI::CheckNewPassword()
{
	CRichEditUI* pEditNewPassword = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("NewPassword")));
	CRichEditUI* pEditComfirmPassword = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("ComfirmPassword")));
	CLabelUI* pLabelError3 = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("errorMsg3")));
	CLabelUI* pLabelError2 = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("errorMsg2")));

	if (_tcscmp(pEditNewPassword->GetText().GetData(), _T("")) != 0)
	{
		pLabelError3->SetVisible(false);

		if (_tcscmp(pEditNewPassword->GetText().GetData(), pEditComfirmPassword->GetText().GetData()) == 0)
		{
			pLabelError2->SetVisible(false);
			return TRUE;
		}
		else
		{
			pLabelError2->SetVisible(true);
			return FALSE;
		}
	}
	else
	{
		pLabelError3->SetVisible(true);
		return FALSE;
	}

	return FALSE;

	
}

void CModifyPasswordDialogUI::OnBtnOkClick(TNotifyUI& msg)
{
	if (CheckOldPassword())
	{
		if (CheckNewPassword())
		{
			tstring strConfinFile = GetLocalPath();
			strConfinFile += _T("\\Setting\\Config.ini");
			
			CRichEditUI* pEditNewPassword = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("NewPassword")));
				
			CString strPassword = Encrypt(pEditNewPassword->GetText().GetData(), (WORD)100);

			WritePrivateProfileString(_T("Option"), _T("password"), strPassword.GetBuffer(), strConfinFile.c_str());
			CToast::GetInstance(this->GetHWND())->Show(_T("ÐÞ¸ÄÃÜÂë³É¹¦"));
			
			Close();
		}
	}
}
