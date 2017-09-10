#pragma once
#include "StdAfx.h"
#include "DUI/IComponent.h"
#include "DUI/LocalComponent.h"


DuiLib::CDialogBuilder CLocalComponent::m_ChapterStyleBuilder;
CLocalComponent::CLocalComponent( IComponent* pComponent )
{
	m_pComponent = pComponent;
}

CLocalComponent::~CLocalComponent()
{
	m_pComponent = NULL;
}

void CLocalComponent::DoInit( CContainerUI* pParent, IControlDelegate* pDelegate )
{
	CHorizontalLayoutUI* pChapterLayout = NULL;
	if (!m_ChapterStyleBuilder.GetMarkup()->IsValid())
	{
		pChapterLayout = dynamic_cast<CHorizontalLayoutUI*>(m_ChapterStyleBuilder.Create(_T("Item\\LocalChapterStyle.xml")));

	}
	else
	{
		pChapterLayout = dynamic_cast<CHorizontalLayoutUI*>(m_ChapterStyleBuilder.Create());
	}
	pParent->Add(pChapterLayout);
	m_pComponent->DoInit(pParent, pDelegate);
}


