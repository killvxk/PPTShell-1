#pragma once

#include "DUICommon.h"
#include "LocalPhotoItem.h"
#include "LocalCourseItem.h"
#include "SubItemer.h"

class CLocalCourseSubItemUI : 
	public CLocalPhotoItemUI,
	public CSubItemer
{
public:
	CLocalCourseSubItemUI();
	~CLocalCourseSubItemUI();

	virtual	void		Init();

	virtual void		OnButtonClick( int nButtonIndex, TNotifyUI& msg );

	virtual	void		OnItemClick(TNotifyUI& msg);

	virtual bool	OnEmptyControlEvent(void* pEvent);

};
