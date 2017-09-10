#include "StdAfx.h"
#include "WANThread.h"

//#include "PacketProcess/Packet.h"
//#include "WANClient.h"
#include "WANClientManager.h"


CWANThread::CWANThread(void)
{
	m_hIOCompletionPort = NULL;
	m_nRecvThreadNum = 1;
	m_bCloseApp = false;
}

CWANThread::~CWANThread(void)
{
	//OutputDebugString(_T("CWANThread Destructor\n"));
}
// 退出所有线程，删除所有资源
void CWANThread::Destroy()
{
	m_bCloseApp = true;
	SetEvent(m_hSendEvent);

	// 等待线程退出
	if(m_hIOCompletionPort != NULL)
	{
		for(int i=0; i<m_nRecvThreadNum; i++)
		{
			PostQueuedCompletionStatus(m_hIOCompletionPort, 0, NULL, NULL);
		}

		for(int i=0; i<m_vtRecvThreadHandle.size(); i++)
		{
			WaitForSingleObject(m_vtRecvThreadHandle[i], INFINITE);
			CloseHandle(m_vtRecvThreadHandle[i]);
		}
		CloseHandle(m_hIOCompletionPort);
	}

	if(m_hSendThread != NULL)
	{
		WaitForSingleObject(m_hSendThread, INFINITE);
		CloseHandle(m_hSendThread);
	}

	// 删除IOCP资源
	map<CWANClient*, IOCPOVERLAPPED*>::iterator iter = m_mapIOCPRecvInfo.begin();
	for(; iter != m_mapIOCPRecvInfo.end(); iter++)
	{
		delete iter->second;
		iter->second = NULL;
	}
	m_mapIOCPRecvInfo.clear();

	CloseHandle(m_hSendEvent);
	DeleteCriticalSection(&m_csSendSection);
	DeleteCriticalSection(&m_csIOCPInfo);

	OutputDebugString(_T("CWANThread Destroy Finished\n"));
}
// 创建事件、临界区、完成端口、线程
BOOL CWANThread::Initialize()
{
	m_hSendEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	InitializeCriticalSectionAndSpinCount(&m_csSendSection, 4000);
	InitializeCriticalSectionAndSpinCount(&m_csIOCPInfo, 4000);

	m_hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if(m_hIOCompletionPort == NULL)
	{
		DWORD dwErr;
		dwErr = GetLastError();

		return FALSE;
	}

	HANDLE hThread = NULL;
	for(int i=0; i<m_nRecvThreadNum; i++)
	{
		hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecvThread, this, 0, NULL);
		if(hThread != NULL)
			m_vtRecvThreadHandle.push_back(hThread);
	}

	m_hSendThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SendThread, this, 0, NULL);

	return TRUE;
}
// 关联客户端的SOCKET到完成端口
BOOL CWANThread::AssociateIOCP(CWANClient* pClient)
{
	HANDLE hTemp = CreateIoCompletionPort((HANDLE)pClient->GetSocket(), m_hIOCompletionPort, (DWORD)pClient, 0);
	if(hTemp == NULL)
	{
		// 关联失败
		DWORD dwErr;
		dwErr = GetLastError();

		return FALSE;
	}

	PostRecv(pClient);

	return TRUE;
}
// 申请OVERLAPPED结构体，保存客户端和结构体信息，最终传递接收请求
void CWANThread::PostRecv(CWANClient* pClient)
{
	IOCPOVERLAPPED* pIOCPOverlapped;
	if(m_mapIOCPRecvInfo.find(pClient) == m_mapIOCPRecvInfo.end())
	{
		pIOCPOverlapped = new IOCPOVERLAPPED;
		EnterCriticalSection(&m_csIOCPInfo);
		m_mapIOCPRecvInfo[pClient] = pIOCPOverlapped;
		LeaveCriticalSection(&m_csIOCPInfo);
	}
	else
	{
		pIOCPOverlapped = m_mapIOCPRecvInfo[pClient];
	}

	PostRecv(pClient, pIOCPOverlapped);
}
// 传递一个接受请求
void CWANThread::PostRecv(CWANClient* pClient, PIOCPOVERLAPPED pIOCPInfo)
{
	DWORD dwRecvCount = 0;
	DWORD dwFlags = 0;
	//char*	pRecvBuffer = NULL;
	//int		nBufferLen = 0;
	//int		nRecvdDataSize = 0;

	// 设置可用的接收内存和字节数
	//pIOCPInfo->nOperType = 1;
	pIOCPInfo->stWsaBuf.buf = pClient->m_szRecvBuffer + pClient->m_nRecvdDataSize;
	pIOCPInfo->stWsaBuf.len = sizeof(pClient->m_szRecvBuffer) - pClient->m_nRecvdDataSize;

	int nRet = WSARecv(pClient->GetSocket(), &pIOCPInfo->stWsaBuf, 1, &dwRecvCount, &dwFlags, &pIOCPInfo->stOverlapped, NULL);

	//OutputDebugString(_T("post recv\n"));
	if(nRet == SOCKET_ERROR)
	{
		DWORD dwErr;
		dwErr = WSAGetLastError();
		if(dwErr == WSA_IO_PENDING)
		{
			// success
		}
		else
		{
			WRITE_LOG_LOCAL(_T("CWANThread PostRecv Error,Code=%u"), dwErr);
		}
	}
}
// 处理接收到的数据，再传递一个接收请求
void CWANThread::DoRecv(CWANClient* pClient, PIOCPOVERLAPPED pIOCPInfo, DWORD dwTransferred)
{
	// 加上本次接收到的字节数
	pClient->m_nRecvdDataSize += dwTransferred;
	pClient->DealRecvData();

	PostRecv(pClient, pIOCPInfo);
}
// 完成端口处理线程，只处理接收数据和关闭客户端
DWORD WINAPI CWANThread::RecvThread(LPARAM lParam)
{
	CWANThread* pThis = (CWANThread*)lParam;
	DWORD			dwTransferred;
	BOOL			bStatusRet;
	LPOVERLAPPED	pOverlapped = NULL;
	long*			pCompletionKey = NULL;

	while(!pThis->m_bCloseApp)
	{
		dwTransferred = 0;
		pOverlapped = NULL;
		pCompletionKey = NULL;
		//OutputDebugString(_T("CWANThread Wait IOCP Status\n"));
		bStatusRet = GetQueuedCompletionStatus(pThis->m_hIOCompletionPort, &dwTransferred, (PULONG_PTR)&pCompletionKey, &pOverlapped, INFINITE);
		if(pThis->m_bCloseApp)
			break;
		CWANClient* pClient = (CWANClient*)pCompletionKey;

		//WRITE_LOG_LOCAL(_T("CWANThread GetIOCPStatus Return trans=%u,client pointer=%x, &pointer=%x, key=%x, &key=%x\n"), dwTransferred, pClient, &pClient, pCompletionKey, &pCompletionKey);

		if(!bStatusRet)
		{
			DWORD dwErr;
			dwErr = WSAGetLastError();

			WRITE_LOG_LOCAL(_T("CWANThread GetIOCPStatus Error=%u\n"), dwErr);

			if(dwErr == WAIT_TIMEOUT || dwErr==ERROR_NETNAME_DELETED)
			{
				// 当MainClient连接被Server断开时，可能会进入这段代码，要关闭本地的MainClient Socket才能重新创建连接线程
				if(pClient == pThis->m_pMainClient)
				{
					OutputDebugString(_T("CWANThread Close MainClient Socket\n"));
					pClient->CloseSocket();
				}
				else
				{
					// 普通客户端被Server断开链接的情况
					pThis->RemoveClient(pClient);
				}
			}
			continue;
		}
		//OutputDebugString(_T("CWANThread RecvThread Deal\n"));

		if(dwTransferred == 0)
		{
			// 客户端关闭的情况
			pClient->CloseSocket();
			pThis->RemoveClient(pClient);
			OutputDebugString(_T("CWANThread closesocket and continue\n"));
			continue;
		}
		// 处理接收数据
		PIOCPOVERLAPPED pIOCPInfo = (PIOCPOVERLAPPED)pOverlapped;
		pThis->DoRecv(pClient, pIOCPInfo, dwTransferred);
	}

	//OutputDebugString(_T("CWANThread RecvThread Exit\n"));
	return 0;
}
// 申请发送结构体，并将数据保存到发送队列
BOOL CWANThread::PostSend(CWANClient* pClient, char* pData, int nDataSize)
{
	if( pClient->m_ClientSocket == INVALID_SOCKET )
		return FALSE;

	if( nDataSize > MAX_CLIENT_PACEKT_SIZE )
		return FALSE;

	QUEUE_PACKET_EX* pPacket = new QUEUE_PACKET_EX;
	if( pPacket == NULL )
		return FALSE;

	pPacket->pClient = pClient;
	pPacket->dwSize = nDataSize;
	memcpy(pPacket->szData, pData, nDataSize);

	//OutputDebugString(_T("postsend\n"));

	EnterCriticalSection(&m_csSendSection);
	m_SendQueue.push_back(pPacket);
	LeaveCriticalSection(&m_csSendSection);
	SetEvent(m_hSendEvent);
	return TRUE;
}
// 发送线程，将队列的数据全部发送出去
DWORD WINAPI CWANThread::SendThread(LPARAM lParam)
{
	CWANThread* pThis = (CWANThread*)lParam;
	while(TRUE)
	{
		WaitForSingleObject(pThis->m_hSendEvent, INFINITE);
		if(pThis->m_bCloseApp)
			break;

		//OutputDebugString(_T("SendThread Run\n"));
		// 取出队列里的所有数据，依次发送出去
		deque<PQUEUE_PACKET_EX> SendQueue;
		EnterCriticalSection(&pThis->m_csSendSection);
		pThis->m_SendQueue.swap(SendQueue);
		LeaveCriticalSection(&pThis->m_csSendSection);

		deque<PQUEUE_PACKET_EX>::iterator itr;
		for(itr = SendQueue.begin(); itr != SendQueue.end(); itr++)
		{
			PQUEUE_PACKET_EX pPacket = *itr;
			if( pPacket == NULL )
				continue;

			pPacket->pClient->SendData(pPacket->szData, pPacket->dwSize);

			delete pPacket;
			pPacket = NULL;
		}
	}

	//OutputDebugString(_T("CWANThread SendThread Exit\n"));
	return 0;
}
// 移除绑定完成端口客户端，释放申请的OVERLAPPED结构体
void CWANThread::RemoveClient(CWANClient* pClient)
{
	IOCPOVERLAPPED* pIOCPOverlapped = NULL;
	EnterCriticalSection(&m_csIOCPInfo);
	if(m_mapIOCPRecvInfo.find(pClient) != m_mapIOCPRecvInfo.end())
	{
		pIOCPOverlapped = m_mapIOCPRecvInfo[pClient];
		m_mapIOCPRecvInfo.erase(pClient);
	}
	LeaveCriticalSection(&m_csIOCPInfo);

	delete pIOCPOverlapped;
}
