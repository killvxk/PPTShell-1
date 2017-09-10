#pragma once

class IClickHandler
{
public:
	/*        
	pNotify->pSender = CStyleItemUI*;
	*/
	virtual void	DoClick(TNotifyUI*	pNotify) = 0;

};

class IRClickHandler
{
public:
		/*        
	pNotify->pSender = CStyleItemUI*;
	*/
	virtual void	DoRClick(TNotifyUI*	pNotify) = 0;
};

class IButtonHandler
{
public:
	/*        
	pNotify->wParam = nButtonIndex*;
	pNotify->pSender = CResourceItemUI*;
	*/
	virtual void	DoButtonClick(
		TNotifyUI*	pNotify)		= 0;

};

class IKeyDownHandler
{
public:
	/*        
	pNotify->pSender = CStyleItemUI*;
	*/
	virtual void	DoKeyDown(TEventUI*	pNotify) = 0;

};

class IMenuHandler
{
public:
	/*        
	pNotify->wParam = nMenuIndex*;
	pNotify->pSender = CResourceItemUI*;
	*/
	virtual void	DoMenuClick(
		TNotifyUI*	pNotify)		= 0;
};

class IFunctionHandler
{
public:
		/*
	pNotify->pSender = CResourceItemUI*;
	*/
	virtual void	DoSetThumbnail(
		TNotifyUI*	pNotify)		= 0;

	/*
	pNotify->pSender = CResourceItemUI*;
	*/
	virtual void	DoDropDown(
		TNotifyUI*	pNotify)		= 0;
};




class ILifeSpanHandler
{
public:
	virtual void	DoInit(CControlUI* pHolder)		= 0;
	virtual void	DoDestroy(CControlUI* pHolder)	= 0;
};
