//===========================================================================
// FileName:				BaseServer.h
// 
// Desc:				
//============================================================================
#ifndef _BASE_SERVER_H_
#define _BASE_SERVER_H_
#include "BaseClient.h"

class CBaseServer
{
public:
	CBaseServer();
	~CBaseServer();

	virtual SOCKET	CreateServerSocket()	= 0;
	virtual int		BindServerSocket()		= 0;
	virtual int		SetServerService()		= 0;
	virtual int 	AcceptConnection()		= 0;

 
	BOOL			Initialize(PVOID pPacketProcessFunc = NULL, PVOID pSocketCloseFunc = NULL);
	BOOL			Destroy();
	BOOL			StartWork(int nPort = 0);
	BOOL			SendPacket(SOCKET sock, char* pData, int nDataSize);
	BOOL			OnSocketClosed(SOCKET sock);
	BOOL			SetPacketSentNotifyFunc(SOCKET sock, PVOID pPacketSentFunc);

	BOOL			IsSocketOnLine(SOCKET sock);
 
	inline BOOL		IsWorking()					{ return m_bWorking;		 }
	inline HANDLE	GetIOCPHandle()				{ return m_hCompletionpPort; }

	static DWORD WINAPI AcceptThread(LPARAM lParam);
	static DWORD WINAPI IOCPWorkThread(LPARAM lParam);

	CBaseClient*	 GetClientBySock(SOCKET sock);

	BOOL OnDataTransfered(SOCKET sock, DWORD dwNumberOfBytesTransfered, WSAOVERLAPPED_EX* lpOverlapped);
protected:
	BOOL									m_bWorking;
	SOCKET									m_ServerSocket;
	HANDLE									m_hCompletionpPort;
	PHANDLE									m_hWorkerThreadArray;
	int										m_nWorkerThreadCount;
	int										m_nPort;

	CRITICAL_SECTION						m_csClientConnections;
	std::map<SOCKET, CBaseClient*>			m_mapClientConnections;		// SOCKET <--> CBaseClient*
	std::map<CBaseClient*, BYTE>			m_mapClients;

	PVOID									m_pPacketProcessFunc;
	PVOID									m_pSocketCloseFunc;
};

#endif