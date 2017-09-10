#pragma once


class CCloudResourceHandler;
class CCloudNdpXHandler:
	public CCloudResourceHandler
{

public:
	CCloudNdpXHandler();
	virtual ~CCloudNdpXHandler();

protected:
	//override
	virtual void	DoButtonClick(TNotifyUI* pNotify);
	virtual void	DoSetThumbnail( TNotifyUI* pNotify );
	virtual void	DoDropDown( TNotifyUI* pNotify );
	virtual bool	OnDeleteComplete(void* pObj);

protected:
	virtual void	ReadFrom( CStream* pStream );
	virtual void	WriteTo( CStream* pStream );

	bool			OnHandleOpen(void* pObj);

private:
	tstring			m_strPreviewUrl;
	tstring			m_strDesc;
	tstring			m_strQuestionName;
	tstring			m_strXmlUrl;

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
