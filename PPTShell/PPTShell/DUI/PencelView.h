#pragma once
#include "InstrumentView.h"

class CPencelViewUI: 
	public CInstrumentView,
	public INotifyUI
{
public:
	CPencelViewUI(void);
	virtual ~CPencelViewUI(void);

	UIBEGIN_MSG_MAP
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK, _T("color_item"),	OnColorItemClick);
	UIEND_MSG_MAP
public:
	virtual void Init();


protected:
	void			CreateColors();
	void			OnColorItemClick( TNotifyUI& msg );
	int				GetCurSel();
	void			SetCurSel(int nSel);

public:
	virtual void	OnSelected();
	virtual void	OnUnSelected();

	virtual	void	OnPageChangeBefore();
	virtual void	OnPageScanneded();

private:
	CListUI*			m_pList;
	CDialogBuilder		m_ColorBuilder;
	HWND				m_hScreenWnd;

	bool OnBlackboardColorRequest( void* pObj );
};
