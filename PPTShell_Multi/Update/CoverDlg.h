#pragma once


// CCoverDlg dialog

class CCoverDlg : public CDialog
{
	DECLARE_DYNAMIC(CCoverDlg)

public:
	CCoverDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCoverDlg();

// Dialog Data
	enum { IDD = IDD_COVER_DIALOG };
public:
	int			m_nCloseBtnStatus; //0 默认 1点击 2热点
	int			m_nSmallBtnStatus; //0 默认 1点击 2热点
	CGdiPlusPngResource *m_pCloseBtn;
	CGdiPlusPngResource *m_pSmallBtn;
	CGdiPlusPngResource *m_pLoadPng;

	HANDLE m_hThread;
	bool m_bThread;

	CRect		m_RctWnd;
	CRect		m_RctCloseBtn;
	CRect		m_RctSmallBtn;
	CRect		m_RctLoading;

	char		m_szSrcPath[MAX_PATH * 2 + 1];
	char		m_szDestDir[MAX_PATH * 2 + 1];
	char		m_szNdCloudPath[MAX_PATH * 2 + 1];
	char		m_szFailPath[MAX_PATH * 2 + 1];
	char		m_szRootDir[MAX_PATH * 2 + 1];
	char		m_szTempPackageDir[MAX_PATH * 2 + 1];

	string		m_strMessage;
	CRect		m_RctMessage;

	void		SetStrMessage(LPCTSTR pszMessage);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	static unsigned __stdcall CoverThread(void* pParam);
	static unsigned __stdcall ShowLoadGif(void* pParam);

	BOOL		ExtractFile(vector<UPDATEDATA>& listBak);
	BOOL		FindFile(char* szRootDir);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnMsgProc(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMessageChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNcHitTest(CPoint point);

	DECLARE_MESSAGE_MAP()
};
