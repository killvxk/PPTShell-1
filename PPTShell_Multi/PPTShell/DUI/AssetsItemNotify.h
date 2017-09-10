#pragma once
//
// INotify.h
// 
#include "EventCenter/EventDefine.h"

class IAssetsItemNotify
{
public:

	virtual void	DoClick(TEventUI* pEvent) = 0;				//左键点击
	virtual void	DoRClick(TEventUI* pEvent) = 0;				//右键点击

	virtual bool	OnControlNotify(void* pEvent)	 = 0;		//Notify事件
	virtual bool	OnEventLoginComplete( void* pObj )	= 0;	//登录成功

	virtual	void	SetDoClickCallBack(CDelegateBase& delegate)	= 0;	//设置鼠标左键点击事件
	virtual	void	SetDoRClickCallBack(CDelegateBase& delegate)	= 0;	//设置鼠标右键点击事件
	virtual	void	SetDoFinishCallBack(CDelegateBase& delegate)	= 0;	//设置数据完成回调
	
protected:
	CEventSource	m_OnDoClickCallBack;
	CEventSource	m_OnDoRClickCallBack;
	CEventSource	m_OnFinishCallBack;
};

