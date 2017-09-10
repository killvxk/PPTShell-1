//===========================================================================
// FileName:				WANClientManager.h
// 
// Desc:				
//============================================================================
#include "stdafx.h"
#include "PacketProcess/Packet.h"
#include "WANClient.h"
#include "WANClientManager.h"

const int HEARTBEAT_RATE = 300000;				//单位毫秒，心跳频率，180s
const int HEARTBEAT_TIMEOFF = 15000;			//每个心跳包发出超时时间 10s内还没收到应答包就重新发送，连发三次都未收到 认为网络已经断开 进行重连
const int HEARTBEAT_SEND_CNT = 3;				//心跳包发送次数，超过这个次数还没有收到应答认为网络断开 

CWANClientManager* g_pWanClientManager = NULL;

void OnMainWanClientPacket(SOCKET sock, int type, char* packet);
void OnWanClientPacket(SOCKET sock, int type, char* packet);
void OnWanClientClosed(SOCKET sock);			//处理主连接以及为每个手机建立的连接

CWANClientManager::CWANClientManager()
{
	m_pMainClient			= NULL;
	g_pWanClientManager		= this;
	m_bCloseApp				= false;
	m_hThreadHeartbeat		= NULL;
	m_hThreadMainClientConn = NULL;
}

CWANClientManager::~CWANClientManager()
{
	m_bCloseApp	= true;
	SetEvent(m_hEventHeartbeatStart);

	if (m_hEventHeartbeatStart != NULL)
		WaitForSingleObject(m_hThreadHeartbeat, INFINITE);

	if(m_hThreadMainClientConn == NULL)
		WaitForSingleObject(m_hThreadMainClientConn, INFINITE);

	// 让线程类先退出，再关闭所有客户端SOCKET
	m_cWanThread.Destroy();
	
	//OutputDebugString(_T("Start Clean WanClient\n"));

	EnterCriticalSection(&m_Lock);
	map<SOCKET, CWANClient*>::iterator iter= m_mapClients.begin();
	for(; iter!=m_mapClients.end(); iter++)
	{
		iter->second->CloseSocket();
		if(iter->second != NULL)
		{
			delete iter->second;
			iter->second = NULL;
		}
	}
	m_mapClients.clear();
	m_mapUIDClients.clear();
	LeaveCriticalSection(&m_Lock);


	DeleteCriticalSection(&m_Lock);
	OutputDebugString(_T("CWANClientManager Destructor\n"));
}

//
// initialize
//
BOOL CWANClientManager::Initialize(char* szWanServerIP, int nWanServerPort, DWORD dwPPTID)
{
	m_nWanServerPort	= nWanServerPort;
	m_dwPPTID			= dwPPTID;

	strcpy_s(m_szWanServerIP, MAX_PATH, szWanServerIP);

	InitializeCriticalSection(&m_Lock);

	// create main client
	m_pMainClient = new CWANClient;
	if( m_pMainClient == NULL )
		return FALSE;

	m_cWanThread.Initialize();
	m_cWanThread.SetMainClient(m_pMainClient);

	BOOL res = m_pMainClient->Initialize();
	if( !res )
		return FALSE;

	m_pMainClient->SetWanThread(&m_cWanThread);

	m_pMainClient->SetRecvCloseNotifyFunc(::OnMainWanClientPacket, ::OnWanClientClosed);

	SOCKET sock = m_pMainClient->GetSocket();

	m_hEventHeartbeatStart = CreateEvent(NULL, FALSE, FALSE, NULL);
	
	m_hThreadMainClientConn = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MainClientConnectThread, this, 0, NULL);
	m_hThreadHeartbeat = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)HeartbeatThread, this, 0, NULL);

	return TRUE;
}

//
// destroy
//
BOOL CWANClientManager::Destroy()
{

	return TRUE;
}

//
// send packet
//
BOOL CWANClientManager::SendPacket(SOCKET sock, char *pData, int nDataSize)
{
	EnterCriticalSection(&m_Lock);

	map<SOCKET, CWANClient*>::iterator itr = m_mapClients.find(sock);
	if( itr != m_mapClients.end() )
	{
		CWANClient* pWanClient = itr->second;

		
		if( pWanClient != m_pMainClient )
		{
			m_cWanThread.PostSend(pWanClient, pData, nDataSize);
			//pWanClient->SendPacket(pData, nDataSize);
		}
		else
		{
			// Error!!
			ASSERT(FALSE);
		}
	}

	LeaveCriticalSection(&m_Lock);
	return TRUE;
}

//
// process main WAN client packet 
//
void CWANClientManager::OnMainWanClientPacket(SOCKET sock, int type, char *packet)
{
	// check whether it is a register packet
	PACKET_HEAD* pHeader = (PACKET_HEAD*)packet;

	if( pHeader->dwMajorType == PT_REGISTER )
	{
		PACKET_REGISTER* pRegisterPacket = (PACKET_REGISTER*)pHeader;

		// create a connection to WAN server
		CWANClient* pWanClient = new CWANClient;
		if (pWanClient == NULL)
		{
			WRITE_LOG_LOCAL(_T("WANClientManager::OnMainWanClientPacket, new CWANClient fail!"))
			return ;
		}

		BOOL res = pWanClient->Initialize();
		if( !res )
		{
			delete pWanClient;
			WRITE_LOG_LOCAL(_T("WANClientManager::OnMainWanClientPacket, CWANClient Initialize fail!"))
			return;
		}

		res = pWanClient->Connect(m_szWanServerIP, m_nWanServerPort);
		if( !res )
		{
			delete pWanClient;
			WRITE_LOG_LOCAL(_T("WANClientManager::OnMainWanClientPacket, Connect PPTServer fail!ip:%s, port:%d"), m_szWanServerIP, m_nWanServerPort)
			return;
		}
		pWanClient->SetRecvCloseNotifyFunc(::OnWanClientPacket, ::OnWanClientClosed);

		// 关联完成端口
		pWanClient->SetWanThread(&m_cWanThread);
		res = m_cWanThread.AssociateIOCP(pWanClient);
		if( !res )
		{
			WRITE_LOG_LOCAL(_T("WanClient Associate IOCP Failed"));
			pWanClient->CloseSocket();
			delete pWanClient;
			pWanClient = NULL;
			return;
		}

		res = pWanClient->SendRegisterPacket(pRegisterPacket->dwMobileID, m_dwPPTID);
		if( !res )
		{
			pWanClient->CloseSocket();
			delete pWanClient;
			pWanClient = NULL;
			return;
		}

		SOCKET ClientSocket = pWanClient->GetSocket();
	
		EnterCriticalSection(&m_Lock);
		map<DWORD, CWANClient*>::iterator itrOldClient = m_mapUIDClients.find(pRegisterPacket->dwMobileID);
		if (itrOldClient != m_mapUIDClients.end())
		{
			CWANClient *pOldClient = itrOldClient->second;
			if (pOldClient->GetSocket()!=INVALID_SOCKET)
			{
				pOldClient->CloseSocket();
			}
			else
			{
				OutputDebugString(_T("OnMainWanClientPacket - Delete InvalidSocket OldClient\n"));
				m_mapClients.erase(pOldClient->GetSocket());
				m_mapUIDClients.erase(itrOldClient);
				//pOldClient->Destroy();
				delete pOldClient;
			}
		
		}
		m_mapClients[ClientSocket] = pWanClient; 
		m_mapUIDClients[pRegisterPacket->dwMobileID] = pWanClient;
		LeaveCriticalSection(&m_Lock);

	}
	else if( m_pPacketProcessFunc != NULL )
		m_pPacketProcessFunc(sock, type, packet);
}

//
// process normal WAN client packet
//
void CWANClientManager::OnWanClientPacket(SOCKET sock, int type, char* packet)
{
	// let packet processor process it
	if( m_pPacketProcessFunc != NULL )
		m_pPacketProcessFunc(sock, type, packet);
}

//
// process main WAN client socket closed
//
void CWANClientManager::OnWanClientClosed(SOCKET sock)
{
	// 关闭程序时由析构函数关掉所有客户端
	if(m_bCloseApp)
		return;

	EnterCriticalSection(&m_Lock);
	map<SOCKET, CWANClient*>::iterator itr= m_mapClients.find(sock);
	if (itr == m_mapClients.end())
	{
		return;
	}
	CWANClient *pClient = itr->second;
	
	if (pClient == m_pMainClient)
	{
		m_mapClients.erase(sock);
		WRITE_LOG_LOCAL(_T("CWANClientManager::OnWanClientClosed, MainClient Closed,UID:%u"), pClient->GetUID());
		//如果是主连接，则打开连接线程，重连
		// reconnect to WAN server
		CloseHandle(m_hThreadMainClientConn);
		m_hThreadMainClientConn = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MainClientConnectThread, this, 0, NULL);
	}
	else
	{
		
		m_mapClients.erase(sock);
		if (pClient == NULL)
		{
			WRITE_LOG_LOCAL(_T("CWANClientManager::OnWanClientClosed, Impossible to get here, sock:%u"), sock);
		}
		else
		{
			WRITE_LOG_LOCAL(_T("CWANClientManager::OnWanClientClosed, WanClient Closed,UID:%u"), pClient->GetUID());

			m_mapUIDClients.erase(pClient->GetUID());
			//pClient->Destroy();
			delete pClient;
			pClient = NULL;
		}
	}
	LeaveCriticalSection(&m_Lock);
}

void OnMainWanClientPacket(SOCKET sock, int type, char* packet)
{
	if( g_pWanClientManager != NULL )
		g_pWanClientManager->OnMainWanClientPacket(sock, type, packet);
}

void OnWanClientPacket(SOCKET sock, int type, char* packet)
{
	if( g_pWanClientManager != NULL )
		g_pWanClientManager->OnWanClientPacket(sock, type, packet);

}

void OnWanClientClosed(SOCKET sock)
{
	if( g_pWanClientManager != NULL )
		g_pWanClientManager->OnWanClientClosed(sock);
}

//
// find WAN Client by UID
//
CWANClient* CWANClientManager::FindWanClient(DWORD dwUID)
{
	map<DWORD, CWANClient*>::iterator itr = m_mapUIDClients.find(dwUID);
	if( itr == m_mapUIDClients.end() )
		return NULL;

	return itr->second;

}

//
// set packet processor
//
BOOL CWANClientManager::SetRecvCloseNotifyFunc(ON_PACKET_RECV_FUNC pPacketProcessFunc, ON_SOCKET_CLOSE_FUNC pSocketCloseFunc)
{
	m_pPacketProcessFunc = pPacketProcessFunc;
	m_pSocketCloseFunc   = pSocketCloseFunc;

	return TRUE;
}

//
// main client connect
//
BOOL CWANClientManager::MainClientConnect()
{
	BOOL res = m_pMainClient->Connect(m_szWanServerIP, m_nWanServerPort);
	if( !res )
	{
		return FALSE;
	}

	// main client's UID is 0
	m_pMainClient->SendRegisterPacket(0, m_dwPPTID);
	AddToHeartBeatList(m_pMainClient, true);
	EnterCriticalSection(&m_Lock);
	m_mapClients[m_pMainClient->GetSocket()] = m_pMainClient;
	LeaveCriticalSection(&m_Lock);

	m_cWanThread.AssociateIOCP(m_pMainClient);
	SetEvent(m_hEventHeartbeatStart);

	return TRUE;
}

DWORD WINAPI CWANClientManager::MainClientConnectThread(LPARAM lParam)
{
	CWANClientManager* pManager = (CWANClientManager*)lParam;
	// 一直尝试知道连上或程序退出为止
	while(!pManager->MainClientConnect() && !pManager->m_bCloseApp)
	{
		Sleep(5000);
	}
	WRITE_LOG_LOCAL(_T("MainClientConnectThread exit"));
	return 0;
}

void CWANClientManager::AddToHeartBeatList(CWANClient* client, bool bIfReconnect)
{
	if (client == NULL || client->GetSocket() == INVALID_SOCKET)
	{
		return;
	}
	SHeartBeatInfo hbInfo;
	hbInfo.ifReConnect = bIfReconnect;
	m_mapHeartbeatClients.insert(make_pair(client, hbInfo));
}

DWORD WINAPI CWANClientManager::HeartbeatThread(LPARAM lParam)
{
	CWANClientManager* pManager = (CWANClientManager*)lParam;
	pManager->DealHeartbeat();

	OutputDebugString(_T("HeartbeatThread Exit\n"));
	return 0;
}

void CWANClientManager::DealHeartbeat()
{
	while(TRUE)
	{
		DWORD ret = WaitForSingleObject(m_hEventHeartbeatStart, INFINITE);
		if(m_bCloseApp)
			break;
		if (ret != 0)
		{
			WRITE_LOG_LOCAL(_T("CWANClientManager::DealHeartbeat, WaitForSingleObject fail!"));
			continue;
		}
		if (m_mapHeartbeatClients.size()==0)
		{
			continue;
		}
		
		WRITE_LOG_LOCAL(_T("----------HeartBeat thread start to work!!!-------------"));

		//开始心跳包处理
		//Sleep(HEARTBEAT_RATE);
		time_t tStart = time(NULL);
		if(!SleepInterrupt(tStart, HEARTBEAT_RATE, m_bCloseApp))
			break;

		while (true)
		{
			Sleep(1);
			map<CWANClient*, SHeartBeatInfo>::iterator itrClient = m_mapHeartbeatClients.begin();
			for (; itrClient!=m_mapHeartbeatClients.end(); itrClient++)
			{
				itrClient->first->SendHeartBeat();
			}
			tStart = time(NULL);		//已发送完心跳包

			//处理应答
			int iDealCnt = 0;
			while (true)
			{
				Sleep(1);
				//Sleep(HEARTBEAT_TIMEOFF);
				if(!SleepInterrupt(tStart, HEARTBEAT_TIMEOFF, m_bCloseApp))
					break;

				bool bAllAck = true;
				iDealCnt ++;
				map<CWANClient*, SHeartBeatInfo>::iterator itrClient = m_mapHeartbeatClients.begin();
				for (; itrClient!=m_mapHeartbeatClients.end(); )
				{
					CWANClient *pClient = itrClient->first;
					SHeartBeatInfo &hbInfo = itrClient->second;
					if (!pClient->IfRcvHeartBeatAck())
					{
						if (iDealCnt==HEARTBEAT_SEND_CNT)
						{
							WRITE_LOG_LOCAL(_T("Have not receive heartbeat ack for 3 times, will  break up the socket! client:%d"), pClient->GetSocket());
							itrClient=m_mapHeartbeatClients.erase(itrClient);		//调用断开处理，并且从心跳包管理列表中删除
							pClient->CloseSocket();		//HEARTBEAT_SEND_CNT次没有收到应答包，认为网络断开，做断开处理
							continue;
						}
						else
						{
							pClient->SendHeartBeat();		//重发心跳包
						}
						bAllAck  = false;
					}
					itrClient ++;
				}

				if (bAllAck)	//所有心跳包都收到应答，退出心跳包应答等待
				{
					break;
				}

				if (iDealCnt == HEARTBEAT_SEND_CNT)
				{
					break;
				}

				if (m_mapHeartbeatClients.size() == 0)
				{
					break;
				}
			}

			if(!SleepInterrupt(tStart, HEARTBEAT_RATE, m_bCloseApp))
				break;
			//time_t tNow = time(NULL);
			//int iInterval = (tNow-tStart)*100;
			//if (iInterval<HEARTBEAT_RATE)
			//{
			//	Sleep(HEARTBEAT_RATE-iInterval);
			//}
		}
	}
}
// 根据SOCKET查找客户端
CWANClient* CWANClientManager::FindWanClient(SOCKET sockFind)
{
	CWANClient* pClient = NULL;
	map<SOCKET, CWANClient*>::const_iterator iter;

	EnterCriticalSection(&m_Lock);
	iter = m_mapClients.find(sockFind);
	if(iter != m_mapClients.end())
		pClient = iter->second;
	LeaveCriticalSection(&m_Lock);

	return pClient;
}
// 总共睡眠ulSleepMS时间，每次睡眠nSleepMS时间，当bBreak为true时结束睡眠
bool CWANClientManager::SleepInterrupt(time_t tStart, unsigned long ulSleepMS, bool& bBreak)
{
	tStart = tStart==0? time(NULL) : tStart;

	time_t tNow;
	int iInterval;
	int nSleepMS = 300;	// 每次Sleep的时间

	while(true)
	{
		// 计算本次Sleep的时间
		tNow = time(NULL);
		iInterval = ulSleepMS - (tNow-tStart) * 100;
		iInterval = iInterval < nSleepMS ? iInterval : nSleepMS;

		// 是否结束睡眠
		if (iInterval <= 0)
			return true;
		else if(bBreak)
			return false;
		else
			Sleep(nSleepMS);
	}
}
