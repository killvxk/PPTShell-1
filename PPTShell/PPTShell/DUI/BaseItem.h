#pragma once
#include "DUICommon.h"


class CBaseItemUI : public CVerticalLayoutUI
{
public:
	CBaseItemUI();
	virtual ~CBaseItemUI();


protected:
	virtual	void	Init();
	CContainerUI*	GetContent();

protected:
	virtual	void	OnItemClick(TNotifyUI& msg);

public:
	virtual void	SetContentHeight(int nHeight);
	void			SetContentWidth(int nWidth);

	void			SetResource(LPCTSTR lptcsResource);
	LPCTSTR			GetResource();

	void			SetTitle(LPCTSTR lptcsName);
	LPCTSTR			GetTitle();
	virtual void	SetImage(LPCTSTR lptcsImage);
	void			SetGroup(LPCTSTR lptcsGroup);
	COptionUI*		GetOption();
	CLabelUI*		GetTitleControl();
	CLabelUI*		GetItemLabelControl();
	void			SetItemLabel(LPCTSTR lptcsLabel, bool bShow = false);
	bool			IsSelected();
	

	static bool		OpenAsDefaultExec(LPCTSTR lptcsPath);
	
protected:
	CContainerUI*			m_cntContent;
	CLabelUI*				m_lbImage;
	CLabelUI*				m_lbName;
	CLabelUI*				m_lbItemLabel;//±Í«©
	COptionUI*				m_optBorder;
	tstring					m_strResource;
	tstring					m_strTitle;
	tstring					m_strImage;
	tstring					m_strItemLabel;//±Í«©
	bool					m_bShowItemLabel;
	tstring					m_strGroup;

};
