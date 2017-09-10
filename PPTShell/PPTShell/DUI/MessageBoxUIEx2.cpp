#include "StdAfx.h"
#include "MessageBoxUIEx2.h"
#include "Util/Util.h"

CMessageBoxUIEx2::CMessageBoxUIEx2(void)
{
}

CMessageBoxUIEx2::~CMessageBoxUIEx2(void)
{
}

LPCTSTR CMessageBoxUIEx2::GetWindowClassName( void ) const
{
	return _T("MessageBox");
}

DuiLib::CDuiString CMessageBoxUIEx2::GetSkinFile()
{
	return _T("MessageBox\\MessageBoxEx2.xml");
}

DuiLib::CDuiString CMessageBoxUIEx2::GetSkinFolder()
{
	return _T("skins");
}

void CMessageBoxUIEx2::InitWindow()
{
	m_pVerticalLayout = static_cast<CVerticalLayoutUI *>(m_PaintManager.FindControl(_T("MessageBox")));
	if (NULL == m_pVerticalLayout) return;
	m_pMessageTitle = static_cast<CLabelUI*>(m_pVerticalLayout->FindSubControl(_T("title")));
	m_pMessageContent = static_cast<CLabelUIEx*>(m_pVerticalLayout->FindSubControl(_T("content")));
	m_pMessageType = m_pVerticalLayout->FindSubControl(_T("type"));	
}


CControlUI * CMessageBoxUIEx2::GetItem(LPCTSTR lpszName)
{
	return static_cast<CControlUI*>(m_pVerticalLayout->FindSubControl(lpszName));
}

void CMessageBoxUIEx2::SetState(ENMessageBoxTypeEx2 eMessageType)
{
	CControlUI* pType = GetItem(_T("type"));
	if (NULL == pType) return;

	switch (eMessageType)
	{
	case enMessageBoxTypeSucceed:
		{
			pType->SetBkImage(_T("file=\'MessageBox\\tooltip_icon.png' source=\'0,0,40,40\'"));
		}
		break;
	case enMessageBoxTypeError:
		{
			pType->SetBkImage(_T("file=\'MessageBox\\tooltip_icon.png' source=\'120,0,160,40\'"));
		}
		break;
	case enMessageBoxTypeInfo:
		{
			pType->SetBkImage(_T("file=\'MessageBox\\tooltip_icon.png' source=\'40,0,80,40\'"));
		}
		break;
	case enMessageBoxTypeWarn:
		{
			pType->SetBkImage(_T("file=\'MessageBox\\tooltip_icon.png' source=\'80,0,120,40\'"));
		}
		break;
	case enMessageBoxTypeQuestion:
		{
			pType->SetBkImage(_T("file=\'MessageBox\\tooltip_icon.png' source=\'160,0,200,40\'"));
		}
		break;
	default:
		break;
	}


}


UINT CMessageBoxUIEx2::MessageBox(HWND hParent, LPCTSTR lpszText, LPCTSTR lpszCaption,  LPCTSTR lpszBtnType, ENMessageBoxTypeEx2 enIconType , UINT nDefaultButtonIndex , LPCTSTR lpszStyleId, int nWidth, int nHeight, bool bDoModal, LPCTSTR lpszId)
{
	int scrWidth, scrHeight;  
	RECT rect;  

	scrWidth = GetSystemMetrics(SM_CXSCREEN);  
	scrHeight = GetSystemMetrics(SM_CYSCREEN);  
	GetWindowRect(this->GetHWND(), &rect);  
	SetWindowPos(this->GetHWND(), HWND_NOTOPMOST, (scrWidth - (rect.right - rect.left)) / 2, (scrHeight - (rect.bottom - rect.top)) / 2, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW);  

	m_pMessageTitle->SetText(lpszCaption);
	m_pMessageTitle->SetToolTip(lpszCaption);
	CenterWindow();

	if(hParent!=NULL)
	{
		RECT rtMain;
		GetWindowRect(hParent, &rtMain);
		m_WndShadow.SetMaskSize(rtMain);
		m_WndShadow.Create(m_hWnd);
	}

	//SetForegroundWindow(m_hWnd);
	tstring strConfinFile = GetLocalPath();
	strConfinFile += _T("\\Setting\\Config.ini");

	TCHAR szIsCheck[MAX_PATH] = {0};
	GetPrivateProfileString(_T("config"), _T("UploadLast"), _T("true"), szIsCheck, MAX_PATH, strConfinFile.c_str());
	if ( _tcscmp(szIsCheck, _T("true")) == 0)
	{
		CCheckBoxUI* checkBoxUI = static_cast<CCheckBoxUI *>(m_PaintManager.FindControl(_T("AddSkyCheck")));
		checkBoxUI->SetCheck(true);
	}
	else
	{
		CCheckBoxUI* checkBoxUI = static_cast<CCheckBoxUI *>(m_PaintManager.FindControl(_T("AddSkyCheck")));
		checkBoxUI->SetCheck(false);
	}

	{
		CControlUI* pInfoControl = GetItem(_T("content"));
		CLabelUIEx* pInfo = static_cast<CLabelUIEx*>(pInfoControl);
		if (pInfo)
		{
			//pInfo->SetAttribute(_T("EnableMouse"),L"1");
			pInfo->SetFixedHeight(78);
			pInfo->SetVisible(true);
			pInfo->SetText(lpszText);
			pInfo->SetToolTip(lpszText);
			UINT	uSytle = pInfo->GetTextStyle();
			uSytle |= DT_WORDBREAK;
			uSytle &= (~DT_CENTER);
			pInfo->SetTextStyle(uSytle);

		}

		CLabelUIEx* pContent1 = dynamic_cast<CLabelUIEx*>(GetItem(_T("content1")));
		pContent1->SetVisible(false);

		CControlUI* pIcon = GetItem(_T("type"));
		if (pIcon)
		{
			SetState(enIconType);
			pIcon->Invalidate();
		}
	}

	// 分解按钮名字
	if (lpszBtnType != NULL)
	{
		int index = 0;
		TCHAR szBtnId[20];
		tstring strBtnType = lpszBtnType;
		TCHAR c = _T(',');
		int i = strBtnType.find_first_of(c);
		if (i <= 0)
		{
			c = _T('|');
			i = strBtnType.find_first_of(c);
		}
		while (i > 0)
		{
			_stprintf(szBtnId, _T("msgboxbtn%d"), index + 1);

			GetItem(szBtnId)->SetVisible(true);
			GetItem(szBtnId)->SetText(strBtnType.substr(0, i).c_str());
			strBtnType = strBtnType.substr(i + 1);
			i = strBtnType.find_first_of(c);
			index++;
		}
		if (strBtnType != _T(""))
		{
			_stprintf(szBtnId, _T("msgboxbtn%d"), index + 1);
			GetItem(szBtnId)->SetVisible(true);
			GetItem(szBtnId)->SetText(strBtnType.substr(0, i).c_str());
		}
	}

	if (m_nDefaultButtonIndex != 0)
	{
		TCHAR szBtnId[20];
		_stprintf(szBtnId, _T("msgboxbtn%d"), m_nDefaultButtonIndex);

		m_nDefaultButtonIndex = nDefaultButtonIndex;
		//SetAttribute(_T("ok"), szBtnId);
	}

	if (bDoModal)
	{
		return ShowModal();
	}
	ShowWindow(true);

	return 0;
}

UINT CMessageBoxUIEx2::MessageBox( HWND hParent, LPCTSTR lpszMainText, DWORD dwMainTextColor, int nMainTextFont, LPCTSTR lpszText, DWORD dwTextColor, int nTextFont, LPCTSTR lpszCaption, LPCTSTR lpszBtnType, ENMessageBoxTypeEx2 enIconType /*= enMessageBoxTypeInfo*/, UINT nDefaultButtonIndex /*= 0*/, LPCTSTR lpszStyleId /*= NULL*/, int nWidth /*= 340*/, int nHeight /*= 150*/,bool bDoModal /*= true */,LPCTSTR lpszId /*= NULL */ )
{
	int scrWidth, scrHeight;  
	RECT rect;  

	scrWidth = GetSystemMetrics(SM_CXSCREEN);  
	scrHeight = GetSystemMetrics(SM_CYSCREEN);  
	GetWindowRect(this->GetHWND(), &rect);  
	SetWindowPos(this->GetHWND(), HWND_NOTOPMOST, (scrWidth - (rect.right - rect.left)) / 2, (scrHeight - (rect.bottom - rect.top)) / 2, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW);  

	m_pMessageTitle->SetText(lpszCaption);
	m_pMessageTitle->SetToolTip(lpszCaption);
	CenterWindow();

	if(hParent!=NULL)
	{
		RECT rtMain;
		GetWindowRect(hParent, &rtMain);
		m_WndShadow.SetMaskSize(rtMain);
		m_WndShadow.Create(m_hWnd);
	}

	//SetForegroundWindow(m_hWnd);
	tstring strConfinFile = GetLocalPath();
	strConfinFile += _T("\\Setting\\Config.ini");

	TCHAR szIsCheck[MAX_PATH];
	GetPrivateProfileString(_T("config"), _T("UploadLast"), _T("true"), szIsCheck, MAX_PATH, strConfinFile.c_str());
	if (_tcscmp(szIsCheck, _T("true")) == 0)
	{
		CCheckBoxUI* checkBoxUI = static_cast<CCheckBoxUI *>(m_PaintManager.FindControl(_T("AddSkyCheck")));
		checkBoxUI->SetCheck(true);
	}
	else
	{
		CCheckBoxUI* checkBoxUI = static_cast<CCheckBoxUI *>(m_PaintManager.FindControl(_T("AddSkyCheck")));
		checkBoxUI->SetCheck(false);
	}

	{
		CLabelUIEx* pContent = dynamic_cast<CLabelUIEx*>(GetItem(_T("content")));
		if (pContent)
		{
			//pInfo->SetAttribute(_T("EnableMouse"),L"1");
			pContent->SetFixedHeight(30);
			pContent->SetVisible(true);
			pContent->SetText(lpszMainText);
			pContent->SetToolTip(lpszMainText);
			pContent->SetTextColor(dwMainTextColor);
			pContent->SetFont(nMainTextFont);
			UINT	uSytle = pContent->GetTextStyle();
			uSytle |= DT_WORDBREAK;
			uSytle &= (~DT_CENTER);
			pContent->SetTextStyle(uSytle);

		}

		CLabelUIEx* pContent1 = dynamic_cast<CLabelUIEx*>(GetItem(_T("content1")));
		if (pContent1)
		{;
		pContent1->SetFixedHeight(48);
		pContent1->SetVisible(true);
		pContent1->SetText(lpszText);
		pContent1->SetToolTip(lpszText);
		pContent1->SetTextColor(dwTextColor);
		pContent1->SetFont(nTextFont);
		UINT	uSytle = pContent1->GetTextStyle();
		uSytle |= DT_WORDBREAK;
		uSytle &= (~DT_CENTER);
		pContent1->SetTextStyle(uSytle);

		}

		CControlUI* pIcon = GetItem(_T("type"));
		if (pIcon)
		{
			SetState(enIconType);
			pIcon->Invalidate();
		}
	}

	// 分解按钮名字
	if (lpszBtnType != NULL)
	{
		int index = 0;
		TCHAR szBtnId[20];
		tstring strBtnType = lpszBtnType;
		TCHAR c = _T(',');
		int i = strBtnType.find_first_of(c);
		if (i <= 0)
		{
			c = _T('|');
			i = strBtnType.find_first_of(c);
		}
		while (i > 0)
		{
			_stprintf(szBtnId, _T("msgboxbtn%d"), index + 1);

			GetItem(szBtnId)->SetVisible(true);
			GetItem(szBtnId)->SetText(strBtnType.substr(0, i).c_str());
			strBtnType = strBtnType.substr(i + 1);
			i = strBtnType.find_first_of(c);
			index++;
		}
		if (strBtnType != _T(""))
		{
			_stprintf(szBtnId, _T("msgboxbtn%d"), index + 1);
			GetItem(szBtnId)->SetVisible(true);
			GetItem(szBtnId)->SetText(strBtnType.substr(0, i).c_str());
		}
	}

	if (m_nDefaultButtonIndex != 0)
	{
		TCHAR szBtnId[20];
		_stprintf(szBtnId, _T("msgboxbtn%d"), m_nDefaultButtonIndex);

		m_nDefaultButtonIndex = nDefaultButtonIndex;
		//SetAttribute(_T("ok"), szBtnId);
	}

	if (bDoModal)
	{
		return ShowModal();
	}
	ShowWindow(true);

	return 0;
}



void CMessageBoxUIEx2::Notify(TNotifyUI& msg)
{
	if( msg.sType == _T("click") )
	{	
		tstring strItem = msg.pSender->GetName();
		if (strItem.length() >= 9 && strItem.substr(0, 9) == _T("msgboxbtn"))
		{
			if (!IsWindow(GetHWND())) return;
			m_nDefaultButtonIndex	= ID_MSGBOX_BTN + _ttoi(strItem.substr(_tcslen(_T("msgboxbtn"))).c_str()) - 1;

			HWND hPaintWnd = m_PaintManager.GetPaintWindow();
			if (hPaintWnd
				&& IsWindowVisible(hPaintWnd))
			{
				::SendMessage(hPaintWnd, WM_MOUSELEAVE, 0, (LPARAM)MAKELONG(0, 0));
			}
			
			if (m_nDefaultButtonIndex == ID_MSGBOX_BTN || m_nDefaultButtonIndex == (ID_MSGBOX_BTN + 1))
			{
				CCheckBoxUI* checkBoxUI = static_cast<CCheckBoxUI *>(m_PaintManager.FindControl(_T("AddSkyCheck")));
				if (checkBoxUI->IsSelected())
				{
					m_nDefaultButtonIndex += 3;

					tstring strConfinFile = GetLocalPath();
					strConfinFile += _T("\\Setting\\Config.ini");
					WritePrivateProfileString(_T("config"), _T("UploadLast"), _T("true"), strConfinFile.c_str());
				}
				else
				{
					tstring strConfinFile = GetLocalPath();
					strConfinFile += _T("\\Setting\\Config.ini");
					WritePrivateProfileString(_T("config"), _T("UploadLast"), _T("false"), strConfinFile.c_str());
				}
			}
			Close(m_nDefaultButtonIndex);
		}
	}
}

LRESULT CMessageBoxUIEx2::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( uMsg == WM_SYSKEYDOWN && wParam == VK_F4 )
	{
		HWND hPaintWnd = m_PaintManager.GetPaintWindow();
		if (hPaintWnd
			&& IsWindowVisible(hPaintWnd))
		{
			::SendMessage(hPaintWnd, WM_MOUSELEAVE, 0, (LPARAM)MAKELONG(0, 0));
		}
		Close(m_nDefaultButtonIndex);

		bHandled = TRUE;
	}

	return 1;
}


UINT  UIMessageBoxEx2(HWND hParent, LPCTSTR lpszText, LPCTSTR lpszCaption, LPCTSTR lpszBtnType, CMessageBoxUIEx2::ENMessageBoxTypeEx2 enIconType/* = CMessageBoxUI::enMessageBoxTypeInfo*/, UINT nDefaultButtonIndex, LPCTSTR lpszStyleId /*= NULL*/, int nWidth/* = 340*/, int nHeight/* = 150*/,bool bDoModal/* = true */,LPCTSTR lpszId /* =NULL */)
{

	CMessageBoxUIEx2 *pMsgBox = MessageBoxUIEx2::GetInstance();

	if ( pMsgBox == NULL )
		return false;

	if(hParent == NULL)
	{
		if(AfxGetApp() && AfxGetApp()->m_pMainWnd)
			hParent = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
	}

	if(pMsgBox->GetHWND() == NULL)
		pMsgBox->Create(hParent, _T("MessageBox"), WS_POPUP, 0, 0,0,0);

	if ( ::IsWindowVisible(pMsgBox->GetHWND()) )
		return -1;

	CRect rect;
	::GetWindowRect(hParent,&rect);
	//MoveWindow(pMsgBox->GetHWND(), rect.left, rect.top, 430, 250, TRUE);
	MoveWindow(pMsgBox->GetHWND(), rect.left, rect.top, 420, 302, TRUE);

	UINT nResult = pMsgBox->MessageBox(hParent, lpszText, lpszCaption, lpszBtnType, enIconType, nDefaultButtonIndex,  lpszStyleId, nWidth, nHeight,bDoModal,lpszId);
	//ShowWindow(pMsgBox->GetHWND(), SW_SHOW);

	return nResult;
}

UINT UIMessageBoxEx2( HWND hParent, LPCTSTR lpszMainText, DWORD dwMainTextColor, int nMainTextFont, LPCTSTR lpszText, DWORD dwTextColor, int nTextFont, LPCTSTR lpszCaption, LPCTSTR lpszBtnType, CMessageBoxUIEx2::ENMessageBoxTypeEx2 enIconType /*= CMessageBoxUI::enMessageBoxTypeInfo*/, UINT nDefaultButtonIndex /*= 0*/, LPCTSTR lpszStyleId /*= NULL*/, int nWidth /*= 340*/, int nHeight /*= 150*/,bool bDoModal /*= true */,LPCTSTR lpszId /*= NULL */ )
{

	CMessageBoxUIEx2 *pMsgBox = MessageBoxUIEx2::GetInstance();

	if ( pMsgBox == NULL )
		return false;

	if(hParent == NULL)
	{
		if(AfxGetApp() && AfxGetApp()->m_pMainWnd)
			hParent = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
	}
	if(pMsgBox->GetHWND() == NULL)
		pMsgBox->Create(hParent, _T("MessageBox"), WS_POPUP, 0, 0,0,0);

	if ( ::IsWindowVisible(pMsgBox->GetHWND()) )
		return -1;

	CRect rect;
	::GetWindowRect(hParent,&rect);
	//MoveWindow(pMsgBox->GetHWND(), rect.left, rect.top, 430, 250, TRUE);
	MoveWindow(pMsgBox->GetHWND(), rect.left, rect.top, 420, 302, TRUE);

	UINT nResult = pMsgBox->MessageBox(hParent, lpszMainText, dwMainTextColor, nMainTextFont, lpszText, dwTextColor, nTextFont, lpszCaption, lpszBtnType, enIconType, nDefaultButtonIndex,  lpszStyleId, nWidth, nHeight,bDoModal,lpszId);
	//ShowWindow(pMsgBox->GetHWND(), SW_SHOW);

	return nResult;
}

