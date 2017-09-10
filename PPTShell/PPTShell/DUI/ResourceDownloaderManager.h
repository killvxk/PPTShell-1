#pragma once



class IDownloaderListener;
class CResourceDownloader;
class CItemHandler;
class CResourceDownloaderManager
{
private:
	CResourceDownloaderManager();
	virtual ~CResourceDownloaderManager();

public:
	//Listener
	void			AddListener(vector<IDownloaderListener*>* pListeners);
	void			AddListener(IDownloaderListener* pListener);
	void			RemoveListener(IDownloaderListener* pListener);


	
	void			NotifyDownloaderCreate(CResourceDownloader* pDownloader, CItemHandler* pItemHandler, bool bNotify = true);
	void			NotifyDownloaderDestroy(CResourceDownloader* pDownloader, int nResult);

	bool			IsDownloaderExists(CResourceDownloader* pDownloader);

	map<CResourceDownloader*, CItemHandler*>*
					GetDownloaders();

	static CResourceDownloaderManager*
					GetInstance();


private:
	static CResourceDownloaderManager*				m_pThis;
	map<IDownloaderListener*, IDownloaderListener*> m_mapListener;
	map<CResourceDownloader*, CItemHandler*>		m_mapDownloader;
};


