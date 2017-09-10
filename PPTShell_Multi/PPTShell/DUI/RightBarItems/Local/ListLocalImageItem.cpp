#include "stdafx.h"
#include "ListLocalImageItem.h"

CListLocalImageItem::CListLocalImageItem()
{

}

CListLocalImageItem::~CListLocalImageItem()
{

}

void CListLocalImageItem::DoInit()
{
	__super::DoInit();

	m_nType = LocalFileImage;
}

bool CListLocalImageItem::OnDownloadDecodeList( void* pObj )
{
	return true;
}
