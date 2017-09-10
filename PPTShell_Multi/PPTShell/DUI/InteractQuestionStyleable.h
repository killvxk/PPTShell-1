#pragma once

class CResourceStyleable;
class CInteractQuestionStyleable:
	public CResourceStyleable
{
public:
	CInteractQuestionStyleable();
	virtual ~CInteractQuestionStyleable();

	virtual int				GetButtonCount();
	virtual LPCTSTR			GetButtonText( int nButton );
	virtual LPCTSTR			GetDescription();
	virtual LPCTSTR			GetDefaultBkIamge();
	virtual LPCTSTR			GetIcon();

	


};