//===========================================================================
// FileName:				ClientConnection.h
// 
// Desc:				
//============================================================================
#ifndef _CLIENT_CONNECTION_H_
#define _CLIENT_CONNECTION_H_
#include "Packet.h"

class CBaseClient;

#define MAX_SERVER_PACEKT_SIZE		(9*1024)
typedef void (*ON_PACKET_FUNC)(CBaseClient* pClient, char* packet);
typedef void (*ON_SOCKET_CLOSE_FUNC)(CBaseClient* pClient);

class CBaseClient
{
public:
	CBaseClient();
	~CBaseClient();

	virtual BOOL Initialize(SOCKET clientSocket, SOCKADDR* clientAddr);

	BOOL OnDataTransfered(DWORD dwNumberOfBytesTransfered, WSAOVERLAPPED_EX* lpOverlapped);
	BOOL OnSocketClosed();
	BOOL RecvPacket();
	BOOL RecvPacketRemainPart();

	BOOL SendPacket(char* pData);

	BOOL SendHeartBeatAck();
	
	inline void   SetType(EClientType type)		{ m_type = type; }
	inline void   SetUID(DWORD dwUID)		{ m_dwUID = dwUID;			}
	inline void   SetPPTID(DWORD dwPPTID)	{ m_dwPPTID = dwPPTID;		}
	
	inline EClientType	  GetType()					{ return m_type; }
	inline DWORD  GetUID()					{ return m_dwUID;			}
	inline DWORD  GetPPTID()				{ return m_dwPPTID;			}
	inline SOCKET GetSocket()				{ return m_ClientSocket;	}
	inline CBaseClient* GetOppositeClient()		{ return m_pOppositeClient; }
	inline void SetOppositeClient(CBaseClient *pClient) { m_pOppositeClient = pClient; }
	inline void SetPCMainClient()	{ m_bIfPCMainClient = TRUE; }
	inline BOOL IsPCMainClient()	{ return m_bIfPCMainClient; }
protected:
	int  ProcessOnePacket(char* pPacket);
	BOOL SendCachePacket(char* pBuffer);

protected:
	SOCKET							m_ClientSocket;
	SOCKADDR_IN					m_ClientAdress;

	WSAOVERLAPPED_EX			m_RecvOverlapped;

	WSABUF					m_wsaRecvBuf;
	char							m_szRecvBuf[MAX_SERVER_PACEKT_SIZE];
	int							m_nSizeRecv;

	WSAOVERLAPPED_EX	*m_pSendOverlapped;
	char *m_szSendBuf;
	bool m_bSending;

	CBaseClient *m_pOppositeClient;		//对于手机client 这个就是pc，否则相反

	CRITICAL_SECTION			m_Lock;

	//
	DWORD						m_dwUID;		// for mobile
	DWORD						m_dwPPTID;		// for PPTShell
	EClientType					m_type;		// mobile or PPTShell
	BOOL							m_bIfPCMainClient;

	vector<char *>	m_vecCachedPackets;			//for Send
	time_t m_tLastTransferTime;
	time_t m_tConnTime;
};

#endif