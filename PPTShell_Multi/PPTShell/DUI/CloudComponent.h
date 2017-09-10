#pragma once

class IComponent;
class IResourceStyleable;
class CCloudComponent:
	public IComponent
{
public:
	CCloudComponent(IComponent* pComponent);
	virtual ~CCloudComponent();

	virtual void	DoInit( CContainerUI* pParent, IControlDelegate* pDelegate );

	virtual tstring GetDescription();

	virtual int		GetHeight();

	virtual IResourceStyleable*	GetResourceStyleable();

private:
	IComponent*				m_pComponent;
	static CDialogBuilder	m_CloudComponentBuilder;

};