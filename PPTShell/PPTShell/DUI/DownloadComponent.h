#pragma once

class IComponent;
class IResourceStyleable;
class CDownloadComponent:
	public IComponent
{
public:
	CDownloadComponent(IComponent* pComponent);
	virtual ~CDownloadComponent();

protected:
	virtual void	DoInit( CContainerUI* pParent, IControlDelegate* pDelegate );
	virtual tstring GetDescription();
	virtual int		GetHeight();
	virtual IResourceStyleable*	GetResourceStyleable();


private:
	IComponent*				m_pComponent;
	static CDialogBuilder	m_DownloadComponentBuilder;

};