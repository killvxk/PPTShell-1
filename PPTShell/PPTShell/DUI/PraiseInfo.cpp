#include "StdAfx.h"
#include "PraiseInfo.h"

CPraiseInfo::CPraiseInfo(void)
{
	m_nId=0;
	m_nNum=0;
	m_sText=_T("");
	m_bIsParised = false;
}

CPraiseInfo::~CPraiseInfo(void)
{
}

void CPraiseInfo::SetId( int id )
{
	m_nId = id;
}

int CPraiseInfo::GetId()
{
	return m_nId;
}

void CPraiseInfo::SetNum( int num )
{
	m_nNum=num;
}

int CPraiseInfo::GetNum()
{
	return m_nNum;
}

void CPraiseInfo::SetText( tstring text )
{
	m_sText=text;
}

tstring CPraiseInfo::GetText()
{
	return m_sText;
}

void CPraiseInfo::SetIsParised(bool isParised)
{
	m_bIsParised=isParised;
}

bool CPraiseInfo::GetIsParised()
{
	return m_bIsParised;
}
