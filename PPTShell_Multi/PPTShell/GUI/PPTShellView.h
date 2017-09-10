
// PPTShellView.h : interface of the CPPTShellView class
//


#pragma once
#include "DUI/UntitledWindow.h"


class CPPTShellView : public CView
{
protected: // create from serialization only
	CPPTShellView();
	DECLARE_DYNCREATE(CPPTShellView)

// Attributes
public:
	CPPTShellDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CPPTShellView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnActivateFrame(UINT nState, CFrameWnd* pDeactivateFrame);

	static DWORD WINAPI InitThread(LPARAM lParam);
	BOOL ClosePPT2013TipDialogThread();
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	

public:
	LRESULT OnCreateOleInView(WPARAM wParam, LPARAM lParam);
	LRESULT OnShowGuide(WPARAM wParam, LPARAM lParam);
	void	ShowUntitledWindow(bool bShow);

	void	CreateUntitleWindow();
	//bool	IsUntitleWindowShow(){if(m_pUntitledWnd!=NULL){ return ::IsWindowVisible(m_pUntitledWnd->GetHWND());}else return false;};

	BOOL	IsUntitledWindowShow()				{ return m_bUntitledWindowShow;		}
	void	SetUntitledWindowShow(bool bShow)	{ m_bUntitledWindowShow = bShow;	}

protected:
	CUntitledWindow*	m_pUntitledWnd;
	BOOL				m_bFirst;
	BOOL				m_bUntitledWindowShow;
};

#ifndef _DEBUG  // debug version in PPTShellView.cpp
inline CPPTShellDoc* CPPTShellView::GetDocument() const
   { return reinterpret_cast<CPPTShellDoc*>(m_pDocument); }
#endif

