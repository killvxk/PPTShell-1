#pragma once
//
// ListItem.h
// 
#include "IListBarNotify.h"
#include "IListItemHandler.h"
#include "ListItemComponent.h"
#include "NDCloud/NDCloudAPI.h"

class CListBarItem : public CListContainerElementUI,
				public IListBarNotify,
				public IListItemHandler,
				public CListItemComponent

{
public:
	CListBarItem();
	virtual ~CListBarItem();


	void			SetJsonUrl(tstring strJsonUrl);					//设置json获取地址
	tstring			GetJsonUrl();

	int				GetItemType();									//获取item类型

	virtual void	DoClick(TNotifyUI*		pNotify) ;				//左键点击
	virtual void	DoRClick(TNotifyUI*		pNotify) ;				//右键点击

	virtual bool	OnControlNotify(void* pNotify);					//Notify事件
	
	virtual	void	DoInit();

	virtual	void	SetDoClickCallBack(CDelegateBase& delegate);	//设置点击回调
	virtual	void	SetDoRClickCallBack(CDelegateBase& delegate);	//设置鼠标右键点击事件

	void			SetParentListPtr(void * pControl);
	void*			GetParentListPtr();
protected:
	DWORD			m_dwDownloadId;
	CStream*		m_pStream;

private:
	
					
	tstring			m_strJsonUrl;
	
	void*			m_pParentListPtr;		//ListUI
};