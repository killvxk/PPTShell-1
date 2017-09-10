#include "stdafx.h"
#include "ResourceDescriber.h"


CResourceDescriber::CResourceDescriber()
{
	m_nType = 0;
}

CResourceDescriber::~CResourceDescriber()
{

}

void CResourceDescriber::SetResourceType( int nType )
{
	m_nType = nType;
}

void CResourceDescriber::SetResourceTitle( LPCTSTR lptcsTitle )
{
	m_strTiltle = lptcsTitle;
}

int CResourceDescriber::GetResourceType()
{
	return m_nType;
}

LPCTSTR CResourceDescriber::GetResourceTitle()
{
	return m_strTiltle.c_str();
}
