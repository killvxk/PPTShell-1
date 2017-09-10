#pragma once

class IComponent;
class IResourceStyleable;
class CCloudComponent:
	public IComponent
{
public:
	CCloudComponent(IComponent* pComponent, LPCTSTR lptcsType = NULL);
	virtual ~CCloudComponent();

protected:
	virtual void	DoInit( CContainerUI* pParent, IControlDelegate* pDelegate );

	virtual tstring GetDescription();

	virtual int		GetHeight();

	virtual IResourceStyleable*	GetResourceStyleable();

private:
	tstring					m_strTagType;
	IComponent*				m_pComponent;
	static CDialogBuilder	m_CloudComponentBuilder;

};