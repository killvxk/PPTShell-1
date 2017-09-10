#include "stdafx.h"
#include "ListBarCloudSearchItem.h"

CListBarCloudSearchItem::CListBarCloudSearchItem()
{

}

CListBarCloudSearchItem::~CListBarCloudSearchItem()
{

}

void CListBarCloudSearchItem::DoInit()
{
	__super::DoInit();
	m_nType = CloudFileSearch;

	m_pContainerIcon	= dynamic_cast<COptionUI*>(FindSubControl(_T("icon")));
	m_pContentBtn		= dynamic_cast<CButtonUI *>(FindSubControl(_T("ContentBtn")));
}

bool CListBarCloudSearchItem::OnChapterChanged( void* pObj )
{
	return true;
}

bool CListBarCloudSearchItem::OnRefreshGroupExplorer( void* pObj )
{
	return true;
}

bool CListBarCloudSearchItem::OnDownloadDecodeList( void* pObj )
{
	return true;
}

void CListBarCloudSearchItem::DoClick( TNotifyUI* pNotify )
{
	if(m_OnDoClickCallBack)
		m_OnDoClickCallBack(pNotify);
}

void CListBarCloudSearchItem::DoRClick( TNotifyUI* pNotify )
{
	if(m_OnDoRClickCallBack)
		m_OnDoRClickCallBack(pNotify);
}

bool CListBarCloudSearchItem::Select( bool bSelect /*= true*/ )
{
	__super::Select(bSelect);

	m_pContainerIcon->Selected(bSelect);
	return true;
}

