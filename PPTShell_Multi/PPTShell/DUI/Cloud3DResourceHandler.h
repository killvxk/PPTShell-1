#pragma once


class CCloudResourceHandler;
class CCloud3DResourceHandler:
	public CCloudResourceHandler
{

public:
	CCloud3DResourceHandler();
	virtual ~CCloud3DResourceHandler();

protected:
	//override
	virtual void	DoButtonClick(TNotifyUI* pNotify);
	virtual void	DoSetThumbnail( TNotifyUI* pNotify );
	virtual void	DoDropDown( TNotifyUI* pNotify );

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
	tstring			m_strThumbnailPath;

private:
	tstring m_strHandlerId;
public:
	virtual void	 InitHandlerId(){
		m_strHandlerId= typeid(this).name();
		m_strHandlerId += GetGuid();
	}
	virtual void	 SetHandlerId(LPCTSTR lptcsId){
		m_strHandlerId = lptcsId;
	}
	virtual tstring	 GetHandlerId(){
		return m_strHandlerId;
	}
};
