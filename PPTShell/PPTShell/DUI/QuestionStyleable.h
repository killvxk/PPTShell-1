#pragma once

class CResourceStyleable;
class CQuestionStyleable:
	public CResourceStyleable
{
public:
	CQuestionStyleable();
	virtual ~CQuestionStyleable();

	virtual LPCTSTR			GetDescription();
	virtual LPCTSTR			GetDefaultBkIamge();
	virtual LPCTSTR			GetIcon();

	
	DeclareStyleable(CQuestionStyleable)

};