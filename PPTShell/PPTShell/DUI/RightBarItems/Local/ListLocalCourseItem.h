#pragma once
//
// ListItem.h
// 
#include "../ListLocalItem.h"

class CListLocalCourseItem :public CListLocalItem
{
public:
	CListLocalCourseItem();
	virtual ~CListLocalCourseItem();

	virtual	void			DoInit();

	virtual bool			OnDownloadDecodeList( void* pObj )			;	//½âÎöjsonÊý¾Ý

protected:

};