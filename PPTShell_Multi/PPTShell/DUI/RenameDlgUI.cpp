#include "StdAfx.h"
#include "RenameDlgUI.h"
#include "Util\Util.h"

CRenameDlgUI::CRenameDlgUI(void)
{
	m_bActiveClose = false;
	m_strDefaultTipText = "";
	m_pEditRename = NULL;
	m_strNewName = "";
	m_strOldName = "";
}

CRenameDlgUI::~CRenameDlgUI(void)
{
}

LPCTSTR CRenameDlgUI::GetWindowClassName() const
{
	return _T("RenameDlgUI");
}

DuiLib::CDuiString CRenameDlgUI::GetSkinFile()
{
	return _T("Commom\\RenameDlg.xml");
}

DuiLib::CDuiString CRenameDlgUI::GetSkinFolder()
{
	return _T("skins");
}

void CRenameDlgUI::InitWindow()
{
	m_pEditRename = dynamic_cast<CEditUI*>(m_PaintManager.FindControl(_T("editRename")));
}



void CRenameDlgUI::OnBtnCancel( TNotifyUI& msg )
{
	m_bActiveClose = true;
	this->Close(IDCANCEL);
}

void CRenameDlgUI::OnBtnOK( TNotifyUI& msg )
{
	bool canClose=false;
	bool isSameWithOld = false;
	if(m_pEditRename)
	{
		string strText= m_pEditRename->GetText();
		if(strText.empty())
		{
			CToast::Toast("请输入名称");
		}
		else
		{
			m_strNewName = m_pEditRename->GetText();
			m_strNewName = trim(m_strNewName);
			if(!IsNameValid(m_strNewName.c_str()))
			{
				CToast::Toast("输入名称含无效字符");
			}
			else if ( m_strNewName.empty() )
			{
				CToast::Toast(_T("名称不能为空或者全空格"));
			}
			else
			{
				if(m_strNewName.compare(m_strOldName) == 0)
				{
					isSameWithOld = true;
				}
				canClose=true;
			}
		}
	}
	if(canClose)
	{
		m_bActiveClose = true;
		if(!isSameWithOld)
			this->Close(IDOK);
		else
			this->Close(IDCANCEL);
	}
}

bool CRenameDlgUI::IsNameValid(const TCHAR* FileName)
{
	TCHAR SpecialCha[] = {'\\','/','\"',':','*','?','<','>','|','\t'};
	for (int i=0; i< sizeof(SpecialCha)/sizeof(TCHAR); i++)
	{
		if (NULL!=_tcschr(FileName, SpecialCha[i]))
			return false;
	}
	return true;
}

string CRenameDlgUI::GetNewName()
{
	return m_strNewName;
}

// LRESULT CRenameDlgUI::OnClose( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
// {
// 	if(!m_bActiveClose)
// 	{
// 		int a=0;
// 		wParam = IDCANCEL;
// 		return FALSE;
// 	}
// 	return __super::OnClose(uMsg, wParam ,lParam, bHandled);
// }

LRESULT CRenameDlgUI::ResponseDefaultKeyEvent( WPARAM wParam )
{
	if (wParam == VK_RETURN)
	{
		return FALSE;
	}
	else if (wParam == VK_ESCAPE)
	{
		Close(IDCANCEL);
		return TRUE;
	}

	return FALSE;
}

void CRenameDlgUI::SetOldName2Edit(string strOldName)
{
	if (m_pEditRename)
	{
//		m_pEditRename->SetFocus();
		m_pEditRename->SetText(strOldName.c_str());
//		m_pEditRename->SetSelAll();
		m_pEditRename->Invalidate();
	}

	m_strOldName = strOldName;
}

void CRenameDlgUI::AdjustWindowPos()
{
	CRect rt;
	HWND hwndMain = AfxGetMainWnd()->GetSafeHwnd();
	::GetWindowRect(hwndMain, rt);

	HWND hwnd = GetHWND();
	CRect rectRename;
	::GetWindowRect(hwnd,&rectRename);

	CRect rtToast;
	::GetWindowRect(hwnd, rtToast);

	::MoveWindow(hwnd,
		rt.left + (rt.Width() - rectRename.Width())/2,
		rt.top + (rt.Height() - rectRename.Height())/2,
		rectRename.Width(),
		rectRename.Height(),
		TRUE);
}

LRESULT CRenameDlgUI::OnSysCommand( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if(wParam ==SC_CLOSE)
	{
		Close(IDCANCEL);
		bHandled = TRUE;
		return FALSE;
	}
	return __super::OnSysCommand(uMsg, wParam, lParam, bHandled);
}
