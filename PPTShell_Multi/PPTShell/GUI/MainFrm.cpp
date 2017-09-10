
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "PPTShell.h"
#include "MainFrm.h"
#include "PPTShellDoc.h"
#include "PPTShellView.h"
#include "PPTControl/PPTController.h"
#include "GUI/SkinManager.h"
#include "NDCloud/NDCloudAPI.h"
#include "EventCenter/EventDefine.h"
#include "EventCenter/EventCenter.h"
#include "Update/UpdateOperation.h"
#include "PPTControl/PPTControllerManager.h"
#include "Statistics/Statistics.h"
#include "Common.h"
#include "dui/GuideDialog.h"
#include "PPTControl/PPTImagesExporter.h"
#include "NDCloud/NDCloudUser.h"
#include "DUI/ShadeWindow.h"
#include "DUI/QRCodeDialogUI.h"
#include "DUI/MessageBoxUIEx.h"
#include "Config.h"
#include "DUI/GroupExplorer.h"
#include "PPTControl/PPTICRPlayControl.h"
#include "DUI/UserReLogin.h"
#include "NDCloud/NDCloudQuestion.h"
#include "NDCloud/NDCloudLocalQuestion.h"
#include "NDCloud/NDCloudCoursewareObjects.h"
#include "DUI\LaserPointerDlg.h"
#include "PPTControl\PPTPack.h"
#include "DUI\PackPPTDialog.h"
#include "DUI/QuestionDialogUI.h"
#include "DUI/ProgressSaving.h"
#include "DUI/ChapterInfoDialog.h"
#include "DUI/UpdateTipDialog.h"
#include "DUI/AddSlideFloatWindow.h"
#include "DUI/ExercisesEditFloatWindow.h"
#include "DUI/VRDistributeFloatWindow.h"
#include "Plugins/Icr/IcrPlayer.h"
#include "PPTControl/PPTInfoSender.h"
#include "Util/FileTypeFilter.h"
#include "NDCefLib/NdCefShareIpc.h"
#include "DUI/CoursePlayUI.h"
#include "DUI/ProgressLoading.h"
#include "gui/LyricsWindow.h"
//#include "DUI/PackPPTTip.h"
#include "DUI/MessageBoxUIEx2.h"
#include "DUI/LoginToolTip.h"
#include "AutoBuildStr.h"

//downloader manager
#include "DUI/ITransfer.h"
#include "DUI/IDownloaderListener.h"
#include "DUI/ITransfer.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IDownloadListener.h"
#include "DUI/ResourceDownloader.h"
#include "DUI/ResourceDownloaderManager.h"
#include "DUI/DownloadManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMainFrame, CSkinFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CSkinFrameWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_MOVE()
	ON_WM_PAINT()
	ON_WM_COPYDATA()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_NOTIFY_UPDATE,			&CMainFrame::OnNotifyUpdate)
	ON_MESSAGE(WM_TITLEBUTTON_OPERATION,	&CMainFrame::OnTitleButtonOperation)
	ON_MESSAGE(WM_TITLE_MOUSE,				&CMainFrame::OnTitleMouse)
	ON_MESSAGE(WM_USER_SEND_EVENT,			&CMainFrame::OnUserSendEvent)
	ON_MESSAGE(WM_USER_POST_EVENT,			&CMainFrame::OnUserPostEvent)
	ON_MESSAGE(WM_USER_HTTP_DOWNLOAD,		&CMainFrame::OnUserHttpMsg)
	ON_MESSAGE(WM_USER_HTTP_UPLOAD,			&CMainFrame::OnUserHttpUploadMsg)
	ON_MESSAGE(WM_USER_PPT_EXPOERT_IMAGES,	&CMainFrame::OnPPTExportImagesMsg)
	ON_MESSAGE(WM_USER_PACK_PPT,			&CMainFrame::OnPPTPackPPTMsg)
	ON_MESSAGE(WM_SHOW_UPDATETIP,			&CMainFrame::OnShowUpdateTipMsg)
	ON_MESSAGE(WM_SHOW_ADD_SLIDE_WINDOW,	&CMainFrame::OnShowAddSlideWindow)
	ON_MESSAGE(WM_SHOW_EXER_EDIT_WINDOW,	&CMainFrame::OnShowExerEditWindow)
	ON_MESSAGE(WM_SHOW_VR_DISTRIBUTE_WINDOW,&CMainFrame::OnShowVRDistributeWindow)
	ON_MESSAGE(WM_USER_PPT_CONTROL,			&CMainFrame::OnUserPPTControl)
	ON_MESSAGE(WM_CRATE_QUESTION_THUMBNAIL,	&CMainFrame::OnQuestionThumbnail)
	ON_MESSAGE(WM_SHOW_UPDATEINFO,			&CMainFrame::OnShowUpdateInfoMsg)
	ON_MESSAGE(WM_DOWNLOAD_MANAGER_CLOSE,	&CMainFrame::OnDownloaderClose)
	ON_WM_SYSCOMMAND()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CResGuard CMainFrame::m_Lock;
HWND CMainFrame::m_hMainWnd = NULL;

CMainFrame::CMainFrame()
{
	m_pAboutBtn		= NULL;
	m_pChapterBtn	= NULL;
	m_pPrevBtn		= NULL;
	m_pNextBtn		= NULL;
	m_pDownloadMgr	= NULL;

	m_pMenu			= NULL;
	m_pUpdateDlg	= NULL;

	m_bClosing		= FALSE;
	m_bCloseAll		= FALSE;
	m_bUploadingOnClose = FALSE;

	m_nSwfIndex			 = 0;
	m_dwLastOperaionerId = 0;
	m_nChangeSubOperation= 0;
	m_nCurPPTCommand	 = 0;
	m_bIsReadOnlyOpen	 = FALSE;

	m_pMainPPTController = NULL;

	m_nPlayMode			= ID_MSGBOX_BTN;
	m_nUpdateStatus		= eUpdateType_Newset;

	m_bFirstStart		= TRUE;
	m_bIsChanged		= FALSE;

	m_hCefMapping		= NULL;
	m_bCefRunning		= FALSE;

	m_bOpening			= FALSE;
	m_dwOpenedTime		= 0;

	m_h3DWnd			= NULL;


	m_pMainPPTController = ::GetPPTController();
	m_pMainPPTController->InitEvent();
	 
	m_pScreenInstrument = NULL;
	m_hUserReLogin = NULL;//重新登录

	m_nSaveToSkyType = 0;

	::OnEvent(EVT_SET_CHAPTER_GUID,		MakeEventDelegate(this, &CMainFrame::OnChapterChange));
	::OnEvent(EVT_NOTIFY_UPDATE,		MakeEventDelegate(this, &CMainFrame::OnUpdateEvent));
	::OnEvent(EVT_PPT_FILE_OPERATION,	MakeEventDelegate(this, &CMainFrame::OnPPTOperation));
	::OnEvent(EVT_ENV_RELEASE,			MakeEventDelegate(this, &CMainFrame::OnEnvRelease));
	::OnEvent(EVT_LOGIN,		MakeEventDelegate(this, &CMainFrame::OnEventLoginCompleteOperation));
	::OnEvent(EVT_RELOGIN,		MakeEventDelegate(this, &CMainFrame::OnEventReLoginOperation));
	::OnEvent(EVT_LOGOUT,		MakeEventDelegate(this, &CMainFrame::OnEventLogOutOperation));
	::OnEvent(EVT_THREAD_CONTROL_PPT,	MakeEventDelegate(this, &CMainFrame::OnEventPPTControl));
	::OnEvent(EVT_THREAD_INSERT_PPT,	MakeEventDelegate(this, &CMainFrame::OnEventPPTInsert));
	::OnEvent(EVT_MOBILE_LOGIN, 	MakeEventDelegate(this, &CMainFrame::OnMobileAutoLogin));
	::OnEvent(EVT_DANMUKU, MakeEventDelegate(this, &CMainFrame::OnDanmukuWordAdd));

	//::OnEvent(IDC_TITLEBUTTON_ABOUT,		MakeEventDelegate(this, &CMainFrame::OnEventAboutSoft));
	
	//::OnEvent(EVT_SHOW_NEXT_SLIDE,		MakeEventDelegate(this, &CMainFrame::OnShowNextSlide));

}

CMainFrame::~CMainFrame()
{
	::CancelEvent(EVT_SET_CHAPTER_GUID,		MakeEventDelegate(this, &CMainFrame::OnChapterChange));
	::CancelEvent(EVT_NOTIFY_UPDATE,		MakeEventDelegate(this, &CMainFrame::OnUpdateEvent));
	::CancelEvent(EVT_PPT_FILE_OPERATION,	MakeEventDelegate(this, &CMainFrame::OnPPTOperation));
	::CancelEvent(EVT_ENV_RELEASE,			MakeEventDelegate(this, &CMainFrame::OnEnvRelease));

	::CancelEvent(EVT_LOGIN,		MakeEventDelegate(this, &CMainFrame::OnEventLoginCompleteOperation));
	::CancelEvent(EVT_RELOGIN,		MakeEventDelegate(this, &CMainFrame::OnEventReLoginOperation));
	::CancelEvent(EVT_LOGOUT,		MakeEventDelegate(this, &CMainFrame::OnEventLogOutOperation));

	::CancelEvent(EVT_THREAD_CONTROL_PPT,	MakeEventDelegate(this, &CMainFrame::OnEventPPTControl));
	::CancelEvent(EVT_THREAD_INSERT_PPT,	MakeEventDelegate(this, &CMainFrame::OnEventPPTInsert));
	::CancelEvent(EVT_DANMUKU,	MakeEventDelegate(this, &CMainFrame::OnDanmukuWordAdd));
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	
	if( !CSkinFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	m_strTitle = _T("101教育PPT"); 
	SetWindowClass(cs);
	return TRUE;
}

void CMainFrame::SetWindowClass(CREATESTRUCT& cs)
{
//	WNDCLASS wc = {0};
	//::GetClassInfo(AfxGetInstanceHandle(), cs.lpszClass, &wc);
	//wc.lpszClassName = _T("PPTShell");
	//AfxRegisterClass(&wc);

}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CSkinFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CSkinFrameWnd::Dump(dc); 
}
#endif //_DEBUG

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CSkinFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_hMainWnd = m_hWnd;
	//download manager
	CDownloadManagerUI::GetInstance()->ShowManagerUI(false);

	CMenu *pMenu = GetMenu();  
	if( pMenu != NULL )  
	{  
		HMENU hMenu = pMenu->GetSafeHmenu();
		int Menucount = GetMenuItemCount(hMenu);

		for (int i = Menucount-1; i > -1; i--)
		{
			EnableMenuItem(hMenu, i, MF_DISABLED|MF_GRAYED);
			DeleteMenu(hMenu, i, MF_BYPOSITION);
		}

		m_bAutoMenuEnable = false;
	}  

	if (!m_wndMainPane.Create(_T(""), this, CRect(0, 0, 100, 600), FALSE, 1000,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS |WS_CLIPCHILDREN | CBRS_RIGHT ))  //CBRS_FLOAT_MULTI
	{
		return FALSE;
	} 

	m_wndMainPane.SetMinSize(CSize(100, 600));
	m_wndMainPane.SetResizeMode(FALSE);
	m_wndMainPane.EnableDocking(CBRS_ALIGN_RIGHT);
	EnableAutoHidePanes(CBRS_ALIGN_RIGHT);
	DockPane(&m_wndMainPane);// LEFT
	SetMainPaneMoveStatus( FALSE );
	CreateTitleButton();

	// event center
	EventCenter::GetInstance()->SetOwner(GetSafeHwnd());

	// http download message 
	HttpDownloadManager::GetInstance()->SetOwner(GetSafeHwnd());
	HttpUploadManager::GetInstance()->SetOwner(GetSafeHwnd());
	NDCloudFileManager::GetInstance()->SetOwner(GetSafeHwnd());

	PPTImagesExporter::GetInstance()->SetOwner(GetSafeHwnd());
	PPTPack::GetInstance()->SetOwner(GetSafeHwnd());
	
	//window to top
	AfxGetMainWnd()->CenterWindow();

	CPPTControllerManager::GetInstance()->SetOwner(this->m_hWnd);

	//Danmuku window
	m_lyricsWindow.Create();
	m_lyricsWindow.GoWords(RightTurn);
	::SetWindowPos(m_lyricsWindow.m_hWnd, HWND_TOPMOST,0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );


	//开启守护进程的开启事件
	m_nPPTDaemonEvent.PPTRun();
	//开启CEF消息接听
	CNdCefShareMemory* nCefShareMemory = NdCefShareMemory::GetInstance();
	nCefShareMemory->CreateThreadSetEvent();
//
#ifndef _DEBUG
	::SetWindowPos(AfxGetMainWnd()->GetSafeHwnd(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
#endif

	//SetForegroundWindow();

	SaveMainFrameHwnd();
	return 0;
}

void CMainFrame::OnClose()
{
	if ( m_bClosing || m_bUploadingOnClose )
		return;
	
	if( GetPPTController() != NULL )
	{
		if ( GetPPTController()->IsInitFailed() )
		{
			CloseRelateApp();
			return;
		}

		if ( !GetPPTController()->IsInit() )
		{
			OnDestroyBefore();
			return;
		}
		
		if (!GetPPTController()->OnApplicationClose())
		{
			return;
		}
	}

	m_bClosing	= TRUE;

	if ( IsIconic())
	{
		this->ShowWindow(SW_RESTORE);
		Sleep(200);
	}

	IsPPTChangedReadOnlyByThread(FALSE, PPTC_CLOSE);	
}

BOOL CMainFrame::OnDestroyBefore()
{
	if (m_pScreenInstrument != NULL)//2015.12.25 cws
	{
		m_pScreenInstrument->CloseInstrument();			
	}

	CloseVRMediaPlayer();
	Close3DAmination();
	
	NDCloudDestroy();
	NDCloudLocalQuestionManager::GetInstance()->Destroy();

	IcrPlayer::GetInstance()->Destroy();

	if ( GetPPTController()->IsInit() )
		CloseAPPByThread();
	else
		CloseRelateApp();

	return TRUE;
}

void CMainFrame::CloseRelateApp()
{
	try
	{
		//::SendMessage(CoursePlayUI::GetInstance()->GetHWND(), WM_CLOSE, 0, 0);
		KillExeCheckParentPid(COURSE_PLAYER_EXE_NAME, TRUE);
		KillExeCheckParentPid(UNZIP_EXE_NAME);
		
		int nKilled = 1;
		int nCount	= 0;
		while ( nKilled != 0  && nCount < 50 )	//some computer kill failed (win8)
		{
			nKilled = KillExeCheckParentPid(PPT_EXE_NAME, FALSE);
			nCount++;
			Sleep(10);
		}

		CCefObject* pObejct = CefObject::GetInstance();
		pObejct->Destroy();

		m_update.SetExit();
		//开启守护进程的关闭事件
		CNdCefShareMemory* nCefShareMemory = NdCefShareMemory::GetInstance();
		nCefShareMemory->SetExit();
		m_nPPTDaemonEvent.SetExit();
		m_lyricsWindow.StopWords();
		m_lyricsWindow.CloseWindow();
		Sleep(100);

	}
	catch (...)
	{
	}
	//__super::OnClose();
	TerminateProcess(GetCurrentProcess(),0);
}

BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam)
{
	if (hWnd) //如果子窗口存在 
	{ 
		TCHAR szText[MAX_PATH]		= {0};
		TCHAR szClassName[MAX_PATH] = {0};

		GetWindowText(hWnd, szText, MAX_PATH);//获取窗口的标题
		GetClassName(hWnd, szClassName, MAX_PATH);
		
		if ( _tcsstr(szClassName, _T("Afx:Slider")) != NULL)
		{
			*((DWORD*)lParam) = (DWORD)hWnd;
			return FALSE;
		}
		
		return   TRUE;   
	}   
	return   FALSE;
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)   
{   
	WNDINFO* pInfo = (WNDINFO*)lParam;   
	DWORD dwProcessId = 0;   
	GetWindowThreadProcessId(hWnd, &dwProcessId);   

	if(dwProcessId == pInfo->dwPid)   
	{   
		pInfo->hWnd = hWnd;   
		return FALSE;   
	}   
	return TRUE;   
}   

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CPPTShellView* pView = (CPPTShellView*)GetActiveView();
	if( pView != NULL )
		pView->OnSize(nType, cx, cy);

	CDownloadManagerUI::GetInstance()->UpdatePos();

	if( GetPPTController() != NULL )
	{
		if (CPPTController::GetPPTVersion() >= PPTVER_2010)
		{
			HWND hFull		= ::FindWindowEx(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), NULL, _T("FullpageUIHost"), NULL);
			if (hFull)
			{	
				::MoveWindow(hFull, 0, -6, cx, cy + 6, TRUE);
				::SetForegroundWindow(hFull);

			}
		}

		if (m_wndMainPane)
		{
			CRect crtPanel;
			m_wndMainPane.GetClientRect(crtPanel);
			GetPPTController()->OnSize(nType, cx - crtPanel.Width(), cy);
		}
		else
		{
			GetPPTController()->OnSize(nType, cx, cy);
		}
	}

	CUpdateTipDialogUI* pDlg = UpdateTipDialogUI::GetInstance();
	if(pDlg->GetHWND() && ::IsWindowVisible(pDlg->GetHWND()))
	{
		::PostMessage(GetSafeHwnd(), WM_SHOW_UPDATETIP, NULL, NULL);
	}
	
	ShowAddSlideWindow();
	MoveExercisesEditWindow();
	MoveVRDistributeWindow();
	CCoursePlayUI * pCoursePlayUI = CoursePlayUI::GetInstance();
	pCoursePlayUI->SetClientRect();
	CSkinFrameWnd::OnSize(nType, cx, cy);
}

BOOL CMainFrame::OnCommand( WPARAM wParam, LPARAM lParam )
{
	//事件通知消息处理
	if ( wParam == COMMAND_ID_PLAY )
	{
		m_h3DWnd = NULL;

		GetQuestionListByThread();
		if (!m_pScreenInstrument)
		{
			m_pScreenInstrument = new CScreenInstrumentUI;
		}
		m_pScreenInstrument->ShowInstrument(HWND_DESKTOP, lParam);
		//m_pScreenInstrument->ShowInstrument(AfxGetMainWnd()->GetSafeHwnd());
		WRITE_LOG_LOCAL("CMainFrame::OnCommand, COMMAND_ID_PLAY");
		BroadcastPostedEvent(EVT_PPT_INFO_SYNC, PPT_PLAY_BEGIN, 0);
	
	}
	else if ( wParam == COMMAND_ID_STOP )
	{
		BroadcastPostedEvent(EVT_PPT_INFO_SYNC, PPT_PLAY_END, 0);
		
		if ( m_h3DWnd != NULL )
			::SendMessage(m_h3DWnd, WM_CLOSE, 0, 0);

		m_h3DWnd = NULL;

		if (m_pScreenInstrument)
		{
			m_pScreenInstrument->CloseInstrument();
			m_pScreenInstrument = NULL;
		}

		CloseVRMediaPlayer();
		Close3DAmination();

		//SetForegroundWindow();
		IcrPlayer::GetInstance()->IcrStop();

		//修复视频缩略图问题
		if (GetPPTController()->GetPPTVersion()<=PPTVER_2010)
		{
			GetPPTController()->RepairPPTBugVideoThumb();
		}

	}
	else if ( wParam == COMMAND_ID_NEXT )
	{
		//OnShowNextSlide(NULL);
		RestoreFlashFrameNumByThread();
		GetVideoListByThread();

		CStream* pStream	= (CStream*) lParam;
		pStream->ResetCursor();

		DWORD dwData		=  pStream->ReadDWORD();
		tstring str3DPath	= pStream->ReadString();

		delete pStream;

		int nCurCount = LOWORD(dwData);

		if (m_pScreenInstrument)
		{
			m_pScreenInstrument->GetPagger()->SetCurrentPage(nCurCount);
		}
		
		// notify user
		if( nCurCount != 0 )
			BroadcastPostedEvent(EVT_PPT_INFO_SYNC, PPT_PLAY_SLIDE, nCurCount);
 
		IcrPlayer::GetInstance()->IcrOnPageIndexChange(nCurCount, nCurCount-1);

		Show3DAmination(str3DPath);
		CloseVRMediaPlayer();
		
	}
	else if ( wParam == COMMAND_ID_INSIDE_PPT_ENTER )
	{
		if (m_pScreenInstrument)
		{
			m_pScreenInstrument->GetPagger()->SetInsideMode(true);
			m_pScreenInstrument->GetPagger()->SetPageCount(lParam);
			m_pScreenInstrument->GetPagger()->CreateThumbnailList();
		}
	}
	else if ( wParam == COMMAND_ID_INSIDE_PPT_LEAVE )
	{
		if (m_pScreenInstrument)
		{
			m_pScreenInstrument->GetPagger()->SetInsideMode(false);
			m_pScreenInstrument->GetPagger()->SetPageCount(lParam);
			m_pScreenInstrument->GetPagger()->CreateThumbnailList();
		}
	}
	else if ( wParam == COMMAND_INIT_UNTITLEWINDOW )
	{
		CPPTShellView* pView = (CPPTShellView*)GetActiveView();
		pView->CreateUntitleWindow();

		HideAddSlideWindow();
		HideExercisesEditWindow();
		HideVRDistributeWindow();
	}
	else if ( wParam == COMMAND_OPEN_FORM_DESKTOP )
	{
		OpenDocByDesktop();
	}
	else
	{
		//CMainFrame* mainFrame = (CMainFrame*)AfxGetMainWnd();
		//mainFrame->m_lyricsWindow.AddString((LPWSTR)L"测试");

		if ( m_pMenu && m_pMenu->GetHWND() != NULL)
		{
			m_pMenu->Close();
			m_pMenu = NULL;
		}
		PostMessage(WM_TITLEBUTTON_OPERATION, wParam, lParam);
	}

	//BOOL bRet = CSkinFrameWnd::OnCommand(wParam, lParam);

	m_pMainPPTController->TopShowSlideView();

	return 1;
}
BOOL CMainFrame::ShowVRMediaPlayer(tstring strPath,tstring nParam)
{
	CResGuard::CGuard gd(m_Lock);
	
	if ( strPath.empty() )
		return FALSE;

	PROCESS_INFORMATION pi;
	STARTUPINFO si;      //隐藏进程窗口
	ZeroMemory( &pi, sizeof(pi) );
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);

	TCHAR szParam[MAX_PATH*2]={0};

	wsprintf(szParam, _T(" \"%s\""), nParam.c_str());

	if (CreateProcess(strPath.c_str(),szParam,  NULL, // process info
		NULL, // thread info      
		FALSE, // inheritable      
		0, // creation flags     
		NULL, // environment       
		NULL, // current directory     
		&si, & pi)) 
	{ 
		CloseHandle(pi.hProcess); 
		CloseHandle(pi.hThread);
		HWND hWnd = ::FindWindow(NULL,  _T("VRMediaPlayer"));
		if (hWnd){
			::SetParent(hWnd, CPPTController::GetSlideShowViewHwnd());
		}
		return TRUE;
	}

	return FALSE;

}

BOOL CMainFrame::CloseVRMediaPlayer()
{

	HWND hWnd = NULL;

	int ncout = 0;
	while( (hWnd = ::FindWindow(NULL,  _T("VRMediaPlayer"))) != NULL )
	{
		::PostMessage(hWnd, WM_CLOSE, 0, 0);//WM_VRMEDIAPLAYCLOSE
		Sleep(100);
		if (ncout>5){
			break;
		}
	}

	return TRUE;
}
BOOL CMainFrame::Show3DAmination(tstring strPath)
{
	CResGuard::CGuard gd(m_Lock);
	
	if ( m_h3DWnd != NULL )
		::SendMessage(m_h3DWnd, WM_CLOSE, 0, 0);

	HWND hWnd = ::FindWindow(_T("screenClass"), NULL);
	while( hWnd != NULL )
	{
		hWnd = ::FindWindowEx(hWnd, NULL, _T("UnityWndClass"), NULL);
		if (hWnd != NULL)
			::SendMessage(hWnd, WM_CLOSE, 0, 0);
	}

	m_h3DWnd = NULL;
	
	if ( strPath.empty() )
		return FALSE;

	PROCESS_INFORMATION pi;
	STARTUPINFO si;      //隐藏进程窗口
	ZeroMemory( &pi, sizeof(pi) );
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);

	if (CreateProcess(strPath.c_str(), NULL,  NULL, // process info
		NULL, // thread info      
		FALSE, // inheritable      
		0, // creation flags     
		NULL, // environment       
		NULL, // current directory     
		&si, & pi)) 
	{ 

		WNDINFO info	= {0};   
		info.hWnd		= NULL;   
		info.dwPid		= pi.dwProcessId;

		CloseHandle(pi.hProcess); 
		CloseHandle(pi.hThread);

		Sleep(100);
		while (info.hWnd == NULL )
		{	
			EnumWindows(EnumWindowsProc, (LPARAM)&info);
			Sleep(1000);
		}
		int nWidth;
		int nHeight;
		GetScreenResolution(nWidth, nHeight);

		m_h3DWnd = info.hWnd;
		//::SetWindowLong(m_h3DWnd, GWL_STYLE, WS_VISIBLE);
		::SetWindowPos(m_h3DWnd, HWND_TOPMOST, 0, 0, nWidth, nHeight, SWP_SHOWWINDOW);
		::SetParent(m_h3DWnd, CPPTController::GetSlideShowViewHwnd());

		return TRUE;
	}

	return FALSE;

}

BOOL CMainFrame::Close3DAmination()
{
	HWND hWnd = ::FindWindow(_T("screenClass"), NULL);
	while( hWnd != NULL )
	{
		hWnd = ::FindWindowEx(hWnd, NULL, _T("UnityWndClass"), NULL);
		if (hWnd != NULL)
			::SendMessage(hWnd, WM_CLOSE, 0, 0);
	}

	while( (hWnd = ::FindWindow(_T("UnityWndClass"), NULL)) != NULL )
	{
		::SendMessage(hWnd, WM_CLOSE, 0, 0);
		Sleep(100);
	}

	return TRUE;
}


int CMainFrame::SavePPT(int nCommand, tstring strFilePath, tstring strFileName, BOOL bBefore, BOOL bReadOnly, BOOL bOpen, BOOL bClosePre, BOOL bICRPlay)
{
	int nRet = IDNO;

	CPPTShellDoc* pDoc = (CPPTShellDoc*)this->GetActiveDocument();

	if ( pDoc != NULL)
	{
		
		TCHAR szInfo[MAX_PATH] = {0};

		if ( bReadOnly )
		{
			wsprintf(szInfo, _T("[%s]为只读打开, 是否另存为新的文件?"), strFileName.c_str());   //pDoc->GetTitle()  zcs
			nRet = UIMessageBox(m_hWnd, szInfo, _T("101教育PPT"), bOpen ? _T("是,否"):_T("是,否,取消"), CMessageBoxUI::enMessageBoxTypeQuestion, IDCANCEL);

		}
		else
		{
			if(bICRPlay)
			{
				nRet = ID_MSGBOX_BTN;
			}
			else
			{
				wsprintf(szInfo, _T("是否保存对[%s]的更改?"), strFileName.c_str());  //pDoc->GetTitle()  zcs
				nRet = UIMessageBox(m_hWnd, szInfo, _T("101教育PPT"), bOpen ? _T("是,否"):_T("是,否,取消"), CMessageBoxUI::enMessageBoxTypeQuestion, IDCANCEL);

			}
		}
		
		if (nRet == ID_MSGBOX_BTN)
		{
			
			if ( bReadOnly )
			{
				BOOL bDiff = FALSE;

				CString strOldPath = strFilePath.c_str(); //pDoc->GetPathName();
				CString strNewPath = _T("");
				wsprintf(szInfo, _T("演示文稿[%s]另存为:"), strFileName.c_str());
				BOOL bFail = FALSE;
				while ( !bDiff ) 
				{
					//保存新文件
					tstring strFile = _T("");
					BOOL bRet = FileSaveDialog(szInfo, strFileName.c_str(), FILTER_PPT,
						OFN_FILEMUSTEXIST|OFN_NONETWORKBUTTON|OFN_PATHMUSTEXIST|OFN_EXPLORER,
						strFile, AfxGetMainWnd()->m_hWnd);

					if( bRet )//是否打开成功
					{ 
						strNewPath = strFile.c_str();

						if (strOldPath.CompareNoCase(strNewPath) != 0)
						{
							bDiff = TRUE;
							nRet = PPTSaving(strNewPath, bBefore, bClosePre, bICRPlay, nCommand); //pDoc->OnSaveDocument(strNewPath);
							return IDYES;
						}
					}
					else
					{
						bDiff = TRUE;
						return IDCANCEL;
					}
					//bFail = TRUE;
				} 
			}
			else
			{

				//if ( !pDoc->GetPathName().IsEmpty() ) //文件是已存在的
				if ( !strFilePath.empty() )
				{
					nRet = PPTSaving(strFilePath.c_str(), bBefore, bClosePre, bICRPlay,  nCommand);  //pDoc->OnSaveDocument(pDoc->GetPathName());
					return IDYES;
				}
				else
				{
					//保存新文件
					tstring strFile = _T("");
					BOOL bRet = FileSaveDialog(_T("保存演示文稿"), strFileName.c_str(), FILTER_PPT,
						OFN_FILEMUSTEXIST|OFN_NONETWORKBUTTON|OFN_PATHMUSTEXIST|OFN_EXPLORER,
						strFile, AfxGetMainWnd()->m_hWnd);
					if ( bRet )//是否打开成功
					{
						//CString strPath = FileDlg.GetPathName();
						DWORD dwRet = GetFileAttributes(strFile.c_str());

						if (dwRet != INVALID_FILE_ATTRIBUTES)
						{
							nRet = UIMessageBox(m_hWnd, _T("文件已存在，是否覆盖?"), _T("101教育PPT"), _T("是,否"), CMessageBoxUI::enMessageBoxTypeWarn, IDCANCEL);
						}

						if (nRet == ID_MSGBOX_BTN)
						{
							nRet = PPTSaving(strFile.c_str(), bBefore, bClosePre, bICRPlay, nCommand);//pDoc->OnSaveDocument(strPath);
							return IDYES;
						}
						else
							return IDCANCEL;
					}
				}
			}
		}
		else if ( nRet == ID_MSGBOX_BTN + 1 )
			return IDNO;
		else if ( nRet == ID_MSGBOX_BTN + 2 )
			return IDCANCEL;
	}
	
	return nRet;
}

int CMainFrame::SavePPT4Close(int nCommand, tstring strFilePath, tstring strFileName, BOOL bBefore, BOOL bReadOnly, BOOL bOpen, BOOL bClosePre, BOOL bICRPlay)
{
	int nRet = IDNO;

	CPPTShellDoc* pDoc = (CPPTShellDoc*)this->GetActiveDocument();

	if ( pDoc != NULL)
	{

		TCHAR szInfo[MAX_PATH] = {0};

		if ( bReadOnly )
		{
			wsprintf(szInfo, _T("[%s]为只读打开, 是否另存为新的文件?"), strFileName.c_str());   //pDoc->GetTitle()  zcs
			if ( NDCloudUser::GetInstance()->GetUserId() == 0 )
			{
				nRet = UIMessageBox(m_hWnd, szInfo, _T("101教育PPT"), bOpen ? _T("是,否"):_T("是,否,取消"), CMessageBoxUI::enMessageBoxTypeQuestion, IDCANCEL);
			}
			else
			{
				nRet = UIMessageBoxEx2(m_hWnd, szInfo, _T("101教育PPT"), bOpen ? _T("是,否"):_T("是,否,取消"), CMessageBoxUIEx2::enMessageBoxTypeQuestion, IDCANCEL);
			}

		}
		else
		{
			if(bICRPlay)
			{
				nRet = ID_MSGBOX_BTN;
			}
			else
			{
				wsprintf(szInfo, _T("是否保存对[%s]的更改?"), strFileName.c_str());  //pDoc->GetTitle()  zcs
				if ( NDCloudUser::GetInstance()->GetUserId() == 0 )
				{
					nRet = UIMessageBox(m_hWnd, szInfo, _T("101教育PPT"), bOpen ? _T("是,否"):_T("是,否,取消"), CMessageBoxUI::enMessageBoxTypeQuestion, IDCANCEL);
				}
				else
				{
					nRet = UIMessageBoxEx2(m_hWnd, szInfo, _T("101教育PPT"), bOpen ? _T("是,否"):_T("是,否,取消"), CMessageBoxUIEx2::enMessageBoxTypeQuestion, IDCANCEL);
				}

			}
		}

		if (nRet == ID_MSGBOX_BTN)
		{

			if ( bReadOnly )
			{
				BOOL bDiff = FALSE;

				CString strOldPath = strFilePath.c_str(); //pDoc->GetPathName();
				CString strNewPath = _T("");
				wsprintf(szInfo, _T("演示文稿[%s]另存为:"), strFileName.c_str());
				BOOL bFail = FALSE;
				while ( !bDiff ) 
				{
					//保存新文件
					tstring strFile = _T("");
					BOOL bRet = FileSaveDialog(szInfo, strFileName.c_str(), FILTER_PPT,
						OFN_FILEMUSTEXIST|OFN_NONETWORKBUTTON|OFN_PATHMUSTEXIST|OFN_EXPLORER,
						strFile, AfxGetMainWnd()->m_hWnd);

					if( bRet )//是否打开成功
					{ 
						strNewPath = strFile.c_str();

						if (strOldPath.CompareNoCase(strNewPath) != 0)
						{
							bDiff = TRUE;
							m_nSaveToSkyType = 1;
							nRet = PPTSaving(strNewPath, bBefore, bClosePre, bICRPlay, nCommand); //pDoc->OnSaveDocument(strNewPath);
							return IDYES;
						}
					}
					else
					{
						bDiff = TRUE;
						return IDCANCEL;
					}
					//bFail = TRUE;
				}

			}
			else
			{

				//if ( !pDoc->GetPathName().IsEmpty() ) //文件是已存在的
				if ( !strFilePath.empty() )
				{
					m_nSaveToSkyType = 1;
					nRet = PPTSaving(strFilePath.c_str(), bBefore, bClosePre, bICRPlay,  nCommand);  //pDoc->OnSaveDocument(pDoc->GetPathName());
					return IDYES;
				}
				else
				{
					//保存新文件
					tstring strFile = _T("");
					BOOL bRet = FileSaveDialog(_T("保存演示文稿"), strFileName.c_str(), FILTER_PPT,
						OFN_FILEMUSTEXIST|OFN_NONETWORKBUTTON|OFN_PATHMUSTEXIST|OFN_EXPLORER,
						strFile, AfxGetMainWnd()->m_hWnd);
					if ( bRet )//是否打开成功
					{
						//CString strPath = FileDlg.GetPathName();
						DWORD dwRet = GetFileAttributes(strFile.c_str());

						if (dwRet != INVALID_FILE_ATTRIBUTES)
						{
							nRet = UIMessageBox(m_hWnd, _T("文件已存在，是否覆盖?"), _T("101教育PPT"), _T("是,否"), CMessageBoxUI::enMessageBoxTypeWarn, IDCANCEL);
						}

						if (nRet == ID_MSGBOX_BTN)
						{
							m_nSaveToSkyType = 1;
							nRet = PPTSaving(strFile.c_str(), bBefore, bClosePre, bICRPlay, nCommand);//pDoc->OnSaveDocument(strPath);
							return IDYES;
						}
						else
							return IDCANCEL;
					}
				}
			}

		}
		else if ( nRet == ID_MSGBOX_BTN + 1 )
		{
			m_nSaveToSkyType = 1;
			return IDNO;
		}

		else if ( nRet == ID_MSGBOX_BTN + 2 )
			return IDCANCEL;
		else if ( nRet == ID_MSGBOX_BTN + 3)
		{
			if ( bReadOnly )
			{
				BOOL bDiff = FALSE;

				CString strOldPath = strFilePath.c_str(); //pDoc->GetPathName();
				CString strNewPath = _T("");
				wsprintf(szInfo, _T("演示文稿[%s]另存为:"), strFileName.c_str());
				BOOL bFail = FALSE;
				while ( !bDiff ) 
				{
					//保存新文件
					tstring strFile = _T("");
					BOOL bRet = FileSaveDialog(szInfo, strFileName.c_str(), FILTER_PPT,
						OFN_FILEMUSTEXIST|OFN_NONETWORKBUTTON|OFN_PATHMUSTEXIST|OFN_EXPLORER,
						strFile, AfxGetMainWnd()->m_hWnd);

					if( bRet )//是否打开成功
					{ 
						strNewPath = strFile.c_str();

						if (strOldPath.CompareNoCase(strNewPath) != 0)
						{
							bDiff = TRUE;
							m_nSaveToSkyType = 2;
							nRet = PPTSaving(strNewPath, bBefore, bClosePre, bICRPlay, nCommand); //pDoc->OnSaveDocument(strNewPath);
							return IDYES;
						}
					}
					else
					{
						bDiff = TRUE;
						return IDCANCEL;
					}
					//bFail = TRUE;
				} 
			}
			else
			{

				//if ( !pDoc->GetPathName().IsEmpty() ) //文件是已存在的
				if ( !strFilePath.empty() )
				{
					m_nSaveToSkyType = 2;
					nRet = PPTSaving(strFilePath.c_str(), bBefore, bClosePre, bICRPlay,  nCommand);  //pDoc->OnSaveDocument(pDoc->GetPathName());
					return IDYES;
				}
				else
				{
					//保存新文件
					tstring strFile = _T("");
					BOOL bRet = FileSaveDialog(_T("保存演示文稿"), strFileName.c_str(), FILTER_PPT,
						OFN_FILEMUSTEXIST|OFN_NONETWORKBUTTON|OFN_PATHMUSTEXIST|OFN_EXPLORER,
						strFile, AfxGetMainWnd()->m_hWnd);
					if ( bRet )//是否打开成功
					{
						//CString strPath = FileDlg.GetPathName();
						DWORD dwRet = GetFileAttributes(strFile.c_str());

						if (dwRet != INVALID_FILE_ATTRIBUTES)
						{
							nRet = UIMessageBox(m_hWnd, _T("文件已存在，是否覆盖?"), _T("101教育PPT"), _T("是,否"), CMessageBoxUI::enMessageBoxTypeWarn, IDCANCEL);
							if (nRet == ID_MSGBOX_BTN)
							{
								m_nSaveToSkyType = 2;
								nRet = PPTSaving(strFile.c_str(), bBefore, bClosePre, bICRPlay, nCommand);//pDoc->OnSaveDocument(strPath);
								return IDYES;
							}
							else
								return IDCANCEL;
						}

						if (nRet == ID_MSGBOX_BTN + 3)
						{
							m_nSaveToSkyType = 2;
							nRet = PPTSaving(strFile.c_str(), bBefore, bClosePre, bICRPlay, nCommand);//pDoc->OnSaveDocument(strPath);
							return IDYES;
						}
						else
							return IDCANCEL;
					}
				}
			}

		}
		else if ( nRet == ID_MSGBOX_BTN + 4)
		{
			m_nSaveToSkyType = 1;
			return IDNO;
		}
	}

	return nRet;
}


int CMainFrame::SelectPlayMode()
{
	int nRet = UIMessageBoxEx(m_hWnd, _T("是否启动互动课堂程序播放PPT?"), _T("101教育PPT"), _T("取消,预览,上课"), CMessageBoxUIEx::enMessageBoxTypeCenterPic, IDCANCEL);//2015.11.03 cws
	return nRet;
}

int	CMainFrame::SavePPTToSkyDrive()
{
	if ( NDCloudUser::GetInstance()->GetUserId() == 0 )
		return IDNO;

	int nRet = UIMessageBox(m_hWnd, _T("是否保存当前课件到“我的网盘”?"), _T("101教育PPT"), _T("是,否,取消"), CMessageBoxUI::enMessageBoxTypeInfo, IDNO);
	
	if ( nRet == ID_MSGBOX_BTN )
		return IDYES;
	else if(nRet == ID_MSGBOX_BTN + 1)
		return IDNO;
	else
		return IDCANCEL;
}


void CMainFrame::PublishPPT()
{
	string str = ::GetPPTController()->GetPresentationName();

	CPPTShellDoc* pDoc = (CPPTShellDoc*)this->GetActiveDocument();
	CString strPPTName = pDoc->GetTitle();

	int pos = strPPTName.ReverseFind('.');
	if( pos != -1 )
	{
		strPPTName = strPPTName.Mid(0, pos);
		strPPTName = strPPTName + ".exe";
	}

	// saves
	CFileDialog openFileDlg(FALSE, "", 
		strPPTName, 
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,  
		"独立课件文件 (*.exe)|*.exe||", 
		NULL);  
	openFileDlg.m_ofn.lpstrTitle = _T("生成独立课件");
	if( openFileDlg.DoModal() != IDOK )
		return;

	CString strSavePath = openFileDlg.GetPathName();

	//
	char szDir[MAX_PATH];
	GetModuleFileName(NULL, szDir, MAX_PATH);

	char* p = strrchr(szDir, '\\');
	*p = '\0';


	char szTempPath[MAX_PATH];
	GetTempPath(MAX_PATH, szTempPath);

	TCHAR szPptFilePath[MAX_PATH];
	_stprintf_s(szPptFilePath, _T("%s\\test.ppt"), szTempPath);

	// save copy to temp path
	::GetPPTController()->SaveCopyAs(szPptFilePath);

	TCHAR szExePath[MAX_PATH];
	_stprintf_s(szExePath, _T("%s\\bin\\PPTViewer.dll"), szDir);


	// exe 
	FILE* fp = NULL;
	errno_t err = _tfopen_s(&fp, szExePath, "rb");
	if( err != 0 )
		return;
		
	
	fseek(fp, 0, SEEK_END);
	int exesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char* pExeFile = new char[exesize];
	fread(pExeFile, exesize, 1, fp);
	fclose(fp);

	// ppt
	err = _tfopen_s(&fp, szPptFilePath, "rb");
	if( err != 0 )
		return;

	fseek(fp,0, SEEK_END);
	int pptsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char* pPptFile = new char[pptsize];
	fread(pPptFile, pptsize, 1, fp);
	fclose(fp);

	// combine
	err = _tfopen_s(&fp, strSavePath.GetString(), "wb");
	if( err != 0 )
		return;

	fwrite(pExeFile, exesize, 1, fp);
	fwrite(pPptFile, pptsize, 1, fp);

	// write offset
	fwrite(&pptsize, 4, 1, fp);
	fclose(fp);

	delete pExeFile;
	delete pPptFile;
}

void CMainFrame::OnMove( int x, int y )
{
	CUpdateTipDialogUI* pDlg = UpdateTipDialogUI::GetInstance();
	if(pDlg->GetHWND() && ::IsWindowVisible(pDlg->GetHWND()))
	{
		::PostMessage(GetSafeHwnd(), WM_SHOW_UPDATETIP, NULL, NULL);
	}

	ShowAddSlideWindow();
	MoveExercisesEditWindow();
	MoveVRDistributeWindow();

	CDownloadManagerUI::GetInstance()->UpdatePos();

	__super::OnMove(x, y);

}

void CMainFrame::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	DrawSliderBkgnd();
}


void CMainFrame::CreateTitleButton()
{
	CDUIButton* pButton		= NULL;
	Image*		pIcon		= NULL;	
	Image*		pImage		= NULL;
	Image*		pTipIcon	= CSkinManager::GetInstance()->GetSkinItem(_T("tip_test.png"));

	CRect rcNCButton;
	GetWindowRect(&rcNCButton);
	rcNCButton.OffsetRect( -rcNCButton.left, -rcNCButton.top);

	if (0)//先屏蔽该按钮
	{
		rcNCButton.left		= 140 - 70;
		rcNCButton.top		= 12;
		rcNCButton.right	= rcNCButton.left - 30 ;
		rcNCButton.bottom	= rcNCButton.top + 30;

		//靠右
		//关于
		rcNCButton.OffsetRect(70,0);
		rcNCButton.right	= rcNCButton.left - 30;
		pIcon		= CSkinManager::GetInstance()->GetSkinItem(_T("icon_menu.png"));
		m_pAboutBtn = AddTitleButton(&rcNCButton, pIcon, 3, IDC_TITLEBUTTON_ABOUT, NULL, TRUE, TRUE, FALSE);
		//m_pAboutBtn->SetIconInfo(pIcon, 3, CSize(30, 30));
	}
	else
	{
		rcNCButton.left		= 110;
		rcNCButton.top		= 12;
		rcNCButton.right	= rcNCButton.left - 30 ;
		rcNCButton.bottom	= rcNCButton.top + 30; 
	}

	//pIcon		= CSkinManager::GetInstance()->GetSkinItem(_T("icon_dot.png"));
	//pButton->SetIconInfo(pIcon, 1, CSize(6, 6), CPoint(20, 5));

	pImage = CSkinManager::GetInstance()->GetSkinItem(_T("button_bg.png"));

#ifdef DEVELOP_VERSION
	//下载管理器
	rcNCButton.top		= 0;
	rcNCButton.bottom	= rcNCButton.top + 50; 

	rcNCButton.OffsetRect(40, 0);
	rcNCButton.right	= rcNCButton.left - 40;

	pIcon	= CSkinManager::GetInstance()->GetSkinItem(_T("icon_download.png"));
	m_pDownloadMgr = AddTitleButton(&rcNCButton, pIcon, 3, IDC_TITLEBUTTON_DOWNLOADMGR, _T(""), TRUE, TRUE, TRUE);

	m_pDownloadMgr->SetCheckBox(TRUE);

	rcNCButton.OffsetRect(15, 0);
	rcNCButton.top		= 12;
	rcNCButton.bottom	= rcNCButton.top + 30; 
#endif

	//登录
	if(g_Config::GetInstance()->GetModuleVisible(MODULE_LOGIN))
	{
		rcNCButton.OffsetRect(35,0);
		rcNCButton.right	= rcNCButton.left - 30;
		m_pUserLoginBtn = AddTitleButton(&rcNCButton, NULL, 3, IDC_TITLEBUTTON_USERLOGIN, _T("登录"), TRUE, TRUE, TRUE);
		pIcon	= CSkinManager::GetInstance()->GetSkinItem(_T("btn_about.png"));
		m_pUserLoginBtn->SetIconInfo(pIcon, 3, CSize(30, 30));

		// add right conrner tip
		m_pUserLoginBtn->SetTipIconInfo(pTipIcon, 1, CPoint(5, -5));
		m_pUserLoginBtn->SetAutoSize(TRUE);
		m_pUserLoginBtn->SetMaxWidth(120);
	} 

	//下一节
	rcNCButton.OffsetRect(35,0);
	rcNCButton.right	= rcNCButton.left - 30;
	pIcon				= CSkinManager::GetInstance()->GetSkinItem(_T("btn_next.png"));
	m_pNextBtn			= AddTitleButton(&rcNCButton, pIcon, 4, IDC_TITLEBUTTON_NEXT, NULL, TRUE, TRUE, TRUE);
	//m_pNextBtn->EnableButton(FALSE);

	//课程名
	rcNCButton.OffsetRect(80,0);
	rcNCButton.right	= rcNCButton.left - 76; 
	pIcon				= CSkinManager::GetInstance()->GetSkinItem(_T("icon_preparation.png")); //icon_lesson
	m_pChapterBtn		= AddTitleButton(&rcNCButton, NULL, 3, IDC_TITLEBUTTON_LESSON, _T(""), TRUE, TRUE, TRUE);
	m_pChapterBtn->SetIconInfo(pIcon, 3, CSize(76, 24), CPoint(0, -1));
	m_pChapterBtn->SetStatusTextInfo(DS_NORMAL, 0xFFFFFFFF);
	m_pChapterBtn->SetAutoSize(TRUE);
	m_pChapterBtn->SetDivison(TRUE);
	//m_pChapterBtn->EnableButton(FALSE);

	//上一节
	rcNCButton.OffsetRect(35,0);
	rcNCButton.right	= rcNCButton.left - 30;
	pIcon				= CSkinManager::GetInstance()->GetSkinItem(_T("btn_previous.png"));
	m_pPrevBtn			= AddTitleButton(&rcNCButton, pIcon, 4, IDC_TITLEBUTTON_PREVIOUS, NULL, TRUE, TRUE, TRUE);
	//m_pPrevBtn->EnableButton(FALSE);

	//
	//	靠左
	//

	rcNCButton.left		= 10;
	rcNCButton.top		= 0;
	rcNCButton.right	= rcNCButton.left + 50;
	rcNCButton.bottom	= rcNCButton.top + 50;

	pImage = CSkinManager::GetInstance()->GetSkinItem(_T("logo.png"));

	pButton = AddTitleButton(&rcNCButton, pImage, 1, IDC_TITLEBUTTON_LOGO, _T(""), TRUE, FALSE, TRUE);
	rcNCButton.OffsetRect(60,0);
	//rcNCButton.right	= rcNCButton.left + 70;
	TCHAR szToolTip[MAX_PATH] = {""};
#ifdef DEVELOP_VERSION
	_stprintf(szToolTip, "101教育PPT %s\r\n%s",g_Config::GetInstance()->GetVersion().c_str(),_STR_BUILD_TIME); 
#else
	_stprintf(szToolTip, "101教育PPT %s\r\n%s",g_Config::GetInstance()->GetVersion().c_str(),_STR_FINAL_TIME); 
#endif

	
	pButton->SetTipText(szToolTip);
	
	//rcNCButton.left		= 10;
	rcNCButton.top		= 12;
	rcNCButton.right	= rcNCButton.left + 75;
	rcNCButton.bottom	= rcNCButton.top + 30;


	pImage = CSkinManager::GetInstance()->GetSkinItem(_T("button_bg.png"));

	//2015.11.05 CWS
	if(g_Config::GetInstance()->GetModuleVisible(MODULE_MOBILE_CONNECT))
	{ 
		rcNCButton.right	= rcNCButton.left + 93;
		rcNCButton.bottom	= rcNCButton.top + 30;
		
		pButton = AddTitleButton(&rcNCButton, pImage, 3, IDC_TITLEBUTTON_PHONE, _T("连接手机"), TRUE, FALSE, TRUE);
		pIcon	= CSkinManager::GetInstance()->GetSkinItem(_T("icon_phone.png"));
		pButton->SetIconInfo(pIcon, 3, CSize(20, 20));
		pButton->EnableButton(FALSE);
		pButton->SetDivison(TRUE);

		// add right conrner tip
		pButton->SetTipIconInfo(pTipIcon, 1, CPoint(0, -6));
		
		//setting next button
		rcNCButton.OffsetRect(100,0);
		rcNCButton.right	= rcNCButton.left + 63;
	} 

	pButton = AddTitleButton(&rcNCButton, pImage, 3, IDC_TITLEBUTTON_OPEN, _T("打开"), TRUE, FALSE, TRUE);
	pIcon	= CSkinManager::GetInstance()->GetSkinItem(_T("icon_open.png"));
	pButton->SetIconInfo(pIcon, 3, CSize(20, 20));
	pButton->SetDivison(TRUE);

	//导出
	rcNCButton.OffsetRect(70,0);
	rcNCButton.right	= rcNCButton.left + 63;

	pButton = AddTitleButton(&rcNCButton, pImage, 3, IDC_TITLEBUTTON_PACKAGE_LESSION, _T("打包"), TRUE, FALSE, TRUE);
	pIcon	= CSkinManager::GetInstance()->GetSkinItem(_T("icon_package.png"));
	pButton->SetIconInfo(pIcon, 3, CSize(20, 20));
	pButton->SetDivison(TRUE);

	// add right conrner tip
	pButton->SetTipIconInfo(pTipIcon, 1, CPoint(0, -6));


	rcNCButton.OffsetRect(70,0);
	rcNCButton.right	= rcNCButton.left + 93;

	pButton = AddTitleButton(&rcNCButton, pImage, 3, IDC_TITLEBUTTON_INSERT_PICTURE, _T("插入图片"), TRUE, FALSE, TRUE);
	pIcon	= CSkinManager::GetInstance()->GetSkinItem(_T("icon_insert.png"));
	pButton->SetIconInfo(pIcon, 3, CSize(20, 20));
	pButton->EnableButton(FALSE);
	pButton->SetDivison(TRUE);

	rcNCButton.OffsetRect(100,0);
	rcNCButton.right	= rcNCButton.left + 93;

	pButton = AddTitleButton(&rcNCButton, pImage, 3, IDC_TITLEBUTTON_INSERT_FLASH, _T("插入动画"), TRUE, FALSE, TRUE);
	pIcon	= CSkinManager::GetInstance()->GetSkinItem(_T("icon_flash.png"));
	pButton->SetIconInfo(pIcon, 3, CSize(20, 20));
	pButton->EnableButton(FALSE);
	pButton->SetDivison(TRUE);


	rcNCButton.OffsetRect(100,0);
	rcNCButton.right	= rcNCButton.left + 93;

	pButton = AddTitleButton(&rcNCButton, pImage, 3, IDC_TITLEBUTTON_INSERT_VIDEO, _T("插入视频"), TRUE, FALSE, TRUE);
	pIcon	= CSkinManager::GetInstance()->GetSkinItem(_T("icon_movie.png"));
	pButton->SetIconInfo(pIcon, 3, CSize(20, 20));
	pButton->EnableButton(FALSE);
	pButton->SetDivison(TRUE);


	rcNCButton.OffsetRect(100,0);
	rcNCButton.right	= rcNCButton.left + 93;

	//新建习题
	pButton = AddTitleButton(&rcNCButton, pImage, 3, IDC_TITLEBUTTON_INSART_EXERCISES, _T("新建习题"), TRUE, FALSE, TRUE);
	pIcon	= CSkinManager::GetInstance()->GetSkinItem(_T("icon_exercises.png"));
	pButton->SetIconInfo(pIcon, 3, CSize(20, 20));
	pButton->SetDivison(TRUE);

	// add right conrner tip
	pButton->SetTipIconInfo(pTipIcon, 1, CPoint(0, -6));

	rcNCButton.OffsetRect(100,0);
	rcNCButton.right	= rcNCButton.left + 63;

	pButton = AddTitleButton(&rcNCButton, pImage, 3, IDC_TITLEBUTTON_REPEALS, _T("撤销"), TRUE, FALSE, TRUE);
	pIcon	= CSkinManager::GetInstance()->GetSkinItem(_T("icon_repeals.png"));
	pButton->SetIconInfo(pIcon, 3, CSize(20, 20));
	pButton->EnableButton(FALSE);
	pButton->SetDivison(TRUE);


	rcNCButton.OffsetRect(70,0);
	rcNCButton.right	= rcNCButton.left + 93;
	pButton = AddTitleButton(&rcNCButton, pImage, 3, IDC_TITLEBUTTON_PLAY_FULLSCREEN, _T("全屏播放"), TRUE, FALSE, TRUE);
	pIcon	= CSkinManager::GetInstance()->GetSkinItem(_T("icon_fullplay.png"));
	pButton->SetIconInfo(pIcon, 3, CSize(20, 20));
	pButton->EnableButton(FALSE);
	pButton->SetDivison(TRUE);

	tstring str3DPPTPath = GetLocalPath();
	str3DPPTPath += _T("\\bin\\3DPPT\\3DPPT.exe");

	if (GetFileAttributes(str3DPPTPath.c_str())  != INVALID_FILE_ATTRIBUTES )  //== FILE_ATTRIBUTE_DIRECTORY
	{
		rcNCButton.OffsetRect(100, 0);
		rcNCButton.right	= rcNCButton.left + 83;
		pButton = AddTitleButton(&rcNCButton, pImage, 3, IDC_TITLEBUTTON_3DPPT, _T("3DPPT"), TRUE, FALSE, TRUE);
		pIcon	= CSkinManager::GetInstance()->GetSkinItem(_T("icon_movie.png"));
		pButton->SetIconInfo(pIcon, 3, CSize(20, 20));
		pButton->SetDivison(TRUE);

		// add right conrner tip
		pButton->SetTipIconInfo(pTipIcon, 1, CPoint(0, -6));
	}


	//rcNCButton.OffsetRect(110, 0);
	//rcNCButton.right	= rcNCButton.left + 73;
	//pButton = AddTitleButton(&rcNCButton, pImage, 3, IDC_TITLEBUTTON_PACKAGE_LESSION, _T("生成课件"), TRUE, FALSE, TRUE);
	//pIcon	= CSkinManager::GetInstance()->GetSkinItem(_T("icon_file.png"));
	//pButton->SetIconInfo(pIcon, 1, CSize(20, 20));

	m_update.m_OnNotify += MakeDelegate(this, &CMainFrame::OnCheckUpdate);
	m_update.CheckUpdate();
	m_update.UpdateServerKeyWord();

	tstring strPath = GetLocalPath();
	strPath += _T("\\setting\\Config.ini");
	TCHAR szBuff[MAX_PATH + 1];
	GetPrivateProfileString(_T("config"), _T("AutoLogin"), _T("false"), szBuff, MAX_PATH, strPath.c_str());
	if(_tcsicmp(szBuff, _T("true")) == 0)
	{
		CGroupExplorerUI::GetInstance()->ShowLoginUI(TRUE);
		TNotifyUI msg;
		CGroupExplorerUI::GetInstance()->OnLogin(msg);
	}
}

bool CMainFrame::OnCheckUpdate( void* pObj )
{
	TEventNotify*  pENotify	= (TEventNotify*) pObj;
	m_nUpdateStatus		= pENotify->nEventType;
	g_Config::GetInstance()->SetCheckUpdateStatus(m_nUpdateStatus);

	if(m_nUpdateStatus == eUpdateType_Has_Update)
	{
		//计算文件总大小
		tstring strTempPath = GetLocalPath();
		strTempPath += _T("\\temp\\");
		tstring strHashFile = strTempPath;
		strHashFile += _T("UpdateHashList.dat");
		CStdioFile File;
		TCHAR szLine[MAX_PATH*2] ;
		TCHAR szFilePath[MAX_PATH];
		m_llAllFilesSize = 0;
		if(File.Open(strHashFile.c_str(), CFile::modeRead))
		{
			while(File.ReadString(szLine, MAX_PATH * 2 - 1))
			{
				char* pPost = strrchr(szLine, '|');
				if (pPost)
				{
					int nFileNameLen = pPost - szLine;
					memcpy(szFilePath, szLine, nFileNameLen);
					szFilePath[nFileNameLen] = '\0';
					tstring strFilePath = strTempPath;
					strFilePath += szFilePath;
					HANDLE hFile = ::CreateFile(strFilePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
						FILE_ATTRIBUTE_NORMAL, NULL);
					if(hFile != INVALID_HANDLE_VALUE)
					{
						m_llAllFilesSize += GetFileSize(hFile, NULL);
						CloseHandle(hFile);
					}
				}
			}
			File.Close();
		}
		//

		m_strVerion			= m_update.GetServerVersion();
		m_strUpdateContent	= m_update.GetUpdateLog();

		SetUpdateIconStatus(m_nUpdateStatus);
		::PostMessage(GetSafeHwnd(), WM_SHOW_UPDATETIP, NULL, NULL);

		CUpdateDialogUI* pUpdateDlg = UpdateDialogUI::GetInstance();
		if(pUpdateDlg->GetHWND())
		{
			::PostMessage(GetSafeHwnd(), WM_NOTIFY_UPDATE, NOTIFY_FORM_UPDATE, 0);
		}
	}
	else if(m_nUpdateStatus == eUpdateType_Update_Processing)
	{
		::PostMessage(GetSafeHwnd(), WM_SHOW_UPDATEINFO, NULL, NULL);
	}
	else if(m_nUpdateStatus == eUpdateType_Failed)
	{
		::PostMessage(GetSafeHwnd(), WM_SHOW_UPDATEINFO, NULL, NULL);
	}

	return true;
}

BOOL CMainFrame::SetMainPaneMoveStatus( BOOL bEnable )
{
	static HWND hWnd = NULL;

	EnumChildWindows(GetSafeHwnd(),(WNDENUMPROC)EnumChildProc,(LPARAM)&hWnd); 

	if ( hWnd )
	{
		::EnableWindow(hWnd, bEnable);
		return TRUE;
	}

	return FALSE;
}

CMainPane *	CMainFrame::getMainPane()
{	
	return &m_wndMainPane;
}
//重绘divider
BOOL CMainFrame::OnEraseBkgnd(CDC* pDC)
{
	DrawSliderBkgnd();
	return TRUE;
}

void CMainFrame::DrawSliderBkgnd()
{	
	CRect rect;

	if (m_wndMainPane.GetDefaultPaneDivider() != NULL)
	{

		m_wndMainPane.GetDefaultPaneDivider()->GetClientRect(&rect);
		HWND hw			= m_wndMainPane.GetDefaultPaneDivider()->GetSafeHwnd();
		HDC hdc			= ::GetDC(hw);            
		rect.left		= -2;
		rect.right		= rect.right+2;
		HBRUSH hBrush	= CreateSolidBrush(RGB(60,60,60));
		FillRect(hdc,&rect, hBrush);              
		DeleteObject(hBrush);              
		::ReleaseDC(hw,hdc);
		m_wndMainPane.GetDefaultPaneDivider()->UpdateWindow();
	}



}

//////////////////////////////////////////////////////////////////////////
void CMainFrame::ShowFloatCtrl()
{
	//if (!m_dlgFloatSlider.IsWindowVisible())
	{ 
		//m_dlgFloatSlider.ShowLocalWnd();
	}
}

void CMainFrame::HideFloatCtrl()
{	 
	//if (m_dlgFloatSlider.IsWindowVisible())
	{ 
		//m_dlgFloatSlider.HideLocalWnd();	 
	}
}

bool CMainFrame::OnChapterChange(void* pNotify)
{
	TEventNotify*  pENotify	= (TEventNotify*) pNotify;
	CStream* pStream		= (CStream*) pENotify->wParam;
	pStream->ResetCursor();
	tstring strGUID			= pStream->ReadString();
	tstring	strChapter		= pStream->ReadString();

	SetChapterButtonInfo(strChapter);
	return true;
}

void CMainFrame::SetChapterButtonInfo(tstring strChapter)
{
	BOOL bFirst = TRUE;
	BOOL bLast  = TRUE;

	NDCloudCurrentPos(bFirst, bLast);

	m_pChapterBtn->SetCaption(strChapter.c_str());
	m_pPrevBtn->EnableButton(!bFirst);
	m_pNextBtn->EnableButton(!bLast);
}
//////////////////////////////////////////////////////////////////////////2015.09.03 CWS

bool CMainFrame::OnPPTOperation(void* pNotify)
{
	TEventNotify*  pENotify	= (TEventNotify*) pNotify;
	int nOperation			= pENotify->wParam;

	CPPTShellDoc* pDoc = (CPPTShellDoc*)this->GetActiveDocument();
	CPPTShellApp* pApp	= (CPPTShellApp*)AfxGetApp();

	if ( nOperation == NEW_FILE )
	{
		OnCommand(IDC_TITLEBUTTON_NEW, 0);
		theApp.SetCurrentPPTPath(_T(""));
	}
	else if ( nOperation == OPEN_FILE )
	{
		OnCommand(IDC_TITLEBUTTON_OPEN, 0);
	}
	else if ( nOperation == CHANGE_FILE )
	{
		CStream* pStream = (CStream*)pENotify->lParam;
		pStream->ResetCursor();

		m_nChangeSubOperation	= pStream->ReadDWORD();
		tstring strGUID			= pStream->ReadString();
		tstring strFilePath		= pStream->ReadString();

		delete pStream;

		if ( _tcsicmp(strFilePath.c_str(), m_strPPPath.c_str()) == 0 )
			return true;

		ShadeWindowShow(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), _T(""), false);
		this->GenarateOperationer();
		
		IsPPTChangedReadOnlyByThread(TRUE, PPTC_PPNEW);  //绕一圈创建DOCument，PPT才不会卡住

		m_strPPPath = strFilePath;

		theApp.SetCurrentPPTPath(m_strPPPath.c_str());	//设置当前打开的文档路径

		//CreateDocByPowerPointer();

	}
	else if ( nOperation == SAVE_FILE)
	{
		CStream* pStream = (CStream*)pENotify->lParam;
		pStream->ResetCursor();

		tstring strFilePath = pStream->ReadString();
		tstring strNowFile = m_pMainPPTController->GetFilePath();

		delete pStream;

		if (strFilePath == strNowFile)
		{
			if ( !strFilePath.empty() )
				pDoc->SetPathName(strFilePath.c_str(), FALSE);

			pApp->SetCurrentPPTFileName(pDoc->GetTitle());

			//触发导出缩略图
			PPTInfoSender::GetInstance()->IncrementSyncSlideImages();
		}

		//保存到本地资源课件
		CStream stream(256);
		stream.WriteDWORD(FILE_FILTER_PPT);
		stream.WriteString(_T(""));
		stream.WriteString(strFilePath.c_str());
		stream.ResetCursor();
		BroadcastEvent(EVT_LOCALRES_ADD_FILE, &stream);

	}
	return true;
}


bool CMainFrame::OnUpdateEvent( void* pNotify )
{
// 	TEventNotify*  pENotify	= (TEventNotify*) pNotify;
// 	CStream* pStream		= (CStream*) pENotify->wParam;
// 	pStream->ResetCursor();
// 	m_nUpdateStatus			= pStream->ReadWORD();
// 	m_strVerion				= pStream->ReadString();
// 	m_strUpdateContent		= pStream->ReadString();

	//启动Update.exe来更新
	
	//
	//首次检测更新
//	if ( m_bFirstCheckUpdate )
// 	{
// 		if ( m_nUpdateStatus == eUpdateType_Has_Update )
// 		{
// 			BroadcastEvent(EVT_ENV_RELEASE, 1, 0, NULL);
// 			return true;
// 		}
// 	}
//	m_bFirstCheckUpdate		= FALSE;

	PostMessage(WM_NOTIFY_UPDATE, NOTIFY_FORM_UPDATE, 0);
	return true;
}

bool CMainFrame::OnEnvRelease(void* pNotify)
{
	m_bClosing	= TRUE;

	TEventNotify*  pENotify	= (TEventNotify*) pNotify;

	m_bForceUpdate = pENotify->wParam;
	if ( !m_bForceUpdate && IsIconic())
	{
			this->ShowWindow(SW_RESTORE);
			Sleep(200);
	}

	//
	//CUpdate update;

	if (GetPPTController()->IsInit())
		IsPPTChangedReadOnlyByThread(FALSE, PPTC_UPDATEAPP);
	else
	{
		m_update.StartCover();
		OnDestroyBefore();
	}


	return true;
}

bool CMainFrame::OnShowNextSlide(void* pNotify)
{
	//int nIndex = ::GetPPTController()->GetSlideShowViewIndex();
	//int nCount = ::GetPPTController()->GetSlideCount();

	return true;
}

bool CMainFrame::OnEventPPTInsert(void* pNotify)
{
	TEventNotify* pENotify = (TEventNotify*)pNotify;

	CStream* pStream = (CStream*)pENotify->wParam;
	pStream->ResetCursor();

	int nCommand = pStream->ReadDWORD();

	if ( nCommand == STAT_INSERT_SLIDE )
	{
		ShowAddSlideWindow();
	}
	else if ( nCommand == STAT_INSERT_QUESTION )
	{
		BOOL bRet		= pStream->ReadBOOL();
		tstring strPath = pStream->ReadString();

		HideVRDistributeWindow();

		if ( bRet > 0 )
			ShowExercisesEditWindow(strPath);
		else
			HideExercisesEditWindow();
	}
	else if ( nCommand == STAT_INSERT_VR )
	{
		BOOL bRet		= pStream->ReadBOOL();
		tstring strPath = pStream->ReadString();

		HideExercisesEditWindow();

		if ( bRet > 0 )
			ShowVRDistributeWindow(strPath);
		else
			HideVRDistributeWindow();
	}

	delete pStream;

	return true;
}

bool CMainFrame::OnMobileAutoLogin(void* pNotify)
{
	TEventNotify* tNotify = (TEventNotify*)pNotify;
	CStream* pStream = (CStream*)tNotify->wParam;
	pStream->ResetCursor();

	CGroupExplorerUI::GetInstance()->MobileLogin(pStream);
	return true;
}

bool CMainFrame::OnEventPPTControl(void* pNotify)
{
	TEventNotify* tNotify = (TEventNotify*)pNotify;
	CStream* pStream = (CStream*)tNotify->wParam;
	pStream->ResetCursor();

	int nCommand = pStream->ReadDWORD();

	switch( nCommand )
	{
	case PPTC_OPEN:
		{
			// sync info to users

			m_bOpening		= FALSE;
			m_dwOpenedTime	= GetTickCount();
			BOOL bRet		= pStream->ReadDWORD();
			BOOL bReadOnly	= pStream->ReadDWORD();
			tstring strPath = pStream->ReadString();
			if ( bRet )
			{
				theApp.SetCurrentPPTPath(strPath.c_str());
				BroadcastPostedEvent(EVT_PPT_INFO_SYNC, PPT_EDIT_OPENFILE, 0);
			}

		}
		break;
	case PPTC_OPEN_PPTFILE :
		{
			tstring strFileName = pStream->ReadString();
			// 用OpenDoc频繁且快速打开PPT文件可能导致程序无法响应
			COPYDATASTRUCT ps;
			ps.dwData	= MSG_OPEN_FILE;
			ps.cbData	= _tcslen(strFileName.c_str());
			ps.lpData	= (PVOID)strFileName.c_str();

			SendMessage(WM_COPYDATA, 0, (LPARAM)&ps);
			//OpenDoc(strFileName);
		}
		break;
	case PPTC_CLOSE:
		{
			// sync info to users
			BroadcastPostedEvent(EVT_PPT_INFO_SYNC, PPT_EDIT_OPENFILE, 0);

		}
		break;

	case PPTC_SAVE:
		{
			ShadeWindowHide();
			BOOL bRet		= pStream->ReadDWORD();
			int nCommand	= pStream->ReadDWORD();
			BOOL bBefore	= pStream->ReadDWORD();
			BOOL bClosePre	= pStream->ReadDWORD();
			BOOL bICRPlay	= pStream->ReadDWORD();
			int nCount		= pStream->ReadDWORD();
			tstring	strPath = pStream->ReadString();

			CourseAddToLocalRes(strPath);
			
			ExecCommandAfterSave(nCommand, bBefore, strPath, nCount);
		}
		break;
	case PPTC_UNINSERT:
		{
			BOOL bRet		= pStream->ReadDWORD();
			if ( bRet )
				CToast::Toast(_STR_PREVIEW_DIALOG_UNDO_OK);
		}
		break;
	case PPTC_GET_SLIDER_INDEX:
		{
			int nIndex	= pStream->ReadDWORD();
			BOOL bFindActiveX = pStream->ReadDWORD();
		}
		break;
	case PPTC_GET_SLIDER_COUNT:
		{
		}
		break;
	case PPTC_NEW:
		{
			BOOL bRet		= pStream->ReadDWORD();
			tstring strTitle= pStream->ReadString();
			CPPTShellDoc* pDoc = (CPPTShellDoc*)GetActiveDocument();
			pDoc->SetTitle(strTitle.c_str());

			// sync info to users
			BroadcastPostedEvent(EVT_PPT_INFO_SYNC, PPT_EDIT_NEWFILE, 0);

		}
		break;
	case PPTC_CLOSE_APP:
		{
			CloseRelateApp();
		}
		break;
	case PPTC_IS_CHANGED_READONLY:
		{
			int  nCommand	= pStream->ReadDWORD();
			BOOL bBefore	= pStream->ReadDWORD();
			BOOL bChanged	= pStream->ReadDWORD();
			BOOL bReadOnly	= pStream->ReadDWORD();
			int nCount		= pStream->ReadDWORD();
			tstring strFilePath = pStream->ReadString();
			tstring strFileName = pStream->ReadString();

			ShadeWindowHide();

			int nRet = IDNO;
			if ( bChanged )
			{
				m_nCurPPTCommand = nCommand;
				if ( nCommand == PPTC_PPNEW )
					nRet = IDNO;//SavePPT(nCommand, strFilePath, strFileName, bBefore, bReadOnly, TRUE, FALSE, FALSE); //1106
				else if ( nCommand == PPTC_CLOSE )
					nRet = SavePPT4Close(nCommand, strFilePath, strFileName, bBefore, bReadOnly, FALSE, TRUE, FALSE); //1106
				else if ( nCommand == PPTC_OPEN_DESKTOPPPT )
					nRet = SavePPT(nCommand, strFilePath, strFileName, bBefore, bReadOnly, FALSE, TRUE, FALSE); //1106
				else if ( nCommand == PPTC_OPEN )
					nRet = SavePPT(nCommand, strFilePath, strFileName, bBefore, bReadOnly, TRUE, TRUE, FALSE); //1106
				else if ( nCommand == PPTC_NDICRPLAY )
					nRet = SavePPT(nCommand, strFilePath, strFileName, bBefore, bReadOnly, FALSE, FALSE, TRUE); //1106
				else if ( nCommand == PPTC_PACKAGE )
					nRet = SavePPT(nCommand, strFilePath, strFileName, bBefore, bReadOnly, FALSE, FALSE, FALSE); //1106
				else if ( nCommand == PPTC_3DPPT )
					nRet = SavePPT(nCommand, strFilePath, strFileName, bBefore, bReadOnly, FALSE, FALSE, FALSE); //1106
				else if ( nCommand == PPTC_UPDATEAPP )
				{
					if ( m_bForceUpdate )
					{
						if(strFilePath.empty())
						{
							strFilePath = NDCloudFileManager::GetInstance()->GetNDCloudDirectory();
							strFilePath += _T("\\PPTTemp\\");
							strFilePath += strFileName;
							strFilePath += _T(".pptx");
						}
						nRet = PPTSaving(strFilePath.c_str(), bBefore, TRUE, FALSE, nCommand); //1106
					}
					else
					{
						nRet = SavePPT(nCommand, strFilePath, strFileName, bBefore, bReadOnly, TRUE, TRUE, FALSE);
					}
				}
				else if ( nCommand == PPTC_OPEN_BYTITLEBTN)
				{
					nCommand = PPTC_OPEN;
					nRet = SavePPT(nCommand, strFilePath, strFileName, bBefore, bReadOnly, FALSE, TRUE, FALSE); //1106
				}
			
			}
			
			if ( nRet == IDNO )
			{
				ExecCommandAfterSave(nCommand, bBefore, strFilePath, nCount);
			}
			else if ( nRet == IDCANCEL )
			{
				m_bCloseAll = FALSE;
			}
			
			m_bOpening  = FALSE;

			m_bClosing	= FALSE;

		}
		break;

	case PPTC_GET_VIDEO_LIST:
		{
			int nIndex = pStream->ReadInt();
			int nCount = pStream->ReadInt();

			vector<float> vecVideoList;
			vecVideoList.clear();

			for(int i = 0; i < nCount; i++)	
			{
				float val = pStream->ReadFloat();
				vecVideoList.push_back(val);
			}

			PPTInfoSender::GetInstance()->SendVideoListToAllUsers(nIndex, vecVideoList);
		}
		break;

	case PPTC_GET_QUESTION_LIST:
		{
			NDCloudUser::GetInstance()->IcrOnStartInit(pStream);
			//IcrPlayer::GetInstance()->IcrOnStart(pStream);
		}
		break;
	case PPTC_HAS_ACTIVEX_AT_PADGE:
		{
			if(m_pScreenInstrument)
			{
				m_pScreenInstrument->GetPagger()->SetPageActiveX(pStream);
			}
		}
		break;
	case  PPTC_SYNC_CHANGE:
		{
			TCHAR szInfo[200] = {0};
			wsprintf(szInfo, _T("PPT发生变化，是否先保存再向手机同步缩略图"));
			int nRet = UIMessageBox(NULL, szInfo, _T("101教育PPT"), _T("是,否"), CMessageBoxUI::enMessageBoxTypeWarn, IDCANCEL);
			if (nRet == ID_MSGBOX_BTN)
			{
				//保存
				m_pMainPPTController->Save();
			}
			else if (nRet == ID_MSGBOX_BTN+1)
			{
				//不保存 直接 触发重新生成缩略图并且增量同步
				PPTInfoSender::GetInstance()->IncrementSyncSlideImages();
			}
		}
		break;
	case PPTC_NEXT_SLIDE:
		{
			DWORD dwRet = pStream->ReadDWORD();
			int nState	= pStream->ReadInt();

			if ( nState == ppSlideShowDone ) //ppSlideShowDone  = 5
			{
				if ( m_h3DWnd != NULL )
					::SendMessage(m_h3DWnd, WM_CLOSE, 0 ,0);
				m_h3DWnd = NULL;
			}
		}
		break;
	}

	delete pStream;
	return true;
}

void CMainFrame::ExecCommandAfterSave(int nCommand, BOOL bBefore, tstring strPath, int nCount /*= 0*/)
{
	if ( nCommand == PPTC_PPNEW )
	{
		//ClosePPTByThread(bBefore);
		CreateDocByPowerPointer();
	}
	else if ( nCommand == PPTC_OPEN || nCommand == PPTC_OPEN_BYTITLEBTN)
	{
		OpenDocByFileDialog();
	}
	else if ( nCommand == PPTC_OPEN_DESKTOPPPT )
	{
		OpenDocByDesktop();
	}
	else if ( nCommand == PPTC_CLOSE )
	{
		if (m_nSaveToSkyType == 0)   
		{
			BOOL bRet	= IDNO;
			if ( !strPath.empty() )
				bRet = SavePPTToSkyDrive();

			if ( IDNO == bRet )
			{
				ClosePPTByThread(FALSE);
				Sleep(100);
				if ( nCount <= 1 )
					OnDestroyBefore();
				else if ( m_bCloseAll )
					IsPPTChangedReadOnlyByThread(FALSE, PPTC_CLOSE);
			}
			else if ( IDYES == bRet)
			{
				m_bUploadingOnClose	= TRUE;
				CProgressSavingUI* pSaveDialog = new CProgressSavingUI;
				pSaveDialog->ShowWindow();
				pSaveDialog->Start(strPath.c_str(), &MakeDelegate(this, &CMainFrame::OnSaveToDBankCompleted), nCount);
			}
		}
		else if (m_nSaveToSkyType == 1)	 
		{
			m_nSaveToSkyType = 0;
			ClosePPTByThread(FALSE);
			Sleep(100);
			if ( nCount <= 1 )
				OnDestroyBefore();
			else if ( m_bCloseAll )
				IsPPTChangedReadOnlyByThread(FALSE, PPTC_CLOSE);
		}
		else if (m_nSaveToSkyType == 2)	 
		{
			if ( NDCloudUser::GetInstance()->GetUserId() == 0 || strPath.empty())
			{
				m_nSaveToSkyType = 0;
				ClosePPTByThread(FALSE);
				Sleep(100);
				if ( nCount <= 1 )
					OnDestroyBefore();
				else if ( m_bCloseAll )
					IsPPTChangedReadOnlyByThread(FALSE, PPTC_CLOSE);
			}
			else
			{
				m_nSaveToSkyType = 0;	
				m_bUploadingOnClose	= TRUE;
				CProgressSavingUI* pSaveDialog = new CProgressSavingUI;
				pSaveDialog->ShowWindow();
				pSaveDialog->Start(strPath.c_str(), &MakeDelegate(this, &CMainFrame::OnSaveToDBankCompleted), nCount);
			}
		}

	}
	else if ( nCommand == PPTC_UPDATEAPP )
	{
		if(m_bForceUpdate)
			m_update.StartCover(strPath);
		else
			m_update.StartCover();
		OnDestroyBefore();
	}
	else if ( nCommand == PPTC_NDICRPLAY )
	{
		m_bIsChanged = TRUE;
		NDStartICRPlay((LPTSTR)strPath.c_str()); //(TCHAR*)((CPPTShellDoc*)GetActiveDocument())->GetPathName().GetString()
	}
	else if ( nCommand == PPTC_PACKAGE )
	{
		if ( !strPath.empty() )
		{
			//package
			CPackPPTDialogUI * pPackPPTDialogUI = new CPackPPTDialogUI;
			pPackPPTDialogUI->Create(GetSafeHwnd(), _T("PackPPTDialog"), WS_POPUP, 0, 0, 0, 0, 0);
			CRect rect;
			::GetWindowRect(this->GetSafeHwnd(), &rect);
			pPackPPTDialogUI->Init(rect);

			pPackPPTDialogUI->ShowWindow(true);
			pPackPPTDialogUI->Pack(strPath);
		}
		else
			CToast::Toast(_STR_PACK_PPT_DOC_IS_EMPTY);
	}
	else if ( nCommand == PPTC_3DPPT )
	{
		if ( strPath.empty() )
		{
			CToast::Toast(_STR_PACK_PPT_DOC_IS_EMPTY);
			return;
		}

		Create3DImages(strPath);
	}
}

void CMainFrame::Create3DImages(tstring strPath)
{
	DWORD dwCrc = CalcFileCRC(strPath.c_str());
	TCHAR szCrc[64] = {0};

	wsprintf(szCrc, "%08X", dwCrc);

	TCHAR szFolderPath[MAX_PATH]= {0};
	TCHAR szDrive[8]			= {0};
	TCHAR szDir[MAX_PATH]		= {0};
	TCHAR szFileName[MAX_PATH]	= {0};
	TCHAR szExt[8]				= {0};
	if (_tsplitpath_s(strPath.c_str(), szDrive, szDir, szFileName, szExt))
	{
		return;
	}

	int nLen = _tcslen(szFileName);
	if ( nLen > MAX_PATH/2 )
		szFileName[MAX_PATH/2] = _T('\0');

	_stprintf_s(szFolderPath, _T("%s\\Cache\\PPTSlides\\%s_%08x"), GetLocalPath().c_str(), szFileName, dwCrc);

	int nRet = GetFileAttributes(szFolderPath);

	if ( nRet == INVALID_FILE_ATTRIBUTES || 0 == g_Config::GetInstance()->GetInt(_T("PPTCRC"), szCrc) )
	{
		CToast::Toast(_STR_FILE_EXPORT_IMAGES, true, 3000);
		DWORD m_dwExportId = PPTImagesExporter::GetInstance()->ExportCurrentPPTTo3DImages(strPath.c_str(), MakeDelegate(this, &CMainFrame::OnPPTExportCompleted), -1, -1, _T("png"));
	}
	else
	{
		Play3DPPT(szFolderPath);
	}
}

bool CMainFrame::OnPPTExportCompleted( void* pObj )
{
	ExportNotify* pNotify = (ExportNotify*)pObj;
	if (pNotify->dwErrorCode != 0)
	{
		return true;
	}

	DWORD dwCrc = CalcFileCRC(pNotify->strPath.c_str());
	TCHAR szCrc[64] = {0};

	wsprintf(szCrc, "%08X", dwCrc);
	g_Config::GetInstance()->SaveInt(_T("PPTCRC"), szCrc, dwCrc);

	Play3DPPT(pNotify->strDir);

	return true;
}

bool CMainFrame::Play3DPPT(tstring strFolderPath)
{
	tstring str3DPPTExePath = GetLocalPath();
	str3DPPTExePath += _T("\\bin\\3DPPT\\3DPPT.exe");

	tstring str3DPPTConfigPath = GetLocalPath();
	str3DPPTConfigPath += _T("\\bin\\3DPPT\\3DPPT_Data\\StreamingAssets\\config.ini");

	WritePrivateProfileString(_T("PPT"), _T("FILE_PATH"), Str2Utf8(strFolderPath).c_str(), str3DPPTConfigPath.c_str());

	tstring strImage = strFolderPath + _T("\\Slide_1.png");

	UINT nWidth		= 0;
	UINT nHeight	= 0;

	if ( GetImageSize(strImage, nWidth, nHeight) ) 
	{  
		float fScale = nWidth * 1.0f / nHeight;

		TCHAR szScale[MAX_PATH] = {0};
		_stprintf_s(szScale, _T("%6.4f"), fScale);

		WritePrivateProfileString(_T("PPT"), _T("RATIO"), szScale, str3DPPTConfigPath.c_str());
	}
		
	PROCESS_INFORMATION pi;
	STARTUPINFO si;      //隐藏进程窗口
	ZeroMemory( &pi, sizeof(pi) );
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);

	if(CreateProcess(str3DPPTExePath.c_str(), NULL,  NULL, // process info
		NULL, // thread info      
		FALSE, // inheritable      
		0, // creation flags     
		NULL, // environment       
		NULL, // current directory     
		&si, & pi)) 
	{ 
		CloseHandle(pi.hProcess); 
		CloseHandle(pi.hThread);
	}

	return true;

}

void CMainFrame::CourseAddToLocalRes(tstring strPath)
{
	CStream stream(256);
	stream.WriteDWORD(FILE_FILTER_PPT);
	stream.WriteString(_T(""));
	stream.WriteString(strPath);
	stream.ResetCursor();
	BroadcastEvent(EVT_LOCALRES_ADD_FILE, &stream);
}

void CMainFrame::CreateDocByPowerPointer()
{
	CPPTShellDoc* pDoc = (CPPTShellDoc*) GetActiveDocument();
	if (m_nChangeSubOperation == OPEN_FILE)
	{
		pDoc->SetPathName(m_strPPPath.c_str(), FALSE);
	}
	else
	{
		pDoc->NewDocument();
		pDoc->SetTitle(m_strPPPath.c_str());
	}
}

void CMainFrame::OpenDocByFileDialog()
{
	tstring strFile		= _T("");
	BOOL bReadOnly		= FALSE;
	BOOL bRet = FileOpenDialog(_T("打开演示文稿"), FILTER_PPT_AND_NDPX, 
		OFN_FILEMUSTEXIST|OFN_NONETWORKBUTTON|OFN_PATHMUSTEXIST|OFN_EXPLORER|OFN_HIDEREADONLY,
		strFile, bReadOnly, AfxGetMainWnd()->m_hWnd);
	if ( bRet )
	{
			OpenDoc(strFile);
	}
}

void CMainFrame::OpenDocByDesktop()
{
	//CloseByThread(FALSE);
	if ( IsIconic() )
		ShowWindow(SW_SHOWNORMAL|SW_RESTORE);

	OpenDoc(m_strDeskTopPPTPath);
}

void CMainFrame::OpenDoc(tstring strPath)
{

	::GetPPTController()->ShowPPTWnd(AfxGetApp()->m_pMainWnd);

	if ( strPath.length() > 256 )
	{
		CToast::Toast(_STR_FILE_OPEN_FAILED, true, 3000);
		m_bOpening = FALSE;
		return;
	}
 
	if ( IsNdpx(strPath) )
		OpenNdpx(strPath);
	else
		OpenPPTByThread(strPath.c_str(), FALSE);

	((CPPTShellView*)GetActiveView())->ShowUntitledWindow(FALSE);
	((CPPTShellDoc*)GetActiveDocument())->SetPathName(strPath.c_str());

	this->GenarateOperationer();
	Statistics::GetInstance()->Report(STAT_OPEN_PPT);
}


void CMainFrame::SetUpdateIconStatus( int nUpdate )
{
	if(0)//关于进行屏蔽
	{
		Image* pIcon = (nUpdate == eUpdateType_Has_Update) ? CSkinManager::GetInstance()->GetSkinItem(_T("icon_dot.png")) : NULL;
		m_pAboutBtn->SetIconInfo(pIcon, 1, CSize(6, 6), CPoint(20, 5), TRUE, TRUE);
	}
}

//更新相关
LRESULT CMainFrame::OnNotifyUpdate(WPARAM wParam, LPARAM lParam)
{
	CRect rect;
	::GetWindowRect(this->GetSafeHwnd(),&rect);
	if ( m_pUpdateDlg == NULL || (m_pUpdateDlg && m_pUpdateDlg->GetHWND() == NULL) )
	{
		m_pUpdateDlg = UpdateDialogUI::GetInstance();
		m_pUpdateDlg->Create(GetSafeHwnd(), _T("UpdateWindow"), WS_POPUP , 0, 0, 0, 0, 0);
	}
	
//	
	m_pUpdateDlg->setUpdateVersion(m_strVerion.c_str());
	m_pUpdateDlg->setUpdateLog(m_strUpdateContent.c_str());
	

	TCHAR szBuff[MAX_PATH];
	if(m_llAllFilesSize < 1024)
	{
		_stprintf(szBuff, _T("%d B"), m_llAllFilesSize);
	}
	else if(m_llAllFilesSize >= 1024 && m_llAllFilesSize < 1024*1024)
	{
		_stprintf(szBuff, _T("%.2f KB"), (float)m_llAllFilesSize/1024);
	}
	else if(m_llAllFilesSize >= 1024*1024 && m_llAllFilesSize < 1024*1024*1024)
	{
		_stprintf(szBuff, _T("%.2f MB"), (float)m_llAllFilesSize/1024/1024);
	}
	else if(m_llAllFilesSize >= 1024*1024*1024)
	{
		_stprintf(szBuff, _T("%.2f GB"), (float)m_llAllFilesSize/1024/1024/1024);
	}

	m_pUpdateDlg->setAllFilesSize(szBuff);
	if (wParam == NOTIFY_FORM_TIMER) 
		m_pUpdateDlg->TimingUpdate();//定时更新

	m_pUpdateDlg->setUpdate(m_nUpdateStatus);
	m_pUpdateDlg->Init(rect);
	m_pUpdateDlg->ShowWindow(true);
	

	return TRUE;
}

LRESULT CMainFrame::OnTitleMouse(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == IDC_TITLEBUTTON_LESSON )
	{
		if ( NDCloudGetChapterGUID().empty() )
			return 1;

		CChapterInfoUI* pDlg = ChapterInfoUI::GetInstance();
		if ( lParam == BMS_HOVER || lParam == BMS_MOUSE )
		{
			if(pDlg->GetHWND() == NULL)
			{
				pDlg->Create(GetSafeHwnd(), _T("ChapterInfoUI"), WS_POPUP , 0, 0, 0, 0, 0);
				pDlg->Init();
			}
			POINT pt = {0, 0};
			GetCursorPos(&pt);
			pt.x += 10;
			pt.y += 20;
			pDlg->SetPos(pt);

		}
		else if ( lParam == BMS_LEAVE  && (pDlg != NULL) )
		{
			pDlg->ShowWindow(false);
		}
	}

	return 1;
}

void CMainFrame::ModifyTitleButtonStatus()
{
	if( CPPTController::GetPPTMenuHwnd() != NULL )
		EnablePPTOperationButton(FALSE, FALSE);
	else if( IsUntitledShow() )
		EnablePPTOperationButton(FALSE, TRUE);
	else
		EnablePPTOperationButton(TRUE);
}

LRESULT CMainFrame::OnTitleButtonOperation(WPARAM wParam, LPARAM lParam)
{
	CPPTShellDoc* pDoc = (CPPTShellDoc*)this->GetActiveDocument();
	
	switch(wParam)
	{
	case IDC_TITLEBUTTON_NEW:
		{
			pDoc->OnNewDocument();
			Statistics::GetInstance()->Report(STAT_NEW_PPT);
		}
		break;
	case IDC_TITLEBUTTON_PHONE:
		{
			if(g_Config::GetInstance()->GetModuleVisible(MODULE_MOBILE_CONNECT))
			{
				CRect rect; 
				::GetWindowRect(this->GetSafeHwnd(),&rect);
				CQRCodeDialogUI* pQRCodeDlg = new CQRCodeDialogUI();
				pQRCodeDlg->Create(this->GetSafeHwnd(), _T("QRCodeWindow"), WS_POPUP | WS_VISIBLE, 0, 0, 0, 0, 0);
				::MoveWindow(pQRCodeDlg->GetHWND(), rect.left, rect.top, rect.Width(), rect.Height(), TRUE); 
			}
		}
		break;
	case IDC_TITLEBUTTON_OPEN:
		{
			/*if ( GetPPTController()->IsInit() )
				//IsPPTChangedReadOnlyByThread(FALSE, PPTC_OPEN);
				IsPPTChangedReadOnlyByThread(FALSE, PPTC_OPEN_BYTITLEBTN);
			else*/
				OpenDocByFileDialog();
		}
		break;
	case IDC_TITLEBUTTON_PACKAGE_LESSION:
		{
			if ( GetPPTController()->IsInit() )
				IsPPTChangedReadOnlyByThread(FALSE, PPTC_PACKAGE);
		}
		break;
	case IDC_TITLEBUTTON_3DPPT:
		{
			if ( GetPPTController()->IsInit() )
				IsPPTChangedReadOnlyByThread(FALSE, PPTC_3DPPT);
		}
		break;
	case IDC_TITLEBUTTON_INSART_EXERCISES:
		{
			if (!isNodeJSRunning())
			{
				CToast::Toast(_STR_NODEJS_NO_PREPARE);
				break;
			}
			NDCloudLocalQuestionManager::GetInstance()->CreateQuestion();
			break;

			//DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();

			//if( dwUserId == 0 )
			//{
			//	CToast::Toast(_STR_PREVIEW_DIALOG_NOUSER);

			//	CItemExplorerUI::GetInstance()->ShowWindow(false);//隐藏C框
			//	CGroupExplorerUI::GetInstance()->ShowWindow(true);
			//	CGroupExplorerUI::GetInstance()->ShowLoginUI();
			//	return 0;
			//}

			//CQuestionDialogUI* pDialog = new CQuestionDialogUI;
			//pDialog->CreateDlg();

			//if (IDOK == pDialog->ShowModal())
			//{
			//	TCHAR szUserId[MAX_PATH];
			//	_stprintf_s(szUserId, _T("%d"), dwUserId);

			//	CCategoryTree* pCategory = NULL;
			//	NDCloudGetCategoryTree(pCategory);

			//	m_bBasicQuestion = pDialog->IsBasicQuestion();
			//	m_strQuestionType = pDialog->GetQuestionType(); 

			//	

			//	NDCloudQuestionManager::GetInstance()->AddQuestion(NDCloudGetChapterGUID(), pDialog->GetQuestionType(), NDCloudGetBookGUID(), pCategory->GetSelectedFullCode(),
			//					pDialog->IsBasicQuestion(), MakeHttpDelegate(this, &CMainFrame::OnQuestionEditUrlObtained));
			//}

		}
		break;
	case IDC_TITLEBUTTON_PLAY_FULLSCREEN:
		{
			OnPptPlay();
		}
		break;
	case MSG_PPT_ENDSTOP://最后一页退出
		{
			// if icrplayer exist then notice user 
			if( g_Config::GetInstance()->GetModuleVisible(MODULE_ND_ICRPLAY) )
			{
				HWND hWnd = ::GetForegroundWindow();

				int nRet = UIMessageBox(hWnd, _T("确定退出互动课堂?"), _T("101教育PPT"), _T("确定,取消"), CMessageBoxUI::enMessageBoxTypeQuestion, IDCANCEL);
				if( nRet != ID_MSGBOX_BTN )
				{
					break;
				}
			}
			CloseVRMediaPlayer();
			KillExeCheckParentPidNotPlayer(_T("CoursePlayer.exe"), TRUE);//先把原先打开的关闭
			::SetForegroundWindow(CPPTController::GetSlideShowViewHwnd());

			HWND hWnd = ::FindWindow(_T("CEFOCX"), NULL);  //向CEFOCX的窗口发送页面变动消息
			if (::IsWindow(hWnd)){
				::SendMessage(hWnd, MSG_CEFOCX_PAGECHANGE, NULL, NULL);
			}
			

			StopPPTByThread();
			Statistics::GetInstance()->Report(STAT_PLAY_EXIT);
		}
		break;
	case MSG_PPT_STOP:
		{
			// if icrplayer exist then notice user 
			if( g_Config::GetInstance()->GetModuleVisible(MODULE_ND_ICRPLAY) )
			{
				//HWND hWnd = ::GetForegroundWindow();
				HWND hWnd= CPPTController::GetSlideShowViewHwnd();
				int nRet = UIMessageBox(hWnd, _T("确定退出互动课堂?"), _T("101教育PPT"), _T("确定,取消"), CMessageBoxUI::enMessageBoxTypeQuestion, IDCANCEL);
				if( nRet != ID_MSGBOX_BTN )
				{
					SetFocusToCefByThread();
					break;
				}
			}
			HWND hWnd = ::FindWindow(_T("CEFOCX"), NULL);  //向CEFOCX的窗口发送页面变动消息
			if (::IsWindow(hWnd)){
				::SendMessage(hWnd, MSG_CEFOCX_PAGECHANGE, NULL, NULL);
			}
			CloseVRMediaPlayer();
			KillExeCheckParentPidNotPlayer(_T("CoursePlayer.exe"), TRUE);//先把原先打开的关闭
			::SetForegroundWindow(CPPTController::GetSlideShowViewHwnd());

			StopPPTByThread();
			Statistics::GetInstance()->Report(STAT_PLAY_EXIT);
		}
		break;
	case IDC_TITLEBUTTON_USERLOGIN:
		{ 
			DWORD dwUserID = NDCloudUser::GetInstance()->GetUserId();
			if (dwUserID == 0)
			{
				CItemExplorerUI::GetInstance()->ShowWindow(false);//隐藏C框
				CGroupExplorerUI::GetInstance()->ShowWindow(true);
				CGroupExplorerUI::GetInstance()->ShowLoginUI();
			}
			else
			{
				bool bLoginComplete = NDCloudUser::GetInstance()->GetLoginComplete();//获取登陆完成 2016.01.27
				if (!bLoginComplete)
				{
					CItemExplorerUI::GetInstance()->ShowWindow(false);//隐藏C框
					CGroupExplorerUI::GetInstance()->ShowWindow(true);
					CGroupExplorerUI::GetInstance()->ShowLoginUI();
				}
				else
				{

					//登录界面 2015.11.20
					POINT pt;
					pt.x = 1700; 
					pt.y  = 30;
					GetCursorPos(&pt);
					//ScreenToClient(&pt);
					CRect lRect;
					GetWindowRect(&lRect);
					pt.x = lRect.right - 100 - 177;
					if (this->IsZoomed())
					{
						pt.x = lRect.right - 100 - 180;
					} 
					pt.y = lRect.top; 

	 
					//每次创建前销毁，避免内存泄露 2016.01.12
					if (::IsWindow(m_hUserReLogin))
					{
						::DestroyWindow(m_hUserReLogin);
					}

					CUserReLogin *pLogin = UserReLoginUI::GetInstance();
				
					pLogin->Create(this->GetSafeHwnd(), _T("UserReLogin"), WS_POPUP | WS_VISIBLE, 0, 0, 0, 0, 0);
					tstring sUNickName= NDCloudUser::GetInstance()->GetNickName();
					/*NDCloudUser::GetInstance()->GetTeacherPhoto();
					tstring sPhoto = NDCloudUser::GetInstance()->GetTeacherPhotoPath();*/
					if (sUNickName.empty())
					{ 
						sUNickName= NDCloudUser::GetInstance()->GetUserName();
					} 

					pLogin->InitWindows(this->GetSafeHwnd(), pt,_T(""), _T(sUNickName.c_str()), m_nLoginType);
					int nChangeClass = 0;//2;//先暂时屏蔽切换班级功能
					int nHeight = 295 + 43* nChangeClass;
					if (m_nLoginType > 0)//如果是第三方登陆的就不显示修改密码 cws2015.01.05
					{
						nHeight = 252+43*nChangeClass;
					}
					m_hUserReLogin = pLogin->GetHWND();
					::MoveWindow(pLogin->GetHWND(), pt.x,pt.y, 172,nHeight, TRUE);
					pLogin->ShowLocalWindows();   
					DWORD dwUserID = NDCloudUser::GetInstance()->GetUserId();
					if (dwUserID == 0)
					{
						m_pUserLoginBtn->SetCaption(_T("登录"));
					} 
				}
			}
		}
		break;
	case IDC_TITLEBUTTON_LOGO:
	case IDC_TITLEBUTTON_ABOUT:
		{
			//判断updatetip
			CUpdateTipDialogUI* pDlg = UpdateTipDialogUI::GetInstance();
			if(pDlg->GetHWND() && ::IsWindowVisible(pDlg->GetHWND()))
			{
				pDlg->ShowWindow(false);
			}
			POINT pt = {-3, -6};
			if ( IsZoomed() )
				pt.x = 0;

			ClientToScreen(&pt);
			if ( m_pMenu == NULL || (m_pMenu && m_pMenu->GetHWND() == NULL ))
			{
				m_pMenu = new CAboutMenu();
				m_pMenu->Create(AfxGetMainWnd()->GetSafeHwnd(), _T("MenuWnd"), WS_VISIBLE, 0, 0, 0, 0);

			}
			bool bNeedUpdate = (m_nUpdateStatus == eUpdateType_Has_Update);
			m_pMenu->Init(AfxGetMainWnd()->GetSafeHwnd(), pt, bNeedUpdate, m_strVerion, m_strUpdateContent);
			m_pMenu->ShowWindow(TRUE);
			return true;
		}
		break;
	case IDC_TITLEBUTTON_INSERT_PICTURE:
		{
			tstring strFile		= _T("");
			BOOL bReadOnly		= FALSE;
			BOOL bRet = FileOpenDialog(_T("插入图片"), FILTER_PICTURE, 
				OFN_FILEMUSTEXIST|OFN_NONETWORKBUTTON|OFN_PATHMUSTEXIST|OFN_EXPLORER|OFN_HIDEREADONLY,
				strFile, bReadOnly, AfxGetMainWnd()->m_hWnd);
			if ( bRet )
			{
				// get image size
				wstring wstr = Str2Unicode(strFile);
				WCHAR* wszPath = (WCHAR*)wstr.c_str();

				Image* pImage = Bitmap::FromFile(wszPath);
				int nWidth = pImage->GetWidth();
				int nHeight = pImage->GetHeight();

				delete pImage;
				InsertPictureByThread(strFile.c_str(), -1, -1, nWidth, nHeight, 0, 0, 0);
				Statistics::GetInstance()->Report(STAT_INSERT_PICTURE);
 
			}
		}
		break;
	case IDC_TITLEBUTTON_INSERT_FLASH:
		{
			tstring strFile		= _T("");
			BOOL bReadOnly		= FALSE;
			BOOL bRet = FileOpenDialog(_T("插入动画"), FILTER_FALSH, 
				OFN_FILEMUSTEXIST|OFN_NONETWORKBUTTON|OFN_PATHMUSTEXIST|OFN_EXPLORER|OFN_HIDEREADONLY,
				strFile, bReadOnly, AfxGetMainWnd()->m_hWnd);
			if ( bRet )
			{
				CString strTempPath = strFile.c_str();
				int nPos = strTempPath.ReverseFind('\\');
				CString strName = strTempPath.Mid(nPos+1, strTempPath.GetLength()-nPos-5);
				InsertSwfByThread(strFile.c_str(), _T(""), strName.GetString());
				Statistics::GetInstance()->Report(STAT_INSERT_FLASH);

			}
		}
		break;
	case IDC_TITLEBUTTON_INSERT_VIDEO:
		{
			tstring strFile		= _T("");
			BOOL bReadOnly		= FALSE;
			BOOL bRet = FileOpenDialog(_T("插入视频"), FILTER_VIDEO, 
				OFN_FILEMUSTEXIST|OFN_NONETWORKBUTTON|OFN_PATHMUSTEXIST|OFN_EXPLORER|OFN_HIDEREADONLY,
				strFile, bReadOnly, AfxGetMainWnd()->m_hWnd);
			if ( bRet )
			{
				InsertVideoByThread(strFile.c_str());
				Statistics::GetInstance()->Report(STAT_INSERT_VIDEO);
			}
		}
		break;
	case IDC_TITLEBUTTON_REPEALS:
		{
			UnDoByThread();
			Statistics::GetInstance()->Report(STAT_UNDO);
		}
		break;
	case IDC_TITLEBUTTON_PREVIOUS:
		{
			BOOL bFirst			= FALSE;
			tstring strChapter	= NDCloudPrevChapter(bFirst);
		}
		break;
	case IDC_TITLEBUTTON_LESSON:
		{
			::PostMessage(getMainPane()->GetMainControlDlg()->GetSafeHwnd(),WM_CREATE_GUIDE,GUIDE_LOCAL_OPEN,NULL);
		}
		break;
	case IDC_TITLEBUTTON_NEXT:
		{
			BOOL bLast			= FALSE;
			tstring strChapter	= NDCloudNextChapter(bLast);
		}
		break;
	case IDC_TITLEBUTTON_DOWNLOADMGR:	//下载管理器按钮
		{
			if(m_pDownloadMgr->IsChecked())
			{
				CDownloadManagerUI::GetInstance()->ShowManagerUI(true);
			}
			else
			{
				CDownloadManagerUI::GetInstance()->ShowManagerUI(false);
			}
			
		}
		break;
	}

	//消息重新派发
	MSG msg = {0};
	::GetMessage(&msg, NULL, 0,0);
	::TranslateMessage(&msg);
	::DispatchMessage(&msg);

	return TRUE;
}

void CMainFrame::EnablePPTOperationButton( BOOL bEnabled, BOOL bEnabledOpen /*= TRUE*/ )
{
	SetTitleButtonEnabled(IDC_TITLEBUTTON_OPEN, bEnabledOpen);
	SetTitleButtonEnabled(IDC_TITLEBUTTON_INSERT_PICTURE, bEnabled);
	SetTitleButtonEnabled(IDC_TITLEBUTTON_INSERT_FLASH, bEnabled);
	SetTitleButtonEnabled(IDC_TITLEBUTTON_INSERT_VIDEO, bEnabled);
	SetTitleButtonEnabled(IDC_TITLEBUTTON_PLAY_FULLSCREEN, bEnabled);
	SetTitleButtonEnabled(IDC_TITLEBUTTON_REPEALS, bEnabled);
	SetTitleButtonEnabled(IDC_TITLEBUTTON_INSART_EXERCISES, bEnabled);
	SetTitleButtonEnabled(IDC_TITLEBUTTON_PACKAGE_LESSION, bEnabled);
	SetTitleButtonEnabled(IDC_TITLEBUTTON_3DPPT, bEnabled);

	if(g_Config::GetInstance()->GetModuleVisible(MODULE_MOBILE_CONNECT))
	{
		SetTitleButtonEnabled(IDC_TITLEBUTTON_PHONE, bEnabledOpen);//2015.11.05 cws
	}
}


LRESULT CMainFrame::OnUserSendEvent(WPARAM wParam, LPARAM lParam)
{
	CStream* pStream = (CStream*)wParam;
	if( pStream == NULL )
		return 0;

	pStream->ResetCursor();

	int nEventType = pStream->ReadInt();
	WPARAM param1  = pStream->ReadDWORD();
	LPARAM param2  = pStream->ReadDWORD();
	void* pSender  = (void*)pStream->ReadDWORD();

	EventCenter::GetInstance()->BroadcastEvent(nEventType, param1, param2, pSender);
	return 0;
}


LRESULT CMainFrame::OnUserPostEvent(WPARAM wParam, LPARAM lParam)
{
	CStream* pStream = (CStream*)wParam;
	if( pStream == NULL )
		return 0;

	pStream->ResetCursor();

	int nEventType = pStream->ReadInt();
	WPARAM param1  = pStream->ReadDWORD();
	LPARAM param2  = pStream->ReadDWORD();
	void* pSender  = (void*)pStream->ReadDWORD();

	delete pStream;	

	EventCenter::GetInstance()->BroadcastEvent(nEventType, param1, param2, pSender);
	return 0;
}

LRESULT CMainFrame::OnUserHttpMsg(WPARAM wParam, LPARAM lParam)
{
	CHttpEventSource* pEventSource = (CHttpEventSource*)wParam;
	if( pEventSource == NULL )
		return 0;

	THttpNotify* pNotify = (THttpNotify*)lParam;
	if( pNotify == NULL )
		return 0;

	bool res = (*pEventSource)(pNotify);
	return res;
}

LRESULT CMainFrame::OnUserHttpUploadMsg(WPARAM wParam, LPARAM lParam)
{
	CHttpEventSource* pEventSource = (CHttpEventSource*)wParam;
	if( pEventSource == NULL )
		return 0;

	THttpNotify* pNotify = (THttpNotify*)lParam;
	if( pNotify == NULL )
		return 0;

	bool res = (*pEventSource)(pNotify);
	return res;
}

LRESULT CMainFrame::OnPPTExportImagesMsg( WPARAM wParam, LPARAM lParam )
{
	CHttpEventSource* pEventSource = (CHttpEventSource*)wParam;
	if( pEventSource == NULL )
		return 0;

	ExportNotify* pNotify = (ExportNotify*)lParam;
	bool res = (*pEventSource)(pNotify);

	return res;
}

LRESULT CMainFrame::OnPPTPackPPTMsg( WPARAM wParam, LPARAM lParam )
{
	PackNotify* pPackNotify = (PackNotify*)wParam;
	bool res = 0;
	if (pPackNotify != NULL)
	{
		if(pPackNotify->nPackStep == PACK_PPT_START)
			res = pPackNotify->OnSetValue(pPackNotify);
		else if(pPackNotify->nPackStep == PACK_PPT_END || pPackNotify->nPackStep == UNPACK_PPT_INIT_FINISH )
			res = pPackNotify->OnCompleted(pPackNotify);
		else
			res = pPackNotify->OnProgress(pPackNotify);

	}

	m_bOpening = FALSE;
	return res;
}

bool CMainFrame::RunPlugin( bool bLastPage )
{
	int nTempIndex = 0;
	
	::GetPPTController()->ShowMediaShape();
	tstring strPath = ::GetPPTController()->GetMediaPath(nTempIndex);
	
	if ( !strPath.empty() )
	{
		if ( (m_nSwfIndex != nTempIndex) || (bLastPage && (m_nSwfIndex == nTempIndex)) )  //bNxit 用来判断最后从黑屏回到最后一页 
		{
			StartSwf(strPath.c_str());
		}
		m_nSwfIndex = nTempIndex;
		return true;
	}

	m_nSwfIndex = nTempIndex;
	return false;
}

void CMainFrame::StartSwf(LPCTSTR strPath)
{
	HWND hPlayerWnd = ::FindWindow(_T("TfrmFloatPlayer"), NULL);
	if ( hPlayerWnd != NULL )
		return;

	TCHAR szExe[MAX_PATH*2]={0};
	wsprintf(szExe, _T("%s\\bin\\floatPlayer.exe"), GetLocalPath().c_str());

	PROCESS_INFORMATION pi;
	STARTUPINFO si;      //隐藏进程窗口
	ZeroMemory( &pi, sizeof(pi) );
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);

	TCHAR szParam[MAX_PATH*2]={0};

	wsprintf(szParam, _T(" %s \"%s\" -h %X"), _T("swf"), strPath, GetSafeHwnd());

	if(CreateProcess(szExe, szParam,  NULL, // process info
		NULL, // thread info      
		FALSE, // inheritable      
		0, // creation flags     
		NULL, // environment       
		GetLocalPath().c_str(), // current directory     
		&si, & pi)) 
	{ 
		CloseHandle(pi.hProcess); 
		CloseHandle(pi.hThread);
	}
}

BOOL CMainFrame::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	CPPTShellDoc* pDoc;
	switch (pCopyDataStruct->dwData)
	{ 	
	case MSG_OPEN_FILE:
		{
			m_strDeskTopPPTPath = (LPCTSTR)pCopyDataStruct->lpData;

			if ( GetTickCount() - m_dwOpenedTime < 3 * 1100 )  //防止频繁操作
			{
				CToast::Toast(_STR_FILE_OPEN_FAST, true);
				//m_dwOpenedTime	= GetTickCount();
				return FALSE;
			}

			pDoc = (CPPTShellDoc*)this->GetActiveDocument();
			if(pDoc)
			{
				if ( ::IsWindowVisible(MessageBoxUI::GetInstance()->GetHWND()) || IsFileDialogShow() )
				{
					CToast::Toast(_STR_POWER_POINT_NOT_ACTIVE, true, 3000);
					return TRUE;
				}

				if ( !IsMainFrmActive() )
				{
					CToast::Toast(_STR_POWER_POINT_NOT_ACTIVE, true, 3000);
					return TRUE;
				}

				if ( m_bUploadingOnClose )
				{
					CToast::Toast(_STR_POWER_POINT_NOT_ACTIVE, true, 3000);
					return TRUE;
				}

				if (theApp.GetCurrentPPTPath().compare(m_strDeskTopPPTPath.c_str()) == 0 )
				{
					CToast::Toast(_STR_FILE_OPENED, true);
					return FALSE;
				}

				if ( m_bOpening )
				{
					//CToast::Toast(_STR_FILE_OPENEING, true);
					return FALSE;
				}

				m_bOpening = TRUE;
				m_dwOpenedTime	= GetTickCount();

				/*if ( GetPPTController()->IsInit() )
					IsPPTChangedReadOnlyByThread(FALSE, PPTC_OPEN_DESKTOPPPT);
				else*/
					PostMessage(WM_COMMAND, COMMAND_OPEN_FORM_DESKTOP);
				//OpenDocByDesktop();
				Statistics::GetInstance()->Report(STAT_OPEN_PPT);
			}
		}
		break;
	case MSG_NEXT:
		NextSlideByThread();
		::SetForegroundWindow(CPPTController::GetSlideShowViewHwnd());
		::SetFocus(CPPTController::GetSlideShowViewHwnd());
		break;
	case MSG_PREV:
		PreviousSlideByThread();
		::SetForegroundWindow(CPPTController::GetSlideShowViewHwnd());
		::SetFocus(CPPTController::GetSlideShowViewHwnd());
		break;
	case MSG_CEFNEXT:
		if( g_Config::GetInstance()->GetModuleVisible(MODULE_ND_ICRPLAY) )
		{
		//互动课堂版本不进行翻页
		}else{
			NextSlideByThread();
			::SetForegroundWindow(CPPTController::GetSlideShowViewHwnd());
			::SetFocus(CPPTController::GetSlideShowViewHwnd());
		}
		break;
	case MSG_CEFPREV:
		if( g_Config::GetInstance()->GetModuleVisible(MODULE_ND_ICRPLAY) )
		{
			//互动课堂版本不进行翻页
		}else{
			PreviousSlideByThread();
			::SetForegroundWindow(CPPTController::GetSlideShowViewHwnd());
			::SetFocus(CPPTController::GetSlideShowViewHwnd());
		}
		break;
	case MSG_ESC:
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_TITLEBUTTON_OPERATION, MSG_PPT_STOP, 0);
		break;
	case MSG_DRAWMETHOD:
		{
			tstring nCefInfo = (LPCTSTR) pCopyDataStruct->lpData;
			tstring nFlag = "//*CEF*"; 
			int nFlagLen = nFlag.length();
			int eventNameLen = nCefInfo.find(nFlag);
			int eventDataStart = eventNameLen+nFlagLen;
			int eventDataEnd = nCefInfo.length()-eventDataStart;
			tstring eventName = nCefInfo.substr(0,eventNameLen);
			tstring eventData = nCefInfo.substr(eventDataStart,eventDataEnd);
			tstring strResult;
			if (m_pScreenInstrument)
			{
				m_pScreenInstrument->HandleDrawMessage(eventName.c_str(), eventData.c_str(), strResult);
			}
			CNdCefShareMemory* CefShareMemory = NdCefShareMemory::GetInstance();
			CefShareMemory->SetIcrEvernt(MSG_DRAWMETHOD,nCefInfo);
			//

		}
		break;
	case MSG_CEFINFO:
		{
			tstring nCefInfo = (LPCTSTR) pCopyDataStruct->lpData;
			tstring nFlag = "//*CEF*"; 
			int nFlagLen = nFlag.length();
			int eventNameLen = nCefInfo.find(nFlag);
			int eventDataStart = eventNameLen+nFlagLen;
			int eventDataEnd = nCefInfo.length()-eventDataStart;
			tstring eventName = nCefInfo.substr(0,eventNameLen);
			tstring eventData = nCefInfo.substr(eventDataStart,eventDataEnd);

			IcrPlayer::GetInstance()->IcrOnRecvExamEvent(eventName.c_str(), eventData.c_str());

		}
		//::GetPPTController()->Stop();
		break;
	case MSG_CEF_ICRINVOKENATIVE:
		{
			tstring nCefInfo = (LPCTSTR) pCopyDataStruct->lpData;

			CNdCefShareMemory* CefShareMemory = NdCefShareMemory::GetInstance();
			
			CefShareMemory->SetIcrEvernt(MSG_CEF_ICRINVOKENATIVE,nCefInfo);

			/*
			CefShareMemory->CreateFileMemoryMapping(GLOBAL_MEMORY_NAME,GLOBAL_EVENT_NAME);
			char* pResult = NULL;
			pResult = IcrPlayer::GetInstance()->IcrInvokeNativeMethod((char *)nCefInfo.c_str());

			// write student infos
			if( pResult != NULL )
			{
				CefShareMemory->WriteMemory(pResult, strlen(pResult),MEMORY_SIZE);
				IcrPlayer::GetInstance()->IcrReleaseMemory(pResult);
			}
			*/

		}
		break;
	case MSG_OBTAIN_ONLINE_STUDENTS:
		{
			CNdCefShareMemory* CefShareMemory = NdCefShareMemory::GetInstance();
			CefShareMemory->CreateFileMemoryMapping(GLOBAL_MEMORY_NAME,GLOBAL_EVENT_NAME);
			

			char* pStudentIds = NULL;
			IcrPlayer::GetInstance()->IcrGetOnlineStudents(&pStudentIds);

			// write student infos
			if( pStudentIds != NULL )
			{
				CefShareMemory->WriteMemory(pStudentIds, strlen(pStudentIds),MEMORY_SIZE);
				IcrPlayer::GetInstance()->IcrReleaseMemory(pStudentIds);
			}

		}
		break;
	case MSG_QUESTION_ADD:
		{
			// local
			string str = (LPCSTR) pCopyDataStruct->lpData;
			int pos = str.find("|");
			if( pos == -1 )
				break;

			string strType = str.substr(0, pos);
			string strLocalGuid = str.substr(pos+1);

			bool bBasicQuestion = atoi(strType.c_str());

			NDCloudLocalQuestionManager::GetInstance()->InsertQuestion(strLocalGuid, bBasicQuestion, true);
			//CToast::Toast(_STR_QUESTION_INSERTING, true, 3000);
		}
		break;
	case MSG_QUESTION_SAVE:
		{
			string str = (LPCSTR) pCopyDataStruct->lpData;
			int pos = str.find("|");
			if( pos == -1 )
				break;

			string strType = str.substr(0, pos);
			string strLocalGuid = str.substr(pos+1);

			bool bBasicQuestion = atoi(strType.c_str());

			NDCloudLocalQuestionManager::GetInstance()->UpdateQuestion(strLocalGuid, bBasicQuestion);
			//CToast::Toast(_STR_QUESTION_SAVING, true, 3000);
		}
		break;
	case MSG_QUESTION_ADDSAVE:
		{
			string str = (LPCSTR) pCopyDataStruct->lpData;
			int pos = str.find("|");
			if( pos == -1 )
				break;

			string strType = str.substr(0, pos);
			string strLocalGuid = str.substr(pos+1);

			bool bBasicQuestion = atoi(strType.c_str());

			NDCloudLocalQuestionManager::GetInstance()->InsertQuestion(strLocalGuid, bBasicQuestion, false);
			//CToast::Toast(_STR_QUESTION_SAVING, true, 3000);
		}
		break;
	case MSG_OPEN_FILE_WEBSITE:
		{
			// loading from website
			LPCTSTR szCommandLine = (LPCTSTR)pCopyDataStruct->lpData;		
			if( strstr(szCommandLine, _T("pptshell:")) != NULL )
			{
				CProgressLoadingUI* pLoadingDialog = new CProgressLoadingUI();
				pLoadingDialog->ShowWindow();
				pLoadingDialog->Start(szCommandLine, MakeHttpDelegate(NULL), MakeHttpDelegate(NULL));
			}
		}
		break;

	}

	return TRUE;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if((pMsg->message == WM_KEYUP ||  pMsg->message == WM_KEYDOWN) &&  pMsg->wParam == VK_F5)
	{	
		if( GetAsyncKeyState(VK_LBUTTON) == 0 )
			OnPptPlay();

		return FALSE;
	}		

	return CSkinFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::GenarateOperationer()
{
	srand((unsigned int)time(NULL));
	m_dwLastOperaionerId = GetTickCount() * rand();
}

bool CMainFrame::IsOperationerChanged( DWORD dwLastOperationerId )
{
	if (dwLastOperationerId != m_dwLastOperaionerId)
	{
		return true;
	}

	return false;
}

DWORD CMainFrame::GetOperationerId()
{
	return m_dwLastOperaionerId;
}

BOOL CMainFrame::PPTSaving( LPCTSTR strPath, BOOL bBefore, BOOL bClosePre, BOOL bICRPlay, int nCommand )
{
	tstring strTip = _T("正在保存请稍后...");
	ShadeWindowShow(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), strTip, true);
	//::SendMessage(getMainPane()->GetMainControlDlg()->GetSafeHwnd(), WM_SHOW_SHADE, (WPARAM)strTip.c_str(), 0);
	BOOL bRet = SavePPTByThread(strPath, bBefore, bClosePre, bICRPlay, nCommand);

	return bRet;
}

void CMainFrame::NDICRPlay(LPTSTR szBuff)//启动互动课堂播放
{
	//NDICRPlayThread((LPARAM)strPath.GetString());
	tstring strTip = _T("互动课堂正在运行中...");
	ShadeWindowShow(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), strTip, true);
	//::SendMessage(getMainPane()->GetMainControlDlg()->GetSafeHwnd(), WM_SHOW_SHADE, (WPARAM)strTip.c_str(), 0);

	CPPTICRPlayControl* pControl = PPTICRPlayControl::GetInstance();
	pControl->OnExportImagesCompleted();
	//PPTImagesExporter::GetInstance()->ExportImages(szBuff, 320, 240, MakeDelegate(pControl, &CPPTICRPlayControl::OnExportImagesCompleted));
}


BOOL CMainFrame::NDStartICRPlay(LPTSTR szPath)
{
	if(_tcslen(szPath))
	{
 		DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();
		if(!m_bPreView)
		{
			if(dwUserId == 0)
			{
				::PostMessage(getMainPane()->GetMainControlDlg()->GetSafeHwnd(),WM_SHOW_UCLOGIN,NULL,NULL); 
				CToast::Toast(_STR_PREVIEW_DIALOG_NOUSER);
				return FALSE;
			}

		}
 		
//		m_pMainPPTController = NULL;
//		CPPTControllerManager::UnInstance();
		NDICRPlay(szPath);

		return TRUE;
	}
	return FALSE;
}

void CMainFrame::SaveMainFrameHwnd()
{
	CPPTController::m_MainFrmHwnd=GetSafeHwnd();
	tstring strPath = GetLocalPath();
	strPath += _T("\\Setting\\Config.ini");
	TCHAR szHwnd[12]={0};
	wsprintf(szHwnd, "%d", GetSafeHwnd());
	WritePrivateProfileString(_T("config"),  _T("MainHwnd"),  szHwnd, strPath.c_str());
}


void CMainFrame::OnPptPlay()
{
	if ( !::GetPPTController()->IsInit() )
		return;
	/*
	if(g_Config::GetInstance()->GetModuleVisible(MODULE_ND_ICRPLAY) && isICRPlayer())
	{
		if ( ::GetPPTController()->GetSlideCount() == 0 )  //fix no slide to play
			return;

		m_nPlayMode = SelectPlayMode();
		int nRet = m_nPlayMode;
		if(nRet == ID_MSGBOX_BTN)
		{
			PlayPPTByThread();
		}
		else if(nRet == ID_MSGBOX_BTN + 1 || nRet == ID_MSGBOX_BTN + 2 )
		{
			if(nRet == ID_MSGBOX_BTN + 1 )
			{
				m_bPreView = TRUE;
				
			}
			else
			{
				m_bPreView = FALSE;

				DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();
				
				if(dwUserId == 0)
				{
					::PostMessage(getMainPane()->GetMainControlDlg()->GetSafeHwnd(),WM_SHOW_UCLOGIN,NULL,NULL); 
					CToast::Toast(_STR_PREVIEW_DIALOG_NOUSER);
					return ;
				}

				//IsPPTChangedReadOnlyByThread(FALSE, PPTC_NDICRPLAY);
			}
			PPTICRPlayControl::GetInstance()->OnExportImagesCompleted();
			
		}
	}
	else
	{
		PlayPPTByThread();
	}*/
	PlayPPTByThread();

	Statistics::GetInstance()->Report(STAT_PLAY_FULLSCREEN);
}

BOOL CMainFrame::IsUntitledShow()
{
	//CPPTShellDoc* pDoc = (CPPTShellDoc*) GetActiveDocument();
	CPPTShellView* pView = (CPPTShellView*)GetActiveView();
	if ( pView == NULL )
		return FALSE;

	return pView->IsUntitledWindowShow();
}

BOOL CMainFrame::SetUntitledShow( bool bShow )
{
	CPPTShellView* pView = (CPPTShellView*)GetActiveView();
	if ( pView == NULL )
		return FALSE;

	pView->ShowUntitledWindow(bShow); //SetUntitledWindowShow(bShow);
	::PostMessage(GetSafeHwnd(), WM_SHOW_ADD_SLIDE_WINDOW, NULL, NULL);
	//::PostMessage(GetSafeHwnd(), WM_SHOW_EXER_EDIT_WINDOW, NULL, NULL);
	return TRUE;
}

bool CMainFrame::OnEventLoginCompleteOperation( void* pNotify )
{
	TEventNotify* pENotify = (TEventNotify*)pNotify;
	m_nLoginType = pENotify->wParam;
	DWORD dwUserID = NDCloudUser::GetInstance()->GetUserId();
	if (dwUserID == 0)
	{
		m_pUserLoginBtn->SetCaption(_T("登录"));
		NDCloudUser::GetInstance()->SetFreeMode(1);//未登陆不弹提示，直接进入自由授课模式
	}
	else
	{		 
		//// class
		//tstring strClassId = NDCloudUser::GetInstance()->GetClassGuid(); 
		//if (strClassId == _T(""))//没有班级信息，直接进入自由授课模式
		//{
		//	NDCloudUser::GetInstance()->SetFreeMode(1);
		//}
		//else
		//{
		//	// students
		//	tstring strStudentInfo = NDCloudUser::GetInstance()->GetStudentInfo();

		//	bool bIsNull = NDCloudUser::GetInstance()->CheckItemsSize(strStudentInfo, "items");
		//	if (!bIsNull)//有班级信息但是没有学生信息，也进入自由授课模式
		//	{
		//		NDCloudUser::GetInstance()->SetFreeMode(1);
		//	}
		//	else
		//	{ 
		//		NDCloudUser::GetInstance()->SetFreeMode(0);
		//	}
		//}

		//登陆成功就设置为上课模式，如果数据有不全的，在调用icronstart的再做判断  2016.02.26 cws
		NDCloudUser::GetInstance()->SetFreeMode(0); 


		tstring sUNickName= NDCloudUser::GetInstance()->GetNickName();
		if (sUNickName.empty())
		{ 
			sUNickName= NDCloudUser::GetInstance()->GetUserName();
		}
		tstring sGradeName= NDCloudUser::GetInstance()->GetFullName();
		sGradeName = NDCloudUser::GetInstance()->GetTeacherGrade(sGradeName.c_str());

		//弹出提示切换班级成功信息
		POINT pt;
		//ScreenToClient(&pt);
		CRect lRect;
		::GetWindowRect(AfxGetMainWnd()->GetSafeHwnd(), &lRect);
		pt.x = lRect.right - 360;
		pt.y = lRect.top + 60; 
		LoginToolTipUI::GetInstance()->ShowLocalWindows(pt, sUNickName.c_str(), sGradeName.c_str());

		sUNickName += _T("老师");
		m_pUserLoginBtn->SetCaption(sUNickName.c_str());

		//tstring sUserFullName= NDCloudUser::GetInstance()->GetFullName();
		//tstring sUserGrade= NDCloudUser::GetInstance()->GetTeacherGrade(sUserFullName);//老师的班级信息
		//if (!sUserGrade.empty())
		//{
		//	CRect lRect;
		//	GetWindowRect(&lRect);
		//	LoginToolTipUI::GetInstance()->ShowLocalWindows();
		//}

		Image* pIcon		= CSkinManager::GetInstance()->GetSkinItem(_T("icon_login.png"));
		m_pUserLoginBtn->SetIconInfo(pIcon, 1, CSize(30, 30), CPoint(5,-1), TRUE, TRUE);
		 
	}
	return true;
}

void CMainFrame::ResetLoginImage()
{
	Image*		pIcon	= NULL;	
	m_pUserLoginBtn->SetCaption(_T("登录"));	
	//退出后图标恢复成默认的人头 2015.11.23 cws
	pIcon	= CSkinManager::GetInstance()->GetSkinItem(_T("btn_about.png"));
	m_pUserLoginBtn->SetIconInfo(pIcon, 3, CSize(30, 30), CPoint(5,-1), TRUE, TRUE);
}

bool CMainFrame::OnEventReLoginOperation( void* pNotify )
{ 
	//ResetLoginImage();
	return true;
}


bool CMainFrame::OnEventLogOutOperation( void* pNotify )
{ 
	ResetLoginImage();
	return true;
}

//bool CMainFrame::OnEventAboutSoft( void* pNotify )
//{
//	//POINT ptMouse;  
//	POINT pt = {0,0 };
//	ClientToScreen(&pt);
//	if ( m_pMenu == NULL || (m_pMenu && m_pMenu->GetHWND() == NULL ))
//	{
//		m_pMenu = new CAboutMenu();
//		m_pMenu->Create(AfxGetMainWnd()->GetSafeHwnd(), _T("MenuWnd"), WS_VISIBLE, 0, 0, 0, 0);
//
//	}
//	bool bNeedUpdate = (m_nUpdateStatus == eUpdateType_Has_Update);
//	m_pMenu->Init(AfxGetMainWnd()->GetSafeHwnd(), pt, bNeedUpdate, m_strVerion, m_strUpdateContent);
//	m_pMenu->ShowWindow(TRUE);
//	return true;
//
//}




BOOL CMainFrame::IsNdpx(tstring strPath)
{
	tstring strExt = strPath.substr(strPath.length() - 5, 5);

	if(_tcsicmp(strExt.c_str(), _T(".ndpx")) != 0)
		return FALSE;
	
	return TRUE;
}

BOOL CMainFrame::OpenNdpx( tstring strPath )
{
	TCHAR szDrive[8]			= {0};
	TCHAR szDir[MAX_PATH*2]		= {0};
	TCHAR szFileName[MAX_PATH*2]	= {0};
	TCHAR szExt[8]				= {0};
	if (_tsplitpath_s(strPath.c_str(), szDrive, szDir, szFileName, szExt))
	{
		return FALSE;
	}

	tstring strMd5 = CalcFileMD5(strPath.c_str());

	TCHAR szCloudDir[MAX_PATH * 2];
	tstring strNDCloudDirectory = NDCloudFileManager::GetInstance()->GetNDCloudDirectory();
	_stprintf(szCloudDir, _T("%s\\PPTTemp\\%s\\"), strNDCloudDirectory.c_str(), strMd5.c_str());

// 	TCHAR szPPTFileName[MAX_PATH*2]	= {0};
// 	TCHAR szPPTCRC[32]				= {0};
// 	tstring strPPTFileName = szFileName;
// 	if( sscanf_s(szFileName, _T("%[^_]_%s") , szPPTFileName, MAX_PATH*2, szPPTCRC , sizeof(szPPTCRC) - 1) == 2)
// 	{
// 		strPPTFileName = szPPTFileName;
// 	}

	tstring strCloudFilePPT = szCloudDir;
	strCloudFilePPT += _T("\\*.*");
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = ::FindFirstFile(strCloudFilePPT.c_str(), &FindFileData);
	if(hFind != INVALID_HANDLE_VALUE)
	{
		do 
		{
			tstring strFileName = FindFileData.cFileName;
			if(strFileName.find(_T("~$")) == tstring::npos && strFileName.find(_T(".ppt")) != tstring::npos )
			{
				strCloudFilePPT = szCloudDir;
				strCloudFilePPT += FindFileData.cFileName;
				FindClose(hFind);
				OpenPPTByThread(strCloudFilePPT.c_str(), FALSE);
				return TRUE;
			}
		}while (::FindNextFile(hFind, &FindFileData));
		FindClose(hFind);
	}
	

	CreateDirectory(szCloudDir, NULL);

	CPackPPTDialogUI * pPackPPTDialogUI = new CPackPPTDialogUI;
	pPackPPTDialogUI->Create(GetSafeHwnd(), _T("PackPPTDialog"), WS_POPUP, 0, 0, 0, 0, 0);
	CRect rect;
	::GetWindowRect(GetSafeHwnd(), &rect);
	pPackPPTDialogUI->Init(rect);

	pPackPPTDialogUI->ShowWindow(true);
	pPackPPTDialogUI->UnPack(strPath);

	return TRUE;
}

bool CMainFrame::OnSaveToDBankCompleted( void* param )
{
	TEventNotify* pNotify = (TEventNotify*)param;

	int nCount = pNotify->lParam;
	
	m_bUploadingOnClose = FALSE;

	ClosePPTByThread(FALSE);
	Sleep(100);
	if ( nCount <= 1 )
		OnDestroyBefore();
	else if ( m_bCloseAll )
		IsPPTChangedReadOnlyByThread(FALSE, PPTC_CLOSE);

	return true;
}


bool CMainFrame::OnQuestionDownloaded( void* param )
{
	THttpNotify* pNotify = (THttpNotify*)param;
	if( pNotify->strFilePath == _T("") )
	{
		CToast::Toast("习题下载失败");
		return false;
	}

	BOOL res = NDCloudQuestionManager::GetInstance()->ConvertQuestionToCourseFile(m_strQuestionGuid, m_strQuestionType, _T(""), pNotify->strFilePath);
	if( !res )
	{
		CToast::Toast(_STR_RIGHTBAR_QUESTION_ITEM_MAIN_SAVE_FAILED);
		return false;
	}

	tstring strMainXmlPath =  pNotify->strFilePath;
	strMainXmlPath += _T("\\main.xml");

	InsertQuestionByThread(strMainXmlPath.c_str(), _T(""), m_strQuestionGuid.c_str() );
	return true;
}

bool CMainFrame::OnCoursewareObjectDownloaded(void* param)
{

	THttpNotify* pNotify = (THttpNotify*)param;
	if( pNotify->strFilePath == _T("") )
	{
		CToast::Toast("习题下载失败");
		return false;
	}

	tstring strMainXmlPath =  pNotify->strFilePath;
	strMainXmlPath += _T("\\main.xml");

	InsertQuestionByThread(strMainXmlPath.c_str(), _T(""), m_strQuestionGuid.c_str() );
	return true;
}

LRESULT CMainFrame::OnShowUpdateTipMsg( WPARAM wParam, LPARAM lParam )
{
	//增加提示
	CUpdateTipDialogUI* pUpdateTipDlg = UpdateTipDialogUI::GetInstance();

	if(pUpdateTipDlg->GetHWND() == NULL)
		pUpdateTipDlg->Create(GetSafeHwnd(), _T("UpdateTipDialog"), WS_POPUP , 0, 0, 0, 0, 0);

	CRect rect;
	::GetWindowRect(GetSafeHwnd(), &rect);
	::MoveWindow(pUpdateTipDlg->GetHWND(), rect.left+5, rect.top +50 , 350 , 135, TRUE);
	pUpdateTipDlg->ShowWindow(true);

	return true;
}

LRESULT CMainFrame::OnShowUpdateInfoMsg( WPARAM wParam, LPARAM lParam )
{
	//增加提示
	CUpdateDialogUI* pUpdateDlg = UpdateDialogUI::GetInstance();

	if(pUpdateDlg->GetHWND() && ::IsWindowVisible(pUpdateDlg->GetHWND()))
	{
		pUpdateDlg->ShowUpdateInfo(m_update.GetUpdateInfoParam());
	}

	return true;
}

// 
LRESULT CMainFrame::OnUserPPTControl(WPARAM wParam, LPARAM lParam)
{
	CNotifyEventSource* pEventSource = (CNotifyEventSource*)wParam;
	if( pEventSource == NULL )
		return 0;

	TEventNotify* pNotify = (TEventNotify*)lParam;
	if( pNotify == NULL )
		return 0;

	bool res = (*pEventSource)(pNotify);
	return res;

}

LRESULT CMainFrame::OnQuestionThumbnail(WPARAM wParam, LPARAM lParam)
{
	CNotifyEventSource* pEventSource = (CNotifyEventSource*)wParam;
	if( pEventSource == NULL )
		return 0;

	TEventNotify* pNotify = (TEventNotify*)lParam;
	if( pNotify == NULL )
		return 0;

	bool res = (*pEventSource)(pNotify);
	return res;

}


// about add slide window

LRESULT CMainFrame::OnShowAddSlideWindow( WPARAM wParam, LPARAM lParam )
{

	//增加提示
	CAddSlideFloatWindow* pAddSlideWindow = AddSlideUI::GetInstance();

	if ( pAddSlideWindow == NULL )
		return false;

	if(pAddSlideWindow->GetHWND() == NULL)
		pAddSlideWindow->Create(GetSafeHwnd(), _T("AddSlideWindow"), WS_POPUP|WS_CHILDWINDOW , 0, 0, 0, 0, 0);

	if ( IsIconic() || IsUntitledShow() )
	{
		pAddSlideWindow->ShowWindow(false);
		return true;
	}

	CRect rtMain;
	::GetWindowRect(GetSafeHwnd(), &rtMain);
		
	int nPosX = rtMain.left+20;	
	int nPosY = rtMain.bottom - 70;

	if ( IsZoomed() )
	{
		nPosX += 6;
		nPosY -= 6;
	}

	::MoveWindow(pAddSlideWindow->GetHWND(), nPosX, nPosY, 86, 38, FALSE);
	pAddSlideWindow->ShowWindow(true, false);

	return true;
}

void CMainFrame::ShowAddSlideWindow()
{
	CAddSlideFloatWindow* pSlideWindow = AddSlideUI::GetInstance();
	if(pSlideWindow != NULL && pSlideWindow->GetHWND() && ::IsWindowVisible(pSlideWindow->GetHWND()))
		::PostMessage(GetSafeHwnd(), WM_SHOW_ADD_SLIDE_WINDOW, NULL, NULL);
}

void CMainFrame::HideAddSlideWindow()
{
	CAddSlideFloatWindow* pAddSlideWindow = AddSlideUI::GetInstance(); //hide addslidewindow
	if ( pAddSlideWindow != NULL && ::IsWindowVisible(pAddSlideWindow->GetHWND()))
		pAddSlideWindow->ShowWindow(false);
}


LRESULT CMainFrame::OnShowExerEditWindow( WPARAM wParam, LPARAM lParam )
{
	//增加提示
	CExercisesEditFloatWindow* pExerEditWindow = ExercisesEditUI::GetInstance();

	if ( pExerEditWindow == NULL )
		return false;

	if(pExerEditWindow->GetHWND() == NULL)
	{
		pExerEditWindow->Create(GetPPTController()->GetActiveWnd(), _T("ExercisesEditWindow"), WS_POPUP|WS_CHILDWINDOW , 0, 0, 0, 0, 0);
	}

	if ( IsIconic() || IsUntitledShow() )
	{
		pExerEditWindow->ShowWindow(false);
		return true;
	}

	CRect rtMain;
	::GetWindowRect(GetSafeHwnd(), &rtMain);

	CRect crtPanel = CRect(0, 0, 0, 0);

	if ( m_wndMainPane )
		m_wndMainPane.GetClientRect(crtPanel);

	int nPosX = rtMain.right - 52 - crtPanel.Width();	
	int nPosY = rtMain.top + 205;

	if ( IsZoomed() )
	{
		nPosX -= 3;
		nPosY += 3;
	}

	::MoveWindow(pExerEditWindow->GetHWND(), nPosX, nPosY, 48, 65, FALSE);
	pExerEditWindow->ShowWindow(true, false);
	//::SetFocus(GetPPTController()->GetActiveWnd()); 
	
	return true;
}

void CMainFrame::ShowExercisesEditWindow(tstring strPath)
{
	if ( CPPTController::GetSlideShowViewHwnd() == NULL )
	{
#ifdef DEVELOP_VERSION
		::PostMessage(GetSafeHwnd(), WM_SHOW_EXER_EDIT_WINDOW, NULL, NULL);
		CExercisesEditFloatWindow* pExerEditWindow = ExercisesEditUI::GetInstance();
		if(pExerEditWindow != NULL)
			pExerEditWindow->GetExercisesInfo(strPath);
#endif
	}
}

void CMainFrame::MoveExercisesEditWindow()
{
#ifdef DEVELOP_VERSION
	CExercisesEditFloatWindow* pExerEditWindow = ExercisesEditUI::GetInstance();
	if(pExerEditWindow != NULL && pExerEditWindow->GetHWND() && ::IsWindowVisible(pExerEditWindow->GetHWND()))
		::PostMessage(GetSafeHwnd(), WM_SHOW_EXER_EDIT_WINDOW, NULL, NULL);
#endif

}

void CMainFrame::HideExercisesEditWindow()
{
#ifdef DEVELOP_VERSION
	CExercisesEditFloatWindow* pExerEditWindow = ExercisesEditUI::GetInstance();
	if(pExerEditWindow != NULL && pExerEditWindow->GetHWND() && ::IsWindowVisible(pExerEditWindow->GetHWND()))
		pExerEditWindow->ShowWindow(false);
#endif
}

//VR Float Window

LRESULT CMainFrame::OnShowVRDistributeWindow( WPARAM wParam, LPARAM lParam )
{
	//增加提示
	CVRDistributeFloatWindow* pVRDistributeWindow = VRDistributeUI::GetInstance();

	if ( pVRDistributeWindow == NULL )
		return false;

	if(pVRDistributeWindow->GetHWND() == NULL)
	{
		pVRDistributeWindow->Create(GetPPTController()->GetActiveWnd(), _T("VRDistributeWindow"), WS_POPUP|WS_CHILDWINDOW , 0, 0, 0, 0, 0);
	}

	if ( IsIconic() || IsUntitledShow() )
	{
		pVRDistributeWindow->ShowWindow(false);
		return true;
	}

	CRect rtMain;
	::GetWindowRect(GetSafeHwnd(), &rtMain);

	CRect crtPanel = CRect(0, 0, 0, 0);

	if ( m_wndMainPane )
		m_wndMainPane.GetClientRect(crtPanel);

	int nPosX = rtMain.right - 52 - crtPanel.Width();	
	int nPosY = rtMain.top + 205;

	if ( IsZoomed() )
	{
		nPosX -= 3;
		nPosY += 3;
	}

	::MoveWindow(pVRDistributeWindow->GetHWND(), nPosX, nPosY, 48, 65, FALSE);
	pVRDistributeWindow->ShowWindow(true, false);
	//::SetFocus(GetPPTController()->GetActiveWnd()); 

	return true;
}

void CMainFrame::ShowVRDistributeWindow(tstring strPath)
{
//#ifdef DEVELOP_VERSION
	if ( g_Config::GetInstance()->GetModuleVisible(MODULE_ND_ICRPLAY) && CPPTController::GetSlideShowViewHwnd() == NULL )
	{
		::PostMessage(GetSafeHwnd(), WM_SHOW_VR_DISTRIBUTE_WINDOW, NULL, NULL);
		CVRDistributeFloatWindow* pVRDistributeWindow = VRDistributeUI::GetInstance();
		if(pVRDistributeWindow != NULL)
			pVRDistributeWindow->GetVRPackageInfo(strPath);
	}
//#endif

}

void CMainFrame::MoveVRDistributeWindow()
{
//#ifdef DEVELOP_VERSION
	CVRDistributeFloatWindow* pVRDistributeWindow = VRDistributeUI::GetInstance();
	if(pVRDistributeWindow != NULL && pVRDistributeWindow->GetHWND() && ::IsWindowVisible(pVRDistributeWindow->GetHWND()))
		::PostMessage(GetSafeHwnd(), WM_SHOW_VR_DISTRIBUTE_WINDOW, NULL, NULL);
//#endif

}

void CMainFrame::HideVRDistributeWindow()
{
//#ifdef DEVELOP_VERSION
	CVRDistributeFloatWindow* pVRDistributeWindow = VRDistributeUI::GetInstance();
	if(pVRDistributeWindow != NULL && pVRDistributeWindow->GetHWND() && ::IsWindowVisible(pVRDistributeWindow->GetHWND()))
		pVRDistributeWindow->ShowWindow(false);
//#endif
}

BOOL CMainFrame::IsMainFrmActive()
{	
	HWND hPop = ::GetLastActivePopup( m_hMainWnd );
	//if ((hPop && m_hWnd != hPop) )
	//{
	//	//修复桌面双击打开时，提示当前对话框被打开导致ppt打开失败的bug
	//	if( AddSlideUI::GetInstance()->GetHWND() != hPop
	//		&& ExercisesEditUI::GetInstance()->GetHWND() != hPop
	//		&&  CToast::GetInstance(m_hWnd)->GetHWND() != hPop)
	//	{
	//		return FALSE;
	//	}
	//}

	if (hPop )
	{
		if( m_hMainWnd == hPop 
			|| AddSlideUI::GetInstance()->GetHWND() == hPop
			|| ExercisesEditUI::GetInstance()->GetHWND() == hPop
			|| VRDistributeUI::GetInstance()->GetHWND() == hPop
			|| CToast::GetInstance(m_hMainWnd)->GetHWND() == hPop
		)
		{
			return TRUE;
		}
	}

	return FALSE;
}


bool CMainFrame::OnDanmukuWordAdd( void* pNotify )
{

	::ShowWindow(m_lyricsWindow.m_hWnd, SW_SHOW);

	TEventNotify* tNotify = (TEventNotify*)pNotify;
	CStream* pStream = (CStream*)tNotify->wParam;
	pStream->ResetCursor();

	tstring strDanmukuWord = pStream->ReadString();
	LPWSTR wstrDanmukuWord;

	int nLen=MultiByteToWideChar(CP_ACP,0,strDanmukuWord.c_str(),-1, NULL,NULL);
	if(nLen){
		wstrDanmukuWord = new WCHAR[nLen+1];
		ZeroMemory(wstrDanmukuWord,sizeof(WCHAR)*(nLen+1));
		MultiByteToWideChar(CP_ACP,0,strDanmukuWord.c_str(),-1,wstrDanmukuWord,nLen); 
	}		

	m_lyricsWindow.AddString(wstrDanmukuWord);

	delete pStream;
	return true;
}

LRESULT CMainFrame::OnDownloaderClose( WPARAM wParam, LPARAM lParam )
{
	if (m_pDownloadMgr)
	{
		m_pDownloadMgr->SetCheck(FALSE);
	}
	return TRUE;
}

void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: Add your message handler code here and/or call default
	if (nID == SC_MINIMIZE)
	{
		if (m_lyricsWindow.m_hWnd != NULL && ::IsWindowVisible(m_lyricsWindow.m_hWnd))
		{
			::ShowWindow(m_lyricsWindow.m_hWnd, SW_HIDE);
		}
	}

	if (nID == SC_RESTORE)
	{
		if (m_lyricsWindow.m_hWnd != NULL && ::IsWindowVisible(m_lyricsWindow.m_hWnd) == FALSE)
		{
			::ShowWindow(m_lyricsWindow.m_hWnd, SW_SHOW);
		}
	}
	
	CSkinFrameWnd::OnSysCommand(nID, lParam);
}

void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	CSkinFrameWnd::OnSetFocus(pOldWnd);

	::PostMessage(CPPTController::GetPPTHwnd(), WM_NCACTIVATE, 1, 0); //让子窗口处于激活状态
}

void CMainFrame::UploadQuestion(tstring strTitle,tstring strGuid,bool bBasicQuestion)
{
	if (NDCloudLocalQuestionManager::GetInstance()){
		NDCloudLocalQuestionManager::GetInstance()->UploadQuestion(strTitle,strGuid, bBasicQuestion, MakeHttpDelegate(NULL));
	}
}
// 获取Setting\Config.ini的班级名称，最长MAX_PATH字节
tstring CMainFrame::GetConfigClassName()
{
	if(m_strConfigClassName.empty())
	{
		TCHAR szClassName[MAX_PATH];

		tstring strPath = GetLocalPath();
		strPath += _T("\\Setting\\Config.ini");
		GetPrivateProfileString(_T("Config"), _T("ClassName"), _T(""), szClassName, MAX_PATH, strPath.c_str());

		m_strConfigClassName = szClassName;
	}

	return m_strConfigClassName;
}
// 设置自定义的班级名称
// 当bWriteConfigFile为TRUE时，将班级名称写入Config.ini中。
BOOL CMainFrame::SetConfigClassName(tstring strConfigClassName, BOOL bWriteConfigFile)
{
	BOOL bReturn = TRUE;
	m_strConfigClassName = strConfigClassName;
	if(bWriteConfigFile)
	{
		tstring strPath = GetLocalPath();
		strPath += _T("\\Setting\\Config.ini");
		bReturn = WritePrivateProfileString(_T("Config"), _T("ClassName"), strConfigClassName.c_str(), strPath.c_str());
	}

	return bReturn;
}

