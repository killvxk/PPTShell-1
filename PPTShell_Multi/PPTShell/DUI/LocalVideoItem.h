#pragma once
#include "DUICommon.h"
#include "LocalItem.h"

class CLocalVideoItemUI : public CLocalItemUI
{
public:
	CLocalVideoItemUI();
	~CLocalVideoItemUI();

	virtual	void		Init();

	virtual void		OnButtonClick( int nButtonIndex, TNotifyUI& msg );

	virtual	void		OnItemClick(TNotifyUI& msg);

};
