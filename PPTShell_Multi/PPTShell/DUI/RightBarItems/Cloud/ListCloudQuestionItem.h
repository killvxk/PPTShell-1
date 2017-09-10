#pragma once
//
// ListItem.h
// 
#include "../ListCloudItem.h"

class CListCloudQuestionItem :public CListCloudItem
{
public:
	CListCloudQuestionItem();
	virtual ~CListCloudQuestionItem();

	virtual	void			DoInit();

	virtual bool			OnDownloadDecodeList( void* pObj )			;	//解析json数据				

	virtual bool			OnChapterChanged( void* pObj )				;	//章节切换事件
protected:

};