#pragma once
//
// ListItem.h
// 
#include "../ListLocalItem.h"

class CListLocalQuestionItem :public CListLocalItem
{
public:
	CListLocalQuestionItem();
	virtual ~CListLocalQuestionItem();

	virtual	void			DoInit();

	virtual bool			OnDownloadDecodeList( void* pObj )			;	//½âÎöjsonÊý¾Ý
protected:

};