#pragma once

class CResourceStyleable;
class C3DResourceStyleable:
	public CResourceStyleable
{
public:
	C3DResourceStyleable();
	virtual ~C3DResourceStyleable();

	virtual int				GetButtonCount();
	virtual int				GetButtonTag( int nButton );
	virtual LPCTSTR			GetDescription();
	virtual LPCTSTR			GetDefaultBkIamge();
	virtual LPCTSTR			GetButtonText( int nButton );
	virtual LPCTSTR			GetIcon();


	DeclareStyleable(C3DResourceStyleable)

};