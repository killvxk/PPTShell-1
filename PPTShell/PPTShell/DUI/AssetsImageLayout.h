#pragma once
//
// ListItem.h
// 
#include "AssetsLayout.h"

class CAssetsImageLayout :public CAssetsLayout
{
public:
	CAssetsImageLayout();
	virtual ~CAssetsImageLayout();

	virtual void		SetStream(CStream* pStream, int nType)	;
protected:

};