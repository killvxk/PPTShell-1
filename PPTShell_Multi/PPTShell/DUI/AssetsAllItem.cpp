#include "stdafx.h"
#include "AssetsAllItem.h"
#include "NDCloud/NDCloudFile.h"
#include "NDCloud/NDCloudAPI.h"

CAssetsAllItem::CAssetsAllItem()
{
	m_bData = false;
}

CAssetsAllItem::~CAssetsAllItem()
{

}

void CAssetsAllItem::DoInit()
{
	m_strName= _T("所有类型");

	__super::DoInit();
}

bool CAssetsAllItem::OnDownloadDecodeList( void* pObj )
{
	return true;
}

bool CAssetsAllItem::OnChapterChanged( void* pObj )
{
	m_bData = false;
	return true;
}

void CAssetsAllItem::RefreshData()
{
	if(!m_bData)
	{
		m_pCountLabel->SetVisible(false);
		m_pGif->PlayGif();
		m_pGif->SetVisible(true);
		m_bData = true;

		GetLayout()->StartLoading();
	}
	
}

void CAssetsAllItem::StartLoadGif()
{
	m_pGif->PlayGif();
	m_pGif->SetVisible(true);
}

void CAssetsAllItem::StopLoadGif()
{
	m_pGif->StopGif();
	m_pGif->SetVisible(false);
}

