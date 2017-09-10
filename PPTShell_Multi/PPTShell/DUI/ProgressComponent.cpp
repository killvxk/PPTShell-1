#pragma once
#include "StdAfx.h"
#include "DUI/IComponent.h"
#include "DUI/ProgressComponent.h"
#include "DUI/ComponentCallback.h"



CProgressComponent::CProgressComponent( IComponent* pComponent )
{
	m_pComponent = pComponent;
}

CProgressComponent::~CProgressComponent()
{
	if(m_pComponent)
	{
		delete m_pComponent;
		m_pComponent = NULL;
	}
}
DuiLib::CDialogBuilder CProgressComponent::m_ProgressComponentBuilder;
void CProgressComponent::DoInit( CContainerUI* pParent, IControlDelegate* pDelegate )
{	
	CContainerUI* pCenterContainer = (CContainerUI*)pParent->FindSubControl(_T("center"));

	//download item
 	CContainerUI* pStyle = NULL;
 	if( !m_ProgressComponentBuilder.GetMarkup()->IsValid() ) {
 		pStyle = static_cast<CContainerUI*>(m_ProgressComponentBuilder.Create(_T("Item\\Download\\ProgressComponent.xml"), NULL, &CComponentCallback::Callback));
 	}
 	else {
 		pStyle = static_cast<CContainerUI*>(m_ProgressComponentBuilder.Create(&CComponentCallback::Callback));
 	}
 	pCenterContainer->Add(pStyle);

	m_pComponent->DoInit(pParent, pDelegate);
	
}

tstring CProgressComponent::GetDescription()
{
	return _T("Progress") + m_pComponent->GetDescription();
}

int CProgressComponent::GetHeight()
{
	return m_pComponent->GetHeight();
}

IResourceStyleable* CProgressComponent::GetResourceStyleable()
{
	return m_pComponent->GetResourceStyleable();
}


