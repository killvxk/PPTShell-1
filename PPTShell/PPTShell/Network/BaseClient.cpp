//===========================================================================
// FileName:				ClientConnection.h
// 
// Desc:				
//============================================================================
#include "stdafx.h"
#include "BaseClient.h"
#include "PacketProcess/Packet.h"

CBaseClient::CBaseClient()
{
	m_nSizeRecv				= 0;
	m_pPacketRecvFunc		= NULL;
	m_pPacketSentFunc		= NULL;
	m_pSocketCloseFunc		= NULL;

	m_ClientSocket			= NULL;
	m_nConnectType			= CONNECT_TYPE_NONE;

	memset(&m_RecvOverlapped, 0, sizeof(m_RecvOverlapped));
	memset(m_szRecvBuf, 0, sizeof(m_szRecvBuf));

	InitializeCriticalSection(&m_Lock);
		
}

CBaseClient::~CBaseClient()
{
	DeleteCriticalSection(&m_Lock);
}


//
// Initialize
//
BOOL CBaseClient::Initialize(SOCKET clientSocket, SOCKADDR *clientAddr)
{
	
	return TRUE;
}


//
// socket closed
//
BOOL CBaseClient::OnSocketClosed()
{
	EnterCriticalSection(&m_Lock);

	LINGER lingerStruct;
	lingerStruct.l_onoff = 1;
	lingerStruct.l_linger = 0;
	setsockopt(m_ClientSocket, SOL_SOCKET, SO_LINGER, (char *)&lingerStruct, sizeof(lingerStruct) );

	CancelIo((HANDLE)m_ClientSocket);
	closesocket(m_ClientSocket);
	m_ClientSocket = INVALID_SOCKET;

	LeaveCriticalSection(&m_Lock);

	// push back packet to process deque
	if( m_pSocketCloseFunc != NULL )
		m_pSocketCloseFunc(m_ClientSocket);

	return TRUE;
}

// 
// send packet
//
BOOL CBaseClient::SendPacket(char *pData, int nDataSize)
{
	// allocate buffer
	char* pBuffer = new char[nDataSize];
	memcpy(pBuffer, pData, nDataSize);

	WSAOVERLAPPED_EX* ol = new WSAOVERLAPPED_EX;
	memset(ol, 0, sizeof(WSAOVERLAPPED_EX));

	ol->op				= 0; // send
	ol->buffer			= pBuffer;
	ol->size			= nDataSize;
	ol->sizeTransferred	= 0;

	WSABUF wsaBuf;
	wsaBuf.buf = pBuffer;
	wsaBuf.len = nDataSize;
	
	DWORD numberOfBytesSent = 0;
	int ret = WSASend(m_ClientSocket, &wsaBuf, 1, &numberOfBytesSent, 0, ol, NULL);
	if( ret == 0 )
	{

	}
	else if( ret == SOCKET_ERROR )
	{
		DWORD dwErrCode = WSAGetLastError();
		if( dwErrCode != WSA_IO_PENDING )
			return FALSE;
	}
	 
	return TRUE;
}


//
// recv packet
//
BOOL CBaseClient::RecvPacket()
{
	DWORD NumberOfBytesRecvd = 0;
	m_nSizeRecv				 = 0;
	DWORD Flags				 = 0;

	memset(m_szRecvBuf, 0, sizeof(m_szRecvBuf));
	memset(&m_RecvOverlapped, 0, sizeof(WSAOVERLAPPED_EX));

	m_wsaRecvBuf.buf = m_szRecvBuf;
	m_wsaRecvBuf.len = sizeof(m_szRecvBuf);

	m_RecvOverlapped.op = 1; // recv operation

	int res= WSARecv(m_ClientSocket, &m_wsaRecvBuf, 1, &NumberOfBytesRecvd, &Flags, &m_RecvOverlapped,  NULL);
	if( res == 0 )  // recv data immediately
	{

	}
	else if( res == SOCKET_ERROR )
	{
		DWORD errCode = WSAGetLastError();
		if( errCode != WSA_IO_PENDING )
			return FALSE;
	}

	return TRUE;
}

//
// recv packet
//
BOOL CBaseClient::RecvPacketRemainPart()
{
	DWORD NumberOfBytesRecvd = 0;
	DWORD Flags				 = 0;

	m_wsaRecvBuf.buf = m_szRecvBuf+m_nSizeRecv;
	m_wsaRecvBuf.len = sizeof(m_szRecvBuf) - m_nSizeRecv;

	m_RecvOverlapped.op = 1; // recv operation

	int res= WSARecv(m_ClientSocket, &m_wsaRecvBuf, 1, &NumberOfBytesRecvd, &Flags, &m_RecvOverlapped,  NULL);
	if( res == 0 )  // recv data immediately
	{

	}
	else if( res == SOCKET_ERROR )
	{
		DWORD errCode = WSAGetLastError();
		if( errCode != WSA_IO_PENDING )
			return FALSE;
		
	}

	return TRUE;
}


//
// data transfered
//
BOOL CBaseClient::OnDataTransfered(DWORD dwNumberOfBytesTransfered, WSAOVERLAPPED_EX* lpOverlapped)
{
	// send completed
	if( lpOverlapped->op == 0 )
	{
		lpOverlapped->sizeTransferred += dwNumberOfBytesTransfered;

		// need to post send request again
		if( lpOverlapped->sizeTransferred < lpOverlapped->size )
		{
			WSABUF wsaBuf;
			wsaBuf.buf = lpOverlapped->buffer+lpOverlapped->sizeTransferred;
			wsaBuf.len = lpOverlapped->size-lpOverlapped->sizeTransferred;

			DWORD numberOfBytesSent = 0;
			int ret = WSASend(m_ClientSocket, &wsaBuf, 1, &numberOfBytesSent, 0, lpOverlapped, NULL);
			if( ret == 0 )
			{

			}
			else if( ret == SOCKET_ERROR )
			{
				DWORD dwErrCode = WSAGetLastError();
				if( dwErrCode != WSA_IO_PENDING )
					return FALSE;
			}
		}
		else
		{
			// send done
			if( lpOverlapped->buffer != NULL )
			{
				delete lpOverlapped->buffer;
				lpOverlapped->buffer = NULL;
			}

			if( lpOverlapped != NULL )
			{
				delete lpOverlapped;
				lpOverlapped = NULL;
			}	

			// notify caller
			if( m_pPacketSentFunc != NULL )
				m_pPacketSentFunc(m_ClientSocket, dwNumberOfBytesTransfered);

		}
	}
	// recv completed
	else if( lpOverlapped->op == 1 )
	{
		EnterCriticalSection(&m_Lock);

		// check recvd size
		m_nSizeRecv += dwNumberOfBytesTransfered;

		PACKET_HEAD* pPacketHeader = NULL;

		while( m_nSizeRecv >= sizeof(PACKET_HEAD) )
		{
			pPacketHeader = (PACKET_HEAD*)m_szRecvBuf;
			if( m_nSizeRecv < (int)pPacketHeader->dwSize )
			{
				if( pPacketHeader->dwSize > MAX_SERVER_PACEKT_SIZE )
				{
					// wrong packet
					memset(m_szRecvBuf, 0, MAX_SERVER_PACEKT_SIZE);
					m_nSizeRecv = 0;
				}

				break;  // continue to recv
			}
			else
			{
				// we can process one packet
				int nPacketSize = ProcessOnePacket();
				if( nPacketSize <= 0 )
				{
					LeaveCriticalSection(&m_Lock);
					return FALSE;			// packet error then close this connection
				}

				m_nSizeRecv -= nPacketSize;

				if( m_nSizeRecv > 0 )
				{ 
					char szBuffer[MAX_SERVER_PACEKT_SIZE] = {0};
					memcpy(szBuffer, m_szRecvBuf+nPacketSize, m_nSizeRecv);
					memcpy(m_szRecvBuf, szBuffer, MAX_SERVER_PACEKT_SIZE);    //m_nRecvdDataSize //zcs 2015-04-21
				}
			} 
		}

		// need to post recv request again
		if( m_nSizeRecv > 0 )
			RecvPacketRemainPart();
		else
			RecvPacket();

		LeaveCriticalSection(&m_Lock);
	}

	
	return TRUE;
}


//
// Process one packet
//
int CBaseClient::ProcessOnePacket()
{
	PACKET_HEAD* pPacketHeader = (PACKET_HEAD*)m_szRecvBuf;

	// push back packet to process deque
	if( m_pPacketRecvFunc != NULL )
		m_pPacketRecvFunc(m_ClientSocket, m_nConnectType, m_szRecvBuf);

	return pPacketHeader->dwSize;
}

//
// Set packet recv and socket close notify function
//
BOOL CBaseClient::SetRecvCloseNotifyFunc(ON_PACKET_RECV_FUNC pPacketRecvFunc, ON_SOCKET_CLOSE_FUNC pSocketCloseFunc)
{
	m_pPacketRecvFunc  = pPacketRecvFunc;
	m_pSocketCloseFunc = pSocketCloseFunc;
	return TRUE;
}

//
// Set packet sent notify function
//
BOOL CBaseClient::SetPacketSentNotifyFunc(ON_PACKET_SENT_FUNC pPacketSentFunc)
{
	m_pPacketSentFunc = pPacketSentFunc;
	return TRUE;
}