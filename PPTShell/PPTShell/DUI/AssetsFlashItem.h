#pragma once
//
// ListItem.h
// 
#include "AssetsItem.h"

class CAssetsFlashItem :public CAssetsItem
{
public:
	CAssetsFlashItem();
	virtual ~CAssetsFlashItem();

	virtual	void			DoInit();

	virtual bool			OnDownloadDecodeList( void* pObj )			;	//解析json数据				

	virtual bool			OnChapterChanged( void* pObj )				;	//章节切换事件
protected:

};