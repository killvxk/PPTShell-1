#pragma once
//
// ListItem.h
// 
#include "AssetsLayout.h"

class CAssetsAllLayout :public CAssetsLayout
{
public:
	CAssetsAllLayout();
	virtual ~CAssetsAllLayout();

	virtual void		SetStream(CStream* pStream, int nType)	;
protected:

};