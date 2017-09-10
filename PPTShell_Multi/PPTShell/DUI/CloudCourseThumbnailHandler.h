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
	
private:
	tstring m_strHandlerId;
public:
	virtual void	 InitHandlerId(){
		m_strHandlerId= typeid(this).name();
		m_strHandlerId += GetPreviewUrl();
	}
	virtual void	 SetHandlerId(LPCTSTR lptcsId){
		m_strHandlerId = lptcsId;
	}
	virtual tstring	 GetHandlerId(){
		return m_strHandlerId;
	}
};
