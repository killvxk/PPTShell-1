#pragma once


class CResourceDownloader;
class CItemHandler;

class IDownloaderListener
{
public:
	virtual void			OnDownloaderCreate(CResourceDownloader* pDownloader, CItemHandler* pItemHandler)	= 0;
	virtual void			OnDownloaderDestroy(CResourceDownloader* pDownloader, bool bCancel)					= 0;
};
