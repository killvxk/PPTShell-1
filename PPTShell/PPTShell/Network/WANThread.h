//
// WANClient的线程类，这里处理所有WANClient的数据接收、发送操作，最终将数据传给WANClient进行处理。
// 创建一个完成端口处理线程和一个数据发送线程。
// 处理线程处理客户端关闭和数据接收操作；
// 发送线程将发送队列的数据依次发送出去。
//
#pragma once
#include "PacketProcess/Packet.h"
#include "WANClient.h"

// 完成端口OV结构体
typedef struct _IOCPOVERLAPPED
{
	OVERLAPPED	stOverlapped;	// 异步IO
	WSABUF		stWsaBuf;		// 数据内存
	unsigned	nOperType;		// 操作，1=接收；2=发送；3=断开

	_IOCPOVERLAPPED()
	{
		nOperType = 0;
		ZeroMemory(&stOverlapped, sizeof(stOverlapped));
	}
}IOCPOVERLAPPED, *PIOCPOVERLAPPED;

// 发送队列数据结构，增加客户端指针
typedef struct _QUEUE_PACKET_EX :QUEUE_PACKET
{
	CWANClient* pClient;
}QUEUE_PACKET_EX, *PQUEUE_PACKET_EX;


class CWANThread
{
public:
	CWANThread(void);
	~CWANThread(void);

	// 初始化
	BOOL Initialize();
	// 获取完成端口句柄
	inline HANDLE GetIOCompletionPort(){ return m_hIOCompletionPort; }
	// 绑定客户端与完成端口
	BOOL AssociateIOCP(CWANClient* pClient);

	// 增加接收请求
	void PostRecv(CWANClient* pClient);
	// 保存待发送数据到发送队列
	BOOL PostSend(CWANClient* pClient, char* pData, int nDataSize);		

	void Destroy();		// 销毁类资源

	//void SetExitFlag(bool bExit){m_bCloseApp = bExit; Destroy();}
	//inline void PushDeleteClient(CWANClient* pDeleteClient);
	void SetMainClient(CWANClient* pMainClient){m_pMainClient = pMainClient;}
protected:
	// 增加接收请求
	inline void PostRecv(CWANClient* pClient, PIOCPOVERLAPPED pIOCPInfo);
	// 处理接收到的数据
	inline void DoRecv(CWANClient* pClient, PIOCPOVERLAPPED pIOCPInfo, DWORD dwTransferred);
	//void DoAccept();
	//void DoSend();
	//void DoCloseClient(CWANClient* pClient);

	// 完成端口接收线程和数据发送线程
	static DWORD WINAPI RecvThread(LPARAM lParam);
	static DWORD WINAPI SendThread(LPARAM lParam);

	// 移除客户端
	inline void RemoveClient(CWANClient* pClient);

	bool							m_bCloseApp;				// 程序将退出

	HANDLE							m_hIOCompletionPort;		// 完成端口句柄
	unsigned						m_nRecvThreadNum;			// 接收线程数/完成端口线程数
	vector<HANDLE>					m_vtRecvThreadHandle;		// 接收线程句柄
	map<CWANClient*, IOCPOVERLAPPED*>	m_mapIOCPRecvInfo;		// 客户端-接收OV
	CRITICAL_SECTION				m_csIOCPInfo;				// m_mapIOCPRecvInfo的临界区

	HANDLE							m_hSendThread;				// 数据发送线程
	HANDLE							m_hSendEvent;				// 发送线程事件
	deque<PQUEUE_PACKET_EX>			m_SendQueue;				// 发送队列
	CRITICAL_SECTION				m_csSendSection;			// m_csSendSection/发送队列临界区
	//map<CWANClient*, IOCPOVERLAPPED*>	m_mapIOCPSendInfo;		// 客户端-发送OV

	//vector<CWANClient*>				m_vtDeleteClients;			// 保存将被删除的客户端指针
	CWANClient*						m_pMainClient;				// PPTShell和PPTServer长连接的Client
};
