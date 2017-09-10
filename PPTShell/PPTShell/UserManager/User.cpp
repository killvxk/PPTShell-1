//===========================================================================
// FileName:				User.cpp
//	
// Desc:				 
//============================================================================
#include "stdafx.h"
#include "User.h"
#include "Network/LANServer.h"

CUser::CUser()
{

}

CUser::~CUser()
{

}

void CUser::Login(SOCKET sock, DWORD dwUid, bool bLeader, DWORD dwPermission, int nConnectType, TCHAR *szName)
{
	m_hSocket				= sock;
	m_dwUID					= dwUid;
	m_dwPermission			= dwPermission;
	m_bLeader				= bLeader;
	m_nConnectType			= nConnectType;
	m_bOnline				= TRUE;

	_tcsncpy(m_szName, szName, MAX_PATH);
	m_setSlidePages.clear();
}

void CUser::Logoff()
{
	m_bOnline			= FALSE;
	m_bLeader			= FALSE;
	m_dwPermission		= PER_NONE;
	m_hSocket			= INVALID_SOCKET;
	m_nConnectType		= 0;
	m_setSlidePages.clear();
}

BOOL CUser::IsOnline()	
{
	if (!m_bOnline)
	{
		return FALSE;
	}

	if (m_nConnectType==CONNECT_TYPE_LAN && !LANServer::GetInstance()->IsSocketOnLine(m_hSocket))
	{
		Logoff();
		return FALSE;
	}
	else
	{
		return	 TRUE;
	}
}