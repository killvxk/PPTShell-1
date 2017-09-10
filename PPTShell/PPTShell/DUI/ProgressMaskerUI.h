#pragma once



class CProgressMaskerUI
	: public WindowImplBase
{
public:
	CProgressMaskerUI(void);
	virtual ~CProgressMaskerUI(void);

public:
	UIBEGIN_MSG_MAP
		//EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("close"),			OnBtnClose);
	UIEND_MSG_MAP

protected:
	virtual void		InitWindow();

	virtual CDuiString	GetSkinFolder();

	virtual CDuiString	GetSkinFile();

	virtual void		OnFinalMessage(HWND hWnd);

	virtual LPCTSTR		GetWindowClassName( void ) const;


public:
	void				Stop();

	void				SetOperationId(DWORD dwId);

	virtual void		ShowWindow( bool bShow = true, bool bTakeFocus = true );


public:
	bool				OnOperationProgress(void* pObj);

	bool				OnOperationCompleted(void* pObj);


private:
	CWndShadow			m_wndShadow;
	DWORD				m_dwTaskId;

};
