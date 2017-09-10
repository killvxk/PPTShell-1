//===========================================================================
// FileName:				PPTEventHandler.cpp
// 
// Desc:				 
//============================================================================
#include "stdafx.h"
#include "GUI\MainFrm.h"
#include "PPTEventHandler.h"
#include "EventCenter/EventDefine.h"
#include "PPTControl/PPTControllerManager.h"
#include "ActiveX/CDandcef.h"
#include "Common.h"
#include "Config.h"
#include "DUI/Toast.h"
#include "DUI/LaserPointerDlg.h"


bool CPPTEventHandler::m_bAppShow	= TRUE;
int CPPTEventHandler::m_nPPTCount	= 0;

IMPLEMENT_DYNAMIC(CPPTEventHandler, CCmdTarget)


CPPTEventHandler::CPPTEventHandler():
m_MoniterThread(NULL),
m_hWaitRecoveryThread(NULL),
m_PPTHwnd(NULL),
m_bNeedCheck(false),
m_bRecovery(false),
m_bNewPPT(false),
m_bInSidePPTPlay(false),
m_strPPTTitleName(_T(""))
{
	EnableAutomation();

	m_bIcrPlayer = g_Config::GetInstance()->GetModuleVisible(MODULE_ND_ICRPLAY);
};

CPPTEventHandler::~CPPTEventHandler()
{
	m_bNeedCheck = false;
}

void CPPTEventHandler::OnFinalRelease()
{
	// 释放了对自动化对象的最后一个引用后，将调用
	// OnFinalRelease。基类将自动
	// 删除该对象。在调用该基类之前，请添加您的
	// 对象所需的附加清理代码。
	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CPPTEventHandler, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CPPTEventHandler, CCmdTarget)
	//DISP_FUNCTION_ID(CPPTEventHandler,"WindowSelectionChange",2001,WindowSelectionChange, VT_EMPTY, VTS_DISPATCH)
	DISP_FUNCTION_ID(CPPTEventHandler,"WindowBeforeRightClick",2002,WindowBeforeRightClick, VT_EMPTY , VTS_DISPATCH VTS_BOOL)
	DISP_FUNCTION_ID(CPPTEventHandler,"WindowBeforeDoubleClick",2003,WindowBeforeDoubleClick, VT_EMPTY, VTS_DISPATCH VTS_BOOL)
	DISP_FUNCTION_ID(CPPTEventHandler,"PresentationClose",2004,PresentationClose, VT_EMPTY, VTS_DISPATCH)
	DISP_FUNCTION_ID(CPPTEventHandler,"PresentationSave",2005,PresentationSave, VT_EMPTY, VTS_DISPATCH)
	DISP_FUNCTION_ID(CPPTEventHandler,"PresentationOpen",2006,PresentationOpen, VT_EMPTY, VTS_DISPATCH)
	DISP_FUNCTION_ID(CPPTEventHandler,"NewPresentation",2007,NewPresentation, VT_EMPTY, VTS_DISPATCH)
	DISP_FUNCTION_ID(CPPTEventHandler,"PresentationNewSlide",2008,PresentationNewSlide, VT_EMPTY, VTS_DISPATCH)
	DISP_FUNCTION_ID(CPPTEventHandler,"WindowActivate",2009,WindowActivate, VT_EMPTY, VTS_DISPATCH VTS_DISPATCH)
	DISP_FUNCTION_ID(CPPTEventHandler,"WindowDeactivate",2010,WindowDeactivate, VT_EMPTY, VTS_DISPATCH VTS_DISPATCH)
	DISP_FUNCTION_ID(CPPTEventHandler,"SlideShowBegin",2011,SlideShowBegin, VT_EMPTY, VTS_DISPATCH )
	DISP_FUNCTION_ID(CPPTEventHandler,"SlideShowNextBuild",2012,SlideShowNextBuild, VT_EMPTY, VTS_DISPATCH )
	DISP_FUNCTION_ID(CPPTEventHandler,"SlideShowNextSlide",2013,SlideShowNextSlide,VT_EMPTY, VTS_DISPATCH )
	DISP_FUNCTION_ID(CPPTEventHandler,"SlideShowEnd",2014,SlideShowEnd, VT_EMPTY, VTS_DISPATCH )
	DISP_FUNCTION_ID(CPPTEventHandler,"PresentationPrint",2015,PresentationPrint, VT_EMPTY, VTS_DISPATCH )
	DISP_FUNCTION_ID(CPPTEventHandler,"SlideSelectionChanged",2016,SlideSelectionChanged, VT_EMPTY, VTS_DISPATCH )
	//DISP_FUNCTION_ID(CPPTEventHandler,"ColorSchemeChanged",2017,ColorSchemeChanged, VT_EMPTY, VTS_DISPATCH )
	DISP_FUNCTION_ID(CPPTEventHandler,"PresentationBeforeSave",2018,PresentationBeforeSave, VT_EMPTY, VTS_DISPATCH VTS_BOOL)
	//DISP_FUNCTION_ID(CPPTEventHandler,"SlideShowNextClick",2019,SlideShowNextClick, VT_EMPTY, VTS_DISPATCH VTS_DISPATCH )
	DISP_FUNCTION_ID(CPPTEventHandler,"AfterNewPresentation",2020,AfterNewPresentation, VT_EMPTY, VTS_DISPATCH )
	DISP_FUNCTION_ID(CPPTEventHandler,"AfterPresentationOpen",2021,AfterPresentationOpen, VT_EMPTY, VTS_DISPATCH )
	//DISP_FUNCTION_ID(CPPTEventHandler,"PresentationSync",2022,PresentationSync, VT_EMPTY, VTS_DISPATCH VTS_I4 )
	DISP_FUNCTION_ID(CPPTEventHandler,"SlideShowOnNext",2025,SlideShowOnNext, VT_EMPTY, VTS_DISPATCH )
	//DISP_FUNCTION_ID(CPPTEventHandler,"SlideShowOnPrevious",2024,SlideShowOnPrevious, VT_EMPTY, VTS_DISPATCH )
END_DISPATCH_MAP()

static const IID IID_IPPTEventsHandler =
{ 0x57A320C, 0x1967, 0x48E5, { 0xAD, 0x3A, 0xBA, 0x80, 0xDB, 0xED, 0x5C, 0x53 } };

void CPPTEventHandler::WindowSelectionChange( LPDISPATCH Pres )
{
	//OutputDebugString(_T("WindowSelectionChange\r\n"));
}

void CPPTEventHandler::WindowBeforeRightClick( LPDISPATCH Pres, VARIANT_BOOL* Cancel )
{
	//OutputDebugString(_T("WindowBeforeRightClick\r\n"));
}

void CPPTEventHandler::WindowBeforeDoubleClick( LPDISPATCH Pres, VARIANT_BOOL* Cancel )
{

}

void CPPTEventHandler::PresentationClose( LPDISPATCH Pres )
{
	//OutputDebugString(_T("PresentationClose\r\n"));
	
	m_nPPTCount--;

	//StartMainWindowMoniter(Pres);
}

void CPPTEventHandler::PresentationSave( LPDISPATCH Pres )
{	
	try
	{
		CMainFrame* pMain =(CMainFrame* )AfxGetApp()->m_pMainWnd;

	if ( CPPTController::GetOperationPPTEvent() == OPE_USER_PACK )
		return;

	if ( !pMain->m_bClosing )
	{
		tstring strPath		= ((_PresentationPtr)Pres)->GetPath();

		if ( !strPath.empty() )
		{
			TagsPtr pTags	= ((_PresentationPtr)Pres)->GetTags(); // 附加一个线程中关闭的标志，过滤掉线程中的回调和通知
			if(pTags!=NULL)
			{
				tstring strInfo	= pTags->Item(_T("save"));
				if( _tcsicmp(strInfo.c_str(), _T("ExportToImages"))!= 0 )
				{
					tstring strFullPath = ((_PresentationPtr)Pres)->GetFullName();
					CStream* pStream = new CStream(256);
					pStream->WriteString(strFullPath);
					BroadcastPostedEvent(EVT_PPT_FILE_OPERATION, SAVE_FILE, (LPARAM)pStream, 0);
					}else
					{
						pTags->Delete(_T("save"));
					}
				}
			}
		}

		pMain->m_bClosing = FALSE;
	}catch(...)
	{
		WRITE_LOG_DEBUG("PresentationSave error");
	}
}

void CPPTEventHandler::PresentationOpen( LPDISPATCH Pres )
{
	try
	{
	StartMainWindowMoniter(Pres);
	OutputDebugString(_T("PresentationOpen\r\n"));
	m_bNewPPT	= TRUE;
	m_bAppShow	= TRUE;

		if ( CPPTController::GetPPTVersion() != PPTVER_2010 )		//2010插入时关闭没有Close消息
			AddPPTCount();

	CMainFrame* pMain =(CMainFrame* )AfxGetApp()->m_pMainWnd;

	int nOperationPpt = CPPTController::GetOperationPPTEvent();
	if ( OPE_USER_INSERT == nOperationPpt || OPE_USER_EXPORT == nOperationPpt || OPE_USER_PACK == nOperationPpt )
	{
		m_bNewPPT	= FALSE;
		return;
	}
		if ( CPPTController::GetPPTVersion() == PPTVER_2010 )		//2010插入时关闭没有Close消息
			AddPPTCount();

	pMain->SetUntitledShow(false);

	_PresentationPtr pPresenation = (_PresentationPtr)Pres;
	_ApplicationPtr AppPtr = pPresenation->GetApplication();
	TagsPtr pTags = pPresenation->GetTags();

	tstring strName = pPresenation->GetName();
	tstring strPath = pPresenation->GetPath();

	TCHAR szTitle[MAX_PATH*2] = {0};
	::GetWindowText((HWND)AppPtr->GetHWND(), szTitle, MAX_PATH*2 - 1);

#ifdef _DEBUG
	TCHAR szMsg[1024] = {0};
	wsprintf(szMsg, _T("Name:%s - Path:%s\r\n"), strName.c_str(), strPath.c_str());
	OutputDebugString(szMsg);
#endif

	//fiter special open mode
	if ( m_PPTHwnd == (HWND)AppPtr->GetHWND() && strPath.empty()
		&& (strName.rfind(_T(" 中的 幻灯片")) != tstring::npos 
			|| strName.rfind(_T(" 中的 演示文稿")) != tstring::npos
			|| strName.compare(_T("标题版式")) == 0 )
		)  //从内部打开。
	{
		CRect rt;
		GetClientRect(pMain->m_hWnd, &rt);
		pMain->PostMessage(WM_SIZE, 0, MAKELONG(rt.Width()-1, rt.Height()-1));
		pMain->PostMessage(WM_SIZE, 0, MAKELONG(rt.Width(), rt.Height()));
		OutputDebugString(_T("Open By  in PPT -- PresentationOpen"));
		pTags->Add(PPT_GUID, PPT_INSIDE_PPT);
		m_bNewPPT	= FALSE;
		return;
	}

		//内部打开的重用幻灯片

		if ( m_PPTHwnd == (HWND)AppPtr->GetHWND() && 
			strName.length() != 0 &&
			_tcsnicmp(szTitle, _T("Microsoft PowerPoint"), _tcslen(szTitle)) != 0 )
		{	 
			BOOL bReusing = FALSE;

			if ( strPath.empty() )
				bReusing = TRUE;
			else if ( CPPTController::GetPPTVersion() == PPTVER_2010 )
			{
				int nPos = strName.rfind(_T(".")); //2010 取得的标题有的不带.ppt 后缀
				if ( nPos != tstring::npos )
					strName = strName.substr(0, nPos);

				if (_tcsnicmp(strName.c_str(), szTitle, strName.length()) != 0)
					bReusing = TRUE;
			}

			if ( bReusing )
			{
				pTags->Add(PPT_GUID, PPT_REUSING_PPT);
				m_bNewPPT	= FALSE;
				return;
			}
			
		}


	tstring strGUID = pTags->Item(PPT_GUID);

	if ( strGUID == PPT_INSIDE_PPT ) //检测是不是内部PPT
	{
		m_bNewPPT	= FALSE;
		return;
	}

	strGUID = GenerateGuid();

	BOOL bFinal = pPresenation->GetFinal();

	if ( bFinal )
		pPresenation->PutFinal(VARIANT_FALSE);

	pTags->Add(PPT_GUID, strGUID.c_str());

	CPPTController::UpdatePresenationGuid(strGUID);

	if ( m_PPTHwnd != NULL )
		ShowWindow(m_PPTHwnd, SW_HIDE);

	{
		m_PPTHwnd	= (HWND)AppPtr->GetHWND();
		AttatchPPTWindow(m_PPTHwnd, pMain);
		CPPTController::UpdatePPTHwnd(m_PPTHwnd);
	}

	if ( OPE_POWERPOINT == CPPTController::GetOperationPPTEvent() )
	{
		OutputDebugString(_T("Other--PresentationOpen\r\n"));

		//判断恢复文档时我们等待
	
		TCHAR szTitle[MAX_PATH] = {0}; 
		::GetWindowText(CPPTController::GetPPTHwnd(), szTitle, MAX_PATH);
		TCHAR* pPos = NULL;
		pPos = _tcsrchr(szTitle, '.');
		if ( (pPos != NULL) && _tcsstr(pPos, _T("保存")) != NULL )
		{
			//pMain->SetUntitledShow(true);
			pTags->Add(PPT_RECOVER, PPT_RECOVER);
			m_bRecovery = TRUE;
		}
		
		if ( !m_bRecovery )
		{
			tstring strFullName = pPresenation->GetFullName();

			CStream* pStream = new CStream(256);
			pStream->WriteDWORD(OPEN_FILE);
			pStream->WriteString(strGUID);
			pStream->WriteString(strFullName);
			
			BroadcastPostedEvent(EVT_PPT_FILE_OPERATION, CHANGE_FILE, (LPARAM)pStream, 0);
		}
	}

	pPresenation->PutSaved(Office::msoTrue);

	if ( bFinal )
		pPresenation->PutFinal(VARIANT_TRUE);

	m_bNewPPT	= FALSE;

	//m_bNeedCheck = false;

	//ChangePPTWorkDirectory(m_PPTHwnd, strPath);
	//StartMainWindowMoniter(Pres);
	//ShowWindow(m_PPTHwnd, SW_SHOW);
	}catch(...)
	{
		WRITE_LOG_DEBUG("PresentationOpen error");
	}
}

void CPPTEventHandler::AfterPresentationOpen( LPDISPATCH Pres )
{
	OutputDebugString(_T("AfterPresentationOpen\r\n"));
	
	try
	{
	_PresentationPtr pPresenation = (_PresentationPtr)Pres;
	TagsPtr pTags = pPresenation->GetTags();

	if ( pTags == NULL )
		return;

	tstring strRecovery = pTags->Item(PPT_RECOVER);

	if ( strRecovery == PPT_RECOVER )
	{
		m_bRecovery	= FALSE;
		
		CMainFrame* pMain =(CMainFrame* )AfxGetApp()->m_pMainWnd;
		pMain->SetUntitledShow(false);

		BOOL bFinal = pPresenation->GetFinal();

		if ( bFinal )
			pPresenation->PutFinal(VARIANT_FALSE);

		pTags->Delete(PPT_RECOVER);

		pPresenation->PutSaved(Office::msoTrue);

		if ( bFinal )
			pPresenation->PutFinal(VARIANT_TRUE);

		tstring strGuid = pTags->Item(PPT_GUID);
		tstring strFullName = pPresenation->GetFullName();

		CStream* pStream = new CStream(256);
		pStream->WriteDWORD(OPEN_FILE);
		pStream->WriteString(strGuid);
		pStream->WriteString(strFullName);

		BroadcastPostedEvent(EVT_PPT_FILE_OPERATION, CHANGE_FILE, (LPARAM)pStream, 0);
	}
	}catch(...)
	{
		WRITE_LOG_DEBUG("AfterPresentationOpen error");
	}
}


void CPPTEventHandler::NewPresentation( LPDISPATCH Pres )
{
	OutputDebugString(_T("NewPresentation\r\n"));
	try
	{

	m_bNewPPT	= TRUE;
	m_bAppShow	= TRUE;

	AddPPTCount();

	StartMainWindowMoniter(Pres);

	CMainFrame* pMain =(CMainFrame* )AfxGetApp()->m_pMainWnd;

	int nOperationPpt = CPPTController::GetOperationPPTEvent();
	if ( OPE_USER_INSERT == nOperationPpt || OPE_USER_EXPORT == nOperationPpt || OPE_USER_PACK == nOperationPpt )
	{
		m_bNewPPT	= FALSE;
		return;
	}

	pMain->SetUntitledShow(false);

	_PresentationPtr pPresenation	= (_PresentationPtr)Pres;
	_ApplicationPtr AppPtr			= pPresenation->GetApplication();
	PresentationsPtr pPresentations	= AppPtr->GetPresentations();

	TagsPtr pTags = pPresenation->GetTags();

	tstring strName = pPresenation->GetName();
	tstring strPath = pPresenation->GetPath();

	if ( m_PPTHwnd == (HWND)AppPtr->GetHWND() && strPath.empty()
		&& (strName.rfind(_T(" 中的 幻灯片")) != tstring::npos 
		|| strName.rfind(_T(" 中的 演示文稿")) != tstring::npos )
		)  //从内部打开。
	{
		OutputDebugString(_T("Open By  in PPT -- PresentationOpen\r\n"));
		pTags->Add(PPT_GUID, PPT_INSIDE_PPT);
		m_bNewPPT	= FALSE;
		return;
	}

	tstring strGUID = pTags->Item(PPT_GUID);

	if ( strGUID == PPT_INSIDE_PPT ) //检测是不是内部PPT
	{
		m_bNewPPT	= FALSE;
		return;
	}

	strGUID = GenerateGuid();

	pTags->Add(PPT_GUID, strGUID.c_str());
	CPPTController::UpdatePresenationGuid(strGUID);

	WRITE_LOG_LOCAL(_T("GUID -- %s"), strGUID.c_str());

	if ( m_PPTHwnd != NULL )
		ShowWindow(m_PPTHwnd, SW_HIDE);
	//{
		m_PPTHwnd	= (HWND)AppPtr->GetHWND();
		CPPTController::UpdatePPTHwnd(m_PPTHwnd);
		AttatchPPTWindow(m_PPTHwnd, pMain);
	//}

	if ( OPE_POWERPOINT == CPPTController::GetOperationPPTEvent() )
	{
		OutputDebugString(_T("Other--NewPresentation\r\n"));

		tstring strTitle = pPresenation->GetName();

		CStream* pStream = new CStream(256);
		pStream->WriteDWORD(NEW_FILE);
		pStream->WriteString(strGUID);
		pStream->WriteString(strTitle);

		BroadcastPostedEvent(EVT_PPT_FILE_OPERATION, CHANGE_FILE, (LPARAM)pStream, 0);
	}

	m_bNewPPT	= FALSE;

	//pPresenation->PutSaved(Office::msoTrue);

	//m_bNeedCheck = false;
	//ShowWindow(m_PPTHwnd, SW_SHOW);
	}catch(...)
	{
		WRITE_LOG_DEBUG("NewPresentation error");
	}
}

void CPPTEventHandler::AfterNewPresentation( LPDISPATCH Pres )
{
	//OutputDebugString(_T("AfterNewPresentation\r\n"));
/*	CMainFrame* pMain =(CMainFrame* )AfxGetApp()->m_pMainWnd;
	if ( !pMain->GetPPTController()->IsMyOperation() )
	{
		_ApplicationPtr AppPtr = ((_PresentationPtr)Pres)->GetApplication();
		long hwNew = AppPtr->GetHWND();
		BroadcastPostedEvent(EVT_PPT_FILE_OPERATION, AFTER_NEW_FILE, hwNew, 0);
	}	*/ 
	//CStream* pStream = new CStream(256);
	//pStream->WriteDWORD(hwNew);
	//BroadcastPostedEvent(EVT_PPT_FILE_OPERATION, AFTER_NEW_FILE, hwNew, 0);
}


void CPPTEventHandler::PresentationNewSlide( LPDISPATCH Pres )
{
	OutputDebugString(_T("PresentationNewSlide\r\n"));
}

void CPPTEventHandler::WindowActivate( LPDISPATCH Pres,LPDISPATCH Wn )
{
	OutputDebugString(_T("WindowActivate\r\n"));

	try
	{
	//插入相册在这处理

	CMainFrame* pMain = (CMainFrame* )AfxGetApp()->m_pMainWnd;

	if ( pMain == NULL )
		return;

	int nOperationPpt = CPPTController::GetOperationPPTEvent();
	if ( OPE_USER_INSERT == nOperationPpt || OPE_USER_EXPORT == nOperationPpt || OPE_USER_PACK == nOperationPpt )
		return;

	pMain->SetUntitledShow(false);

	_PresentationPtr pPresenation = (_PresentationPtr)Pres;
	_ApplicationPtr AppPtr = pPresenation->GetApplication();

	m_PPTHwnd = (HWND)AppPtr->GetHWND();

	SetForegroundWindow(m_PPTHwnd);

	CPPTController::UpdatePPTHwnd(m_PPTHwnd);

	tstring strName = pPresenation->GetName();
	tstring strPath = pPresenation->GetPath();

	if ( !(strName == _T("相册") && strPath.empty()) )
		return;

	tstring strGUID = GenerateGuid();

	TagsPtr pTags = pPresenation->GetTags();
	pTags->Add(PPT_GUID, strGUID.c_str());
	CPPTController::UpdatePresenationGuid(strGUID);

	m_PPTHwnd	= (HWND)AppPtr->GetHWND();
	CPPTController::UpdatePPTHwnd(m_PPTHwnd);
	AttatchPPTWindow(m_PPTHwnd, pMain);

	if ( OPE_POWERPOINT == CPPTController::GetOperationPPTEvent() )
	{
		tstring strTitle = pPresenation->GetFullName();

		CStream* pStream = new CStream(256);
		pStream->WriteDWORD(NEW_FILE);
		pStream->WriteString(strGUID);
		pStream->WriteString(strTitle);

		BroadcastPostedEvent(EVT_PPT_FILE_OPERATION, CHANGE_FILE, (LPARAM)pStream, 0);
	}
	}
	catch (...)
	{
		WRITE_LOG_DEBUG("WindowActivate error");
	}

	//pPresenation->PutSaved(Office::msoTrue);

	//m_bNeedCheck = false;
}

void CPPTEventHandler::WindowDeactivate( LPDISPATCH Pres, LPDISPATCH Wn )
{
	OutputDebugString(_T("WindowDeactivate\r\n"));
}

void CPPTEventHandler::SlideShowBegin( LPDISPATCH Wn )
{
	WRITE_LOG_LOCAL(_T("-----SlideShowBegin\r\n"));

	try
	{
	// get current index
	SlideShowWindowPtr pShowWindow = (SlideShowWindowPtr)Wn;
	if( pShowWindow == NULL )
		return;

	HWND hwScreen = (HWND)pShowWindow->GetHWND();
	CPPTController::UpdateSlideShowViewHwnd(hwScreen);

	_PresentationPtr pPresenation = pShowWindow->GetPresentation();
	if( pPresenation == NULL )
		return;

	SlidesPtr	pSlides = pPresenation->GetSlides();
	if ( pSlides == NULL)
		return;

	int nCount = pSlides->GetCount();

	CPPTController::SetShowViewCount(nCount);

	CPPTControllerManager::GetInstance()->m_bCancelInsert = TRUE;
	CToast::SetShow(FALSE);

	m_bInSidePPTPlay		= FALSE;
	m_strPPTTitleName		= pPresenation->GetName();

	////2010以上"阅读视图"问题处理
	//if ( CPPTController::GetPPTVersion() >= PPTVER_2010 )
	//{
	//	TCHAR szClass[16] = {0};//mdiClass
	//	GetClassName(hwScreen, szClass, sizeof(szClass));
	//	if( _tcsicmp(szClass, _T("mdiClass")) == 0 )
	//		return;
	//}	

	if ( !pShowWindow->GetIsFullScreen() )
		return;

	CMainFrame* pMain = (CMainFrame* )AfxGetApp()->m_pMainWnd;

	//test
	pMain->PostMessage(WM_COMMAND, COMMAND_ID_PLAY, nCount);

	//m_bNeedCheck = false;
	}
	catch (...)
	{
		WRITE_LOG_DEBUG("SlideShowBegin error");
	}
}

void CPPTEventHandler::SlideShowNextBuild( LPDISPATCH Wn )
{
	OutputDebugString(_T("SlideShowNextBuild\r\n"));
}

void CPPTEventHandler::SlideShowNextSlide( LPDISPATCH Wn )
{
	try
	{
	// get current index
	SlideShowWindowPtr pShowWindow = (SlideShowWindowPtr)Wn;
	if( pShowWindow == NULL )
		return;

	SlideShowViewPtr pShowView = pShowWindow->GetView();
	if( pShowView == NULL )
		return;

	PpSlideShowState pState = pShowView->GetState();
	if( (pState != ppSlideShowRunning) && (pState != ppSlideShowPaused) )
		return;

	_SlidePtr pSlide = pShowView->GetSlide();
	if( pSlide == NULL )
		return;
	
	int nIndex = pSlide->GetSlideIndex();
	CPPTController::SetShowViewIndex(nIndex);

	//解决打开内部PPT时全屏工具上总页数不对的问题
	_ApplicationPtr pApp = pSlide->GetApplication();
	if ( pApp == NULL )
		return;

	_PresentationPtr pPresentation = pApp->GetActivePresentation();
	if ( pPresentation == NULL )
		return;

	if ( m_strPPTTitleName.empty() )
		m_strPPTTitleName = pPresentation->GetName();

	tstring strName = pPresentation->GetName();
	tstring strPath = pPresentation->GetPath();

	SlidesPtr pSlides = pPresentation->GetSlides();
	if ( pSlides == NULL )
		return;

	int nCount = pSlides->GetCount();
	CPPTController::SetShowViewCount(nCount);

	if ( !m_bInSidePPTPlay && strPath.empty() && strName.compare(m_strPPTTitleName) != 0 )
	{
		::PostMessage(CPPTController::m_MainFrmHwnd, WM_COMMAND, COMMAND_ID_INSIDE_PPT_ENTER, nCount);
		m_bInSidePPTPlay = TRUE;	
	}
	else if ( m_bInSidePPTPlay && ( !strPath.empty() || (strPath.empty() && strName.compare(m_strPPTTitleName) == 0 ) ) )
	{
		::PostMessage(CPPTController::m_MainFrmHwnd, WM_COMMAND, COMMAND_ID_INSIDE_PPT_LEAVE, nCount);
		m_bInSidePPTPlay = FALSE;
	}

	DWORD dwData = MAKELPARAM(nIndex, nCount);

		tstring str3DPath = "";
		try
		{
	tstring str3DPath = Get3DPath( (LPDISPATCH)pSlide );
	SetFocuesCef( (LPDISPATCH)pSlide );
		}
		catch (...)
		{
		}

	CStream* pStream = new CStream(256);
	pStream->WriteDWORD(dwData);
	pStream->WriteString(str3DPath);
	pStream->ResetCursor();

	//BroadcastPostedEvent(EVT_PPT_FILE_OPERATION, CHANGE_FILE, (LPARAM)pStream, 0);

	::PostMessage(CPPTController::m_MainFrmHwnd, WM_COMMAND, COMMAND_ID_NEXT, (LPARAM) pStream);

	}
	catch (...)
	{
		WRITE_LOG_DEBUG("SlideShowNextSlide error");
	}
}

tstring CPPTEventHandler::Get3DPath( LPDISPATCH pSlide)
{
	tstring strPath = _T("");
	
	ShapePtr pShape	= NULL;
	
	ShapesPtr pShapes = ((_SlidePtr)pSlide)->GetShapes();

	if ( pShapes == NULL )
		return strPath;

	for ( int i=1; i<=pShapes->GetCount(); i++)
	{
		pShape = pShapes->Item(_variant_t(long(i)));
		if ( pShape == NULL )
			continue;

		int nType = pShape->GetType();
		if ( nType == Office::msoPicture || nType == Office::msoPlaceholder )
		{
			tstring strFilePath = pShape->GetAlternativeText();
			tstring strSign = strFilePath.substr(0, _tcslen(PLUGIN_3D));
			if ( strSign.compare(PLUGIN_3D) == 0 )
			{
				strPath = strFilePath.substr(_tcslen(PLUGIN_3D));
				break;
			}
		}
	}
	
	return strPath;
}

tstring CPPTEventHandler::GetVRPath( LPDISPATCH pSlide)
{
	tstring strPath = _T("");

	ShapePtr pShape	= NULL;

	ShapesPtr pShapes = ((_SlidePtr)pSlide)->GetShapes();

	if ( pShapes == NULL )
		return strPath;

	for ( int i=1; i<=pShapes->GetCount(); i++)
	{
		pShape = pShapes->Item(_variant_t(long(i)));
		if ( pShape == NULL )
			continue;

		int nType = pShape->GetType();
		if ( nType == Office::msoPicture || nType == Office::msoPlaceholder )
		{
			tstring strFilePath = pShape->GetAlternativeText();
			tstring strSign = strFilePath.substr(0, _tcslen(PLUGIN_VR));
			if ( strSign.compare(PLUGIN_VR) == 0 )
			{
				strPath = strFilePath.substr(_tcslen(PLUGIN_VR));
				break;
			}
		}
	}

	return strPath;
}
tstring CPPTEventHandler::GetExercisesPath( LPDISPATCH pSlide )
{
	tstring strPath		= _T("");
	try
	{
	ShapePtr pShape		= NULL;
	ShapesPtr pShapes	= ((_SlidePtr)pSlide)->GetShapes();

	if ( pShapes == NULL )
		return strPath;

	for ( int i=1; i<=pShapes->GetCount(); i++)
	{
		pShape = pShapes->Item(_variant_t(long(i)));
		if ( pShape == NULL )
			continue;

		int nType = pShape->GetType();
		if( pShape->GetType() == Office::msoOLEControlObject )
		{
			_bstr_t strName = pShape->GetName();
			if( _tcsstr((TCHAR*)strName, _T("andcef")) == NULL )
				continue;

			OLEFormatPtr pOleFormat = pShape->GetOLEFormat();
			IDispatchPtr iDisPtr = pOleFormat->Object;

			CDandcef pp;
			pp.AttachDispatch(iDisPtr, FALSE);
			strPath = pp.GetUrl();
			pp.ReleaseDispatch();

			tstring strStart = _T("main=/");
			tstring strEnd	 = _T("sys=");

			int nStartPos	= strPath.find(strStart);
			int nEndPos		= strPath.rfind(strEnd);

			if ( nStartPos != tstring::npos && nEndPos != tstring::npos && nEndPos > nStartPos )
			{
				strPath = strPath.substr(nStartPos + strStart.length(), nEndPos - nStartPos - strStart.length() - 1);
				return replace_all_distinct(strPath, _T("/"), _T("\\"));
			}else
				return _T("");
		}
	}
	}
	catch (...)
	{
		return _T("");
	}
	
	return strPath;
}

BOOL CPPTEventHandler::SetFocuesCef( LPDISPATCH pSlide)
{
	//CPPTController* pController = GetPPTController();

	//BOOL isChange = FALSE;
	//if (pController){
	//	isChange = pController->IsChanged(false);
	//}
	ShapePtr pShape	= NULL;

	ShapesPtr pShapes = ((_SlidePtr)pSlide)->GetShapes();

	if ( pShapes == NULL )
		return FALSE;

	for ( int i=1; i<=pShapes->GetCount(); i++)
	{
		pShape = pShapes->Item(_variant_t(long(i)));
		if ( pShape == NULL )
			continue;

		int nType = pShape->GetType();
		if( pShape->GetType() == Office::msoOLEControlObject )
		{
			_bstr_t strName = pShape->GetName();
			if( _tcsstr((TCHAR*)strName, _T("andcef")) == NULL )
				continue;

			OLEFormatPtr pOleFormat = pShape->GetOLEFormat();
			IDispatchPtr iDisPtr = pOleFormat->Object;

			CDandcef pp;
			pp.AttachDispatch(iDisPtr, FALSE);

			pp.SetFocus(((_SlidePtr)pSlide)->GetSlideIndex());
			pp.ReleaseDispatch();
		
			return TRUE;
		}
	}
	HWND hWnd = FindWindow(_T("CEFOCX"), NULL);  //向CEFOCX的窗口发送页面变动消息
	if (IsWindow(hWnd)){
		if( g_Config::GetInstance()->GetModuleVisible(MODULE_ND_ICRPLAY) )
		{
			SendMessage(hWnd, MSG_CEFOCX_ICR, NULL, NULL);
		}
		SendMessage(hWnd, MSG_CEFOCX_PAGECHANGE, NULL, NULL);
	}
	//if (pController){
	//	if (!isChange){
	//		pController->SetSaved(TRUE);
	//	}
	//}
	return FALSE;
}

void CPPTEventHandler::SlideShowEnd( LPDISPATCH Pres )
{
	OutputDebugString(_T("SlideShowEnd\r\n"));
	KillExeCheckParentPidNotPlayer(_T("CoursePlayer.exe"), TRUE);//先把原先打开的关闭
	::SetForegroundWindow(CPPTController::GetSlideShowViewHwnd()); //解决部分电脑习题页面退出鼠标消失的问题
	CPPTControllerManager::GetInstance()->m_bCancelInsert = FALSE;
	CToast::SetShow(TRUE);
	CMainFrame* pMain = (CMainFrame* )AfxGetApp()->m_pMainWnd;
	pMain->PostMessage(WM_COMMAND, COMMAND_ID_STOP);
	
	m_bInSidePPTPlay		= FALSE;
	m_strPPTTitleName		= _T("");
	LaserPointerDlg::GetInstance()->Hide();
	CPPTController::UpdateSlideShowViewHwnd(NULL);
	//SetForegroundWindow(pMain->GetSafeHwnd());//最后一张是习题会丢失鼠标，目前暂时是点击完可以复现
}

void CPPTEventHandler::SlideShowOnNext( LPDISPATCH Wn )
{
	OutputDebugString(_T("SlideShowOnNext\r\n"));
}

void CPPTEventHandler::PresentationPrint( LPDISPATCH Pres )
{
	OutputDebugString(_T("PresentationPrint\r\n"));	
}

void CPPTEventHandler::SlideSelectionChanged( LPDISPATCH SldRange )
{
	try
	{
	SlideRangePtr pSlideRange = (SlideRangePtr)SldRange;
	if (pSlideRange == NULL)
		return;

	CMainFrame* pMain = (CMainFrame* )AfxGetApp()->m_pMainWnd;

	if (pSlideRange->GetCount() <= 0)
	{
		pMain->HideExercisesEditWindow();
			pMain->HideVRDistributeWindow();
		return;
	}

	_SlidePtr pSlide = pSlideRange->Item(_variant_t(long(1)));
	if (pSlide == NULL )
	{
		pMain->HideExercisesEditWindow();
			pMain->HideVRDistributeWindow();
		return;
	}

	int iSlideIdx = pSlide->GetSlideIndex();

		WRITE_LOG_LOCAL("CPPTEventHandler::SlideSelectionChanged, slideid:%d", iSlideIdx);
		BroadcastPostedEvent(EVT_PPT_INFO_SYNC, PPT_PREVIEW_CHANGE, iSlideIdx);
	if ( CPPTController::GetOperationPPTEvent() == OPE_POWERPOINT )
	{
			//习题
		tstring strPath = GetExercisesPath(pSlide);

		if ( !strPath.empty() )
			pMain->ShowExercisesEditWindow(strPath);
		else
			pMain->HideExercisesEditWindow();
			if ( m_bIcrPlayer )
			{
				strPath = GetVRPath(pSlide);

				if ( !strPath.empty() )
					pMain->ShowVRDistributeWindow(strPath);
				else
					pMain->HideVRDistributeWindow();
			}
		}
	}
	catch (...)
	{
		WRITE_LOG_DEBUG("SlideSelectionChanged error");
	}
}

void CPPTEventHandler::PresentationBeforeSave(LPDISPATCH Pres, VARIANT_BOOL * Cancel)
{
	//OutputDebugString(_T("PresentationBeforeSave\r\n"));
}


//=================================================================================
//========================== coustmer operation ===================================
//=================================================================================
void CPPTEventHandler::StartMainWindowMoniter(LPDISPATCH Pres)
{
	if( m_MoniterThread == NULL)
	{
		//_PresentationPtr pClosePresentation = Pres;
		//m_pApplication	= ((_PresentationPtr)Pres)->GetApplication();
		TagsPtr pTags	= ((_PresentationPtr)Pres)->GetTags(); // 附加一个线程中关闭的标志，过滤掉线程中的回调和通知
		tstring strInfo	= pTags->Item(_T("close"));
		if( _tcsicmp(strInfo.c_str(), _T("thread")) != 0 )
		{
			m_bNeedCheck	= true;
			m_MoniterThread = (HANDLE)_beginthreadex(NULL, 0, (unsigned(_stdcall *)(void *))MainWindowMoniterThread, this, 0, NULL);
		}
		
	}
}

void CPPTEventHandler::MainWindowMoniter()
{	
//#ifdef _DEBUG
//	return;
//#endif
	try
	{
		int nCount	= 0;
		m_bAppShow	= TRUE;
		ShowWindow(m_PPTHwnd, SW_SHOW);
		CMainFrame* pMain = (CMainFrame* )(AfxGetApp()->m_pMainWnd);

		while ( m_bNeedCheck )
		{
			Sleep(1000);

			if (!IsPPTActive())
				continue;

			BOOL bVisible = IsWindowVisible(m_PPTHwnd);
			if ( (!bVisible && !m_bNewPPT) || 
				(bVisible && !m_bNewPPT && (m_nPPTCount <= 0) && !CPPTController::isPlayMode() && pMain->IsMainFrmActive()
				&& GetPPTController()->GetPresentationCount() == 0) )
			{

				if ( m_bAppShow && nCount++ >= 3 )
				{
					m_nPPTCount = -1;
					ShowWindow(m_PPTHwnd, SW_HIDE);
					pMain->PostMessage(WM_COMMAND, COMMAND_INIT_UNTITLEWINDOW);
					m_bAppShow = false;
				}
			}
			else
			{
				nCount		= 0;
				m_bAppShow	= true;

				DealAbnormalMsgBox();
			}
		}
	}
	catch( ... )
	{
		OutputDebugString(_T("MainWindowMoniter Thread Error!"));
	}
}

//处理弹出的异常对话框
void CPPTEventHandler::DealAbnormalMsgBox()
{

	HWND hWnd = NULL;
	if ( CPPTController::GetPPTVersion() <= PPTVER_2007 )
		hWnd = ::FindWindow(NULL, _T("Microsoft Office PowerPoint"));
	else
		hWnd = ::FindWindow(NULL, _T("Microsoft PowerPoint"));

	if ( hWnd != NULL )
	{
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE);  
		SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE);  

		HWND hTipWnd = ::FindWindowEx(hWnd, NULL, _T("MSOUNISTAT"), NULL);
		if ( hTipWnd != NULL )
		{

			TCHAR szTip[MAX_PATH] = {0};
			::GetWindowText(hTipWnd, szTip, MAX_PATH);
			if ( _tcsstr(szTip, _T("此字体在您当前使用的设备上不可用，因此可能会使用另一种字体显示和/或打印")) != NULL
			|| _tcsstr(szTip, _T("是否仍使用该字体")) != NULL )
			{
				HWND hBtnWnd = ::FindWindowEx(hWnd, NULL, _T("Button"), _T("否(&N)"));
				if ( hBtnWnd != NULL )
				{
					::PostMessage(hBtnWnd, BM_CLICK, 0, 0);
				}
			}
		}
	}

	HWND hInsertWnd =  NULL;

	if ( CPPTController::GetPPTVersion() <= PPTVER_2007 )
		hInsertWnd = ::FindWindow(NULL, _T("插入声音"));
	else
		hInsertWnd = ::FindWindow(NULL, _T("插入音频"));

	if ( hInsertWnd != NULL )
	{
		SetWindowPos(hInsertWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE);  
		SetWindowPos(hInsertWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE);  
		Sleep(3000);
	}

	if ( CPPTController::GetPPTVersion() <= PPTVER_2007 )
		hInsertWnd = ::FindWindow(NULL, _T("插入影片"));
	else
		hInsertWnd = ::FindWindow(NULL, _T("插入视频文件"));

	if ( hInsertWnd != NULL )
	{
		SetWindowPos(hInsertWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE);  
		SetWindowPos(hInsertWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE);  
		Sleep(3000);
	}

	//播放时的警告对话框
	hWnd = ::FindWindow(NULL, _T("警告"));

	if ( hWnd != NULL )
	{
		HWND hCheckWnd = ::FindWindowEx(hWnd, NULL, _T("Button"), _T("我可以识别此内容。允许播放。"));
		if ( hCheckWnd != NULL )
		{
			::PostMessage(hCheckWnd, BM_CLICK, 0, 0);
			HWND hBtnWnd = ::FindWindowEx(hWnd, NULL, _T("Button"), _T("继续"));
			if ( hBtnWnd != NULL )
			{
				::PostMessage(hBtnWnd, BM_CLICK, 0, 0);
			}
		}
	}
}

BOOL CPPTEventHandler::IsPPTActive()
{
	if(m_PPTHwnd != NULL)
	{
		HWND hPop = GetLastActivePopup(m_PPTHwnd);
		if (hPop && m_PPTHwnd != hPop)
			return FALSE;
		else
			return TRUE;
	}
	return TRUE;
}

DWORD WINAPI CPPTEventHandler::MainWindowMoniterThread(LPVOID lparam)
{
	CPPTEventHandler *pThis = (CPPTEventHandler*)lparam;
	pThis->MainWindowMoniter();
	CloseHandle(pThis->m_MoniterThread);
	pThis->m_MoniterThread = NULL;
	_endthreadex(0);	
	return 0;
}

void CPPTEventHandler::ShowUntitledWindow()
{
	CMainFrame* pMain = (CMainFrame* )AfxGetApp()->m_pMainWnd;
	pMain->PostMessage(WM_COMMAND, COMMAND_INIT_UNTITLEWINDOW);

}

void CPPTEventHandler::TerminateMainMoniterThread()
{
	if ( m_MoniterThread != NULL )
	{
		TerminateThread(m_MoniterThread, 0);
		m_MoniterThread = NULL;
	}
}

tstring CPPTEventHandler::GenerateGuid()
{
	static int i = 1;
	TCHAR szGuid[MAX_PATH];

	_stprintf_s(szGuid, "PPT%d_%d", i++, GetTickCount());
	return szGuid;
}

void CPPTEventHandler::AttatchPPTWindow(HWND hPptWnd, CWnd* pParent)
{
	DWORD dwStyle = WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE;// | WS_CHILD;

	if ( CPPTController::GetPPTVersion() == PPTVER_2007 )
		dwStyle |= WS_CHILD;

	SetWindowLong(hPptWnd, GWL_STYLE, dwStyle);
	::SetParent(hPptWnd, pParent->GetSafeHwnd());//set parent of ms paint to our dialog

	CPanelInnerDlg::AdjustMainPanel();

	SetForegroundWindow(pParent->m_hWnd);
	PostMessage(hPptWnd, WM_NCACTIVATE, 1, 0); //让子窗口处于激活状态

	// disable system menu
	HMENU hMenu = GetSystemMenu(hPptWnd, FALSE);
	int nCount = GetMenuItemCount(hMenu);

	for(int i = nCount-2; i >= 0; i--)
		DeleteMenu(hMenu, i, MF_BYPOSITION);

	UINT nID = GetMenuItemID(hMenu, 0);
	EnableMenuItem(hMenu, nID, MF_DISABLED);

}

//============================================================
// recovery
//

DWORD WINAPI CPPTEventHandler::WaitRecoverySaveDialogThread( LPVOID lParam )
{
	CPPTEventHandler* pThis = (CPPTEventHandler*)lParam;
	pThis->WaitForRecoverySaveDialogClose();
	
	CloseHandle(pThis->m_hWaitRecoveryThread);
	pThis->m_hWaitRecoveryThread = NULL;

	return 1;
}

void CPPTEventHandler::WaitForRecoverySaveDialogClose()
{

	OutputDebugString(_T("WaitForRecoverySaveDialogClose() -- Start\r\n"));
	while ( m_bRecovery && !GetPPTController()->IsPPTActive() )
	{
		Sleep(500);
	}
	OutputDebugString(_T("WaitForRecoverySaveDialogClose() -- End\r\n"));
}

void CPPTEventHandler::AddPPTCount()
{
	if (m_nPPTCount < 0)
		m_nPPTCount = 1;
	else
		m_nPPTCount++;
}
