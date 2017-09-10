#pragma once
#include "StdAfx.h"
#include "DUI/IComponent.h"
#include "DUI/DownloadComponent.h"
#include "DUI/ComponentCallback.h"


CDownloadComponent::CDownloadComponent( IComponent* pComponent )
{
	m_pComponent = pComponent;
}

CDownloadComponent::~CDownloadComponent()
{
	if(m_pComponent)
	{
		delete m_pComponent;
		m_pComponent = NULL;
	}
}
DuiLib::CDialogBuilder CDownloadComponent::m_DownloadComponentBuilder;
void CDownloadComponent::DoInit( CContainerUI* pParent, IControlDelegate* pDelegate )
{	
	//download item
 	CContainerUI* pStyle = NULL;
 	if( !m_DownloadComponentBuilder.GetMarkup()->IsValid() ) {
 		pStyle = static_cast<CContainerUI*>(m_DownloadComponentBuilder.Create(_T("Item\\Download\\DownloadComponent.xml"), NULL, &CComponentCallback::Callback));
 	}
 	else {
 		pStyle = static_cast<CContainerUI*>(m_DownloadComponentBuilder.Create(&CComponentCallback::Callback));
 	}
 	pParent->Add(pStyle);

	m_pComponent->DoInit(pStyle, pDelegate);
	
}

tstring CDownloadComponent::GetDescription()
{
	return _T("Download") + m_pComponent->GetDescription();
}

int CDownloadComponent::GetHeight()
{
	return m_pComponent->GetHeight() + 60;
}

IResourceStyleable* CDownloadComponent::GetResourceStyleable()
{
	return m_pComponent->GetResourceStyleable();
}


