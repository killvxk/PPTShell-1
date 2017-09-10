#pragma once
#include "StdAfx.h"
#include "DUI/IComponent.h"
#include "DUI/ResourceStyleable.h"
#include "DUI/3DResourceStyleable.h"
#include "DUI/IButtonTag.h"

C3DResourceStyleable::C3DResourceStyleable()
{

}

C3DResourceStyleable::~C3DResourceStyleable()
{

}

ImplementStyleable(C3DResourceStyleable)


LPCTSTR C3DResourceStyleable::GetDescription()
{
	return _T("3D资源");
}

LPCTSTR C3DResourceStyleable::GetDefaultBkIamge()
{
	return _T("Item\\item_bg_image.png");
}

LPCTSTR C3DResourceStyleable::GetIcon()
{
	return _T("Item\\bg_tit_pic.png");
}

int C3DResourceStyleable::GetButtonCount()
{
	return 2;
}

int C3DResourceStyleable::GetButtonTag( int nButton )
{
	if (nButton == 0)
	{
		return eClickFor_Insert;
	}
	else if (nButton == 1)
	{
		return eClickFor_Play;
	}

	return eClickFor_Unkown;
}

LPCTSTR C3DResourceStyleable::GetButtonText( int nButton )
{
	if (nButton == 0)
	{
		return _T("插入");
	}
	else if (nButton == 1)
	{
		return _T("播放");
	}
	return NULL;
}

