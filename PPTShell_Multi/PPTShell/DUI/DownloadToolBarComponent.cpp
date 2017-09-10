#pragma once
#include "StdAfx.h"
#include "DUI/IComponent.h"
#include "DUI/DownloadToolBarComponent.h"
#include "DUI/IButtonTag.h"



CDownloadToolBarComponent::CDownloadToolBarComponent()
{
	m_bNeedPauseBtn = true;
}

CDownloadToolBarComponent::~CDownloadToolBarComponent()
{

}

void CDownloadToolBarComponent::DoInit( CContainerUI* pParent, IControlDelegate* pDelegate )
{	

	//<VerticalLayout>
	CVerticalLayoutUI*	pToolBarLayout	= new CVerticalLayoutUI;

	//<Control />
	CControlUI*			pPadding		= new CControlUI;
	pToolBarLayout->Add(pPadding);

	CHorizontalLayoutUI* pToolBar	= new CHorizontalLayoutUI;
	pToolBar->SetFixedHeight(26);
	pToolBar->SetChildPadding(15);
	pToolBar->SetAttribute(_T("inset"), _T("20,0,0,0"));

	if (m_bNeedPauseBtn)
	{
		CButtonUI*		pStartBtn	= new CButtonUI;
		pStartBtn->SetVisible(false);
		pStartBtn->SetFixedWidth(26);
		pStartBtn->SetToolTip(_T("¿ªÊ¼"));
		pStartBtn->SetName(_T("dm_item_download_start"));
		pStartBtn->SetTag((UINT_PTR)eClickFor_Start);
		pStartBtn->SetNormalImage(_T("Item\\Download\\btn_start_normal.png"));
		pStartBtn->SetHotImage(_T("Item\\Download\\btn_start_hot.png"));
		pStartBtn->SetPushedImage(_T("Item\\Download\\btn_start_pushed.png"));
		pStartBtn->OnNotify += MakeDelegate(pDelegate, &IControlDelegate::OnControlNotify);
		pStartBtn->OnEvent	+= MakeDelegate(pDelegate, &IControlDelegate::OnControlEvent);
		pToolBar->Add(pStartBtn);
		

		CButtonUI*		pPauseBtn	= new CButtonUI;
		pPauseBtn->SetName(_T("dm_item_download_pause"));
		pPauseBtn->SetFixedWidth(26);
		pPauseBtn->SetToolTip(_T("ÔÝÍ£"));
		pPauseBtn->SetTag((UINT_PTR)eClickFor_Pause);
		pPauseBtn->SetNormalImage(_T("Item\\Download\\btn_pause_normal.png"));
		pPauseBtn->SetHotImage(_T("Item\\Download\\btn_pause_hot.png"));
		pPauseBtn->SetPushedImage(_T("Item\\Download\\btn_pause_pushed.png"));
		pPauseBtn->OnNotify += MakeDelegate(pDelegate, &IControlDelegate::OnControlNotify);
		pPauseBtn->OnEvent	+= MakeDelegate(pDelegate, &IControlDelegate::OnControlEvent);
		pToolBar->Add(pPauseBtn);

		CButtonUI*		pRetryBtn	= new CButtonUI;
		pRetryBtn->SetVisible(false);
		pRetryBtn->SetFixedWidth(26);
		pRetryBtn->SetToolTip(_T("ÖØÊÔ"));
		pRetryBtn->SetName(_T("dm_item_download_retry"));
		pRetryBtn->SetTag((UINT_PTR)eClickFor_Retry);
		pRetryBtn->SetNormalImage(_T("Item\\Download\\btn_retry_normal.png"));
		pRetryBtn->SetHotImage(_T("Item\\Download\\btn_retry_hot.png"));
		pRetryBtn->SetPushedImage(_T("Item\\Download\\btn_retry_pushed.png"));
		pRetryBtn->OnNotify += MakeDelegate(pDelegate, &IControlDelegate::OnControlNotify);
		pRetryBtn->OnEvent	+= MakeDelegate(pDelegate, &IControlDelegate::OnControlEvent);
		pToolBar->Add(pRetryBtn);
		
	}
	else
	{
		pPadding	= new CControlUI;
		pPadding->SetFixedWidth(26);

		pToolBar->Add(pPadding);
	}

	CButtonUI*		pCloseBtn	= new CButtonUI;

	pCloseBtn->SetName(_T("dm_item_download_close"));
	pCloseBtn->SetFixedWidth(26);
	pCloseBtn->SetToolTip(_T("É¾³ý"));
	pCloseBtn->SetTag((UINT_PTR)eClickFor_Close);
	pCloseBtn->SetNormalImage(_T("Item\\Download\\btn_close_normal.png"));
	pCloseBtn->SetHotImage(_T("Item\\Download\\btn_close_hot.png"));
	pCloseBtn->SetPushedImage(_T("Item\\Download\\btn_close_pushed.png"));
	pCloseBtn->OnNotify += MakeDelegate(pDelegate, &IControlDelegate::OnControlNotify);
	pCloseBtn->OnEvent	+= MakeDelegate(pDelegate, &IControlDelegate::OnControlEvent);
	pToolBar->Add(pCloseBtn);

	pToolBarLayout->Add(pToolBar);
	//<Control />
	pPadding = new CControlUI;
	pToolBarLayout->Add(pPadding);

	//</VerticalLayout>
	pParent->Add(pToolBarLayout);
	//<VerticalLayout>
	//	<Control />
	//	<HorizontalLayout childpadding="20" height="26">

	//	</HorizontalLayout>
	//	<Control />
	//</VerticalLayout>
}

tstring CDownloadToolBarComponent::GetDescription()
{
	return _T("Toolbar");
}

int CDownloadToolBarComponent::GetHeight()
{
	return 0;
}

IResourceStyleable* CDownloadToolBarComponent::GetResourceStyleable()
{
	return NULL;
}

void CDownloadToolBarComponent::SetNeedPauseBtn( bool bNeed )
{
	m_bNeedPauseBtn = bNeed;
}




