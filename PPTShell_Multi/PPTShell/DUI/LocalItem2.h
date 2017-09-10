#pragma once
#include "DUICommon.h"
#include "ResourceItem.h"

class CLocalItemUI : public CResourceItemUI
{
public:
	CLocalItemUI();
	~CLocalItemUI();


	virtual	void		InitItem();

	virtual LPCTSTR		GetButtonText( int nIndex );

	virtual void		OnButtonClick(int nButtonIndex, TNotifyUI& msg );

	virtual	void		OnItemClick(TNotifyUI& msg);

	virtual void		SetResourceUrl( LPCTSTR lptcsResourceUrl );
	virtual	void		OnItemDragFinish();

	void				InsertPPTOperate();

};
