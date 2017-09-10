#pragma once
#include "DUI/ToolItem.h"


enum InteractCourseToolType
{
	InteractTool_Unkown,
	InteractTool_ZoomIn,
	InteractTool_Roster,
	InteractTool_PushScreen,
	InteractTool_ClassTraining,
	InteractTool_ClassSummary,
};

class CInteractCourseToolItemUI :public CToolItemUI
{
public:
	CInteractCourseToolItemUI(void);
	virtual ~CInteractCourseToolItemUI(void);

	void	SetToolType(int nType);

	int		GetToolType();

	virtual void	OnClick(CContainerUI* pParent);

private:
	int		m_nInteractCourseToolType;
};
