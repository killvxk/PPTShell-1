#include "stdafx.h"
#include "ListBarDBankUploadItem.h"

CListBarDBankUploadItem::CListBarDBankUploadItem()
{

}

CListBarDBankUploadItem::~CListBarDBankUploadItem()
{

}

void CListBarDBankUploadItem::DoInit()
{
	__super::DoInit();
	m_nType = DBankUpload;

	m_pUploadBtn = dynamic_cast<CButtonUI *>(FindSubControl(_T("btn_dbank_upload")));
	m_pUploadBtn->OnNotify += MakeDelegate(this, &CListBarDBankUploadItem::OnBtnUpload);
}

bool CListBarDBankUploadItem::OnChapterChanged( void* pObj )
{
	return true;
}

bool CListBarDBankUploadItem::OnRefreshGroupExplorer( void* pObj )
{
	return true;
}

bool CListBarDBankUploadItem::OnDownloadDecodeList( void* pObj )
{
	return true;
}

void CListBarDBankUploadItem::DoClick( TNotifyUI* pNotify )
{
	
}

bool CListBarDBankUploadItem::OnBtnUpload( void * pObj )
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
