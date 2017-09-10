#include "stdafx.h"
#include "ListBarButton.h"

CListBarButton::CListBarButton()
{

}

CListBarButton::~CListBarButton()
{

}


void CListBarButton::DoInit()
{
	this->OnNotify			+= MakeDelegate(this, &CListBarButton::OnControlNotify);
	m_strName				=  GetUserData();
}

bool CListBarButton::OnControlNotify( void* pNotify )
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

void CListBarButton::DoClick( TNotifyUI* pNotify )
{
	if(m_OnDoClickCallBack)
		m_OnDoClickCallBack(pNotify);
}

void CListBarButton::DoRClick( TNotifyUI* pNotify )
{

}


void CListBarButton::SetDoClickCallBack( CDelegateBase& delegate )
{
	m_OnDoClickCallBack.clear();
	m_OnDoClickCallBack += delegate;
}

void CListBarButton::SetDoRClickCallBack( CDelegateBase& delegate )
{
	m_OnDoRClickCallBack.clear();
	m_OnDoRClickCallBack += delegate;
}

int CListBarButton::GetItemType()
{
	return m_nType;
}