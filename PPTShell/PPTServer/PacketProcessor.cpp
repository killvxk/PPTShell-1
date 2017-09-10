//===========================================================================
// FileName:				PacketProcessor.cpp
//	
// Desc:					 process packets
//============================================================================
#include "stdafx.h"
#include "Packet.h"
#include "BaseClient.h"
#include "PacketProcessor.h"

typedef multimap<CBaseClient*, CBaseClient*>::iterator ITER_MOBI;
typedef multimap<DWORD, CBaseClient*>::iterator ITER_PPT;
typedef multimap<DWORD, CLIENT_PACKET*>::iterator ITER_PACKET;

CPacketProcessor::CPacketProcessor()
{

}

CPacketProcessor::~CPacketProcessor()
{

}

void CPacketProcessor::Initialize()
{
	InitializeCriticalSection(&m_cs);
	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ProcessPacketThread, this, 0, NULL);
}

void CPacketProcessor::Destroy()
{

}

void CPacketProcessor::OnProcessPacket(CBaseClient* pClient, char* pPacket)
{
	PACKET_HEAD* pHeader = (PACKET_HEAD*)pPacket;
	if( pHeader->dwSize > MAX_PACKET_SIZE )
	{
		WRITE_LOG_SERVER("[Error]: packet size error: size = %d, ctype: %d, type: %d", pHeader->dwSize, pClient->GetType(), pHeader->dwMajorType);
		return;
	}
	

	// add to process deque
	CLIENT_PACKET* p = new CLIENT_PACKET;

	p->pClient		= pClient;
	p->pBuffer		= new char[pHeader->dwSize+1];
	p->bSocketClose = false;

	memset(p->pBuffer, 0, pHeader->dwSize+1);
	memcpy(p->pBuffer, pPacket, pHeader->dwSize);


	EnterCriticalSection(&m_cs);
	m_ProcessDeque.push_back(p);
	LeaveCriticalSection(&m_cs);

	SetEvent(m_hEvent);
}

void CPacketProcessor::OnSocketClosed(CBaseClient* pClient)
{
	// add to process deque
	CLIENT_PACKET* p = new CLIENT_PACKET;

	p->pClient		= pClient;
	p->pBuffer		= NULL;
	p->bSocketClose = true;


	EnterCriticalSection(&m_cs);
	m_ProcessDeque.push_back(p);
	LeaveCriticalSection(&m_cs);

	SetEvent(m_hEvent);
}

void CPacketProcessor::ProcessPacket()
{
	while( TRUE )
	{
		DWORD ret = WaitForSingleObject(m_hEvent, INFINITE);
		if( ret != 0 )
			continue;

		deque<CLIENT_PACKET*> packetDeque;

		EnterCriticalSection(&m_cs);
		m_ProcessDeque.swap(packetDeque);
		LeaveCriticalSection(&m_cs);


		deque<CLIENT_PACKET*>::iterator itr;
		for(itr = packetDeque.begin(); itr != packetDeque.end(); itr++)
		{
			CLIENT_PACKET* packet = *itr;

			if( packet->bSocketClose )
				ProcessSocketClose(packet);
			else
				ProcessOnePacket(packet);

			if( packet->pBuffer != NULL )
			{
				delete packet->pBuffer;
				packet->pBuffer = NULL;
			}

			if( packet != NULL )
			{
				delete packet;
				packet = NULL;
			}
		}
	}
}

DWORD WINAPI CPacketProcessor::ProcessPacketThread(LPARAM lParam)
{
	CPacketProcessor* pThis = (CPacketProcessor*)lParam;
	if( pThis == NULL )
		return 0;

	pThis->ProcessPacket();
}

//
// process one packet
//
void CPacketProcessor::ProcessOnePacket(CLIENT_PACKET* pPacket)
{
	PACKET_HEAD* pHeader = (PACKET_HEAD*)pPacket->pBuffer;
	if( pHeader == NULL )
		return;

	CBaseClient* pClient = pPacket->pClient;

	// check size
	if( pHeader->dwSize > MAX_PACKET_SIZE )
	{
		if( pClient->GetType() == CTYPE_MOBILE )
		{
			WRITE_LOG_SERVER("[M->P]: packet size error size = %d, type = %d", pHeader->dwSize, pHeader->dwMajorType);
		}
		else if( pClient->GetType() == CTYPE_PPTSHELL )
		{
			WRITE_LOG_SERVER("[P->M]: packet size error size = %d, type = %d", pHeader->dwSize, pHeader->dwMajorType);
		}

		return;
	}

	if( pHeader->dwMajorType == PT_REGISTER )
	{
		PACKET_REGISTER* pRegisterPacket = (PACKET_REGISTER*)pHeader;

		pClient->SetType(pRegisterPacket->dwClientType);
		pClient->SetUID(pRegisterPacket->dwMobileID);
		pClient->SetPPTID(pRegisterPacket->dwPPTShellID);

		if( pRegisterPacket->dwClientType == CTYPE_MOBILE )
		{
			m_mapMobiles[pRegisterPacket->dwMobileID] = pClient;

			// find main PPTShell client
			CBaseClient* pPPTShellClient = FindPPTShell(pRegisterPacket->dwPPTShellID, 0);
			if( pPPTShellClient != NULL )
			{			
				// check whether already have a connection for this mobile
				CBaseClient* pTempClient = FindPPTShell(pRegisterPacket->dwPPTShellID, pRegisterPacket->dwMobileID);
				if( pTempClient == NULL )
				{
					// notify main PPTShell to create a new connection for this mobile
					pPPTShellClient->SendPacket(pPacket->pBuffer, sizeof(PACKET_REGISTER));
				}
			
				WRITE_LOG_SERVER("[Register]: type: mobile | UID: %u | PPTID: %u", pRegisterPacket->dwMobileID, pRegisterPacket->dwPPTShellID);
			}
			else
			{
				WRITE_LOG_SERVER("[PPTShell Not Found]: PPTID: %u, UID: %u, PPTCount: %d", pRegisterPacket->dwPPTShellID, pRegisterPacket->dwMobileID, m_mapPPTShells.size());
				pClient->CloseSocket();

				// print out all PPTShell's IDs
				WRITE_LOG_SERVER("-------------------------PPTShell List---------------------");
				multimap<DWORD, CBaseClient*>::iterator itr;

				string strInfo;
				for(itr = m_mapPPTShells.begin(); itr != m_mapPPTShells.end(); itr++)
				{
					CBaseClient* p = itr->second;
					
					char szBuf[MAX_PATH];
					sprintf(szBuf, "%u:%u", p->GetPPTID(), p->GetUID());

					strInfo += szBuf;
					strInfo += " | ";
				}

				WRITE_LOG_SERVER((char*)strInfo.c_str());

				WRITE_LOG_SERVER("-----------------------------------------------------------");
			}
		 
		}
		else if( pRegisterPacket->dwClientType == CTYPE_PPTSHELL )
		{
			// send all cached packets for this mobile
			if( pRegisterPacket->dwMobileID != 0 )
				SendCachedPackets(pClient);

			// remove previous PPTShell
			RemovePPTShell(pClient->GetPPTID(), pClient->GetUID());

			m_mapPPTShells.insert(pair<DWORD, CBaseClient*>(pRegisterPacket->dwPPTShellID, pClient));
			WRITE_LOG_SERVER("[Register]: type: PPTShell | PPTID: %u | UID: %u", pRegisterPacket->dwPPTShellID, pRegisterPacket->dwMobileID);
		}

	}
	else if( pHeader->dwMajorType == PT_CLOSE_MOBILE )
	{
		DWORD dwUID = pClient->GetUID();

		CBaseClient* pMobileClient = FindMobile(dwUID);
		if( pMobileClient == NULL )
			return;

		pMobileClient->CloseSocket();
		WRITE_LOG_SERVER("[CloseMobile]: %d", pMobileClient->GetUID());
	}
	else if( pHeader->dwMajorType == PT_HEARTBEAT_RQST)		//心跳包，发应答
	{
		WRITE_LOG_SERVER("Receive heartbeat request! socket:%d", pClient->GetSocket());
		pClient->SendHeartBeatAck();
	}
	else
	{ 
		int nType = pClient->GetType();

		// transfer to PPT
		if( nType == CTYPE_MOBILE )
		{
			DWORD dwPPTID = pClient->GetPPTID();
			DWORD dwUID = pClient->GetUID();

			CBaseClient* pPPTShellClient = FindPPTShell(dwPPTID, dwUID);
			if( pPPTShellClient == NULL )
			{
				// cache this packet
				CacheOnePacket(pPacket);
				return;
			}

			// multiple channel
			BOOL res = pPPTShellClient->SendPacket((char*)pHeader, pHeader->dwSize);
			if( !res )
			{
				DWORD dwErrCode = WSAGetLastError();
				WRITE_LOG_SERVER("[M->P]: send packet failed: type: %d, errCode: %d", pHeader->dwMajorType, dwErrCode);
			}
			
			//WRITE_LOG_SERVER("[M->P]: %d", pHeader->dwType);
		}
		// transfer to Mobile
		else if( nType == CTYPE_PPTSHELL )
		{
			DWORD dwUID = pClient->GetUID();

			CBaseClient* pMobileClient = FindMobile(dwUID);
			if( pMobileClient == NULL )
				return;

			BOOL res = pMobileClient->SendPacket((char*)pHeader, pHeader->dwSize);
			if( !res )
			{
				DWORD dwErrCode = WSAGetLastError();
				WRITE_LOG_SERVER("[P->M]: type: %d, UID: %d, size: %d errCode: %d, send packet failed", pHeader->dwMajorType, dwUID, pHeader->dwSize, dwErrCode);
			}
		}
	}
}


//
// process socket close
//
void CPacketProcessor::ProcessSocketClose(CLIENT_PACKET* pPacket)
{
	CBaseClient* pClient = pPacket->pClient;

	if( pClient->GetType() == CTYPE_MOBILE )
	{
		m_mapMobiles.erase(pClient->GetUID());

		// notify PPTShell which user logoff
		CBaseClient* pPPTShellClient = FindPPTShell(pClient->GetPPTID(), pClient->GetUID());
		if( pPPTShellClient != NULL )
		{
			PACKET_HEAD LogoffPacket;
			LogoffPacket.dwMajorType = ACTION_DISCONNECT;
			LogoffPacket.dwSize = sizeof(PACKET_HEAD);

			pPPTShellClient->SendPacket((char*)&LogoffPacket, LogoffPacket.dwSize);
		}

		WRITE_LOG_SERVER("[Disconnection]: mobile | %u", pClient->GetUID());
	}
	else if( pClient->GetType() == CTYPE_PPTSHELL )
	{
		// close this mobile
		DWORD dwPPTID = pClient->GetPPTID();
		DWORD dwUID = pClient->GetUID();
		if( dwUID != 0 )
		{
			CBaseClient* pMobileClient = FindMobile(dwUID);
			if( pMobileClient == NULL )
				return;

			pMobileClient->CloseSocket();
			
			// remove mobile
			m_mapMobiles.erase(dwUID);
			WRITE_LOG_SERVER("[Disconnection]: mobile, PPTID: %u, MobileID: %u", pMobileClient->GetPPTID(), pMobileClient->GetUID());
		}

		// remove PPTShell
		RemovePPTShell(dwPPTID, dwUID);

		WRITE_LOG_SERVER("[Disconnection]: PPTShell, PPTID: %u, MobileID: %u", pClient->GetPPTID(), pClient->GetUID());
	}

	WRITE_LOG_SERVER("[Count]: PPTShell: %d, Mobile: %d", m_mapPPTShells.size(), m_mapMobiles.size());
}

//
// find mobile client
//
CBaseClient* CPacketProcessor::FindMobile(DWORD dwUID)
{
	map<DWORD, CBaseClient*>::iterator itr = m_mapMobiles.find(dwUID);
	if( itr == m_mapMobiles.end() )
		return NULL;

	return itr->second;
}

//
// find PPTShell client
//
CBaseClient* CPacketProcessor::FindPPTShell(DWORD dwPPTID, DWORD dwUID)
{
	pair<ITER_PPT, ITER_PPT> range = m_mapPPTShells.equal_range(dwPPTID);
	for(ITER_PPT itr = range.first; itr != range.second; itr++)
	{
		CBaseClient* pPPTShellClient = itr->second;
		if( pPPTShellClient == NULL )
			continue;

		// this PPTShell client is for this Mobile
		if( pPPTShellClient->GetUID() == dwUID )
			return pPPTShellClient;
		
	}

	return NULL;
}

//
// cache one packet
//
void CPacketProcessor::CacheOnePacket(CLIENT_PACKET* pPacket)
{
	CLIENT_PACKET* p = new CLIENT_PACKET;

	CBaseClient* pClient = pPacket->pClient;
	PACKET_HEAD* pHeader = (PACKET_HEAD*)pPacket->pBuffer;
	DWORD dwUID = pClient->GetUID();

	p->pClient		= pClient;
	p->pBuffer		= new char[pHeader->dwSize+1];
	p->bSocketClose = false;

	memset(p->pBuffer, 0, pHeader->dwSize+1);
	memcpy(p->pBuffer, pHeader, pHeader->dwSize);

	m_mapCachedPackets.insert(pair<DWORD, CLIENT_PACKET*>(dwUID, p));
}

//
// send cached packets
//
void CPacketProcessor::SendCachedPackets(CBaseClient *pClient)
{
	DWORD dwUID = pClient->GetUID();

	pair<ITER_PACKET, ITER_PACKET> range = m_mapCachedPackets.equal_range(dwUID);
	for(ITER_PACKET itr = range.first; itr != range.second; itr++)
	{
		CLIENT_PACKET* pPacket = itr->second;
		if( pPacket == NULL )
			continue;

		PACKET_HEAD* pHeader = (PACKET_HEAD*)pPacket->pBuffer;
		if( pHeader == NULL )
			continue;

		pClient->SendPacket((char*)pHeader, pHeader->dwSize);

		delete pPacket->pBuffer;
		delete pPacket;
	}

	m_mapCachedPackets.erase(dwUID);
}

// 
// remove PPTShell
//
void CPacketProcessor::RemovePPTShell(DWORD dwPPTID, DWORD dwUID)
{
	if( dwUID == 0 )
	{
		// remove all PPTShells which has same PPTID because this PPTShell is main
		m_mapPPTShells.erase(dwPPTID);
	}
	else
	{
		pair<ITER_PPT, ITER_PPT> range = m_mapPPTShells.equal_range(dwPPTID);
		for(ITER_PPT itr = range.first; itr != range.second; itr++)
		{
			CBaseClient* pPPTShellClient = itr->second;
			if( pPPTShellClient == NULL )
				continue;

			// this PPTShell client is for this Mobile
			if( pPPTShellClient->GetUID() == dwUID )
			{
				m_mapPPTShells.erase(itr);
				break;
			}

		}
	}
	
}