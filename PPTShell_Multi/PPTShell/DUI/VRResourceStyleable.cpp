#pragma once
#include "StdAfx.h"
#include "DUI/IComponent.h"
#include "DUI/ResourceStyleable.h"
#include "DUI/VRResourceStyleable.h"
#include "DUI/IButtonTag.h"

CVRResourceStyleable::CVRResourceStyleable()
{

}

CVRResourceStyleable::~CVRResourceStyleable()
{

}

ImplementStyleable(CVRResourceStyleable)


LPCTSTR CVRResourceStyleable::GetDescription()
{
	return _T("VR◊ ‘¥");
}

LPCTSTR CVRResourceStyleable::GetDefaultBkIamge()
{
	return _T("Item\\item_bg_image.png");
}

LPCTSTR CVRResourceStyleable::GetIcon()
{
	return _T("Item\\bg_tit_pic.png");
}

int CVRResourceStyleable::GetButtonCount()
{
	return 2;
}

int CVRResourceStyleable::GetButtonTag( int nButton )
{
	if (nButton == 0)
	{
		return eClickFor_Insert;
	}
	else if (nButton == 1)
	{
		return eClickFor_Introduce;
	}

	return eClickFor_Unkown;
}

LPCTSTR CVRResourceStyleable::GetButtonText( int nButton )
{
	if (nButton == 0)
	{
		return _T("≤Â»Î");
	}
	else if (nButton == 1)
	{
		return _T("ºÚΩÈ");
	}
	return NULL;
}

