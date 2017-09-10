#pragma once
//
// ListItem.h
// 
#include "../ListCloudItem.h"

class CListCloudCourseItem :public CListCloudItem
{
public:
	CListCloudCourseItem();
	virtual ~CListCloudCourseItem();

	virtual	void			DoInit();

	virtual bool			OnDownloadDecodeList( void* pObj )			;	//解析json数据				

	virtual bool			OnChapterChanged( void* pObj )				;	//章节切换事件
protected:

};