#pragma once

class IUploadListener
{
public:
	virtual void			OnUploadBefore(THttpNotify* pHttpNotify)		= 0;
	virtual void			OnUploadProgress(THttpNotify* pHttpNotify)		= 0;
	virtual void			OnUploadCompleted(THttpNotify* pHttpNotify)		= 0;

};