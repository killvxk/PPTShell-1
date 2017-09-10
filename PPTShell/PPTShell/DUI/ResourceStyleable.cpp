#pragma once
#include "StdAfx.h"
#include "DUI/IComponent.h"
#include "DUI/ResourceStyleable.h"
#include "DUI/IButtonTag.h"




CResourceStyleable::CResourceStyleable()
{

}

CResourceStyleable::~CResourceStyleable()
{

}

int CResourceStyleable::GetButtonCount()
{
	return 2;
}


int CResourceStyleable::GetButtonTag( int nButton )
{
	if (nButton == 0)
	{
		return eClickFor_Insert;
	}
	else if (nButton == 1)
	{
		return eClickFor_Preview;
	}

	return NULL;
}


LPCTSTR CResourceStyleable::GetButtonText( int nButton )
{
	if (nButton == 0)
	{
		return _T("≤Â»Î");
	}
	else if (nButton == 1)
	{
		return _T("‘§¿¿");
	}

	return NULL;
}

int CResourceStyleable::GetHeight()
{
	return 110;
}

DWORD CResourceStyleable::GetTagColor()
{
	return 0xFF339933;
}
