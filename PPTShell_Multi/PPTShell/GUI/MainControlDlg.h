#pragma once
#include "DUI\ListBarDlg.h"
#include "PanelInnerDlg.h"


// CMainControlDlg dialog

class CMainControlDlg : public CPanelInnerDlg
{
	DECLARE_DYNAMIC(CMainControlDlg)

public:
	CMainControlDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMainControlDlg();
	
	virtual int		GetInnerIndex();
	virtual int		GetInnerWidth();
	virtual void	SetInnerWidth( int nWidth );
	virtual CWindowWnd* GetWindowWnd();

	inline CListBarDlg* GetListBarDlg()	{	return m_pListBarDlg;	};
	
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL	OnInitDialog();
	DECLARE_MESSAGE_MAP()

	

	



private:
	CListBarDlg* m_pListBarDlg; 
	 
};
