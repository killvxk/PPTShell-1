//===========================================================================
// FileName:				BlueToothServer.cpp
// 
// Desc:				
//============================================================================
#include "stdafx.h"
#include <ws2bth.h>  
#include "BaseClient.h"
#include "BaseServer.h"
#include "BlueToothClient.h"
#include "BlueToothServer.h"


CBlueToothServer::CBlueToothServer()
{

}

CBlueToothServer::~CBlueToothServer()
{

}

//
// Create server socket
//
SOCKET CBlueToothServer::CreateServerSocket()
{
	SOCKET ServerSocket = WSASocket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM, NULL, 0, WSA_FLAG_OVERLAPPED);
	return ServerSocket;
}


//
// Bind server socket
//
int CBlueToothServer::BindServerSocket()
{
	if( m_ServerSocket == INVALID_SOCKET )
		return SOCKET_ERROR;

	// listen
	SOCKADDR_BTH serverAddr;
	serverAddr.btAddr			= 0;
	serverAddr.port				= BT_PORT_ANY;
	serverAddr.addressFamily	= AF_BTH;
	serverAddr.serviceClassId	= GUID_NULL;

	int ret = ::bind(m_ServerSocket, (sockaddr*)&serverAddr, sizeof(serverAddr) );
	return ret;

}

//
// Set server service
//
GUID PPTServiceClass_UUID = { 0xb62c4e8d, 0x62cc, 0x404b, 0xbb, 0xbf, 0xbf, 0x3e, 0x3b, 0xbb, 0x13, 0x74 }; 

int CBlueToothServer::SetServerService()
{
	if( m_ServerSocket == INVALID_SOCKET )
		return SOCKET_ERROR;

	SOCKADDR_BTH serverAddr;

	int nLen = sizeof(SOCKADDR_BTH);
	int ret = getsockname(m_ServerSocket, (SOCKADDR *)&serverAddr, &nLen);
	if( ret == SOCKET_ERROR )
		return ret;

	CSADDR_INFO sockInfo;
	sockInfo.iProtocol = BTHPROTO_RFCOMM;
	sockInfo.iSocketType = SOCK_STREAM;
	sockInfo.LocalAddr.lpSockaddr = (SOCKADDR *)&serverAddr;
	sockInfo.LocalAddr.iSockaddrLength = sizeof(SOCKADDR_BTH);
	sockInfo.RemoteAddr.lpSockaddr = (SOCKADDR *)&serverAddr;
	sockInfo.RemoteAddr.iSockaddrLength = sizeof(SOCKADDR_BTH);

	WSAQUERYSET svcInfo = { 0 };
	svcInfo.dwSize = sizeof(svcInfo);
	svcInfo.dwNameSpace = NS_BTH;
	svcInfo.lpszServiceInstanceName = _T("PPTShellService");
	svcInfo.lpServiceClassId = (LPGUID) &PPTServiceClass_UUID;
	svcInfo.dwNumberOfCsAddrs = 1;
	svcInfo.lpcsaBuffer = &sockInfo;

	ret = WSASetService(&svcInfo, RNRSERVICE_REGISTER, 0);
	return ret;	

}

//
// Accept connection
//
int CBlueToothServer::AcceptConnection()
{
	if( m_ServerSocket == INVALID_SOCKET )
		return SOCKET_ERROR;

	SOCKADDR_BTH ClientAddr;
	int clientLen = sizeof(ClientAddr); 

	SOCKET ClientSocket = accept(m_ServerSocket, (SOCKADDR*)&ClientAddr, &clientLen);
	if( ClientSocket == INVALID_SOCKET )
		return SOCKET_ERROR;

	// set socket option
	BOOL bDontLinger = FALSE; 
	setsockopt(ClientSocket, SOL_SOCKET, SO_DONTLINGER, (const char*)&bDontLinger, sizeof(BOOL));

	// create one cClientConnection
	CBaseClient* pClientConnection = new CBlueToothClient;
	if( pClientConnection == NULL )
	{
		closesocket(ClientSocket);
		return SOCKET_ERROR;
	}

	pClientConnection->Initialize(ClientSocket, (SOCKADDR*)&ClientAddr);
	pClientConnection->SetRecvCloseNotifyFunc((ON_PACKET_RECV_FUNC)m_pPacketProcessFunc, (ON_SOCKET_CLOSE_FUNC)m_pSocketCloseFunc);

	// add to list
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