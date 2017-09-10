//===========================================================================
// FileName:				ClientConnection.h
// 
// Desc:				
//============================================================================
#include "stdafx.h"
#include "BaseClient.h"
#include "Packet.h"

CBaseClient::CBaseClient()
{
	m_nSizeRecv				= 0;
	m_pPacketProcessFunc	= NULL;
	m_pSocketCloseFunc		= NULL;
	m_nType					= CTYPE_NONE;
	m_dwUID					= 0;
	m_dwPPTID				= 0;
	m_ClientSocket			= INVALID_SOCKET;
	 
	InitializeCriticalSection(&m_Lock);
	
}

CBaseClient::~CBaseClient()
{
	DeleteCriticalSection(&m_Lock);
	
}



//
// Initialize
//
BOOL CBaseClient::Initialize(SOCKET clientSocket, SOCKADDR* clientAddr)
{
	m_nSizeRecv				= 0;
	m_pPacketProcessFunc	= NULL;
	m_pSocketCloseFunc		= NULL;

	m_nType					= CTYPE_NONE;
	m_dwUID					= 0;
	m_dwPPTID				= 0;

	memset(m_szRecvBuf, 0, sizeof(m_szRecvBuf));
	memset(&m_RecvOverlapped, 0, sizeof(WSAOVERLAPPED_EX));

	m_ClientSocket = clientSocket;
	memcpy(&m_ClientAdress, clientAddr, sizeof(SOCKADDR_IN));

	return TRUE;

}

//
// close socket
//
BOOL CBaseClient::CloseSocket()
{
	// already closed
	if( m_ClientSocket == INVALID_SOCKET )
		return FALSE;

	EnterCriticalSection(&m_Lock);

	LINGER lingerStruct;
	lingerStruct.l_onoff = 1;
	lingerStruct.l_linger = 0;
	setsockopt(m_ClientSocket, SOL_SOCKET, SO_LINGER, (char *)&lingerStruct, sizeof(lingerStruct) );

	CancelIo((HANDLE)m_ClientSocket);
	closesocket(m_ClientSocket);

	m_ClientSocket = INVALID_SOCKET; 
	LeaveCriticalSection(&m_Lock);

	return TRUE;
}


//
// socket closed
//
BOOL CBaseClient::OnSocketClosed()
{
	EnterCriticalSection(&m_Lock);

	// already closed
	if( m_ClientSocket == INVALID_SOCKET )
	{
		LeaveCriticalSection(&m_Lock);
		return FALSE;
	}

	LINGER lingerStruct;
	lingerStruct.l_onoff = 1;
	lingerStruct.l_linger = 0;
	setsockopt(m_ClientSocket, SOL_SOCKET, SO_LINGER, (char *)&lingerStruct, sizeof(lingerStruct) );

	CancelIo((HANDLE)m_ClientSocket);
	closesocket(m_ClientSocket);

	m_ClientSocket = INVALID_SOCKET;

	// push back packet to process deque
	if( m_pSocketCloseFunc != NULL )
		m_pSocketCloseFunc(this);

	LeaveCriticalSection(&m_Lock);

	return TRUE;
}

BOOL CBaseClient::SendHeartBeatAck()
{
	if( m_ClientSocket == INVALID_SOCKET )
		return FALSE;

	PACKET_HEAD pkgHead;
	memset(&pkgHead, 0, sizeof(PACKET_HEAD));
	pkgHead.dwMajorType		= PT_HEARTBEAT_ACK;
	pkgHead.dwMinorType		= 0;
	pkgHead.dwSize			= sizeof(PACKET_HEAD);
	return SendPacket((char*)&pkgHead, sizeof(PACKET_HEAD));
}

// 
// send packet
//
BOOL CBaseClient::SendPacket(char *pData, int nDataSize)
{
	if( nDataSize > 0xFFFF || m_ClientSocket == INVALID_SOCKET )
		return FALSE;

	// allocate buffer
	char* pBuffer = new char[nDataSize];
	memcpy(pBuffer, pData, nDataSize);

	WSAOVERLAPPED_EX* ol = new WSAOVERLAPPED_EX;
	memset(ol, 0, sizeof(WSAOVERLAPPED_EX));

	ol->op				= 0; // send
	ol->buffer			= pBuffer;
	ol->size			= nDataSize;
	ol->sizeTransferred	= 0;
	ol->wsaBuf.buf		= pBuffer;
	ol->wsaBuf.len		= nDataSize;
	
	DWORD numberOfBytesSent = 0;
	int ret = WSASend(m_ClientSocket, &ol->wsaBuf, 1, &numberOfBytesSent, 0, ol, NULL);
	if( ret == 0 )
	{
		
	}
	else if( ret == SOCKET_ERROR )
	{
		DWORD dwErrCode = WSAGetLastError();
		if( dwErrCode != WSA_IO_PENDING )
		{
			//WRITE_LOG_SERVER("Send packet failed!");
			return FALSE;
		}
	}
	 
	return TRUE;
}


//
// recv packet
//这里一定要加一个说明，WSARecv其实是加一个m_ClientSocket的接收监听，并且把缓存地址给完成端口，
//完成端口收到数据会通过GetQueuedCompletionStatus这个接口通知用户说有消息收到，
//然后用户就可以直接处理缓存中收到的数据，处理完后再调用WSARecv监听下一个包
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
		{
			WRITE_LOG_SERVER("Recv packet failed!");
			return FALSE;
		}
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
		{
			WRITE_LOG_SERVER("Recv packet remain part failed!");
			return FALSE;
		}
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
				{
					return FALSE;
				}
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
			if( m_nSizeRecv < pPacketHeader->dwSize )
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
				int nPacketSize = ProcessOnePacket(m_szRecvBuf);
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
					memcpy(m_szRecvBuf, szBuffer, MAX_SERVER_PACEKT_SIZE);    
				}
			} 
		}

		// continue to recv 
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
int CBaseClient::ProcessOnePacket(char* pPacket)
{
	PACKET_HEAD* pPacketHeader = (PACKET_HEAD*)pPacket;

	// push back packet to process deque
	if( m_pPacketProcessFunc != NULL )
		m_pPacketProcessFunc(this, pPacket);

	return pPacketHeader->dwSize;
}

//
// Set packet process function
//
BOOL CBaseClient::SetSocketNotifyFunc(ON_PACKET_FUNC pPacketProcessFunc, ON_SOCKET_CLOSE_FUNC pSocketCloseFunc)
{
	m_pPacketProcessFunc = pPacketProcessFunc;
	m_pSocketCloseFunc = pSocketCloseFunc;
	return TRUE;
}