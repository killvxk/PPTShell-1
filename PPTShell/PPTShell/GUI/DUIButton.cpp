#include "StdAfx.h"
#include "DUIButton.h"
#include "SkinFrameWnd.h"
#include "Config.h"

CDUIButton::CDUIButton()
{
	m_bIsCheckBox	= FALSE;
	m_bCheck		= FALSE;
	m_bHovering		= FALSE;
	m_bPressed		= FALSE;
	m_bDisabled		= FALSE;
	m_pImage		= NULL;
	m_pIcon			= NULL;
	m_pTipIcon		= NULL;
	m_pParent		= NULL;
	m_bRightOffset	= FALSE;
	m_bRelativeLayout= FALSE;
	m_bAutoSize		= FALSE;
	m_bDivision		= FALSE;
	m_bShowIcon		= TRUE;

	m_strCaption	= _T("");
	m_strTipText	= _T("");
	m_hWndToolTip	= NULL;

	m_nMaxWidth		= BUTTON_MAX_WIDTH;

	m_rcCornerRect = CRect(4, 4, 4, 4);

	memset(m_pDrawImage, 0, sizeof(m_pDrawImage));
	memset(m_rtImage, 0, sizeof(m_rtImage));

	memset(m_pDrawIcon, 0, sizeof(m_pDrawIcon));
	memset(m_rtIcon, 0, sizeof(m_rtIcon));

	memset(m_pDrawTipIcon, 0, sizeof(m_pDrawTipIcon));
	memset(m_rtTipIcon, 0, sizeof(m_rtTipIcon));

	memset(&m_sToolTip, 0, sizeof(TOOLINFO));

	m_dwColor[DS_NORMAL]	= Color::MakeARGB(75, 255, 255, 255);
	m_dwColor[DS_PRESSED]	= Color::White;//Color::MakeARGB(255, 16, 176, 182);
	m_dwColor[DS_HOVERING]	= Color::White;
	m_dwColor[DS_DISABLED]	= Color::Gray;
	m_nLeftMaxLen = 0;
	if (g_Config::GetInstance()->GetModuleVisible(MODULE_MOBILE_CONNECT))
	{
		m_nLeftMaxLen = 120;
	}
	if (g_Config::GetInstance()->GetModuleVisible(MODULE_LOGIN))
	{
		m_nLeftMaxLen += 120;
	}

}

CDUIButton::~CDUIButton()
{
	/*if ( m_sToolTip != NULL )
		delete m_sToolTip;*/
}

// 绘制按钮
int CDUIButton::DrawButton( Gdiplus::Graphics&  graphics, CSize offset)
{
	// 获取按钮图片信息

	UINT	nButtonStatus = DS_NORMAL;

	if ( m_bDisabled )
		nButtonStatus = DS_DISABLED;
	else if ( m_bPressed )
		nButtonStatus = DS_PRESSED;
	else if ( m_bHovering )
		nButtonStatus = DS_HOVERING;
	else if (m_bIsCheckBox && m_bCheck )
		nButtonStatus = DS_PRESSED;


	RectF  rtImage	= m_rtImage[nButtonStatus];
	Image* pImage	= m_pDrawImage[nButtonStatus];

	RectF  rtIcon	= m_rtIcon[nButtonStatus];
	Image* pIcon	= m_pDrawIcon[nButtonStatus];

	RectF  rtTipIcon	= m_rtTipIcon[nButtonStatus];
	Image* pTipIcon		= m_pDrawTipIcon[nButtonStatus];

	//绘制文字
	FontFamily fontFamily(L"微软雅黑");// L"微软雅黑"
	Gdiplus::Font font(&fontFamily, 14, FontStyleBold, UnitPixel);

	int nNeedX				= 0;
	//BOOL bDrawNormalIcon	= TRUE;

	if ( m_bAutoSize && !m_strCaption.IsEmpty() )   //计算文字长度
	{
		CStringW strCaption(m_strCaption);
		RectF boundRect;  
		INT codePointsFitted	= 0;  
		INT linesFitted			= 0; 
		RectF layoutRect;

		//计算文字所需的宽度
		layoutRect.X		=(Gdiplus::REAL)m_rcRect.left;
		layoutRect.Y		=(Gdiplus::REAL)m_rcRect.top;
		layoutRect.Width	= (Gdiplus::REAL)m_nMaxWidth;
		layoutRect.Height	= (Gdiplus::REAL)m_rcRect.Height();

		graphics.MeasureString(strCaption,strCaption.GetLength(),&font,layoutRect,NULL,&boundRect,&codePointsFitted,&linesFitted);

		CRect rtParent;
		m_pParent->GetWindowRect(rtParent);

		int nMaxWidth = WINDOW_MIN_WIDTH;		
		
		nMaxWidth += m_nLeftMaxLen; //2015.11.11 cws进行动态宽度调整
		int ButtonMaxWidth = rtParent.Width() > nMaxWidth ? m_nMaxWidth : m_nMaxWidth - (nMaxWidth - rtParent.Width()) ;  //当窗口小于nMaxWidth的时候，显示章节按钮不能太大

		/*if ( rtIcon.Width > ButtonMaxWidth )
			bDrawNormalIcon = FALSE;*/
		
		nNeedX += int(boundRect.Width < ButtonMaxWidth ? (boundRect.Width) : (ButtonMaxWidth));

		if ( nNeedX < 0 ) nNeedX = 0;

		int nXOffset = nNeedX;//s - m_rcRect.Width();
		
		if ( pIcon )
		{
			nXOffset += (m_ptIcon.x);  //图标到按钮的偏移
			nXOffset += m_csizeIcon.cx;	//图标大小
			nXOffset += TEXT_OFFSET;	//图标到文字的偏移
		}

		if ( !m_bRightOffset )
		{
			m_rcRect.right = m_rcRect.left + nXOffset;
			//m_rcRect.right += (nXOffset - m_rcOffsetRect.Width());
			//m_rcOffsetRect.right += nXOffset;
			//m_rcOffsetRect.OffsetRect(-nXOffset, 0);
		}
		else
		{
			m_rcRect.left = m_rcRect.right - nXOffset;
			//m_rcRect.left -= (nXOffset);
			//m_rcOffsetRect.left += nXOffset;
			//m_rcOffsetRect.OffsetRect(nXOffset, 0);
			//m_rcRect.left = m_rcRect.right - nXOffset;
		}

	}
	
	RectF grect;
	grect.X			= (Gdiplus::REAL)m_rcRect.left;
	grect.Y			= (Gdiplus::REAL)m_rcRect.top;
	grect.Width		= (Gdiplus::REAL)m_rcRect.Width();
	grect.Height	= (Gdiplus::REAL)m_rcRect.Height();

	if ( m_bRelativeLayout )
	{
		grect.X += offset.cx;
		grect.Y += offset.cy;
	}

	// 在指定位置绘制按钮
	if ( m_pImage )
	{
		if ( !m_bDivision )
		{
			graphics.DrawImage(m_pImage, grect, rtImage.GetLeft(), rtImage.GetTop(), rtImage.Width, rtImage.Height, UnitPixel);
		}
		else
		{
			//左上角
			RectF destRect;
			destRect.X = grect.X;
			destRect.Y = grect.Y;
			destRect.Width = REAL(m_rcCornerRect.left);
			destRect.Height = REAL(m_rcCornerRect.top);
			graphics.DrawImage(m_pImage,destRect,
				rtImage.GetLeft(),
				rtImage.GetTop(),
				REAL(destRect.Width),
				REAL(destRect.Height),
				UnitPixel);

			//上中
			destRect.X = grect.X + m_rcCornerRect.left;
			destRect.Y = grect.Y;
			destRect.Width = REAL(grect.Width - m_rcCornerRect.left - m_rcCornerRect.right);
			destRect.Height = REAL(m_rcCornerRect.top);
			graphics.DrawImage(m_pImage, destRect,
				rtImage.GetLeft() + m_rcCornerRect.left,
				rtImage.GetTop(), 
				REAL(rtImage.Width - m_rcCornerRect.left - m_rcCornerRect.right), 
				REAL(destRect.Height), UnitPixel);

			//右上角
			destRect.X = grect.X + (grect.Width - m_rcCornerRect.left);
			destRect.Y = grect.Y;
			destRect.Width = REAL(m_rcCornerRect.right);
			destRect.Height = REAL(m_rcCornerRect.top);
			graphics.DrawImage(m_pImage,destRect, 
				rtImage.GetLeft() + (rtImage.Width - m_rcCornerRect.right),
				rtImage.GetTop(),
				REAL(destRect.Width), REAL(destRect.Height), UnitPixel);

			//中左
			destRect.X = grect.X;
			destRect.Y = grect.Y + m_rcCornerRect.top;
			destRect.Width = REAL(m_rcCornerRect.left);
			destRect.Height = REAL(grect.Height - m_rcCornerRect.top - m_rcCornerRect.bottom);
			graphics.DrawImage(m_pImage, destRect,
				rtImage.GetLeft(),
				rtImage.GetTop() + m_rcCornerRect.top, 
				REAL(destRect.Width), 
				REAL(rtImage.Height - m_rcCornerRect.top - m_rcCornerRect.bottom), UnitPixel);

			//中中
			destRect.X = grect.X + m_rcCornerRect.left;
			destRect.Y = grect.Y + m_rcCornerRect.top;
			destRect.Width = REAL(grect.Width - m_rcCornerRect.left - m_rcCornerRect.right);
			destRect.Height = REAL(grect.Height - m_rcCornerRect.top - m_rcCornerRect.bottom);
			graphics.DrawImage(m_pImage, destRect,
				rtImage.GetLeft() + m_rcCornerRect.left,
				rtImage.GetTop() + m_rcCornerRect.top, 
				REAL(rtImage.Width - m_rcCornerRect.left - m_rcCornerRect.right),
				REAL(rtImage.Height - m_rcCornerRect.top - m_rcCornerRect.bottom), UnitPixel);

			//中右
			destRect.X = grect.X + (grect.Width - m_rcCornerRect.left);
			destRect.Y = grect.Y + m_rcCornerRect.top;
			destRect.Width = REAL(m_rcCornerRect.right);
			destRect.Height = REAL(grect.Height - m_rcCornerRect.top - m_rcCornerRect.bottom);
			graphics.DrawImage(m_pImage,destRect, 
				rtImage.GetLeft() + (rtImage.Width - m_rcCornerRect.right),
				rtImage.GetTop() + m_rcCornerRect.top,
				REAL(destRect.Width),
				REAL(rtImage.Height - m_rcCornerRect.top - m_rcCornerRect.bottom), UnitPixel);

			//左下角
			destRect.X = grect.X;
			destRect.Y = grect.Y + (grect.Height - m_rcCornerRect.bottom);
			destRect.Width = REAL(m_rcCornerRect.left);
			destRect.Height = REAL(m_rcCornerRect.bottom);
			graphics.DrawImage(m_pImage,destRect,
				rtImage.GetLeft(),
				rtImage.GetTop() + (rtImage.Height - m_rcCornerRect.bottom), 
				REAL(destRect.Width),
				REAL(destRect.Height), UnitPixel);

			//下中
			destRect.X = grect.X + m_rcCornerRect.left;
			destRect.Y = grect.Y + (grect.Height - m_rcCornerRect.bottom);
			destRect.Width = REAL(grect.Width - m_rcCornerRect.left - m_rcCornerRect.right);
			destRect.Height = REAL(m_rcCornerRect.bottom);
			graphics.DrawImage(m_pImage,destRect,
				rtImage.GetLeft() + m_rcCornerRect.left,
				rtImage.GetTop() + (rtImage.Height - m_rcCornerRect.bottom), 
				REAL(rtImage.Width - m_rcCornerRect.left - m_rcCornerRect.right),
				REAL(destRect.Height), UnitPixel);

			//右下角
			destRect.X = grect.X + (grect.Width - m_rcCornerRect.left);
			destRect.Y = grect.Y + (grect.Height - m_rcCornerRect.bottom);
			destRect.Width = REAL(m_rcCornerRect.right);
			destRect.Height = REAL(m_rcCornerRect.bottom);
			graphics.DrawImage(m_pImage,destRect,
				rtImage.GetLeft() + (rtImage.Width - m_rcCornerRect.right),
				rtImage.GetTop() + (rtImage.Height - m_rcCornerRect.bottom), 
				REAL(destRect.Width),
				REAL(destRect.Height), UnitPixel);
		}
	}

	StringFormat stringFormat;

	if ( m_pIcon && m_bShowIcon )
	{
		//int DrawX = rtIcon.Width;
		//if ( !bDrawNormalIcon )
		//	DrawX = m_csizeIcon.cx;
		//
		//RectF rtf = RectF(m_ptIcon.x + m_rcRect.left, m_ptIcon.y + m_rcRect.top, DrawX, m_csizeIcon.cy);

		//rtf.X -= (m_rtIcon->Width - m_csizeIcon.cx);

		//if ( !m_bRelativeLayout ) //靠左自动偏移
		//{
		//	rtf.X += offset.cx;
		//	rtf.Y += offset.cy;
		//}

		//graphics.DrawImage(m_pIcon, rtf, rtIcon.GetLeft(), rtIcon.GetTop(), rtIcon.Width, rtIcon.Height, UnitPixel);
		//grect.X=(Gdiplus::REAL)m_ptIcon.x + m_rcRect.left + DrawX + TEXT_OFFSET - (m_rtIcon->Width - m_csizeIcon.cx); //文字的起始位置
		//grect.Width = (Gdiplus::REAL)m_rcRect.Width() - m_ptIcon.x - DrawX + (m_rtIcon->Width - m_csizeIcon.cx);  //文字的宽度

		RectF rtf = RectF(m_ptIcon.x + m_rcRect.left, m_ptIcon.y + m_rcRect.top, m_csizeIcon.cx, m_csizeIcon.cy);

		if ( !m_bRelativeLayout ) //靠左自动偏移
		{
			rtf.X += offset.cx;
			rtf.Y += offset.cy;
		}

		graphics.DrawImage(m_pIcon, rtf, rtIcon.GetLeft(), rtIcon.GetTop(), rtIcon.Width, rtIcon.Height, UnitPixel);
		grect.X=(Gdiplus::REAL)m_ptIcon.x + m_rcRect.left + m_csizeIcon.cx + TEXT_OFFSET; //文字的起始位置
		grect.Width = (Gdiplus::REAL)m_rcRect.Width() - m_ptIcon.x - m_csizeIcon.cx;  //文字的宽度

		if ( !m_bRelativeLayout )
		{
			grect.X += offset.cx;
		}

		stringFormat.SetTrimming(StringTrimmingEllipsisCharacter);
		//stringFormat.SetFormatFlags(StringFormatFlagsDirectionVertical);
		stringFormat.SetAlignment(StringAlignmentNear);
		stringFormat.SetLineAlignment(StringAlignmentCenter);
	}
	else
	{
		//stringFormat.SetFormatFlags( StringFormatFlagsDirectionVertical);
		stringFormat.SetAlignment(StringAlignmentCenter);
		stringFormat.SetLineAlignment(StringAlignmentCenter);
	}

	//button text
	if ( !m_strCaption.IsEmpty() )
	{
		grect.Y			= m_rcRect.top + (m_rcRect.Height() - font.GetHeight(&graphics))/2;
		if (grect.Width < 1) grect.Width = 1;
		grect.Height	= font.GetHeight(&graphics) + 2;

		CStringW strTitle(m_strCaption);

		SolidBrush brush(m_dwColor[nButtonStatus]);

		graphics.SetTextRenderingHint(TextRenderingHintSystemDefault);  
		graphics.DrawString(strTitle, strTitle.GetLength(), &font, grect, &stringFormat, &brush);
	}

	if (m_pTipIcon)	//提示图标
	{
		RectF rtf = RectF(m_rcRect.left + m_rcRect.Width() - m_csizeTipIcon.cx + m_ptTipIcon.x, m_ptTipIcon.y + m_rcRect.top, m_csizeTipIcon.cx, m_csizeTipIcon.cy);

		if ( !m_bRelativeLayout ) //靠左自动偏移
		{
			rtf.X += offset.cx;
			rtf.Y += offset.cy;
		}

		graphics.DrawImage(m_pTipIcon, rtf, rtTipIcon.GetLeft(), rtTipIcon.GetTop(), rtTipIcon.Width, rtTipIcon.Height, UnitPixel);
	}

	return nNeedX;
}

// 创建按钮
void CDUIButton::CreateButton(LPCTSTR lpszCaption,LPRECT lpRect,CWnd* pParent, Image* pImage, UINT nCount/* = 4*/, UINT nID/* = 0*/, BOOL bHorizontal /*= TRUE*/, BOOL bRight /*= FALSE*/, BOOL bRelativeLayout /*= FALSE*/)
{
	if (lpszCaption)
	{
		m_strCaption = lpszCaption;
	}
	m_pParent		= pParent;
	m_rcRect		= lpRect;
	m_rcOffsetRect	= lpRect;
	m_pImage		= pImage;
	m_nImageCount	= nCount;
	m_nID			= nID;
	m_bRightOffset	= bRight;
	m_bRelativeLayout= bRelativeLayout;

	if ( pImage == NULL)
		return;

	int nWidth	= 0;
	int nHight	= 0;

	if ( bHorizontal )
	{
		nWidth	= pImage->GetWidth()/nCount;
		nHight	= pImage->GetHeight();
	}
	else
	{
		nWidth	= pImage->GetWidth();
		nHight	= pImage->GetHeight()/nCount;
	}

	int i = 0;
	
	//初始化已有的图片数据
	for (i = 0; i < nCount; i++)
	{
		m_pDrawImage[i] = pImage;

		RectF rt;
		if ( bHorizontal )
			rt = RectF( i * nWidth, 0, nWidth, nHight );
		else
			rt = RectF(0, i * nHight, nWidth, nHight );

		m_rtImage[i] = rt;
	}

	//其它没图的的设置跟正常一样
	for ( i; i < DS_TOTAL; i++)
	{
		m_pDrawImage[i] = m_pDrawImage[DS_NORMAL];
		m_rtImage[i]	= m_rtImage[DS_NORMAL];
	}
}

void CDUIButton::SetBKImage(Image *pImage, UINT nCount, BOOL bHorizontal, BOOL bRedraw)
{
	if ( pImage == NULL)
		return;
	
	m_pImage		= pImage;
	m_nImageCount	= nCount;

	int nWidth	= 0;
	int nHight	= 0;

	if ( bHorizontal )
	{
		nWidth	= pImage->GetWidth()/nCount;
		nHight	= pImage->GetHeight();
	}
	else
	{
		nWidth	= pImage->GetWidth();
		nHight	= pImage->GetHeight()/nCount;
	}

	int i = 0;

	//初始化已有的图片数据
	for (i = 0; i < nCount; i++)
	{
		m_pDrawImage[i] = pImage;

		RectF rt;
		if ( bHorizontal )
			rt = RectF( i * nWidth, 0, nWidth, nHight );
		else
			rt = RectF(0, i * nHight, nWidth, nHight );

		m_rtImage[i] = rt;
	}

	//其它没图的的设置跟正常一样
	for ( i; i < DS_TOTAL; i++)
	{
		m_pDrawImage[i] = m_pDrawImage[DS_NORMAL];
		m_rtImage[i]	= m_rtImage[DS_NORMAL];
	}
}

//设置按钮图标信息
void  CDUIButton::SetIconInfo(Image *pIcon, UINT nCount /* = 1 */, CPoint ptPos/* = CPoint(5,3)*/, BOOL bHorizontal /*= TRUE*/,BOOL bRedraw/*=FALSE*/)
{
    m_pIcon		= pIcon;
	m_nIconCount= nCount;

	memset(m_pDrawIcon, 0, sizeof(m_pDrawIcon));
	memset(m_rtIcon, 0, sizeof(m_rtIcon));

	if ( pIcon == NULL)
	{
		if (bRedraw)
		{
			((CSkinFrameWnd*)m_pParent)->OnNcPaint();
		}
		return;
	}
	
	int nWidth	= 0;
	int nHight	= 0;

	if ( bHorizontal )
	{
		nWidth	= pIcon->GetWidth()/nCount;
		nHight	= pIcon->GetHeight();
	}
	else
	{
		nWidth	= pIcon->GetWidth();
		nHight	= pIcon->GetHeight()/nCount;
	}
	
	if (ptPos.y == -1)
	{
		int nPosY	= (m_rcRect.Height() - nHight) / 2;
		m_ptIcon	= CPoint(ptPos.x, nPosY);
	}
	else
	{
		m_ptIcon	= CPoint(ptPos.x, ptPos.y);
	}

	m_csizeIcon		= CSize(nWidth, nHight);

	int i = 0;

	//初始化已有的图片数据
	for (i = 0; i < nCount; i++)
	{
		m_pDrawIcon[i] = pIcon;

		RectF rt;
		if ( bHorizontal )
			rt = RectF( i * nWidth, 0, nWidth, nHight );
		else
			rt = RectF(0, i * nHight, nWidth, nHight );

		m_rtIcon[i] = rt;
	}

	//其它没图的的设置跟正常一样
	for ( i; i < DS_TOTAL; i++)
	{
		m_pDrawIcon[i] = m_pDrawIcon[DS_NORMAL];
		m_rtIcon[i]	= m_rtIcon[DS_NORMAL];
	}

	if (bRedraw)
	{
		((CSkinFrameWnd*)m_pParent)->OnNcPaint();
	}
}

void CDUIButton::SetIconInfo( Image *pIcon, UINT nCount, CSize cSize/*= CSize(20,20)*/, CPoint ptPos /*= CPoint(5,3)*/, BOOL bHorizontal /*= TRUE*/, BOOL bRedraw/*=FALSE*/ )
{
	m_pIcon		= pIcon;
	m_nIconCount= nCount;

	memset(m_pDrawIcon, 0, sizeof(m_pDrawIcon));
	memset(m_rtIcon, 0, sizeof(m_rtIcon));

	if ( pIcon == NULL)
	{
		if (bRedraw)
		{
			((CSkinFrameWnd*)m_pParent)->OnNcPaint();
		}
		return;
	}

	int nWidth	= 0;
	int nHight	= 0;

	if ( bHorizontal )
	{
		nWidth	= pIcon->GetWidth()/nCount;
		nHight	= pIcon->GetHeight();
	}
	else
	{
		nWidth	= pIcon->GetWidth();
		nHight	= pIcon->GetHeight()/nCount;
	}

	if (ptPos.y == -1)
	{
		int nPosY	= (m_rcRect.Height() - cSize.cy) / 2;
		m_ptIcon	= CPoint(ptPos.x, nPosY);
	}
	else
	{
		m_ptIcon	= CPoint(ptPos.x, ptPos.y);
	}

	m_csizeIcon	= cSize;

	int i = 0;

	//初始化已有的图片数据
	for (i = 0; i < nCount; i++)
	{
		m_pDrawIcon[i] = pIcon;

		RectF rt;
		if ( bHorizontal )
			rt = RectF( i * nWidth, 0, nWidth, nHight );
		else
			rt = RectF(0, i * nHight, nWidth, nHight );

		m_rtIcon[i] = rt;
	}

	//其它没图的的设置跟正常一样
	for ( i; i < DS_TOTAL; i++)
	{
		m_pDrawIcon[i] = m_pDrawIcon[DS_NORMAL];
		m_rtIcon[i]	= m_rtIcon[DS_NORMAL];
	}
	if (bRedraw)
	{
		((CSkinFrameWnd*)m_pParent)->OnNcPaint();
	}
}

void  CDUIButton::SetStatusIconInfo(Image *pIcon, UINT nStatus, CSize cSize, CPoint ptPos/* = CPoint(5,3)*/)
{
	if ( pIcon == NULL)
		return;
	
	m_pDrawIcon[nStatus]	= pIcon;
	m_rtIcon[nStatus]		= RectF(0, 0, pIcon->GetWidth(), pIcon->GetHeight());
	m_ptIcon				= CPoint(ptPos.x, ptPos.y);
	m_csizeIcon				= cSize;

	if (ptPos.y == -1)
	{
		int nPosY	= (m_rcRect.Height() - cSize.cy) / 2;
		m_ptIcon	= CPoint(ptPos.x, nPosY);
	}
	else
	{
		m_ptIcon	= CPoint(ptPos.x, ptPos.y);
	}
}

//设置提示图标信息
void  CDUIButton::SetTipIconInfo(Image *pIcon, UINT nCount /*= 1*/, CPoint ptPos /*= CPoint(5,-1)*/, BOOL bHorizontal /*= TRUE*/, BOOL bRedraw /*= FALSE*/)
{
	m_pTipIcon		= pIcon;
	m_nTipIconCount	= nCount;

	memset(m_pDrawTipIcon, 0, sizeof(m_pDrawTipIcon));
	memset(m_rtTipIcon, 0, sizeof(m_rtTipIcon));

	if ( pIcon == NULL)
	{
		if (bRedraw)
		{
			((CSkinFrameWnd*)m_pParent)->OnNcPaint();
		}
		return;
	}

	int nWidth	= 0;
	int nHight	= 0;

	if ( bHorizontal )
	{
		nWidth	= pIcon->GetWidth()/nCount;
		nHight	= pIcon->GetHeight();
	}
	else
	{
		nWidth	= pIcon->GetWidth();
		nHight	= pIcon->GetHeight()/nCount;
	}

	if (ptPos.y == -1)
	{
		int nPosY	= (m_rcRect.Height() - nHight) / 2;
		m_ptTipIcon	= CPoint(ptPos.x, nPosY);
	}
	else
	{
		m_ptTipIcon	= CPoint(ptPos.x, ptPos.y);
	}

	m_csizeTipIcon	= CSize(nWidth, nHight);

	int i = 0;

	//初始化已有的图片数据
	for (i = 0; i < nCount; i++)
	{
		m_pDrawTipIcon[i] = pIcon;

		RectF rt;
		if ( bHorizontal )
			rt = RectF( i * nWidth, 0, nWidth, nHight );
		else
			rt = RectF(0, i * nHight, nWidth, nHight );

		m_rtTipIcon[i] = rt;
	}

	//其它没图的的设置跟正常一样
	for ( i; i < DS_TOTAL; i++)
	{
		m_pDrawTipIcon[i]	= m_pDrawTipIcon[DS_NORMAL];
		m_rtTipIcon[i]		= m_rtTipIcon[DS_NORMAL];
	}

	if (bRedraw)
	{
		((CSkinFrameWnd*)m_pParent)->OnNcPaint();
	}
}

void  CDUIButton::SetTipIconInfo(Image *pIcon, UINT nCount, CSize cSize, CPoint ptPos /*= CPoint(5,-1)*/, BOOL bHorizontal /*= TRUE*/, BOOL bRedraw /*= FALSE*/)
{
	m_pTipIcon		= pIcon;
	m_nTipIconCount	= nCount;

	memset(m_pDrawTipIcon, 0, sizeof(m_pDrawTipIcon));
	memset(m_rtTipIcon, 0, sizeof(m_rtTipIcon));

	if ( pIcon == NULL)
	{
		if (bRedraw)
		{
			((CSkinFrameWnd*)m_pParent)->OnNcPaint();
		}
		return;
	}

	int nWidth	= 0;
	int nHight	= 0;

	if ( bHorizontal )
	{
		nWidth	= pIcon->GetWidth()/nCount;
		nHight	= pIcon->GetHeight();
	}
	else
	{
		nWidth	= pIcon->GetWidth();
		nHight	= pIcon->GetHeight()/nCount;
	}

	if (ptPos.y == -1)
	{
		int nPosY	= (m_rcRect.Height() - cSize.cy) / 2;
		m_ptIcon	= CPoint(ptPos.x, nPosY);
	}
	else
	{
		m_ptTipIcon	= CPoint(ptPos.x, ptPos.y);
	}

	m_csizeTipIcon	= cSize;

	int i = 0;

	//初始化已有的图片数据
	for (i = 0; i < nCount; i++)
	{
		m_pDrawTipIcon[i] = pIcon;

		RectF rt;
		if ( bHorizontal )
			rt = RectF( i * nWidth, 0, nWidth, nHight );
		else
			rt = RectF(0, i * nHight, nWidth, nHight );

		m_rtTipIcon[i] = rt;
	}

	//其它没图的的设置跟正常一样
	for ( i; i < DS_TOTAL; i++)
	{
		m_pDrawTipIcon[i]	= m_pDrawTipIcon[DS_NORMAL];
		m_rtTipIcon[i]		= m_rtIcon[DS_NORMAL];
	}
	if (bRedraw)
	{
		((CSkinFrameWnd*)m_pParent)->OnNcPaint();
	}
}

void  CDUIButton::SetStatusTipIconInfo(Image *pIcon, UINT nStatus, CSize cSize, CPoint ptPos /*= CPoint(5,-1)*/)
{
	if ( pIcon == NULL)
		return;

	m_pDrawTipIcon[nStatus]		= pIcon;
	m_rtTipIcon[nStatus]		= RectF(0, 0, pIcon->GetWidth(), pIcon->GetHeight());
	m_ptTipIcon					= CPoint(ptPos.x, ptPos.y);
	m_csizeTipIcon				= cSize;

	if (ptPos.y == -1)
	{
		int nPosY	= (m_rcRect.Height() - cSize.cy) / 2;
		m_ptTipIcon	= CPoint(ptPos.x, nPosY);
	}
	else
	{
		m_ptTipIcon	= CPoint(ptPos.x, ptPos.y);
	}
}

void CDUIButton::SetRect(LPCRECT lpRect)
{
	//CPoint ptIconOrg(m_ptIcon.x - m_rcRect.left,m_ptIcon.y - m_rcRect.top);
	m_rcRect = lpRect;
	//m_ptIcon = CPoint(m_rcRect.left+ptIconOrg.x,m_rcRect.top+ptIconOrg.y);
}

void CDUIButton::SetCaption( CString strCaption )
{
	m_strCaption = strCaption;
	((CSkinFrameWnd*)m_pParent)->OnNcPaint();
}

void CDUIButton::SetStatusTextInfo( UINT nStatus, DWORD dwColor )
{
	m_dwColor[nStatus] = dwColor;
}

void CDUIButton::SetDivsionRect( CRect rect )
{
	m_bDivision = TRUE;
	m_rcCornerRect = rect;
}

void CDUIButton::ShowToolTip()
{
	if ( m_strTipText.IsEmpty() )
		return;

	if ( m_sToolTip.hwnd == NULL )
	{
		memset(&m_sToolTip, 0, sizeof(TOOLINFO));
		m_sToolTip.cbSize		= sizeof(TOOLINFO);
		m_sToolTip.uFlags		= TTF_IDISHWND;
		m_sToolTip.hwnd			= m_pParent->m_hWnd;
		m_sToolTip.uId			= (UINT_PTR) m_pParent->m_hWnd;
		m_sToolTip.hinst		= AfxGetApp()->m_hInstance;
	}

	m_sToolTip.lpszText			= (LPTSTR)m_strTipText.GetString();

	CPoint pt;
	GetCursorPos(&pt);

	if( m_hWndToolTip == NULL )
	{
		m_ToolLastPos = pt;
		m_hWndToolTip = ::CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, NULL, NULL);
		::SendMessage(m_hWndToolTip, TTM_ADDTOOL, 0, (LPARAM)&m_sToolTip);
		::SendMessage(m_hWndToolTip, TTM_SETMAXTIPWIDTH, 0, 500);
	}

	::SendMessage( m_hWndToolTip, TTM_SETMAXTIPWIDTH,0, 500);
	if (m_ToolLastPos.x != pt.x || m_ToolLastPos.y != pt.y)
	{
		::SendMessage(m_hWndToolTip, TTM_SETTOOLINFO, 0, (LPARAM)&m_sToolTip);
		m_ToolLastPos = pt;
	}
	::SendMessage(m_hWndToolTip, TTM_TRACKACTIVATE, (WPARAM)TRUE, (LPARAM)&m_sToolTip);
}

void CDUIButton::MoveToolTip()
{
	if( m_hWndToolTip == NULL ) 
		return;
	
	CPoint pt;
	GetCursorPos(&pt);
	if (m_ToolLastPos.x != pt.x || m_ToolLastPos.y != pt.y)
	{
		::SendMessage(m_hWndToolTip, TTM_SETTOOLINFO, 0, (LPARAM)&m_sToolTip);
		m_ToolLastPos = pt;
	}
	::SendMessage(m_hWndToolTip, TTM_TRACKACTIVATE, (WPARAM)TRUE, (LPARAM)&m_sToolTip);
}

void CDUIButton::HideToolTip()
{
	if( m_hWndToolTip != NULL ) 
		::SendMessage(m_hWndToolTip, TTM_TRACKACTIVATE, FALSE, (LPARAM)&m_sToolTip);
}

void CDUIButton::SetShowIcon( BOOL bShow )
{
	
	if ( m_pIcon && !m_bAutoSize && (m_bShowIcon != bShow) )
	{
		if ( !bShow )
		{
			m_rcRect.right -= m_rtIcon->Width;
			m_rcOffsetRect.right -= m_rtIcon->Width;
		}
		else
		{
			m_rcRect.right += m_rtIcon->Width;
			m_rcOffsetRect.right += m_rtIcon->Width;
		}

		m_bShowIcon = bShow;
	}
}


//void CDUIButton::DrawColor(HDC hDC, const RECT& rc, DWORD color)
//{
//    //if( color <= 0x00FFFFFF ) return;
//    //if( color >= 0xFF000000 )
//    //{
//    //    ::SetBkColor(hDC, RGB(GetBValue(color), GetGValue(color), GetRValue(color)));
//    //    ::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
//    //}
//    //else
//    //{
//    //    // Create a new 32bpp bitmap with room for an alpha channel
//    //    BITMAPINFO bmi = { 0 };
//    //    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
//    //    bmi.bmiHeader.biWidth = 1;
//    //    bmi.bmiHeader.biHeight = 1;
//    //    bmi.bmiHeader.biPlanes = 1;
//    //    bmi.bmiHeader.biBitCount = 32;
//    //    bmi.bmiHeader.biCompression = BI_RGB;
//    //    bmi.bmiHeader.biSizeImage = 1 * 1 * sizeof(DWORD);
//    //    LPDWORD pDest = NULL;
//    //    HBITMAP hBitmap = ::CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (LPVOID*) &pDest, NULL, 0);
//    //    if( !hBitmap ) return;
//
//    //    *pDest = color;
//
//    //    RECT rcBmpPart = {0, 0, 1, 1};
//    //    RECT rcCorners = {0};
//    //    DrawImage(hDC, hBitmap, rc, rc, rcBmpPart, rcCorners, true, 255);
//    //    ::DeleteObject(hBitmap);
//    //}
//}