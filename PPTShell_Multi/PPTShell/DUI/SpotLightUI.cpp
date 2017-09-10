#include "stdafx.h"
#include "SpotLightUI.h"
#include "SpotLightWindow.h"

CSpotLightUI::CSpotLightUI()
{
	
}

CSpotLightUI::~CSpotLightUI()
{

}

LPCTSTR CSpotLightUI::GetClass() const
{
	return _T("SpotLightUI");
}

LPVOID	CSpotLightUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcsicmp(pstrName, _T("SpotLightUI")) == 0 ) return static_cast<CSpotLightUI*>(this);
	return CControlUI::GetInterface(pstrName);
}

void CSpotLightUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
{
	 if( _tcscmp(pstrName, _T("bktranscolor")) == 0 ) {
		 while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
		 if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		 LPTSTR pstr = NULL;
		 DWORD dwTransColor = _tcstoul(pstrValue, &pstr, 16);
		 SetTransColor(dwTransColor);
	 }
	 else if( _tcscmp(pstrName, _T("bktranscolor1")) == 0 ) {
		 while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
		 if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		 LPTSTR pstr = NULL;
		 DWORD dwTransColor = _tcstoul(pstrValue, &pstr, 16);
		 SetTransColor1(dwTransColor);
	 }
	
	CControlUI::SetAttribute(pstrName,pstrValue);
}

void CSpotLightUI::DoPaint( HDC hDC, const RECT& rcPaint )
{

// 	CControlUI::DoPaint(  hDC,  rcPaint );

	if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;

//	

	
	// 

	// »æÖÆÑ­Ðò£º±³¾°ÑÕÉ«->±³¾°Í¼->×´Ì¬Í¼->ÎÄ±¾->±ß¿ò
	CRenderClip roundClip;
	if(m_dwShape == SPOTLIGHT_SHAPE_CIRCLE)
		CRenderClip::GenerateRoundClip(hDC, m_rcPaint,  m_rcItem, m_rcItem.right-m_rcItem.left, m_rcItem.bottom-m_rcItem.top, roundClip);
	else
		CRenderClip::GenerateRoundClip(hDC, m_rcPaint,  m_rcItem, 0, 0, roundClip);
	PaintBkColor(hDC);
	PaintBkImage(hDC);
	PaintStatusImage(hDC);
	PaintText(hDC);
	PaintBorder(hDC);

	// 

	if(m_dwShape == SPOTLIGHT_SHAPE_CIRCLE)
	{
		Pen pen(m_dwTransColor);
		pen.SetWidth(6);

		Gdiplus::Graphics graphics( hDC );
		graphics.SetSmoothingMode(SmoothingModeHighQuality);
		graphics.DrawEllipse(&pen, m_rcItem.left, m_rcItem.top, m_rcItem.right-m_rcItem.left, m_rcItem.bottom-m_rcItem.top);

		Pen pen1(m_dwTransColor1);
		pen1.SetWidth(6);
		graphics.DrawEllipse(&pen1, m_rcItem.left + 5, m_rcItem.top + 5, m_rcItem.right-m_rcItem.left - 10, m_rcItem.bottom-m_rcItem.top - 10);

// 		HRGN h = ((CSpotLightWindowUI *)m_pSpotLightWindow)->GetSpotLightHRgn();
// 		Region fillrgn(h); 
// 		graphics.FillRegion(&SolidBrush(Color::Green),&fillrgn);  
		
	}
	else
	{
		Gdiplus::Graphics graphics( hDC );
		graphics.SetSmoothingMode(SmoothingModeHighQuality);

		Pen pen(m_dwTransColor1);
		pen.SetWidth(6);
		graphics.DrawRectangle(&pen, m_rcItem.left + 2, m_rcItem.top + 2, m_rcItem.right-m_rcItem.left - 4, m_rcItem.bottom-m_rcItem.top - 4);

// 		HRGN h = ((CSpotLightWindowUI *)m_pSpotLightWindow)->GetSpotLightHRgn();
// 		Region fillrgn(h); 
// 		graphics.FillRegion(&SolidBrush(Color::Green),&fillrgn);  
	}
	
//	graphics.DrawEllipse(&pen, m_rcItem.left, m_rcItem.top, 0, 0);
	
}

void CSpotLightUI::SetTransColor( DWORD dwTransColor )
{
	m_dwTransColor = dwTransColor;
}

DWORD CSpotLightUI::GetTransColor()
{
	return m_dwTransColor;
}

void CSpotLightUI::SetTransColor1( DWORD dwTransColor )
{
	m_dwTransColor1 = dwTransColor;
}

DWORD CSpotLightUI::GetTransColor1()
{
	return m_dwTransColor1;
}


void CSpotLightUI::DoInit()
{
	CControlUI::DoInit();
}

void CSpotLightUI::SetShape( DWORD dwShape )
{
	m_dwShape = dwShape;
}

DWORD CSpotLightUI::GetShape()
{
	return m_dwShape;
}
