//===========================================================================
// FileName:				BaseServer.h
// 
// Desc:				
//============================================================================
#include "stdafx.h"
#include "BaseClient.h"
#include "BaseServer.h"

CBaseServer::CBaseServer()
{
	m_ServerSocket			= INVALID_SOCKET;
	m_hCompletionpPort		= NULL;
	m_nPort					= 0;
}

CBaseServer::~CBaseServer()
{

}

//
// initialize
//
BOOL CBaseServer::Initialize(PVOID pPacketProcessFunc /* = NULL */, PVOID pSocketCloseFunc /*= NULL*/)
{
	InitializeCriticalSection(&m_csClientConnections);
	m_mapClientConnections.clear();

	m_pPacketProcessFunc = pPacketProcessFunc;
	m_pSocketCloseFunc = pSocketCloseFunc;

	return TRUE;
}

//
// Destroy
//
BOOL CBaseServer::Destroy()
{
	DeleteCriticalSection(&m_csClientConnections);

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
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AcceptThread, this, 0, NULL);

	// create worker thread
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);

	for(int i = 0; i < SystemInfo.dwNumberOfProcessors; i++)
		/*m_hWorkerThreadArray[i] =*/ CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)IOCPWorkThread, this, 0, NULL);

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
	CBaseClient*			pClientConnect = NULL;
	DWORD					NumberOfBytesTransfered = 0;
	LPOVERLAPPED			lpOverlapped;


	CBaseServer* pThis = (CBaseServer*)lParam;
	if( pThis == NULL )
		return 0;


	while( pThis->IsWorking() )
	{
		BOOL ret = GetQueuedCompletionStatus(pThis->GetIOCPHandle(), &NumberOfBytesTransfered, (PULONG_PTR)&pClientConnect, &lpOverlapped, INFINITE);
		
		if( !pThis->IsClientExist(pClientConnect) )
			continue;

		if( !ret )
		{
			if( pClientConnect != NULL )
			{
				WRITE_LOG_SERVER("0---[SocketClose]: type: %d, PPTID: %u, MobileID: %u", pClientConnect->GetType(), pClientConnect->GetPPTID(), pClientConnect->GetUID());
				pThis->OnSocketClosed(pClientConnect);
			}
				

			// release send overlapped
			if( lpOverlapped != NULL )
			{
				WSAOVERLAPPED_EX* pOverlappedEx = (WSAOVERLAPPED_EX*)lpOverlapped;
				if( pOverlappedEx->op == 0 )
				{
					if( pOverlappedEx->buffer != NULL )
					{
						delete pOverlappedEx->buffer;
						pOverlappedEx->buffer = NULL;
					}

					delete pOverlappedEx;
					pOverlappedEx = NULL;
				}
			}

			continue;
		}

		if( pClientConnect == NULL || lpOverlapped == NULL )
		{
			WRITE_LOG_SERVER("pClientConnect == NULL || lpOverlapped == NULL");
			continue;
		}

		if(pThis->m_mapClients.find(pClientConnect)==pThis->m_mapClients.end())
		{
			continue;
		}

		// error occured
		if( NumberOfBytesTransfered == 0 )
		{
			WRITE_LOG_SERVER("1---[SocketClose]: type: %d, PPTID: %u, MobileID: %u", pClientConnect->GetType(), pClientConnect->GetPPTID(), pClientConnect->GetUID());
			pThis->OnSocketClosed(pClientConnect);
			continue;
		}

		WSAOVERLAPPED_EX* pOverlappedEx = (WSAOVERLAPPED_EX*)lpOverlapped;

		// we recv data we want from client
		pClientConnect->OnDataTransfered(NumberOfBytesTransfered, pOverlappedEx);
	}

	return 0;
}

//
// send packet
//
BOOL CBaseServer::SendPacket(SOCKET sock ,char *pData, int nDataSize)
{
	map<SOCKET, CBaseClient*>::iterator itr = m_mapClientConnections.find(sock);
	if( itr != m_mapClientConnections.end() )
	{
		CBaseClient* pClient = itr->second;
		return pClient->SendPacket(pData, nDataSize);
	}

	return FALSE;
}

BOOL CBaseServer::OnSocketClosed(CBaseClient* pClientConnect)
{
	if( pClientConnect == NULL )
		return FALSE;

	// find
	BOOL exist = TRUE;

	EnterCriticalSection(&m_csClientConnections);
	if( m_mapClients.find(pClientConnect) == m_mapClients.end() )
		exist = FALSE;

	m_mapClients.erase(pClientConnect);	
	LeaveCriticalSection(&m_csClientConnections);

	if( !exist )
		return FALSE;

	//
	SOCKET sock = pClientConnect->GetSocket();
	pClientConnect->OnSocketClosed();

	EnterCriticalSection(&m_csClientConnections);
	m_mapClientConnections.erase(sock);
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
	pClientConnection->SetSocketNotifyFunc((ON_PACKET_FUNC)m_pPacketProcessFunc,  (ON_SOCKET_CLOSE_FUNC)m_pSocketCloseFunc);

	// associate socket to completion port
	CreateIoCompletionPort((HANDLE)ClientSocket, m_hCompletionpPort, (ULONG)pClientConnection, 0);

	WRITE_LOG_SERVER("[Connection]: %s:%d, ClientConnection = %08lX", inet_ntoa(ClientAddr.sin_addr), ntohs(ClientAddr.sin_port), pClientConnection);

	// post a recv request
	BOOL res = pClientConnection->RecvPacket();
	if( res )
	{
		// add to list
		EnterCriticalSection(&m_csClientConnections);
		m_mapClientConnections[ClientSocket] = pClientConnection;
		m_mapClients[pClientConnection] = 1;
		LeaveCriticalSection(&m_csClientConnections);

	}
	else
	{
		WRITE_LOG_SERVER("[RecvPacket]: failed: %s:%d", inet_ntoa(ClientAddr.sin_addr), ntohs(ClientAddr.sin_port));
		OnSocketClosed(pClientConnection);
	}

	return 0;

}

//
// Check client whether it is exist 
//
BOOL CBaseServer::IsClientExist(CBaseClient* pClientConnect)
{
	BOOL bExist = FALSE;

	EnterCriticalSection(&m_csClientConnections);
	
	if( m_mapClients.find(pClientConnect) != m_mapClients.end() )
		bExist = TRUE;

	LeaveCriticalSection(&m_csClientConnections);
	return bExist;
}