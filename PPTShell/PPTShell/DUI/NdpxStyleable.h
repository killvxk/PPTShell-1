#pragma once

class CCourseStyleable;
class CNdpXStyleable:public CCourseStyleable
{
public:
	CNdpXStyleable();
	~CNdpXStyleable();

	virtual int		GetButtonTag( int nButton );
	virtual LPCTSTR GetButtonText( int nButton );

	DeclareStyleable(CNdpXStyleable)
};
