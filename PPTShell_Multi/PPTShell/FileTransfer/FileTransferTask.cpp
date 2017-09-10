//===========================================================================
// FileName:				FileTransferTask.cpp
//	
// Desc:					
//============================================================================
#include "stdafx.h"
#include "PPTShell.h"
#include "Util/Util.h"
#include "PacketProcess/Packet.h"
#include "PacketProcess/PacketStream.h"
#include "FileTransferTask.h"

//----------------------------------------------------------
// send file task
//
CFileSendTask::CFileSendTask(LPCTSTR szFilePath, SOCKET receiver, int nConnectType, int nFileType/* = 0*/, int iFileID/*=0*/)
{
	m_Receiver      = receiver;
	m_nConnectType  = nConnectType;
	m_nFileType		= nFileType;
	m_dwFileCrc		= 0;
	m_iFileID = iFileID;

	_tcscpy_s(m_szFilePath, szFilePath);
	
}

CFileSendTask::~CFileSendTask()
{

}

BOOL CFileSendTask::Start()
{
	CPPTShellApp* pApp = (CPPTShellApp*)AfxGetApp();
	if( pApp == NULL )
		return FALSE;

	// read file
	HANDLE hFile = CreateFile(m_szFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if( hFile == INVALID_HANDLE_VALUE )
		return FALSE;

	DWORD dwFileSize = GetFileSize(hFile, NULL);
	if( dwFileSize == 0 )
		return FALSE;


	HANDLE hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, dwFileSize, NULL);
	if( hFileMapping == NULL )
	{
		CloseHandle(hFile);
		return FALSE;
	}

	char* pFileData = (char*)MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, dwFileSize);
	if( pFileData == NULL )
	{
		CloseHandle(hFileMapping);
		CloseHandle(hFile);
		return FALSE;
	}

	// File name
	char szFileName[MAX_PATH] = {0};

	char* p = strrchr(m_szFilePath, '\\');
	if( p != NULL )
		strcpy_s(szFileName, p+1);

	// Calculate file crc
	DWORD dwFileCrc = CalcCRC(pFileData, dwFileSize);
	m_dwFileCrc = dwFileCrc;

	// Send file header
	CPacketStream FileHeaderStream;
	FileHeaderStream.PackFileHead(szFileName, dwFileSize, dwFileCrc, m_nFileType, m_iFileID);
	pApp->SendStreamPacket(m_Receiver, &FileHeaderStream, m_nConnectType);


	// Send file data
	char FileDataBuffer[MAX_FILE_DATA_LEN+STREAM_BUFFER_SIZE];
	int nPacketCount = dwFileSize / MAX_FILE_DATA_LEN;

	for(int i = 0; i < nPacketCount; i++)
	{
		// must use a big buffer
		CPacketStream FileDataStream(FileDataBuffer, MAX_FILE_DATA_LEN+STREAM_BUFFER_SIZE);
		FileDataStream.PackFileData(dwFileCrc, pFileData+i*MAX_FILE_DATA_LEN, MAX_FILE_DATA_LEN);
		pApp->SendStreamPacket(m_Receiver, &FileDataStream, m_nConnectType);
	}

	// Send remain file data
	DWORD dwRemainSize = dwFileSize % MAX_FILE_DATA_LEN;
	if( dwRemainSize != 0 )
	{
		CPacketStream FileDataStream(FileDataBuffer, MAX_FILE_DATA_LEN+STREAM_BUFFER_SIZE);
		FileDataStream.PackFileData(dwFileCrc, pFileData+nPacketCount*MAX_FILE_DATA_LEN, dwRemainSize);
		pApp->SendStreamPacket(m_Receiver, &FileDataStream, m_nConnectType);
	}

	// Send end flag
	CPacketStream FileEndStream;
	FileEndStream.PackFileEnd(dwFileCrc, m_iFileID);
	pApp->SendStreamPacket(m_Receiver, &FileEndStream,m_nConnectType);

	// Close file
	UnmapViewOfFile(pFileData);
	CloseHandle(hFileMapping);
	CloseHandle(hFile);

	return TRUE;
}

BOOL CFileSendTask::Stop()
{
	// cancel io
	BOOL res = CancelIo((HANDLE)m_Receiver);
	return TRUE;
}

DWORD CFileSendTask::GetFileCrc()
{
	return m_dwFileCrc;
}

//----------------------------------------------------------
// recv file task
//
CFileRecvTask::CFileRecvTask(char *szFilePath, DWORD dwFileSize, DWORD dwFileCrc)
{
	m_dwFileSize		= dwFileSize;
	m_dwFileCrc			= dwFileCrc;

	m_hFile				= INVALID_HANDLE_VALUE;
	m_hFileMapping		= NULL;
	m_pFileData			= NULL;
	m_dwRecvdSize		= 0;

	strcpy_s(m_szFilePath, szFilePath);
}

CFileRecvTask::~CFileRecvTask()
{

}

BOOL CFileRecvTask::Start()
{
	m_hFile = CreateFile(m_szFilePath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if( m_hFile == INVALID_HANDLE_VALUE )
		return FALSE;

	m_hFileMapping = CreateFileMapping(m_hFile, NULL, PAGE_READWRITE, 0, m_dwFileSize, NULL);
	if( m_hFileMapping == NULL )
	{
		DWORD dwError = GetLastError();
		CloseHandle(m_hFile);
		return FALSE;
	}

	m_pFileData = (char*)MapViewOfFile(m_hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, m_dwFileSize);
	if( m_pFileData == NULL )
	{
		CloseHandle(m_hFileMapping);
		CloseHandle(m_hFile);
		return FALSE;
	}

	return TRUE;
}

BOOL CFileRecvTask::Stop()
{
	if( m_pFileData == NULL )
		return FALSE;

	UnmapViewOfFile(m_pFileData);
	CloseHandle(m_hFileMapping);
	CloseHandle(m_hFile);

	m_pFileData = NULL;

	// delete file
	DeleteFile(m_szFilePath);

	return TRUE;
}

BOOL CFileRecvTask::Finish()
{
	if( m_pFileData == NULL )
		return FALSE;

	UnmapViewOfFile(m_pFileData);
	CloseHandle(m_hFileMapping);
	CloseHandle(m_hFile);

	m_pFileData = NULL;

	return TRUE;
}

BOOL CFileRecvTask::WriteFileData(char *pData, int nDataSize)
{
	if( m_pFileData == NULL )
		return FALSE;

	DWORD dwRemainSize = m_dwFileSize - m_dwRecvdSize;
	if(dwRemainSize < nDataSize)
	{
		WRITE_LOG_LOCAL(_T("WriteFileData - WriteData Overflow, TotalFileSize=%u, RecvdSize=%u, RecvDataSize=%u, File=%s")
			, m_dwFileSize, m_dwRecvdSize, nDataSize, m_szFilePath);

		nDataSize = dwRemainSize;
	}

	memcpy(m_pFileData+m_dwRecvdSize, pData, nDataSize);
	m_dwRecvdSize += nDataSize;

	return TRUE;
}