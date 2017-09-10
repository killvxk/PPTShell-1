#include "stdafx.h"
#include "ListBarLocalSearchItem.h"

CListBarLocalSearchItem::CListBarLocalSearchItem()
{

}

CListBarLocalSearchItem::~CListBarLocalSearchItem()
{

}

void CListBarLocalSearchItem::DoInit()
{
	__super::DoInit();
	m_nType = LocalFileSearch;
}

bool CListBarLocalSearchItem::OnChapterChanged( void* pObj )
{
	return true;
}

bool CListBarLocalSearchItem::OnRefreshGroupExplorer( void* pObj )
{
	return true;
}

bool CListBarLocalSearchItem::OnDownloadDecodeList( void* pObj )
{
	return true;
}

void CListBarLocalSearchItem::DoClick( TNotifyUI* pNotify )
{
	if(m_OnDoClickCallBack)
		m_OnDoClickCallBack(pNotify);
}

void CListBarLocalSearchItem::DoRClick( TNotifyUI* pNotify )
{
	if(m_OnDoRClickCallBack)
		m_OnDoRClickCallBack(pNotify);
}
