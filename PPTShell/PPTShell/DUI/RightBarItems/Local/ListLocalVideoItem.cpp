#include "stdafx.h"
#include "ListLocalVideoItem.h"

CListLocalVideoItem::CListLocalVideoItem()
{

}

CListLocalVideoItem::~CListLocalVideoItem()
{

}

void CListLocalVideoItem::DoInit()
{
	__super::DoInit();

	m_nType = LocalFileVideo;
}

bool CListLocalVideoItem::OnDownloadDecodeList( void* pObj )
{
	return true;
}
