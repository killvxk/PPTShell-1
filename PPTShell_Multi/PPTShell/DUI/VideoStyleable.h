#pragma once

class CResourceStyleable;
class CVideoStyleable:
	public CResourceStyleable
{
public:
	CVideoStyleable();
	virtual ~CVideoStyleable();

	virtual LPCTSTR			GetDescription();
	virtual LPCTSTR			GetDefaultBkIamge();
	virtual LPCTSTR			GetIcon();

	DeclareStyleable(CVideoStyleable)

};