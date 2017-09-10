#pragma once

class CCourseStyleable;
class CNdpStyleable:public CCourseStyleable
{
public:
	CNdpStyleable();
	~CNdpStyleable();

	virtual int		GetButtonTag( int nButton );
	virtual LPCTSTR GetButtonText( int nButton );

	DeclareStyleable(CNdpStyleable)
};
