#include "StdAfx.h"
#include "CoursePlayerShadow.h"

CCoursePlayerShadow::CCoursePlayerShadow(void)
{
	m_pShadowLayout = NULL;
}

CCoursePlayerShadow::~CCoursePlayerShadow(void)
{
}


LPCTSTR CCoursePlayerShadow::GetWindowClassName() const
{
	return _T("CoursePlayerShadow");
}

CDuiString CCoursePlayerShadow::GetSkinFile()
{
	return _T("CoursePlayer\\CoursePlayerShadow.xml");
}

CDuiString CCoursePlayerShadow::GetSkinFolder()
{
	return CDuiString(_T("skins"));
}
void CCoursePlayerShadow::SetShadowLayoutSize(RECT rcPos){
	if (m_pShadowLayout){
		SIZE szXY = {100, 100};
		m_pShadowLayout->SetFixedXY(szXY);
		m_pShadowLayout->SetFixedWidth(rcPos.right-rcPos.left);
		m_pShadowLayout->SetFixedHeight(rcPos.bottom-rcPos.top);
	}
}

void CCoursePlayerShadow::InitWindow()
{
	m_pShadowLayout = static_cast<CVerticalLayoutUI *>(m_PaintManager.FindControl(_T("ShadowLayout")));
}
