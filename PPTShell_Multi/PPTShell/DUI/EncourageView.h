#pragma once
#include "DUI/InstrumentView.h"

class CEncourageViewUI: 
	public CInstrumentView,
	public INotifyUI
{
public:
	CEncourageViewUI(void);
	virtual ~CEncourageViewUI(void);

	UIBEGIN_MSG_MAP
		EVENT_ID_HANDLER(DUI_MSGTYPE_ITEMSELECT, _T("encourages"),	OnColorItemChanged);
	UIEND_MSG_MAP
public:
	virtual void Init();


protected:
	void		CreateColors();
	void		OnColorItemChanged( TNotifyUI& msg );

private:
	CListUI*			m_pList;
	CDialogBuilder		m_ColorBuilder;
};
