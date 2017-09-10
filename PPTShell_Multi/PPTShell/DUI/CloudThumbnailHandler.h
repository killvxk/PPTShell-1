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
