#pragma once
//
// ListItem.h
// 
#include "IListBarNotify.h"
#include "ListItemComponent.h"
#include "NDCloud/NDCloudAPI.h"

class CListBarButton : public CButtonUI,
	public IListBarNotify,
	public CListItemComponent

{
public:
	CListBarButton();
	virtual ~CListBarButton();

	virtual void	DoClick(TNotifyUI*		pNotify) ;		//左键点击
	virtual void	DoRClick(TNotifyUI*		pNotify) ;		//右键点击
	

	virtual bool	OnControlNotify(void* pNotify);			//Notify事件

	virtual	void	SetDoClickCallBack(CDelegateBase& delegate);	//设置点击回调
	virtual	void	SetDoRClickCallBack(CDelegateBase& delegate);	//设置鼠标右键点击事件

	int				GetItemType();									//获取item类型

protected:
	//override control
	virtual	void	DoInit();
};