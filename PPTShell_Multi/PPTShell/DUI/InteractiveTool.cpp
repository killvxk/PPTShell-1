#include "StdAfx.h"
#include "InteractiveTool.h"

CInteractiveTool::CInteractiveTool(void):m_sName(""),m_sImagePath(""),m_eType(UnKnow)
{
}

CInteractiveTool::~CInteractiveTool(void)
{
}

void CInteractiveTool::SetName( string name )
{
	m_sName = name;
}

string CInteractiveTool::GetName()
{
	return m_sName;
}

void CInteractiveTool::SetImagePath( string imagePath )
{
	m_sImagePath = imagePath;
}

string CInteractiveTool::GetImagePath()
{
	return m_sImagePath;
}

void CInteractiveTool::SetType( InteractiveToolType type )
{
	m_eType = type;
}

InteractiveToolType CInteractiveTool::GetType()
{
	return m_eType;
}
