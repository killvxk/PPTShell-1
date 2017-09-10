#pragma once


class CCloudResourceHandler;
class CCloudPhotoHandler:
	public CCloudResourceHandler
{

public:
	CCloudPhotoHandler();
	virtual ~CCloudPhotoHandler();

protected:
	//override
	virtual void	DoButtonClick(TNotifyUI* pNotify);
	virtual void	DoSetThumbnail( TNotifyUI* pNotify );
	virtual void	DoDropDown( TNotifyUI* pNotify );
	virtual bool	OnDeleteComplete(void* pObj);


protected:
	bool			OnGetPlaceHolderCompleted(void* pObj);
	bool			OnHandleThumbnail(void* pObj);
	bool			OnHandleInsert(void* pObj);


	DeclareHandlerId(CCloudPhotoHandler);
};
