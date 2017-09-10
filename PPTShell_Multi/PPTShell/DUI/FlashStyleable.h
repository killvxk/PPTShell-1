#pragma once

class CResourceStyleable;
class CFlashStyleable:
	public CResourceStyleable
{
public:
	CFlashStyleable();
	virtual ~CFlashStyleable();

	virtual LPCTSTR			GetDescription();
	virtual LPCTSTR			GetDefaultBkIamge();
	virtual LPCTSTR			GetIcon();

	
	DeclareStyleable(CFlashStyleable)

};