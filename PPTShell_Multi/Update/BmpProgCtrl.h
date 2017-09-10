// BmpProgCtrl.h : header file
//

#if !defined(AFX_BMPPROGCTRL_H__612712DC_DE51_4C23_95D3_A0B7DF296A42__INCLUDED_)
#define AFX_BMPPROGCTRL_H__612712DC_DE51_4C23_95D3_A0B7DF296A42__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBmpProgCtrl : public CStatic
{
// Construction
public:
	CBmpProgCtrl();
	virtual ~CBmpProgCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBmpProgCtrl2)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetRange(int nLower, int nUpper);
	void GetRange(int &lower,int &upper);
	int SetPos(int nPos);
	int GetPos();
//	int SetStep(int nStep);
//	int StepIt();
//	int OffsetPos(int nPos);
//	void SetImage(CString BackImagePath,CString ProgressImagePath);
//	void SetImage(UINT nBkIDResource, UINT nProgressIDResource);
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CBmpProgCtrl2)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	int m_nLower;
	int m_nUpper;
	int m_nPos;
	CBitmap m_BkBmp;
	CBitmap m_PerBmp;
	CDC* m_pDC;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BMPPROGCTRL_H__612712DC_DE51_4C23_95D3_A0B7DF296A42__INCLUDED_)
