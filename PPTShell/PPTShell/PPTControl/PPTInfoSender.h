//===========================================================================
// FileName:				PPTInfoSender.h
//	
// Desc:					 
//============================================================================
#ifndef _PPT_INFO_SENDER_H_
#define _PPT_INFO_SENDER_H_

#include "Util/Singleton.h"
#include <set>
#include "PacketProcess/PacketStream.h"

class CPPTInfoSender
{
private:
	CPPTInfoSender();
	virtual ~CPPTInfoSender();

public:
	BOOL Initialize();
	BOOL Destory();

	// ppt event handler
	bool OnSyncPPTInfo(void* param);

	// send functions
	BOOL SendBasicInfo(SOCKET sock, int nConnectType);
	BOOL SendPPTOutLine(SOCKET sock, int nConnectType);
	BOOL SendSlideThumb(SOCKET sock, int nConnectType, int nStartSlideIdx = -1, int nEndSlideIdx = -1);
	BOOL SendSlideThumbList(SOCKET sock, int nConnectType, const vector<int> &vecSlideIdxs);
	BOOL SendSlideThumbOfFile(SOCKET sock, int nConnectType, const tstring& strPPTFile, int iSlideIdx);
	BOOL SendPlaySlide(SOCKET sock, int nConnectType, int nSlideIdx = -1);
	BOOL SendPlayBegin(SOCKET sock, int nConnectType);
	BOOL SendPlayEnd(SOCKET sock, int nConnectType);
	BOOL SendVideoList(SOCKET sock, int nConnectType, int nSlideIdx, vector<float> vecVideoList);
	BOOL SendSlideUpdateStart(SOCKET sock, int nConnectType);
	BOOL SendSlideUpdatePageCnt(SOCKET sock, int nConnectType, int iPageCnt);
	BOOL SendSlideUpdateNew(SOCKET sock, int nConnectType, int iNewID, int iOldID);
	BOOL SendSLideUpdateEnd(SOCKET sock, int nConnectType);
	BOOL SendSyncDataPPTChanged(SOCKET sock, int nConnectType);
	BOOL SendPreviewSlideIdx(SOCKET sock, int nConnectType, int iSlideIdx);

	BOOL SendBasicInfoToAllUsers();
	BOOL SendPPTOutLineToAllUsers();
	BOOL SendSlideThumbToAllUsers(int nStartSlideIdx = -1, int nEndSlideIdx = -1);
	BOOL SendPlayBeginToAllUsers();
	BOOL SendPlaySlideToAllUsers(int nSlideIdx);
	BOOL SendPlayExitToAllUsers();
	BOOL SendVideoListToAllUsers(int nSlideIdx, vector<float> vecVideoList);
	BOOL SendPreviewSlideIdxToAllUser(int iSlideIdx);

	// export thumbnail
	BOOL StartExportThumbnails();
	void SetThumbnailDirty(bool bFlag);
	void IncrementSyncSlideImages();

	bool IsSyncDataPPTChangedAck();
	void SetAckSyncDataPPTChanged()	{m_bSyncDataPPTChangedAck = true;}

	DECLARE_SINGLETON_CLASS(CPPTInfoSender);

	BOOL SendCommandToAllUsers(CPacketStream& packetStream);
protected:
	static DWORD WINAPI ExportThumbnailThread(LPARAM lParam);
	void ExportThumbnail();
	BOOL IsAnyUserOnline();
	void UpdateSlideThumbToAllUser(const tstring &strPPTFileName, int iSlideCnt);

	void GetRqstedPages(set<int> &setAllPagesRqst);
	void ClearUserSlidePageData();

protected:
	BOOL				m_bPlaying;
	HANDLE				m_hThread;
	HANDLE				m_hEvent;
	int					m_nStartThumbnailIdx;
	int					m_nEndThumbnailIdx;
	BOOL				m_bThumbnailDirty;
	map<int, DWORD>		m_mapSlideCRCCodeNow;
	map<DWORD, int>		m_mapSlideCrcToFile;
	tstring				m_strPPTFilePath;
	CRITICAL_SECTION	m_lockSlideCRC;

	bool				m_bSyncDataPPTChangedAck;			//ppt发生改变同步包是否已经应答
	bool				m_bImageSynchronizing;				//ppt缩略图同步中
};

typedef Singleton<CPPTInfoSender> PPTInfoSender;

#endif