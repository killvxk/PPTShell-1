#pragma once

class CComponentCallback:
	public IDialogBuilderCallback
{
public:
	virtual CControlUI* CreateControl( LPCTSTR pstrClass );

	static CComponentCallback Callback;
};

