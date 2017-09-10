#pragma once
//
// ListItem.h
// 
#include "AssetsItem.h"

class CAssetsAllItem :public CAssetsItem
{
public:
	CAssetsAllItem();
	virtual ~CAssetsAllItem();

	virtual	void			DoInit();

	virtual bool			OnDownloadDecodeList( void* pObj )			;	//解析json数据				

	virtual bool			OnChapterChanged( void* pObj )				;	//章节切换事件

	virtual	void			RefreshData();

	void					StartLoadGif();
	void					StopLoadGif();
protected:
	bool					m_bData;
};