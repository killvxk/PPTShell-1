#include "StdAfx.h"
#include "InteractCourseToolItem.h"
#include "DUI/InstrumentItem.h"


CInteractCourseToolItemUI::CInteractCourseToolItemUI(void)
{
	m_nInteractCourseToolType = InteractTool_Unkown;
}	

CInteractCourseToolItemUI::~CInteractCourseToolItemUI(void)
{

}


void CInteractCourseToolItemUI::OnClick( CContainerUI* pParent )
{

}

void CInteractCourseToolItemUI::SetToolType( int nType )
{
	m_nInteractCourseToolType = nType;
}

int CInteractCourseToolItemUI::GetToolType()
{
	return m_nInteractCourseToolType;
}
