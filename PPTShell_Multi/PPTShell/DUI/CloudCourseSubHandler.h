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

private:
	tstring m_strHandlerId;
public:
	virtual void	 InitHandlerId(){
		m_strHandlerId = typeid(this).name();
		m_strHandlerId += m_strPreviewUrl;
	}
	virtual void	 SetHandlerId(LPCTSTR lptcsId){
		m_strHandlerId = lptcsId;
	}
	virtual tstring	 GetHandlerId(){
		return m_strHandlerId;
	}
};
