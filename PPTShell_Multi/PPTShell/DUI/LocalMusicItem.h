#pragma once
#include "DUICommon.h"
#include "LocalItem.h"

class CLocalMusicItemUI : public CLocalItemUI
{
public:
	CLocalMusicItemUI();
	~CLocalMusicItemUI();

	virtual	void		Init();

	virtual LPCTSTR		GetButtonText( int nIndex );

	virtual void		OnButtonClick( int nButtonIndex, TNotifyUI& msg );

	virtual	void		OnItemClick(TNotifyUI& msg);
	

};
