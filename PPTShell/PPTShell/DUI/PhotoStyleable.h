#pragma once

class CResourceStyleable;
class CPhotoStyleable:
	public CResourceStyleable
{
public:
	CPhotoStyleable();
	virtual ~CPhotoStyleable();

	virtual LPCTSTR			GetDescription();
	virtual LPCTSTR			GetDefaultBkIamge();
	virtual LPCTSTR			GetIcon();
	virtual DWORD			GetTagColor();


	DeclareStyleable(CPhotoStyleable)

};