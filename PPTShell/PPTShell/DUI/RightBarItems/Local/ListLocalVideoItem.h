#pragma once
//
// ListItem.h
// 
#include "../ListLocalItem.h"

class CListLocalVideoItem :public CListLocalItem
{
public:
	CListLocalVideoItem();
	virtual ~CListLocalVideoItem();

	virtual	void			DoInit();

	virtual bool			OnDownloadDecodeList( void* pObj )			;	//½âÎöjsonÊý¾Ý
protected:

};