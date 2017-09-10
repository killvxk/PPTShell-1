#pragma once

class CResourceStyleable;
class CCourseStyleable:
	public CResourceStyleable
{
public:
	CCourseStyleable();
	virtual ~CCourseStyleable();

	virtual int				GetButtonCount();
	virtual int				GetButtonTag( int nButton );
	virtual LPCTSTR			GetButtonText( int nButton );
	virtual LPCTSTR			GetDescription();
	virtual LPCTSTR			GetDefaultBkIamge();
	virtual LPCTSTR			GetIcon();
	virtual int				GetHeight();

	DeclareStyleable(CCourseStyleable)

};