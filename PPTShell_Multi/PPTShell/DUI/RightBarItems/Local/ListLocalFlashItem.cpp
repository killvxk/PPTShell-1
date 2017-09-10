#include "stdafx.h"
#include "ListLocalFlashItem.h"

CListLocalFlashItem::CListLocalFlashItem()
{

}

CListLocalFlashItem::~CListLocalFlashItem()
{

}

void CListLocalFlashItem::DoInit()
{
	__super::DoInit();

	m_nType = LocalFileFlash;
}

bool CListLocalFlashItem::OnDownloadDecodeList( void* pObj )
{
	return true;
}
