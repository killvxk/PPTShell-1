#pragma once
#include "StdAfx.h"
#include "DUI/IComponent.h"
#include "DUI/ToolbarComponent.h"
#include "DUI/IButtonTag.h"

DuiLib::CDialogBuilder CToolbarComponent::m_ItemToolbarBuilder;
CToolbarComponent::CToolbarComponent( IResourceStyleable* pStyleable )
{
	m_pStyleable		= pStyleable;
	m_bNeedInterposeBtn	= false;
}

CToolbarComponent::~CToolbarComponent()
{
	m_pStyleable = NULL;
}

void CToolbarComponent::DoInit( CContainerUI* pParent, IControlDelegate* pDelegate )
{
	//toolbar
// 	CContainerUI* pStyle = NULL;
// 	if( !m_ItemToolbarBuilder.GetMarkup()->IsValid() ) {
// 		pStyle = static_cast<CContainerUI*>(m_ItemToolbarBuilder.Create(_T("Item\\ResourceToolbarSytle.xml")));
// 	}
// 	else {
// 		pStyle = static_cast<CContainerUI*>(m_ItemToolbarBuilder.Create());
// 	}
// 
// 	pParent->Add(pStyle);
// 
// 	CContainerUI* pToolbar	= static_cast<CContainerUI*>(pStyle->FindSubControl(_T("toolbar")));


// 	<VerticalLayout>
// 		<VerticalLayout name="empty" >
// 		</VerticalLayout>
// 		<Progress name="progress" height="6"	bkimage="file='Item/item_pro_bk.png' corner='5,2,5,2'" foreimage="file='Item/item_pro_fore.png' corner='5,2,5,2'" min="0" max="100" visible="false"/>
// 
// 		<HorizontalLayout name="toolbar" height="30" >
// 		<!--<Button name="btn1" text="1"  normalimage="file='Item/btn_bk_normal.png'  corner='1,1,1,1'"   textcolor="#FFFFFFFF"/>
// 		<Button name="btn2" text="2"  normalimage="file='Item/btn_bk_normal.png'  corner='1,1,1,1'"   textcolor="#FFFFFFFF"/>-->
// 		</HorizontalLayout>
// 	</VerticalLayout>


	//<VerticalLayout>
	CVerticalLayoutUI* pToolbarStyle = new CVerticalLayoutUI();
	
	//<VerticalLayout name="empty" />
	CVerticalLayoutUI* pEmpty = new CVerticalLayoutUI();
	pEmpty->SetName(_T("empty"));
	pToolbarStyle->Add(pEmpty);

	//<Progress name="progress"/>
	CProgressUI* pProgress = new CProgressUI();
	pProgress->SetName(_T("progress"));
	pProgress->SetFixedHeight(6);
	pProgress->SetBkImage(_T("file='Item/item_pro_bk.png' corner='5,2,5,2'"));
	pProgress->SetForeImage(_T("file='Item/item_pro_fore.png' corner='5,2,5,2"));
	pProgress->SetVisible(false);
	pToolbarStyle->Add(pProgress);


	//<<HorizontalLayout name="toolbar" height="30" >
	CHorizontalLayoutUI* pToolbar = new CHorizontalLayoutUI();
	pToolbar->SetFixedHeight(30);
	pToolbar->SetName(_T("toolbar"));

	if (m_pStyleable)
	{
		if (m_bNeedInterposeBtn)
		{
			CButtonUI* pBtn		= new CButtonUI;

			pBtn->SetVisible(false);
			pBtn->SetName(_T("btn_start"));
			pBtn->SetTag((UINT_PTR)eClickFor_Start);
			pBtn->SetHotBkColor(0xFF11B0B6);
			pBtn->SetText(_T("下载"));
			pBtn->SetNormalImage(_T("file='Item/btn_bk_normal.png'  corner='1,1,1,1'"));
			pBtn->SetTextColor(0xFFFFFFFF);
			pBtn->OnNotify	+= MakeDelegate(pDelegate, &IControlDelegate::OnControlNotify);
			pBtn->OnEvent	+= MakeDelegate(pDelegate, &IControlDelegate::OnControlEvent);

			pToolbar->Add(pBtn);

			pBtn		= new CButtonUI;
			pBtn->SetVisible(false);
			pBtn->SetName(_T("btn_pause"));
			pBtn->SetTag((UINT_PTR)eClickFor_Pause);
			pBtn->SetText(_T("暂停"));
			pBtn->SetHotBkColor(0xFF11B0B6);
			pBtn->SetNormalImage(_T("file='Item/btn_bk_normal.png'  corner='1,1,1,1'"));
			pBtn->SetTextColor(0xFFFFFFFF);
			pBtn->OnNotify	+= MakeDelegate(pDelegate, &IControlDelegate::OnControlNotify);
			pBtn->OnEvent	+= MakeDelegate(pDelegate, &IControlDelegate::OnControlEvent);

			pToolbar->Add(pBtn);

		}
		for (int i = 0; i < m_pStyleable->GetButtonCount(); ++i)
		{
			TCHAR szBtnId[32]	= {0};
			_stprintf_s(szBtnId, _T("btn%d"), i + 1);
			CButtonUI* pBtn		= new CButtonUI;

			//<Button name="btn1"   normalimage="file='RightBar/item/btn_bk_normal.png'  corner='1,1,1,1'"   textcolor="#FFFFFFFF"/>
			pBtn->SetName(szBtnId);
			pBtn->SetHotBkColor(0xFF11B0B6);
			pBtn->SetText(m_pStyleable->GetButtonText(i));
			pBtn->SetTag((UINT_PTR)m_pStyleable->GetButtonTag(i));
			//pBtn->SetFont(12);//2016.02.01 cws 使用默认字体，不要再设置字体，避免B框按钮字体变大
			pBtn->SetNormalImage(_T("file='Item/btn_bk_normal.png'  corner='1,1,1,1'"));
			pBtn->SetTextColor(0xFFFFFFFF);
			pBtn->OnNotify	+= MakeDelegate(pDelegate, &IControlDelegate::OnControlNotify);
			pBtn->OnEvent	+= MakeDelegate(pDelegate, &IControlDelegate::OnControlEvent);

			pToolbar->Add(pBtn);
		}
	}
	pToolbarStyle->Add(pToolbar);
	//</HorizontalLayout>


	pParent->Add(pToolbarStyle);
	//</VerticalLayout>

	//CControlUI*	pEmptyCtrl	= static_cast<CContainerUI*>(pParent->FindSubControl(_T("empty")));
	pEmpty->OnEvent		+= MakeDelegate(pDelegate, &IControlDelegate::OnControlEvent);

}

tstring CToolbarComponent::GetDescription()
{
	return tstring(_T("Toolbar")) + m_pStyleable->GetDescription();
}

int CToolbarComponent::GetHeight()
{
	return m_pStyleable->GetHeight();
}

IResourceStyleable* CToolbarComponent::GetResourceStyleable()
{
	return m_pStyleable;
}

void CToolbarComponent::SetNeedInterposeBtn( bool bNeed )
{
	m_bNeedInterposeBtn = bNeed;
}


