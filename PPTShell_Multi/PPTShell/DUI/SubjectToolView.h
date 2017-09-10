#pragma once
#include "InstrumentView.h"


class CSubjectToolViewUI:
	public CInstrumentView,
	public IDialogBuilderCallback,
	public INotifyUI
{
public:
	CSubjectToolViewUI(void);
	virtual ~CSubjectToolViewUI(void);

	UIBEGIN_MSG_MAP
		EVENT_ID_HANDLER(DUI_MSGTYPE_SETFOCUS, _T("subject_item"),	OnSubjectItemClick);
	UIEND_MSG_MAP
private:
	virtual void			Init();
	virtual CControlUI*		CreateControl( LPCTSTR pstrClass );



protected:
	void			CreateSubjectWithConfig();
	CControlUI*		CreateItemUI(int nToolType);
	CContainerUI*	CreateGroupUI(LPCTSTR lptcsGroup);
	CTileLayoutUI*	GetGroupList(CContainerUI* pGroup);
	void			CreateGroupFromDirectory(LPCTSTR lptcsGroupDir, LPCTSTR lptcsGroupName);
	void			CreateGroupFromValue(LPCTSTR lptcsGroup, LPVOID lpGroup);

	void			OnSubjectItemClick( TNotifyUI& msg );
	virtual	void	OnPageChangeBefore();

private:
	CContainerUI*			m_pMask;
	CContainerUI*			m_pList;

	static CDialogBuilder	m_ItemBuilder;
	static CDialogBuilder	m_GroupBuilder;

	int						m_nContentHeight;
	int						m_nItemType;
};
