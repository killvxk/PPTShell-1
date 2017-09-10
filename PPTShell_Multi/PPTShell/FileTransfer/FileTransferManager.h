//===========================================================================
// FileName:				FileTransferManager.h
//	
// Desc:					
//============================================================================
#ifndef _FILE_TRANSFER_MANAGER_H_
#define _FILE_TRANSFER_MANAGER_H_

#include "Util/Singleton.h"

class CFileTransferManager
{
private:
	CFileTransferManager();
	~CFileTransferManager();

public:
	BOOL Initialize(SEND_PACKET_FUNC pSendPacketFunc = NULL);
	BOOL Destroy();

	BOOL SendFile(char* szFilePath, SOCKET receiver, int nConnectType, int nFileType = 0, int iFileID=0);
	BOOL RecvFile(char* szFilePath, DWORD dwFileSize, DWORD dwFileCrc, int nRecvStep, char* pFileData = NULL, int nDataSize = 0);
	BOOL CancelFile(DWORD dwFileCrc);
	void OnPacketSent(SOCKET sock, DWORD dwSizeSent);

	BOOL CheckFileIsExists(string& strFilePath, DWORD dwRecvFileCRC);	// kyp20160121

	DECLARE_SINGLETON_CLASS(CFileTransferManager);

protected:
	CFileRecvTask* FindRecvTask(DWORD dwFileCrc);
	CFileSendTask* FindSendTask(DWORD dwFileCrc);

protected:
	map<DWORD, CFileSendTask*>			m_mapSendTasks;		// FileCrc <--> CFileSendTask*
	map<DWORD, CFileRecvTask*>			m_mapRecvTasks;		// FileCrc <--> CFileRecvTask*
	CRITICAL_SECTION					m_csSendTask;
	CRITICAL_SECTION					m_csRecvTask;

	SEND_PACKET_FUNC					m_pSendPacketFunc;
};

typedef Singleton<CFileTransferManager> FileTransferManager;

#endif