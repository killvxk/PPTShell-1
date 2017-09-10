#pragma once

class IResourceStyleable;
class CResourceStyleable:
	public IResourceStyleable

{
public:
	CResourceStyleable();
	virtual ~CResourceStyleable();

	virtual int				GetButtonCount();
	virtual int				GetButtonTag( int nButton );
	virtual LPCTSTR			GetButtonText( int nButton );
	virtual int				GetHeight();

	


};