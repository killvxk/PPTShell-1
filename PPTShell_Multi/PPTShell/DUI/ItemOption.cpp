#pragma once
#include "stdafx.h"
#include "ItemOption.h"

CItemOptionUI::CItemOptionUI()
{

}

CItemOptionUI::~CItemOptionUI()
{

}

void CItemOptionUI::Selected( bool bSelected )
{
	__super::Selected(bSelected);
	if (OnSelect)
	{
		TNotifyUI notify;
		notify.pSender	= this;
		OnSelect(&notify);
	}
	
}
