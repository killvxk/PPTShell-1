#pragma once
//
// INotify.h
// 
#include "EventCenter/EventDefine.h"

class IListBarNotify
{
public:

	virtual void	DoClick(TNotifyUI*		pNotify) = 0;		//左键点击
	virtual void	DoRClick(TNotifyUI*		pNotify) = 0;		//右键点击
	

	virtual bool	OnControlNotify(void* pNotify)	 = 0;		//Notify事件

	virtual	void	SetDoClickCallBack(CDelegateBase& delegate)	= 0;	//设置鼠标左键点击事件
	virtual	void	SetDoRClickCallBack(CDelegateBase& delegate)	= 0;	//设置鼠标右键点击事件
protected:
	CEventSource	m_OnDoClickCallBack;
	CEventSource	m_OnDoRClickCallBack;
};

