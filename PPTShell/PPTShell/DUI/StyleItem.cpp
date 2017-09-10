#include "StdAfx.h"
#include "DUI/StyleItem.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IDownloadListener.h"
CStyleItemUI::CStyleItemUI( IComponent* pComponent )
{
	m_pComponent	= pComponent;
	m_pHandler		= NULL;
}


CStyleItemUI::~CStyleItemUI()
{
	if(m_pComponent)
	{
		delete m_pComponent;
		m_pComponent = NULL;
	}

	if (GetHandler())
	{
		GetHandler()->DoDestroy(this);
		SetHandler(NULL);
	}
}

void CStyleItemUI::DoInit()
{
	__super::DoInit();

	if(m_pComponent)
	{
		m_pComponent->DoInit(this, this);
		SetItemHeight(m_pComponent->GetHeight());
	}

	if (m_pHandler)
	{
		m_pHandler->SetTrigger(this);
		m_pHandler->DoInit(this);
	}
	
}

void CStyleItemUI::SetItemHeight( int nHeight )
{
	this->SetFixedHeight(nHeight);
}

void CStyleItemUI::SetItemWidth( int nWidth )
{
	this->SetFixedWidth(nWidth);
}

IComponent* CStyleItemUI::GetComponent()
{
	return m_pComponent;
}

CItemHandler* CStyleItemUI::GetHandler()
{
	return m_pHandler;
}

void CStyleItemUI::SetHandler( CItemHandler* pHandler )
{
	m_pHandler = pHandler;
	if (m_pHandler)
	{
		m_pHandler->AddHolder(this);
	}
}
