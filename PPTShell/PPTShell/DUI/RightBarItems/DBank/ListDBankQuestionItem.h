#pragma once
//
// ListItem.h
// 
#include "../ListDBankItem.h"

class CListDBankQuestionItem :public CListDBankItem
{
public:
	CListDBankQuestionItem();
	virtual ~CListDBankQuestionItem();

	virtual	void			DoInit();

	virtual bool			OnDownloadDecodeList( void* pObj )			;	//解析json数据				

	virtual bool			OnEventLoginComplete( void* pObj )		;

	virtual bool			OnGetTotalCount(void * pParam)				;	//获取总量回调
protected:

};