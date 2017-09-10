#pragma once


class CQuestionDownloader;
class CInteractQuestionDownloader:
	public CQuestionDownloader
{

public:
	CInteractQuestionDownloader();
	virtual ~CInteractQuestionDownloader();

	//ITransfer
	virtual bool		Transfer();
	virtual void		Cancel();
	virtual void		Pause();
	virtual void		Resume();
	virtual ITransfer*	Copy();

	virtual bool CancelTransfer();

	//IResourceDescriber
	virtual int		GetResourceType();


};

