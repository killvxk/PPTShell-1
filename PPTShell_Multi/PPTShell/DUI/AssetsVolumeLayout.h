#pragma once
//
// ListItem.h
// 
#include "AssetsLayout.h"

class CAssetsVolumeLayout :public CAssetsLayout
{
public:
	CAssetsVolumeLayout();
	virtual ~CAssetsVolumeLayout();

	virtual void		SetStream(CStream* pStream, int nType)	;
protected:

};