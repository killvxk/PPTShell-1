#pragma once


class CItemHandler;
class IDownloadListener;
class IThumbnailListener;
class IBaseParamer;
class CResourceDownloader;
class IDownloaderVisitor;
class IDownloaderRefresher;

class CCloudResourceHandler:
	public CItemHandler,
	public IDownloaderVisitor,
	public IDownloaderRefresher,
	public IDownloadListener
{

public:
	CCloudResourceHandler();
	virtual ~CCloudResourceHandler();
	//place holder
	virtual	bool			GetPlaceHolderId(
		LPCTSTR				szPath,
		int					nLeft	= -1,
		int					nTop	= -1,
		int					nWidth	= -1,
		int					nHeight = -1,
		int					nSlideIdx = 0);
protected:
	virtual void	DoClick(TNotifyUI*	pNotify);
	virtual void	DoRClick(TNotifyUI*	pNotify);
	virtual void	DoButtonClick(TNotifyUI* pNotify);
	virtual void	DoMenuClick(TNotifyUI*	pNotify);
	virtual void	DoKeyDown( TEventUI* pNotify );
	virtual void	DoInit(CControlUI* pHolder);
	virtual void	DoDestroy(CControlUI* pHolder);

	void			DestroyPossible();

protected:
	//download
	virtual void			OnDownloadBefore( THttpNotify* pHttpNotify );
	virtual void			OnDownloadProgress( THttpNotify* pHttpNotify );
	virtual void			OnDownloadCompleted( THttpNotify* pHttpNotify );
	virtual void			OnDownloadInterpose(THttpNotify* pHttpNotify);

	//First,call GetDownloader to check whether if pDownloader is existed.
	//Then,call between pDownloader invoke transfer and pDownloader data been setted.
	void					SetDownloader(CResourceDownloader* pDownloader);
	CResourceDownloader*	GetDownloader();

	void					RefreshDownloader( CResourceDownloader* pDownloader );

	void					SetThumbnailDownloader(CResourceDownloader* pDownloader);
	CResourceDownloader*	GetThumbnailDownloader();

	void					RemoveDownloader(CResourceDownloader* pDownloader, bool bCancel = false);
	bool					HasDownloaders();

	//IHolderContainer
	virtual void			AddHolder( CControlUI* pHolder );
	virtual void			RemoveHolder( CControlUI* pHolder );

	//listener
	void						NotifyThumbnailCompleted(THttpNotify* pHttpNotify);
	vector<IDownloadListener*> 	GetDownloadListeners();

	//conent service
	void					Rename();
	virtual bool			OnDeleteComplete(void* pObj);
	bool					OnRenameComplete(void* pObj);
	void					AddContentService(DWORD dwServiceId);
	void					RemoveContentService(DWORD dwServiceId);
	bool					HasContentService();


	virtual	bool			OnGetPlaceHolderCompleted(void* pObj);

public:

	virtual void			ReadFrom( CStream* pStream );
	virtual void			WriteTo( CStream* pStream );

	//resource
	LPCTSTR					GetResource();
	LPCTSTR					GetTitle();
	LPCTSTR					GetGuid();
	LPCTSTR					GetMD5();

	//menu
	void					SetIsDbank(bool bIsDbank);
	bool					IsDbank();

	//conent service
	void					SetContentType(int nType);
	int						GetContentType();



protected:
	tstring				m_strUrl;
	tstring				m_strGuid;
	tstring				m_strTitle;
	tstring				m_strMD5;
	tstring				m_strJsonInfo;
	bool isDeleting;
private:
	//conent service
	int								m_nConentType;
	map<DWORD, DWORD>				m_mapContentService;
	bool							m_bIsDBank;
	//download
	CResourceDownloader*			m_pCurrentDownloader;
	CResourceDownloader*			m_pThumbnailDownloader;
	IThumbnailListener*				m_pThumbnailListener;

	int								m_nGettingPlaceHolderReference;

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

#define DeclareHandlerId()\
private:\
	tstring m_strHandlerId;\
public:\
	virtual void	 InitHandlerId(){\
		m_strHandlerId= typeid(this).name();\
		m_strHandlerId += GetGuid();\
	}\
	virtual void	 SetHandlerId(LPCTSTR lptcsId){\
		m_strHandlerId = lptcsId;\
	}\
	virtual tstring	 GetHandlerId(){\
		return m_strHandlerId;\
	}