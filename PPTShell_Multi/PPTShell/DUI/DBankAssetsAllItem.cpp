#include "stdafx.h"
#include "DBankAssetsAllItem.h"
#include "NDCloud/NDCloudFile.h"
#include "NDCloud/NDCloudAPI.h"

CDBankAssetsAllItem::CDBankAssetsAllItem()
{
	
}

CDBankAssetsAllItem::~CDBankAssetsAllItem()
{

}

void CDBankAssetsAllItem::DoInit()
{
	m_strName= _T("所有类型");

	__super::DoInit();
}

bool CDBankAssetsAllItem::OnDownloadDecodeList( void* pObj )
{
	return true;
}

bool CDBankAssetsAllItem::OnChapterChanged( void* pObj )
{
	return true;
}

void CDBankAssetsAllItem::RefreshData()
{
	m_pCountLabel->SetVisible(false);
	m_pGif->PlayGif();
	m_pGif->SetVisible(true);

	GetLayout()->StartLoading();
}

void CDBankAssetsAllItem::StartLoadGif()
{
	m_pGif->PlayGif();
	m_pGif->SetVisible(true);
}

void CDBankAssetsAllItem::StopLoadGif()
{
	m_pGif->StopGif();
	m_pGif->SetVisible(false);
}

