#include "StdAfx.h"
#include "ListButton.h"

namespace DuiLib
{

	// MenuUI
	const TCHAR* const kListButtonUIClassName		= _T("ListButtonUI");
	const TCHAR* const kListButtonUIInterfaceName	= _T("ListButton");

	CListButtonUI::CListButtonUI()
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

	CListButtonUI::~CListButtonUI()
	{}

	LPCTSTR CListButtonUI::GetClass() const
	{
		return kListButtonUIClassName;
	}

	LPVOID CListButtonUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, kListButtonUIInterfaceName) == 0 ) return static_cast<CListButtonUI*>(this);
		return CButtonUI::GetInterface(pstrName);
	}

	void CListButtonUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
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

	void CListButtonUI::PaintStatusImage(HDC hDC)
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

		//CDuiString strPath = this->GetManager()->GetResourcePath();
		//strPath += m_strTipIcon;

		//SIZE size = CRenderEngine::GetImageSize(strPath.GetData());

		//if ( size.cx == 0 || size.cy == 0 )
		//	return;

		TDrawInfo diTip;
		diTip.rcDestOffset.left		= m_rcItem.left + m_rcTipPos.left;
		diTip.rcDestOffset.top		= m_rcTipPos.top;
		diTip.rcDestOffset.right	= diTip.rcDestOffset.left + m_rcTipPos.right;
		diTip.rcDestOffset.bottom	= diTip.rcDestOffset.top + m_rcTipPos.bottom;
		diTip.sDrawString			= m_strTipIcon;
		DrawImage(hDC, diTip);
	}

	//void CListButtonUI::DoEvent( TEventUI& event )
	//{
	//	CButtonUI::DoEvent(event);

	//	if (event.Type == UIEVENT_BUTTONDOWN)
	//	{
	//		m_bClickDown = true;
	//	}
	//	else if (event.Type == UIEVENT_BUTTONUP && m_bClickDown)
	//	{
	//		m_bClickDown	= false;
	//		m_bExpand		= !m_bExpand;
	//	}
	//	else if ( event.Type == UIEVENT_MOUSELEAVE || event.Type == UIEVENT_MOUSEENTER)
	//	{
	//		m_bClickDown = false;
	//	}
	//}

	void CListButtonUI::Expand( bool bExpand /*= true*/ )
	{
		m_bExpand = bExpand;
		
		SetTextColor(bExpand ? m_dwExpandColor : m_dwNormalColor);
	}
}

