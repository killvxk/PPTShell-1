#pragma once
#include "StdAfx.h"
#include "DUI/IComponent.h"
#include "DUI/ResourceStyleable.h"
#include "DUI/VolumeStyleable.h"
#include "DUI/IButtonTag.h"

CVolumeStyleable::CVolumeStyleable()
{

}

CVolumeStyleable::~CVolumeStyleable()
{

}
ImplementStyleable(CVolumeStyleable)
LPCTSTR CVolumeStyleable::GetButtonText( int nButton )
{
	if (nButton == 1)
	{
		return _T(" ‘Ã˝");
	}

	return __super::GetButtonText(nButton);
}
int CVolumeStyleable::GetButtonTag( int nButton )
{
	if (nButton == 1)
	{
		return eClickFor_Audition;
	}
	return __super::GetButtonTag(nButton);
}


LPCTSTR CVolumeStyleable::GetDescription()
{
	return _T("“Ù∆µ");
}

LPCTSTR CVolumeStyleable::GetDefaultBkIamge()
{
	return _T("Item\\item_bg_volume.png");
}

LPCTSTR CVolumeStyleable::GetIcon()
{
	return _T("Item\\bg_tit_volume.png");
}

