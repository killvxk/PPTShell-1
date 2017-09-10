//===========================================================================
// FileName:				FileTransferTask.h
//	
// Desc:					 
//============================================================================
#ifndef _FILE_TRANSFER_TASK_H_
#define _FILE_TRANSFER_TASK_H_

#define MAX_FILE_DATA_LEN				8176

enum FileRecvStep
{
	FILE_RECV_START,
	FILE_RECV_DATA,
	FILE_RECV_END,
};

enum FileType
{
	FILE_TYPE_NONE,
	FILE_TYPE_IMAGE,
	FILE_TYPE_SLIDE,
	FILE_TYPE_APK,
	FILE_TYPE_SCRIPT,
	FILE_TYPE_PPTFILE,
};

typedef void (*SEND_PACKET_FUNC)(SOCKET sock, char *pData, int nDataSize, int nConnectType);

//
// send file task
//
class CFileSendTask 
{
public:
	CFileSendTask(LPCTSTR szFilePath, SOCKET receiver, int nConnectType, int nFileType = 0, int iFileID=0);
	~CFileSendTask();

	BOOL				Start();
	BOOL				Stop();
	DWORD				GetFileCrc();

protected:
	TCHAR				m_szFilePath[MAX_PATH*2];
	SOCKET				m_Receiver;
	int					m_nConnectType;
	int					m_nFileType;
	DWORD				m_dwFileCrc;
	int					m_iFileID;
};

//
// recv file task
//
class CFileRecvTask
{
public:
	CFileRecvTask(TCHAR* szFilePath, DWORD dwFileSize, DWORD dwFileCrc);
	~CFileRecvTask();

	BOOL				Start();
	BOOL				Stop();
	BOOL				Finish();
	BOOL				WriteFileData(char* pData, int nDataSize);

protected:
	TCHAR				m_szFilePath[MAX_PATH*2];
	DWORD				m_dwFileSize;
	DWORD				m_dwFileCrc;

	HANDLE				m_hFile;
	HANDLE				m_hFileMapping;
	char*					m_pFileData;

	DWORD				m_dwRecvdSize;
};

#endif