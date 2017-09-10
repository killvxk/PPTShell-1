#include "stdafx.h"
#include "DBankAssetsWindow.h"

#include "AssetsVideoLayout.h"
#include "AssetsImageLayout.h"
#include "AssetsFlashLayout.h"
#include "AssetsVolumeLayout.h"

CDBankAssetsWindowUI::CDBankAssetsWindowUI()
{
	m_pAssetsVideoItem		= new CDBankAssetsVideoItem;
	m_pAssetsImageItem		= new CDBankAssetsImageItem;
	m_pAssetsFlashItem		= new CDBankAssetsFlashItem;
	m_pAssetsVolumeItem		= new CDBankAssetsVolumeItem;
	m_pAssetsAllItem		= new CDBankAssetsAllItem;
	m_pCurSelected			= NULL;
	m_nDoFinishCount		= 0;
}

CDBankAssetsWindowUI::~CDBankAssetsWindowUI()
{

}

void CDBankAssetsWindowUI::Init()
{
	CHorizontalLayoutUI* pLayout = dynamic_cast<CHorizontalLayoutUI*>(FindSubControl(_T("guide")));
	CControlUI * pControl = new CControlUI;
	pLayout->Add(pControl);
	pControl->SetFixedWidth(20);
	m_pAssetsAllItem->SetFixedWidth(80);
	m_pAssetsAllItem->SetFixedHeight(40);
	m_pAssetsAllItem->SetBkColor(0xFF515151);
	pLayout->Add(m_pAssetsAllItem);
	pControl = new CControlUI;
	pLayout->Add(pControl);
	pControl->SetFixedWidth(3);
	m_pAssetsImageItem->SetFixedWidth(68);
	m_pAssetsImageItem->SetFixedHeight(40);
	m_pAssetsImageItem->SetBkColor(0xFF515151);
	pLayout->Add(m_pAssetsImageItem);
	pControl = new CControlUI;
	pLayout->Add(pControl);
	pControl->SetFixedWidth(3);
	m_pAssetsVideoItem->SetFixedWidth(68);
	m_pAssetsVideoItem->SetFixedHeight(40);
	m_pAssetsVideoItem->SetBkColor(0xFF515151);
	pLayout->Add(m_pAssetsVideoItem);
	pControl = new CControlUI;
	pLayout->Add(pControl);
	pControl->SetFixedWidth(3);
	m_pAssetsFlashItem->SetFixedWidth(68);
	m_pAssetsFlashItem->SetFixedHeight(40);
	m_pAssetsFlashItem->SetBkColor(0xFF515151);
	pLayout->Add(m_pAssetsFlashItem);
	pControl = new CControlUI;
	pLayout->Add(pControl);
	pControl->SetFixedWidth(3);
	m_pAssetsVolumeItem->SetFixedWidth(68);
	m_pAssetsVolumeItem->SetFixedHeight(40);
	m_pAssetsVolumeItem->SetBkColor(0xFF515151);
	pLayout->Add(m_pAssetsVolumeItem);

	CContainerUI* pContainer = dynamic_cast<CContainerUI*>(FindSubControl(_T("content")));

	CAssetsAllLayout* pAssetsAllLayout = new CAssetsAllLayout;
	m_pAssetsAllItem->SetLayout(pAssetsAllLayout);
	pContainer->Add(pAssetsAllLayout);
	m_pAssetsAllLayout = pAssetsAllLayout;

	CAssetsImageLayout* pAssetsImageLayout = new CAssetsImageLayout;
	m_pAssetsImageItem->SetLayout(pAssetsImageLayout);
	pContainer->Add(pAssetsImageLayout);
	pAssetsImageLayout->SetVisible(false);

	CAssetsVideoLayout* pAssetsVideoLayout = new CAssetsVideoLayout;
	m_pAssetsVideoItem->SetLayout(pAssetsVideoLayout);
	pContainer->Add(pAssetsVideoLayout);
	pAssetsVideoLayout->SetVisible(false);

	CAssetsFlashLayout* pAssetsFlashLayout = new CAssetsFlashLayout;
	m_pAssetsFlashItem->SetLayout(pAssetsFlashLayout);
	pContainer->Add(pAssetsFlashLayout);
	pAssetsFlashLayout->SetVisible(false);

	CAssetsVolumeLayout* pAssetsVolumeLayout = new CAssetsVolumeLayout;
	m_pAssetsVolumeItem->SetLayout(pAssetsVolumeLayout);
	pContainer->Add(pAssetsVolumeLayout);
	pAssetsVolumeLayout->SetVisible(false);

	m_pAssetsAllItem->SetDoClickCallBack(MakeDelegate(this, &CDBankAssetsWindowUI::OnItemClick));
	m_pAssetsVideoItem->SetDoClickCallBack(MakeDelegate(this, &CDBankAssetsWindowUI::OnItemClick));
	m_pAssetsImageItem->SetDoClickCallBack(MakeDelegate(this, &CDBankAssetsWindowUI::OnItemClick));
	m_pAssetsFlashItem->SetDoClickCallBack(MakeDelegate(this, &CDBankAssetsWindowUI::OnItemClick));
	m_pAssetsVolumeItem->SetDoClickCallBack(MakeDelegate(this, &CDBankAssetsWindowUI::OnItemClick));

	m_nDoFinishCount = 0;
	m_pAssetsVideoItem->SetDoFinishCallBack(MakeDelegate(this, &CDBankAssetsWindowUI::OnDoFinish));
	m_pAssetsImageItem->SetDoFinishCallBack(MakeDelegate(this, &CDBankAssetsWindowUI::OnDoFinish));
	m_pAssetsFlashItem->SetDoFinishCallBack(MakeDelegate(this, &CDBankAssetsWindowUI::OnDoFinish));
	m_pAssetsVolumeItem->SetDoFinishCallBack(MakeDelegate(this, &CDBankAssetsWindowUI::OnDoFinish));

	RefreshData();

	m_pAssetsAllItem->Select(true);
	m_pCurSelected = m_pAssetsAllItem;
}

bool CDBankAssetsWindowUI::OnItemClick( void* pObj )
{
	if(m_pCurSelected != NULL)
	{
		if(m_pCurSelected == pObj)
			return false;
		else
		{
			((CAssetsItem *)m_pCurSelected)->Select(false);
			((CAssetsItem *)m_pCurSelected)->GetLayout()->SetVisible(false);
			((CAssetsItem *)pObj)->GetLayout()->SetVisible(true);
			m_pCurSelected = pObj;
		}
	}
	else
		m_pCurSelected = pObj;
	return true;
}

bool CDBankAssetsWindowUI::OnDoFinish( void* pObj )
{
	m_nDoFinishCount++;
	if(m_nDoFinishCount == 4)
	{
		m_pAssetsAllItem->GetLayout()->GetContentLayout()->RemoveAll();

		int nCount = 0;
		m_pAssetsAllItem->StopLoadGif();
		m_pAssetsAllItem->GetLayout()->StopLoading();

		if(m_pAssetsImageItem->GetNetStatus() ||
			m_pAssetsVideoItem->GetNetStatus() ||
			m_pAssetsFlashItem->GetNetStatus() ||
			m_pAssetsVolumeItem->GetNetStatus()
			)
		{
			m_pAssetsAllItem->SetCountText(nCount);
			m_pAssetsAllItem->GetLayout()->ShowNetLess();
			m_nDoFinishCount = 0;
			return true;
		}

		CStream* pStream = m_pAssetsImageItem->GetStream();
		pStream->ResetCursor();
		nCount += pStream->ReadInt();
		m_pAssetsAllLayout->SetStream(pStream, DBankImage);

		pStream = m_pAssetsVideoItem->GetStream();
		pStream->ResetCursor();
		nCount += pStream->ReadInt();
		m_pAssetsAllLayout->SetStream(pStream, DBankVideo);

		pStream = m_pAssetsFlashItem->GetStream();
		pStream->ResetCursor();
		nCount += pStream->ReadInt();
		m_pAssetsAllLayout->SetStream(pStream, DBankFlash);

		pStream = m_pAssetsVolumeItem->GetStream();
		pStream->ResetCursor();
		nCount += pStream->ReadInt();
		m_pAssetsAllLayout->SetStream(pStream, DBankVolume);

		m_pAssetsAllItem->SetCountText(nCount);

		m_nDoFinishCount = 0;
		m_pAssetsAllItem->GetLayout()->ShowNetLess(false);
	}

	return true;
}

void CDBankAssetsWindowUI::RefreshData()
{
	m_nDoFinishCount = 0;
	m_pAssetsAllItem->RefreshData();
	m_pAssetsVideoItem->RefreshData(true);
	m_pAssetsImageItem->RefreshData(true);
	m_pAssetsFlashItem->RefreshData(true);
	m_pAssetsVolumeItem->RefreshData(true);
}