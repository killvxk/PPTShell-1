#pragma once

#include "DUICommon.h"
#include "PhotoItem.h"
#include "CourseItem.h"
#include "SubItemer.h"

class CCourseSubItemUI : 
	public CPhotoItemUI,
	public CSubItemer
{
public:
	CCourseSubItemUI();
	~CCourseSubItemUI();

	virtual	void		Init();

	virtual void		OnButtonClick( int nButtonIndex, TNotifyUI& msg );

	virtual	void		OnItemClick(TNotifyUI& msg);

	bool				OnDownloadThumbnailCompleted(void* pNotify);

	virtual void		ReadStream( CStream* pStream );

	virtual void		OnDownloadResourceCompleted( int nButtonIndex, LPCTSTR lptcsPath );

	virtual	void		OnItemDragFinish();

protected:
	virtual	void		DownloadThumbnail();

	virtual void		DownloadResource( int nButtonIndex, int nType, int nThumbnailSize );

};
