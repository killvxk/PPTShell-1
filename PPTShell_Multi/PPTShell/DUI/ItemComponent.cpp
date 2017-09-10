#pragma once
#include "StdAfx.h"
#include "DUI/IComponent.h"
#include "DUI/ItemComponent.h"
#include "DUI/ItemOption.h"


CItemComponent::CItemComponent( IComponent* pComponent)
{
	m_pComponent	= pComponent;
	m_pStyleable	= NULL;
}

CItemComponent::CItemComponent( IResourceStyleable* pStyleable )
{
	m_pComponent	= NULL;
	m_pStyleable	= pStyleable;
}

CItemComponent::~CItemComponent()
{
	if(m_pComponent)
	{
		delete m_pComponent;
		m_pComponent = NULL;
	}
}

void CItemComponent::DoInit( CContainerUI* pParent, IControlDelegate* pDelegate )
{	
	//item

// 	<Control      name="image"  bkimage="Item/item_normal.png" />
// 	<ItemOption name="item"   pushedimage="file='Item/item_border_pushed.png'  corner='5,5,5,5'" normalimage="file='Item/item_border_normal.png'  corner='5,5,5,5'" selectedimage="file='Item/item_border_pushed.png'  corner='5,5,5,5'"/>

	CControlUI* pImage		= new CControlUI();
	pImage->SetBkColor(0xFFFFFFFF);
	pImage->SetName(_T("image"));
	if (GetResourceStyleable())
	{
		pImage->SetBkImage(GetResourceStyleable()->GetDefaultBkIamge());
	}
	else if (m_pStyleable)
	{
		pImage->SetBkImage(m_pStyleable->GetDefaultBkIamge());
	}

	CItemOptionUI* pItem	= new CItemOptionUI();
	pItem->SetName(_T("item"));
	pItem->SetPushedImage(_T("Item/item_border_pushed.png'  corner='5,5,5,5'"));
	pItem->SetNormalImage(_T("file='Item/item_border_normal.png'  corner='5,5,5,5'"));
	pItem->SetSelectedImage(_T("file='Item/item_border_pushed.png'  corner='5,5,5,5'"));
	

	pParent->Add(pImage);
	pParent->Add(pItem);


	pItem->OnNotify	+= MakeDelegate(pDelegate, &IControlDelegate::OnControlNotify);

	if (m_pComponent)
	{
		m_pComponent->DoInit(pParent, pDelegate);
	}
}

tstring CItemComponent::GetDescription()
{
	tstring strDescription = _T("Item");
	if (m_pComponent)
	{
		strDescription +=  m_pComponent->GetDescription();
	}

	return strDescription;
	
}

int CItemComponent::GetHeight()
{
	int nHeight = 0;
	if (m_pComponent)
	{
		nHeight += m_pComponent->GetHeight();
	}
	return nHeight;
}

IResourceStyleable* CItemComponent::GetResourceStyleable()
{
	if (m_pComponent)
	{
		return m_pComponent->GetResourceStyleable();
	}

	return NULL;
}




