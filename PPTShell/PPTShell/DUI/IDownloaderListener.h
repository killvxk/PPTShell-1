#pragma once


class CResourceDownloader;
class CItemHandler;

enum DestroyResult{
	eResult_Error	=-1,
	eResult_Normal	=0,
	eResult_Cancel	=1,

};

class IDownloaderListener
{
public:
	virtual void			OnDownloaderCreate(CResourceDownloader* pDownloader, CItemHandler* pItemHandler)	= 0;
	virtual void			OnDownloaderDestroy(CResourceDownloader* pDownloader, int nResult)					= 0;
};
