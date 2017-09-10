#pragma once
//
// ListItem.h
// 
#include "../ListCloudItem.h"

class CListCloud3DResourceItem :public CListCloudItem
{
public:
	CListCloud3DResourceItem();
	virtual ~CListCloud3DResourceItem();

	virtual	void			DoInit();

	virtual bool			OnDownloadDecodeList( void* pObj )			;	//解析json数据				

	virtual bool			OnChapterChanged( void* pObj )				;	//章节切换事件
protected:

};