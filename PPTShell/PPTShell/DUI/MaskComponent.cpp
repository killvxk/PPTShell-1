#pragma once
#include "StdAfx.h"
#include "DUI/IComponent.h"
#include "DUI/MaskComponent.h"
#include "DUI/ComponentCallback.h"


CMaskComponent::CMaskComponent( IComponent* pComponent )
{
	m_pComponent	= pComponent;
	m_pStyleable	= NULL;
}

CMaskComponent::CMaskComponent( IResourceStyleable* pStyleable )
{
	m_pStyleable	= pStyleable;
	m_pComponent	= NULL;
}

CMaskComponent::~CMaskComponent()
{
	if(m_pComponent)
	{
		delete m_pComponent;
		m_pComponent = NULL;
	}
}

void CMaskComponent::DoInit( CContainerUI* pParent, IControlDelegate* pDelegate )
{
	//<VerticalLayout normalimage="file='Item/btn_bk_normal.png'  corner='1,1,1,1'" name="mask" visible="false">
	//	<Control />
	//	<HorizontalLayout height="36">
	//	<Control />
	//	<GifAnim name="loading" width="36" height="36" bkimage="Item/item_loading.gif" autoplay="true"/>
	//	<Control />
	//	</HorizontalLayout>
	//	<Label name="loading_tip"  text="加载中..." padding="0,4,0,0" textcolor="#FFFFFFFF" align="center" />
	//	<Control />
	//</VerticalLayout>

	//<VerticalLayout>
	CVerticalLayoutUI* pMask = new CVerticalLayoutUI();
	pMask->SetVisible(false);
	pMask->SetName(_T("mask"));
	
	CControlUI* pCtrl = new CControlUI;
	pMask->Add(pCtrl);

	//<HorizontalLayout height="36">
	CHorizontalLayoutUI* pLoadingLayout = new CHorizontalLayoutUI();
	pLoadingLayout->SetFixedHeight(36);


	pCtrl = new CControlUI;
	pLoadingLayout->Add(pCtrl);

 	CGifAnimUI* pLoading = new CGifAnimUI;
	pLoading->SetName(_T("loading"));
	pLoading->SetFixedHeight(36);
	pLoading->SetFixedWidth(36);
	pLoading->SetBkImage(_T("Item/item_loading.gif"));
	pLoading->SetAutoPlay(true);
 	pLoadingLayout->Add(pLoading);
	

	pCtrl = new CControlUI;
	pLoadingLayout->Add(pCtrl);

	pMask->Add(pLoadingLayout);
	//</HorizontalLayout>


	CLabelUI* pTip = new CLabelUI();
	pTip->SetName(_T("loading_tip"));
	pTip->SetText(_T("加载中.."));
	pTip->SetTextColor(0xFFFFFFFF);
	pTip->SetFont(120000);
	pTip->SetAttribute(_T("align"), _T("center"));

	pMask->Add(pTip);

	pCtrl = new CControlUI;
	pMask->Add(pCtrl);

	//</VerticalLayout>
	pParent->Add(pMask);


	if (m_pComponent)
	{
		m_pComponent->DoInit(pParent, pDelegate);
	}
	
}

tstring CMaskComponent::GetDescription()
{
	if (m_pComponent)
	{
		return _T("Mask") + m_pComponent->GetDescription();
	}

	return _T("Mask");
}

int CMaskComponent::GetHeight()
{
	int nHeight = 0;
	if (m_pComponent)
	{
		nHeight += m_pComponent->GetHeight();
	}
	return nHeight;
}

IResourceStyleable* CMaskComponent::GetResourceStyleable()
{
	if (m_pComponent)
	{
		return m_pComponent->GetResourceStyleable();
	}
	return m_pStyleable;
}



