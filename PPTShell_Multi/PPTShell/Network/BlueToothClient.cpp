//===========================================================================
// FileName:				BlueToothClient.cpp
// 
// Desc:				
//============================================================================
#include "stdafx.h"
#include <ws2bth.h>  
#include "BaseClient.h"
#include "BlueToothClient.h"

CBlueToothClient::CBlueToothClient()
{
	memset(&m_ClientAdress, 0, sizeof(m_ClientAdress));
}

CBlueToothClient::~CBlueToothClient()
{

}

//
// Initialize
//
BOOL CBlueToothClient::Initialize(SOCKET clientSocket, SOCKADDR* clientAddr)
{
	m_nConnectType = CONNECT_TYPE_BLUETOOTH;
	m_ClientSocket = clientSocket;

	memcpy(&m_ClientAdress, clientAddr, sizeof(SOCKADDR_BTH));

	__super::Initialize(clientSocket, clientAddr);
	return TRUE;

}