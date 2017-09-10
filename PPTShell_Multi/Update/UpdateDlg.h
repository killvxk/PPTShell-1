// UpdateDlg.h : header file
//

#if !defined(AFX_UPDATEDLG_H__82D25975_2F78_4C26_AB7A_E446B3544501__INCLUDED_)
#define AFX_UPDATEDLG_H__82D25975_2F78_4C26_AB7A_E446B3544501__INCLUDED_

#include "resource.h"
#include "BmpProgCtrl.h"
#include "AutoUpdate.h"
#include <map>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CUpdateDlg dialog
struct ModeInfo 
{
	char szModeName[MAX_PATH];
	string strPathUrl;
	string strCompressPath;
};

typedef std::vector<ModeInfo>				VEC_MODEINFO;
typedef std::map<int, VEC_MODEINFO>			MAP_MODEINFO;
typedef std::map<int, VEC_MODEINFO>::iterator MAP_MODEINFO_IT;

struct State 
{
	bool bEnd;
	UINT64 nComplete;
	UINT64 nTotal;
};

struct PatchType
{
	bool bAddFile;
	UINT unHashContent;
	UINT64 unFileLength;
	UINT unFileNameSize;
};

struct PatchInfo
{
	PatchType pt;
	char      szFileName[MAX_PATH];
};

enum E_ACTION;
#define PATCH_FILE_DIR "temp"

class CUpdateDlg : public CDialog
{
// Construction
public:
	CUpdateDlg(CWnd* pParent = NULL);	// standard constructor

public:
	CFont		m_font;
	CRect		m_RctWnd;
	CRect		m_RctTxt;
	CRect		m_RctCloseBtn;
	CString		m_strTxt;
	int			m_nAll;
	int			m_nDown;
	bool		m_bPipeOpen;
	HANDLE		m_hInPipe;
	HANDLE		m_hOutPipe;

	bool		m_bSelfUpdate;
	int			m_nCloseBtnStatus; //0 默认 1点击 2热点
	CGdiPlusPngResource *m_pCloseBtn;

	AutoUpdate_Param	m_updateParam;


	std::map<string, State> m_mapRevolve;
// Dialog Data
	//{{AFX_DATA(CUpdateDlg)
	enum { IDD = IDD_UPDATE_DIALOG };
	CBmpProgCtrl	m_progress;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUpdateDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
public:
	bool PatchStart(const char* szApplicationPath, vector<string>& vecXmlName);
	bool DownloadPatch(vector<string>& vecXmlName, vector<string>& vecDownFilePath);
	bool GetDownCountFromXmlFiles(std::vector<string> vecXmls);
	bool XmlParse(const char* szXmlName, MAP_MODEINFO& mapMode);
	bool DownloadFile(const char* szXmlName, string& strDownFilePath, bool bIsRedownload = false);
	bool ParseDecompressTotalSize(std::vector<string>& vecDownFilePath);
	bool DecompressPatch(vector<string>& vecDownFilePath, vector<string>& vecDecompDir);
	bool PatchListParse(const char* szDecompressDir, map<string, PatchType>& mapPatchInfo, string& strPatchPath);

	bool CoverPatch(string strApplicationPath, vector<string>& vecDecompDir, vector<string>& vecRedownload);
	bool RedownloadFileList(string strApplicationPath, 
		map<string, map<string, PatchType>>& mapPatchs,
		vector<string>& vecRedownload);
	bool CopyPatchList(string strApplicationPath, map<string, map<string, PatchType>>& mapPatchs);

	void InsertVecRedownload(string strPath, vector<string>& vecRedownload);
	void RemoveVecRedownload(string strPath, vector<string>& vecRedownload);

// 	void InsertVecCover(string strPath, vector<string>& vecCover);
// 	void RemoveVecCover(string strPath, vector<string>& vecCover);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CUpdateDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	
	afx_msg void OnNcMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nFlags, CPoint point);

	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnMsgProc(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void		SetUpdateType(bool b) {m_bSelfUpdate = b ;}
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UPDATEDLG_H__82D25975_2F78_4C26_AB7A_E446B3544501__INCLUDED_)
