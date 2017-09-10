#include "StdAfx.h"
#include "PasswordLoginDialogUI.h"
#include "Util/Util.h"
#include "Encode.h"

CPasswordLoginDialogUI::CPasswordLoginDialogUI(void)
{
}

CPasswordLoginDialogUI::~CPasswordLoginDialogUI(void)
{
}

LPCTSTR CPasswordLoginDialogUI::GetWindowClassName( void ) const
{
	return _T("CPasswordLoginDialog");
}

DuiLib::CDuiString CPasswordLoginDialogUI::GetSkinFile()
{
	return _T("Options\\PasswordLogin.xml");
}

DuiLib::CDuiString CPasswordLoginDialogUI::GetSkinFolder()
{
	return _T("skins");
}

void CPasswordLoginDialogUI::InitWindow()
{
	CLabelUI* pLabelTitle = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("title")));
	pLabelTitle->SetText(m_title.c_str());
}

void CPasswordLoginDialogUI::OnBtnCloseClick(TNotifyUI& msg)
{
	Close(IDCANCEL);
}

void CPasswordLoginDialogUI::SetTitleWords(tstring title)
{
	m_title = title;
}

void CPasswordLoginDialogUI::OnBtnOKClick( TNotifyUI& msg )
{
	tstring strConfinFile = GetLocalPath();
	strConfinFile += _T("\\Setting\\Config.ini");
	TCHAR password[MAX_PATH + 1] = {0};
	GetPrivateProfileString(_T("Option"), _T("password"), _T(""), password, MAX_PATH, strConfinFile.c_str());

	CString strPassword = Decrypt(password, (WORD)100);

	CRichEditUI* pEditPassword = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("password")));
	if (_tcscmp(strPassword.GetBuffer(), pEditPassword->GetText().GetData()) == 0)
	{
		Close(IDOK);
	}

}

void CPasswordLoginDialogUI::OnEditPasswordKillfocus( TNotifyUI& msg )
{
	tstring strConfinFile = GetLocalPath();
	strConfinFile += _T("\\Setting\\Config.ini");
	TCHAR password[MAX_PATH + 1] = {0};
	GetPrivateProfileString(_T("Option"), _T("password"), _T(""), password, MAX_PATH, strConfinFile.c_str());

	CString strPassword = Decrypt(password, (WORD)100);

	CRichEditUI* pEditPassword = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("password")));
	if (_tcscmp(strPassword.GetBuffer(), pEditPassword->GetText().GetData()) == 0)
	{
		CLabelUI* pEditLabel = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("errorMsg")));
		pEditLabel->SetVisible(FALSE);
	}
	else
	{
		CLabelUI* pEditLabel = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("errorMsg")));
		pEditLabel->SetVisible(TRUE);
	}
}