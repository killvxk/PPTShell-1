#pragma once


class CResourceDownloader;
class CAssetDownloader:
	public CResourceDownloader
{

public:
	CAssetDownloader();
	virtual ~CAssetDownloader();

	//for image
	void	SetThumbnailSize(int nSize);

	void	SetAssetType(int nType);
	void	SetAssetGuid(LPCTSTR lptcstr);
	void	SetAssetUrl(LPCTSTR lptcstr);
	void	SetAssetTitle(LPCTSTR lptcstr);
	void	SetAssetMD5(LPCTSTR lptcstr);

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
	//these ptr can't be used after being transfered.
	LPCTSTR	m_pGuid;
	LPCTSTR	m_pUrl;
	LPCTSTR	m_pTitle;
	LPCTSTR m_pMD5;

	int		m_nAssetType;
	DWORD	m_dwTransferId;

private:
	int		m_nThumbnailSize;


};
