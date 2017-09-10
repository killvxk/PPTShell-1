#pragma once

#include "DUICommon.h"
#include "Util/Singleton.h"

class CPackPPTTipUI : public WindowImplBase
{
public:
	CPackPPTTipUI();
	~CPackPPTTipUI();

public:
	LPCTSTR GetWindowClassName() const;	

	virtual void InitWindow();

	virtual CDuiString GetSkinFile();

	virtual CDuiString GetSkinFolder();

};

typedef Singleton<CPackPPTTipUI> PackPPTTipUI;