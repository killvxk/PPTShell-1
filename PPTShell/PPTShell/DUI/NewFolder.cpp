#include "StdAfx.h"
#include "NewFolder.h"
#include "ListBarDlg.h"
#include "Toast.h"


CNewFolder* CNewFolder::m_pInstance = NULL;
CNewFolder* CNewFolder::GetInstance()
{
	if (!m_pInstance)
	{
		m_pInstance = new CNewFolder;
	}
	return m_pInstance;
}

//CNewFolder::CNewFolder(CListBarDlg* pDlg)
//{
//	m_pEdit			= NULL;
//	m_pButton		= NULL;
//	m_pListBarDlg	= pDlg;
//}

CNewFolder::CNewFolder(void)
{
	m_pEdit			= NULL;
	m_pButton		= NULL;
	m_plblTip		= NULL;
	m_bModify		= false;
}

CNewFolder::~CNewFolder(void)
{
}

LPCTSTR CNewFolder::GetWindowClassName() const
{
	return _T("NewFolder");
}

DuiLib::CDuiString CNewFolder::GetSkinFile()
{
	return _T("NewFolder\\NewFolder.xml");
}

DuiLib::CDuiString CNewFolder::GetSkinFolder()
{
	return _T("skins");
}

void CNewFolder::Init( HWND hWndParent, void* pDlg)
{
	m_pListBarDlg = pDlg;

	Create(hWndParent, _T("NewFolder"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);

	m_WndShadow.Create(m_hWnd, hWndParent);

	m_pEdit		= static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("SearchEdit")));
	m_plblTip	= static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lblTip")));
	m_pButton	= static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("AddBtn")));

	//m_pEdit->OnEvent += MakeDelegate(this, &CNewFolder::OnRichEditNotify);
}

void CNewFolder::OnBtnClose(TNotifyUI& msg)
{
	Close();
}

void CNewFolder::OnRichEditEvent()
{
	if ( m_pEdit != NULL && m_pButton != NULL)
	{
		CDuiString strText		= m_pEdit->GetText();
		CDuiString strTipText	= m_pEdit->GetTipText();
		tstring str				= strText.GetData();
		str = trim(str);

		if (str.empty() || strText.Compare(strTipText) == 0 )
		{
			m_pButton->SetEnabled(false);
		}
		else
		{
			m_pButton->SetEnabled(true);
		}
			
	}

}

bool CNewFolder::IsFileNameValid(const TCHAR* FileName)
{
	TCHAR SpecialCha[] = {'\\','/','\"',':','*','?','<','>','|','\t'};
	for (int i=0; i< sizeof(SpecialCha)/sizeof(TCHAR); i++)
	{
		if (NULL!=_tcschr(FileName, SpecialCha[i]))
			return false;
	}
	return true;
}

LRESULT CNewFolder::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
#ifdef _DEBUG
	if (wParam == VK_F5)
	{
		return __super::HandleMessage(uMsg, wParam, lParam);
	}
#endif

	if( uMsg == WM_KEYDOWN)
	{
		switch (wParam)
		{
		case VK_ESCAPE:
		case VK_SPACE:
		case VK_RETURN:
			return S_FALSE;
		}
	}
	OnRichEditEvent();

	return __super::HandleMessage(uMsg, wParam, lParam);
}

void CNewFolder::SetFolderName( CDuiString strName )
{
	m_bModify		= true;
	m_strOldName	= strName;
	m_pEdit->SetText(strName);
}

bool CNewFolder::OnRichEditNotify( void* pObj )
{
	TEventUI* pEvent = (TEventUI*)pObj;

	if ( pEvent->Type == UIEVENT_SETFOCUS )
	{
		m_pEdit->SetSelAll();
	}
	//else if (pEvent->Type == UIEVENT_NOTIFY)
	//{
	//	if (m_pEdit->GetText() != m_pEdit->GetTipText())
	//		m_pEdit->SetTextColor(m_pEdit->GetTextColor());
	//}

	return 1;
}

void CNewFolder::OnBtnAdd( TNotifyUI& msg )
{

}
