//===========================================================================
// FileName:				WANClient.h
// 
// Desc:				
//============================================================================
#include "stdafx.h"
#include "PacketProcess/Packet.h"
#include "BaseClient.h"
#include "WANClient.h"
#include "WANThread.h"


CWANClient::CWANClient()
{
	m_ClientSocket			= INVALID_SOCKET;
	m_nRecvdDataSize		= 0;
	m_nConnectType			= CONNECT_TYPE_WAN;
	m_bHeartbeatRqst		= false;
	m_iHeartbeatTryCnt = 0;
	m_iThreadDestroyCnt = 0;
	m_pWanThread = NULL;
}

CWANClient::~CWANClient()
{
	DeleteCriticalSection(&m_Lock);
	if (m_SendQueue.size()>0)
	{
		deque<QUEUE_PACKET*>::iterator itr;
		for(itr = m_SendQueue.begin(); itr != m_SendQueue.end(); itr++)
		{
			delete (*itr);
			*itr = NULL;
		}
		m_SendQueue.clear();
	}

	//TerminateThread(m_hSendThread, 0);
	//TerminateThread(m_hRecvThread, 0);
}

BOOL CWANClient::Initialize()
{
	InitializeCriticalSection(&m_Lock);
	m_SendQueue.clear();

	// create events
	//m_hRecvEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	//m_hSendEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	// create a thread to process packet
	//m_hRecvThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecvThread, this, 0, NULL);
	//m_hSendThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SendThread, this, 0, NULL);

	m_nRecvdDataSize = 0;
	memset(m_szRecvBuffer, 0, sizeof(m_szRecvBuffer));

	return TRUE;
}

BOOL CWANClient::Destroy()
{
	//m_iThreadDestroyCnt = 2;
	//SetEvent(m_hRecvEvent);
	//SetEvent(m_hSendEvent);
	return TRUE;
}

//
// Set packet process function
//
BOOL CWANClient::SetRecvCloseNotifyFunc(ON_PACKET_RECV_FUNC pPacketProcessFunc, ON_SOCKET_CLOSE_FUNC pSocketCloseFunc)
{
	m_pPacketProcessFunc = pPacketProcessFunc;
	m_pSocketCloseFunc = pSocketCloseFunc;
	return TRUE;
}

//
// connect to WAN server
//
BOOL CWANClient::Connect(char* szServerIP, int nServerPort)
{
	//m_ClientSocket = socket(AF_INET, SOCK_STREAM, 0);
	m_ClientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);// 使用异步SOCKET
	if( m_ClientSocket == INVALID_SOCKET )
		return FALSE;

	// connect to WAN server
	SOCKADDR_IN ServerAddress;
	ServerAddress.sin_family					= AF_INET;
	ServerAddress.sin_port						= htons(nServerPort);
	ServerAddress.sin_addr.S_un.S_addr			= inet_addr(szServerIP);

	int ret = connect(m_ClientSocket, (SOCKADDR*)&ServerAddress, sizeof(SOCKADDR_IN));
	if( ret == SOCKET_ERROR )
	{
		closesocket(m_ClientSocket);
		m_ClientSocket = INVALID_SOCKET;
		return FALSE;
	}

	// select
	//ret = WSAEventSelect(m_ClientSocket, m_hRecvEvent, FD_READ | FD_CLOSE);
	//if( ret == SOCKET_ERROR )
	//{
	//	closesocket(m_ClientSocket);
	//	m_ClientSocket = INVALID_SOCKET;
	//	return FALSE;
	//}

	m_bHeartbeatRqst = false;
	m_iHeartbeatTryCnt = 0;
	return TRUE;
}

void CWANClient::CloseSocket()
{
	EnterCriticalSection(&m_Lock);
	if (m_ClientSocket == INVALID_SOCKET)
	{
		LeaveCriticalSection(&m_Lock);
		return;
	}

	LINGER lingerStruct;
	lingerStruct.l_onoff = 1;
	lingerStruct.l_linger = 0;
	setsockopt(m_ClientSocket, SOL_SOCKET, SO_LINGER, (char *)&lingerStruct, sizeof(lingerStruct) );

	CancelIo((HANDLE)m_ClientSocket);
	closesocket(m_ClientSocket);
	SOCKET sockThis = m_ClientSocket;
	m_ClientSocket = INVALID_SOCKET;
	LeaveCriticalSection(&m_Lock);

	if( m_pSocketCloseFunc != NULL )
		m_pSocketCloseFunc(sockThis);
}

DWORD WINAPI CWANClient::RecvThread(LPARAM lParam)
{
	CWANClient* pThis = (CWANClient*)lParam;
	pThis->ProcessRecvEvent();

	if (pThis->ThreadExit()==0)
	{
		delete pThis;
		pThis = NULL;
	}

	return 0;
}

void CWANClient::ProcessRecvEvent()
{
	while(TRUE)
	{
		DWORD res = WaitForSingleObject(m_hRecvEvent, INFINITE);
		if( res != 0 )
			continue;

		if (m_iThreadDestroyCnt>0)
		{
			break;
		}

		// enum network events
		WSANETWORKEVENTS networkEvents;
		int ret = WSAEnumNetworkEvents(m_ClientSocket, NULL, &networkEvents);
		if( ret == SOCKET_ERROR )
			break;

		// 不能调用这行代码！！！
		//WSAResetEvent(m_hRecvEvent);

		if( networkEvents.lNetworkEvents & FD_READ )
		{
			if( networkEvents.iErrorCode[FD_READ_BIT] != 0 )
				break;

			OnDataRecvd();
		}
		else if( networkEvents.lNetworkEvents & FD_CLOSE )
		{
			if( m_pSocketCloseFunc != NULL )
				m_pSocketCloseFunc(m_ClientSocket);
		}
	}
}

BOOL CWANClient::OnDataRecvd()
{
	// 
	// recv MAX_SERVER_PACEKT_SIZE size
	//
	int nSizeToRecv = MAX_CLIENT_PACEKT_SIZE - m_nRecvdDataSize;
	int nSize = recv(m_ClientSocket, m_szRecvBuffer+m_nRecvdDataSize, nSizeToRecv, 0);

	if( nSize == SOCKET_ERROR )
	{
		DWORD dwErrorCode = WSAGetLastError();
		if( dwErrorCode == WSAEWOULDBLOCK )
			return TRUE;
		else
			return FALSE;  // remote close the connection and we will receive a FD_CLOSE notify

	}
	else if( nSize == 0 )
	{
		return FALSE;
	}

	m_nRecvdDataSize += nSize;

	//
	// check recvd size
	//
	PACKET_HEAD* pPacketHeader = NULL;

	while( m_nRecvdDataSize >= sizeof(PACKET_HEAD) )
	{
		pPacketHeader = (PACKET_HEAD*)m_szRecvBuffer;
		if( m_nRecvdDataSize < (int)pPacketHeader->dwSize )
		{
			if( pPacketHeader->dwSize > MAX_CLIENT_PACEKT_SIZE )
			{
				// wrong packet
				memset(m_szRecvBuffer, 0, MAX_CLIENT_PACEKT_SIZE);
				m_nRecvdDataSize = 0;
			}

			break;  // continue to recv
		}
		else
		{
			//心跳应答包处理
			if (m_bHeartbeatRqst && pPacketHeader->dwMajorType == PT_HEARTBEAT_ACK)
			{
				m_bHeartbeatRqst = false;
				m_iHeartbeatTryCnt = 0;
				WRITE_LOG_LOCAL("receive heartbeat ack!! socket:%d", m_ClientSocket);
				break;
			}

			// we can process one packet
			int nPacketSize = ProcessPacket();
			if( nPacketSize <= 0 )
				return FALSE;			// packet error then close this connection
			

			m_nRecvdDataSize -= nPacketSize;

			if( m_nRecvdDataSize > 0 )
			{ 
				char szBuffer[MAX_CLIENT_PACEKT_SIZE] = {0};
				memcpy(szBuffer, m_szRecvBuffer+nPacketSize, m_nRecvdDataSize);
				memcpy(m_szRecvBuffer, szBuffer, MAX_CLIENT_PACEKT_SIZE);    //m_nRecvDataSize //zcs 2015-04-21
			}
			else if( m_nRecvdDataSize < 0 )
			{
			
			}

		} 
	}

	return TRUE;
}

BOOL CWANClient::DealRecvData()
{
	PACKET_HEAD* pPacketHeader = NULL;

	while( m_nRecvdDataSize >= sizeof(PACKET_HEAD) )
	{
		pPacketHeader = (PACKET_HEAD*)m_szRecvBuffer;
		if( m_nRecvdDataSize < (int)pPacketHeader->dwSize )
		{
			if( pPacketHeader->dwSize > MAX_CLIENT_PACEKT_SIZE )
			{
				// wrong packet
				memset(m_szRecvBuffer, 0, MAX_CLIENT_PACEKT_SIZE);
				m_nRecvdDataSize = 0;
			}

			break;  // continue to recv
		}
		else
		{
			//心跳应答包处理
			if (m_bHeartbeatRqst && pPacketHeader->dwMajorType == PT_HEARTBEAT_ACK)
			{
				m_bHeartbeatRqst = false;
				m_iHeartbeatTryCnt = 0;
				WRITE_LOG_LOCAL("receive heartbeat ack!! socket:%d", m_ClientSocket);
				break;
			}

			// we can process one packet
			int nPacketSize = ProcessPacket();
			if( nPacketSize <= 0 )
				return FALSE;			// packet error then close this connection


			m_nRecvdDataSize -= nPacketSize;

			if( m_nRecvdDataSize > 0 )
			{ 
				char szBuffer[MAX_CLIENT_PACEKT_SIZE] = {0};
				memcpy(szBuffer, m_szRecvBuffer+nPacketSize, m_nRecvdDataSize);
				memcpy(m_szRecvBuffer, szBuffer, MAX_CLIENT_PACEKT_SIZE);    //m_nRecvDataSize //zcs 2015-04-21
			}
			else if( m_nRecvdDataSize < 0 )
			{

			}

		} 
	}

	return TRUE;
}

BOOL CWANClient::SendHeartBeat()
{
	if( m_ClientSocket == INVALID_SOCKET )
		return FALSE;

	PACKET_HEAD pkgHead;
	memset(&pkgHead, 0, sizeof(PACKET_HEAD));
	pkgHead.dwMajorType		= PT_HEARTBEAT_RQST;
	pkgHead.dwMinorType		= 0;
	pkgHead.dwSize			= sizeof(PACKET_HEAD);
	m_iHeartbeatTryCnt ++;
	m_bHeartbeatRqst = true;
	WRITE_LOG_LOCAL("Send Heartbeat to socket:%d, try cnt:%d", m_ClientSocket, m_iHeartbeatTryCnt);
	//return SendPacket((char*)&pkgHead, sizeof(PACKET_HEAD));
	return m_pWanThread->PostSend(this, (char*)&pkgHead, sizeof(PACKET_HEAD));
}

bool CWANClient::IfRcvHeartBeatAck()
{
	return !m_bHeartbeatRqst;
}


// 
// process packet
//
int CWANClient::ProcessPacket()
{
	PACKET_HEAD* pPacketHeader = (PACKET_HEAD*)m_szRecvBuffer;
	
	if( m_pPacketProcessFunc != NULL )
		m_pPacketProcessFunc(m_ClientSocket, m_nConnectType, m_szRecvBuffer);

	return pPacketHeader->dwSize;
}

int CWANClient::ThreadExit()
{
	EnterCriticalSection(&m_Lock);
	m_iThreadDestroyCnt--;
	int iCnt = m_iThreadDestroyCnt;
	LeaveCriticalSection(&m_Lock);
	return iCnt;
}
//
// send thread
//
DWORD WINAPI CWANClient::SendThread(LPARAM lParam)
{
	CWANClient* pThis = (CWANClient*)lParam;
	pThis->ProcessSendEvent();

	if (pThis->ThreadExit()==0)
	{
		delete pThis;
		pThis = NULL;
	}

	return 0;
}

void CWANClient::ProcessSendEvent()
{
	while(TRUE)
	{
		DWORD res = WaitForSingleObject(m_hSendEvent, INFINITE);
		if( res == WAIT_FAILED )
			continue;
		
		if (m_iThreadDestroyCnt>0)
		{
			break;
		}

		deque<QUEUE_PACKET*> SendQueue;
		
		EnterCriticalSection(&m_Lock);
		SendQueue.swap(m_SendQueue);
		LeaveCriticalSection(&m_Lock);

		deque<QUEUE_PACKET*>::iterator itr;
		for(itr = SendQueue.begin(); itr != SendQueue.end(); itr++)
		{
			QUEUE_PACKET* p = *itr;
			if( p == NULL )
				continue;

			SendData(p->szData, p->dwSize);

			delete p;
			p = NULL;
		}

	}
}

//
// send packet
//
BOOL CWANClient::SendPacket(char *pData, int nDataSize)
{
	if( m_ClientSocket == INVALID_SOCKET )
		return FALSE;

	if( nDataSize > MAX_CLIENT_PACEKT_SIZE )
		return FALSE;

	QUEUE_PACKET* p = new QUEUE_PACKET;
	if( p == NULL )
		return FALSE;

	p->dwSize = nDataSize;
	memcpy(p->szData, pData, nDataSize);

	EnterCriticalSection(&m_Lock);
	m_SendQueue.push_back(p);
	LeaveCriticalSection(&m_Lock);

	SetEvent(m_hSendEvent);
	return TRUE;
}


//
// send register packet
//
BOOL CWANClient::SendRegisterPacket(DWORD dwMobileID, DWORD dwPPTShellID)
{
	if( m_ClientSocket == INVALID_SOCKET )
		return FALSE;

	PACKET_REGISTER RegisterPacket;
	memset(&RegisterPacket, 0, sizeof(PACKET_REGISTER));

	RegisterPacket.dwMajorType		= PT_REGISTER;
	RegisterPacket.dwMinorType		= 0;
	RegisterPacket.dwSize			= sizeof(PACKET_REGISTER);

	RegisterPacket.dwClientType		= WAN_PPTSHELL;
	RegisterPacket.dwMobileID		= dwMobileID;
	RegisterPacket.dwPPTShellID		= dwPPTShellID;

	m_dUID = dwMobileID;
			
	//return SendPacket((char*)&RegisterPacket, sizeof(PACKET_REGISTER));
	return m_pWanThread->PostSend(this, (char*)&RegisterPacket, sizeof(PACKET_REGISTER));
}

//
// send data
//
BOOL CWANClient::SendData(char* pData, int nSize)
{
	int totalLen = nSize;
	int curLen = 0;

	while( curLen < totalLen )
	{
		int len = send(m_ClientSocket, pData+curLen, totalLen-curLen, 0);	
		if( len == 0 ) // close
		{
			WRITE_LOG_LOCAL("CWANClient::SendData, send 0 byte!");
			return FALSE;
		}
		else if( len == SOCKET_ERROR )
		{
			WRITE_LOG_LOCAL(_T("CWANClient::SendData, send return -1!, totalLen=%d, curLen=%d\n"), totalLen, curLen);

			DWORD dwErrorCode = GetLastError();
			if( dwErrorCode == WSAEWOULDBLOCK ) // block
			{
				Sleep(10);
				continue;
			}
			else
			{
				WRITE_LOG_LOCAL("CWANClient::SendData, get a SOCKET_ERROR, errno:%d!", dwErrorCode);
				return FALSE;	// close
			}
		}

		curLen += len;
	}

	return TRUE;
}

//
// get socket
//
SOCKET CWANClient::GetSocket()
{
	return m_ClientSocket;
}
