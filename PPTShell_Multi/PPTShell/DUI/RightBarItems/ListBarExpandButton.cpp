#include "stdafx.h"
#include "ListBarExpandButton.h"

CListBarExpandButton::CListBarExpandButton()
{
	m_bExpand		= false;
	m_bClickDown	= false;
	m_dwNormalColor = 0;
	m_dwExpandColor = 0;
	m_strNormalIcon = _T("");
	m_strHotIcon	= _T("");
	m_strExpandIcon = _T("");
	m_strTipIcon	= _T("");
	m_rcTipPos		= CRect(55, 10, 34, 17);
}

CListBarExpandButton::~CListBarExpandButton()
{

}

void CListBarExpandButton::DoInit()
{
	__super::DoInit();
}

void CListBarExpandButton::Expand( bool bExpand /*= true*/ )
{
	m_bExpand = bExpand;

	SetTextColor(bExpand ? m_dwExpandColor : m_dwNormalColor);
}

void CListBarExpandButton::PaintStatusImage( HDC hDC )
{
	CButtonUI::PaintStatusImage(hDC);

	TDrawInfo di;
	di.rcDestOffset.left	= (m_rcItem.right - m_nLogoWidth)/2;
	di.rcDestOffset.top		= m_rcLogoPadding.top;
	di.rcDestOffset.right	= di.rcDestOffset.left + m_nLogoWidth;
	di.rcDestOffset.bottom	= di.rcDestOffset.top + m_nLogoHeight;

	if ( (m_uButtonState & UISTATE_HOT) != 0 && !m_strHotIcon.IsEmpty() )
		di.sDrawString = m_strHotIcon;
	else if (!m_bExpand && !m_strNormalIcon.IsEmpty())
		di.sDrawString = m_strNormalIcon;
	else if (!m_strExpandIcon.IsEmpty())
		di.sDrawString = m_strExpandIcon;

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

void CListBarExpandButton::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
{
	if( _tcscmp(pstrName, _T("normalicon")) == 0 )
	{
		m_strNormalIcon = pstrValue;
	}
	else if( _tcscmp(pstrName, _T("hoticon")) == 0 )
	{
		m_strHotIcon	= pstrValue;
	}
	else if( _tcscmp(pstrName, _T("expandicon")) == 0 )
	{
		m_strExpandIcon	= pstrValue;
	}
	else if( _tcscmp(pstrName, _T("tipicon")) == 0 )
	{
		m_strTipIcon	= pstrValue;
	}
	else if(_tcscmp(pstrName, _T("logowidth")) == 0 )
	{
		LPTSTR pstr		= NULL;
		m_nLogoWidth	= _tcstol(pstrValue, &pstr, 10);    
	}
	else if(_tcscmp(pstrName, _T("logoheight")) == 0 )
	{
		LPTSTR pstr		= NULL;
		m_nLogoHeight	= _tcstol(pstrValue, &pstr, 10);    
	}
	else if(_tcscmp(pstrName, _T("logopadding")) == 0 )
	{
		LPTSTR pstr = NULL;
		m_rcLogoPadding.left	= _tcstol(pstrValue, &pstr, 10);	ASSERT(pstr);    
		m_rcLogoPadding.top		= _tcstol(pstr + 1, &pstr, 10);		ASSERT(pstr);    
		m_rcLogoPadding.right	= _tcstol(pstr + 1, &pstr, 10);		ASSERT(pstr);    
		m_rcLogoPadding.bottom	= _tcstol(pstr + 1, &pstr, 10);		ASSERT(pstr);     
	}
	else if(_tcscmp(pstrName, _T("tipPos")) == 0 )
	{
		LPTSTR pstr = NULL;
		m_rcTipPos.left	= _tcstol(pstrValue, &pstr, 10);	ASSERT(pstr);    
		m_rcTipPos.top		= _tcstol(pstr + 1, &pstr, 10);		ASSERT(pstr);    
		m_rcTipPos.right	= _tcstol(pstr + 1, &pstr, 10);		ASSERT(pstr);    
		m_rcTipPos.bottom	= _tcstol(pstr + 1, &pstr, 10);		ASSERT(pstr);     
	}
	else if(_tcscmp(pstrName, _T("textcolor")) == 0 )
	{
		if( *pstrValue	== _T('#')) 
			pstrValue = ::CharNext(pstrValue);

		LPTSTR pstr		= NULL;
		m_dwNormalColor	= _tcstoul(pstrValue, &pstr, 16);
		m_dwExpandColor = m_dwNormalColor;
	}
	else if(_tcscmp(pstrName, _T("expandtextcolor")) == 0 )
	{
		if( *pstrValue	== _T('#')) 
			pstrValue = ::CharNext(pstrValue);

		LPTSTR pstr		= NULL;
		m_dwExpandColor	= _tcstoul(pstrValue, &pstr, 16);  
	}
	CButtonUI::SetAttribute(pstrName, pstrValue);
}

void CListBarExpandButton::AddToVecControls( CControlUI* pControl )
{
	m_vecControls.push_back(pControl);
}

bool CListBarExpandButton::GetExpandStaus()
{
	return m_bExpand;
}
