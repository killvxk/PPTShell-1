#pragma once
//
// IHandler.h
// 

class IListItemHandler
{
public:
	virtual bool			OnChapterChanged( void* pObj )				= 0;	//章节切换事件
	virtual bool			OnRefreshGroupExplorer( void* pObj )		= 0;	//GroupExplorer刷新按钮事件

	virtual bool			OnDownloadDecodeList( void* pObj )			= 0;	//解析json数据

	

private:
	
};