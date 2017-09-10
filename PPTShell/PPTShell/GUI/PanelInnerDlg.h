#pragma once
#include "Resource.h"

// CPanelInnerDlg 对话框

enum eInnerIndex
{
	eInner_Navgate,
	eInner_Group,
	eInner_Item,
};

class IInnerable{

public:
	virtual	int			GetInnerIndex()				= 0;
	virtual	int			GetInnerWidth()				= 0;
	virtual	void		SetInnerWidth(int nWidth)	= 0;
	virtual CWindowWnd* GetWindowWnd()				= 0;

};


class CPanelInnerDlg : 
	public CDialog,
	public IInnerable
{
	DECLARE_DYNAMIC(CPanelInnerDlg)

public:
	CPanelInnerDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPanelInnerDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_MAIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage( MSG* pMsg );

	DECLARE_MESSAGE_MAP()
	afx_msg virtual void OnSize(UINT nType, int cx, int cy);

public:
	static CDockablePane*	m_MainPanel;
	static void				AdjustMainPanel();

	

};
