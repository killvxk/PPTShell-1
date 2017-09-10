#pragma once

class IComponent;
class IResourceStyleable;
class CDownloadToolBarComponent:
	public IComponent
{
public:
	CDownloadToolBarComponent();
	virtual ~CDownloadToolBarComponent();

protected:
	virtual void	DoInit( CContainerUI* pParent, IControlDelegate* pDelegate );
	virtual tstring GetDescription();
	virtual int		GetHeight();
	virtual IResourceStyleable*	GetResourceStyleable();

public:
	void			SetNeedPauseBtn(bool bNeed);

private:
	bool			m_bNeedPauseBtn;

};