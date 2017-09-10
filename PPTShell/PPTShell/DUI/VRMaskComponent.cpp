#pragma once
#include "StdAfx.h"
#include "DUI/IComponent.h"
#include "DUI/VRMaskComponent.h"
#include "DUI/ItemOption.h"
#include "DUI/ComponentCallback.h"


CVRMaskComponent::CVRMaskComponent( IComponent* pComponent )
{
	m_pComponent = pComponent;
}

CVRMaskComponent::~CVRMaskComponent()
{
	if(m_pComponent)
	{
		delete m_pComponent;
		m_pComponent = NULL;
	}
}
DuiLib::CDialogBuilder CVRMaskComponent::m_VRMaskComponentBuilder;
void CVRMaskComponent::DoInit( CContainerUI* pParent, IControlDelegate* pDelegate )
{	
	//cloud item
 	CContainerUI* pStyle = NULL;
 	if( !m_VRMaskComponentBuilder.GetMarkup()->IsValid() ) {
 		pStyle = static_cast<CContainerUI*>(m_VRMaskComponentBuilder.Create(_T("Item\\VRMaskComponent.xml"), NULL, &CComponentCallback::Callback));
 	}
 	else {
 		pStyle = static_cast<CContainerUI*>(m_VRMaskComponentBuilder.Create(&CComponentCallback::Callback));
 	}
 	pParent->Add(pStyle);

	m_pComponent->DoInit(pParent, pDelegate);
	
}

tstring CVRMaskComponent::GetDescription()
{
	return m_pComponent->GetDescription();
}

int CVRMaskComponent::GetHeight()
{
	return m_pComponent->GetHeight();
}

IResourceStyleable* CVRMaskComponent::GetResourceStyleable()
{
	return m_pComponent->GetResourceStyleable();
}




