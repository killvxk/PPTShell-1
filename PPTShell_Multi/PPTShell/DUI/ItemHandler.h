#pragma once

#include "DUI/IVisitor.h"
#include "DUI/INotifyHandler.h"
#include "DUI/IItemHandler.h"
#include "DUI/IStreamReader.h"


class CItemHandler:
	public IItemHandler,
	public IClickHandler,
	public IKeyDownHandler,
	public IButtonHandler,
	public IMenuHandler,
	public IFunctionHandler,
	public ILifeSpanHandler,
	public IStreamVisitor,
	public IResourceVisitor,
	public IHolderContainer,
	public ITriggerVisitor
{

public:
	CItemHandler();
	virtual ~CItemHandler();

public:
	virtual void	DoClick(TNotifyUI*	pNotify);

	virtual void	DoRClick(TNotifyUI*	pNotify);

	virtual void	DoButtonClick(TNotifyUI* pNotify);

	virtual void	DoMenuClick(TNotifyUI*	pNotify);

	virtual void	DoKeyDown( TEventUI* pNotify );


	//IHolderContainer
	virtual CControlUI*	GetHolderAt(int nIndex);
		
	virtual int			GetHolderCount();

	virtual void		AddHolder( CControlUI* pHolder );

	virtual void		RemoveHolder( CControlUI* pHolder );
	//return -1 if has't holder
	virtual int			GetHolderIndex( CControlUI* pHolder );

	//ITriggerVisitor
	CControlUI*		GetTrigger();

	void			SetTrigger( CControlUI* pTrigger );

	IHandlerVisitor* GetHolderHandlerVisitor();

	virtual void			InitHandlerId()		= 0;
	virtual tstring			GetHandlerId()		= 0;
	virtual void			SetHandlerId(LPCTSTR lptcsId)	= 0;

public:
	static	map<DWORD, CItemHandler*>	m_mapHandlers;
	static  CItemHandler*	GetExistedHandler(LPCTSTR lptcsId);
	static  void			AppendHandler(LPCTSTR lptcsId, CItemHandler* pHandler);
	static  void			RemoveHandler(LPCTSTR lptcsId);
	static	bool			OpenAsDefaultExec( LPCTSTR lptcsPath );

	
public:
	CEventSource	OnClick;
	CEventSource	OnRClick;
	CEventSource	OnKeyDown;
	CEventSource	OnMenuClick;
	CEventSource	OnButtonClick;

protected:
	vector<CControlUI*>	m_vctrHolder;


private:
	CStream*			m_pItemStream;
	CControlUI*			m_pTrigger;
	IHandlerVisitor*	m_pHolderHandlerVisitor;
};
