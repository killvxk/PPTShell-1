
// 101PPTDesktopAlertDlg.h : header file
//

#pragma once


// CMy101PPTDesktopAlertDlg dialog
class CMy101PPTDesktopAlertDlg : public CDialog
{
// Construction
public:
	CMy101PPTDesktopAlertDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MY101PPTDESKTOPALERT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	CPoint m_ptPopup;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnClosePopup(WPARAM,LPARAM);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
};
