#pragma once

class CItemHandler;
class IHandlerVisitor{

public:
	virtual CItemHandler*	GetHandler()						= 0;
	virtual void			SetHandler(CItemHandler* pHandler)	= 0;
};

class IHolderContainer{

public:
	virtual int				GetHolderIndex( CControlUI* pHolder)= 0;
	virtual CControlUI*		GetHolderAt(int nIndex)				= 0;
	virtual int				GetHolderCount()					= 0;
	virtual void			AddHolder(CControlUI* pHolder)		= 0;
	virtual void			RemoveHolder(CControlUI* pHolder)	= 0;
};

class IHolderVisitor{

public:
	virtual CControlUI*		GetHolder()							= 0;
	virtual void			SetHolder(CControlUI* pHolder)		= 0;

};

class ITriggerVisitor{

public:
	virtual CControlUI*		GetTrigger()						= 0;
	virtual void			SetTrigger(CControlUI* pTrigger)	= 0;
};



class CResourceDownloader;
class IDownloaderVisitor
{
public:
	virtual	CResourceDownloader*	GetDownloader()										= 0;
	virtual	void					SetDownloader(CResourceDownloader* pDownloader)		= 0;
};

class IDownloaderRefresher
{
public:
	virtual	void					RefreshDownloader(CResourceDownloader* pDownloader)	= 0;
};