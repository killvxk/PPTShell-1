#pragma once
#include "DUI/ToolItem.h"
#include "DUI/InstrumentView.h"
class CSubjectToolItemUI :
	public CToolItemUI,
	public IPageListener
{
public:
	CSubjectToolItemUI(void);
	virtual ~CSubjectToolItemUI(void);

	bool		HasProcess();
	void		SetProcess(HANDLE hProcess);
	void		KillProcess();

	virtual void	OnClick(CContainerUI* pParent);

	virtual void OnPageChangeBefore();

	virtual void OnPageChanged( int nPageIndex );

	virtual void OnPageScanneded();

protected:
	HANDLE		m_hProcess;

};
