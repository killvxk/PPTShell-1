//===========================================================================
// FileName:				FileTransferManager.cpp
//	
// Desc:					
//============================================================================
#include "stdafx.h"
#include "PPTShell.h"
#include "FileTransferTask.h"
#include "FileTransferManager.h"


CFileTransferManager* g_pFileTransferManager = NULL;
void OnPacketSent(SOCKET sock, DWORD dwSizeSent);

CFileTransferManager::CFileTransferManager()
{
	g_pFileTransferManager	= this;
}

CFileTransferManager::~CFileTransferManager()
{

}

//
// initialize
//
BOOL CFileTransferManager::Initialize(SEND_PACKET_FUNC pSendPacketFunc /*= NULL*/)
{
	InitializeCriticalSection(&m_csSendTask);
	InitializeCriticalSection(&m_csRecvTask);

	m_pSendPacketFunc = pSendPacketFunc;
	return TRUE;
}

//
// destroy
//
BOOL CFileTransferManager::Destroy()
{

	return TRUE;
}

//
// send file
//
BOOL CFileTransferManager::SendFile(char *szFilePath, SOCKET receiver, int nConnectType, int nFileType /*= 0*/, int iFileID/*=0*/)
{
	CPPTShellApp* pApp = (CPPTShellApp*)AfxGetApp();
	if( pApp == NULL )
		return FALSE;

	CFileSendTask* pTask = new CFileSendTask(szFilePath, receiver, nConnectType, nFileType, iFileID);
	if( pTask == NULL )
		return FALSE;

	// set packet sent notify function
	pApp->SetPacketSentNotifyFunc(receiver, nConnectType, ::OnPacketSent);
	
	// cannot find this file
	BOOL res = pTask->Start();
	if( !res )
	{
		delete pTask;
		pApp->SetPacketSentNotifyFunc(receiver, nConnectType, NULL);
		return FALSE;
	}

	DWORD dwFileCrc = pTask->GetFileCrc();

	EnterCriticalSection(&m_csSendTask);
	m_mapSendTasks[dwFileCrc] = pTask;
	LeaveCriticalSection(&m_csSendTask);

	return TRUE;
}

//
// recv file
//
BOOL CFileTransferManager::RecvFile(char* szFilePath, DWORD dwFileSize, DWORD dwFileCrc, int nRecvStep, char* pFileData /* = NULL */, int nDataSize /* = 0 */)
{
	if( nRecvStep == FILE_RECV_START )
	{
		CFileRecvTask* pTask = new CFileRecvTask(szFilePath, dwFileSize, dwFileCrc);
		if( pTask == NULL )
			return FALSE;

		BOOL res = pTask->Start();
		if( res )
		{
			EnterCriticalSection(&m_csRecvTask);
			m_mapRecvTasks[dwFileCrc] = pTask;
			LeaveCriticalSection(&m_csRecvTask);
		}
	}
	else if( nRecvStep == FILE_RECV_DATA )
	{
		CFileRecvTask* pTask = FindRecvTask(dwFileCrc);
		if( pTask == NULL )
			return FALSE;

		pTask->WriteFileData(pFileData, nDataSize);
		
	}
	else if( nRecvStep == FILE_RECV_END )
	{
		CFileRecvTask* pTask = FindRecvTask(dwFileCrc);
		if( pTask == NULL )
			return FALSE;

		pTask->Finish();
	}

	return TRUE;
}

//
// cancel transfer
//
BOOL CFileTransferManager::CancelFile(DWORD dwFileCrc)
{
	CFileRecvTask* pRecvTask = FindRecvTask(dwFileCrc);
	if( pRecvTask != NULL )
	{
		pRecvTask->Stop();
		return TRUE;
	}

	CFileSendTask* pSendTask = FindSendTask(dwFileCrc);
	if( pSendTask != NULL )
	{
		pSendTask->Stop();
		return TRUE;
	}

	return TRUE;
}

//
// find send task
//
CFileSendTask* CFileTransferManager::FindSendTask(DWORD dwFileCrc)
{
	CFileSendTask* pTask = NULL;

	EnterCriticalSection(&m_csSendTask);

	map<DWORD, CFileSendTask*>::iterator itr = m_mapSendTasks.find(dwFileCrc);
	if( itr != m_mapSendTasks.end() )
		pTask = itr->second;

	LeaveCriticalSection(&m_csSendTask);

	return pTask;
}

//
// find recv task
//
CFileRecvTask* CFileTransferManager::FindRecvTask(DWORD dwFileCrc)
{
	CFileRecvTask* pTask = NULL;

	EnterCriticalSection(&m_csRecvTask);

	map<DWORD, CFileRecvTask*>::iterator itr = m_mapRecvTasks.find(dwFileCrc);
	if( itr != m_mapRecvTasks.end() )
		pTask = itr->second;
	
	LeaveCriticalSection(&m_csRecvTask);

	return pTask;
}

//
// packet sent notify function
//
void CFileTransferManager::OnPacketSent(SOCKET sock, DWORD dwSizeSent)
{
	
}

void OnPacketSent(SOCKET sock, DWORD dwSizeSent)
{
	if( g_pFileTransferManager )
		g_pFileTransferManager->OnPacketSent(sock, dwSizeSent);
}

// 函数：	CheckFileIsExists
// 参数：	strFilePath		- 本地文件，包含路径和文件名
//			dwRecvFileSize	- 接收文件的文件大小
//			dwRecvFileCRC	- 接收文件的CRC
// 返回：	TRUE			- 与本地文件相同
//			FALSE			- 与本地文件不相同
// 描述：	检查文件的文件名、CRC与本地文件是否相同
BOOL CFileTransferManager::CheckFileIsExists(string& strFilePath, DWORD dwRecvFileCRC)
{
	// kyp20160125 1.检查文件是否存在；2.检查文件的CRC是否一致
	if(!PathFileExists(strFilePath.c_str()))
	{
		return FALSE;
	}

	BOOL bFileExists = FALSE;		// FALSE=与本地文件不相同
	char* pFileData = NULL;			// 文件内容指针
	HANDLE hFileMapping = NULL;		// 文件映射句柄

	OFSTRUCT stOpenFile;
	HANDLE hFile = (HANDLE)OpenFile(strFilePath.c_str(), &stOpenFile, OF_SHARE_DENY_NONE);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwLoclaFileSizeHigh = 0;
		DWORD dwLocalFileSize = GetFileSize(hFile, &dwLoclaFileSizeHigh);

		// 使用内存映射，如果内存映射失败，返回FALSE
		hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, dwLocalFileSize, NULL);
		if( hFileMapping != NULL )
		{
			pFileData = (char*)MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, dwLocalFileSize);
			if( pFileData != NULL )
			{
				DWORD dwLocalFileCRC = CalcCRC(pFileData, dwLocalFileSize);
				if(dwLocalFileCRC == dwRecvFileCRC)
				{
					bFileExists = TRUE;
				}
			}
			else
			{
				// pFileData == NULL
			}
		}
		else
		{
			// hFileMapping == NULL
		}
	}

	if(pFileData != NULL)
		UnmapViewOfFile(pFileData);
	if(hFileMapping != NULL)
		CloseHandle(hFileMapping);
	if(hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);

	return bFileExists;
}
