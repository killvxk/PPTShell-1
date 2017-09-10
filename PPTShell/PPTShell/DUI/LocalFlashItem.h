#pragma once
#include "DUICommon.h"
#include "LocalItem.h"

class CLocalFlashItemUI : public CLocalItemUI
{
public:
	CLocalFlashItemUI();
	~CLocalFlashItemUI();

	virtual	void		Init();

	virtual void		OnButtonClick(int nButtonIndex, TNotifyUI& msg );

	virtual	void		OnItemClick(TNotifyUI& msg);


};
