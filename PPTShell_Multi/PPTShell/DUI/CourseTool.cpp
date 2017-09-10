#include "StdAfx.h"
#include "CourseTool.h"

CCourseTool::CCourseTool(void):m_sName(""),m_sImagePath(""),m_sActionPath("")
{
}

CCourseTool::~CCourseTool(void)
{
	
}

void CCourseTool::SetName( string name )
{
	m_sName = name;
}

string CCourseTool::GetName()
{
	return m_sName;
}

void CCourseTool::SetImagePath( string imagePath )
{
	m_sImagePath = imagePath;
}

string CCourseTool::GetImagePath()
{
	return m_sImagePath;
}

void CCourseTool::SetActionPath( string actionPath )
{
	m_sActionPath = actionPath;
}

string CCourseTool::GetActionPath()
{
	return m_sActionPath;
}
