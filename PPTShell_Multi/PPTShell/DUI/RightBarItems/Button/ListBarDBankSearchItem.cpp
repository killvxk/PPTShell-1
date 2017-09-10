#include "stdafx.h"
#include "ListBarDBankSearchItem.h"

CListBarDBankSearchItem::CListBarDBankSearchItem()
{

}

CListBarDBankSearchItem::~CListBarDBankSearchItem()
{

}

void CListBarDBankSearchItem::DoInit()
{
	__super::DoInit();
	m_nType = DBankSearch;
}

bool CListBarDBankSearchItem::OnChapterChanged( void* pObj )
{
	return true;
}

bool CListBarDBankSearchItem::OnRefreshGroupExplorer( void* pObj )
{
	return true;
}

bool CListBarDBankSearchItem::OnDownloadDecodeList( void* pObj )
{
	return true;
}

void CListBarDBankSearchItem::DoClick( TNotifyUI* pNotify )
{
	if(m_OnDoClickCallBack)
		m_OnDoClickCallBack(pNotify);
}

void CListBarDBankSearchItem::DoRClick( TNotifyUI* pNotify )
{
	if(m_OnDoRClickCallBack)
		m_OnDoRClickCallBack(pNotify);
}
