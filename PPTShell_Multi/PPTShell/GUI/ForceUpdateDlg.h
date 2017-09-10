#pragma once
#include "DUI/ForceUpdate.h"

// CForceUpdateDlg dialog

class CForceUpdateDlg : public CDialog
{
	DECLARE_DYNAMIC(CForceUpdateDlg)

public:
	CForceUpdateDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CForceUpdateDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_FORCEUPDATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	CForceUpdate m_ForceUpdateDUI;

};
