#pragma once
#include "DUICommon.h"
#include "WndShadow.h"

//#include "ListBarDlg.h"

class CNewFolder : public WindowImplBase
{
public:
	//CNewFolder(CListBarDlg* pDlg);
	CNewFolder(void);
	~CNewFolder(void);

	UIBEGIN_MSG_MAP
		EVENT_BUTTON_CLICK(_T("CloseBtn"),			OnBtnClose);
		EVENT_BUTTON_CLICK(_T("AddBtn"),			OnBtnAdd);
		
	UIEND_MSG_MAP

public:
	LPCTSTR GetWindowClassName() const;	
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual 

	void			OnRichEditEvent();
	void			Init(HWND hWndParent, void* pDlg);	
	void			SetFolderName(CDuiString strName);
	//void			SetListBar(CListBarDlg* pDlg) {	m_pListBarDlg = pDlg;	}

	bool			OnRichEditNotify(void* pNotify);

	static CNewFolder* GetInstance();

protected:
	void		OnBtnClose(TNotifyUI& msg);
	void		OnBtnAdd(TNotifyUI& msg);

	bool		IsFileNameValid(const TCHAR* FileName);

	static CNewFolder*		m_pInstance;
	CWndShadow				m_WndShadow;

protected:
	CRichEditUI*	m_pEdit;
	CLabelUI*		m_plblTip;
	CButtonUI*		m_pButton;
	void*			m_pListBarDlg;

	bool			m_bModify;
	CDuiString		m_strOldName;

};
