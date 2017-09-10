//===========================================================================
// FileName:				UserManager.cpp
//	
// Desc:				 
//============================================================================
#include "stdafx.h"
#include "User.h"
#include "UserManager.h"


CUserManager::CUserManager(void)
{
	InitializeCriticalSection(&m_csUserMgr);
	m_bMajorUser	= FALSE;
	m_mapUser.clear();
}

CUserManager::~CUserManager(void)
{
	DeleteCriticalSection(&m_csUserMgr);
}

CUser* CUserManager::UserLogin(SOCKET sock, DWORD dwUid, int nConnectType, TCHAR* szName)
{
	CUser* pUser		= FindUser(dwUid);

	bool bLeader		= false;
	DWORD dwPermission	= PER_DEAFULT;

	if( !m_bMajorUser || (m_bMajorUser && pUser && pUser->IsLeader() && pUser->IsOnline()) )
	{
		m_bMajorUser	= true;
		bLeader			= true;
		dwPermission	= PER_ALL;
	}

	if ( pUser == NULL )
	{
		pUser = new CUser;

		if( pUser == NULL )
			return NULL;

		pUser->Login(sock, dwUid, bLeader, dwPermission, nConnectType, szName);

		m_mapUser[dwUid] = pUser;
		m_nOnlineUserCount++;
	}
	else
	{
		pUser->Login(sock, dwUid, bLeader, dwPermission, nConnectType, szName);
		m_nOnlineUserCount++;
	}

	return pUser;
}


BOOL CUserManager::UserLogoff( DWORD dwUid )
{
	CUser* pUser		= FindUser(dwUid);

	if ( pUser )
	{
		if (pUser->IsLeader())
			m_bMajorUser = false;

		pUser->Logoff();

		m_nOnlineUserCount--;

		if ( m_nOnlineUserCount < 0 )
			m_nOnlineUserCount = 0;

		return true;
	}

	return false;
}

//
// find player
//
CUser* CUserManager::FindUser(DWORD dwUid)
{
	CUser* pUser = NULL;

	EnterCriticalSection(&m_csUserMgr);

	std::map<DWORD, CUser*>::iterator iter = m_mapUser.find(dwUid);
	if( iter != m_mapUser.end() )
		pUser = iter->second;

	LeaveCriticalSection(&m_csUserMgr);

	return pUser;
}

//  Do not use for WAN connection !!!
CUser* CUserManager::FindUser(SOCKET sock)
{
	CUser* pFoundUser = NULL;

	EnterCriticalSection(&m_csUserMgr);

	std::map<DWORD, CUser*>::iterator iter;
	for(iter = m_mapUser.begin(); iter != m_mapUser.end(); iter++)
	{
		CUser* pUser = iter->second;
		if( pUser->GetSocket() == sock )
		{
			pFoundUser = pUser;
			break;
		}
	}

	LeaveCriticalSection(&m_csUserMgr);

	return pFoundUser;
}

CUser* CUserManager::GetLeader()
{
	CUser* pUser = NULL;

	EnterCriticalSection(&m_csUserMgr);

	std::map<DWORD, CUser*>::iterator iter = m_mapUser.begin();

	for (iter; iter != m_mapUser.end(); iter++ )
	{
		CUser* pTempUser = iter->second;

		if ( pTempUser->IsLeader() )
		{
			pUser = pTempUser;
			break;
		}
	}

	LeaveCriticalSection(&m_csUserMgr);

	return pUser;
}
