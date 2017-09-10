//===========================================================================
// FileName:				PPTInfoSender.cpp
//	
// Desc:					 
//============================================================================
#include "stdafx.h"
#include "PPTShell.h"
#include "Util/Stream.h"
#include "PacketProcess/PacketStream.h"
#include "PPTController.h"
#include "PPTControllerManager.h"
#include "PPTInfoSender.h"
#include "UserManager/User.h"
#include "UserManager/UserManager.h"
#include "EventCenter/EventDefine.h"
#include "FileTransfer/FileTransferTask.h"
#include "FileTransfer/FileTransferManager.h"
#include "Util/Util.h"

#define DEFAULT_GEN_SLIDE_IMG_CNT		5

CUser* GetUserBySock(SOCKET sock)
{
	CUserManager *userManager = UserManager::GetInstance();
	if (userManager == NULL)
	{
		return NULL;
	}
	return userManager->FindUser(sock);
}

CPPTInfoSender::CPPTInfoSender()
{
	m_nStartThumbnailIdx	= -1;
	m_nEndThumbnailIdx		= -1;
	m_bThumbnailDirty		= TRUE;
	m_bPlaying				= FALSE;
	InitializeCriticalSection(&m_lockSlideCRC);
	m_bSyncDataPPTChangedAck = true;
}

CPPTInfoSender::~CPPTInfoSender()
{
	DeleteCriticalSection(&m_lockSlideCRC);
}

BOOL CPPTInfoSender::Initialize()
{
	tstring strPath = GetLocalPath();
	strPath += _T("\\Cache");
	CreateDirectory(strPath.c_str(),NULL);

	strPath += _T("\\PPTSlides");
	CreateDirectory(strPath.c_str(),NULL);


	::OnEvent(EVT_PPT_INFO_SYNC, MakeEventDelegate(this, &CPPTInfoSender::OnSyncPPTInfo));

	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ExportThumbnailThread, this, 0, NULL);
	
	return TRUE;
}

BOOL CPPTInfoSender::Destory()
{
	return TRUE;
}

void CPPTInfoSender::SetThumbnailDirty(bool bFlag)
{
	m_bThumbnailDirty = bFlag;
}

void CPPTInfoSender::IncrementSyncSlideImages()
{
	m_nStartThumbnailIdx = -1;
	m_nEndThumbnailIdx = -1;
	SetThumbnailDirty(true);
	StartExportThumbnails();
}

BOOL CPPTInfoSender::StartExportThumbnails()
{
	if( !m_bThumbnailDirty )
		return TRUE;

	SetEvent(m_hEvent);
	m_bThumbnailDirty = FALSE;
	

	return TRUE;
}

DWORD WINAPI CPPTInfoSender::ExportThumbnailThread(LPARAM lParam)
{
	CPPTInfoSender* pThis = (CPPTInfoSender*)lParam;
	pThis->ExportThumbnail();

	return 0;
}

void CPPTInfoSender::ExportThumbnail()
{
	CPPTController* pController = GetPPTController();
	if( pController == NULL )
		return;

	while( TRUE )
	{
		DWORD ret = WaitForSingleObject(m_hEvent, INFINITE);
		if( ret == WAIT_FAILED )
			break;
		
		int nSlideCount = pController->GetSlideCount();

		tstring strPPTFileName = pController->GetFileName();
		
		if(strPPTFileName.empty())
			continue;
			
		if ( strPPTFileName.length() > 150 )
			strPPTFileName = strPPTFileName.substr(0, strPPTFileName.length()/2);  //fix file name length to long

		EnterCriticalSection(&m_lockSlideCRC);
		TCHAR szFolderPath[MAX_PATH];
		_stprintf_s(szFolderPath, _T("%s\\Cache\\PPTSlides\\%s"), GetLocalPath().c_str(), strPPTFileName.c_str());
		DeleteDir(szFolderPath);		//删除历史文件
		m_strPPTFilePath = szFolderPath;
		
		set<int> setAllPagesRqst;
		GetRqstedPages(setAllPagesRqst);
		bool bRequestedPages = setAllPagesRqst.size()>0?true:false;
		
		int iDefaultGenCnt = nSlideCount>DEFAULT_GEN_SLIDE_IMG_CNT?DEFAULT_GEN_SLIDE_IMG_CNT:nSlideCount;
		for (int i=1; i<=iDefaultGenCnt; i++)
		{
			setAllPagesRqst.insert(i);
		}
		
		int iBeginIdx = -1;
		int iLastIdx = -1;
		int iCurrIdx = -1;
		set<int>::iterator itrPage = setAllPagesRqst.begin();
		for (; itrPage!=setAllPagesRqst.end(); itrPage++)
		{
			iCurrIdx = *itrPage;
			if (iBeginIdx == -1)
			{
				iBeginIdx = iCurrIdx;
				iLastIdx = iCurrIdx;
				continue;
			}

			if (iCurrIdx == iLastIdx+1)
			{
				iLastIdx = iCurrIdx;
				continue;
			}
			else
			{
				pController->ExportToImages(szFolderPath, "png", iBeginIdx, iLastIdx);	
				iBeginIdx = iCurrIdx;
				iLastIdx = iCurrIdx;
			}
		}
		pController->ExportToImages(szFolderPath, "png", iBeginIdx, iLastIdx);	
//		pController->ExportToImages(szFolderPath, "png");	

		m_mapSlideCRCCodeNow.clear();
		TCHAR szPath[MAX_PATH];
		itrPage = setAllPagesRqst.begin();
		for (; itrPage!=setAllPagesRqst.end(); itrPage++)
		{
			int iIdx = *itrPage;
			_stprintf_s(szPath, _T("%s\\Slide_%d.png"), szFolderPath, iIdx);
			DWORD slideCRC = CalcFileCRC(szPath);
			m_mapSlideCRCCodeNow.insert(make_pair(iIdx, slideCRC));
		}
		
		//之后每次只同步增量
		if (bRequestedPages)
		{
			UpdateSlideThumbToAllUser(strPPTFileName, nSlideCount);
		}
		
		m_mapSlideCrcToFile.clear();
		map<int, DWORD>::iterator itrCrc= m_mapSlideCRCCodeNow.begin();
		for (; itrCrc!=m_mapSlideCRCCodeNow.end(); itrCrc++)
		{
			m_mapSlideCrcToFile.insert(make_pair(itrCrc->second, itrCrc->first));
		}
		
		LeaveCriticalSection(&m_lockSlideCRC);
	}
}

void CPPTInfoSender::GetRqstedPages(set<int> &setAllPagesRqst)
{
	CUserManager* pUserManager = UserManager::GetInstance();
	map<DWORD, CUser*> mapUsers = pUserManager->GetUsers();

	//逐个用户同步
	map<DWORD, CUser*>::iterator itr;
	for(itr = mapUsers.begin(); itr != mapUsers.end(); itr++)
	{
		CUser* pUser = itr->second;
		if( !pUser->IsOnline() )
			continue;
		
		set<int> setSlidesOfUser = pUser->GetUsrOwnSlidePages();

		setAllPagesRqst.insert(setSlidesOfUser.begin(), setSlidesOfUser.end());
	}
}

void CPPTInfoSender::UpdateSlideThumbToAllUser(const string &strPPTFileName, int iSlideCnt)
{
	CUserManager* pUserManager = UserManager::GetInstance();
	map<DWORD, CUser*> mapUsers = pUserManager->GetUsers();

	//逐个用户同步
	map<DWORD, CUser*>::iterator itr;
	for(itr = mapUsers.begin(); itr != mapUsers.end(); itr++)
	{
		CUser* pUser = itr->second;
		if( !pUser->IsOnline() )
			continue;
		DWORD dSocket = pUser->GetSocket();
		DWORD dConnType = pUser->GetConnectType();
		set<int> setSlidesOfUser = pUser->GetUsrOwnSlidePages();
		if (setSlidesOfUser.size() == 0)
		{
			continue;
		}

		//发送增量开始包
		SendSlideUpdateStart(dSocket, dConnType);
		//发送增量更新页数
		SendSlideUpdatePageCnt(dSocket, dConnType, iSlideCnt);
		
		//逐页更新
		set<int>::iterator itrPage = setSlidesOfUser.begin();
		for (;itrPage!=setSlidesOfUser.end(); itrPage++)
		{
			int iIdx = *itrPage;
			map<int, DWORD>::iterator itrNow = m_mapSlideCRCCodeNow.find(iIdx);
			if (itrNow == m_mapSlideCRCCodeNow.end())
			{
				WRITE_LOG_LOCAL("CPPTInfoSender::UpdateSlideThumbToAllUser, have not gen new slide images' crccode! impossible to get here!!");
				return ;
			}
			DWORD dNowCRC = itrNow->second;

			map<DWORD, int>::iterator itrOldCrcToFile = m_mapSlideCrcToFile.find(dNowCRC);
			if (itrOldCrcToFile==m_mapSlideCrcToFile.end() || itrOldCrcToFile->second>iSlideCnt)
			{
				WRITE_LOG_LOCAL("Increment update, send new image, newid:%d", iIdx);
				SendSlideUpdateNew(dSocket, dConnType, iIdx, 0);
//				SendSlideThumbOfFile(dSocket, dConnType, strPPTFileName, iIdx);
			}
			else
			{
				WRITE_LOG_LOCAL("Increment update, send update image, newid:%d, oldid:%d", iIdx, itrOldCrcToFile->second);
				SendSlideUpdateNew(dSocket, dConnType, iIdx, itrOldCrcToFile->second);
			}
		}

		//更新结束
		SendSLideUpdateEnd(dSocket, dConnType);
	}
}


//---------------------------------------------------------------
// event handler
//
bool CPPTInfoSender::OnSyncPPTInfo(void *param)
{
	TEventNotify* pNotify = (TEventNotify*)param;

	//
	switch( pNotify->wParam )
	{
	case PPT_EDIT_OPENFILE:
	case PPT_EDIT_NEWFILE:
		{
			// send informations to all users
			SendBasicInfoToAllUsers();
			//SendPPTOutLineToAllUsers();

			// export to thumbnails
			m_nStartThumbnailIdx = -1;
			m_nEndThumbnailIdx = -1;

			// if there are any user connected then export thumbnails 
			if( IsAnyUserOnline() )
			{
				EnterCriticalSection(&m_lockSlideCRC);
				m_mapSlideCrcToFile.clear();
				ClearUserSlidePageData();
				LeaveCriticalSection(&m_lockSlideCRC);
				SetEvent(m_hEvent);
			}
			else
				m_bThumbnailDirty = TRUE;

			//设置状态
			CPPTController* pController = GetPPTController();
			if(pController == NULL )
				break;
			if(CPPTController::m_bNewByPPTControl==TRUE)
			{
				CPPTController::m_bNewByPPTControl=FALSE;
				pController->SetSaved(TRUE);
			}

			// 打开新PPT后，需要发送一次当前页号给移动端
			int nSlideIndex = pController->GetActiveSlideIndex();
			if(nSlideIndex >= 0)
				SendPreviewSlideIdxToAllUser(nSlideIndex);
			else
				WRITE_LOG_LOCAL("PPT_EDIT_NEWFILE SlideIndex Error, SlideIndex=%d", nSlideIndex);

		}
		break;

	case PPT_EDIT_INSERT_SLIDE:
		{

		}
		break;

	case PPT_EDIT_DELETE_SLIDE:
		break;

	case PPT_EDIT_CHANGE_SLIDE:
		break;

	case PPT_PLAY_BEGIN:
		{
			m_bPlaying = TRUE;
			SendPlayBeginToAllUsers();
		}
		break;

	case PPT_PLAY_SLIDE:
		SendPlaySlideToAllUsers(pNotify->lParam);
		break;

	case PPT_PLAY_END:
		{
			m_bPlaying = FALSE;
			SendPlayExitToAllUsers();
		}
		break;
	case PPT_PREVIEW_CHANGE:
		{
			int iSlideIdx = pNotify->lParam;
			SendPreviewSlideIdxToAllUser(iSlideIdx);
		}
		break;

	}
	

	return true;
}

void CPPTInfoSender::ClearUserSlidePageData()
{
	CUserManager* pUserManager = UserManager::GetInstance();
	map<DWORD, CUser*> mapUsers = pUserManager->GetUsers();

	map<DWORD, CUser*>::iterator itr;
	for(itr = mapUsers.begin(); itr != mapUsers.end(); itr++)
	{
		CUser* pUser = itr->second;
		if( pUser->IsOnline() )
		{
			pUser->ClearUserOwnSlidePage();
			return ;
		}
	}

	return ;
}

//---------------------------------------------------------------
// send packet
//
BOOL CPPTInfoSender::SendBasicInfo(SOCKET sock, int nConnectType)
{
	CPPTShellApp* pApp = (CPPTShellApp*)AfxGetApp();
	CPPTController* pController = GetPPTController();

	if( pApp == NULL || pController == NULL )
		return FALSE;

	string strPPTFileName = pController->GetFileName();
	if( strPPTFileName == _T("") )
		return FALSE;

	// send PPT basic information
	CPacketStream PPTInfoStream;
	PPTInfoStream.PackPushPPTBasicInfo(strPPTFileName.c_str(), pController->GetSlideCount());
	return pApp->SendStreamPacket(sock, &PPTInfoStream, nConnectType);
}

BOOL CPPTInfoSender::SendPPTOutLine(SOCKET sock, int nConnectType)
{
	CPPTShellApp* pApp = (CPPTShellApp*)AfxGetApp();
	CPPTController* pController = GetPPTController();

	if( pApp == NULL || pController == NULL )
		return FALSE;

	char OutLineBuffer[8192];
	int nStartIdx = 0;

	vector<string> vecOutline;
	vecOutline.clear();
	pController->GetOutLine(vecOutline, "幻灯片");

	int nTotalPackedCount = 0;
	while( nTotalPackedCount < (int)vecOutline.size() )
	{
		int nPackedCount = 0;

		CPacketStream PPTOutLineStream(OutLineBuffer, sizeof(OutLineBuffer));
		PPTOutLineStream.PackPushOutlineInfo(vecOutline, nStartIdx+nPackedCount, nPackedCount);
		BOOL res = pApp->SendStreamPacket(sock, &PPTOutLineStream, nConnectType);
		if( !res )
			return FALSE;

		nTotalPackedCount += nPackedCount;
	}

	return TRUE;
}

BOOL CPPTInfoSender::SendSlideThumb(SOCKET sock, int nConnectType, int nStartIdx, int nEndIdx)
{
	CPPTController* pController = GetPPTController();
	if( pController == NULL )
		return FALSE;

	string strPPTFileName = pController->GetFileName();
 
// 	TCHAR szFolderPath[MAX_PATH];
// 	_stprintf_s(szFolderPath, _T("%s\\Cache\\PPTSlides\\%s"), GetLocalPath().c_str(), strPPTFileName.c_str());

	if( nStartIdx == -1 || nEndIdx == -1 )
	{
		int nSlideCount = pController->GetSlideCount();
		if( nSlideCount == 0 )
			return FALSE;
		nStartIdx = 1;
		nEndIdx = nSlideCount;
	}
	CUser *pUser = GetUserBySock(sock);
	// send these slide thumb
//	TCHAR szPath[MAX_PATH];
	for(int i = nStartIdx; i <= nEndIdx; i++)
	{
		SendSlideThumbOfFile(sock, nConnectType, strPPTFileName, i);
		if (pUser != NULL)
		{
			pUser->AddUsrOwnSlidePage(i);
		}
// 		_stprintf_s(szPath, _T("%s\\Slide_%d.png"), szFolderPath, i);
// 		FileTransferManager::GetInstance()->SendFile(szPath, sock, nConnectType, FILE_TYPE_SLIDE, i);
	}

	return TRUE;
}

BOOL CPPTInfoSender::SendSlideThumbList(SOCKET sock, int nConnectType, const vector<int> &vecSlideIdxs)
{
	CPPTController* pController = GetPPTController();
	if( pController == NULL )
		return FALSE;

	string strPPTFileName = pController->GetFileName();

	CUser *pUser = GetUserBySock(sock);
	
	EnterCriticalSection(&m_lockSlideCRC);
	for(unsigned int i = 0; i <vecSlideIdxs.size(); i++)
	{
		int iIdx = vecSlideIdxs[i];
		if (m_mapSlideCRCCodeNow.find(iIdx) == m_mapSlideCRCCodeNow.end())
		{
			pController->ExportToImages(m_strPPTFilePath.c_str(), "png", iIdx, iIdx);
			TCHAR szPath[MAX_PATH];
			_stprintf_s(szPath, _T("%s\\Slide_%d.png"), m_strPPTFilePath.c_str(), iIdx);
			DWORD crc = CalcFileCRC(szPath);
			m_mapSlideCRCCodeNow.insert(make_pair(iIdx, crc));
		}
		SendSlideThumbOfFile(sock, nConnectType, strPPTFileName, iIdx);
		if (pUser != NULL)
		{
			pUser->AddUsrOwnSlidePage(iIdx);
		}
	}
	LeaveCriticalSection(&m_lockSlideCRC);

	return TRUE;
}

BOOL CPPTInfoSender::SendSlideThumbOfFile(SOCKET sock, int nConnectType, const string& strPPTFileName, int iSlideIdx)
{
	static string strPath = GetLocalPath();
	TCHAR szPath[MAX_PATH];
	_stprintf_s(szPath, _T("%s\\Cache\\PPTSlides\\%s\\Slide_%d.png"), strPath.c_str(), strPPTFileName.c_str(), iSlideIdx);
	FileTransferManager::GetInstance()->SendFile(szPath, sock, nConnectType, FILE_TYPE_SLIDE, iSlideIdx);
	WRITE_LOG_LOCAL("Socket:%d, send slide image, index:%d", sock, iSlideIdx);
	return TRUE;
}


BOOL CPPTInfoSender::SendPlayBegin(SOCKET sock, int nConnectType)
{
	if( !m_bPlaying )
		return FALSE;

	CPPTShellApp* pApp = (CPPTShellApp*)AfxGetApp();

	CPacketStream PPTInfoStream;
	PPTInfoStream.PackPushPlayBegin();
	WRITE_LOG_LOCAL("----SendPlayBegin to user:%d", sock);
	return pApp->SendStreamPacket(sock, &PPTInfoStream, nConnectType);
}

BOOL CPPTInfoSender::SendPlaySlide(SOCKET sock, int nConnectType, int nSlideIdx /*= -1*/)
{
	CPPTShellApp* pApp = (CPPTShellApp*)AfxGetApp();

	if( nSlideIdx == -1 )
	{
		CPPTController* pController = GetPPTController();
		
		if ( pController->IsPPTShowViewActive() )
			nSlideIdx = pController->GetShowViewIndex();
		else
			return TRUE;

		if( nSlideIdx == 0 )
			return TRUE;
	}

	CPacketStream PPTInfoStream;
	PPTInfoStream.PackPushPlaySlide(nSlideIdx);
	return pApp->SendStreamPacket(sock, &PPTInfoStream, nConnectType);

}

BOOL CPPTInfoSender::SendPlayEnd(SOCKET sock, int nConnectType)
{
	CPPTShellApp* pApp = (CPPTShellApp*)AfxGetApp();

	CPacketStream PPTInfoStream;
	PPTInfoStream.PackPushPlayEnd();
	return pApp->SendStreamPacket(sock, &PPTInfoStream, nConnectType);
}

BOOL CPPTInfoSender::SendVideoList(SOCKET sock, int nConnectType, int nSlideIdx, vector<float> vecVideoList)
{
	CPPTShellApp* pApp = (CPPTShellApp*)AfxGetApp();

	CPacketStream VideoStream;
	VideoStream.PackPushVideoList(nSlideIdx, vecVideoList);
	return pApp->SendStreamPacket(sock, &VideoStream, nConnectType);
}

BOOL CPPTInfoSender::SendSlideUpdateStart(SOCKET sock, int nConnectType)
{
	CPPTShellApp* pApp = (CPPTShellApp*)AfxGetApp();

	CPacketStream pkgStream;
	pkgStream.PackPushSlideUpdateStart();
	return pApp->SendStreamPacket(sock, &pkgStream, nConnectType);
	WRITE_LOG_LOCAL("socket:%d, Increment update, send begin", sock);
}

BOOL CPPTInfoSender::SendSlideUpdatePageCnt(SOCKET sock, int nConnectType, int iPageCnt)
{
	CPPTShellApp* pApp = (CPPTShellApp*)AfxGetApp();

	CPacketStream pkgStream;
	pkgStream.PackPushSlideUpdatePageCnt(iPageCnt);
	return pApp->SendStreamPacket(sock, &pkgStream, nConnectType);
	WRITE_LOG_LOCAL("socket:%d, Increment update, send page count:%d", sock, iPageCnt);
}

BOOL CPPTInfoSender::SendSlideUpdateNew(SOCKET sock, int nConnectType, int iNewID, int iOldID)
{
	CPPTShellApp* pApp = (CPPTShellApp*)AfxGetApp();

	CPacketStream pkgStream;
	pkgStream.PackPushSlideUpdateNew(iNewID, iOldID);
	return pApp->SendStreamPacket(sock, &pkgStream, nConnectType);
	WRITE_LOG_LOCAL("socket:%d, Increment update, send replace image, newid:%d, oldid:%d", sock, iNewID, iOldID);
}

BOOL CPPTInfoSender::SendSLideUpdateEnd(SOCKET sock, int nConnectType)
{
	CPPTShellApp* pApp = (CPPTShellApp*)AfxGetApp();

	CPacketStream pkgStream;
	pkgStream.PackPushSlideUpdateEnd();
	return pApp->SendStreamPacket(sock, &pkgStream, nConnectType);
	WRITE_LOG_LOCAL("socket:%d, Increment update, send end", sock);
}

BOOL CPPTInfoSender::SendSyncDataPPTChanged(SOCKET sock, int nConnectType)
{
	CPPTShellApp* pApp = (CPPTShellApp*)AfxGetApp();

	CPacketStream pkgStream;
	pkgStream.PackSyncDataPPTChanged();
	m_bSyncDataPPTChangedAck = false;
	return pApp->SendStreamPacket(sock, &pkgStream, nConnectType);
}

BOOL CPPTInfoSender::SendPreviewSlideIdx(SOCKET sock, int nConnectType, int iSlideIdx)
{
	if (iSlideIdx < 0 || m_bPlaying)
	{
		return FALSE;
	}
	CPPTShellApp* pApp = (CPPTShellApp*)AfxGetApp();

	CPacketStream pkgStream;
	pkgStream.PackPushPreviewSlideIdx(iSlideIdx);
	return pApp->SendStreamPacket(sock, &pkgStream, nConnectType);
}
//---------------------------------------------------------------
// send informations to all users
//
BOOL CPPTInfoSender::SendBasicInfoToAllUsers()
{
	CUserManager* pUserManager = UserManager::GetInstance();
	map<DWORD, CUser*> mapUsers = pUserManager->GetUsers();

	map<DWORD, CUser*>::iterator itr;
	for(itr = mapUsers.begin(); itr != mapUsers.end(); itr++)
	{
		CUser* pUser = itr->second;
		if( !pUser->IsOnline() )
			continue;

		SendBasicInfo(pUser->GetSocket(), pUser->GetConnectType());
	}

	return TRUE;
}

BOOL CPPTInfoSender::SendPPTOutLineToAllUsers()
{
	CUserManager* pUserManager = UserManager::GetInstance();
	map<DWORD, CUser*> mapUsers = pUserManager->GetUsers();

	map<DWORD, CUser*>::iterator itr;
	for(itr = mapUsers.begin(); itr != mapUsers.end(); itr++)
	{
		CUser* pUser = itr->second;
		if( !pUser->IsOnline() )
			continue;

	}

	return TRUE;
}

BOOL CPPTInfoSender::SendSlideThumbToAllUsers(int nStartSlideIdx /*= -1*/, int nEndSlideIdx /*= -1*/)
{
	CUserManager* pUserManager = UserManager::GetInstance();
	map<DWORD, CUser*> mapUsers = pUserManager->GetUsers();

	map<DWORD, CUser*>::iterator itr;
	for(itr = mapUsers.begin(); itr != mapUsers.end(); itr++)
	{
		CUser* pUser = itr->second;
		if( !pUser->IsOnline() )
			continue;

		SendSlideThumb(pUser->GetSocket(), pUser->GetConnectType(), nStartSlideIdx, nEndSlideIdx);
	}
	return TRUE;
}

BOOL CPPTInfoSender::SendPlayBeginToAllUsers()
{
	CUserManager* pUserManager = UserManager::GetInstance();
	map<DWORD, CUser*> mapUsers = pUserManager->GetUsers();
	
	bool bCheckChanged = false;
	bool bChanged = false;
	map<DWORD, CUser*>::iterator itr;
	for(itr = mapUsers.begin(); itr != mapUsers.end(); itr++)
	{
		CUser* pUser = itr->second;
		if( !pUser->IsOnline() )
			continue;
		
		SendPlayBegin(pUser->GetSocket(), pUser->GetConnectType());
		WRITE_LOG_LOCAL("CPPTInfoSender::SendPlayBeginToAllUsers");
		
		if (!bCheckChanged)
		{
			CPPTController* pController = GetPPTController();
			if( pController != NULL )
			{
				bChanged = pController->IsChanged(FALSE);
			}
			bCheckChanged = true;
		}

		if (bChanged)
		{
			SendSyncDataPPTChanged(pUser->GetSocket(), pUser->GetConnectType());
		}

	}

	return TRUE;
}

BOOL CPPTInfoSender::SendPlaySlideToAllUsers(int nSlideIdx)
{
	CUserManager* pUserManager = UserManager::GetInstance();
	map<DWORD, CUser*> mapUsers = pUserManager->GetUsers();

	map<DWORD, CUser*>::iterator itr;
	for(itr = mapUsers.begin(); itr != mapUsers.end(); itr++)
	{
		CUser* pUser = itr->second;
		if( !pUser->IsOnline() )
			continue;

		SendPlaySlide(pUser->GetSocket(), pUser->GetConnectType(), nSlideIdx);
	}
	
	return TRUE;
}

BOOL CPPTInfoSender::SendPlayExitToAllUsers()
{
	CUserManager* pUserManager = UserManager::GetInstance();
	map<DWORD, CUser*> mapUsers = pUserManager->GetUsers();

	map<DWORD, CUser*>::iterator itr;
	for(itr = mapUsers.begin(); itr != mapUsers.end(); itr++)
	{
		CUser* pUser = itr->second;
		if( !pUser->IsOnline() )
			continue;

		SendPlayEnd(pUser->GetSocket(), pUser->GetConnectType());
	}

	return TRUE;
}

BOOL CPPTInfoSender::SendVideoListToAllUsers(int nSlideIdx, vector<float> vecVideoList)
{
	if( vecVideoList.size() == 0 )
		return FALSE;

	CUserManager* pUserManager = UserManager::GetInstance();
	map<DWORD, CUser*> mapUsers = pUserManager->GetUsers();

	map<DWORD, CUser*>::iterator itr;
	for(itr = mapUsers.begin(); itr != mapUsers.end(); itr++)
	{
		CUser* pUser = itr->second;
		if( !pUser->IsOnline() )
			continue;

		SendVideoList(pUser->GetSocket(), pUser->GetConnectType(), nSlideIdx, vecVideoList);
	}

	return TRUE;
}

BOOL CPPTInfoSender::SendPreviewSlideIdxToAllUser(int iSlideIdx)
{
	if (iSlideIdx<0 || m_bPlaying)
	{
		return FALSE;
	}

	CUserManager* pUserManager = UserManager::GetInstance();
	map<DWORD, CUser*> mapUsers = pUserManager->GetUsers();

	map<DWORD, CUser*>::iterator itr;
	for(itr = mapUsers.begin(); itr != mapUsers.end(); itr++)
	{
		CUser* pUser = itr->second;
		if( !pUser->IsOnline() )
			continue;

		SendPreviewSlideIdx(pUser->GetSocket(), pUser->GetConnectType(), iSlideIdx);
	}

	return TRUE;
}

BOOL CPPTInfoSender::IsAnyUserOnline()
{
	CUserManager* pUserManager = UserManager::GetInstance();
	map<DWORD, CUser*> mapUsers = pUserManager->GetUsers();

	map<DWORD, CUser*>::iterator itr;
	for(itr = mapUsers.begin(); itr != mapUsers.end(); itr++)
	{
		CUser* pUser = itr->second;
		if( pUser->IsOnline() )
			return TRUE;
	}

	return FALSE;
}

bool CPPTInfoSender::IsSyncDataPPTChangedAck()
{
	return m_bSyncDataPPTChangedAck;
}