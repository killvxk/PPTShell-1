//===========================================================================
// FileName:				PacketStream.h
//	
// Desc:						protocols
//============================================================================
#ifndef _PACKET_STREAM_H_
#define _PACKET_STREAM_H_

#define STREAM_BUFFER_SIZE			512

class CPacketStream : public CStream
{
public:
	CPacketStream(char* pBuffer = NULL, int BufferSize = 0);
	~CPacketStream();
	
	void PackCommand(DWORD dwMajorType, DWORD dwMinorType);
	void PackThreeCommand(DWORD dwMajorType, DWORD dwMinorType, DWORD dwThirdCmd);
	
	// user action
	void UnPackUserLogon(DWORD& dwUID, tstring& strName, BOOL &bMobileLogin);
	void UnPackModifyName(DWORD& dwUID, tstring& strName);
	void UnPackModifyPermission(DWORD& dwUID, DWORD& dwPermission);
	
	void PackUserLogonResponse(BOOL bLeader, DWORD dwPermission);
	void PackGuestLogonResponse(DWORD dwUID, DWORD dwPermission, tstring strName);
	void PackGuestLogoffResponse(DWORD dwUID);
	void PackGuestModifyName(DWORD dwUID, tstring strName);

	// control action
	void UnPackPPTControl(DWORD& dwControlCommand);
	void UnPackImageControl(DWORD& dwControlCommand);
	void UnPack3DControl(DWORD& dwControlCommand);
	void UnPackLaserControl();
	void UnPackWhiteBoardControl();
	void UnPackMagnifier(DWORD& dwSize, DWORD& dwControlCommand);

	void PackMobileControl();
	void PackControlResponse(DWORD dwControlCommand, BOOL bSuccess, tstring strErrorDesc);

	// push data
	void PackPushPPTBasicInfo(tstring strPPTName, DWORD dwPageCount);
	void PackPushOutlineInfo(vector<tstring> vecOutline, int nStartIdx, int& nPackedCount);
	void PackPushPCInfo(tstring strComputerName, int nScreenWidth, int nScreenHeight);

	void PackPushPlayBegin();
	void PackPushPlayEnd();
	void PackPushPlaySlide(int nSlideIdx);
	void PackPushVideoList(int nSlideIdx, vector<float> vecVideoList);

	void PackPushSlideUpdateStart();
	void PackPushSlideUpdatePageCnt(int iPageCnt);
	void PackPushSlideUpdateNew(int iNewID, int iOldID);
	void PackPushSlideUpdateEnd();

	void PackPushPreviewSlideIdx(int iSlideIdx);

	void PackSyncDataPPTChanged();

	void PackPushMagnifierRect(float fPerLeft, float fPerTop, float fPerRight, float fPerBottom);

	// pull data




	// file transfer
	void UnPackFileHead(tstring& strFileName, DWORD& dwFileSize, DWORD& dwFileCrc, DWORD& dwFileType);
	void UnPackFileData(DWORD& dwFileCrc, char* pOutBuffer, DWORD& dwDataSize);
	void UnPackFileEnd(DWORD& dwFileCrc);
	void UnPackFileCancel(DWORD& dwFileCrc);
	void UnPackFileCRC(tstring& strFileName, DWORD& dwFileCRC);	// kyp20160125

	void PackFileHead(TCHAR* szFileName, DWORD dwFileSize, DWORD dwFileCrc, DWORD dwFileType, DWORD dwFileID);
	void PackFileData(DWORD dwFileCrc, char* pFileData, DWORD dwDataSize);
	void PackFileEnd(DWORD dwFileCrc, DWORD dwFileID);
	void PackFileCancel(DWORD dwFileCrc);
	void PackFileCheckResult(tstring& strFileName, BOOL bCheckCRCResult);	// kyp20160125

	void SkipHeader();
protected:
	
	void ReadHeader(DWORD& dwMajorType, DWORD& dwMinorType, DWORD& dwSize);
	void WriteHeader(DWORD dwMajorType, DWORD dwMinorType);
	void CorrectSize();



protected:
	char			m_StreamBuffer[STREAM_BUFFER_SIZE];		// small buffer
	

};

#endif

