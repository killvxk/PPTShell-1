//===========================================================================
// FileName:				BaseServer.h
// 
// Desc:				
//============================================================================
#include "stdafx.h"
#include "BaseClient.h"
#include "BaseServer.h"

CBaseServer* CBaseServer::m_pInst = nullptr;

CBaseServer* CBaseServer::CreateInstance()
{
	if (m_pInst == nullptr)
	{
		m_pInst = new CBaseServer();
	}
	else
	{
		WRITE_LOG_SERVER("CBaseServer instance has be created! Please call interface[GetInstance] to get Instance!");
	}
	return m_pInst;
}

CBaseServer* CBaseServer::GetInstance()
{
	if (m_pInst == nullptr)
	{
		WRITE_LOG_SERVER("CBaseServer has not be created! You will get a null instance!");
	}
	return m_pInst;
}

void CBaseServer::DestroyInstance()
{
	if (m_pInst != nullptr)
	{
		delete m_pInst;
		m_pInst = nullptr;
	}
}

CBaseServer::CBaseServer()
{
	m_ServerSocket			= INVALID_SOCKET;
	m_hCompletionpPort		= NULL;
	m_nPort					= 0;
}

CBaseServer::~CBaseServer()
{
	DeleteCriticalSection(&m_csClientConnections);
	DeleteCriticalSection(&m_csMobileConn);
	DeleteCriticalSection(&m_csPCMainConn);
	DeleteCriticalSection(&m_csCachedPackets);
}

//
// initialize
//
BOOL CBaseServer::Initialize()
{
	InitializeCriticalSection(&m_csClientConnections);
	InitializeCriticalSection(&m_csMobileConn);
	InitializeCriticalSection(&m_csPCMainConn);
	InitializeCriticalSection(&m_csCachedPackets);
	m_mapClientConnections.clear();
	m_setAllClients.clear();
	m_mapMobiles.clear();
	m_mapPCMainConn.clear();
	m_mapCachedPackets.clear();
	return TRUE;
}

//
// Start to listen 
//
BOOL CBaseServer::StartWork(int nPort /*= 0*/)
{
	m_nPort = nPort;

	m_ServerSocket = CreateServerSocket();
	if( m_ServerSocket == INVALID_SOCKET )
		return FALSE;

	int ret = BindServerSocket();
	if( ret == SOCKET_ERROR )
		return FALSE;

	ret = listen(m_ServerSocket, SOMAXCONN);
	if( ret == SOCKET_ERROR )
		return FALSE;


	// create completion port
	m_hCompletionpPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if( m_hCompletionpPort == NULL )
		return false;

	m_bWorking = TRUE;

	// create accept thread
	HANDLE hdThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AcceptThread, this, 0, NULL);
	CloseHandle(hdThread);

	// create worker thread
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);

	for (unsigned int i = 0; i < SystemInfo.dwNumberOfProcessors; i++)
	{
		hdThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)IOCPWorkThread, this, 0, NULL);
		CloseHandle(hdThread);
	}

	return TRUE;
}


//
// accept thread
//
DWORD WINAPI CBaseServer::AcceptThread(LPARAM lParam)
{
	CBaseServer* pThis = (CBaseServer*)lParam;
	if( pThis == NULL )
		return 0;

	while( pThis->IsWorking() )
		pThis->AcceptConnection();

	return 0;
}

//
// completion port worker thread
//
DWORD WINAPI CBaseServer::IOCPWorkThread(LPARAM lParam)
{
	SOCKET sock = INVALID_SOCKET;
	DWORD					NumberOfBytesTransfered = 0;
	LPOVERLAPPED			lpOverlapped;


	CBaseServer* pThis = (CBaseServer*)lParam;
	if( pThis == nullptr )
		return 0;


	while(pThis->IsWorking())
	{
		BOOL ret = GetQueuedCompletionStatus(pThis->GetIOCPHandle(), &NumberOfBytesTransfered, (PULONG_PTR)&sock, &lpOverlapped, INFINITE);
		if (sock == INVALID_SOCKET)	//the way to break thread, use PostQueuedCompletionStatus
		{
			WRITE_LOG_SERVER("CBaseServer::IOCPWorkThread, Recieve break signal!!");
			break;
		}

		if (NumberOfBytesTransfered==0 && lpOverlapped==nullptr)
		{
			WRITE_LOG_SERVER("IOCPWorkThread recieve stop status,[SocketClose:%u]", sock);
			pThis->OnSocketClosed(sock);
			continue;
		}
		
		if(!ret)
		{
			WRITE_LOG_SERVER("GetQueuedCompletionStatus error---sock:%u", sock);
			pThis->OnSocketClosed(sock);
			continue;
		}
		
		// error occured
		if (NumberOfBytesTransfered == 0 || lpOverlapped == NULL)
		{
			WRITE_LOG_SERVER("Error occured!! sock:%u", sock);
			pThis->OnSocketClosed(sock);
			continue;
		}

		WSAOVERLAPPED_EX* pOverlappedEx = (WSAOVERLAPPED_EX*)lpOverlapped;

		// we recv data we want from client
		pThis->OnDataTransfered(sock, NumberOfBytesTransfered, pOverlappedEx);
	}

	return 0;
}

BOOL CBaseServer::OnDataTransfered(SOCKET sock, DWORD dwNumberOfBytesTransfered, WSAOVERLAPPED_EX* lpOverlapped)
{
	EnterCriticalSection(&m_csClientConnections);
	CBaseClient * pClient = GetClientBySock(sock);
	if (pClient == nullptr)
	{
		LeaveCriticalSection(&m_csClientConnections);
		return FALSE;
	}

	pClient->OnDataTransfered(dwNumberOfBytesTransfered, lpOverlapped);

	LeaveCriticalSection(&m_csClientConnections);
	return TRUE;
}

BOOL CBaseServer::OnSocketClosed(SOCKET sock)
{
	EnterCriticalSection(&m_csClientConnections);
	CBaseClient * pClient = GetClientBySock(sock);
	if (pClient == nullptr)
	{
		LeaveCriticalSection(&m_csClientConnections);
		return FALSE;
	}

	DWORD dwUID = pClient->GetUID();
	DWORD dwPPTID = pClient->GetPPTID();

	WRITE_LOG_SERVER("CBaseServer::OnSocketClosed, sock:%u type: %d, PPTID: %u, MobileID: %u", pClient->GetSocket(), pClient->GetType(), dwPPTID, dwUID);

	if (dwUID>0)
	{
		EnterCriticalSection(&m_csMobileConn);
		auto itrMb = m_mapMobiles.find(dwUID);
		if (itrMb != m_mapMobiles.end() && itrMb->second == pClient)		//因为这个值有可能已经是新创建的连接
		{
			m_mapMobiles.erase(itrMb);
		}
		LeaveCriticalSection(&m_csMobileConn);

		if (pClient->GetType() == EClientType::CTYPE_MOBILE)
		{
			EnterCriticalSection(&m_csCachedPackets);
			m_mapCachedPackets.erase(dwUID);
			LeaveCriticalSection(&m_csCachedPackets);
		}
	}

	if (pClient->IsPCMainClient())
	{
		EnterCriticalSection(&m_csPCMainConn);
		auto itrPC = m_mapPCMainConn.find(dwPPTID);
		if (itrPC != m_mapPCMainConn.end() && itrPC->second == pClient)		//因为这个值有可能已经是新创建的连接
		{
			m_mapPCMainConn.erase(itrPC);
		}
		LeaveCriticalSection(&m_csPCMainConn);
	}

	
	m_mapClientConnections.erase(pClient->GetSocket());
	m_setAllClients.erase(pClient);
	
	pClient->OnSocketClosed();

	delete pClient;

	LeaveCriticalSection(&m_csClientConnections);

	return TRUE;
}

//
// Create server socket
//
SOCKET CBaseServer::CreateServerSocket()
{
	SOCKET ServerSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	return ServerSocket;
}

//
// Bind server socket
//
int CBaseServer::BindServerSocket()
{
	if( m_ServerSocket == INVALID_SOCKET )
		return SOCKET_ERROR;

	// listen
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family		= AF_INET;
	serverAddr.sin_port			= htons(m_nPort);
	serverAddr.sin_addr.s_addr	= htonl(INADDR_ANY);

	int ret = bind(m_ServerSocket, (sockaddr*)&serverAddr, sizeof(serverAddr) );
	return ret;

}


//
// Accept connection
//
int CBaseServer::AcceptConnection()
{
	if( m_ServerSocket == INVALID_SOCKET )
		return SOCKET_ERROR;

	SOCKADDR_IN ClientAddr;
	int clientLen = sizeof(ClientAddr); 

	SOCKET ClientSocket = accept(m_ServerSocket, (sockaddr*)&ClientAddr, &clientLen);
	if( ClientSocket == INVALID_SOCKET )
		return SOCKET_ERROR;

	// create one cClientConnection
	CBaseClient* pClientConnection = new CBaseClient();
	if( pClientConnection == NULL )
	{
		closesocket(ClientSocket);
		return SOCKET_ERROR;
	}

	pClientConnection->Initialize(ClientSocket, (SOCKADDR*)&ClientAddr);

	// add to list
	EnterCriticalSection(&m_csClientConnections);
	auto itrOldConn = m_mapClientConnections.find(ClientSocket);
	if (itrOldConn != m_mapClientConnections.end())
	{
		if (m_setAllClients.find(itrOldConn->second) != m_setAllClients.end())
		{
			m_setAllClients.erase(itrOldConn->second);
		}
		delete itrOldConn->second;
		m_mapClientConnections.erase(itrOldConn);
		WRITE_LOG_SERVER("!!!!!!!!!!!!!!It's impossible to get here!!! It means a socketfd have not release,but now it's reused!!");
	}
	m_mapClientConnections[ClientSocket] = pClientConnection;
	m_setAllClients.insert(pClientConnection);

	WRITE_LOG_SERVER("NEW-------[Connection：%u]: %s:%d, ClientConnection = %08lX", ClientSocket, inet_ntoa(ClientAddr.sin_addr), ntohs(ClientAddr.sin_port), pClientConnection);

	// associate socket to completion port
	CreateIoCompletionPort((HANDLE)ClientSocket, m_hCompletionpPort, (ULONG)ClientSocket, 0);

	// post a recv request
	BOOL res = pClientConnection->RecvPacket();
	if( !res )
	{
		WRITE_LOG_SERVER("ERROR----:[RecvPacket]: failed: %s:%d", inet_ntoa(ClientAddr.sin_addr), ntohs(ClientAddr.sin_port));
		OnSocketClosed(ClientSocket);
	}

	LeaveCriticalSection(&m_csClientConnections);
	return 0;
}

//
// Check client whether it is exist 
//
CBaseClient* CBaseServer::GetClientBySock(SOCKET sock)
{	
	CBaseClient *pClient = nullptr;
	auto itr = m_mapClientConnections.find(sock);
	if (itr != m_mapClientConnections.end())
	{
		pClient = itr->second;
	}
	return pClient;
}


BOOL CBaseServer::RegistClient(EClientType clientType, CBaseClient *pClient)
{
	if (clientType == EClientType::CTYPE_MOBILE)
	{
		EnterCriticalSection(&m_csMobileConn);
		DWORD dwUID = pClient->GetUID();
		auto itrMBClient = m_mapMobiles.find(dwUID);
		if (itrMBClient==m_mapMobiles.end() || itrMBClient->second==nullptr)
		{
			m_mapMobiles[dwUID] = pClient;
			WRITE_LOG_SERVER("CBaseServer::RegistClient, it's a --【mobile】-- client, a 【clean】 client.socket:%u, UID:%u, PPTID:%u", pClient->GetSocket(), dwUID, pClient->GetPPTID());
		}
		else if (pClient != itrMBClient->second)
		{
			CBaseClient *pOldMBClient = itrMBClient->second;
			m_mapMobiles[dwUID] = pClient;
			CBaseClient *pPCClient = pOldMBClient->GetOppositeClient();
			pOldMBClient->SetOppositeClient(nullptr);
			PostCompletionStopStatus(pOldMBClient);

			pClient->SetOppositeClient(pPCClient);
			if (pPCClient!=nullptr)
			{
				pPCClient->SetOppositeClient(pClient);
			}
			WRITE_LOG_SERVER("CBaseServer::RegistClient, it's a  --【mobile】--  client, a 【dirty】 client.socket:%u, UID:%u, PPTID:%u", pClient->GetSocket(), dwUID, pClient->GetPPTID());
		}
		LeaveCriticalSection(&m_csMobileConn);
	}
	else if (clientType == EClientType::CTYPE_PPTSHELL)
	{
		DWORD dwUID = pClient->GetUID();
		DWORD dwPPTID = pClient->GetPPTID();
		if (dwUID == 0)	//主连接注册
		{
			EnterCriticalSection(&m_csPCMainConn);
			auto itrMainClient = m_mapPCMainConn.find(dwPPTID);
			if (itrMainClient == m_mapPCMainConn.end() || itrMainClient->second == nullptr)
			{
				m_mapPCMainConn[dwPPTID] = pClient;
				WRITE_LOG_SERVER("CBaseServer::RegistClient, it's a  --【pc main】--  client, a 【clean】 client.socket:%u, PPTID:%u, UID:0", pClient->GetSocket(), pClient->GetPPTID());
			}
			else
			{
				CBaseClient *pMainClient = itrMainClient->second;	//pc端又发来注册，说明上一次的连接已经失效，注销相关连接信息
				itrMainClient->second = pClient;
				WRITE_LOG_SERVER("ERROR----:CBaseServer::RegistClient, it's a --【pc main】-- client, a 【dirty】 client.socket:%u, PPTID:%u, UID:0", pClient->GetSocket(), pClient->GetPPTID());
				PostCompletionStopStatus(pMainClient);
			}
			pClient->SetPCMainClient();
			LeaveCriticalSection(&m_csPCMainConn);
		}
		else		//PC端为每个连上来的手机创建的连接注册 （20160121 先这样。。否则PC端也要做改造 比较麻烦 这次先解决稳定性的问题）
		{
			EnterCriticalSection(&m_csMobileConn);
			auto itrMBClient = m_mapMobiles.find(dwUID);
			if (itrMBClient == m_mapMobiles.end() || itrMBClient->second == nullptr)
			{
				//这种情况 说明没有对应的手机连接 可以直接关闭PC端连接
//				PostCompletionStopStatus(pClient);	//调用出触发
				if (itrMBClient != m_mapMobiles.end())
				{
					m_mapMobiles.erase(itrMBClient);		//基本不可能到这里
					WRITE_LOG_SERVER("ERROR----:CBaseServer::RegistClient, pc conn for mobile, impossible to get here!");
				}
				WRITE_LOG_SERVER("CBaseServer::RegistClient, it's a  --【PC】--  client, Can't find opposite mobile client.socket:%u, UID:%u, PPTID:%u", pClient->GetSocket(), dwUID, dwPPTID);
				LeaveCriticalSection(&m_csMobileConn);
				return FALSE;
			}
			else
			{
				CBaseClient *pMBClient = itrMBClient->second;
				CBaseClient *pOldPCClient = pMBClient->GetOppositeClient();
				if (pOldPCClient != nullptr)
				{
					pOldPCClient->SetOppositeClient(nullptr);
					PostCompletionStopStatus(pOldPCClient);
					WRITE_LOG_SERVER("CBaseServer::RegistClient, it's a --【PC】--  client, find a Old connection.socket:%u, UID:%u, PPTID:%u, oldsock:%u", pClient->GetSocket(), dwUID, pOldPCClient->GetPPTID(), pOldPCClient->GetSocket());
				}
				pMBClient->SetOppositeClient(pClient);
				pClient->SetOppositeClient(pMBClient);
				WRITE_LOG_SERVER("CBaseServer::RegistClient, it's a --【PC】-- client, regist success.socket:%u, UID:%u, PPTID:%u", pClient->GetSocket(), dwUID, dwPPTID);
			}
			LeaveCriticalSection(&m_csMobileConn);
		}
	}
	else
	{
		WRITE_LOG_SERVER("BaseServer::RegistClient, 【【Unknow】】  client type:%d, socket:%u, UID:%u, PPTID:%u", clientType, pClient->GetSocket(), pClient->GetUID(), pClient->GetPPTID());
		return FALSE;
	}
	return TRUE;
}

CBaseClient* CBaseServer::GetPCMainClientByPPTID(DWORD dwPPTID)
{
	EnterCriticalSection(&m_csPCMainConn);
	CBaseClient *pClient = nullptr;
	auto itrMainClient = m_mapPCMainConn.find(dwPPTID);
	if (itrMainClient != m_mapPCMainConn.end())
	{
		pClient = itrMainClient->second;
	}
	LeaveCriticalSection(&m_csPCMainConn);
	return pClient;
}

void CBaseServer::PostCompletionStopStatus(CBaseClient *pClient)
{
	PostQueuedCompletionStatus(m_hCompletionpPort, 0, pClient->GetSocket(), nullptr);
}

void CBaseServer::GetCachedPacketByUID(DWORD dwUID, std::vector<char *> &vecCachedPackets)
{
	vecCachedPackets.clear();
	EnterCriticalSection(&m_csCachedPackets);
	auto pos = m_mapCachedPackets.equal_range(dwUID);
	if (pos.first == m_mapCachedPackets.end())
	{
		LeaveCriticalSection(&m_csCachedPackets);
		return;
	}

	while (pos.first != pos.second)
	{
		vecCachedPackets.push_back(pos.first->second);
		++pos.first;
	}
	m_mapCachedPackets.erase(dwUID);
	LeaveCriticalSection(&m_csCachedPackets);
	return;
}

void CBaseServer::AddCachedPacket(DWORD dwUID, char *pData)
{
	EnterCriticalSection(&m_csCachedPackets);
	m_mapCachedPackets.insert(std::make_pair(dwUID, pData));
	LeaveCriticalSection(&m_csCachedPackets);
}

void CBaseServer::ShowCurrInfo()
{
	string strLogMsg = "		SERVER CURRENT INFO\n";
	strLogMsg += "*******************************************************\n";
	strLogMsg += "* PPTShell   CNT : ";    strLogMsg += toString(m_mapPCMainConn.size()); strLogMsg += "\n";
	strLogMsg += "* Mobile     CNT : ";		strLogMsg += toString(m_mapMobiles.size()); strLogMsg += "\n";
	strLogMsg += "* Total        CNT : ";		strLogMsg += toString(m_mapClientConnections.size()); strLogMsg += "\n";
	strLogMsg += "* CachePkg CNT : ";		strLogMsg += toString(m_mapCachedPackets.size()); strLogMsg += "\n";
	strLogMsg += "*******************************************************\n\n";
	WRITE_LOG_GLOBAL("%s", strLogMsg.c_str());
//	printf("%s", strLogMsg.c_str());
}