#pragma once
//
// ListItem.h
// 
#include "../ListLocalItem.h"

class CListLocalVolumeItem :public CListLocalItem
{
public:
	CListLocalVolumeItem();
	virtual ~CListLocalVolumeItem();

	virtual	void			DoInit();

	virtual bool			OnDownloadDecodeList( void* pObj )			;	//½âÎöjsonÊý¾Ý
protected:

};