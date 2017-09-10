#pragma once
//
// ListItem.h
// 
#include "../ListDBankItem.h"

class CListDBankCoursewareObjectsItem :public CListDBankItem
{
public:
	CListDBankCoursewareObjectsItem();
	virtual ~CListDBankCoursewareObjectsItem();

	virtual	void			DoInit();

	virtual bool			OnDownloadDecodeList( void* pObj )			;	//解析json数据				

	virtual bool			OnEventLoginComplete( void* pObj )		;

	virtual bool			OnGetTotalCount(void * pParam)				;	//获取总量回调
protected:

};