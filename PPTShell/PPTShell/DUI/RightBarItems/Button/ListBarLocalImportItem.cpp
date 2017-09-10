#include "stdafx.h"
#include "ListBarLocalImportItem.h"

CListBarLocalImportItem::CListBarLocalImportItem()
{

}

CListBarLocalImportItem::~CListBarLocalImportItem()
{

}

void CListBarLocalImportItem::DoInit()
{
	__super::DoInit();
	m_nType = LocalFileImport;

	m_pImportBtn = dynamic_cast<CButtonUI *>(FindSubControl(_T("btn_local_import")));
	m_pImportBtn->OnNotify += MakeDelegate(this, &CListBarLocalImportItem::OnBtnImport);
}

bool CListBarLocalImportItem::OnChapterChanged( void* pObj )
{
	return true;
}

bool CListBarLocalImportItem::OnRefreshGroupExplorer( void* pObj )
{
	return true;
}

bool CListBarLocalImportItem::OnDownloadDecodeList( void* pObj )
{
	return true;
}

void CListBarLocalImportItem::DoClick( TNotifyUI* pNotify )
{
	
}

bool CListBarLocalImportItem::OnBtnImport( void* pObj )
{
	TNotifyUI* Notify = (TNotifyUI *)pObj;
	if(Notify->sType == DUI_MSGTYPE_CLICK)
	{
		TNotifyUI NotifyEx;
		NotifyEx.pSender = this;
		if(m_OnDoClickCallBack)
			m_OnDoClickCallBack(&NotifyEx);
	}

	return true;
}

