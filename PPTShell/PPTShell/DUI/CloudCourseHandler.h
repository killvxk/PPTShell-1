#pragma once


class CCloudResourceHandler;
class CCloudCourseHandler:
	public CCloudResourceHandler
{

public:
	CCloudCourseHandler();
	virtual ~CCloudCourseHandler();

protected:
	//override
	virtual void	DoClick(TNotifyUI*	pNotify);
	virtual void	DoButtonClick(TNotifyUI* pNotify);
	virtual void	DoSetThumbnail( TNotifyUI* pNotify );
	virtual void	DoDropDown( TNotifyUI* pNotify );
	virtual bool	OnDeleteComplete(void* pObj);
	
	virtual void	ReadFrom( CStream* pStream );
	virtual void	WriteTo( CStream* pStream );
	bool			OnHandleThumbnail(void* pObj);
	bool			OnHandleApply(void* pObj);

	virtual void	InsertPPT(void* pObj);

public:
	bool			OnGetPlaceHolderCompleted(void* pObj);
	
	
private:
	CStream*		m_pPreviewsStream;
	
protected:
	tstring			m_strPreviewUrl;

	DeclareHandlerId(CCloudCourseHandler);
};
