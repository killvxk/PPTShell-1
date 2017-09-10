#pragma once


class CCloudCourseHandler;
class CCloudCourseSubHandler:
	public CCloudCourseHandler
{

public:
	CCloudCourseSubHandler();
	virtual ~CCloudCourseSubHandler();

protected:
	//override
	virtual void	DoClick(TNotifyUI*	pNotify);
	virtual void	DoButtonClick(TNotifyUI* pNotify);
	virtual void	InsertPPT(void* pObj);


	virtual void	ReadFrom( CStream* pStream );
	virtual void	WriteTo( CStream* pStream );

private:
	int				m_nSlideIndex;



	virtual void	 InitHandlerId();
	
};
