#include "StdAfx.h"
#include "DUI/ITransfer.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IVisitor.h"
#include "DUI/BaseParamer.h"


CInvokeParamer::CInvokeParamer()
{
	m_pHttpNotify	= NULL;
	m_pTrigger		= NULL;
}

CInvokeParamer::~CInvokeParamer()
{

}

void CInvokeParamer::SetCompletedDelegate( CDelegateBase& completedHandler )
{
	OnCompletedHandler += completedHandler;
}

void CInvokeParamer::InvokeHandler( THttpNotify* pNotify, ITransfer* pTransfer )
{
	if (OnCompletedHandler)
	{
		m_pHttpNotify	= pNotify;
		m_pTransfer		= pTransfer;
		OnCompletedHandler(this);
	}
}

THttpNotify* CInvokeParamer::GetHttpNotify()
{
	return m_pHttpNotify;
}

ITransfer* CInvokeParamer::GetTransfer()
{
	return m_pTransfer;
}

CControlUI* CInvokeParamer::GetTrigger()
{
	return m_pTrigger;
}

void CInvokeParamer::SetTrigger( CControlUI* pTrigger )
{
	m_pTrigger = pTrigger;
}

IBaseParamer* CInvokeParamer::Copy()
{
	CInvokeParamer* pParamer = new CInvokeParamer;
	pParamer->m_pTrigger		= this->m_pTrigger;
	pParamer->OnCompletedHandler= this->OnCompletedHandler;

	return pParamer;
}
