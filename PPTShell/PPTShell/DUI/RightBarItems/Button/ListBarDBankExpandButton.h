#pragma once
//
// ListItem.h
// 
#include "../ListBarExpandButton.h"

class CListBarDBankExpandButton : public CListBarExpandButton
{
public:
	CListBarDBankExpandButton();
	~CListBarDBankExpandButton();
	
	virtual void		DoClick(TNotifyUI*		pNotify) ;				//×ó¼üµã»÷

	virtual void		Expand(bool bExpand);
protected:
	//override control
	virtual	void			DoInit();

};