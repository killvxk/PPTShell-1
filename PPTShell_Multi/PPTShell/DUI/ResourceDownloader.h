#pragma once


class ITransfer;
class IDownloadListener;
class IBaseParamer;

class CResourceDownloader:
	public ITransfer,
	public IResourceDescriber
{

public:
	CResourceDownloader();
	virtual ~CResourceDownloader();
	void SetIsSelfFree(bool isSelfFree);
public:
	void			SetParamer(IBaseParamer* pParamer);

	//Listener
	void			AddListener(vector<IDownloadListener*>* pListeners);
	void			AddListener(IDownloadListener* pListener);
	void			RemoveListener(IDownloadListener* pListener);

	bool			OnDownloadResourceProgress(void* pObj);
	bool			OnDownloadResourceCompleted(void* pObj);
	bool			OnDownloadResourcePause(void* pObj);

protected:
	void			ClearListener();
	void			NotifyDownloadInterpose( int nInterpose );


protected:
	IBaseParamer*	m_pParamer;
	//for copy paramer on down fail
	IBaseParamer*	m_pCopyParamer;
	bool			m_bSelfFree;//标识是否自己释放

	map<IDownloadListener*, IDownloadListener*> m_mapListener;
};
