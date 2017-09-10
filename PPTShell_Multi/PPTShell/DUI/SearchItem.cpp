#include "StdAfx.h"
#include "SearchItem.h"

const TCHAR* const kSearchItemUIClassName = _T("SearchItemUI");
const TCHAR* const kSearchItemUIInterfaceName = _T("SearchItem");

CSearchItemUI::CSearchItemUI(void)
{
}

CSearchItemUI::~CSearchItemUI(void)
{
}


LPCTSTR CSearchItemUI::GetClass() const
{
	return kSearchItemUIClassName;
}

void CSearchItemUI::ChangeLayoutWidth(bool bLeave)
{
	CHorizontalLayoutUI* pHorLayout = static_cast<CHorizontalLayoutUI*>(FindSubControl(_T("btn_seacher")));  

	if ( pHorLayout != NULL )
	{
		RECT rt = {5, 5, 15, 0};
		if (bLeave && !IsSelected())
			rt.right = 5;

		pHorLayout->SetPadding(rt);

		__super::Invalidate();
	}

	if ( m_pBtn != NULL )
	{
		RECT rt = {10, 0, 0, 0};
		if (bLeave && !IsSelected())
			rt.left = 20;

		m_pBtn->SetTextPadding(rt);

		__super::Invalidate();
	}
}

void CSearchItemUI::DoEvent( TEventUI& event )
{
	//ChangeLayoutWidth(event.Type == UIEVENT_MOUSELEAVE);

	CListItemUI::DoEvent(event);
}

bool CSearchItemUI::Select(bool bSelect /* = true */)
{
	bool bRet = CListItemUI::Select(bSelect);

	ChangeLayoutWidth(!bSelect);

	return bRet;
}

void CSearchItemUI::DoInit()
{
	CListItemUI::DoInit();
}
