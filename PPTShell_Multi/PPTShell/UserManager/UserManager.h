//===========================================================================
// FileName:				UserManager.h
//	
// Desc:				 
//============================================================================
#ifndef _USER_MANAGER_H_
#define _USER_MANAGER_H_

#include "Util/Singleton.h"

class CUserManager
{
private:
	CUserManager();
	~CUserManager();

public:
	CUser*		UserLogin(SOCKET sock, DWORD dwUid, int nConnectType, char* szName);
	BOOL		UserLogoff(DWORD dwUid);
	CUser*		FindUser(DWORD dwUId);
	CUser*		FindUser(SOCKET sock);
	CUser*		GetLeader();

	inline int					GetOnlineUserCount()			{	return m_nOnlineUserCount;	}	//这个结果不准确 需要实时去计算 如果有用请自行修改
	inline map<DWORD, CUser*>	GetUsers()						{	return m_mapUser;			}

	DECLARE_SINGLETON_CLASS(CUserManager);

protected:
	CRITICAL_SECTION			m_csUserMgr;
	BOOL						m_bMajorUser;
	int							m_nOnlineUserCount;
	map<DWORD, CUser*>			m_mapUser;
};

typedef Singleton<CUserManager> UserManager;

#endif