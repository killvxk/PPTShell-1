#include "stdafx.h"
#include "ListBarSearchButton.h"
#include "DUI/GroupExplorer.h"

CListBarSearchButton::CListBarSearchButton()
{
	m_bSelected = false;
}

CListBarSearchButton::~CListBarSearchButton()
{

}

void CListBarSearchButton::DoInit()
{
	__super::DoInit();
	m_nType = SearchALLRes;
}

void CListBarSearchButton::Selected( bool bSelected )
{
	m_bSelected = bSelected;

	SetTextColor(m_bSelected ? m_dwSelectedTextColor : m_dwNormalColor);
}

bool CListBarSearchButton::GetSelected()
{
	return m_bSelected;
}

void CListBarSearchButton::DoClick( TNotifyUI* pNotify )
{
	__super::DoClick(pNotify);
	Selected(true);
}

void CListBarSearchButton::PaintStatusImage( HDC hDC )
{
	CButtonUI::PaintStatusImage(hDC);

	TDrawInfo di;
	di.rcDestOffset.left	= (m_rcItem.right - m_nLogoWidth)/2;
	di.rcDestOffset.top		= m_rcLogoPadding.top;
	di.rcDestOffset.right	= di.rcDestOffset.left + m_nLogoWidth;
	di.rcDestOffset.bottom	= di.rcDestOffset.top + m_nLogoHeight;

	if ( (m_uButtonState & UISTATE_HOT) != 0 && !m_strHotIcon.IsEmpty() )
		di.sDrawString = m_strHotIcon;
	else if (!m_bSelected && !m_strNormalIcon.IsEmpty())
		di.sDrawString = m_strNormalIcon;
	else if (!m_strSelectedIcon.IsEmpty())
		di.sDrawString = m_strSelectedIcon;

	DrawImage(hDC, di);

	if ( m_strTipIcon.IsEmpty() )
		return;

	TDrawInfo diTip;
	diTip.rcDestOffset.left		= m_rcItem.left + m_rcTipPos.left;
	diTip.rcDestOffset.top		= m_rcTipPos.top;
	diTip.rcDestOffset.right	= diTip.rcDestOffset.left + m_rcTipPos.right;
	diTip.rcDestOffset.bottom	= diTip.rcDestOffset.top + m_rcTipPos.bottom;
	diTip.sDrawString			= m_strTipIcon;
	DrawImage(hDC, diTip);
}

void CListBarSearchButton::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
{
	if(_tcscmp(pstrName, _T("selectedtextcolor")) == 0 )
	{
		if( *pstrValue	== _T('#')) 
			pstrValue = ::CharNext(pstrValue);

		LPTSTR pstr		= NULL;
		m_dwSelectedTextColor= _tcstoul(pstrValue, &pstr, 16);  
	}
	else if(_tcscmp(pstrName, _T("selectedicon")) == 0 )
	{
		m_strSelectedIcon = pstrValue; 
	}
	__super::SetAttribute(pstrName, pstrValue);
}

bool CListBarSearchButton::OnBtnCloseResponse( void * pObj )
{
	if(m_bSelected)
		Selected(false);

	return true;
}

