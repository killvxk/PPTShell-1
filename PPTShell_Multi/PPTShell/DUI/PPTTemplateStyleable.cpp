#include "StdAfx.h"
#include "DUI/IComponent.h"
#include "DUI/ResourceStyleable.h"
#include "DUI/IButtonTag.h"
#include "DUI/CourseStyleable.h"
#include "DUI/PPTTemplateStyleable.h"

CPPTTemplateStyleable::CPPTTemplateStyleable()
{
}

CPPTTemplateStyleable::~CPPTTemplateStyleable()
{
}
ImplementStyleable(CPPTTemplateStyleable)
LPCTSTR CPPTTemplateStyleable::GetButtonText( int nButton )
{
	if (nButton == 0)
	{
		return _T("”¶”√");
	}
	return NULL;
}
