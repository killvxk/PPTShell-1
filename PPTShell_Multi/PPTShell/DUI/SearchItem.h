#pragma once

#include "DUICommon.h"
#include "ListItem.h"


class CSearchItemUI : public CListItemUI
{
public:
	CSearchItemUI(void);
	~CSearchItemUI(void);

	virtual LPCTSTR GetClass() const;

	virtual void	DoEvent(TEventUI& event);

	virtual	void	DoInit();

	virtual bool	Select(bool bSelect /* = true */);

	void ChangeLayoutWidth(bool bLeave = false);

};
