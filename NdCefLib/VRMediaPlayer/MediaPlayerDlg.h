#pragma once
#include "wmpplayer4.h"
#include "wmpcontrols.h"
#include "wmpsettings.h"
#include "wmpplaylist.h"
#include "wmpmedia.h"


/////////////////////////////////////////////////////////////////////////////
// CMediaPlayerDlg dialog

class CMediaPlayerDlg : public CDialog
{
// Construction
public:
	CMediaPlayerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMediaPlayerDlg)
	enum { IDD = IDD_MEDIAPLAYER_DIALOG };
	CWMPPlayer4	m_player;
	CWMPControls m_control;
	CWMPSettings m_setting;
	CWMPMedia m_media;
	CString m_strFileName;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMediaPlayerDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMediaPlayerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnFullscreen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
