#include "stdafx.h"
#include "ListBarLoginLayout.h"

CListBarLoginLayout::CListBarLoginLayout()
{

}

CListBarLoginLayout::~CListBarLoginLayout()
{

}

void CListBarLoginLayout::DoInit()
{
	__super::DoInit();

	m_pNdbankLoginBtn		= dynamic_cast<CButtonUI *>(FindSubControl(_T("btnLogin")));
	m_pNdbankRegisterBtn	= dynamic_cast<CButtonUI *>(FindSubControl(_T("btnRegister")));

	m_pNdbankLoginBtn->OnNotify			+= MakeDelegate(this, &CListBarLoginLayout::OnBtnLogin);
	m_pNdbankRegisterBtn->OnNotify		+= MakeDelegate(this, &CListBarLoginLayout::OnBtnRegister);
}

void CListBarLoginLayout::DoClick( TNotifyUI* pNotify )
{
	
}

void CListBarLoginLayout::DoRClick( TNotifyUI* pNotify )
{
	
}

bool CListBarLoginLayout::OnBtnLogin( void* pObj )
{
	TNotifyUI* Notify = (TNotifyUI *)pObj;
	if(Notify->sType == DUI_MSGTYPE_CLICK)
	{
		TNotifyUI NotifyEx;
		NotifyEx.pSender = this;
		m_nType = DBankLogin;
		if(m_OnDoClickCallBack)
			m_OnDoClickCallBack(&NotifyEx);
	}

	return true;
}

bool CListBarLoginLayout::OnBtnRegister( void* pObj )
{
	TNotifyUI* Notify = (TNotifyUI *)pObj;
	if(Notify->sType == DUI_MSGTYPE_CLICK)
	{
		TNotifyUI NotifyEx;
		NotifyEx.pSender = this;
		m_nType = DBankRegister;
		if(m_OnDoClickRegister)
			m_OnDoClickRegister(&NotifyEx);
	}

	return true;
}

void CListBarLoginLayout::OnSetRegisterCallBack( CDelegateBase& delegate )
{
	m_OnDoClickRegister.clear();
	m_OnDoClickRegister += delegate;
}
