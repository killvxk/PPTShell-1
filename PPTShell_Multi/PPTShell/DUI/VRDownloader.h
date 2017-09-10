#pragma once


class CAssetDownloader;
class CVRDownloader:
	public CAssetDownloader
{

public:
	CVRDownloader();
	virtual ~CVRDownloader();

	virtual bool		Transfer();
	virtual void		Cancel();
	virtual void		Pause();
	virtual void		Resume();
	virtual ITransfer*	Copy();

protected:
	bool			OnPermissionObtained(void* param);
	bool			OnSessionObtained(void* param);

protected:
	DWORD			m_dwSessionId;
	DWORD			m_dwPermissionId;
};

