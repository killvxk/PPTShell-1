#include "StdAfx.h"
#include "MessageBoxUIEx.h"

CMessageBoxUIEx::CMessageBoxUIEx(void)
{
}

CMessageBoxUIEx::~CMessageBoxUIEx(void)
{
}

LPCTSTR CMessageBoxUIEx::GetWindowClassName( void ) const
{
	return _T("MessageBoxEx");
}

DuiLib::CDuiString CMessageBoxUIEx::GetSkinFile()
{
	return _T("MessageBox\\MessageBoxEx.xml");
}

DuiLib::CDuiString CMessageBoxUIEx::GetSkinFolder()
{
	return _T("skins");
}

void CMessageBoxUIEx::InitWindow()
{
	m_pVerticalLayout = static_cast<CVerticalLayoutUI *>(m_PaintManager.FindControl(_T("MessageBox")));	
	if (NULL == m_pVerticalLayout) return;
	//////////////////////////////////////////////////////////////////////////
	m_pLayoutHeight = static_cast<CVerticalLayoutUI *>(m_pVerticalLayout->FindSubControl(_T("MessageBoxHeight")));	
	m_pLayoutCenterHeight = static_cast<CVerticalLayoutUI *>(m_pVerticalLayout->FindSubControl(_T("MessageBoxCenterHeight")));
	//////////////////////////////////////////////////////////////////////////2015.11.04 cws 控制消息框的高度和消息内容高度
	m_pMessageTitle = static_cast<CLabelUI*>(m_pVerticalLayout->FindSubControl(_T("title")));
	m_pMessageContent = static_cast<CLabelUIEx*>(m_pVerticalLayout->FindSubControl(_T("content")));
	m_pMessageType = m_pVerticalLayout->FindSubControl(_T("type"));	
}


CControlUI * CMessageBoxUIEx::GetItem(LPCTSTR lpszName)
{
	return static_cast<CControlUI*>(m_pVerticalLayout->FindSubControl(lpszName));
}

void CMessageBoxUIEx::SetState(ENMessageBoxTypeEx eMessageType)
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
		//////////////////////////////////////////////////////////////////////////
	case enMessageBoxTypeCenterPic:
		{
			//
			CRect lrect;    
			GetWindowRect(this->GetHWND(), &lrect); 
			lrect.left = (lrect.Width()  - 81)/ 2;
			lrect.top = 30;
			lrect.bottom = 81+30;
			lrect.right = lrect.left + 81;

			pType->SetPos(lrect);
			pType->SetBkImage(_T("file=\'MessageBox\\center_pic.png'"));
		}
		break;
		//////////////////////////////////////////////////////////////////////////2015.11.03 cws
	default:
		break;
	}	
	
}


UINT CMessageBoxUIEx::MessageBox(HWND hParent, LPCTSTR lpszText, LPCTSTR lpszCaption,  LPCTSTR lpszBtnType, ENMessageBoxTypeEx enIconType , UINT nDefaultButtonIndex , LPCTSTR lpszStyleId, int nWidth, int nHeight, bool bDoModal, LPCTSTR lpszId)
{
	if (enIconType == CMessageBoxUIEx::enMessageBoxTypeCenterPic)
	{
		m_pLayoutHeight->SetFloat(true);
		m_pLayoutHeight->SetFixedHeight(300);
		m_pLayoutHeight->SetFloat(false);

		m_pLayoutCenterHeight->SetFloat(true);
		m_pLayoutCenterHeight->SetFixedHeight(138);
		m_pLayoutCenterHeight->SetFloat(false); 
	}
	else
	{
		m_pLayoutHeight->SetFloat(true);
		m_pLayoutHeight->SetFixedHeight(250);
		m_pLayoutHeight->SetFloat(false);

		m_pLayoutCenterHeight->SetFloat(true);
		m_pLayoutCenterHeight->SetFixedHeight(78);
		m_pLayoutCenterHeight->SetFloat(false);
	}
	int scrWidth, scrHeight;  
	RECT rect;  

	scrWidth = GetSystemMetrics(SM_CXSCREEN);  
	scrHeight = GetSystemMetrics(SM_CYSCREEN);  
	GetWindowRect(this->GetHWND(), &rect);  	
	SetWindowPos(this->GetHWND(), HWND_DESKTOP, (scrWidth - (rect.right - rect.left)) / 2, (scrHeight - (rect.bottom - rect.top)) / 2, rect.right - rect.left, rect.bottom - rect.top + 200, SWP_SHOWWINDOW);   
	
	m_pMessageTitle->SetText(lpszCaption);
	m_pMessageTitle->SetToolTip(lpszCaption);
	CenterWindow();

	RECT rtMain;
	GetWindowRect(hParent, &rtMain);
	m_WndShadow.SetMaskSize(rtMain);
	m_WndShadow.Create(m_hWnd);

	{
 		CControlUI* pInfoControl = GetItem(_T("content"));
		CLabelUIEx* pInfo = static_cast<CLabelUIEx*>(pInfoControl);
 		if (pInfo)
 		{
			//////////////////////////////////////////////////////////////////////////2015.11.03  cws
 			//pInfo->SetAttribute(_T("EnableMouse"),L"1");

			pInfo->SetFixedHeight(78);
			pInfo->SetFixedWidth(270);
 			pInfo->SetVisible(true);
 			pInfo->SetText(lpszText);
 			pInfo->SetToolTip(lpszText);
			UINT	uSytle = pInfo->GetTextStyle();
			uSytle |= DT_WORDBREAK;
			uSytle &= (~DT_CENTER);
			pInfo->SetTextStyle(uSytle);
			//////////////////////////////////////////////////////////////////////////
			if (enIconType == enMessageBoxTypeCenterPic)//图像居中，文字在下
			{
				CRect lrect;    
				GetWindowRect(this->GetHWND(), &lrect); 
				lrect.left = 90;
				lrect.top = 81 + 35;
				pInfo->SetFloat(true);
				pInfo->SetPos(lrect);
				pInfo->Invalidate();
			} 	  
			//////////////////////////////////////////////////////////////////////////2015.11.03  cws
		
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
		m_nDefaultButtonIndex = nDefaultButtonIndex;//2015.11.04 cws赋值要先
		_stprintf(szBtnId, _T("msgboxbtn%d"), m_nDefaultButtonIndex);

		//SetAttribute(_T("ok"), szBtnId);
	}



	//2015.11.03 cws
	if (enIconType == enMessageBoxTypeCenterPic)//图像居中，文字在下
	{
		TCHAR szBtnId[20];
		_stprintf(szBtnId, _T("msgboxbtn1") );
		((CButtonUI*)GetItem(szBtnId))->SetNormalImage(_T("file=\'MessageBox\\btn_centerpic_default.png'")); 
		((CButtonUI*)GetItem(szBtnId))->SetHotImage(_T("file=\'MessageBox\\btn_centerpic_hover.png'")); 
		((CButtonUI*)GetItem(szBtnId))->SetTextColor(RGB(136,136,136));
		//CRect lrect;
		//lrect.left = 70;
		//lrect.right = 190;
		//lrect.top = 30;
		//lrect.bottom = 65;
		//((CButtonUI*)GetItem(szBtnId))->SetFloat(true);
		//((CButtonUI*)GetItem(szBtnId))->SetPos(lrect);
		//((CButtonUI*)GetItem(szBtnId))->SetFloat(false);

		_stprintf(szBtnId, _T("msgboxbtn2") ); 
		((CButtonUI*)GetItem(szBtnId))->SetNormalImage(_T("file=\'MessageBox\\btn_centerpic_default.png'")); 
		((CButtonUI*)GetItem(szBtnId))->SetHotImage(_T("file=\'MessageBox\\btn_centerpic_hover.png'")); 
		((CButtonUI*)GetItem(szBtnId))->SetTextColor(RGB(136,136,136));
		//lrect.left = 228;
		//lrect.right = 348; 
		//((CButtonUI*)GetItem(szBtnId))->SetFloat(true);
		//((CButtonUI*)GetItem(szBtnId))->SetPos(lrect);
		//((CButtonUI*)GetItem(szBtnId))->SetFloat(false);

	}

	
	if (bDoModal)
	{  
		return ShowModal();
	}
	ShowWindow(true);

	return 0;
}



void CMessageBoxUIEx::Notify(TNotifyUI& msg)
{
	if( msg.sType == _T("click") )
	{	
		tstring strItem = msg.pSender->GetName();
		if (strItem.length() >= 9 && strItem.substr(0, 9) == _T("msgboxbtn"))
		{
			if (!IsWindow(GetHWND())) return;
			m_nDefaultButtonIndex	= ID_MSGBOX_BTN + _ttoi(strItem.substr(_tcslen(_T("msgboxbtn"))).c_str()) - 1;
			Close(m_nDefaultButtonIndex);
		}
	}
}

LRESULT CMessageBoxUIEx::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( uMsg == WM_SYSKEYDOWN && wParam == VK_F4 )
	{
		Close(m_nDefaultButtonIndex);
		bHandled = TRUE;
	}

	return 1;
}

UINT  UIMessageBoxEx(HWND hParent, LPCTSTR lpszText, LPCTSTR lpszCaption, LPCTSTR lpszBtnType, CMessageBoxUIEx::ENMessageBoxTypeEx enIconType/* = CMessageBoxUI::enMessageBoxTypeInfo*/, UINT nDefaultButtonIndex, LPCTSTR lpszStyleId /*= NULL*/, int nWidth/* = 340*/, int nHeight/* = 150*/,bool bDoModal/* = true */,LPCTSTR lpszId /* =NULL */)
{
	CRect rect;
	::GetWindowRect(hParent,&rect);
	CMessageBoxUIEx *pMsgBox = new CMessageBoxUIEx;
	pMsgBox->Create(hParent, _T("MessageBoxEx"), WS_VISIBLE, 0, 0,0,0); 
	MoveWindow(pMsgBox->GetHWND(), rect.left, rect.top, 430, 250, TRUE); 
	
	UINT nResult = pMsgBox->MessageBox(hParent, lpszText, lpszCaption, lpszBtnType, enIconType, nDefaultButtonIndex,  lpszStyleId, nWidth, nHeight,bDoModal,lpszId);
	
	
	if (bDoModal)
		delete pMsgBox;

	return nResult;

}