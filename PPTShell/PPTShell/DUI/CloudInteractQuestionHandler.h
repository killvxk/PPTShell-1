#pragma once


class CCloudResourceHandler;
class CCloudInteractQuestionHandler:
	public CCloudResourceHandler
{

public:
	CCloudInteractQuestionHandler();
	virtual ~CCloudInteractQuestionHandler();
	tstring GetQuestionName();
protected:
	//override
	virtual void	DoRClick(TNotifyUI*	pNotify);
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

private:
	tstring			m_strPreviewUrl;
	tstring			m_strDesc;
	tstring			m_strQuestionName;
	tstring			m_strXmlUrl;

	DeclareHandlerId(CCloudInteractQuestionHandler);
};
