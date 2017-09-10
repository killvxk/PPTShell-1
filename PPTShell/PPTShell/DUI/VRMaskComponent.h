#pragma once

class IComponent;
class IResourceStyleable;
class CVRMaskComponent:
	public IComponent
{
public:
	CVRMaskComponent(IComponent* pComponent);
	virtual ~CVRMaskComponent();

	virtual void	DoInit( CContainerUI* pParent, IControlDelegate* pDelegate );

	virtual tstring GetDescription();

	virtual int		GetHeight();

	virtual IResourceStyleable*	GetResourceStyleable();

private:
	IComponent*				m_pComponent;
	static CDialogBuilder	m_VRMaskComponentBuilder;

};