#pragma once

class IComponent;
class CLocalComponent:
	public IComponent
{
public:
	CLocalComponent(IComponent* pComponent);
	virtual ~CLocalComponent();

	virtual void	DoInit( CContainerUI* pParent, IControlDelegate* pDelegate );

private:
	IComponent*				m_pComponent;
	static CDialogBuilder	m_ChapterStyleBuilder;

};