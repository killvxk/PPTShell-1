#pragma once
#include "DUICommon.h"
#include "LocalItem.h"

class CLocalPhotoItemUI : public CLocalItemUI
{
public:
	CLocalPhotoItemUI();
	~CLocalPhotoItemUI();


	virtual	void		Init();

	virtual void		OnButtonClick( int nButtonIndex, TNotifyUI& msg );

	virtual	void		OnItemClick(TNotifyUI& msg);

};
