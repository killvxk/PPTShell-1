//===========================================================================
// FileName:				PacketProcessor.cpp
//	
// Desc:					 process packets
//============================================================================
#include "stdafx.h"
#include "PPTShell.h"
#include "Util/Util.h"
#include "Util/Stream.h"
#include "Util/Singleton.h"
#include "PacketStream.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "PPTControl/PPTInfoSender.h"
#include "FileTransfer/FileTransferTask.h"
#include "FileTransfer/FileTransferManager.h"
#include "PacketProcessor.h"
#include "DUI/LaserPointerDlg.h"
#include "DUI/ImageCtrlDialogUI.h"
#include "NDCloud/NDCloudUser.h"
#include "gui/MainFrm.h" 
#include "DUI/MobilePenView.h" 
#include "DUI/MagicPen_Main.h"
#include "DUI/BlackBoarder.h"

#define DIRECTORY_MOBILE_RECV_FILE			_T("RecvFiles")
void GenReceiveFilePath(const tstring& strFileName, tstring &strFullName);

CPacketProcessor::CPacketProcessor()
{ 
	m_pNewLineShape = NULL;
}

CPacketProcessor::~CPacketProcessor()
{
	m_bCloseApp = TRUE;

	SetEvent(m_hEvent);
	if(m_hProcessThread != NULL)
		WaitForSingleObject(m_hProcessThread, 5000);

	DWORD dwExitCode;
	if(GetExitCodeThread(m_hProcessThread, &dwExitCode))
	{
		if(dwExitCode == STILL_ACTIVE)
			TerminateThread(m_hProcessThread, 0);
	}

	CloseHandle(m_hEvent); 
}

void CPacketProcessor::Initialize()
{
	m_bCloseApp = FALSE;
	m_hProcessThread = NULL;

	InitializeCriticalSection(&m_cs);
	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	m_hProcessThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ProcessPacketThread, this, 0, NULL);
}

void CPacketProcessor::Destroy()
{

}

void CPacketProcessor::OnProcessPacket(SOCKET sock, int connectType, char *pPacket)
{
	PACKET_HEAD* pHeader = (PACKET_HEAD*)pPacket;
	if( pHeader->dwSize > 0xffff )
		return;
	

	// add to process deque
	SOCK_PACKET* p = new SOCK_PACKET;

	p->sock = sock;
	p->type = connectType;
	p->buffer = new char[pHeader->dwSize+1];

	memset(p->buffer, 0, pHeader->dwSize+1);
	memcpy(p->buffer, pPacket, pHeader->dwSize);


	EnterCriticalSection(&m_cs);
	m_ProcessDeque.push_back(p);
	LeaveCriticalSection(&m_cs);

	SetEvent(m_hEvent);
}

void CPacketProcessor::OnSocketClosed(SOCKET sock)
{
	// notify leader
	CPPTShellApp* pApp = (CPPTShellApp*)AfxGetApp();
	CUserManager* pUserManager = UserManager::GetInstance();

	if( pApp == NULL || pUserManager == NULL )
		return;
	
	CUser* pUser = pUserManager->FindUser(sock);
	if( pUser == NULL )
		return;

	CUser* pLeader = pUserManager->GetLeader();
	if( pLeader == NULL || pUser == pLeader || !pLeader->IsOnline() )
		return;

	CPacketStream GuestLogoffStream;
	GuestLogoffStream.PackGuestLogoffResponse(pUser->GetUid());
	pApp->SendStreamPacket(pLeader->GetSocket(), &GuestLogoffStream, pLeader->GetConnectType());
	
	pUserManager->UserLogoff(pUser->GetUid());
}

void CPacketProcessor::ProcessPacket()
{
	while( TRUE )
	{
		DWORD ret = WaitForSingleObject(m_hEvent, INFINITE);
		if(m_bCloseApp)
			return;
		if( ret != 0 )
			continue;

		deque<SOCK_PACKET*> packetDeque;

		EnterCriticalSection(&m_cs);
		m_ProcessDeque.swap(packetDeque);
		LeaveCriticalSection(&m_cs);


		deque<SOCK_PACKET*>::iterator itr;
		for(itr = packetDeque.begin(); itr != packetDeque.end(); itr++)
		{
			SOCK_PACKET* packet = *itr;
			ProcessOnePacket(packet);

			if( packet->buffer != NULL )
			{
				delete packet->buffer;
				packet->buffer = NULL;
			}

			if( packet != NULL )
			{
				delete packet;
				packet = NULL;
			}
		}
		
		time_t tmStart = time(NULL);
		while(true)
		{
			if(m_bCloseApp)
				return;

			//180秒内如果有消息就转去处理
			if (m_ProcessDeque.size()>0)
			{
				break;
			}

			//180秒内如果还没有消息就挂起
			if (time(NULL)-tmStart>180)
			{
				break;
			}

			Sleep(1);
		}
	}
}

DWORD WINAPI CPacketProcessor::ProcessPacketThread(LPARAM lParam)
{
	CPacketProcessor* pThis = (CPacketProcessor*)lParam;
	if( pThis == NULL )
		return 0;

	pThis->ProcessPacket();

	return 1;
}

//
// process one packet
//
void CPacketProcessor::ProcessOnePacket(SOCK_PACKET* pPacket)
{
	PACKET_HEAD* pHeader = (PACKET_HEAD*)pPacket->buffer;
	if( pHeader == NULL )
		return;
 
	switch( pHeader->dwMajorType )
	{
	case ACTION_USER:
		ProcessActionUser(pPacket);
		break;

	case ACTION_CONTROL:
		ProcessActionControl(pPacket);
		break;

	case ACTION_PUSH_DATA:
		ProcessActionPushData(pPacket);
		break;

	case ACTION_PULL_DATA:
		ProcessActionPullData(pPacket);
		break;

	case ACTION_SYNC_DATA:
		ProcessActionSyncData(pPacket);
		break;

	case ACTION_TRANSFER_FILE:
		ProcessActionTransferFile(pPacket);
		break;
	case  MOBILE_HEARTBEAT:
		WRITE_LOG_LOCAL(_T("Receive a heartbeat package from 【%d】(1:ios, 2:android)"), pHeader->dwMinorType);
		break;
	default:
		WRITE_LOG_LOCAL(_T("Recieve a unrecognize package! majortype:%d"), pHeader->dwMajorType);
		break;
	}
}

//
// process user action
//
void CPacketProcessor::ProcessActionUser(SOCK_PACKET* pSockPacket)
{
	CPPTShellApp* pApp = (CPPTShellApp*)AfxGetApp();
	CUserManager* pUserManager = UserManager::GetInstance();

	if( pApp == NULL || pUserManager == NULL )
		return;

	PACKET_HEAD* pHeader = (PACKET_HEAD*)pSockPacket->buffer;
	if( pHeader == NULL )
		return;

	switch( pHeader->dwMinorType )
	{
	case USER_LOGON:
		{
			DWORD dwUID;
			tstring strUserName;
			BOOL bMobileLogin;

			CPacketStream LogonStream(pSockPacket->buffer);
			LogonStream.UnPackUserLogon(dwUID, strUserName, bMobileLogin);

			CUser* pUser = pUserManager->UserLogin(pSockPacket->sock, dwUID, pSockPacket->type, (TCHAR *)strUserName.c_str());
			if( pUser == NULL )
				return;

			WRITE_LOG_LOCAL(_T("UserLogin sock:%u, uid:%u, username:%s"), pSockPacket->sock, dwUID, strUserName.c_str());

			// send response to this user
			CPacketStream LogonResponseStream;
			LogonResponseStream.PackUserLogonResponse(pUser->IsLeader(), pUser->GetPermission());
			pApp->SendStreamPacket(pSockPacket->sock, &LogonResponseStream, pSockPacket->type);

			// send pc information to this user
			int nScreenWidth, nScreenHeight;
			GetScreenResolution(nScreenWidth, nScreenHeight);
			// 获取Config.ini的本机班级名称，如果为空则获取计算机名
			CMainFrame* pMainFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
			tstring strNodeName = pMainFrame->GetConfigClassName();
			if(strNodeName.empty())
				strNodeName = GetPCName();

			CPacketStream PCInfoStream;
			PCInfoStream.PackPushPCInfo(strNodeName, nScreenWidth, nScreenHeight);
			pApp->SendStreamPacket(pSockPacket->sock, &PCInfoStream, pSockPacket->type);

			// send PPT basic information and outline and slide images
			CPPTInfoSender* pPPTInfoSender = PPTInfoSender::GetInstance();
			pPPTInfoSender->SendBasicInfo(pSockPacket->sock, pSockPacket->type);
//			pPPTInfoSender->SendPPTOutLine(pSockPacket->sock, pSockPacket->type);
//			pPPTInfoSender->SendSlideThumb(pSockPacket->sock, pSockPacket->type);
			pPPTInfoSender->SendPlayBegin(pSockPacket->sock, pSockPacket->type);
			pPPTInfoSender->SendPlaySlide(pSockPacket->sock, pSockPacket->type);

			pPPTInfoSender->StartExportThumbnails();		//实际上只有第一次登陆才会触发生成缩略图
			
			CPPTController* pController = GetPPTController();
			if( pController != NULL )
			{
				BOOL bRet = pController->IsChanged(FALSE);
				if (bRet)
				{
					pPPTInfoSender->SendSyncDataPPTChanged(pSockPacket->sock, pSockPacket->type);
					WRITE_LOG_LOCAL(_T("user logon, find ppt is changed, send notice to user...."));
//					pController->Save();
// 					CStream* pTmpStream	= new CStream(50);
// 					pTmpStream->WriteDWORD(PPTC_SYNC_CHANGE);
// 					BroadcastPostedEvent(EVT_THREAD_CONTROL_PPT, (WPARAM)pTmpStream, 0, NULL);
				}

				if (!pController->IsPPTShowViewActive())
				{
					int iCurrSlideIdx = pController->GetActiveSlideIndex();
					pPPTInfoSender->SendPreviewSlideIdx(pSockPacket->sock, pSockPacket->type, iCurrSlideIdx);
				}
				if(pController->IfPPTSlideShowView())
				{
					// 如果正在放映PPT，同步视频信息给手机端
					GetVideoListByThread();
				}
			}

			//手机自动登录
			if (bMobileLogin)
			{
				time_t tNow = time(NULL);
				CNDCloudUser *pCloudUser = NDCloudUser::GetInstance();
				if (pCloudUser && !pCloudUser->IsLogin() && (tNow-pCloudUser->GetLastMobileLoginRqstTime())>=60)
				{
					CPacketStream pkgStream;
					pkgStream.PackCommand(ACTION_USER, NDUSER_REQUEST_LOGON_INFO);
					pApp->SendStreamPacket(pSockPacket->sock, &pkgStream, pSockPacket->type);
					pCloudUser->SetMobileLoginRqstTime(tNow);
				}
			}

// 			CStream* pTmpStream	= new CStream(10);
// 			BroadcastPostedEvent(EVT_MOBILE_LOGIN, (WPARAM)pTmpStream, 0, NULL);

			// leader
			CUser* pLeader = pUserManager->GetLeader();
			if( pLeader == NULL )
				return;

			// send all the other user informations to leader
			if( pUser == pLeader )
			{
				map<DWORD, CUser*> mapUsers = pUserManager->GetUsers();
				map<DWORD, CUser*>::iterator itr;

				for(itr = mapUsers.begin(); itr != mapUsers.end(); itr++)
				{
					pUser = itr->second;
					if( pUser != NULL && pUser != pLeader && pUser->IsOnline() )
					{
						// guest logon packet
						CPacketStream GuestLogonStream;
						GuestLogonStream.PackGuestLogonResponse(pUser->GetUid(), pUser->GetPermission(), pUser->GetName());
						pApp->SendStreamPacket(pLeader->GetSocket(), &GuestLogonStream, pLeader->GetConnectType());
					}
				}
			}
			else
			{
				// send to leader
				if( !pLeader->IsOnline() )
					return;

				CPacketStream GuestLogonStream;
				GuestLogonStream.PackGuestLogonResponse(pUser->GetUid(), pUser->GetPermission(), pUser->GetName());
				pApp->SendStreamPacket(pLeader->GetSocket(), &GuestLogonStream, pLeader->GetConnectType());
			}

		}
		break;

	case USER_LOGOFF:
		{
			
		}
		break;

	case USER_MODIFY_NAME:
		{
			DWORD dwUID;
			tstring strName;

			CPacketStream ModifyNameStream(pSockPacket->buffer);
			ModifyNameStream.UnPackModifyName(dwUID, strName);

			CUser* pUser = pUserManager->FindUser(dwUID);
			if( pUser == NULL )
				return;

			pUser->SetName((TCHAR *)strName.c_str());

			// send user info to leader
			CUser* pLeader = pUserManager->GetLeader();
			if( pLeader != NULL && pLeader != pUser && pLeader->IsOnline() )
			{
				CPacketStream GuestModifyNameStream;
				GuestModifyNameStream.PackGuestModifyName(dwUID, strName);

				pApp->SendStreamPacket(pLeader->GetSocket(), &GuestModifyNameStream, pLeader->GetConnectType());
			}
		}
		break;

	case USER_MODIFY_PERMISSION:
		{ 
			DWORD dwUID;
			DWORD dwPermission;

			CPacketStream ModifyPermissionStream(pSockPacket->buffer);
			ModifyPermissionStream.UnPackModifyPermission(dwUID, dwPermission);

			CUser* pUser = pUserManager->FindUser(dwUID);
			if( pUser == NULL )
				return;

			pUser->SetPermission(dwPermission);

			// send user info 
			if( pUser->IsOnline() )
				pApp->SendStreamPacket(pUser->GetSocket(), &ModifyPermissionStream, pUser->GetConnectType());
		}
		break;
	case NDUSER_LOGON_INFO:
		{
			char *pData = (char *)pHeader+sizeof(PACKET_HEAD);
			CNDCloudUser *pCloudUser = NDCloudUser::GetInstance();
			if (pCloudUser==NULL || pCloudUser->IsLogin())
			{
				WRITE_LOG_LOCAL(_T("Reveive dumplicate NDUSER_LOGON_INFO!"));
				return;
			}

			int iLen = pHeader->dwSize - sizeof(PACKET_HEAD);
			CStream* pTmpStream	= new CStream(iLen+10);
			pTmpStream->Write(pData, iLen);
			BroadcastPostedEvent(EVT_MOBILE_LOGIN, (WPARAM)pTmpStream, 0, NULL);
		}
		break;
	}
}

//
// process control action
//
void CPacketProcessor::ProcessActionControl(SOCK_PACKET* pSockPacket)
{
	PACKET_HEAD* pHeader = (PACKET_HEAD*)pSockPacket->buffer;
	if( pHeader == NULL )
		return;

	CPPTShellApp* pApp = (CPPTShellApp*)AfxGetApp();
	CMainFrame* pMainFrame = (CMainFrame*)pApp->GetMainWnd();
	if(pMainFrame!=NULL && pMainFrame->IsPPTOpening())
	{
		// 发送应答
		CPacketStream streamError;
		streamError.PackControlResponse(pHeader->dwMinorType, FALSE, _T("PC端正在打开PPT，无法处理指令"));
		pApp->SendStreamPacket(pSockPacket->sock, &streamError, pSockPacket->type);

		WRITE_LOG_LOCAL(_T("Recv PPTControl Action When PPT Opening, cmd is %d-%d"), pHeader->dwMajorType, pHeader->dwMinorType);
		return;
	}

	switch( pHeader->dwMinorType )
	{
	case CONTROL_M2P_PPT:
		PPTControl(pSockPacket);
		break;

	case CONTROL_M2P_LASER:
		LaserControl(pSockPacket, pHeader->dwSize);
		break;
	case CONTROL_M2P_BRUSH:
		BrushControl(pSockPacket, pHeader->dwSize);
		break;
	case CONTROL_M2P_3D:
		ThreeDControl(pSockPacket);
		break;

	case CONTROL_M2P_IMAGE:
		ImageControl(pSockPacket);
		break;

#ifdef DEVELOP_VERSION
	case CONTROL_M2P_WHITE_BOARD:
		BlackBoardControl(pSockPacket);
		break;
#endif

	case CONTROL_M2P_MAGNIFIER:
		MagnifierControl(pSockPacket);
		break;
#ifdef DEVELOP_VERSION
	case CONTROL_M2P_SPOTLIGHT:
		SpotlightControl(pSockPacket);
		break;
#endif
	}
}

void CPacketProcessor::LaserControl(SOCK_PACKET* pSockPacket, DWORD pkgLen)
{
	CPPTController* pController = GetPPTController();
	if (pController == NULL)
	{
		return ;
	}

	CPacketStream laserControlStream(pSockPacket->buffer);
	laserControlStream.SkipHeader();

	DWORD laserType = laserControlStream.ReadDWORD();
	switch(laserType)
	{
	case LASER_MOVE:
		{
			int iPointCnt = (pkgLen-sizeof(PACKET_HEAD))/8;
			if (iPointCnt<=0)
			{
				WRITE_LOG_LOCAL(_T("CPacketProcessor::LaserControl, invalid message!"));
				return ;
			}
			for (int i=0; i<iPointCnt; i++)
			{
				float ptX = laserControlStream.ReadFloat();
				float ptY = laserControlStream.ReadFloat();
				
				
				pController->SlidePosToScreenPos(ptX, ptY);
				clock_t tclk = clock();
				logToOutput(_T("CPacketProcessor::LaserControl,posX:%f, posY:%f----clock:%ld"), ptX, ptY, tclk);
				LaserPointerDlg::GetInstance()->Show((int)ptX, (int)ptY);
				tclk = clock();
				logToOutput(_T("CPacketProcessor::LaserControl--------------show done----clock:%ld"), tclk);
				SleepRunNull(6);
			}
		}
		break;
	case LASER_END:
		LaserPointerDlg::GetInstance()->Hide();
		break;
	default:
		break;
	}

}

void CPacketProcessor::BrushControl(SOCK_PACKET* pSockPacket, DWORD pkgLen)
{
	CPacketStream brushControlStream(pSockPacket->buffer);
	brushControlStream.SkipHeader();
	
	CPPTController* pController = GetPPTController();
	if (pController==NULL)
	{
		WRITE_LOG_LOCAL(_T("CPacketProcessor::BrushControl, GetPPTController fail!"));
		return ;
	}
	
	DWORD brushType = brushControlStream.ReadDWORD();

	if(!pController->IfPPTSlideShowView())
	{
		return ;
	}
	
	switch(brushType)
	{
	case BRUSH_COLOR:
		{
			DWORD color = brushControlStream.ReadDWORD();
			DWORD red = color&0x00ff0000;
			DWORD green = color&0x0000ff00;
			DWORD blue = color&0x000000ff;
			DWORD colorDest = (blue<<16) | green | (red>>16);
			pController->SetPenColor(colorDest);
		}
		break;
	case BRUSH_STYLE:
		break;
	case BRUSH_WIDTH:
		break;
	case BRUSH_MOVE:
		{

//			WRITE_LOG_LOCAL("--------------new BRUSH_MOVE---------------");
			int iDataLen = pkgLen-sizeof(PACKET_HEAD)-4;
			int iPointCnt = iDataLen/8;
			if ( iDataLen%8 != 0)
			{
				WRITE_LOG_LOCAL(_T("CPacketProcessor::BrushControl, MOVE Data error!"));
				return ;
			}	

			float fLastX = -1;
			float fLastY = -1;
			bool bSetMouseDown = false;
				
			for (int i=0; i<iPointCnt; i++)
			{
				float fCurrX = brushControlStream.ReadFloat();
				float fCurrY = brushControlStream.ReadFloat(); 
				//WRITE_LOG_LOCAL("mobile send[%d]:%f,%f\n",i, fCurrX, fCurrY);

				if (fCurrX==-2 || fCurrY==-2)
				{ 
					mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0); //这个暂时不能去掉，去掉会出现最后一个坐标没有绘制 2016.03.04 cws
 
					MobilePenViewUI::GetInstance()->AddShapes(m_pNewLineShape);
					break;
				}

				if (fCurrX==-1 || fCurrY ==-1)
				{ 
					m_pNewLineShape = NULL;					
					m_pNewLineShape = new CLineShape;

					DWORD dwCurrentColor = RGB(234,55,68);
					dwCurrentColor  &= 0x00FFFFFF;
					if(dwCurrentColor == 0)
					{
						dwCurrentColor = 0x1;
					}
					dwCurrentColor = RGB(GetBValue(dwCurrentColor), GetGValue(dwCurrentColor), GetRValue(dwCurrentColor));
					m_pNewLineShape->SetColor(dwCurrentColor);
					m_pNewLineShape->SetWidth(5);

					continue;
				}
 
				pController->SlidePosToScreenPos(fCurrX, fCurrY); 
			
				POINT ptMouse;
				ptMouse.x = (int)fCurrX;
				ptMouse.y = (int)fCurrY;
				//WRITE_LOG_LOCAL("mobile input:%d,%d\n", ptMouse.x,ptMouse.y);
				m_pNewLineShape->AddPoint(ptMouse); 
			}
		}
		break;
	case BRUSH_UNDO:
		{

		}
		break;
	case BRUSH_CLEAR:
		{
			MobilePenViewUI::GetInstance()->ClearPanel();
			//pController->PenEraseDrawing();
		}
		break;
	case BRUSH_BEGIN:
		{ 
			CScreenInstrumentUI* pScreenIns = CScreenInstrumentUI::GetMainInstrument(); 
			if(pScreenIns != NULL)
				PostMessage(pScreenIns->GetHWND(), WM_SHOWMOBILEPEN, 0,0);
		}
		break;
	case BRUSH_END:
		{ 
			CScreenInstrumentUI* pScreenIns = CScreenInstrumentUI::GetMainInstrument(); 
			if(pScreenIns != NULL)
				PostMessage(pScreenIns->GetHWND(), WM_HIDEMOBILEPEN, 0,0);
		}
		break;
	case BRUSH_SAVE:
		{
			HWND hdPrompt = ::FindWindow(_T("#32770"), _T("Microsoft PowerPoint"));
			if(hdPrompt!=NULL)
			{
				HWND hdSave = ::FindWindowEx(hdPrompt, NULL, _T("Button"), _T("保留(&K)"));
				PostMessage(hdSave, BM_CLICK, 0, 0);
			}
		}
		break;
	case  BRUSH_CANCEL_SAVE:
		{
			HWND hdPrompt = ::FindWindow(_T("#32770"), _T("Microsoft PowerPoint"));
			if (hdPrompt != NULL)
			{
				HWND hdSave = ::FindWindowEx(hdPrompt, NULL, _T("Button"), _T("放弃(&D)"));
				PostMessage(hdSave, BM_CLICK, 0, 0);
			}
		}
		break;
	default:
		break;
	}

}
void CPacketProcessor::MagnifierControl(SOCK_PACKET* pSockPacket)
{
	// PPT未放映时不使用放大镜
	CPPTController* pController = GetPPTController();
	if(pController!=NULL && !pController->IfPPTSlideShowView())
	{
		// 未放映时收到放大镜数据
		return;
	}

	DWORD dwSize;
	DWORD dwCommand;

	CPacketStream stPacket(pSockPacket->buffer);
	stPacket.UnPackMagnifier(dwSize, dwCommand);

	// 只允许一个手机端端操作
	static SOCKET sockControl = INVALID_SOCKET;
	if(MagicPen_MainUI::GetInstance()->IsShowing() || MagicPen_MainUI::GetInstance()->IsMagnifierShowing())
	{
		// 如果手机端未发送过命令，则认为当前手机端是操作端，忽略其他手机端的命令
		if(sockControl == INVALID_SOCKET)
			sockControl = pSockPacket->sock;
	}
	else
	{
		sockControl = INVALID_SOCKET;
	}
	if(sockControl != INVALID_SOCKET && sockControl != pSockPacket->sock)
	{
		// 当其他手机端或PC端正在控制时，不处理本次操作
		CPacketStream packRespone;
		packRespone.PackControlResponse(dwCommand, FALSE, _T("其他人正在使用放大镜，无法处理本次操作"));
		CPPTShellApp* pApp = (CPPTShellApp*)AfxGetApp();
		pApp->SendStreamPacket(pSockPacket ->sock, &packRespone, pSockPacket->type);
		return;
	}

	try{
		switch(dwCommand)
		{
		case MAGNIFIER_OPEN:
			{
				// 打开放大镜，当放大镜已经打开时，不再发送消息
				WRITE_LOG_LOCAL(_T("MAGNIFIER_OPEN"));
				sockControl = pSockPacket->sock;
				if(!MagicPen_MainUI::GetInstance()->IsShowing() && !MagicPen_MainUI::GetInstance()->IsMagnifierShowing())
				{
					CScreenInstrumentUI* pScreenIns = CScreenInstrumentUI::GetMainInstrument(); 
					if(pScreenIns != NULL)
						PostMessage(pScreenIns->GetHWND(), WM_SHOWMAGICPEN, 0,0);
					else
						WRITE_LOG_LOCAL(_T("Maginifier Open, GetScreenInstrument Failed"));
				}
			}
			break;
		case MAGNIFIER_TOGGLE:
			{
				// 关闭放大镜
				WRITE_LOG_LOCAL(_T("MAGNIFIER_TOGGLE"));
				sockControl = INVALID_SOCKET;
				CScreenInstrumentUI* pScreenIns = CScreenInstrumentUI::GetMainInstrument(); 
				if(pScreenIns != NULL)
					PostMessage(pScreenIns->GetHWND(), WM_HIDEMAGICPEN, 0,0);
				else
					WRITE_LOG_LOCAL(_T("Maginifier Close, GetScreenInstrument Failed"));
			}
			break;

		case MAGNIFIER_MOVE:
			{
				// 拖动放大镜的点，(-1,-1)后接起始点坐标，之后的都是移动点/终止点的点坐标，截图点右下角是(-2,-2)前一个点
				// PC端正在操控时忽略本次数据
				int nControlMode = MagicPen_MainUI::GetInstance()->GetControlMode();
				if(nControlMode == 1)
					break;

				int iDataLen = dwSize-sizeof(PACKET_HEAD)-4;
				int iPointCnt = iDataLen/8;
				if ( iDataLen%8 != 0)
				{
					WRITE_LOG_LOCAL(_T("CPacketProcessor::MagnifierControl, MOVE Data error!"));
					return ;
				}	

				float fLastX = -1;
				float fLastY = -1;
				bool bGetStartPoint = false;			// true - 下一个点是起始点
				static POINT ptMouse;					// 计算的坐标点

				for (int i=0; i<iPointCnt; i++)
				{
					float fCurrX = stPacket.ReadFloat();
					float fCurrY = stPacket.ReadFloat(); 
					//WRITE_LOG_LOCAL(_T("mobile send[%d]:%f,%f"),i, fCurrX, fCurrY);

					if (fCurrX==-1 || fCurrY ==-1)
					{ 
						WRITE_LOG_LOCAL(_T("1 mobile send[%d]:%f,%f"),i, fCurrX, fCurrY);
						// 下一个点是起始点
						bGetStartPoint = true;
						continue;
					}
					else if (fCurrX==-2 || fCurrY==-2)
					{ 
						WRITE_LOG_LOCAL(_T("2 mobile send[%d]:%f,%f"),i, fCurrX, fCurrY);
						// 释放点
						POINT* ptPost = new POINT;
						ptPost->x = ptMouse.x;
						ptPost->y = ptMouse.y;
						CScreenInstrumentUI* pScreenIns = CScreenInstrumentUI::GetMainInstrument(); 
						if(pScreenIns != NULL)
							PostMessage(pScreenIns->GetHWND(), WM_MOVEMAGICPEN, 2, (LPARAM)ptPost);
						else
							WRITE_LOG_LOCAL(_T("Maginifier Move Finish, GetScreenInstrument Failed"));
						break;
					}

					// 真实坐标
					pController->SlidePosToScreenPos(fCurrX, fCurrY); 
					ptMouse.x = (int)fCurrX;
					ptMouse.y = (int)fCurrY;

					POINT* ptPost = new POINT;
					//if(ptPost != NULL && !bGetStartPoint)
					//	continue;
					ptPost->x = ptMouse.x;
					ptPost->y = ptMouse.y;
					if(bGetStartPoint)
					{
						CScreenInstrumentUI* pScreenIns = CScreenInstrumentUI::GetMainInstrument(); 
						if(pScreenIns != NULL)
						{
							bGetStartPoint = false;
							PostMessage(pScreenIns->GetHWND(), WM_MOVEMAGICPEN, 0, (LPARAM)ptPost);
						}
						else
						{
							WRITE_LOG_LOCAL(_T("Maginifier Move Start, GetScreenInstrument Failed"));
						}
					}
					else
					{
						CScreenInstrumentUI* pScreenIns = CScreenInstrumentUI::GetMainInstrument(); 
						if(pScreenIns != NULL)
							PostMessage(pScreenIns->GetHWND(), WM_MOVEMAGICPEN, 1, (LPARAM)ptPost);
						else
							WRITE_LOG_LOCAL(_T("Maginifier Moving, GetScreenInstrument Failed"));
					}
				}
			}
			break;
		}

	}catch(...)
	{
		WRITE_LOG_LOCAL(_T("Catch Exception At MagnifierControl, Command=%d"), dwCommand);
	}
}
void CPacketProcessor::SpotlightControl(SOCK_PACKET* pSockPacket)
{
	// PPT未放映时不使用聚光灯
	CPPTController* pController = GetPPTController();
	if(pController!=NULL && !pController->IfPPTSlideShowView())
	{
		// 未放映时收到聚光灯数据
		return;
	}

	CPacketStream stPacket(pSockPacket->buffer);
	stPacket.SkipHeader();
	DWORD dwCommand;
	dwCommand = stPacket.ReadDWORD();

	CScreenInstrumentUI* pScreenIns = CScreenInstrumentUI::GetMainInstrument(); 
	if(pScreenIns == NULL)
		return;
	CSpotLightWindowUI * pSpotLightWnd = pScreenIns->GetSpotLightWindow();
	CSpotLightUI* pSpotLightUI = pSpotLightWnd->GetSpotLightUI();
	switch(dwCommand)
	{
	case SPOTLIGHT_OPEN:	// 打开
		{
			OutputDebugString(_T("SPOTLIGHT_OPEN\n"));
			pSpotLightWnd->ShowSpotLightWindow(true);
		}
		break;

	case SPOTLIGHT_SWITCH:
		{
			// 切换形状，1-CIRCLE，2-RECTANGLE
			OutputDebugString(_T("SPOTLIGHT_SWITCH\n"));
			DWORD dwType = stPacket.ReadDWORD();
			if(dwType == 1)
			{
				pSpotLightWnd->SetShapeType(SPOTLIGHT_SHAPE_CIRCLE);
			}
			else if(dwType == 2)
			{
				pSpotLightWnd->SetShapeType(SPOTLIGHT_SHAPE_RECTANGLE);
			}
		}
		break;

	case SPOTLIGHT_CHANGE:	// 改变大小
		{
			TCHAR szLog[100] = {0};

			if(pSpotLightUI->GetShape() == SPOTLIGHT_SHAPE_CIRCLE)
			{
				// 半径，百分比
				float fPerRadius;
				fPerRadius = stPacket.ReadFloat();
				DWORD dwRadius = PerFloatToSlideLength(fPerRadius, 0);

				_stprintf_s(szLog, _T("SPOTLIGHT_CHANGE-CIRCLE, radius=%d, fRadius=%f\n"), dwRadius, fPerRadius);
				OutputDebugString(szLog);
				pSpotLightWnd->SetCircleRadius(dwRadius);
			}
			else
			{
				// 矩形有宽高
				float fPerWidth, fPerHeight;
				DWORD dwWidth, dwHeight;

				fPerWidth = stPacket.ReadFloat();
				fPerHeight  = stPacket.ReadFloat();
				dwWidth = PerFloatToSlideLength(fPerWidth, 0);
				dwHeight = PerFloatToSlideLength(fPerHeight, 1);

				_stprintf_s(szLog, _T("SPOTLIGHT_CHANGE-RECTANGLE, width=%d, height=%d, fWidth=%f, fHeight=%f\n"), dwWidth, dwHeight, fPerWidth, fPerHeight);
				OutputDebugString(szLog);
				pSpotLightWnd->SetRectanglePos(dwWidth/2, dwHeight/2);
			}
		}
		break;

	case SPOTLIGHT_MOVE:	// 移动位置
		{
			float fPercentX, fPercentY;
			DWORD dwXPos, dwYPos;
			TCHAR szLog[100];

			fPercentX = stPacket.ReadFloat();
			fPercentY = stPacket.ReadFloat();
			pController->SlidePosToScreenPos(fPercentX, fPercentY);
			dwXPos = (DWORD)fPercentX;
			dwYPos = (DWORD)fPercentY;

			_stprintf_s(szLog, _T("SPOTLIGHT_MOVE, x=%d, y=%d, fx=%f, fy=%f\n"), dwXPos, dwYPos, fPercentX, fPercentY);
			OutputDebugString(szLog);
			//pSpotLightWnd->MoveSpotLight(dwXPos, dwYPos);
			pSpotLightWnd->SetSpotLightPos(dwXPos, dwYPos);
		}
		break;

	case SPOTLIGHT_OPEN_LIGHT:	// 开灯
		OutputDebugString(_T("SPOTLIGHT_OPEN_LIGHT\n"));
		pSpotLightWnd->SetLightStatus(SPOTLIGHT_STATUS_CLOSE);
		break;

	case SPOTLIGHT_CLOSE_LIGHT:	// 关灯
		OutputDebugString(_T("SPOTLIGHT_CLOSE_LIGHT\n"));
		pSpotLightWnd->SetLightStatus(SPOTLIGHT_STATUS_ON);
		break;

	case SPOTLIGHT_TOGGLE:	// 关闭聚光灯
		OutputDebugString(_T("SPOTLIGHT_TOGGLE\n"));
		pSpotLightWnd->ShowSpotLightWindow(false);
		break;
	}

}
// 将浮点数转为在PPT页面的长度。
// nLengthType标识根据PPT/屏幕宽还是高来转换长度。0——根据宽度来转换；其他数字——根据高度来转换。
// bIncludeBlank标识是否计算屏幕显示的左右或上下2边空白部分，
// true-包含空白部分，计算fPercent在屏幕的长度；false-减去空白部分的宽/高，计算fPercent在PPT页面的长度
int CPacketProcessor::PerFloatToSlideLength(float fPercent, int nLengthType, bool bIncludeBlank)
{
	int nLength;

	int nSlideWidth = 0;
	int nSlideHeight = 0;
	CPPTController* pController = GetPPTController();
	pController->GetSlideSize(nSlideWidth, nSlideHeight);
	int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int nScreenHight = GetSystemMetrics(SM_CYSCREEN);

	// 计算PPT页面的放大系数
	float fPerW, fPerH;
	fPerW = 1.0 * nScreenWidth / nSlideWidth;
	fPerH = 1.0 * nScreenHight / nSlideHeight;

	// 计算出实际的PTT页面宽度和高度
	int nActWidth, nActHeight; // 实际的PTT页面宽度和高度
	if(fPerW < fPerH)
	{
		nActWidth = nScreenWidth;
		nActHeight = fPerW * nSlideHeight;
	}
	else
	{
		nActWidth = fPerH * nSlideWidth;
		nActHeight = nScreenHight;
	}

	// 根据参数确定转换长度
	int nReferLength = 0;
	if(nLengthType == 0)
	{
		if(bIncludeBlank)
			nReferLength = nScreenWidth;
		else
			nReferLength = nActWidth;
	}
	else
	{
		if(bIncludeBlank)
			nReferLength = nScreenHight;
		else
			nReferLength = nActHeight;
	}

	nLength = abs(fPercent) * nReferLength;
	return nLength;
}
void CPacketProcessor::BlackBoardControl(SOCK_PACKET* pSockPacket)
{
	// PPT未放映时不使用黑板
	CPPTController* pController = GetPPTController();
	if(pController!=NULL && !pController->IfPPTSlideShowView())
	{
		// 未放映时收到黑板数据
		return;
	}

	CPacketStream stPacket(pSockPacket->buffer);
	stPacket.SkipHeader();
	DWORD dwCommand;
	dwCommand = stPacket.ReadDWORD();
	switch(dwCommand)
	{
	case BLACKBOARD_OPEN:
		{
			// 有创建窗口，要用PostMessage
			CScreenInstrumentUI* pScreenIns = CScreenInstrumentUI::GetMainInstrument();
			if(pScreenIns == NULL)
				WRITE_LOG_LOCAL(_T("BlackBoardControl Open BlackBoard, MainInstrument Instance is null"));
			PostMessage(pScreenIns->GetHWND(), WM_OPENBLACKBOARD, 0, 0);
		}
		break;
	case BLACKBOARD_CLOSE:
		{
			CScreenInstrumentUI* pScreenIns = CScreenInstrumentUI::GetMainInstrument();
			if(pScreenIns == NULL)
				WRITE_LOG_LOCAL(_T("BlackBoardControl Close BlackBoard, MainInstrument Instance is null"));
			PostMessage(pScreenIns->GetHWND(), WM_CLOSEBLACKBOARD, 0, 0);
		}
		break;
	}
}

//
// process file transfer action
//
void CPacketProcessor::ProcessActionTransferFile(SOCK_PACKET *pSockPacket)
{
	PACKET_HEAD* pHeader = (PACKET_HEAD*)pSockPacket->buffer;
	if( pHeader == NULL )
		return;

	switch( pHeader->dwMinorType )
	{
	case TRANSFER_FILE_START:
		{
			CPacketStream FileHeadStream(pSockPacket->buffer);

			tstring strFileName;
			DWORD dwFileSize = 0;
			DWORD dwFilecCrc = 0;
			DWORD dwFileType = 0;

			FileHeadStream.UnPackFileHead(strFileName, dwFileSize, dwFilecCrc, dwFileType);

			tstring strFilePath;
			GenReceiveFilePath(strFileName, strFilePath);
			//tstring strFilePath = GetLocalPath();
			//strFilePath += _T("\\");
			//strFilePath += DIRECTORY_MOBILE_RECV_FILE;

			//// create directory
			//CreateDirectory(strFilePath.c_str(), NULL);

			//strFilePath += _T("\\");
			//strFilePath += strFileName;

			// 如果文件没在传输列表中，也添加进去。图片文件等不会做CRC校验流程
			if(m_mapTransferFile.count(strFileName) == 0)
				m_mapTransferFile[strFileName] = dwFilecCrc;
			WRITE_LOG_LOCAL(_T("TRANSFER_FILE_START filename=%s"), strFileName.c_str());

			FileTransferManager::GetInstance()->RecvFile((TCHAR*)strFilePath.c_str(), dwFileSize, dwFilecCrc, FILE_RECV_START);
	
		}
		break;

	case TRANSFER_FILE_END:
		{
			DWORD dwFileCrc;
			CPacketStream FileEndStream(pSockPacket->buffer);
			FileEndStream.UnPackFileEnd(dwFileCrc);

			// 移除已传输完的文件信息
			tstring strFileName;
			for(map<tstring, DWORD>::iterator iter=m_mapTransferFile.begin(); iter!=m_mapTransferFile.end(); iter++)
			{
				if(iter->second == dwFileCrc)
				{
					strFileName = iter->first;
					m_mapTransferFile.erase(iter);
					break;
				}
			}

			// 如果strFileName是空表示文件已被移出传输列表，传输同名文件，但2个文件的CRC不同时会出现strFileName为空
			if(strFileName.empty())
				WRITE_LOG_LOCAL(_T("TRANSFER_FILE_END, File Retransferred"), strFileName.c_str())
			else
				WRITE_LOG_LOCAL(_T("TRANSFER_FILE_END, filename=%s"), strFileName.c_str())

			FileTransferManager::GetInstance()->RecvFile(NULL, 0, dwFileCrc, FILE_RECV_END);
		}
		break;

	case TRANSFER_FILE_DATA:
		{
			char szFileData[MAX_FILE_DATA_LEN];

			DWORD dwFileCrc;
			DWORD dwDataSize;

			CPacketStream FileDataStream(pSockPacket->buffer);
			FileDataStream.UnPackFileData(dwFileCrc, szFileData, dwDataSize);

			FileTransferManager::GetInstance()->RecvFile(NULL, 0, dwFileCrc, FILE_RECV_DATA, szFileData, dwDataSize);

		}
		break;

	case TRANSFER_FILE_CANCEL:
		{
			DWORD dwFileCrc;
			CPacketStream FileCancelStream(pSockPacket->buffer);
			FileCancelStream.UnPackFileCancel(dwFileCrc);

			// 移除取消传输的文件信息
			tstring strFileName;
			for(map<tstring, DWORD>::iterator iter=m_mapTransferFile.begin(); iter!=m_mapTransferFile.end(); iter++)
			{
				if(iter->second == dwFileCrc)
				{
					strFileName = iter->first;
					m_mapTransferFile.erase(iter);
					break;
				}
			}

			FileTransferManager::GetInstance()->CancelFile(dwFileCrc);
			
		}
		break;

	case TRANSFER_FILE_CHECK_EXISTS:
		{
			TransferFileCheckExists(pSockPacket);
			break;
		}
	}
}

void CPacketProcessor::ProcessActionPushData(SOCK_PACKET *pSockPacket)
{
	 
}

void CPacketProcessor::ProcessActionPullData(SOCK_PACKET *pSockPacket)
{
	PACKET_HEAD* pHeader = (PACKET_HEAD*)pSockPacket->buffer;
	if( pHeader == NULL )
		return;

	switch( pHeader->dwMinorType )
	{
	case PULL_M2P_APKFILE:
		{
			// send PPTShell.apk to mobile
			tstring strFilePath = GetLocalPath() + _T("\\Bin\\PPTShell.apk");
			FileTransferManager::GetInstance()->SendFile((TCHAR*)strFilePath.c_str(), pSockPacket->sock, pSockPacket->type, FILE_TYPE_APK);

		}
		break;
	case PULL_M2P_PPTFILE:
		{
			CPPTController* pController = GetPPTController();
			if( pController != NULL )
			{
				tstring strFilePath = pController->GetFilePath();
				FileTransferManager::GetInstance()->SendFile((TCHAR*)strFilePath.c_str(), pSockPacket->sock, pSockPacket->type, FILE_TYPE_PPTFILE);
			}
		}
		break;
	}

}

void CPacketProcessor::ProcessActionSyncData(SOCK_PACKET* pSockPacket)
{
	PACKET_HEAD* pHeader = (PACKET_HEAD*)pSockPacket->buffer;
	if( pHeader == NULL )
		return;

	switch( pHeader->dwMinorType )
	{
	case SYNC_DATA_REQUEST_SLIDE_IMAGE:
		SyncSlideImgRequest(pSockPacket);
		break;
	case SYNC_SAVE_CHANGE:
		{
			CPPTController* pController = GetPPTController();
			CPPTInfoSender *pptSender = PPTInfoSender::GetInstance();
			if( pController != NULL && pController->IsChanged(FALSE) && pptSender!=NULL && !pptSender->IsSyncDataPPTChangedAck())
			{
				pController->Save();
				pptSender->SetAckSyncDataPPTChanged();
			}
		}
		break;
	case SYNC_NOSAVE_CHANGE:
		{
			CPPTInfoSender *pptSender = PPTInfoSender::GetInstance();
			if (pptSender != NULL)
			{
				pptSender->SetAckSyncDataPPTChanged();
			}
		}
		break;
	default:
		WRITE_LOG_LOCAL(_T("CPacketProcessor::ProcessActionSyncData, unknow minor type:%d"), pHeader->dwMinorType);
		break;

	}
}

void CPacketProcessor::SyncSlideImgRequest(SOCK_PACKET* pSockPacket)
{
	CPPTInfoSender *pptSender = PPTInfoSender::GetInstance();
	if (pptSender == NULL)
	{
		WRITE_LOG_LOCAL(_T("CPacketProcessor::SyncSlideImgRequest, CPPTInfoSender is null!"))
		return;
	}
	CPacketStream syncControlStream(pSockPacket->buffer);
	syncControlStream.SkipHeader();

	DWORD rqstCnt = syncControlStream.ReadDWORD();
	
	tstring strLog = _T("---------Rcv mobile request slide:");
	vector<int> vecSlideIdxs;
	for (int i=0; i<rqstCnt; i++)
	{
		int iPageIdx = syncControlStream.ReadDWORD();
		if (iPageIdx>0)
		{
			vecSlideIdxs.push_back(iPageIdx);
			strLog += iPageIdx; 
			strLog += _T("|");
		}
	}
	WRITE_LOG_LOCAL(_T("%s"), strLog.c_str());
	pptSender->SendSlideThumbList(pSockPacket->sock, pSockPacket->type, vecSlideIdxs);
}

//
// PPT control
//
void CPacketProcessor::PPTControl(SOCK_PACKET* pSockPacket)
{
	BOOL res = FALSE;

	CPPTController* pController = GetPPTController();
	if( pController == NULL )
		return;

	DWORD dwControlCommand = 0;
	CPacketStream PPTControlStream(pSockPacket->buffer);
	PPTControlStream.UnPackPPTControl(dwControlCommand);

	tstring strErrorDesc = _T("");

	switch(dwControlCommand)
	{
	case PPTCTL_PLAY:
		{
			DWORD dwSlideId = PPTControlStream.ReadDWORD();
			if( dwSlideId == 0 )
				res = pController->Play(TRUE);
			else
				res = pController->Play(FALSE);

			if( !res )
				strErrorDesc = _T("没有PPT可以播放");
			
		}
		break;

	case PPTCTL_STOP:
		{
			if( !pController->IfPPTSlideShowView() )
				strErrorDesc = _T("PPT未播放");
			else
			{
				StopPPTByThread();
				res = TRUE;
			}
		}		
		break;

	case PPTCTL_PREV:
		{
			if (pController->IfPPTSlideShowView())
			{
				pController->TopShowSlideView();//置顶，不然被抢焦点后就会导致无法切页 2016.03.04 cws
				res = pController->Prev();
				if( !res )
					strErrorDesc = _T("PPT未播放");
			}
			else
			{
				int iSlideID = pController->GetActiveSlideIndex();
				iSlideID -= 1;
				res = pController->PresentationGoto(iSlideID);
				if( !res )
					strErrorDesc = _T("PPT预览跳转失败");
				else
					PPTInfoSender::GetInstance()->SendPreviewSlideIdxToAllUser(iSlideID);
			} 
		}
		break;

	case PPTCTL_NEXT:
		{
			if (pController->IfPPTSlideShowView())
			{
				pController->TopShowSlideView();//置顶，不然被抢焦点后就会导致无法切页 2016.03.04 cws
				res = pController->Next();
				if( !res )
				{
					strErrorDesc = _T("PPT未播放");
				}
				else
				{
					// 当到达黑幕时要发送当前页号给手机端,其他情况有正常流程
					//if(pController->IfPPTSlideShowView())
					//static bool bSentBlackScreen = false;
					//if(!bSentBlackScreen)
					//{
					int nPage = pController->GetCurrentShowPosition();
					//WRITE_LOG_LOCAL(_T("PPTCTL_NEXT GetCurrentShowPosition=%d\n"), nPage);
					int nCount = CPPTController::GetShowViewCount();
					if(nPage > nCount)
					{
						OutputDebugString(_T("Send Black Screen Index\n"));
						//bSentBlackScreen = true;
						PPTInfoSender::GetInstance()->SendPlaySlideToAllUsers(nCount + 1);
					}
					//}
				}
			}
			else
			{
				int iSlideID = pController->GetActiveSlideIndex();
				iSlideID += 1;
				res = pController->PresentationGoto(iSlideID);
				if( !res )
					strErrorDesc = _T("PPT预览跳转失败");
				else
					PPTInfoSender::GetInstance()->SendPreviewSlideIdxToAllUser(iSlideID);
			} 
		}
		break;

	case PPTCTL_GOTO:
		{

			int iSlideID = PPTControlStream.ReadDWORD();

			if (pController->IfPPTSlideShowView())
			{
				pController->TopShowSlideView();//置顶，不然被抢焦点后就会导致无法切页 2016.03.04 cws
				res = pController->Goto(iSlideID);
				if( !res )
					strErrorDesc = _T("PPT未播放");
			}
			else
			{
				res = pController->PresentationGoto(iSlideID);
				if( !res )
					strErrorDesc = _T("PPT预览跳转失败");
				else
					PPTInfoSender::GetInstance()->SendPreviewSlideIdxToAllUser(iSlideID);
			} 
		}
		break;

	case PPTCTL_CLEAR_INK:
		break;

	case PPTCTL_BLACK_BACKGROUND:
		{
			PpSlideShowState state = (PpSlideShowState)pController->GetSlideShowViewState();
			res = pController->SetSlideShowViewState(ppSlideShowBlackScreen);
			if( !res )
				strErrorDesc = _T("PPT未播放");
		}
		break;

	case PPTCTL_WHITE_BACKGROUND:
		{
			res = pController->SetSlideShowViewState(ppSlideShowWhiteScreen);
			if( !res )
				strErrorDesc = _T("PPT未播放");
		}
		break;
		
	case PPTCTL_CANCEL_WHITE_BACKGROUND:
		{
			if(pController->GetPPTVersion() <= PPTVER_2007)
			{
				res = pController->SetSlideShowViewState(ppSlideShowRunning);
				if( !res )
					strErrorDesc = _T("PPT未播放");
			}
			else//OFFICE2013环境下设置ppSlideShowRunning参数无效，采用模拟按键来触发2016.01.05 CWS
			{
				res = TRUE;
				pController->TopShowSlideView();
				keybd_event('W', 0,0,0);
				keybd_event('W',0, KEYEVENTF_KEYUP, 0);
			}
		}
		break;
	case PPTCTL_CANCEL_BLACK_BACKGROUND:
		{
			// OFFICE2007环境下设置ppSlideShowRunning参数后，PPT页面变成缩略图
			// OFFICE2013环境下设置ppSlideShowRunning参数无效，采用模拟按键来触发2016.01.05 CWS
			//res = pController->SetSlideShowViewState(ppSlideShowRunning);
			res = TRUE;
			pController->TopShowSlideView();
			keybd_event('B', 0,0,0);
			keybd_event('B',0, KEYEVENTF_KEYUP, 0);
		}
		break;

	case PPTCTL_HANDLCLAP:
		PlayEffect(0);
		break;

	case PPTCTL_CAIDAI:
		PlayEffect(1);
		break;

	case PPTCTL_CONFETTI:
		PlayEffect(2);
		break;

	case PPTCTL_CALC:
		{
			res = ExecCalculator();
			if( !res )
				strErrorDesc = _T("运行计算器失败");
		}
		break;

	case PPTCTL_CANCEL_CALC:
		{
			res = KillCalculator();
			// 强制返回成功给手机端。
			res = TRUE;
			strErrorDesc = _T("计算器未运行");
		}
		break;

	case PPTCTL_LASER_POINTER:
		break;

	case PPTCTL_PEN_POINTER:
		break;

	case PPTCTL_NORMAL_POINTER:
		break;

	case PPTCTL_PLAY_VIDEO:
		{
			int nVideoIndex = PPTControlStream.ReadInt();
			res = pController->PlayVideo(nVideoIndex);
			if( !res )
				strErrorDesc = _T("视频播放失败");

		}
		break;

	case PPTCTL_PAUSE_VIDEO:
		{
			int nSlideIdx = PPTControlStream.ReadInt();
			res = pController->PauseVideo(nSlideIdx);
			if( !res )
				strErrorDesc = _T("视频暂停失败");
		}
		break;

	case PPTCTL_STOP_VIDEO:
		{
			int nSlideIdx = PPTControlStream.ReadInt();
			res = pController->StopVideo(nSlideIdx);
			if( !res )
				strErrorDesc = _T("视频停止失败");
		}
		break;

	case PPTCTL_FLAME:
		break;
	case PPTCTL_OPEN:
		{
			if ( CPPTController::isPlayMode() )
			{
				strErrorDesc = _T("正在播放PPT，无法打开");
				WRITE_LOG_LOCAL(_T("Recv PPTControl Action When PPT Playing, cmd is %d"), PPTCTL_OPEN);
			}
			else
			{

				string strUtf8FileName = PPTControlStream.ReadCString();
				tstring strFileName = Utf82Str(strUtf8FileName);

				// 当文件重复传时，忽略前面的文件打开命令
				if(m_mapTransferFile.count(strFileName) != 0)
				{
					DWORD dwCRC = CalcFileCRC(strFileName.c_str());
					if(m_mapTransferFile[strFileName] == dwCRC)
					{
						break;
					}
				}

				tstring strFilePath = _T("");
				GenReceiveFilePath(strFileName, strFilePath);

				WRITE_LOG_LOCAL(_T("PPTCTL_OPEN filename=%s"), strFileName.c_str());

				CStream* pStream	= new CStream(256);
				pStream->WriteDWORD(PPTC_OPEN_PPTFILE);
				pStream->WriteString(strFilePath);
				BroadcastPostedEvent(EVT_THREAD_CONTROL_PPT, (WPARAM)pStream, 0, NULL);
			}
		}
		break;
	case PPTCTL_BARRAGE:
		{
			
#ifdef _UNICODE
			tstring wstrDanmukuWord = PPTControlStream.ReadString();
			string astrDanmukuWord = string((char *)wstrDanmukuWord.c_str());
			tstring strDanmukuWord = Utf82Str(astrDanmukuWord);
#else
			tstring strDanmukuWord = PPTControlStream.ReadString();
#endif
			//strDanmukuWord = Utf82Str(strDanmukuWord);
			//wstring wstrDanmukuWord = Str2Unicode(strDanmukuWord);
			
			CStream* pStream = new CStream(256);
			pStream->WriteString(strDanmukuWord);
			BroadcastPostedEvent(EVT_DANMUKU, (WPARAM)pStream, 0, NULL);
			
		}
		break;
	case PPTCTL_YAOHAO:
		break;
	}

	CPacketStream ControlResponseStream;
	ControlResponseStream.PackControlResponse(dwControlCommand, res, strErrorDesc);
	
	CPPTShellApp* pApp = (CPPTShellApp*)AfxGetApp();
	pApp->SendStreamPacket(pSockPacket->sock, &ControlResponseStream, pSockPacket->type);

}

void GenReceiveFilePath(const tstring& strFileName, tstring &strFullName)
{
	strFullName = GetLocalPath();
	strFullName += _T("\\");
	strFullName += _T("Cache");
	CreateDirectory(strFullName.c_str(), NULL);

	strFullName += _T("\\");
	strFullName += DIRECTORY_MOBILE_RECV_FILE;
	CreateDirectory(strFullName.c_str(), NULL);

	strFullName += _T("\\");
	strFullName += strFileName;
}

//
// Image control
//
void CPacketProcessor::ImageControl(SOCK_PACKET *pSockPacket)
{
	BOOL res = FALSE;

	DWORD dwControlCommand = 0;
	CPacketStream ImageControlStream(pSockPacket->buffer);
	ImageControlStream.UnPackImageControl(dwControlCommand);

	CPPTController* pPPTController = GetPPTController();

	switch( dwControlCommand )
	{
	case IMGCTL_OPEN:
		{
			// image file name
			string strUtf8FileName = ImageControlStream.ReadCString();
			tstring strFileName = Utf82Str(strUtf8FileName);

			tstring strFilePath = _T("");
			GenReceiveFilePath(strFileName, strFilePath);

			ImageCtrlDialogUI::GetInstance()->OpenDialog(strFilePath.c_str());
		}
		break;

	case IMGCTL_ZOOMIN:
		{
			bool bClosed = ImageCtrlDialogUI::GetInstance()->IsWindowClosed();
			if(!bClosed)
			{
				float fScale = ImageControlStream.ReadFloat();
				ImageCtrlDialogUI::GetInstance()->ZoomIn(fScale);
			}	
		}
		break;

	case IMGCTL_ZOOMOUT:
		{
			bool bClosed = ImageCtrlDialogUI::GetInstance()->IsWindowClosed();
			if(!bClosed)
			{
				float fScale = ImageControlStream.ReadFloat();
				ImageCtrlDialogUI::GetInstance()->ZoomOut(fScale);
			}
		}
		break;

	case IMGCTL_NORMAL:
		{
			bool bClosed = ImageCtrlDialogUI::GetInstance()->IsWindowClosed();
			if(!bClosed)
				ImageCtrlDialogUI::GetInstance()->Normal();
		}
		break;

	case IMGCTL_CLOSE:
		// 当本地窗口已关闭后，忽略其他处理
		{
			bool bClosed = ImageCtrlDialogUI::GetInstance()->IsWindowClosed();
			if(!bClosed)
				ImageCtrlDialogUI::GetInstance()->CloseDialog();
		}
		break;
	}

}

//
// 3D Control
//
void CPacketProcessor::ThreeDControl(SOCK_PACKET *pSockPacket)
{
	BOOL res = FALSE;

	DWORD dwControlCommand = 0;
	CPacketStream ThreedControlStream(pSockPacket->buffer);
	ThreedControlStream.UnPack3DControl(dwControlCommand);

	switch( dwControlCommand )
	{
	case THREEDCTL_ZOOMIN:
		break;

	case THREEDCTL_ZOOMOUT:
		break;

	case THREEDCTL_MOVE:
		{
			// points of position
			int nPointCount = ThreedControlStream.ReadInt();
			for(int i = 0; i < nPointCount; i++)
			{
				
			}

		}
		break;

	case THREEDCTL_CLOSE:
		break;
	}
	
}

//
// util functions
//
BOOL CPacketProcessor::PlayEffect(int nEffectType)
{
	if( nEffectType == -1 )
		return FALSE;

	tstring sActionPath[] = { _T("4"), _T("5"), _T("6"), _T(""), _T(""), _T(""), _T(""), _T("")};
	TCHAR szExePath[MAX_PATH*2]; 
	TCHAR szImageDir[MAX_PATH*2];// 运行目录不指定，声音不会播放 2015.12.18
 
	_stprintf_s(szImageDir, _T("%s\\bin"), GetLocalPath().c_str()); 
 
	_stprintf_s(szExePath, _T("%s\\bin\\GifPlayer.exe"), GetLocalPath().c_str());
	HINSTANCE hInstance = ShellExecute(NULL, _T("open"), szExePath, sActionPath[nEffectType].c_str(), szImageDir, SW_SHOW );
	if( (int)hInstance < 32 )
		return FALSE;


	return TRUE;
}

BOOL CPacketProcessor::ExecCalculator()
{
	//KillExeCheckParentPid(_T("CALC.EXE"), TRUE);//先把原先打开的关闭 2015.12.18 CWS  2016.01.15 打开
	if (! ControlCalcTop())
	{
		TCHAR szExePath[MAX_PATH*2]; 
		TCHAR szSystemPath[MAX_PATH*2];

		GetSystemDirectory(szSystemPath, MAX_PATH*2);
		_stprintf_s(szExePath, _T("%s\\calc.exe"), szSystemPath);

		HINSTANCE hInstance = ShellExecute(NULL, _T("open"), szExePath, _T(""), _T(""), SW_SHOW);
		if( (int)hInstance < 32 )
			return FALSE;
		ControlCalcTop();//再次置顶2016.06.15 cws
	}
 
	return TRUE;
}

BOOL CPacketProcessor::ControlCalcTop()
{ 
	HWND hWnd = FindWindow(_T("CalcFrame"), NULL); 
	if( !hWnd )
	{   
		hWnd = FindWindow(NULL, _T("计算器"));//win10系统 中文系统先解决。。2016.01.07 cws 
	} 
	if (hWnd)
	{
		CRect rectCal;
		::GetWindowRect(hWnd,&rectCal);
		int calWidth = rectCal.Width();
		int calHeight = rectCal.Height();

		int nCx  = GetSystemMetrics(SM_CXSCREEN);
		int nCy = GetSystemMetrics(SM_CYSCREEN);
		::SetWindowPos(hWnd,HWND_TOPMOST ,(nCx - calWidth) /2, (nCy - calHeight) / 2, calWidth,calHeight,SWP_NOACTIVATE|SWP_SHOWWINDOW);		
		return TRUE; 
	}
	return FALSE;
}

BOOL CPacketProcessor::KillCalculator()
{
	HWND hWnd = FindWindow(_T("CalcFrame"), NULL);
	if (!hWnd)
	{
		hWnd = FindWindow(NULL, _T("计算器"));//win10系统 中文系统先解决。。2016.01.07 cws 
	}
	if( hWnd )
		PostMessage(hWnd, WM_CLOSE, 0, 0);
	else
	{ 
		return FALSE; 
	}

	return TRUE;
}
// 函数：	TransferFileCheckExists
// 参数：	pSockPacket		- 接收到的网络帧
// 返回：	TRUE			- 发送回复帧成功
//			FALSE			- 处理异常
// 描述：	校验本地文件是否已存在，发送校验结果回复帧。
BOOL CPacketProcessor::TransferFileCheckExists(SOCK_PACKET *pSockPacket)
{
	tstring strFileName;
	DWORD dwFileCRC = 0;
	BOOL bExists = FALSE;

	CPacketStream FileCheckExists(pSockPacket->buffer);
	FileCheckExists.UnPackFileCRC(strFileName, dwFileCRC);

	// 检查本地文件
	tstring strFilePath;
	GenReceiveFilePath(strFileName, strFilePath);
	//tstring strFilePath = GetLocalPath();
	//strFilePath += _T("\\");
	//strFilePath += DIRECTORY_MOBILE_RECV_FILE;
	//CreateDirectory(strFilePath.c_str(), NULL);
	//strFilePath += _T("\\");
	//strFilePath += strFileName;

	bExists = FileTransferManager::GetInstance()->CheckFileIsExists(strFilePath, dwFileCRC);

	m_mapTransferFile[strFileName] = dwFileCRC;
	//if( !bExists)
	//{
	//	// 如果文件传输中断，会导致后续原文件无法上传
	//	// 如果CRC与本地文件不一致，可能是文件传输未结束，也可能文件有修改。检查文件是否在传输列表
	//	map<tstring, DWORD>::iterator iter = m_mapTransferFile.find(strFileName);
	//	if(iter != m_mapTransferFile.end())
	//	{
	//		// 文件在传输传输列表，检查下CRC是否相同
	//		if(dwFileCRC == iter->second)
	//			bExists = TRUE;
	//		else
	//			bExists = FALSE;
	//	}
	//	else
	//	{
	//	}
	//	// 保存本次传输的文件信息
	//	if(!bExists)
	//		m_mapTransferFile[strFileName] = dwFileCRC;
	//}

 	CPPTShellApp* pApp = (CPPTShellApp*)AfxGetApp();
	if( pApp == NULL )
		return FALSE;

	
	// 发送检查结果帧。bExists=1表示本地文件已存在；bExists=0表示本地文件不存在
	CPacketStream FileCheckResult;
	FileCheckResult.PackFileCheckResult(strFileName, bExists);
	pApp->SendStreamPacket(pSockPacket->sock, &FileCheckResult, pSockPacket->type);

	return TRUE;
}
