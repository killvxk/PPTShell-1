#pragma once
//
// ListItem.h
// 
#include "AssetsLayout.h"

class CAssetsFlashLayout :public CAssetsLayout
{
public:
	CAssetsFlashLayout();
	virtual ~CAssetsFlashLayout();

	virtual void		SetStream(CStream* pStream, int nType)	;
protected:

};