#include "stdafx.h"
#include "ListBarLayout.h"

CListBarLayout::CListBarLayout()
{

}

CListBarLayout::~CListBarLayout()
{

}

void CListBarLayout::DoInit()
{
	this->OnNotify			+= MakeDelegate(this, &CListBarLayout::OnControlNotify);
	m_strName				=  GetUserData();
}

bool CListBarLayout::OnControlNotify( void* pNotify )
{
	TNotifyUI* Notify = (TNotifyUI *)pNotify;
	if(Notify->sType == DUI_MSGTYPE_CLICK)
	{
		DoClick(Notify);
	}
	else if(Notify->sType == DUI_MSGTYPE_RCLICK)
	{
		DoRClick(Notify);
	}
	return true;
}

void CListBarLayout::DoClick( TNotifyUI* pNotify )
{
	if(m_OnDoClickCallBack)
		m_OnDoClickCallBack(pNotify);
}

void CListBarLayout::DoRClick( TNotifyUI* pNotify )
{

}

void CListBarLayout::SetDoClickCallBack( CDelegateBase& delegate )
{
	m_OnDoClickCallBack.clear();
	m_OnDoClickCallBack += delegate;
}

void CListBarLayout::SetDoRClickCallBack( CDelegateBase& delegate )
{
	m_OnDoRClickCallBack.clear();
	m_OnDoRClickCallBack += delegate;
}

int CListBarLayout::GetItemType()
{
	return m_nType;
}
