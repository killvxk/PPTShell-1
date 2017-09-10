#pragma once
//
// ListItem.h
// 
#include "../ListLocalItem.h"

class CListLocalCoursewareObjectsItem :public CListLocalItem
{
public:
	CListLocalCoursewareObjectsItem();
	virtual ~CListLocalCoursewareObjectsItem();

	virtual	void			DoInit();

	virtual bool			OnDownloadDecodeList( void* pObj )			;	//½âÎöjsonÊý¾Ý

protected:

};