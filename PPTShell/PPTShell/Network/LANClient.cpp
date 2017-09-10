//===========================================================================
// FileName:				WifiClient.cpp
// 
// Desc:				
//============================================================================
#include "stdafx.h"
#include "BaseClient.h"
#include "LANClient.h"

CLANClient::CLANClient()
{

}

CLANClient::~CLANClient()
{

}

//
// Initialize
//
BOOL CLANClient::Initialize(SOCKET clientSocket, SOCKADDR* clientAddr)
{
	CBaseClient::Initialize(clientSocket, clientAddr);

	m_nConnectType = CONNECT_TYPE_LAN;
	m_ClientSocket = clientSocket;

	memcpy(&m_ClientAdress, clientAddr, sizeof(SOCKADDR_IN));

	__super::Initialize(clientSocket, clientAddr);
	return TRUE;

}