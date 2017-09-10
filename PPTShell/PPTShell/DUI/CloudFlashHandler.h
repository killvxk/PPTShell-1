#pragma once


class CCloudResourceHandler;
class CCloudFlashHandler:
	public CCloudResourceHandler
{

public:
	CCloudFlashHandler();
	virtual ~CCloudFlashHandler();

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
	bool			OnHandlePreivew(void* pObj);

private:
	tstring			m_strPreviewUrl;

	DeclareHandlerId(CCloudFlashHandler);
};
