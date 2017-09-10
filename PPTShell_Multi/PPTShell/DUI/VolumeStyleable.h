#pragma once

class CResourceStyleable;
class CVolumeStyleable:
	public CResourceStyleable
{
public:
	CVolumeStyleable();
	virtual ~CVolumeStyleable();

	virtual LPCTSTR			GetButtonText( int nButton );
	virtual int				GetButtonTag( int nButton );
	virtual LPCTSTR			GetDescription();
	virtual LPCTSTR			GetDefaultBkIamge();
	virtual LPCTSTR			GetIcon();

	DeclareStyleable(CVolumeStyleable)

		


};