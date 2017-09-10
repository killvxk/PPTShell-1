#pragma once
//
// ListItem.h
// 
#include "AssetsItem.h"

class CDBankAssetsImageItem :public CAssetsItem
{
public:
	CDBankAssetsImageItem();
	virtual ~CDBankAssetsImageItem();

	virtual	void			DoInit();

	virtual bool			OnDownloadDecodeList( void* pObj )			;	//解析json数据				

	virtual bool			OnChapterChanged( void* pObj )				;	//章节切换事件

	virtual bool			OnEventLoginComplete( void* pObj )			;	//登录成功
protected:

};