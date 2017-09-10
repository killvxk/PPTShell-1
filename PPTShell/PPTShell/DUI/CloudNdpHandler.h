#pragma once


class CCloudResourceHandler;
class CCloudNdpHandler:
	public CCloudResourceHandler
{

public:
	CCloudNdpHandler();
	virtual ~CCloudNdpHandler();

protected:
	//override
	virtual void	DoButtonClick(TNotifyUI* pNotify);
	virtual void	DoSetThumbnail( TNotifyUI* pNotify );
	virtual void	DoDropDown( TNotifyUI* pNotify );
	virtual bool	OnDeleteComplete(void* pObj);

protected:
	virtual void	ReadFrom( CStream* pStream );
	virtual void	WriteTo( CStream* pStream );

	bool			OnHandlePlay(void* pObj);

private:
	tstring			m_strPreviewUrl;
	tstring			m_strDesc;
	tstring			m_strXmlUrl;

	DeclareHandlerId(CCloudNdpHandler);
};
