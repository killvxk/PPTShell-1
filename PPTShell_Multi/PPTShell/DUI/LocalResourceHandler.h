#pragma once


class CItemHandler;
class IUploadListener;
class IThumbnailListener;
class CLocalResourceHandler:
	public CItemHandler
{

public:
	CLocalResourceHandler();
	virtual ~CLocalResourceHandler();

protected:
	virtual void	DoClick(TNotifyUI*	pNotify);
	virtual void	DoRClick(TNotifyUI*	pNotify);
	virtual void	DoButtonClick(TNotifyUI* pNotify);
	virtual void	DoMenuClick(TNotifyUI*	pNotify);

protected:
	//download
	virtual DWORD	UploadResourceInternal(LPCSTR lptcsUrl, LPCSTR lptcsSessionId, LPCSTR lptcsUploadGuid, LPCSTR lptcsServerFilePath);
	virtual bool	UploadResource(LPCTSTR lptcsServer);
	virtual bool	OnUploadResourceRequestCompleted(void* pObj);
	virtual bool	OnUploadResourceProgress(void* pObj);
	virtual bool	OnUploadResourceCompleted(void* pObj);
	virtual bool	OnUploadResourceCommitted(void* pObj);
	

	void			SetUploading(bool bIsUploading);
	bool			IsUploading();

	IUploadListener*	GetUploadListener();
	IThumbnailListener*	GetThumbnailListener();

public:
	virtual void		ReadStream( CStream* pStream );

	LPCTSTR				GetResource();
	LPCTSTR				GetTitle();
	

protected:
	tstring				m_strPath;
	tstring				m_strTitle;

protected:


private:
	bool				m_bIsDownloading;
	IUploadListener*	m_pUploadListener;
	IThumbnailListener*	m_pThumbnailListener;

};
