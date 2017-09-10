#include "stdafx.h"
#include "ListLocalVolumeItem.h"

CListLocalVolumeItem::CListLocalVolumeItem()
{

}

CListLocalVolumeItem::~CListLocalVolumeItem()
{

}

void CListLocalVolumeItem::DoInit()
{
	__super::DoInit();

	m_nType = LocalFileVolume;
}

bool CListLocalVolumeItem::OnDownloadDecodeList( void* pObj )
{
	return true;
}
