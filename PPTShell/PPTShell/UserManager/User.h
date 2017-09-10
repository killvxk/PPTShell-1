//===========================================================================
// FileName:				User.h
//	
// Desc:				 
//============================================================================
#ifndef _USER_H_
#define _USER_H_
#include <set>
enum 
{
	PER_NONE			= 1 << 0,
	PER_COMMENT			= 1 << 1,
	PER_INTERACTIVE		= 1 << 2,
	PER_SWITCH_PAGE		= 1 << 3,
	PER_SEND_FILE		= 1 << 4,

	PER_DEAFULT			= PER_COMMENT | PER_INTERACTIVE,
	PER_ALL				= PER_COMMENT | PER_INTERACTIVE | PER_SEND_FILE | PER_SWITCH_PAGE,
};


class CUser
{
public:
	CUser();
	~CUser();

	void				Login(SOCKET sock, DWORD dwUid, bool bLeader, DWORD dwPermission, int nConnectType, TCHAR* szName);
	void				Logoff();

	inline	DWORD		GetUid()						{	return m_dwUID;				}	
	inline	void		SetUid(DWORD dwUID)				{	m_dwUID = dwUID;			}

	inline	DWORD		GetSocket()						{	return m_hSocket;			}
	inline	void		SetSocket(SOCKET sock)			{	m_hSocket = sock;			}

	inline	BOOL		IsLeader()						{	return m_bLeader;			}
	inline	void		SetLeader(BOOL bLeader)			{	m_bLeader = bLeader;		}

	inline	TCHAR*		GetName()						{	return m_szName;			}
	inline	void		SetName(TCHAR* szName)			{	_tcsncpy(m_szName, szName, MAX_PATH); }

	BOOL		IsOnline()	;

	inline	DWORD		GetConnectType()				{	return m_nConnectType;		}
	inline	void		SetConnectType(int nType)		{	m_nConnectType	= nType;	}

	inline	DWORD		GetPermission()					{	return m_dwPermission;		}
	inline	void		SetPermission(DWORD dwPerm)		{	m_dwPermission = dwPerm;	}

	const std::set<int>& GetUsrOwnSlidePages()	{ return m_setSlidePages; }
	void AddUsrOwnSlidePage(int iPageIdx) { m_setSlidePages.insert(iPageIdx); }
	void ClearUserOwnSlidePage() {m_setSlidePages.clear();}
private:
	DWORD		m_dwUID;
	DWORD		m_hSocket;

	DWORD		m_dwPermission;
	int			m_nConnectType;

	BOOL		m_bLeader;
	BOOL		m_bOnline;

	TCHAR		m_szName[MAX_PATH];
	
	 std::set<int> m_setSlidePages;		//该用户目前拥有的幻灯片页
};

#endif