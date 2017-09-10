//===========================================================================
// FileName:				ClientConnection.h
// 
// Desc:				
//============================================================================
#ifndef _CLIENT_CONNECTION_H_
#define _CLIENT_CONNECTION_H_


class CBaseClient;

#define MAX_SERVER_PACEKT_SIZE		(9*1024)
typedef void (*ON_PACKET_FUNC)(CBaseClient* pClient, char* packet);
typedef void (*ON_SOCKET_CLOSE_FUNC)(CBaseClient* pClient);

//
// connect type
//
enum
{
	CONNECT_TYPE_NONE		= 0,
	CONNECT_TYPE_LAN,
	CONNECT_TYPE_BLUETOOTH,
	CONNECT_TYPE_WAN,
};

struct WSAOVERLAPPED_EX : WSAOVERLAPPED
{
	char		op;			// 0: send 1:recv
	WSABUF		wsaBuf;
	char*		buffer;
	int			size;
	int			sizeTransferred;

};

class CBaseClient
{
public:
	CBaseClient();
	~CBaseClient();

	virtual BOOL Initialize(SOCKET clientSocket, SOCKADDR* clientAddr);

	BOOL SetSocketNotifyFunc(ON_PACKET_FUNC pPacketProcessFunc, ON_SOCKET_CLOSE_FUNC pSocketCloseFunc);
	BOOL OnDataTransfered(DWORD dwNumberOfBytesTransfered, WSAOVERLAPPED_EX* lpOverlapped);
	BOOL OnSocketClosed();
	BOOL RecvPacket();
	BOOL RecvPacketRemainPart();

	BOOL SendPacket(char* pData, int nDataSize);
	BOOL CloseSocket();

	BOOL SendHeartBeatAck();
	
	inline void   SetType(int nType)		{ m_nType = nType;			}
	inline void   SetUID(DWORD dwUID)		{ m_dwUID = dwUID;			}
	inline void   SetPPTID(DWORD dwPPTID)	{ m_dwPPTID = dwPPTID;		}
	
	inline int	  GetType()					{ return m_nType;			}
	inline DWORD  GetUID()					{ return m_dwUID;			}
	inline DWORD  GetPPTID()				{ return m_dwPPTID;			}
	inline SOCKET GetSocket()				{ return m_ClientSocket;	}

protected:
	int  ProcessOnePacket(char* pPacket);

protected:
	SOCKET						m_ClientSocket;
	SOCKADDR_IN					m_ClientAdress;

	WSAOVERLAPPED_EX			m_RecvOverlapped;

	WSABUF						m_wsaRecvBuf;
	char						m_szRecvBuf[MAX_SERVER_PACEKT_SIZE];
	int							m_nSizeRecv;



	ON_PACKET_FUNC				m_pPacketProcessFunc;
	ON_SOCKET_CLOSE_FUNC		m_pSocketCloseFunc;

	CRITICAL_SECTION			m_Lock;

	//
	DWORD						m_dwUID;		// for mobile
	DWORD						m_dwPPTID;		// for PPTShell
	int							m_nType;		// mobile or PPTShell
	
};

#endif