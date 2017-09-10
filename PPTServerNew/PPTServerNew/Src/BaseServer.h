//===========================================================================
// FileName:				BaseServer.h
// 
// Desc:				
//============================================================================
#ifndef _BASE_SERVER_H_
#define _BASE_SERVER_H_
#include "BaseClient.h"

class CBaseServer
{
public:
	static CBaseServer* CreateInstance();
	static CBaseServer* GetInstance();
	static void DestroyInstance();

	virtual SOCKET	CreateServerSocket();
	virtual int		BindServerSocket();
	virtual int 	AcceptConnection();

 
	BOOL			Initialize();
	BOOL			StartWork(int nPort = 0);
	BOOL			OnSocketClosed(SOCKET sock);
	CBaseClient*	 GetClientBySock(SOCKET sock);
	BOOL			RegistClient(EClientType clientType, CBaseClient *pClient);

	BOOL OnDataTransfered(SOCKET sock, DWORD dwNumberOfBytesTransfered, WSAOVERLAPPED_EX* lpOverlapped);

	inline BOOL		IsWorking()					{ return m_bWorking;		 }
	inline HANDLE	GetIOCPHandle()				{ return m_hCompletionpPort; }

	static DWORD WINAPI AcceptThread(LPARAM lParam);
	static DWORD WINAPI IOCPWorkThread(LPARAM lParam);

	void ShowCurrInfo();
protected:
	CBaseServer();
	~CBaseServer();
	static CBaseServer* m_pInst;

	CBaseClient* GetPCMainClientByPPTID(DWORD dwPPTID);
	void PostCompletionStopStatus(CBaseClient *pClient);
	void GetCachedPacketByUID(DWORD dwUID, std::vector<char *> &vecCachedPackets);
	void AddCachedPacket(DWORD dwUID, char *pData);
protected:
	BOOL									m_bWorking;
	SOCKET									m_ServerSocket;
	HANDLE									m_hCompletionpPort;
	PHANDLE									m_hWorkerThreadArray;
	int										m_nWorkerThreadCount;
	int										m_nPort;

	CRITICAL_SECTION m_csClientConnections;
	std::map<SOCKET, CBaseClient*>	 m_mapClientConnections;
	std::set<CBaseClient*> m_setAllClients;
	CRITICAL_SECTION	m_csMobileConn;
	std::map<DWORD, CBaseClient*> m_mapMobiles;			// UID		<--> Mobile
	CRITICAL_SECTION	m_csPCMainConn;
	std::map<DWORD, CBaseClient*> m_mapPCMainConn;			//PPTID	<---> PPTShells , just main connection
	CRITICAL_SECTION m_csCachedPackets;
	std::multimap<DWORD, char*> m_mapCachedPackets;		// UID      <--> Cached packets before regist

	PVOID									m_pPacketProcessFunc;
	PVOID									m_pSocketCloseFunc;

	friend class CBaseClient;
};

#endif