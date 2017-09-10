#pragma once

#include "PNGButtonCtl.h"
// CAlertWnd dialog

class CAlertWnd : public CMFCDesktopAlertDialog
{
	DECLARE_DYNCREATE(CAlertWnd)

public:
	CAlertWnd();   // standard constructor
	virtual ~CAlertWnd();

// Dialog Data
	enum { IDD = IDD_ALERTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
protected:
	virtual BOOL OnInitDialog();
	virtual void OnDraw (CDC* pDC);
	PNGButton	m_cClose;
	DECLARE_MESSAGE_MAP()
};
