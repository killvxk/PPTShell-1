#include "stdafx.h"
#include "RichEditClear.h"

CRichEditClearUI::CRichEditClearUI()
{
	m_pClearButton = new CButtonUI;
}

CRichEditClearUI::~CRichEditClearUI()
{

}

LPCTSTR CRichEditClearUI::GetClass() const
{
	return _T("RichEditClearUI");
}

LPVOID	CRichEditClearUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcsicmp(pstrName, "RichEditClear") == 0 ) return static_cast<CRichEditClearUI*>(this);
	return CRichEditUI::GetInterface(pstrName);
}

void CRichEditClearUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if( _tcscmp(pstrName, _T("clear")) == 0 ) m_bClear = true;
	else if( _tcscmp(pstrName, _T("clearimage")) == 0 )
		m_clearImagePath = pstrValue;
	CRichEditUI::SetAttribute(pstrName,pstrValue);
}

void CRichEditClearUI::OnTxNotify(DWORD iNotify, void *pv)
{
	CRichEditUI::OnTxNotify(iNotify,pv);
	if(m_bClear)
	{
		CDuiString str =  GetText();
		if(str.GetLength() > 0 && str != GetTipText())
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
				m_pClearButton->SetBkImage("file='RightBar\\close.png'");


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