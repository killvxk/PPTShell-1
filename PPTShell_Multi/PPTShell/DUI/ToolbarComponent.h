#pragma once

class IResourceStyleable;
class IComponent;
class CToolbarComponent:
	public IComponent
{
public:
	CToolbarComponent(IResourceStyleable* pStyleable);
	virtual ~CToolbarComponent();

protected:
	//IComponent
	virtual void	DoInit( CContainerUI* pParent, IControlDelegate* pDelegate );
	virtual tstring GetDescription();
	virtual int		GetHeight();
	virtual IResourceStyleable* GetResourceStyleable();

public:
	//¡Ÿ ±‘ˆº”
	void			SetNeedInterposeBtn(bool bNeed);

private:
	static CDialogBuilder	m_ItemToolbarBuilder;
	IResourceStyleable*		m_pStyleable;
	bool					m_bNeedInterposeBtn;
};