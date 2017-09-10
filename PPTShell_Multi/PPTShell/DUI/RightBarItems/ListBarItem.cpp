#include "stdafx.h"
#include "ListBarItem.h"
#include "NDCloud/NDCloudFile.h"
#include "NDCloud/NDCloudAPI.h"

CListBarItem::CListBarItem()
{
	m_pStream					= NULL;
	m_dwDownloadId				= -1;
}

CListBarItem::~CListBarItem()
{
	if(m_dwDownloadId != -1)
	{
		NDCloudDownloadCancel(m_dwDownloadId);
	}

	if(m_pStream)
		delete m_pStream;
}

void CListBarItem::DoInit()
{
	this->OnNotify			+= MakeDelegate(this, &CListBarItem::OnControlNotify);
	m_strName				= GetUserData();
}

bool CListBarItem::OnControlNotify( void* pNotify )
{
	TNotifyUI* Notify = (TNotifyUI *)pNotify;
	if(Notify->sType == DUI_MSGTYPE_ITEMCLICK)
	{
		DoClick(Notify);
	}
	else if(Notify->sType == DUI_MSGTYPE_ITEMRCLICK)
	{
		DoRClick(Notify);
	}

	return true;
}

void CListBarItem::DoClick( TNotifyUI* pNotify )
{
	
}

void CListBarItem::DoRClick( TNotifyUI* pNotify )
{
	
}

void CListBarItem::SetJsonUrl( tstring strJsonUrl )
{
	m_strJsonUrl = strJsonUrl;
}

tstring CListBarItem::GetJsonUrl()
{
	return m_strJsonUrl;
}

int CListBarItem::GetItemType()
{
	return m_nType;
}

void CListBarItem::SetDoClickCallBack( CDelegateBase& delegate )
{
	m_OnDoClickCallBack.clear();
	m_OnDoClickCallBack += delegate;
}

void CListBarItem::SetParentListPtr( void * pControl )
{
	m_pParentListPtr = pControl;
}

void* CListBarItem::GetParentListPtr()
{
	return m_pParentListPtr;
}

void CListBarItem::SetDoRClickCallBack( CDelegateBase& delegate )
{
	m_OnDoRClickCallBack.clear();
	m_OnDoRClickCallBack += delegate;
}
