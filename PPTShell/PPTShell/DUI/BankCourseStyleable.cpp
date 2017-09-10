#include "StdAfx.h"
#include "DUI/IComponent.h"
#include "DUI/ResourceStyleable.h"
#include "DUI/IButtonTag.h"
#include "BankCourseStyleable.h"

CBankCourseStyleable::CBankCourseStyleable()
{
}

CBankCourseStyleable::~CBankCourseStyleable()
{
}
ImplementStyleable(CBankCourseStyleable)
LPCTSTR CBankCourseStyleable::GetButtonText( int nButton )
{
	if (nButton == 0)
	{
		return _T("´ò¿ª");
	}
	return NULL;
}
