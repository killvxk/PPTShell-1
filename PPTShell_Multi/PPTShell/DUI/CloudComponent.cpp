#pragma once
#include "StdAfx.h"
#include "DUI/IComponent.h"
#include "DUI/CloudComponent.h"
#include "DUI/ItemOption.h"


CCloudComponent::CCloudComponent( IComponent* pComponent )
{
	m_pComponent = pComponent;
}

CCloudComponent::~CCloudComponent()
{
	if(m_pComponent)
	{
		delete m_pComponent;
		m_pComponent = NULL;
	}
}
DuiLib::CDialogBuilder CCloudComponent::m_CloudComponentBuilder;
void CCloudComponent::DoInit( CContainerUI* pParent, IControlDelegate* pDelegate )
{	
	//cloud item
// 	CContainerUI* pStyle = NULL;
// 	if( !m_CloudComponentBuilder.GetMarkup()->IsValid() ) {
// 		pStyle = static_cast<CContainerUI*>(m_CloudComponentBuilder.Create(_T("Item\\CloudComponent.xml"), NULL, &CComponentCallback::Callback));
// 	}
// 	else {
// 		pStyle = static_cast<CContainerUI*>(m_CloudComponentBuilder.Create(&CComponentCallback::Callback));
// 	}
// 	pParent->Add(pStyle);

// 	<VerticalLayout>
// 		<Container name ="content" bkcolor="#FFFFFFFF">
// 		<Control      name="image"  bkimage="Item/item_normal.png" />
// 		<ItemOption name="item"   pushedimage="file='Item/item_border_pushed.png'  corner='5,5,5,5'" normalimage="file='Item/item_border_normal.png'  corner='5,5,5,5'" selectedimage="file='Item/item_border_pushed.png'  corner='5,5,5,5'"/>
// 		</Container>
// 
// 		<Label name="name"  font="12" padding="0,4,0,0" textcolor="#FFFFFFFF" align="center" endellipsis="true"/>
//	</VerticalLayout>


	//<VerticalLayout>
	CVerticalLayoutUI* pCloudComponent = new CVerticalLayoutUI();
	
	//<Container>
	CContainerUI* pContent	= new CContainerUI();
	pContent->SetName(_T("content"));
	
	pCloudComponent->Add(pContent);
	//</Container>

	//<Label/>
	CLabelUI* pName = new CLabelUI();
	pName->SetName(_T("name"));
	//pName->SetFont(120000);//2016.02.01 cws 使用默认字体，不要再设置字体，避免B框项字体变大
	pName->SetAttribute(_T("padding"), _T("0,4,0,0"));
	pName->SetAttribute(_T("align"), _T("center"));
	pName->SetAttribute(_T("endellipsis"), _T("true"));
	pName->SetTextColor(0xFFFFFFFF);
	pCloudComponent->Add(pName);


	pParent->Add(pCloudComponent);
	//</VerticalLayout>
	
	m_pComponent->DoInit(pContent, pDelegate);
	
}

tstring CCloudComponent::GetDescription()
{
	return _T("Cloud") + m_pComponent->GetDescription();
}

int CCloudComponent::GetHeight()
{
	return m_pComponent->GetHeight() + 20;
}

IResourceStyleable* CCloudComponent::GetResourceStyleable()
{
	return m_pComponent->GetResourceStyleable();
}




