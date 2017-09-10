#pragma once
//
// ListItem.h
// 
#include "../ListCloudItem.h"

class CListCloudPPTTemplateItem :public CListCloudItem
{
public:
	CListCloudPPTTemplateItem();
	virtual ~CListCloudPPTTemplateItem();

	virtual	void			DoInit();

	virtual bool			OnDownloadDecodeList( void* pObj )			;	//解析json数据				

	virtual bool			OnChapterChanged( void* pObj )				;	//章节切换事件
protected:
	map<tstring,CStream *>		m_mapPPTTemplateStream;
};