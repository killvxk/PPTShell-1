#pragma once

#include <GdiPlus.h>
using namespace Gdiplus;

enum DRAW_STATUS
{
	DS_NORMAL = 0,
	DS_HOVERING,
	DS_PRESSED,
	DS_DISABLED,

	DS_TOTAL,
};

enum BUTTON_MOUSE_STATUS
{
	BMS_HOVER = 1,
	BMS_MOUSE,
	BMS_LEAVE,
};


#define		BUTTON_MAX_WIDTH	(250)
#define		WINDOW_MIN_WIDTH	(1200)


#define		TEXT_OFFSET			(2)
//#define  BUTTON_MAX_WIDTH	(200)

class CDUIButton
{
public:
	// 标准构造函数
	CDUIButton();

	virtual ~CDUIButton();

	//修改按钮文字
	void SetCaption(CString strCaption);

	//是否靠右
	BOOL IsRightOffset()	{return m_bRightOffset;}

	//是否相对布局
	BOOL IsRelativeLayout()	{return m_bRelativeLayout;}

	//得到相对布局偏移
	void GetOffsetRect(LPRECT lpRect){*lpRect = *m_rcOffsetRect;}

	// 设置按钮矩形位置
	void SetRect(LPCRECT lpRect);
	// 获取按钮矩形位置
	void GetRect(LPRECT lpRect){*lpRect = *m_rcRect;}
	// 检测指定点是否处于按钮内
	BOOL PtInButton(CPoint& point){return m_rcRect.PtInRect(point);}
	// 绘制按钮
	virtual int DrawButton( Gdiplus::Graphics&  graphics, CSize offset = CSize(0, 0));
	// 创建按钮
	void CreateButton(LPCTSTR lpszCaption, LPRECT lpRect, CWnd* pParent, Image* pImage, UINT nCount = 4, UINT nID = 0, BOOL bHorizontal = TRUE, BOOL bRight = FALSE, BOOL bRelativeLayout = FALSE);
	//void CreateButton(LPCTSTR lpszCaption, LPRECT lpRect, CWnd* pParent, Image* pImage, UINT nCount = 4, UINT nID = 0, BOOL bHorizontal = TRUE, BOOL bRight = FALSE, BOOL bRelativeLayout = FALSE);
	//设置按钮状态图片
	void  SetBKImage(Image *pImage, UINT nCount = 4, BOOL bHorizontal = TRUE, BOOL bRedraw = FALSE);
	
	//设置按钮图标信息
	void  SetIconInfo(Image *pIcon, UINT nCount = 1, CPoint ptPos = CPoint(5,-1), BOOL bHorizontal = TRUE, BOOL bRedraw = FALSE);
	void  SetIconInfo(Image *pIcon, UINT nCount, CSize cSize, CPoint ptPos = CPoint(5,-1), BOOL bHorizontal = TRUE, BOOL bRedraw = FALSE);
	void  SetStatusIconInfo(Image *pIcon, UINT nStatus, CSize cSize, CPoint ptPos = CPoint(5,-1));
	
	//设置图标是否显示
	void  SetShowIcon( BOOL bShow );

	//设置提示图标信息
	void  SetTipIconInfo(Image *pIcon, UINT nCount = 1, CPoint ptPos = CPoint(5,-1), BOOL bHorizontal = TRUE, BOOL bRedraw = FALSE);
	void  SetTipIconInfo(Image *pIcon, UINT nCount, CSize cSize, CPoint ptPos = CPoint(5,-1), BOOL bHorizontal = TRUE, BOOL bRedraw = FALSE);
	void  SetStatusTipIconInfo(Image *pIcon, UINT nStatus, CSize cSize, CPoint ptPos = CPoint(5,-1));

	void  SetStatusTextInfo(UINT nStatus, DWORD dwColor);

	void  SetAutoSize(BOOL bAutoSize)	{	m_bAutoSize = bAutoSize;	};
	void  SetMaxWidth(int nWidth = BUTTON_MAX_WIDTH )	{	m_nMaxWidth = nWidth;	};

	//设置为九宫格切割
	void  SetDivison(BOOL bDivison)		{	m_bDivision = bDivison;		};
	void  SetDivsionRect(CRect rect);
	// 设置按钮状态
	void EnableButton(BOOL bEnable)
	{
		m_bDisabled = !bEnable;
		m_bHovering = FALSE;
		m_bPressed  = FALSE;
		m_pParent->InvalidateRect(m_rcRect);
	}

	void SetCheckBox(BOOL bCheck = TRUE)
	{
		m_bIsCheckBox = bCheck;
	}

	BOOL IsChecked()	{	return m_bCheck;	}

	// 设置按钮选定状态
	void SetCheck(BOOL bCheck)
	{
		m_bCheck = bCheck;
		m_pParent->InvalidateRect(m_rcRect);
	}
	// 获取按钮Id
	UINT GetControlId(){return m_nID;};
	// 触发按钮事件
	void DoCommand()
	{
		if(!m_bDisabled)
		{
			m_bCheck = !m_bCheck;
			m_pParent->InvalidateRect(m_rcRect);
			m_pParent->PostMessage(WM_COMMAND, m_nID);
		}
	}

	// 鼠标按下按钮事件
	void LButtonDown()
	{
		if(!m_bDisabled)
		{
			m_bPressed = TRUE;
			m_pParent->InvalidateRect(m_rcRect);
		}
	};
	// 鼠标放下按钮事件
	void LButtonUp()
	{
		if(!m_bDisabled)
		{
			if(m_bPressed == TRUE)
			{
				m_bCheck = !m_bCheck;
				m_bPressed = FALSE;
				m_pParent->InvalidateRect(m_rcRect);
				m_pParent->PostMessage(WM_COMMAND, m_nID);
			}
		}
	}
	// 鼠标进入按钮区域事件
	void MouseHover()
	{
		if( !m_bDisabled )
		{
			m_bHovering = TRUE;
			m_pParent->InvalidateRect(m_rcRect);
			ShowToolTip();
			m_pParent->PostMessage(WM_TITLE_MOUSE, m_nID, BMS_HOVER);
		}
	}

	// 鼠标按钮区域移动事件
	void MouseMouse()
	{
		if( !m_bDisabled && m_bHovering )
		{
			MoveToolTip();
			m_pParent->PostMessage(WM_TITLE_MOUSE, m_nID, BMS_MOUSE);
		}
	}

	// 鼠标离开按钮区域事件
	void MouseLeave()
	{
		if( !m_bDisabled )
		{
			m_bPressed = FALSE;
			m_bHovering = FALSE;
			m_pParent->InvalidateRect(m_rcRect);
		}
		HideToolTip();
		m_pParent->PostMessage(WM_TITLE_MOUSE, m_nID, BMS_LEAVE);
	}

	// 设置悬浮提示文字
	void SetTipText(LPCTSTR lpstrTipText)
	{
		m_strTipText = lpstrTipText;
	}

	CWnd* GetParentWnd() { return m_pParent;}

protected:
	//
	void ShowToolTip();
	void MoveToolTip();
	void HideToolTip();
	//用颜色绘背景
	void DrawColor(Gdiplus::Graphics&  graphics, const RECT& rc, DWORD color);
	// 按钮矩形位置
	CRect	m_rcRect;
	// 按钮偏移矩形位置
	CRect	m_rcOffsetRect;
	//四个角范围
	CRect	m_rcCornerRect;
	// 标识鼠标进入按钮区域
	BOOL	m_bHovering;
	// 标识已经点击按钮
	BOOL	m_bPressed;
	// 标识按钮已经被禁用
	BOOL	m_bDisabled;
	// 标识按钮已经被选中
	BOOL	m_bCheck;
	// 标识是否是CheckBox
	BOOL	m_bIsCheckBox;
	// 标识按钮是否以右计算偏移
	BOOL	m_bRightOffset;
	// 相对布局
	BOOL	m_bRelativeLayout;

	BOOL	m_bAutoSize;

	//按钮按9宫格分割
	BOOL	m_bDivision;

	// 按钮图片
	Image*	m_pImage;
	//状态图片按钮数组
	Image*	m_pDrawImage[4];
	RectF	m_rtImage[4];
	//状态文字颜色数组
	DWORD	m_dwColor[4];

	// 按钮图片中包含的图片个数
	UINT	m_nImageCount;

//============================================
//				小图标
//============================================
//图标(一个按钮可以给它设置一个小图标)
	Image*	m_pIcon;
	Image*	m_pDrawIcon[4];
	RectF	m_rtIcon[4];

	// 按钮图片中包含的图片个数
	UINT	m_nIconCount;
	//图标左上角位置
	CPoint	m_ptIcon;
	//图标的大小
	CSize	m_csizeIcon;

	BOOL	m_bShowIcon;

//============================================
//				提示图标
//============================================
	Image*	m_pTipIcon;
	Image*	m_pDrawTipIcon[4];
	RectF	m_rtTipIcon[4];

	// 按钮图片中包含的提示图标个数
	UINT	m_nTipIconCount;
	//图标左上角位置
	CPoint m_ptTipIcon;
	//图标的大小
	CSize  m_csizeTipIcon;

	// 按钮Id号
	int		m_nID;
	// 按钮父窗口
	CWnd*	m_pParent;
	//按钮标题文本
	CString m_strCaption;

// 提示相关
	
	TOOLINFO	m_sToolTip;
	HWND		m_hWndToolTip;
	CString		m_strTipText;
	POINT		m_ToolLastPos;


	//按钮宽度
	int		m_nMaxWidth;
	int		m_nLeftMaxLen;//2015.11.11 cws


		
	//Gdiplus::Graphics  m_graphics;
	//文本颜色
	//Color m_colorNormal;
	//Color m_colorDisable;
	//Color m_colorPressed;
	//Color m_colorHovering;
};
