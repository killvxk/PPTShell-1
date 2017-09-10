#pragma once


class CResourceDownloader;
class CNdpDownloader:
	public CResourceDownloader
{

public:
	CNdpDownloader();
	virtual ~CNdpDownloader();

	//for image
	void			SetNdpGuid(LPCTSTR lptcstr);
	void			SetNdpTitle(LPCTSTR lptcstr);

	//ITransfer
	virtual bool		Transfer();
	virtual void		Cancel();
	virtual void		Pause();
	virtual void		Resume();
	virtual ITransfer*	Copy();

	//IResourceDescriber
	virtual int		GetResourceType();
	virtual	LPCTSTR	GetResourceTitle();
	virtual	LPCTSTR	GetResourceGuid();

protected:
	bool			GetNdpDownloadUrl();
	bool			OnGetNdpDownloadUrl(void* pObj);
	
protected:
	//these ptr can't be used after being transfered.
	LPCTSTR			m_pGuid;
	LPCTSTR			m_pTitle;


	DWORD			m_dwGetNdpDownloadUrlId;
	DWORD			m_dwTransferId;



};

