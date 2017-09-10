#pragma once

class CCourseStyleable;
class CPPTTemplateStyleable:public CCourseStyleable
{
public:
	CPPTTemplateStyleable();
	~CPPTTemplateStyleable();

	virtual LPCTSTR GetButtonText( int nButton );
	virtual LPCTSTR	GetDescription();

	DeclareStyleable(CPPTTemplateStyleable)
};
