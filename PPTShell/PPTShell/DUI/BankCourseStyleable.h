#pragma once
#include "CourseStyleable.h"

class CBankCourseStyleable:public CCourseStyleable
{
public:
	CBankCourseStyleable();
	~CBankCourseStyleable();

	virtual LPCTSTR GetButtonText( int nButton );

	DeclareStyleable(CBankCourseStyleable)
};
