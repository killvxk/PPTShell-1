#pragma once

#include "PanelInnerDlg.h"
// CGroupExplorerDlg 对话框

class CGroupExplorerDlg : public CPanelInnerDlg
{
	DECLARE_DYNAMIC(CGroupExplorerDlg)

public:
	CGroupExplorerDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGroupExplorerDlg();

	virtual int GetInnerIndex();

	virtual int GetInnerWidth();

	virtual void SetInnerWidth( int nWidth );

	virtual CWindowWnd* GetWindowWnd();

protected:
	virtual BOOL OnInitDialog();
	//virtual BOOL PreTranslateMessage( MSG* pMsg );

	DECLARE_MESSAGE_MAP()

	


private:
	int m_nWidth;

	

	

};
