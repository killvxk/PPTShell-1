#pragma once

class IComponent;
class IResourceStyleable;
class CItemComponent:
	public IComponent
{
public:
	CItemComponent( IComponent* pComponent );
	CItemComponent( IResourceStyleable* pStyleable );
	virtual ~CItemComponent();

	virtual void	DoInit( CContainerUI* pParent, IControlDelegate* pDelegate );

	virtual tstring GetDescription();

	virtual int		GetHeight();

	virtual IResourceStyleable*	GetResourceStyleable();

private:
	IComponent*				m_pComponent;
	IResourceStyleable*		m_pStyleable;

};