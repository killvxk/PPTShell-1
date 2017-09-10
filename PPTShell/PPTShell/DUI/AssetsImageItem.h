#pragma once
//
// ListItem.h
// 
#include "AssetsItem.h"

class CAssetsImageItem :public CAssetsItem
{
public:
	CAssetsImageItem();
	virtual ~CAssetsImageItem();

	virtual	void			DoInit();

	virtual bool			OnDownloadDecodeList( void* pObj )			;	//解析json数据				

	virtual bool			OnChapterChanged( void* pObj )				;	//章节切换事件
protected:

};