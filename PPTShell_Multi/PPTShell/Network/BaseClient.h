//===========================================================================
// FileName:				BaseClient.h
// 
// Desc:				
//============================================================================
#ifndef _CLIENT_CONNECTION_H_
#define _CLIENT_CONNECTION_H_

#define MAX_SERVER_PACEKT_SIZE		8192

typedef void (*ON_PACKET_RECV_FUNC)(SOCKET sock, int type, char* packet);
typedef void (*ON_PACKET_SENT_FUNC)(SOCKET sock, DWORD dwSizeSent);
typedef void (*ON_SOCKET_CLOSE_FUNC)(SOCKET sock);

//
// connect type
//
enum
{
	CONNECT_TYPE_NONE		= 0,
	CONNECT_TYPE_LAN,
	CONNECT_TYPE_WAN,
	CONNECT_TYPE_BLUETOOTH,
	
};

struct WSAOVERLAPPED_EX : WSAOVERLAPPED
{
	char		op;			// 0: send 1:recv
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

	BOOL SetRecvCloseNotifyFunc(ON_PACKET_RECV_FUNC pPacketRecvFunc, ON_SOCKET_CLOSE_FUNC pSocketCloseFunc);
	BOOL SetPacketSentNotifyFunc(ON_PACKET_SENT_FUNC pPacketSentFunc);

	BOOL OnDataTransfered(DWORD dwNumberOfBytesTransfered, WSAOVERLAPPED_EX* lpOverlapped);
	BOOL OnSocketClosed();
	BOOL RecvPacket();
	BOOL RecvPacketRemainPart();
	BOOL SendPacket(char* pData, int nDataSize);
	
	inline SOCKET GetSocket()	{ return m_ClientSocket; }
	

protected:
	int  ProcessOnePacket();

protected:
	SOCKET						m_ClientSocket;
	int							m_nConnectType;

	WSAOVERLAPPED_EX			m_RecvOverlapped;

	WSABUF						m_wsaRecvBuf;
	char						m_szRecvBuf[MAX_SERVER_PACEKT_SIZE];
	int							m_nSizeRecv;

	ON_PACKET_RECV_FUNC			m_pPacketRecvFunc;
	ON_PACKET_SENT_FUNC			m_pPacketSentFunc;
	ON_SOCKET_CLOSE_FUNC		m_pSocketCloseFunc;
	

	CRITICAL_SECTION			m_Lock;

	
};

#endif