#pragma once
#include "StdAfx.h"
#include "DUI/ComponentCallback.h"
#include "DUI/GifAnimUI.h"
#include "DUI/ItemOption.h"

CComponentCallback CComponentCallback::Callback;
CControlUI* CComponentCallback::CreateControl( LPCTSTR pstrClass )
{
	if(_tcsicmp(pstrClass, _T("GifAnim")) == 0)
	{
		return new CGifAnimUI;
	}
	else if(_tcsicmp(pstrClass, _T("ItemOption")) == 0)
	{
		return new CItemOptionUI;
	}

	return NULL;
}




