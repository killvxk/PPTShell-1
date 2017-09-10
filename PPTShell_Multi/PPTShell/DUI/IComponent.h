#pragma once

class IControlDelegate
{
public:
	virtual bool			OnControlNotify(void* pObj)			= 0;
	virtual bool			OnControlEvent(void* pObj)			= 0;

};

class IResourceStyleable
{
public:
	virtual int				GetButtonCount()			= 0;
	virtual int				GetButtonTag(int nButton)	= 0;
	virtual LPCTSTR			GetButtonText(int nButton)	= 0;
	virtual LPCTSTR			GetDefaultBkIamge()			= 0;
	virtual LPCTSTR			GetDescription()			= 0;
	virtual LPCTSTR			GetIcon()					= 0;
	virtual int				GetHeight()					= 0;
};

class IComponent
{
public:
	virtual void				DoInit(
		CContainerUI*			pParent,
		IControlDelegate*		pDelegate)							= 0;

	virtual tstring				GetDescription()					= 0;

	virtual int					GetHeight()							= 0;

	virtual IResourceStyleable*	GetResourceStyleable()				= 0;
};


#define DeclareStyleable(cls)\
public:\
	static cls	Styleable;


#define ImplementStyleable(cls)\
	cls cls::Styleable;
	
