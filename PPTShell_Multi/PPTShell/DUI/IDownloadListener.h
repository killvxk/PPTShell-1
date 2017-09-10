#pragma once

enum DownloadInterposeType
{
	eInterpose_Unkown	= 0,
	eInterpose_Pause,
	eInterpose_Resume,
	eInterpose_Cancel,
};

class IDownloadListener
{
public:
	//unused now
	virtual void			OnDownloadBefore(THttpNotify* pHttpNotify)		= 0;

	virtual void			OnDownloadProgress(THttpNotify* pHttpNotify)	= 0;
	virtual void			OnDownloadCompleted(THttpNotify* pHttpNotify)	= 0;
	virtual void			OnDownloadInterpose(THttpNotify* pHttpNotify)	= 0;

};
