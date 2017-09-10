#pragma once


#include "DUI/IComponent.h"
#include "DUI/ItemHandler.h"
#include "DUI/IVisitor.h"
#include "DUI/IComponent.h"

class CStyleItemUI : 
	public CContainerUI,
	public IHandlerVisitor,
	public IControlDelegate
{
public:
	CStyleItemUI(IComponent* pComponent);
	virtual ~CStyleItemUI();

protected:
	//super
	virtual	void			DoInit();

public:
	//IHandlerVisitor
	virtual CItemHandler*	GetHandler();
	virtual void			SetHandler( CItemHandler* pHandler );

	void					SetItemHeight(int nHeight);
	void					SetItemWidth(int nWidth);

	IComponent*				GetComponent();


private:
	IComponent*				m_pComponent;
	CItemHandler*			m_pHandler;

};
