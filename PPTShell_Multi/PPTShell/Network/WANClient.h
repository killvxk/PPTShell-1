//===========================================================================
// FileName:				WANClient.h
// 
// Desc:				
//============================================================================
#ifndef _WAN_CLIENT_H_
#define _WAN_CLIENT_H_

#define MAX_CLIENT_PACEKT_SIZE		1024*9

class CWANThread;

//-------------------------------------------------------------------
// WAN Client
//
enum
{
	WAN_NONE	= 0,
	WAN_MOBILE,
	WAN_PPTSHELL,
};

enum
{
	PT_TRANSFER				= 0xFFFF,
	PT_REGISTER				= 0xFFFE,
	PT_CLOSE_MOBILE			= 0xFFFD,
	PT_HEARTBEAT_RQST		=0xfffb,			//心跳包
	PT_HEARTBEAT_ACK		=0xfffc	,		//心跳包 应答
	PT_DISCONNECT				= 	0xfffa		//断开连接
};


typedef struct PACKET_REGISTER : PACKET_HEAD
{
	DWORD		dwClientType;			// 0: none 1: mobile / 2: PPTShell
	DWORD		dwMobileID;				// for mobile ID
	DWORD		dwPPTShellID;			// for PPTShell

} PACKET_REGISTER, *PPACKET_REGISTER;

//
// queue packet
//
struct QUEUE_PACKET
{
	DWORD		dwSize;
	char		szData[MAX_CLIENT_PACEKT_SIZE];
};

//
// notify functions
//
typedef void (*ON_PACKET_RECV_FUNC)(SOCKET sock, int type, char* packet);
typedef void (*ON_SOCKET_CLOSE_FUNC)(SOCKET sock);

class CWANClient
{
public:
	CWANClient();
	~CWANClient();

	BOOL				Initialize();
	BOOL				Destroy();

	BOOL				Connect(char* szServerIP, int nServerPort);
	BOOL				SetRecvCloseNotifyFunc(ON_PACKET_RECV_FUNC pPacketProcessFunc, ON_SOCKET_CLOSE_FUNC pSocketCloseFunc);
	BOOL				OnDataRecvd();				// 已不使用
	void				CloseSocket();

	BOOL				SendRegisterPacket(DWORD dwMobileID, DWORD dwPPTShellID);
	BOOL				SendPacket(char* pData, int nDataSize);		// 已不使用

	// threads
	void				ProcessRecvEvent();			// 已不使用
	static DWORD WINAPI RecvThread(LPARAM lParam);	// 已不使用
	int					ProcessPacket();

	void				ProcessSendEvent();			// 已不使用
	static DWORD WINAPI SendThread(LPARAM lParam);	// 已不使用

	SOCKET				GetSocket();
	
	//心跳包处理
	BOOL				SendHeartBeat();
	bool				IfRcvHeartBeatAck();

	DWORD				GetUID() { return m_dUID; }

	// 处理接收到的数据，代码与OnDataRecvd里大部分相同
	BOOL				DealRecvData();
	void	SetWanThread(CWANThread* pWanThread){ m_pWanThread = pWanThread; }
protected:
	BOOL				SendData(char* pData, int nDataSize);

	int					ThreadExit();

protected:
	SOCKET							m_ClientSocket;
	int								m_nConnectType;

	HANDLE							m_hRecvEvent;		// 已不使用
	HANDLE							m_hSendEvent;		// 已不使用
	HANDLE							m_hRecvThread;		// 已不使用
	HANDLE							m_hSendThread;		// 已不使用
	
	int								m_nRecvdDataSize;
	char							m_szRecvBuffer[MAX_CLIENT_PACEKT_SIZE];

	CRITICAL_SECTION				m_Lock;
	deque<QUEUE_PACKET*>			m_SendQueue;		// 已不使用

	DWORD m_dUID;
	
	bool m_bHeartbeatRqst;			//是否发起心跳包
	int m_iHeartbeatTryCnt;

	ON_PACKET_RECV_FUNC				m_pPacketProcessFunc;
	ON_SOCKET_CLOSE_FUNC			m_pSocketCloseFunc;
	int m_iThreadDestroyCnt;

	friend class CWANThread;
	CWANThread* m_pWanThread;		// 线程类指针
};

#endif