//===========================================================================
// FileName:				PacketProcessor.h
//	
// Desc:					 process packets
//============================================================================
#ifndef _PACKET_PROCESSOR_H_
#define _PACKET_PROCESSOR_H_

struct CLIENT_PACKET
{
	CBaseClient*	pClient;		// source
	char*			pBuffer;
	bool			bSocketClose;
};

class CPacketProcessor
{
public:
	CPacketProcessor();
	~CPacketProcessor();

	void			Initialize();
	void			Destroy();
	void			OnProcessPacket(CBaseClient* pClient, char* pPacket);
	void			OnSocketClosed(CBaseClient* pClient);

	void			ProcessPacket();
	void			ProcessOnePacket(CLIENT_PACKET* pPacket);
	void			ProcessSocketClose(CLIENT_PACKET* pPacket);

	static DWORD WINAPI ProcessPacketThread(LPARAM lParam);

protected:
	CBaseClient*	FindMobile(DWORD dwUID);
	CBaseClient*	FindPPTShell(DWORD dwPPTID, DWORD dwUID);
	void			CacheOnePacket(CLIENT_PACKET* pPacket);
	void			SendCachedPackets(CBaseClient* pClient);
	void			RemovePPTShell(DWORD dwPPTID, DWORD dwUID);

protected:
	HANDLE										m_hEvent;
	CRITICAL_SECTION							m_cs;
	deque<CLIENT_PACKET*>						m_ProcessDeque;	

	map<DWORD, CBaseClient*>					m_mapMobiles;			// UID		<--> Mobile
	multimap<DWORD, CBaseClient*>				m_mapPPTShells;			// PPTID	<--> PPTShells
	multimap<DWORD, CLIENT_PACKET*>				m_mapCachedPackets;		// UID      <--> Cached packets
};

#endif