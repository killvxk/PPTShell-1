#pragma once

class CResourceStyleable;
class CVRResourceStyleable:
	public CResourceStyleable
{
public:
	CVRResourceStyleable();
	virtual ~CVRResourceStyleable();

	virtual int				GetButtonCount();
	virtual int				GetButtonTag( int nButton );
	virtual LPCTSTR			GetDescription();
	virtual LPCTSTR			GetDefaultBkIamge();
	virtual LPCTSTR			GetButtonText( int nButton );
	virtual LPCTSTR			GetIcon();


	DeclareStyleable(CVRResourceStyleable)

};