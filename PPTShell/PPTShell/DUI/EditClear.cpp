#include "stdafx.h"
#include "EditClear.h"

CEditClearUI::CEditClearUI()
{
	m_pClearButton = new CButtonUI;
	m_bClearStatus = true;

	m_nType = 0;
}

CEditClearUI::~CEditClearUI()
{

}

LPCTSTR CEditClearUI::GetClass() const
{
	return _T("EditClearUI");
}

LPVOID	CEditClearUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcsicmp(pstrName, _T("EditClear")) == 0 ) return static_cast<CEditClearUI*>(this);
	return CEditUI::GetInterface(pstrName);
}

void CEditClearUI::DoInit()
{
	SetText(GetTipText().c_str());
	SetTextColor(GetTipTextColor());
}

void CEditClearUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if( _tcscmp(pstrName, _T("clear")) == 0 ) m_bClear = true;
	else if( _tcscmp(pstrName, _T("clearimage")) == 0 )
		m_clearImagePath = pstrValue;
	else if( _tcscmp(pstrName, _T("type")) == 0)
	{
		m_nType = _ttoi(pstrValue);
	}

	CEditUI::SetAttribute(pstrName,pstrValue);
}

void CEditClearUI::SetText(LPCTSTR pstrText)
{
	CEditUI::SetText(pstrText);
}
/*
LRESULT CEditClearUI::OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(m_bClear)
	{
		tstring str =  GetText();
		if(str.length() > 0 && str != m_tiptext)
		{
			//»­Çå³ý°´Å¥
			CControlUI * pControl = static_cast<CControlUI *>(((CContainerUI *)GetParent())->FindSubControl(CLEAR_BUTTON_NAME));
			if(pControl)
			{
				if(!pControl->IsVisible())
					pControl->SetVisible(true);
			}
			else
			{

				m_pClearButton->SetName(CLEAR_BUTTON_NAME);

				((CContainerUI *)GetParent())->Add(m_pClearButton);

				RECT rect = m_rcItem;
				rect.right = rect.right - rect.left;
				rect.left = rect.right - 24;
				rect.right = rect.left+16;
				rect.top = 9;
				rect.bottom = 25;
				m_pClearButton->SetFloat(true);
				m_pClearButton->SetFixedWidth(16);
				m_pClearButton->SetFixedHeight(16);
				m_pClearButton->SetPos(rect);
				m_pClearButton->SetBkImage(m_clearImagePath.c_str());


			}

			//
		}
		else
		{
			//É¾µôÇå³ý°´Å¥
			CControlUI * pControl = static_cast<CControlUI *>(((CContainerUI *)GetParent())->FindSubControl(CLEAR_BUTTON_NAME));
			if(pControl)
			{
				if(pControl->IsVisible())
					pControl->SetVisible(false);
			}
			//
		}
	}
}
*/
void CEditClearUI::DoEvent(TEventUI& event)
{
	if( event.Type == UIEVENT_SETFOCUS )
	{
		tstring strText = GetText();
		if(strText == GetTipText() || strText == _T(""))
		{
			SetTextColor(GetTextColor());
			SetText(_T(""));
		}
	}
	else if( event.Type == UIEVENT_KILLFOCUS )
	{
		tstring strText = GetText();
		if(strText == _T("") )
		{
			SetTextColor(GetTipTextColor());
			SetText(GetTipText().c_str());
		}
	}
	CEditUI::DoEvent(event);
}

void CEditClearUI::SetClearBtn()
{
	if(m_bClear)
	{
		tstring str =  GetText();
		if(str.length() > 0 && str != GetTipText())
		{
			//»­Çå³ý°´Å¥
			CControlUI * pControl = static_cast<CControlUI *>(((CContainerUI *)GetParent())->FindSubControl(CLEAR_BUTTON_NAME));
			if(pControl)
			{
				if(!pControl->IsVisible())
					pControl->SetVisible(true);
			}
			else
			{
				
				m_pClearButton->SetName(CLEAR_BUTTON_NAME);
				m_pClearButton->SetTag((UINT_PTR)this);
				((CContainerUI *)GetParent())->Add(m_pClearButton);

				

				//ÏÂ²ß
				RECT rect = m_rcItem;

				if(m_nType == 1)
				{
					rect.left = 300;
					rect.right = rect.left+16;
					rect.top = 11;
					rect.bottom = 27;
				}
				else if(m_nType == 2)
				{
					rect.left = 165;
					rect.right = rect.left+16;
					rect.top = 11;
					rect.bottom = 27;
				}
				else if(m_nType == 3)
				{
					rect.left = 245;
					rect.right = rect.left+16;
					rect.top = 10;
					rect.bottom = 26;
				}
				else
				{
					rect.right = rect.right - rect.left;
					rect.left = rect.right - 24;
					rect.right = rect.left+16;
					rect.top = 9;
					rect.bottom = 25;
					
					
				}
				m_pClearButton->SetFloat(true);
				m_pClearButton->SetFixedWidth(16);
				m_pClearButton->SetFixedHeight(16);
				m_pClearButton->SetPos(rect);
				m_pClearButton->SetBkImage(m_clearImagePath.c_str());
				//
				//			m_pClearButton->SetFloat(true);
				

			}
			SetTextColor(GetTextColor());
			//
		}
		else
		{
			//É¾µôÇå³ý°´Å¥
			CControlUI * pControl = static_cast<CControlUI *>(((CContainerUI *)GetParent())->FindSubControl(CLEAR_BUTTON_NAME));
			if(pControl)
			{
				if(pControl->IsVisible())
					pControl->SetVisible(false);
			}
			SetTextColor(GetTipTextColor());
			//
		}
	}
}

void CEditClearUI::HideClearBtn()
{
	CControlUI * pControl = static_cast<CControlUI *>(((CContainerUI *)GetParent())->FindSubControl(CLEAR_BUTTON_NAME));
	if(pControl)
	{
		if(pControl->IsVisible())
			pControl->SetVisible(false);
	}
}

void CEditClearUI::PaintText( HDC hDC )
{
	if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
	if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

	if( m_sText.IsEmpty() ) return;

	if( m_bPasswordMode ) {
		if(m_sText == m_sTipText)
		{
			RECT rc = m_rcItem;
			rc.left += m_rcTextPadding.left;
			rc.right -= m_rcTextPadding.right;
			rc.top += m_rcTextPadding.top;
			rc.bottom -= m_rcTextPadding.bottom;
			if( IsEnabled() ) {
				CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, m_dwTextColor, \
					m_iFont, DT_SINGLELINE | m_uTextStyle);
			}
			else {
				CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, m_dwDisabledTextColor, \
					m_iFont, DT_SINGLELINE | m_uTextStyle);

			}
			return;
		}	
	}

	__super::PaintText( hDC );
}

tstring CEditClearUI::GetTipText()
{
	return CEditUI::GetTipText().GetData();
}
