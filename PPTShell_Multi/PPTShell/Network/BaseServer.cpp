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
	DeleteCriticalSection(&m_csClientConnections);
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

	ret = SetServerService();
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

	for(DWORD i = 0; i < 1 /*SystemInfo.dwNumberOfProcessors*/; i++)
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
	SOCKET					sock = INVALID_SOCKET;


	CBaseServer* pThis = (CBaseServer*)lParam;
	if( pThis == NULL )
		return 0;


	while( pThis->IsWorking() )
	{
		BOOL ret = GetQueuedCompletionStatus(pThis->GetIOCPHandle(), &NumberOfBytesTransfered, (PULONG_PTR)&sock, &lpOverlapped, INFINITE);
		if (sock == INVALID_SOCKET)	//the way to break thread, use PostQueuedCompletionStatus
		{
			WRITE_LOG_SERVER("CBaseServer::IOCPWorkThread, Recieve break signal!!");
			break;
		}

		if (NumberOfBytesTransfered==0 && lpOverlapped == NULL)
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

//
// set packet sent notify function
//
BOOL CBaseServer::SetPacketSentNotifyFunc(SOCKET sock, PVOID pPacketSentFunc)
{
	map<SOCKET, CBaseClient*>::iterator itr = m_mapClientConnections.find(sock);
	if( itr != m_mapClientConnections.end() )
	{
		CBaseClient* pClient = itr->second;
		return pClient->SetPacketSentNotifyFunc((ON_PACKET_SENT_FUNC)pPacketSentFunc);
	}

	return FALSE;
}

BOOL CBaseServer::OnSocketClosed(SOCKET sock)
{
	EnterCriticalSection(&m_csClientConnections);

	CBaseClient* pClient = GetClientBySock(sock);
	if (pClient == NULL)
	{
		LeaveCriticalSection(&m_csClientConnections);
		return FALSE;
	}

	pClient->OnSocketClosed();
	m_mapClients.erase(pClient);
	m_mapClientConnections.erase(sock);

	delete pClient;
	pClient = NULL;

	LeaveCriticalSection(&m_csClientConnections);

	return TRUE;
}

BOOL CBaseServer::IsSocketOnLine(SOCKET sock)
{
	BOOL bExist = FALSE;

	EnterCriticalSection(&m_csClientConnections);

	if( m_mapClientConnections.find(sock) != m_mapClientConnections.end() )
		bExist = TRUE;

	LeaveCriticalSection(&m_csClientConnections);
	return bExist;
}

//
// Check client whether it is exist 
//
CBaseClient* CBaseServer::GetClientBySock(SOCKET sock)
{	
	CBaseClient *pClient = NULL;
	std::map<SOCKET, CBaseClient*>::iterator itr = m_mapClientConnections.find(sock);
	if (itr != m_mapClientConnections.end())
	{
		pClient = itr->second;
	}

	return pClient;
}

BOOL CBaseServer::OnDataTransfered(SOCKET sock, DWORD dwNumberOfBytesTransfered, WSAOVERLAPPED_EX* lpOverlapped)
{
	EnterCriticalSection(&m_csClientConnections);
	CBaseClient * pClient = GetClientBySock(sock);
	if (pClient == NULL)
	{
		LeaveCriticalSection(&m_csClientConnections);
		return FALSE;
	}

	pClient->OnDataTransfered(dwNumberOfBytesTransfered, lpOverlapped);

	LeaveCriticalSection(&m_csClientConnections);
	return TRUE;
}
