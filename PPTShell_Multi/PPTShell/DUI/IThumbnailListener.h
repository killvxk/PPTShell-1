#pragma once

class IThumbnailListener
{
public:
	virtual void			OnGetThumbnailBefore()						= 0;
	virtual void			OnGetThumbnailCompleted(LPCTSTR	lptcsPath)	= 0;
};

