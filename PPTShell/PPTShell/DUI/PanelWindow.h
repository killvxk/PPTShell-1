#pragma once


class CPanelViewUI;
class CPanelWindowUI:
	public WindowImplBase
{
public:
	CPanelWindowUI(void);
	~CPanelWindowUI(void);



protected:
	virtual CDuiString	GetSkinFolder();

	virtual CDuiString	GetSkinFile();

	virtual LPCTSTR		GetWindowClassName( void ) const;

	virtual	CControlUI*	CreateControl(LPCTSTR pstrClass);

	virtual void		OnFinalMessage( HWND hWnd );

	virtual LRESULT		OnKeyDown( UINT , WPARAM , LPARAM , BOOL& bHandled );

	virtual void		InitWindow();

public:
	void	ShowPanel(HWND hParent);

	void	ClosePanel();

	void	ClearPanel();

	void	SetPenColor(DWORD dwColor);


	void	SetPointerType(int nPointerType);
	

	



private:
	CPanelViewUI* m_pPanelView;

	

};
