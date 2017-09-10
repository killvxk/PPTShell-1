#pragma once

class IStyleable
{
public:
	virtual LPCTSTR			GetStyle()								= 0;
	virtual	CDialogBuilder*	GetStyleBuilder()						= 0;
	virtual	CControlUI*		OnCreateStyleControl(LPCTSTR lptcsName)	= 0;
};