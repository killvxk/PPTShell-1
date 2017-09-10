#pragma once
#include "stdafx.h"
#include "DUICommon.h"
#include "BaseItem.h"
#include "SubItemer.h"


CSubItemer::CSubItemer()
{
	m_pItem		= NULL;
	m_nIndex	= -1;
}

CSubItemer::~CSubItemer()
{

}

void CSubItemer::SetParentItem( CBaseItemUI* pItem )
{
	m_pItem = pItem;
}

CBaseItemUI* CSubItemer::GetParentItem()
{
	return m_pItem;
}

void CSubItemer::SetIndex( int nIndex )
{
	m_nIndex = nIndex;
}

int CSubItemer::GetIndex()
{
	return m_nIndex;
}
