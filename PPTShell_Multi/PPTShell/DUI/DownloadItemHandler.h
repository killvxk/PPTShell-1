#pragma once

class CItemHandler;
class CResourceDownloader;
class IDownloaderVisitor;

class CDownloadItemHandler:
	public CItemHandler,
	public IDownloaderVisitor,
	public IDownloadListener
{

public:
	CDownloadItemHandler();
	virtual ~CDownloadItemHandler();

protected:
	virtual void	DoInit(CControlUI* pHolder);
	virtual void	DoDestroy(CControlUI* pHolder);
	virtual void	DoButtonClick(TNotifyUI* pNotify);

	virtual void	OnDownloadBefore( THttpNotify* pHttpNotify );
	virtual void	OnDownloadProgress( THttpNotify* pHttpNotify );
	virtual void	OnDownloadCompleted( THttpNotify* pHttpNotify );
	virtual void	OnDownloadInterpose( THttpNotify* pHttpNotify );

	//IHolderContainer
	virtual void	AddHolder( CControlUI* pHolder );
	virtual void	RemoveHolder( CControlUI* pHolder );
	
	//unused override we must optimize later
	virtual void	DoSetThumbnail( TNotifyUI* pNotify );
	virtual void	DoDropDown( TNotifyUI* pNotify );
	virtual void	ReadFrom( CStream* pStream );
	virtual void	WriteTo( CStream* pStream );
	virtual LPCTSTR GetResource();
	virtual LPCTSTR GetTitle();
	virtual LPCTSTR GetGuid();
	virtual void	InitHandlerId();
	virtual tstring GetHandlerId();
	virtual void	SetHandlerId( LPCTSTR lptcsId );

public:
	void					SetDownloader(CResourceDownloader* pDownloader);
	CResourceDownloader*	GetDownloader();

	void					SetOriginalHandler(CItemHandler* pItemHandler);


private:
	CItemHandler*			m_pOriginalItemHandler;
	CResourceDownloader*	m_pDownloader;
	ITransfer*				m_pRetryTransfer;
};
