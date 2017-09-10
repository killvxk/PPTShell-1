#include "stdafx.h"
#include "LabelMulti.h"


CLabelMultiUI::CLabelMultiUI()
{

}

CLabelMultiUI::~CLabelMultiUI()
{

}

LPCTSTR CLabelMultiUI::GetClass() const
{
	return _T("LabelMultiUI");
}

LPVOID	CLabelMultiUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcsicmp(pstrName, _T("LabelMulti")) == 0 ) return static_cast<CLabelMultiUI*>(this);
	return CLabelUI::GetInterface(pstrName);
}

void CLabelMultiUI::PaintText( HDC hDC )
{
	UINT uStyle = GetTextStyle();  
	if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();  
	if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();  

	RECT rc = m_rcItem;  
	rc.left += m_rcTextPadding.left;  
	rc.right -= m_rcTextPadding.right;  
	rc.top += m_rcTextPadding.top;  
	rc.bottom -= m_rcTextPadding.bottom;  

	if(!GetEnabledEffect())  
	{  
		if( m_sText.IsEmpty() ) return;  
		int nLinks = 0;  
		if( IsEnabled() ) {  
			if( m_bShowHtml )  
				CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText, m_dwTextColor, \
				NULL, NULL, nLinks, DT_SINGLELINE | m_uTextStyle);
			else  
				CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, m_dwTextColor, \
				m_iFont, m_uTextStyle); 
		}  
		else {  
			if( m_bShowHtml )  
				CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText, m_dwDisabledTextColor, \
				NULL, NULL, nLinks, DT_SINGLELINE | m_uTextStyle);
			else  
				CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, m_dwDisabledTextColor, \
				m_iFont, m_uTextStyle);
		}  
	}  
	else  
	{  
		Gdiplus::Font	nFont(hDC,m_pManager->GetFont(GetFont()));

		Graphics nGraphics(hDC);
		nGraphics.SetTextRenderingHint(m_TextRenderingHintAntiAlias);

		StringFormat format;
		StringAlignment sa = StringAlignment::StringAlignmentNear;
		if ((m_uTextStyle & DT_VCENTER) != 0) sa = StringAlignment::StringAlignmentCenter;
		else if( (m_uTextStyle & DT_BOTTOM) != 0) sa = StringAlignment::StringAlignmentFar;
		format.SetAlignment((StringAlignment)sa);
		sa = StringAlignment::StringAlignmentNear;
		if ((m_uTextStyle & DT_CENTER) != 0) sa = StringAlignment::StringAlignmentCenter;
		else if( (m_uTextStyle & DT_RIGHT) != 0) sa = StringAlignment::StringAlignmentFar;
		format.SetLineAlignment((StringAlignment)sa);

		RectF nRc((float)rc.left,(float)rc.top,(float)rc.right-rc.left,(float)rc.bottom-rc.top);
		RectF nShadowRc = nRc;
		nShadowRc.X += m_ShadowOffset.X;
		nShadowRc.Y += m_ShadowOffset.Y;

		int nGradientLength	= GetGradientLength();

		if(nGradientLength == 0)
			nGradientLength = (rc.bottom-rc.top);

		LinearGradientBrush nLineGrBrushA(Point(GetGradientAngle(), 0),Point(0,nGradientLength),_MakeRGB(GetTransShadow(),GetTextShadowColorA()),_MakeRGB(GetTransShadow1(),GetTextShadowColorB() == -1?GetTextShadowColorA():GetTextShadowColorB()));
		LinearGradientBrush nLineGrBrushB(Point(GetGradientAngle(), 0),Point(0,nGradientLength),_MakeRGB(GetTransText(),GetTextColor()),_MakeRGB(GetTransText1(),GetTextColor1() == -1?GetTextColor():GetTextColor1()));

		if(GetEnabledStroke() && GetStrokeColor() > 0)
		{
			LinearGradientBrush nLineGrBrushStroke(Point(GetGradientAngle(),0),Point(0,rc.bottom-rc.top+2),_MakeRGB(GetTransStroke(),GetStrokeColor()),_MakeRGB(GetTransStroke(),GetStrokeColor()));

#ifdef _UNICODE
			nRc.Offset(-1,0);
			nGraphics.DrawString(m_TextValue,m_TextValue.GetLength(),&nFont,nRc,&format,&nLineGrBrushStroke);
			nRc.Offset(2,0);
			nGraphics.DrawString(m_TextValue,m_TextValue.GetLength(),&nFont,nRc,&format,&nLineGrBrushStroke);
			nRc.Offset(-1,-1);
			nGraphics.DrawString(m_TextValue,m_TextValue.GetLength(),&nFont,nRc,&format,&nLineGrBrushStroke);
			nRc.Offset(0,2);
			nGraphics.DrawString(m_TextValue,m_TextValue.GetLength(),&nFont,nRc,&format,&nLineGrBrushStroke);
			nRc.Offset(0,-1);
#else
			USES_CONVERSION;
			wstring mTextValue = A2W(m_TextValue.GetData());

			nRc.Offset(-1,0);
			nGraphics.DrawString(mTextValue.c_str(),mTextValue.length(),&nFont,nRc,&format,&nLineGrBrushStroke);
			nRc.Offset(2,0);
			nGraphics.DrawString(mTextValue.c_str(),mTextValue.length(),&nFont,nRc,&format,&nLineGrBrushStroke);
			nRc.Offset(-1,-1);
			nGraphics.DrawString(mTextValue.c_str(),mTextValue.length(),&nFont,nRc,&format,&nLineGrBrushStroke);
			nRc.Offset(0,2);
			nGraphics.DrawString(mTextValue.c_str(),mTextValue.length(),&nFont,nRc,&format,&nLineGrBrushStroke);
			nRc.Offset(0,-1);
#endif

		}
#ifdef _UNICODE
		if(GetEnabledShadow() && (GetTextShadowColorA() > 0 || GetTextShadowColorB() > 0))
			nGraphics.DrawString(m_TextValue,m_TextValue.GetLength(),&nFont,nShadowRc,&format,&nLineGrBrushA);

		nGraphics.DrawString(m_TextValue,m_TextValue.GetLength(),&nFont,nRc,&format,&nLineGrBrushB);
#else
		USES_CONVERSION;
		wstring mTextValue = A2W(m_TextValue.GetData());

		if(GetEnabledShadow() && (GetTextShadowColorA() > 0 || GetTextShadowColorB() > 0))
			nGraphics.DrawString(mTextValue.c_str(),mTextValue.length(),&nFont,nShadowRc,&format,&nLineGrBrushA);

		nGraphics.DrawString(mTextValue.c_str(),mTextValue.length(),&nFont,nRc,&format,&nLineGrBrushB);
#endif

	}  
}

Color CLabelMultiUI::_MakeRGB(int a, Color cl)  
{  
	return Color(a, cl.GetR(), cl.GetG(), cl.GetB());  
}  

Color CLabelMultiUI::_MakeRGB(int r, int g, int b)  
{  
	return Color(255, r, g, b);  
}  

void CLabelMultiUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
{
	if( _tcscmp(pstrName, _T("multiline")) == 0 ) {
		m_uTextStyle |= (DT_WORDBREAK | DT_EDITCONTROL);
	}
	CLabelUI::SetAttribute(pstrName,pstrValue);
}
