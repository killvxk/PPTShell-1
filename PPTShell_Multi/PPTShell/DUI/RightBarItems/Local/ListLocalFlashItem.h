#pragma once
//
// ListItem.h
// 
#include "../ListLocalItem.h"

class CListLocalFlashItem :public CListLocalItem
{
public:
	CListLocalFlashItem();
	virtual ~CListLocalFlashItem();

	virtual	void			DoInit();

	virtual bool			OnDownloadDecodeList( void* pObj )			;	//½âÎöjsonÊý¾Ý
protected:

};