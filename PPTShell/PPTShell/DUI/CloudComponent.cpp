#pragma once
#include "StdAfx.h"
#include "DUI/IComponent.h"
#include "DUI/CloudComponent.h"
#include "DUI/ItemOption.h"


CCloudComponent::CCloudComponent( IComponent* pComponent, LPCTSTR lptcsType /*= NULL*/ )
{
	m_pComponent	= pComponent;
	if (lptcsType)
	{
		m_strTagType= lptcsType;
	}
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

	CPaintManagerUI* pPaintManagerUI = pParent->GetManager();

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


	if (!m_strTagType.empty())
	{
		//create label tag
		CHorizontalLayoutUI* pLabelLay = new CHorizontalLayoutUI;
		pLabelLay->SetAttribute(_T("padding"), _T("0,4,0,0"));
		pLabelLay->SetFixedHeight(20);

		//left auto padding
		CControlUI* pPadding = new CControlUI;
		pLabelLay->Add(pPadding);

		//tag
		CLabelUI* pTag = new CLabelUI();
		pTag->SetName(_T("tag"));
		pTag->SetAttribute(_T("padding"), _T("0,2,0,2"));
		pTag->SetAttribute(_T("borderround"), _T("2,2"));
		pTag->SetAttribute(_T("align"), _T("center"));
		pTag->SetAttribute(_T("endellipsis"), _T("true"));
		pTag->SetTextColor(0xFFFFFFFF);
		if (GetResourceStyleable())
		{
			pTag->SetBkColor(GetResourceStyleable()->GetTagColor());
			pTag->SetText(GetResourceStyleable()->GetDescription());
			SIZE size = CRenderEngine::GetTextSize(pPaintManagerUI->GetPaintDC(),
				pPaintManagerUI,
				pTag->GetText(),
				pTag->GetFont(),
				DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT| DT_NOPREFIX);
			pTag->SetFixedWidth(size.cx);
		}
	
		pLabelLay->Add(pTag);

		//tag name
		CLabelUI* pName = new CLabelUI();
		pName->SetName(_T("tag_name"));
		pName->SetAttribute(_T("padding"), _T("6,0,0,0"));
		pName->SetAttribute(_T("align"), _T("center"));
		pName->SetAttribute(_T("endellipsis"), _T("true"));
		pName->SetTextColor(0xFF898989);
		pName->SetText(m_strTagType.c_str());
		SIZE size = CRenderEngine::GetTextSize(pPaintManagerUI->GetPaintDC(),
			pPaintManagerUI,
			pName->GetText(),
			pName->GetFont(),
			DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT| DT_NOPREFIX);
		pName->SetFixedWidth(size.cx);

		pLabelLay->Add(pName);

		
		//right auto padding
		pPadding = new CControlUI;
		pLabelLay->Add(pPadding);

		pCloudComponent->Add(pLabelLay);
	}

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
	if (m_strTagType.empty())
	{
		return m_pComponent->GetHeight() + 20;
	}

	return m_pComponent->GetHeight() + 20 + 20;
}

IResourceStyleable* CCloudComponent::GetResourceStyleable()
{
	return m_pComponent->GetResourceStyleable();
}



