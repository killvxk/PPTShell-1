#pragma once


class CCloudResourceHandler;
class CCloudVideoHandler  :
	public CCloudResourceHandler
{

public:
	CCloudVideoHandler();
	virtual ~CCloudVideoHandler();

protected:
	//override
	virtual void	DoButtonClick(TNotifyUI* pNotify);
	virtual void	DoSetThumbnail( TNotifyUI* pNotify );
	virtual void	DoDropDown( TNotifyUI* pNotify );
	virtual bool	OnDeleteComplete(void* pObj);

protected:
	virtual void	ReadFrom( CStream* pStream );
	virtual void	WriteTo( CStream* pStream );

	bool			OnGetPlaceHolderCompleted(void* pObj);
	bool			OnHandleThumbnail(void* pObj);
	bool			OnHandleInsert(void* pObj);
	bool			OnHandlePreivew(void* pObj);

public:
	void			SetHasPreviewUrl(bool bHas);
	

private:
	tstring			m_strPreviewUrl;
	bool			m_bHasPreviewUrl;

	DeclareHandlerId(CCloudVideoHandler);

};
