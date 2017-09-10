//===========================================================================
// FileName:				WANClientManager.h
// 
// Desc:				
//============================================================================
#ifndef _WAN_CLIENT_MANAGER_H_
#define _WAN_CLIENT_MANAGER_H_

#include "Util/Singleton.h"
#include "WANThread.h"

class CWANClientManager
{
private:
	CWANClientManager();
	~CWANClientManager();

public:
	struct SHeartBeatInfo
	{
		bool ifReConnect;		//是否重连
		SHeartBeatInfo():ifReConnect(false)
		{}
		SHeartBeatInfo(bool bReconn):ifReConnect(bReconn)
		{
			
		}
	};
	BOOL					Initialize(char* szWanServerIP, int nWanServerPort, DWORD dwPPTID);
	BOOL					Destroy();
	BOOL					SendPacket(SOCKET sock, char* pData, int nDataSize);
	BOOL					SetRecvCloseNotifyFunc(ON_PACKET_RECV_FUNC pPacketProcessFunc, ON_SOCKET_CLOSE_FUNC pSocketCloseFunc);

	void					OnMainWanClientPacket(SOCKET sock, int type, char* packet);
	void					OnWanClientPacket(SOCKET sock, int type, char* packet);
	void					OnWanClientClosed(SOCKET sock);

	DECLARE_SINGLETON_CLASS(CWANClientManager);

protected:
	CWANClient*				FindWanClient(DWORD dwUID);
	BOOL					MainClientConnect();
	static DWORD WINAPI		MainClientConnectThread(LPARAM lParam);

	CWANClient*				FindWanClient(SOCKET sockFind);
	bool					SleepInterrupt(time_t tStart, unsigned long ulSleepMS, bool& bBreak);
	
	//心跳处理线程 管理需要心跳的所有连接的心跳以及负责重连
	static DWORD WINAPI		HeartbeatThread(LPARAM lParam);
	void DealHeartbeat();
	void					AddToHeartBeatList(CWANClient* client, bool bIfReconnect);
protected:
	char							m_szWanServerIP[MAX_PATH];
	int								m_nWanServerPort;
	DWORD							m_dwPPTID;

	CWANClient*						m_pMainClient;		// main 
	map<SOCKET, CWANClient*>		m_mapClients;		// SOCKET <--> CWANClient*
	map<DWORD, CWANClient*>			m_mapUIDClients;	// UID	  <--> CWANClient*
	CRITICAL_SECTION				m_Lock;

	map<CWANClient*, SHeartBeatInfo> m_mapHeartbeatClients;
	HANDLE					m_hEventHeartbeatStart;
	HANDLE					m_hThreadHeartbeat;
	HANDLE					m_hThreadMainClientConn;

	ON_PACKET_RECV_FUNC				m_pPacketProcessFunc;
	ON_SOCKET_CLOSE_FUNC			m_pSocketCloseFunc;

	bool			m_bCloseApp;		// 程序是否退出
	CWANThread		m_cWanThread;		// 线程类,负责所有WAN客户端数据收发
};

typedef Singleton<CWANClientManager> WANClientManager;

#endif