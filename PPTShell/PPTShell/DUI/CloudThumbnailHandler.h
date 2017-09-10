#pragma once


class CCloudPhotoHandler;
class CCloudThumbnailHandler:
	public CCloudPhotoHandler
{

public:
	CCloudThumbnailHandler();
	virtual ~CCloudThumbnailHandler();

protected:
	//override
	virtual void	DoClick( TNotifyUI* pNotify );

	virtual void	ReadFrom( CStream* pStream );


	DeclareHandlerId(CCloudThumbnailHandler);
};
