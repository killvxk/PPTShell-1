#pragma once


class CAssetDownloader;
class CCourseDownloader:
	public CAssetDownloader
{

public:
	CCourseDownloader();
	virtual ~CCourseDownloader();

	virtual bool	Transfer();
	virtual int		GetResourceType();
};
