#pragma once

class ITransfer
{
public:
	virtual bool			Transfer()		= 0;
	virtual void			Cancel()		= 0;
	virtual void			Pause()			= 0;
	virtual void			Resume()		= 0;
	virtual ITransfer*		Copy()			= 0;
};

class IResourceDescriber
{
public:
	virtual	int				GetResourceType()	= 0;
	virtual	LPCTSTR			GetResourceTitle()	= 0;
	virtual	LPCTSTR			GetResourceGuid()	= 0;
};
