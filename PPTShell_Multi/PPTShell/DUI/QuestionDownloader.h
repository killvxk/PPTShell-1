#pragma once


class CResourceDownloader;
class CQuestionDownloader:
	public CResourceDownloader
{

public:
	CQuestionDownloader();
	virtual ~CQuestionDownloader();

	//for image
	void	SetQuestionGuid(LPCTSTR lptcstr);
	void	SetQuestionTitle(LPCTSTR lptcstr);

	//ITransfer
	virtual bool		Transfer();
	virtual void		Cancel();
	virtual void		Pause();
	virtual void		Resume();
	virtual ITransfer*	Copy();

	virtual bool CancelTransfer();

	//IResourceDescriber
	virtual int		GetResourceType();
	virtual	LPCTSTR	GetResourceTitle();
	virtual	LPCTSTR	GetResourceGuid();

protected:
	//these ptr can't be used after being transfered.
	LPCTSTR		m_pGuid;
	LPCTSTR		m_pTitle;

};

