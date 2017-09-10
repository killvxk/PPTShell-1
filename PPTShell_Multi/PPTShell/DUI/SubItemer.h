#pragma once
#include "DUICommon.h"
#include "BaseItem.h"

class CSubItemer
{
public:
	CSubItemer();
	~CSubItemer();

public:
	void			SetParentItem(CBaseItemUI* pItem);

	CBaseItemUI*	GetParentItem();

	void			SetIndex(int nIndex);

	int				GetIndex();

private:
	int				m_nIndex;
	CBaseItemUI*	m_pItem;

};


class IItemContainer
{
public:
	virtual bool	HasSubitems() = 0;

};
