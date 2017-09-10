#pragma once
//
// ListItem.h
// 
#include "AssetsItem.h"

class CAssetsVideoItem :public CAssetsItem
{
public:
	CAssetsVideoItem();
	virtual ~CAssetsVideoItem();

	virtual	void			DoInit();

	virtual bool			OnDownloadDecodeList( void* pObj )			;	//解析json数据				

	virtual bool			OnChapterChanged( void* pObj )				;	//章节切换事件
protected:

};