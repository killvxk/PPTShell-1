#pragma once
#include "StdAfx.h"
#include "DUI/IComponent.h"
#include "DUI/ResourceStyleable.h"
#include "DUI/IButtonTag.h"
#include "DUI/CourseStyleable.h"


CCourseStyleable::CCourseStyleable()
{

}

CCourseStyleable::~CCourseStyleable()
{

}
ImplementStyleable(CCourseStyleable)
int CCourseStyleable::GetButtonCount()
{
	return 1;
}


int CCourseStyleable::GetButtonTag( int nButton )
{
	if (nButton == 0)
	{
		return eClickFor_Apply;
	}

	return eClickFor_Unkown;
}


LPCTSTR CCourseStyleable::GetButtonText( int nButton )
{
	if (nButton == 0)
	{
		return _T("²åÈë¿Î¼þ");
	}
	return NULL;
}

LPCTSTR CCourseStyleable::GetDescription()
{
	return _T("¿Î¼þ");
}

LPCTSTR CCourseStyleable::GetDefaultBkIamge()
{
	return _T("Item\\item_bg_course.png");
}

LPCTSTR CCourseStyleable::GetIcon()
{
	return _T("Item\\bg_tit_class.png");
}

int CCourseStyleable::GetHeight()
{
	return 80;
}
