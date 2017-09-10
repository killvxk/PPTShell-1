#pragma once

class IComponent;
class IResourceStyleable;
class CProgressComponent:
	public IComponent
{
public:
	CProgressComponent(IComponent* pComponent);
	virtual ~CProgressComponent();

protected:
	virtual void	DoInit( CContainerUI* pParent, IControlDelegate* pDelegate );
	virtual tstring GetDescription();
	virtual int		GetHeight();
	virtual IResourceStyleable*	GetResourceStyleable();


private:
	IComponent*				m_pComponent;
	static CDialogBuilder	m_ProgressComponentBuilder;

};