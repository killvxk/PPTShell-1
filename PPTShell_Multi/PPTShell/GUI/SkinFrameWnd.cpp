// SkinFrameWnd.cpp : implementation file
//

#include "stdafx.h"
#include "SkinManager.h"
#include "SkinFrameWnd.h"
#include "GUI/MainFrm.h"
//#include "EventCenter/EventDefine.h"
#include "Config.h"
#include <math.h>

#define  ID_MASK_SYSTEM_BTN  9999
#define  ID_CLOSE_BTN 110
#define  ID_MAX_BTN   111
#define  ID_MIN_BTN   112

#define TITLE_RIGHT_MIN_WIDTH (1250)	//增加左边按钮，要修改这个长，是从标题尾到右边框的最小宽度
#define	LEFT_BUTTON_TO_LEFT_MIN_WIDTH	(0)
#define	RIGHT_BUTTON_TO_LEFT_MIN_WIDTH	(560)
#define SHOW_ICON_WINDOW_MIN_WIDTH (1100)

// CSkinFrameWnd

IMPLEMENT_DYNCREATE(CSkinFrameWnd,CFrameWndEx)

CSkinFrameWnd::CSkinFrameWnd()
{

	m_nTitleHeight			= 50;
	m_nFrameWidth			= 3;
	m_nMaximizeTopOffset	= 0;
	m_pBtnRestore			= NULL;

	m_TitleBtn.clear();

	Image *pImage = CSkinManager::GetInstance()->GetSkinItem(_T("btn_min.png"));
	m_WindowBtnImage.push_back(pImage);

	pImage = CSkinManager::GetInstance()->GetSkinItem(_T("btn_max.png"));
	m_WindowBtnImage.push_back(pImage);

	pImage = CSkinManager::GetInstance()->GetSkinItem(_T("btn_restore.png"));
	m_WindowBtnImage.push_back(pImage);

	pImage = CSkinManager::GetInstance()->GetSkinItem(_T("btn_close.png"));
	m_WindowBtnImage.push_back(pImage);

	m_pBkImage	= CSkinManager::GetInstance()->GetSkinItem(_T("window_bg.png"));

	m_pPPTImage = CSkinManager::GetInstance()->GetSkinItem(_T("logo.png"));

	m_iXButtonHovering = -1;

	m_bNCHovering = FALSE;
	m_bNCTracking = FALSE;

	m_rcIcon = CRect(15,13,15+GetSystemMetrics(SM_CXSMICON),13+GetSystemMetrics(SM_CYSMICON));

	m_nAddLen = 0;
	if (g_Config::GetInstance()->GetModuleVisible(MODULE_MOBILE_CONNECT))
	{
		m_nAddLen = 150;
	}
}

CSkinFrameWnd::~CSkinFrameWnd()
{
} 


BEGIN_MESSAGE_MAP(CSkinFrameWnd, CFrameWndEx)
	ON_WM_NCPAINT()
	ON_WM_NCCALCSIZE()
	ON_WM_NCHITTEST()
	ON_WM_GETMINMAXINFO()
	ON_WM_NCACTIVATE()
	ON_WM_ACTIVATE()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCLBUTTONUP()
	ON_WM_NCRBUTTONDOWN()
	ON_WM_NCMOUSEMOVE()
	ON_WM_NCMOUSEHOVER()
	ON_WM_NCMOUSELEAVE()
	ON_WM_SIZE()
	ON_WM_SYSCOMMAND()
	ON_WM_NCCREATE()
END_MESSAGE_MAP()


// CSkinFrameWnd message handlers

void CSkinFrameWnd::OnNcPaint()
{
	
	CWindowDC dc(this);

	if ( m_pBkImage == NULL )
	{
		return;
	}

	CDC MemDC;

	// 获取位置
	CRect rcWindow, rcRelWindow;
	GetWindowRect(&rcWindow);

	rcWindow.OffsetRect(-(rcWindow.left), -(rcWindow.top));

	CRect rcClient;
	GetClientRect(&rcClient);

	//剪除掉客户区
	rcClient.OffsetRect(m_nFrameWidth, m_nTitleHeight);

	if ( IsZoomed() )
	{
		CRect rt;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0);
		//int nSX = GetSystemMetrics(SM_CXSCREEN);
		//int nSY	= GetSystemMetrics(SM_CYSCREEN);
		//int nTaskBarHight = nSX - rt.bottom;

		if ( rcWindow.left < rt.right/2 )
		{
			rcWindow.bottom = rt.bottom;
			rcClient.bottom = rt.bottom;	
		}
		//rcWindow.right = rcWindow.left + GetSystemMetrics(SM_CXSCREEN);
	}

	dc.ExcludeClipRect(rcClient.left, rcClient.top,
		rcClient.right, rcClient.bottom);

	MemDC.CreateCompatibleDC(&dc);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc,rcWindow.Width(),rcWindow.Height());
	MemDC.SelectObject(&bmp);
	MemDC.SetBkMode(TRANSPARENT);

	Gdiplus::Graphics graphics(MemDC.GetSafeHdc());
	RectF destRect;

	//绘制底部和边框
	destRect.X = 0;
	destRect.Y = REAL(rcWindow.Height() - m_nFrameWidth);
	destRect.Width = REAL(rcWindow.Width());
	destRect.Height = REAL(m_nFrameWidth);
	graphics.DrawImage(m_pBkImage,destRect,REAL(m_nTitleHeight),REAL(m_pBkImage->GetHeight()-m_nFrameWidth),REAL(m_nFrameWidth),REAL(m_nFrameWidth),UnitPixel);

	destRect.X = REAL(rcWindow.right-m_nTitleHeight);
	destRect.Y = REAL(rcWindow.bottom - m_nTitleHeight);
	destRect.Width = REAL(m_nTitleHeight);
	destRect.Height = REAL(m_nTitleHeight);
	graphics.DrawImage(m_pBkImage,destRect,REAL(m_pBkImage->GetWidth()-m_nTitleHeight),REAL(m_pBkImage->GetHeight()-m_nTitleHeight),REAL(m_nTitleHeight),REAL(m_nTitleHeight),UnitPixel);

	destRect.X = 0;
	destRect.Y = REAL(rcWindow.Height() - m_nTitleHeight);
	destRect.Width = REAL(m_nTitleHeight);
	destRect.Height = REAL(m_nTitleHeight);
	graphics.DrawImage(m_pBkImage,destRect,0,REAL(m_pBkImage->GetHeight()-m_nTitleHeight),REAL(m_nTitleHeight),REAL(m_nTitleHeight),UnitPixel);


	//左边框
	destRect.X = 0;
	destRect.Y = REAL(m_nTitleHeight);
	destRect.Width = REAL(m_nFrameWidth);
	destRect.Height = REAL(rcWindow.Height() - 2*m_nTitleHeight);
	graphics.DrawImage(m_pBkImage,destRect,0,REAL(m_nTitleHeight),REAL(m_nFrameWidth),REAL(m_nFrameWidth),UnitPixel);


	//右边框
	destRect.X = REAL(rcWindow.Width() - m_nFrameWidth);
	destRect.Y = REAL(m_nTitleHeight);
	destRect.Width = REAL(m_nFrameWidth);
	destRect.Height = REAL(rcWindow.Height() - 2*m_nTitleHeight);
	graphics.DrawImage(m_pBkImage,destRect,REAL(m_pBkImage->GetWidth()-m_nFrameWidth),REAL(m_nTitleHeight),REAL(m_nFrameWidth),REAL(m_nFrameWidth),UnitPixel);


	//绘制标题栏
	destRect.X = 0;
	destRect.Y = 0;
	destRect.Width = REAL(rcWindow.Width());
	destRect.Height = REAL(m_nTitleHeight);
	graphics.DrawImage(m_pBkImage,destRect,50,0, REAL(m_pBkImage->GetWidth()-50),REAL(m_nTitleHeight),UnitPixel);	//中间
	graphics.DrawImage(m_pBkImage,0,0,0,0,50,m_nTitleHeight,UnitPixel);  //左边
	graphics.DrawImage(m_pBkImage,rcWindow.Width()-50,0,m_pBkImage->GetWidth()-50,0,50,m_nTitleHeight,UnitPixel); //右边

	if ( IsZoomed() )
		m_nMaximizeTopOffset = 3; 
	else
		m_nMaximizeTopOffset = 0;

	int xLOffset = m_nMaximizeTopOffset*2;
	int yLOffset = floor(m_nTitleHeight*1.0000/2) - 7;

	int xROffset = 0;
	int yROffset = floor(m_nTitleHeight*1.0000/2) - 7;

	//HICON hIcon = AfxGetApp()->LoadIcon(128);// GetIcon(FALSE);
	//if (hIcon)
	//{
	//	DrawIconEx(MemDC,xPos, yPos-2,hIcon,GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0, 0, DI_NORMAL);

	//	xPos += GetSystemMetrics(SM_CXSMICON);
	//	xPos += 5;
	//}

	//if ( m_pPPTImage != NULL )
	//{
	//	if ( IsZoomed() )
	//		m_nMaximizeTopOffset = 3; 
	//	else
	//		m_nMaximizeTopOffset = 0;

	//	int nImageXOffset = m_nMaximizeTopOffset*2 + 10;

	//	//graphics.DrawImage(m_pPPTImage, 1, 1, 0, 0, m_pPPTImage->GetWidth(), m_pPPTImage->GetHeight(), UnitPixel);
	//	//graphics.DrawImage(m_pPPTImage, PointF(REAL(m_nMaximizeTopOffset*2), REAL(m_nMaximizeTopOffset)));
	//	graphics.DrawImage(m_pPPTImage, nImageXOffset, m_nMaximizeTopOffset, 0, 0, m_pPPTImage->GetWidth(), m_pPPTImage->GetHeight(), UnitPixel);
	//	xLOffset += nImageXOffset;
	//	xLOffset += m_pPPTImage->GetWidth();
	//	yLOffset += m_nMaximizeTopOffset;
	//}

	//CString strText = _T(" ");
	////GetWindowText(strText);

	//if (!strText.IsEmpty())
	//{
	//	//绘制标题
	//	LOGFONT lfFont;
	//	memset(&lfFont, 0, sizeof(lfFont));
	//	lfFont.lfHeight		= -15;
	//	lfFont.lfWeight		|= FW_BOLD;
	//	lstrcpy(lfFont.lfFaceName, _T("微软雅黑"));
	//	Gdiplus::Font font(dc.GetSafeHdc(), &lfFont);

	//	StringFormat stringFormat;
	//	stringFormat.SetTrimming(StringTrimmingEllipsisCharacter);
	//	stringFormat.SetFormatFlags( StringFormatFlagsNoWrap);
	//	stringFormat.SetAlignment(StringAlignmentNear);
	//	stringFormat.SetLineAlignment(StringAlignmentNear);
	//	CStringW strTitle(strText);
	//	SolidBrush brush((ARGB)Color::White);
	//	RectF layoutRect;

	//	layoutRect.X=(Gdiplus::REAL)xLOffset;
	//	layoutRect.Y=(Gdiplus::REAL)yLOffset;
	//	int nWidth = rcWindow.Width() - TITLE_RIGHT_MIN_WIDTH - m_nAddLen;
	//	layoutRect.Width	= REAL( (nWidth > 0) ? nWidth : 1);//2015.11.11 cws
	//	layoutRect.Height	= REAL(m_nTitleHeight);
	//	graphics.SetTextRenderingHint(TextRenderingHintSystemDefault); 
	//	graphics.DrawString(strTitle, strTitle.GetLength(), &font, layoutRect, &stringFormat, &brush);

	//	//RectF boundRect;  
	//	//INT codePointsFitted	=0;  
	//	//INT linesFitted			=0; 
	//	//graphics.MeasureString(strTitle,strTitle.GetLength(),&font,layoutRect,NULL,&boundRect,&codePointsFitted,&linesFitted);

	//	//得到文本的宽度
	//	GraphicsPath graphicsPathObj;
	//	FontFamily fontfamily;
	//	font.GetFamily(&fontfamily);
	//	graphicsPathObj.AddString(strTitle,-1,&fontfamily,font.GetStyle(),font.GetSize(),PointF(0,0),&stringFormat);
	//	RectF rcBound;
	//	/// 获取边界范围
	//	graphicsPathObj.GetBounds(&rcBound);

	//	if (rcBound.Width <= rcWindow.Width() - TITLE_RIGHT_MIN_WIDTH - m_nAddLen)//2015.11.11 cws
	//		xLOffset += (int)rcBound.Width;
	//	else
	//		xLOffset += int(rcWindow.Width() - TITLE_RIGHT_MIN_WIDTH - 30 - m_nAddLen);//2015.11.11 cws

	//}

	//m_relX = xOffset;

	// use by relativeLayout
	xLOffset += int(rcWindow.Width() - TITLE_RIGHT_MIN_WIDTH - 30 - m_nAddLen);

	if ( xLOffset > m_nMaximizeTopOffset*2 )
		xLOffset	= m_nMaximizeTopOffset*2;

	int nLeftButtonToLeftMinWidth	= LEFT_BUTTON_TO_LEFT_MIN_WIDTH + m_nMaximizeTopOffset*2;  //加上边框的变化
	int nRightButtonToLeftMinWidth	= RIGHT_BUTTON_TO_LEFT_MIN_WIDTH;
	if (m_nAddLen > 0)
	{
		nRightButtonToLeftMinWidth += 73;//2015.11.11 cws
	}

	// 判断是否显示标题按钮图标
	BOOL bShow = (rcWindow.Width() >= SHOW_ICON_WINDOW_MIN_WIDTH) ? TRUE : FALSE;

	CRect rcPaint;
	dc.GetClipBox(&rcPaint);
	std::map<int,CDUIButton>::iterator iter;
	for (iter = m_TitleBtn.begin(); iter != m_TitleBtn.end(); iter++)
	{
		CDUIButton& dcControl = iter->second;
		CRect rcControl;

		dcControl.GetRect(&rcControl);
		dcControl.SetShowIcon(bShow);

		if (dcControl.IsRelativeLayout())
		{
			CRect rcOffset;
			dcControl.GetOffsetRect(&rcOffset);
			if (!dcControl.IsRightOffset())
			{
				rcOffset.OffsetRect(xLOffset, 0);

				if (rcOffset.left < nLeftButtonToLeftMinWidth)  //left btton to client min width
				{
					rcOffset.OffsetRect(nLeftButtonToLeftMinWidth-rcOffset.left, 0);
					nLeftButtonToLeftMinWidth += rcOffset.Width()+1;
				}
			}
			else
			{
				rcOffset.left	= rcWindow.right - rcOffset.left;
				rcOffset.right	= rcWindow.right - rcOffset.right;
				rcOffset.OffsetRect(-xROffset, 0);
			}
			rcControl.left	= rcOffset.left;
			rcControl.right	= rcOffset.right;
		}
		dcControl.SetRect(rcControl);

		// 判断当前按钮是否需要重绘
		if(!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
		{
			continue;
		}

		int nButtonOffsetX = dcControl.DrawButton(graphics);

		if (dcControl.IsRightOffset())
			xROffset += nButtonOffsetX;
	}


	dc.BitBlt(0,0,rcWindow.Width(),rcWindow.Height(),&MemDC,0,0,SRCCOPY);

	dc.SelectClipRgn(NULL);
}

void CSkinFrameWnd::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	if (bCalcValidRects)   
	{   
		CRect& rc	= (CRect&)lpncsp->rgrc[0];//get the client rectangle
		rc.top		+= m_nTitleHeight;   
		rc.left		+= m_nFrameWidth;   
		rc.bottom	-= m_nFrameWidth;
		rc.right	-= m_nFrameWidth; 

		if ( IsZoomed())
		{
			CRect rt;
			SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0);
			//int nTaskBarHight = GetSystemMetrics(SM_CYSCREEN) - rt.bottom;
			
			if ( rc.left < rt.right/2 && rc.bottom > rt.bottom)
			{
				rc.bottom = rt.bottom;
				rc.right -= m_nFrameWidth;
			}
		}

	}
}

LRESULT CSkinFrameWnd::OnNcHitTest(CPoint point)
{
	CRect rtButton;
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	CPoint pt = point;
	pt.x -=rcWindow.left;
	pt.y -= rcWindow.top;  

	int iButton = TButtonHitTest(pt, rtButton);
	if(iButton != -1)
	{
		switch(iButton)
		{
		case ID_CLOSE_BTN:
			{
				return HTCLOSE;
			}
			break;
		case ID_MAX_BTN:
			{

				return HTMAXBUTTON;
			}
			break;
		case ID_MIN_BTN:
			{
				return HTMINBUTTON;
			}
			break;
		}
	}
	else if (m_rcIcon.PtInRect(pt))
	{
		return HTSYSMENU;
	}


	return CFrameWndEx::OnNcHitTest(point);
}

void CSkinFrameWnd::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	 lpMMI->ptMinTrackSize.y = m_nTitleHeight + m_nFrameWidth;

	 int nMinWidth = 1024;
	 int nMinHight = 768;

	 int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	 int nScreenHight = GetSystemMetrics(SM_CYSCREEN);

	 if ( nScreenWidth > SHOW_ICON_WINDOW_MIN_WIDTH )		//当屏幕分辨率大于1100的时候，最小设置为1100
		 nMinWidth = SHOW_ICON_WINDOW_MIN_WIDTH;

	if ( nMinWidth > nScreenWidth )
		nMinWidth = nScreenWidth;
	
	if ( nMinHight > nScreenHight)
		nMinHight = nScreenHight;
		

	 if (lpMMI->ptMinTrackSize.x < nMinWidth)
		 lpMMI->ptMinTrackSize.x	= nMinWidth;
	 if (lpMMI->ptMinTrackSize.y < nMinHight)
		 lpMMI->ptMinTrackSize.y	= nMinHight;
	CFrameWnd::OnGetMinMaxInfo(lpMMI);
}

BOOL CSkinFrameWnd::OnNcActivate(BOOL bActive)
{
	 OnNcPaint();
	 return TRUE;
}

void CSkinFrameWnd::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CFrameWndEx::OnActivate(nState, pWndOther, bMinimized);

	OnNcPaint();
}

void CSkinFrameWnd::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	rcWindow.bottom = rcWindow.top + m_nTitleHeight;

	CPoint pt = CPoint(point.x-rcWindow.left, point.y-rcWindow.top);
    CRect rtButton;
	int iButton = TButtonHitTest(pt, rtButton);
	if(iButton != -1)
	{
		m_TitleBtn[iButton].LButtonDown();
		OnNcPaint();
		return;
	}

	if (rcWindow.PtInRect(point))
	{
		if (IsZoomed())
		{
			ShowWindow(SW_RESTORE);
		}
		else
		{
			ShowWindow(SW_MAXIMIZE);
		}

		OnNcPaint();
		return;
	}

	OnNcPaint();
}

void CSkinFrameWnd::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	CRect rtButton;
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	CPoint pt = point;
	pt.x -=rcWindow.left;
	pt.y -= rcWindow.top; 

	int iButton = TButtonHitTest(pt, rtButton);
	if(iButton != -1)
	{
		m_TitleBtn[iButton].LButtonDown();
		OnNcPaint(); 
		return;
	}

	//CMenu *pSysMenu = GetSystemMenu(FALSE);
	//int nCount		= pSysMenu->GetMenuItemCount();
	//UINT pID		= pSysMenu->GetMenuItemID(nCount-1);
	//pSysMenu->EnableMenuItem(pID, MF_DISABLED);

	//ModifyStyle(WS_MAXIMIZEBOX, 0); 
	//ModifyStyle(WS_MINIMIZEBOX, 0); 
	//
	//if(m_rcIcon.PtInRect(pt)) 
	//{
	//	CMenu *pSysMenu = GetSystemMenu(TRUE);
	//	if (pSysMenu)
	//	{
	//		pSysMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y,this, NULL);
	//	}
	//	return;
	//}
	//else 
	if(nHitTest >= HTLEFT && nHitTest <= HTBOTTOMRIGHT || nHitTest == HTCAPTION)
	{		
		CFrameWnd::OnNcLButtonDown(nHitTest, point);
		return;
	}

	CFrameWndEx::OnNcLButtonDown(nHitTest, point);
}

void CSkinFrameWnd::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
	CRect rtButton;
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	CPoint pt = point;
	pt.x -=rcWindow.left;
	pt.y -= rcWindow.top; 

	int iButton = TButtonHitTest(pt, rtButton);
	if(iButton != -1)
	{
		switch(iButton)
		{
		case ID_CLOSE_BTN:
			{
				((CMainFrame*)(AfxGetApp()->m_pMainWnd))->m_bCloseAll = TRUE;
				SendMessage(WM_SYSCOMMAND,SC_CLOSE,0);
			}
			break;
		case ID_MAX_BTN:
			{
				if (IsZoomed())
				{
					SendMessage(WM_SYSCOMMAND,SC_RESTORE,0);
					m_TitleBtn[iButton].MouseLeave();
				}
				else
				{
					SendMessage(WM_SYSCOMMAND,SC_MAXIMIZE,0);
					m_TitleBtn[iButton].MouseLeave();
				}
			}
			break;
		case ID_MIN_BTN:
			{
				SendMessage(WM_SYSCOMMAND,SC_MINIMIZE,0);
				m_TitleBtn[iButton].MouseLeave();
			}
			break;
		default:
			{
               m_TitleBtn[iButton].LButtonUp();
			}
			break;
		}
		OnNcPaint();
	}
	else
	{
		CFrameWndEx::OnNcLButtonUp(nHitTest, point);
	}
}

void CSkinFrameWnd::OnNcRButtonDown( UINT nHitTest, CPoint point )
{
	CMenu *pSysMenu = GetSystemMenu(FALSE);
	int nCount	= pSysMenu->GetMenuItemCount();
	UINT pID	= pSysMenu->GetMenuItemID(nCount-1);
	pSysMenu->EnableMenuItem(pID, MF_ENABLED);

	ModifyStyle(0, WS_MAXIMIZEBOX); 
	ModifyStyle(0, WS_MINIMIZEBOX); 

	CFrameWndEx::OnNcRButtonDown(nHitTest, point);
}

void CSkinFrameWnd::OnNcMouseMove(UINT nHitTest, CPoint point)
{ 	
	if (!m_bNCTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_NONCLIENT | TME_LEAVE | TME_HOVER;//增加离开和悬停的时间出来 2015.11.10 cws
		tme.dwHoverTime = 1;
		m_bNCTracking = _TrackMouseEvent(&tme);
	}

	if(nHitTest>=HTLEFT && nHitTest <= HTBOTTOMRIGHT || 
		nHitTest == HTCAPTION)
	{
		CFrameWnd::OnNcMouseMove(nHitTest, point);
	}

	CRect rtButton;
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	point.x -=rcWindow.left;
	point.y -= rcWindow.top;
	int iButton = TButtonHitTest(point, rtButton);
	if ( iButton != m_iXButtonHovering )
	{

		if(m_iXButtonHovering != -1)
		{
			m_TitleBtn[m_iXButtonHovering].MouseLeave();
			m_iXButtonHovering = -1;
		}
		if(iButton != -1)
		{		
			((CMainFrame*)(AfxGetApp()->m_pMainWnd))->ModifyTitleButtonStatus();

			m_iXButtonHovering = iButton;
			m_TitleBtn[m_iXButtonHovering].MouseHover();

		}

		OnNcPaint();
	}
	else if ( iButton != -1 )
	{
		m_TitleBtn[m_iXButtonHovering].MouseMouse();
	}

	//OnNcPaint();
}

void CSkinFrameWnd::OnNcMouseHover(UINT nFlags, CPoint point)
{

	m_bNCHovering = TRUE;
	CRect rtButton;
	CRect rcWindow;
	GetWindowRect(&rcWindow);	
	CFrameWndEx::OnNcMouseHover(nFlags, point);
}

void CSkinFrameWnd::OnNcMouseLeave()
{
	m_bNCTracking = FALSE;
	m_bNCHovering = FALSE;

	if(m_iXButtonHovering != -1)
	{
		m_TitleBtn[m_iXButtonHovering].MouseLeave();
		m_iXButtonHovering = -1;
	}

	OnNcPaint(); 
	CFrameWndEx::OnNcMouseLeave();
}

void CSkinFrameWnd::OnSize(UINT nType, int cx, int cy)
{
	CFrameWndEx::OnSize(nType, cx, cy);
	
	if ( nType == SIZE_MAXIMIZED || nType == SIZE_MAXSHOW  || IsZoomed())
	{
		if ( m_pBtnRestore != NULL )
		{
			m_pBtnRestore->SetBKImage(m_WindowBtnImage[2], 3);
			//m_pBtnRestore->SetTipText(_T("还原"));
		}
			
		m_nMaximizeTopOffset	= 0 ;//3;

		if (m_Rgn.GetSafeHandle())
		{
			m_Rgn.DeleteObject();
		}

		CRect rc;
		GetWindowRect(&rc); //获得窗口矩形
		rc.OffsetRect(-rc.left, -rc.top);

		//判断系统版本
		OSVERSIONINFO osVer;
		osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		::GetVersionEx( &osVer );
		if( (osVer.dwMajorVersion == 5) && (osVer.dwMinorVersion == 1) )//Windows XP 5.1
		{
			m_Rgn.CreateRoundRectRgn(rc.left, rc.top, rc.right, rc.bottom, 0, 0); //根据窗口矩形创建一个圆角矩形	
		}
		else
		{
			m_Rgn.CreateRoundRectRgn(rc.left+(m_nFrameWidth*2)+1, rc.top+(m_nFrameWidth*2)+1, rc.right-(m_nFrameWidth*2)-1, rc.bottom-(m_nFrameWidth*2)-1, 0, 0); //根据窗口矩形创建一个圆角矩形
		}

		SetWindowRgn(m_Rgn, TRUE); //根据圆角矩形指定的区域改变窗口的形状
	}
	else if (nType != SIZE_MINIMIZED && nType != SIZE_MAXHIDE )
	{
		if ( m_pBtnRestore != NULL )
		{
			m_pBtnRestore->SetBKImage(m_WindowBtnImage[1], 3);
			//m_pBtnRestore->SetTipText(_T("最大化"));
		}
		
		if (m_Rgn.GetSafeHandle())
		{
			m_Rgn.DeleteObject();
		}

		CRect rc;
		GetWindowRect(&rc); //获得窗口矩形
		rc.OffsetRect(-rc.left, -rc.top);

		m_Rgn.CreateRoundRectRgn(rc.left, rc.top, rc.right, rc.bottom, 0, 0); //根据窗口矩形创建一个圆角矩形
		SetWindowRgn(m_Rgn, TRUE); //根据圆角矩形指定的区域改变窗口的形状
		m_nMaximizeTopOffset	= 0;

	}
	else
	{
		m_nMaximizeTopOffset	= 0;
	}


	CRect rcWnd;
	GetWindowRect(&rcWnd);
	rcWnd.OffsetRect(-rcWnd.left, -rcWnd.top);


	std::map<int,CDUIButton>::iterator iter;
	for (iter = m_TitleBtn.begin(); iter != m_TitleBtn.end(); iter++)
	{
		CDUIButton* dcControl = &iter->second;

		CRect rcControl;
		CRect rcOffset;
		dcControl->GetRect(&rcControl);
		dcControl->GetOffsetRect(&rcOffset);

		if ( dcControl->IsRightOffset() )
		{
			rcControl.left	= rcWnd.right - rcOffset.left;
			rcControl.right = rcWnd.right - rcOffset.right;

			//if ( dcControl->IsRelativeLayout() )
			//{
			//	rcControl.left	-= m_relX;
			//	rcControl.right -= m_relX;
			//}

			if ( nType == SIZE_MAXIMIZED || IsZoomed())
			{
				rcControl.left		-= m_nFrameWidth;
				rcControl.right		-= m_nFrameWidth;
			}
		}
		//else if ( dcControl->IsRelativeLayout() )
		//{
		//	rcControl.left	= rcOffset.left + m_relX;
		//	rcControl.right = rcOffset.right + m_relX;
		//}

		if ( nType == SIZE_MAXIMIZED || IsZoomed())  // zoomed windows rect outside screen
		{
			rcControl.top		= rcOffset.top + m_nFrameWidth;
			rcControl.bottom	= rcOffset.bottom + m_nFrameWidth;
		}
		else
		{
			rcControl.top		= rcOffset.top;
			rcControl.bottom	= rcOffset.bottom;
		}

		dcControl->SetRect(&rcControl);
	}

	if (nType == SIZE_MAXIMIZED||
		nType == SIZE_RESTORED)
	{
		OnNcPaint();
	}
}

void CSkinFrameWnd::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID != SC_CLOSE)
	{
		OnNcPaint();
	}

	CFrameWndEx::OnSysCommand(nID, lParam);
}

LRESULT CSkinFrameWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == 0x125)
	{
		OnNcPaint();
	}

	if (message ==0x00AE||//WM_NCUAHDRAWCAPTION
		message==0x00AF)//WM_NCUAHDRAWFRAME)
	{
		OnNcPaint();
		return 0;
	} 

	//OnNcPaint(); 
	return CFrameWndEx::WindowProc(message, wParam, lParam);
}

BOOL CSkinFrameWnd::OnNcCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (!CFrameWndEx::OnNcCreate(lpCreateStruct))
		return FALSE;

	CRect rcWnd;
	GetWindowRect(&rcWnd);
	rcWnd.OffsetRect( -rcWnd.left, -rcWnd.top);

	CRect rcMask;
	rcMask.left	= 150;
	rcMask.top	= 0;
	rcMask.right = rcMask.left - 150;
	rcMask.bottom = rcMask.top + m_nTitleHeight;
	AddTitleButton(rcMask, NULL, 3, ID_MASK_SYSTEM_BTN, NULL, TRUE, TRUE);
	SetTitleButtonEnabled(ID_MASK_SYSTEM_BTN, FALSE);

	//系统按钮
	CRect rcControl;
	rcControl.left	= 40;
	rcControl.top	= 10;
	rcControl.right = rcControl.left - 30;
	rcControl.bottom = rcControl.top + 30;
	CDUIButton* pBtn = AddTitleButton(rcControl, m_WindowBtnImage[3], 3, ID_CLOSE_BTN, NULL, TRUE, TRUE);
	//pBtn->SetTipText(_T("关闭"));
	
	rcControl.OffsetRect(32, 0);
	m_pBtnRestore = AddTitleButton(rcControl, m_WindowBtnImage[1], 3, ID_MAX_BTN, NULL, TRUE, TRUE);
	//m_pBtnRestore->SetTipText(_T("还原"));

	rcControl.OffsetRect(32, 0);
	pBtn = AddTitleButton(rcControl, m_WindowBtnImage[0], 3, ID_MIN_BTN, NULL, TRUE, TRUE);
	//pBtn->SetTipText(_T("最小化"));

	return TRUE;
}

//往对话框添加一个按钮
CDUIButton* CSkinFrameWnd::AddTitleButton(LPRECT lpRect, Image *pImage, UINT nCount /*= 3*/ , UINT nID/* = 0*/, LPCTSTR lpszCaption/*=NULL*/, BOOL bHorizontal/* = TRUE*/, BOOL bRightOffset/* = FALSE*/, BOOL bRelativeLayout /* = FALSE*/)
{
	CDUIButton dcControl;
	dcControl.CreateButton(lpszCaption, lpRect, this, pImage, nCount, nID, bHorizontal, bRightOffset, bRelativeLayout);
	m_TitleBtn[nID] = dcControl;  	 
	return &m_TitleBtn[nID];
}

BOOL CSkinFrameWnd::SetTitleButtonCaption( UINT nID, LPCTSTR lpszCaption )
{
	std::map<int,CDUIButton>::iterator iter;
	iter = m_TitleBtn.find(nID);
	if ( iter != m_TitleBtn.end() )
	{
		if ( lpszCaption )
			iter->second.SetCaption(lpszCaption);
		else
			iter->second.SetCaption(_T(""));

		OnNcPaint();
		return TRUE;
	}

	return FALSE;
}

BOOL CSkinFrameWnd::SetTitleButtonEnabled( UINT nID, BOOL bEnabled )
{
	std::map<int,CDUIButton>::iterator iter;
	iter = m_TitleBtn.find(nID);
	if ( iter != m_TitleBtn.end() )
	{
		iter->second.EnableButton(bEnabled);

		OnNcPaint();
		return TRUE;
	}

	return FALSE;
}

int CSkinFrameWnd::TButtonHitTest(CPoint point, CRect& rtButton)
{
	std::map<int,CDUIButton>::iterator iter;
	for (iter = m_TitleBtn.begin(); iter != m_TitleBtn.end(); iter++)
	{
		CDUIButton& dcControl = iter->second;
		if(dcControl.PtInButton(point))
		{
			dcControl.GetRect(&rtButton);
			return dcControl.GetControlId();
		}
	}
	return -1;
} 