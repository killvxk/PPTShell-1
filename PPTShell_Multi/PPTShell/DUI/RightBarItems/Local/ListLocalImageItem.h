#pragma once
//
// ListItem.h
// 
#include "../ListLocalItem.h"

class CListLocalImageItem :public CListLocalItem
{
public:
	CListLocalImageItem();
	virtual ~CListLocalImageItem();

	virtual	void			DoInit();

	virtual bool			OnDownloadDecodeList( void* pObj )			;	//½âÎöjsonÊý¾Ý
protected:

};