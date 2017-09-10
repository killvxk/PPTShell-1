#pragma once
#include "DUICommon.h"
#include "DUI/GifAnimUI.h"

class CClearMask: public WindowImplBase
{
public:
	CClearMask(void);
	~CClearMask(void);

	virtual CDuiString GetSkinFolder();
	virtual CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName(void) const;
	virtual	void InitWindow();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	void CloseMask();

	CGifAnimUI* pMask;
};
