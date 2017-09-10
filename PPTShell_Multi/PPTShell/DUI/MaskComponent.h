#pragma once

class IResourceStyleable;
class IComponent;
class CMaskComponent:
	public IComponent
{
public:
	CMaskComponent(IComponent* pComponent);
	CMaskComponent(IResourceStyleable* pStyleable);
	virtual ~CMaskComponent();

protected:
	//IComponent
	virtual void	DoInit( CContainerUI* pParent, IControlDelegate* pDelegate );
	virtual tstring GetDescription();
	virtual int		GetHeight();

	virtual IResourceStyleable* GetResourceStyleable();


private:
	IComponent*				m_pComponent;
	IResourceStyleable*		m_pStyleable;
};