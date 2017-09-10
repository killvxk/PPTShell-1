#pragma once


class CCloudCourseHandler;
class CCloudCourseThumbnailHandler:
	public CCloudCourseHandler
{

public:
	CCloudCourseThumbnailHandler();
	virtual ~CCloudCourseThumbnailHandler();

protected:
	//override
	virtual void	DoClick(TNotifyUI*	pNotify);

	virtual void	ReadFrom( CStream* pStream );
	virtual void	WriteTo( CStream* pStream );

public:
	LPCTSTR			GetPreviewUrl();
	
	virtual void	 InitHandlerId();
};
