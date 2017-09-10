#pragma once

#include "DUIButton.h"
#include <map> 
using namespace std;
/*
框架窗口非客户区自绘类
供大家学习参考
Author: 李其宋
Date:2013-10-25
*/


// CSkinFrameWnd frame

class CSkinFrameWnd : public CFrameWndEx
{
	DECLARE_DYNCREATE(CSkinFrameWnd)
protected:
	CSkinFrameWnd();           // protected constructor used by dynamic creation
	virtual ~CSkinFrameWnd();
     CDUIButton* AddTitleButton(LPRECT lpRect, Image *pImage, UINT nCount = 3 , UINT nID = 0, LPCTSTR lpszCaption = NULL, BOOL bHorizontal = TRUE, BOOL bRightOffset = FALSE, BOOL bRelativeLayout = FALSE);
	 BOOL		 SetTitleButtonCaption(UINT nID, LPCTSTR lpszCaption);
	 BOOL		 SetTitleButtonEnabled( UINT nID, BOOL bEnabled );
	 BOOL		 SetAllTitleButtonState();
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNcPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnNcMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnNcMouseLeave();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam); 
	afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	 int		TButtonHitTest(CPoint point, CRect& rtButton);  

protected:
	int			m_nTitleHeight;
	int			m_nFrameWidth;
	int			m_nMaximizeTopOffset;
	
	int			m_relX;			//水平相对布局偏移

	int			m_nScreenWidth;
	int			m_nScreenHight;

	Image*		m_pBkImage;
	Image*		m_pPPTImage;
	
	//标题按钮列表
	std::map<int,CDUIButton> m_TitleBtn;
	int    m_iXButtonHovering;
	BOOL   m_bNCHovering;
	BOOL   m_bNCTracking;
	CRgn   m_Rgn;
	//三个标题按钮图片
	vector<Image*>	m_WindowBtnImage;
	CDUIButton*		m_pBtnRestore;
	CRect  m_rcIcon;

	int	   m_nAddLen;//窗口增加大小

};


