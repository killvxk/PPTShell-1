#pragma once
#include "DUI/SubjectToolItem.h"
class CExecToolItemUI :public CSubjectToolItemUI
{
public:
	CExecToolItemUI(void);
	virtual ~CExecToolItemUI(void);

	virtual void	OnClick(CContainerUI* pParent);


private:
	RECT	m_calcRect;
};
