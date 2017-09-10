#pragma once
//
// ListItem.h
// 
#include "AssetsLayout.h"

class CAssetsVideoLayout :public CAssetsLayout
{
public:
	CAssetsVideoLayout();
	virtual ~CAssetsVideoLayout();

	virtual void		SetStream(CStream* pStream, int nType)	;
protected:

};