#pragma once

class IDeleteListener
{
public:
	virtual void	OnDeleteComplete(LPCTSTR lptcsError	)						= 0;
};


class IRenameListener
{
public:
	virtual void	OnRenameComplete(LPCTSTR lptcsError, LPCTSTR lptcsNewName)	= 0;
};

