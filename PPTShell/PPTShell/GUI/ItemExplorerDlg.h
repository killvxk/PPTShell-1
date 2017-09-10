#pragma once
#include "PanelInnerDlg.h"

// CItemExplorerDlg 对话框

class CItemExplorerDlg : public CPanelInnerDlg
{
	DECLARE_DYNAMIC(CItemExplorerDlg)

public:
	CItemExplorerDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CItemExplorerDlg();

	virtual int GetInnerIndex();
	virtual int GetInnerWidth();
	virtual void SetInnerWidth( int nWidth );
	virtual CWindowWnd* GetWindowWnd();

protected:
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	

	



};
