//===========================================================================
// FileName:				WifiServer.cpp
// 
// Desc:				
//============================================================================
#include "stdafx.h"
#include "BaseClient.h"
#include "BaseServer.h"
#include "LANClient.h"
#include "LANServer.h"

CLANServer::CLANServer()
{
	 
}

CLANServer::~CLANServer()
{

}

//
// Create server socket
//
SOCKET CLANServer::CreateServerSocket()
{
	SOCKET ServerSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	return ServerSocket;
}

//
// Bind server socket
//
int CLANServer::BindServerSocket()
{
	if( m_ServerSocket == INVALID_SOCKET )
		return SOCKET_ERROR;

	// listen
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family		= AF_INET;
	serverAddr.sin_port			= htons(m_nPort);
	serverAddr.sin_addr.s_addr	= htonl(INADDR_ANY);

	int ret = ::bind(m_ServerSocket, (sockaddr*)&serverAddr, sizeof(serverAddr) );
	return ret;

}

//
// Set server service
//
int CLANServer::SetServerService()
{
	return 0;
}

//
// Accept connection
//
int CLANServer::AcceptConnection()
{
	if( m_ServerSocket == INVALID_SOCKET )
		return SOCKET_ERROR;

	SOCKADDR_IN ClientAddr;
	int clientLen = sizeof(ClientAddr); 

	SOCKET ClientSocket = accept(m_ServerSocket, (sockaddr*)&ClientAddr, &clientLen);
	if( ClientSocket == INVALID_SOCKET )
		return SOCKET_ERROR;

	// set socket option
	BOOL bDontLinger = FALSE; 
	setsockopt(ClientSocket, SOL_SOCKET, SO_DONTLINGER, (const char*)&bDontLinger, sizeof(BOOL));

	// create one cClientConnection
	CBaseClient* pClientConnection = new CLANClient;
	if( pClientConnection == NULL )
	{
		closesocket(ClientSocket);
		return SOCKET_ERROR;
	}

	pClientConnection->Initialize(ClientSocket, (SOCKADDR*)&ClientAddr);
	pClientConnection->SetRecvCloseNotifyFunc((ON_PACKET_RECV_FUNC)m_pPacketProcessFunc,  (ON_SOCKET_CLOSE_FUNC)m_pSocketCloseFunc);

	EnterCriticalSection(&m_csClientConnections);
	m_mapClientConnections[ClientSocket] = pClientConnection;
	m_mapClients[pClientConnection] = 1;
	LeaveCriticalSection(&m_csClientConnections);

	// associate socket to completion port
	CreateIoCompletionPort((HANDLE)ClientSocket, m_hCompletionpPort, (ULONG)ClientSocket, 0);

	// post a recv request
	BOOL res = pClientConnection->RecvPacket();
	if( !res )
	{
		OnSocketClosed(ClientSocket);
	}

	return 0;

}