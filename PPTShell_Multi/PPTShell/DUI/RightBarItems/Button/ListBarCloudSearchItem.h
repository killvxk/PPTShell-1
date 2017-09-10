#pragma once
//
// ListItem.h
// 
#include "../ListBarItem.h"

class CListBarCloudSearchItem : public CListBarItem
{
public:
	CListBarCloudSearchItem();
	~CListBarCloudSearchItem();

	virtual bool			OnChapterChanged( void* pObj )				;	//章节切换事件
	virtual bool			OnRefreshGroupExplorer( void* pObj )		;	//GroupExplorer刷新按钮事件
	virtual bool			OnDownloadDecodeList( void* pObj )			;	//解析json数据	

	virtual void			DoClick(TNotifyUI*		pNotify) ;				//左键点击
	virtual void			DoRClick(TNotifyUI*		pNotify) ;				//右键点击

	bool					Select(bool bSelect = true);	
protected:
	//override control
	virtual	void			DoInit();

	COptionUI*				m_pContainerIcon;
	CButtonUI*				m_pContentBtn;					//文本按钮
};