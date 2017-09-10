#pragma once
#include "stdafx.h"
#include "DUICommon.h"   

#define MAGICPEN_TIMER_ID 90501
#define WM_SHOWMAGICPEN                      0x0218
#define WM_HIDEMAGICPEN                      0x0219
#define WM_MOVEMAGICPEN						 0x021A
class CMagicPen_Main : public WindowImplBase
{
public:
	CMagicPen_Main(void);
	~CMagicPen_Main(void);

	UIBEGIN_MSG_MAP
	UIEND_MSG_MAP
 
enum DrawState
	{
		DRAWRECT_START,
		DRAWRECT_DRAWING,
		DRAWRECT_END,		
	};
public:
	LPCTSTR GetWindowClassName() const;	
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();

 
	virtual void InitWindow();
	//virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled); 

public:
	//提供给手机调用2016.03.08	
	void	DrawRoundRect(int nState, int nX, int nY);//绘制矩形框 nState =0,开始绘制，nState =1绘制过程, nState = 2绘制结束
  
	void	ShowLocalWindows();//显示
	void	CloseLocalWindows();//隐藏
	void	SetControlMode(int nMode = 0) { m_nControlMode = nMode;}
	int		GetControlMode(){ return m_nControlMode; }
	bool	IsShowing() {return m_bIsShowing;}
	bool	IsMagnifierShowing() { return IsWindow(m_hMagicPenAltha); }	// 放大后的图片窗口是否已显示
	void	TopMostView();//显示
private:  
	bool IsMagicPenMode(){ return m_bMagicPenMode; }   
	bool OnCloseWndEvent(void* pObj);
	void StartDrawRoundRect(int nX, int nY);
	void EndDrawRoundRect(int nX, int nY);
	void DrawingRoundRect(int nX, int nY);
	bool m_bMagicPenMode;  
	CBitmap* m_pbmpScreen;
	CBitmap* m_pbmpCopyScreen;

	CBitmap* m_pbmpToolTip;
	CPoint m_ptStart;
	CPoint m_ptEnd;
	int m_nCx;
	int m_nCy;
	bool m_bHaveShow;//已经显示了放大镜 
	HWND m_hMagicPenAltha;//透明图层
	DWORD m_nStartTimeTick;
	int m_nControlMode;		// 操作来源；0-手机控制；1-PC端控制;2-操作结束
	bool  m_bIsShowing;
protected:  
	HWND					m_hWndParent; 
	CVerticalLayoutUI*		m_pRoundRect;
	CLabelUI*				m_pShowToolTip;
};

 
typedef Singleton<CMagicPen_Main>		MagicPen_MainUI;