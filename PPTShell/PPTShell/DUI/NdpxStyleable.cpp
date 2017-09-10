#include "StdAfx.h"
#include "DUI/IComponent.h"
#include "DUI/ResourceStyleable.h"
#include "DUI/IButtonTag.h"
#include "DUI/CourseStyleable.h"
#include "DUI/NdpxStyleable.h"

CNdpXStyleable::CNdpXStyleable()
{
}

CNdpXStyleable::~CNdpXStyleable()
{
}
ImplementStyleable(CNdpXStyleable)
LPCTSTR CNdpXStyleable::GetButtonText( int nButton )
{
	if (nButton == 0)
	{
		return _T("´ò¿ª");
	}
	return NULL;
}

int CNdpXStyleable::GetButtonTag( int nButton )
{
	if (nButton == 0)
	{
		return eClickFor_Open;
	}
	return eClickFor_Unkown;
}
