#include "StdAfx.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IVisitor.h"
#include "DUI/BaseParamer.h"
#include "DUI/InsertParamer.h"

CInsertParamer::CInsertParamer()
{
	m_dwPlaceHolderId		= 0;
	m_dwOperationerId		= 0;
	m_dwSlideId				= 0;
	m_ptInsertPos.x			= -1;
	m_ptInsertPos.y			= -1;
	m_pUserData = NULL;
}

CInsertParamer::~CInsertParamer()
{

}

DWORD CInsertParamer::GetPlaceHolderId()
{
	return m_dwPlaceHolderId;
}

void CInsertParamer::SetPlaceHolderId( DWORD dwId )
{
	m_dwPlaceHolderId = dwId;

}

DWORD CInsertParamer::GetSlideId()
{	
	return m_dwSlideId;
}

void CInsertParamer::SetSlideId( DWORD dwId )
{
	m_dwSlideId		= dwId;
}

DWORD CInsertParamer::GetOperationerId()
{
	return m_dwOperationerId;
}

void CInsertParamer::SetOperationerId( DWORD dwId )
{
	m_dwOperationerId = dwId;

}

void CInsertParamer::SetInsertPos( POINT pt )
{
	m_ptInsertPos = pt;
}

void CInsertParamer::SetInsertPos( int x, int y )
{
	m_ptInsertPos.x = x;
	m_ptInsertPos.y	= y;
}

POINT CInsertParamer::GetInsertPos()
{
	return m_ptInsertPos;
}

void CInsertParamer::SetUserData( void* pUserData )
{
	m_pUserData = pUserData;
}

void* CInsertParamer::GetUserData()
{
	return m_pUserData;
}

IBaseParamer* CInsertParamer::Copy()
{
	CInsertParamer* pParamer = new CInsertParamer;
	//super
	pParamer->m_pTrigger		= __super::m_pTrigger;
	pParamer->OnCompletedHandler= __super::OnCompletedHandler;
	//self
	pParamer->SetSlideId(this->GetSlideId());
	pParamer->SetPlaceHolderId(this->GetPlaceHolderId());
	pParamer->SetOperationerId(this->GetOperationerId());
	pParamer->SetInsertPos(this->GetInsertPos());

	return pParamer;
}
