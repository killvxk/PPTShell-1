//===========================================================================
// FileName:				ClientConnection.h
// 
// Desc:				
//============================================================================
#include "stdafx.h"
#include "BaseClient.h"
#include "Packet.h"
#include "BaseServer.h"

CBaseClient::CBaseClient()
{
	m_nSizeRecv				= 0;
	m_type = EClientType::CTYPE_NONE;
	m_dwUID					= -1;
	m_dwPPTID				= -1;
	m_ClientSocket			= INVALID_SOCKET;
	m_pOppositeClient = nullptr;
	InitializeCriticalSection(&m_Lock);
	m_bIfPCMainClient = FALSE;
	m_bSending = false;
	m_pSendOverlapped = nullptr;
	m_tConnTime = time(nullptr);
}

CBaseClient::~CBaseClient()
{
	DeleteCriticalSection(&m_Lock);
	if (m_pSendOverlapped != nullptr)
	{
		delete m_pSendOverlapped;
		m_pSendOverlapped = nullptr;
	}
	if (m_vecCachedPackets.size()>0)
	{
		for (unsigned int i = 0; i < m_vecCachedPackets.size(); i++)
		{
			delete m_vecCachedPackets[i];
		}
		m_vecCachedPackets.clear();
	}
}



//
// Initialize
//
BOOL CBaseClient::Initialize(SOCKET clientSocket, SOCKADDR* clientAddr)
{
	m_nSizeRecv				= 0;

	m_type = EClientType::CTYPE_NONE;
	m_dwUID					= -1;
	m_dwPPTID				= -1;

	memset(m_szRecvBuf, 0, sizeof(m_szRecvBuf));
	memset(&m_RecvOverlapped, 0, sizeof(WSAOVERLAPPED_EX));

	m_ClientSocket = clientSocket;
	memcpy(&m_ClientAdress, clientAddr, sizeof(SOCKADDR_IN));

	m_pSendOverlapped = new WSAOVERLAPPED_EX;
	m_szSendBuf = new char[MAX_SERVER_PACEKT_SIZE];

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

	WRITE_LOG_SERVER("CBaseClient::OnSocketClosed, socket:%u", m_ClientSocket);

	LINGER lingerStruct;
	lingerStruct.l_onoff = 1;
	lingerStruct.l_linger = 0;
	setsockopt(m_ClientSocket, SOL_SOCKET, SO_LINGER, (char *)&lingerStruct, sizeof(lingerStruct) );

	CancelIo((HANDLE)m_ClientSocket);
	closesocket(m_ClientSocket);

	m_ClientSocket = INVALID_SOCKET;

	if (m_pOppositeClient != nullptr)
	{
		m_pOppositeClient->SetOppositeClient(nullptr);
		CBaseServer::GetInstance()->PostCompletionStopStatus(m_pOppositeClient);
	}

	m_dwUID = -1;
	m_dwPPTID = -1;

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
	return SendPacket((char*)&pkgHead);
}

// 
// send packet
//
BOOL CBaseClient::SendPacket(char *pData)
{
	PACKET_HEAD *pkgHead = (PACKET_HEAD *)pData;
	int nDataSize = pkgHead->dwSize;

	if (nDataSize > MAX_SERVER_PACEKT_SIZE || m_ClientSocket == INVALID_SOCKET)
		return FALSE;

	char* pBuffer = new char[nDataSize];
	memcpy(pBuffer, pData, nDataSize);

	if (m_bSending)
	{
		m_vecCachedPackets.push_back(pBuffer);
		return TRUE;
	}

	return SendCachePacket(pBuffer);
}

BOOL CBaseClient::SendCachePacket(char* pBuffer)
{
	m_bSending = true;

	PACKET_HEAD *pkgHead = (PACKET_HEAD *)pBuffer;
	int nDataSize = pkgHead->dwSize;
	if (nDataSize <= 0 || nDataSize>MAX_SERVER_PACEKT_SIZE)
	{
		delete pBuffer;
		return FALSE;
	}

	memset(m_pSendOverlapped, 0, sizeof(WSAOVERLAPPED_EX));

	memset(m_szSendBuf, 0, sizeof(m_szSendBuf));
	memcpy(m_szSendBuf, pBuffer, nDataSize);
	delete pBuffer;
	pBuffer = nullptr;

	m_pSendOverlapped->op = 0; // send
	m_pSendOverlapped->buffer = m_szSendBuf;
	m_pSendOverlapped->size = nDataSize;
	m_pSendOverlapped->sizeTransferred = 0;
	m_pSendOverlapped->wsaBuf.buf = m_szSendBuf;
	m_pSendOverlapped->wsaBuf.len = nDataSize;

	DWORD numberOfBytesSent = 0;
	int ret = WSASend(m_ClientSocket, &m_pSendOverlapped->wsaBuf, 1, &numberOfBytesSent, 0, m_pSendOverlapped, NULL);
	if (ret == 0)
	{

	}
	else if (ret == SOCKET_ERROR)
	{
		DWORD dwErrCode = WSAGetLastError();
		if (dwErrCode != WSA_IO_PENDING)
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
	m_tLastTransferTime = time(nullptr);
	EnterCriticalSection(&m_Lock);
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
			m_bSending = false;
			if (m_vecCachedPackets.size()>0)
			{
				auto itr = m_vecCachedPackets.begin();
				char *pBuffer = *itr;
				m_vecCachedPackets.erase(itr);
				SendCachePacket(pBuffer);
			}
		}
	}
	// recv completed
	else if( lpOverlapped->op == 1 )
	{
		// check recvd size
		m_nSizeRecv += dwNumberOfBytesTransfered;

		PACKET_HEAD* pPacketHeader = NULL;

		while( m_nSizeRecv >= sizeof(PACKET_HEAD) )
		{
			pPacketHeader = (PACKET_HEAD*)m_szRecvBuf;
			if (pPacketHeader->dwSize<0)
			{
				CBaseServer::GetInstance()->PostCompletionStopStatus(this);
				WRITE_LOG_SERVER("Client Rcv a packet have a nagtive length!! sock:%u, UID:%u, PPTID:%u, len:%d, client type:%d", m_ClientSocket, m_dwUID, m_dwPPTID, pPacketHeader->dwSize, m_type);
				return -1;
			}

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
				ProcessOnePacket(m_szRecvBuf);

				m_nSizeRecv -= pPacketHeader->dwSize;

				if( m_nSizeRecv>0 && m_nSizeRecv<MAX_SERVER_PACEKT_SIZE)
				{ 
					char szBuffer[MAX_SERVER_PACEKT_SIZE] = {0};
					memcpy(szBuffer, m_szRecvBuf + pPacketHeader->dwSize, m_nSizeRecv);
					memcpy(m_szRecvBuf, szBuffer, MAX_SERVER_PACEKT_SIZE);
				}
			} 
		}

		// continue to recv 
		if( m_nSizeRecv > 0 )
			RecvPacketRemainPart();
		else
			RecvPacket();
	}

	LeaveCriticalSection(&m_Lock);
	return TRUE;
}


//
// Process one packet
//
int CBaseClient::ProcessOnePacket(char* pPacket)
{
	PACKET_HEAD* pHeader = (PACKET_HEAD*)pPacket;

	// check size
	if( pHeader->dwSize > MAX_PACKET_SIZE )
	{
		if (m_type == EClientType::CTYPE_MOBILE)
		{
			WRITE_LOG_SERVER("[M->P]: packet size error size = %d, type = %u", pHeader->dwSize, pHeader->dwMajorType);
		}
		else if (m_type == EClientType::CTYPE_PPTSHELL)
		{
			WRITE_LOG_SERVER("[P->M]: packet size error size = %d, type = %u", pHeader->dwSize, pHeader->dwMajorType);
		}

		return -1;
	}

	
	if( pHeader->dwMajorType == PT_REGISTER )
	{
		PACKET_REGISTER* pRegisterPacket = (PACKET_REGISTER*)pHeader;
		
		int iType = pRegisterPacket->dwClientType;
		m_type = (EClientType)iType;
		m_dwUID = pRegisterPacket->dwMobileID;
		m_dwPPTID = pRegisterPacket->dwPPTShellID;

		CBaseServer *pServer = CBaseServer::GetInstance();
		if (pServer == nullptr)
		{
			WRITE_LOG_SERVER("CBaseClient::ProcessOnePacket, get baseserver instance fail!");
			return -1;
		}
		
		if (!pServer->RegistClient(m_type, this))
		{
			pServer->PostCompletionStopStatus(this);
			WRITE_LOG_SERVER("CBaseClient::ProcessOnePacket, RegistClient fail!")
			return -1;
		}
		

		if (m_type == EClientType::CTYPE_MOBILE)
		{
			if (m_pOppositeClient == nullptr)
			{
				CBaseClient *pMainClient = pServer->GetPCMainClientByPPTID(m_dwPPTID);
				if (pMainClient != nullptr)
				{
					// notify main PPTShell to create a new connection for this mobile
					pMainClient->SendPacket(pPacket);

					WRITE_LOG_SERVER("[Register]: type: mobile | socket:%u| UID: %u | PPTID: %u", m_ClientSocket, m_dwUID, m_dwPPTID);
				}
				else
				{
					WRITE_LOG_SERVER("[PPTShell Not Found]:socket:%u, UID: %u, PPTID: %u", m_ClientSocket, m_dwUID, m_dwPPTID);
					pServer->PostCompletionStopStatus(this);
				}
			}
		}
		else if (m_type == EClientType::CTYPE_PPTSHELL)
		{
			// send all cached packets for this mobile
			if (m_dwUID > 0)
			{
				vector<char*> vecCaches;
				pServer->GetCachedPacketByUID(m_dwUID, vecCaches);
				for (unsigned int i = 0; i<vecCaches.size(); i++)
				{
					SendPacket(vecCaches[i]);
				}
				WRITE_LOG_SERVER("Socket:%u, Send cached packets to pc[PPTID:%u], count:%d", m_ClientSocket, m_dwPPTID, vecCaches.size());
			}
		}

	}
	else if(pHeader->dwMajorType == PT_HEARTBEAT_RQST)		//心跳包，发应答
	{
//		WRITE_LOG_SERVER("Socket:%u, Receive heartbeat request! UID:%u, PPTID:%u", m_ClientSocket, m_dwUID, m_dwPPTID);
		SendHeartBeatAck();
	}
	else
	{ 
		// transfer to PPT
		if( m_type == EClientType::CTYPE_MOBILE )
		{
			if (m_pOppositeClient == nullptr)
			{
				CBaseServer *pServer = CBaseServer::GetInstance();
				if (pServer != nullptr)
				{
					pServer->AddCachedPacket(m_dwUID, pPacket);
				}
			}
			else
			{
				BOOL res = m_pOppositeClient->SendPacket(pPacket);
				if (!res)
				{
					DWORD dwErrCode = WSAGetLastError();
					WRITE_LOG_SERVER("[M:%u->P:%u]: send packet failed: type: %u, UID:%u, PPTID:%u, errCode: %d", 
							m_ClientSocket, m_pOppositeClient->GetSocket(), pHeader->dwMajorType, m_dwUID, m_dwPPTID, dwErrCode);
				}
			}
		}
		// transfer to Mobile
		else if(m_type == EClientType::CTYPE_PPTSHELL )
		{
			if (m_pOppositeClient == nullptr)
			{
				CBaseServer *pServer = CBaseServer::GetInstance();
				if (pServer != nullptr)
				{
					pServer->PostCompletionStopStatus(this);
				}
				WRITE_LOG_SERVER("Transfer data from pptshell[sock:%u, pptid:%u] to mobile[uid:%u], found mobile is null! It's impossible to get here!", m_ClientSocket, m_dwPPTID, m_dwUID);
				return -1;
			}
			else
			{
				BOOL res = m_pOppositeClient->SendPacket((char*)pHeader);
				if (!res)
				{
					DWORD dwErrCode = WSAGetLastError();
					WRITE_LOG_SERVER("[P:%u->M:%u]: type: %u, UID: %u, PPTID:%u, size: %d errCode: %d, send packet failed", 
							m_ClientSocket, m_pOppositeClient->GetSocket(), pHeader->dwMajorType, m_dwUID, m_dwPPTID, pHeader->dwSize, dwErrCode);
					return -1;
				}
			}
		}
	}

	return pHeader->dwSize;
}

