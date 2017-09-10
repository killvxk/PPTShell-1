#pragma once

class CStream;
class IStreamVisitor
{
public:
	//must implement or override all of them.
	virtual	void		ReadFrom(CStream* pStream)	= 0;
	virtual	void		WriteTo(CStream* pStream)	= 0;
};

class IResourceVisitor
{
public:
	virtual	LPCTSTR		GetResource()	= 0;
	virtual	LPCTSTR		GetTitle()		= 0;
	virtual	LPCTSTR		GetGuid()		= 0;



};