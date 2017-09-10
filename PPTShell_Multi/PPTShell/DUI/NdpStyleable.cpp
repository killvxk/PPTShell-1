#include "StdAfx.h"
#include "DUI/IComponent.h"
#include "DUI/ResourceStyleable.h"
#include "DUI/IButtonTag.h"
#include "DUI/CourseStyleable.h"
#include "DUI/NdpStyleable.h"

CNdpStyleable::CNdpStyleable()
{
}

CNdpStyleable::~CNdpStyleable()
{
}
ImplementStyleable(CNdpStyleable)
LPCTSTR CNdpStyleable::GetButtonText( int nButton )
{
	if (nButton == 0)
	{
		return _T("²¥·Å");
	}
	return NULL;
}

int CNdpStyleable::GetButtonTag( int nButton )
{
	if (nButton == 0)
	{
		return eClickFor_Play;
	}
	return eClickFor_Unkown;
}
