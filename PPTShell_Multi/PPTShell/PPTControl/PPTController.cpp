//===========================================================================
// FileName:				PPTController.cpp
// 
// Desc:				All operations to control PPT
//============================================================================
#include "stdafx.h"
#include "windows.h"
#include "PPTController.h"
#include "Util/Util.h"
#include "Util/Stream.h"
#include "Common.h"
#include "ActiveX/CShockwaveFlash.h"
#include "ActiveX/CWMPPlayer4.h"
#include "ActiveX/CWMPControls.h"
#include "ActiveX/CWMPSettings.h"
#include "ActiveX/CDandcef.h"
#include "PPTShell.h"
#include "GUI/PanelInnerDlg.h"
#include "PPTControl/PPTControllerManager.h"
#include <regex>
#include "PPTControl/PPTPack.h"
#include "ThirdParty/Tinyxml/tinyxml.h"
#include "NDCloud/NDCloudFile.h"
#include "DUI/AddSlideFloatWindow.h"

#define BEGIN_CATCH_COM_ERROR				try {

#define END_CATCH_COM_ERROR					} \
	catch(_com_error& e) \
{  \
	TCHAR szErrorInfo[MAX_PATH] = {0}; \
	TCHAR szDescription[MAX_PATH] = {0}; \
	\
	if( (LPCTSTR)e.ErrorMessage() )\
	_stprintf_s(szErrorInfo, _T("%s"),  (LPCTSTR)e.ErrorMessage());\
	\
	if( (LPCTSTR)e.Description() )\
	_stprintf_s(szDescription, _T("%s"), (LPCTSTR)e.Description());\
	\
	m_strComErrorInfo = szErrorInfo;  \
	\
	if( szDescription[0] != _T('\0') ) \
{\
	m_strComErrorInfo += _T(":"); \
	m_strComErrorInfo += szDescription; \
} \
	m_nOperationPpt		= OPE_POWERPOINT;\
}

#define END_CATCH_COM_ERROR_RET(res)		}\
	catch(_com_error& e) \
{  \
	TCHAR szErrorInfo[MAX_PATH] = {0}; \
	TCHAR szDescription[MAX_PATH] = {0}; \
	\
	if( (LPCTSTR)e.ErrorMessage() )\
	_stprintf_s(szErrorInfo, _T("%s"),  (LPCTSTR)e.ErrorMessage());\
	\
	if( (LPCTSTR)e.Description() )\
	_stprintf_s(szDescription, _T("%s"), (LPCTSTR)e.Description());\
	\
	m_strComErrorInfo = szErrorInfo;  \
	\
	if( szDescription[0] != _T('\0') ) \
{\
	m_strComErrorInfo += _T(":"); \
	m_strComErrorInfo += szDescription; \
	OutputDebugString(m_strComErrorInfo.c_str()); \
} \
	m_nOperationPpt		= OPE_POWERPOINT;\
	return res; \
}


static const GUID IID_IEApplication = //{ 0x57A320C, 0x1967, 0x48E5, { 0xAD, 0x3A, 0xBA, 0x80, 0xDB, 0xED, 0x5C, 0x53 } };
{0x914934C2,0x5A91,0x11CF, {0x87,0x00,0x00,0xAA,0x00,0x60,0x26,0x3b}};

BOOL CPPTController::m_bNewByPPTControl     = FALSE;
bool CPPTController::m_bPPTWndInit			= false;
bool CPPTController::m_bInitFailed			= false;
int CPPTController::m_nOperationPpt			= OPE_POWERPOINT;
BOOL CPPTController::m_bIsReadOnly			= FALSE;
ShapePtr CPPTController::m_pMediaShape		= NULL;
HWND CPPTController::m_PPTSlideShowHwnd		= NULL;
HWND CPPTController::m_PPTHwnd				= NULL;
HWND CPPTController::m_MainFrmHwnd			= NULL;
tstring CPPTController::m_strNewPreGuid		= _T("");
int CPPTController::m_nShowViewSlideCurIndex= -1;
int CPPTController::m_nShowViewSlideCount	= -1;
float CPPTController::m_fPPTVersion			= 0;
CResGuard CPPTController::m_Lock;
bool CPPTController::m_bIsPPT64				= false;
bool CPPTController::m_bEverTryRepair		= false;

map<HWND, BOOL> CPPTController::m_mapHwnd;

CPPTController::CPPTController()
{
	CoInitializeEx(NULL,COINIT_MULTITHREADED|COINIT_SPEED_OVER_MEMORY);

	//m_pPersentation			= NULL;
	m_dwPPTWndStyleEx		= 0;
	m_dwPPTWndStyle			= 0;
	m_pApplication			= NULL;
	m_bNeedUpdatePrenation	= FALSE;

	m_pEventHandler			= PPTEventHandler::GetInstance();

	initApp();
}

CPPTController::~CPPTController()
{

}

void CPPTController::initApp()
{
	//CResGuard::CGuard gd(m_Lock);

	if (!IsPPTActive())	
		return;

	BEGIN_CATCH_COM_ERROR

	//CheckPPTRunTime(); 不能在此处修复 会导致部分电脑 启动高概率失败

	HRESULT hr = m_pApplication.CreateInstance(__uuidof(MSPpt::Application));

	if( hr != S_OK )
	{
		m_pApplication	= NULL;
		m_bInitFailed	= true;
	}

	if(m_pApplication != NULL )
	{
		PresentationsPtr  pPresentations	= m_pApplication->GetPresentations();
		if ( pPresentations == NULL )
		{
			CToast::Toast(_STR_PPTCTRL_TIP_APP_EXCEPTION);
		}
		tstring  strVer = m_pApplication->GetVersion();
		m_fPPTVersion = (float)_tstof(strVer.c_str());

		if ( GetPPTVersion() <= PPTVER_2007 )
			m_pApplication->put_ShowWindowsInTaskbar(Office::msoFalse);

		//m_EventHandler.m_PPTHwnd = (HWND)m_pApplication->GetHWND();
		m_PPTHwnd = (HWND)m_pApplication->GetHWND();

		//m_pApplication->put_DisplayAlerts(ppAlertsNone);
	}

	END_CATCH_COM_ERROR
}

BOOL CPPTController::New(tstring& strName)
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

		if(m_pApplication == NULL)
			return FALSE;

	//m_bMyOperation	= true;
	m_nOperationPpt		= OPE_USER_NEW;
	PresentationsPtr pPresentations	= m_pApplication->GetPresentations();
	_PresentationPtr pPersentation	= pPresentations->Add(Office::msoTrue);

	SlidesPtr slidesPtr = pPersentation->GetSlides();
	if (slidesPtr && slidesPtr->GetCount() == 0)
		slidesPtr->Add(1, ppLayoutTitle);

	m_nOperationPpt		= OPE_POWERPOINT;

	strName = pPersentation->GetName();
	m_PPTHwnd = (HWND)m_pApplication->GetHWND();
	Sleep(100);
	pPersentation->PutSaved(Office::msoTrue);
	m_bNewByPPTControl = TRUE;

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE);

}

HANDLE g_hMonitoringMenuThread = NULL;

void MonitoringMenuThread(void* pObj)
{
	//Do not use this variable
	Sleep(500);
	CPPTController* pPPTCtrl = (CPPTController*)pObj;
	bool bResetChildWnd=false;
	while(TRUE)
	{
		HWND hFull		= ::FindWindowEx(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), NULL, _T("FullpageUIHost"), NULL);
		if (hFull)
		{

			CRect cRect;
			AfxGetApp()->m_pMainWnd->GetClientRect(&cRect);
			::MoveWindow(hFull, 0, -6, cRect.Width(), cRect.Height() + 6, FALSE);
			HWND hFrameClass= ::FindWindowEx(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), NULL, _T("PPTFrameClass"), NULL);
			if(hFrameClass)
			{
				if (::GetNextWindow(hFull, GW_HWNDNEXT) != hFrameClass)
				{
					::SetWindowPos(hFrameClass, hFull, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
					::SetForegroundWindow(hFull);

					HWND	hMain = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
					::PostMessage(hMain, WM_ERASEBKGND, 0, 0);
					::PostMessage(hMain, WM_NCPAINT, 0, 0);
				}
			}
		}else
		{
			//判断属性框，修复解决ppt属性丢失问题
			HWND hwPPT=CPPTController::GetPPTHwnd();
			if(hwPPT)
			{
				HWND hwFind=FindWindowEx(hwPPT, NULL, _T("MsoCommandBarDock"), _T("MsoDockTop"));
				if(hwFind)
				{
					hwFind=FindWindowEx(hwFind, NULL, _T("MsoCommandBar"), _T("属性编辑器"));
					if(hwFind)
					{
						if(bResetChildWnd==false)
						{				 
							DWORD dwStyle = GetWindowLong(hwPPT, GWL_STYLE);
							SetWindowLong(hwPPT, GWL_STYLE, dwStyle&~WS_CHILDWINDOW);
							bResetChildWnd=true;	
							SetForegroundWindow(hwPPT);
						}
					}else
					{
						if(bResetChildWnd)
						{
							DWORD dwStyle = GetWindowLong(hwPPT, GWL_STYLE);
							SetWindowLong(hwPPT, GWL_STYLE, dwStyle|WS_CHILDWINDOW);
							bResetChildWnd=false;
						}
					}
				}
			}
		}
		Sleep(1000);
	}
}
void CPPTController::AttatchPPTWindow(CWnd* pParent)
{
	if ( m_PPTHwnd == NULL )
		return;

	DWORD dwStyle = WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE;// | WS_CHILD;

	if ( CPPTController::GetPPTVersion() == PPTVER_2007 )
		dwStyle |= WS_CHILD;
	
	SetWindowLong(m_PPTHwnd, GWL_STYLE,  dwStyle); 
	Sleep(10);
	::SetParent(m_PPTHwnd, pParent->GetSafeHwnd());//set parent of ms paint to our dialog
	//::SetForegroundWindow(pParent->GetSafeHwnd());
	PostMessage(m_PPTHwnd, WM_NCACTIVATE, 1, 0); //让子窗口处于激活状态

	//adapt for full menu view
	if (GetPPTVersion() >= PPTVER_2010)
	{
		if (g_hMonitoringMenuThread)
		{
			TerminateThread(g_hMonitoringMenuThread, -1);
		}
		g_hMonitoringMenuThread = (HANDLE)_beginthread(MonitoringMenuThread, 0, this);
	}
}


//初始化和显示、关闭回收PowerPoint
BOOL CPPTController::ShowPPTWnd(CWnd* pParent)
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR
	
		BOOL bShow = TRUE;
		//初始化事件
		if(!m_bPPTWndInit)
		{
			m_PPTHwnd = (HWND)m_pApplication->GetHWND();

			Sleep(10);
			AttatchPPTWindow(pParent);
			m_pApplication->PutVisible(Office::msoTrue);//先设父，再可见，解决占位符右键菜单崩溃问题，标题栏高亮了

			m_MainFrmHwnd = pParent->m_hWnd;

			m_bPPTWndInit = true;

			CRect rt;
			GetClientRect(pParent->m_hWnd, &rt);
			pParent->PostMessage(WM_SIZE, 0, MAKELONG(rt.Width(),rt.Height()));
			Sleep(100);
			::SetWindowPos(pParent->GetSafeHwnd(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW| SWP_NOMOVE);
		}else
		{
			if(!IsWindow(m_PPTHwnd) || !IsWindowVisible(m_PPTHwnd))
			{			
				Sleep(100);
				m_pApplication->PutVisible(Office::msoTrue);
				m_PPTHwnd = (HWND)m_pApplication->GetHWND();
				AttatchPPTWindow(pParent);
			}
		}

		return bShow;

	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::ClosePPTWnd( )
{
	//CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR
		if ( m_bPPTWndInit )
		{
			PresentationsPtr pPresentations = m_pApplication->GetPresentations();
			int nCount = pPresentations->GetCount();

			BEGIN_CATCH_COM_ERROR
			while ( pPresentations->GetCount() > 0 )
			{
				_PresentationPtr pPre = m_pApplication->GetActivePresentation();
				if ( NULL != pPre )
				{
					pPre->Close();
				}
			}
			END_CATCH_COM_ERROR

			m_pEventHandler->TerminateMainMoniterThread();

			//KillExeCheckParentPid(PPT_EXE_NAME);
			DWORD dwSytle = ::GetWindowLong((HWND)m_pApplication->GetHWND(), GWL_STYLE);
			::SetWindowLong((HWND)m_pApplication->GetHWND(), GWL_STYLE, (dwSytle & ~WS_CHILD)); 
			::SetParent((HWND)m_pApplication->GetHWND(), 0);

			//m_pApplication->Quit();	
			//m_EventHandler.m_PPTHwnd
			m_pApplication	= NULL;
			m_bPPTWndInit	= FALSE;
		}

		return TRUE;
		END_CATCH_COM_ERROR_RET(FALSE)
}

//
// Get Presentation
//
_PresentationPtr CPPTController::GetPresentation(BOOL bBefore /*= FALSE*/)
{
	return GetCurrentPresentation();
	
	BEGIN_CATCH_COM_ERROR
	
	if ( m_pApplication == NULL)
		return NULL;

	PresentationsPtr pPresentations = m_pApplication->GetPresentations();
	if (pPresentations == NULL )
		return NULL;

	int nCount = pPresentations->GetCount();

	if ( nCount == 0 )
		return NULL;

	_PresentationPtr pPresentation = NULL;

	if ( nCount == 1 && !bBefore)
	{
		pPresentation = pPresentations->Item(_variant_t(long(1)));

		tstring strGuid = pPresentation->GetTags()->Item(PPT_GUID);

		if ( strGuid == PPT_REUSING_PPT )
			return NULL;
		else
			return pPresentation;
	}

	for (int i = 1; i <= nCount; i++)
	{
		pPresentation = pPresentations->Item(_variant_t(long(i)));
		tstring strGuid = pPresentation->GetTags()->Item(PPT_GUID);

		if ( strGuid == PPT_INSIDE_PPT  || strGuid == PPT_REUSING_PPT )
			continue;
		
		bool bEqual = (_tcsicmp(m_strNewPreGuid.c_str(), strGuid.c_str()) == 0);
		if ( (bEqual && !bBefore) || (!bEqual && bBefore) )
			return pPresentation;
	}

	return NULL;

	END_CATCH_COM_ERROR_RET(NULL)
}

_PresentationPtr CPPTController::GetCurrentPresentation()
{
	BEGIN_CATCH_COM_ERROR

		if ( m_pApplication == NULL)
			return NULL;

	PresentationsPtr pPresentations = m_pApplication->GetPresentations();
	if (pPresentations == NULL )
		return NULL;

	int nCount = pPresentations->GetCount();

	if ( nCount == 0 )
		return NULL;

	return m_pApplication->GetActivePresentation();

	END_CATCH_COM_ERROR_RET(NULL)
}


_PresentationPtr CPPTController::GetFrontPresentation()
{

	BEGIN_CATCH_COM_ERROR

	if ( m_pApplication == NULL)
		return NULL;

	PresentationsPtr pPresentations = m_pApplication->GetPresentations();
	if (pPresentations == NULL )
		return NULL;

	int nCount = pPresentations->GetCount();

	if ( nCount == 0 )
		return NULL;

	return pPresentations->Item(_variant_t(long(1)));

	END_CATCH_COM_ERROR_RET(NULL)
}



//
// Get slides
//
SlidesPtr CPPTController::GetSlides()
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

		if (!IsPPTActive())
		{
			return NULL;
		}

		_PresentationPtr pPresentation = GetCurrentPresentation();  //GetPresentation();
		if( pPresentation == NULL )
			return NULL;

		SlidesPtr pSlides = pPresentation->GetSlides();
		if( pSlides == NULL )
			return NULL;

		return pSlides;

		END_CATCH_COM_ERROR_RET(NULL)
}

//
// Get slide
//
_SlidePtr CPPTController::GetSlide(int nSlideIdx)
{
	BEGIN_CATCH_COM_ERROR

	if ( nSlideIdx <= 0 )
		return NULL;

	SlidesPtr pSlides = GetSlides();
	if( pSlides == NULL )
		return NULL;

	if( nSlideIdx > pSlides->GetCount() )
		return NULL;

	_SlidePtr pSlide = pSlides->Item(_variant_t((long)nSlideIdx));
	return pSlide;

	END_CATCH_COM_ERROR_RET(NULL)
}


_SlidePtr CPPTController::GetSlideById( int nSlideId )
{
	BEGIN_CATCH_COM_ERROR

	if ( nSlideId == 0 )
		return NULL;

	SlidesPtr pSlides = GetSlides();
	if( pSlides == NULL )
		return NULL;

	_SlidePtr pSlide = pSlides->FindBySlideID(nSlideId);

	return pSlide;

	END_CATCH_COM_ERROR_RET(NULL)
}

//
// Get selection
//
SelectionPtr CPPTController::GetSelection()
{	
	BEGIN_CATCH_COM_ERROR

		if( m_pApplication == NULL )
			return NULL;

	DocumentWindowPtr pDocWindow = m_pApplication->GetActiveWindow();
	if( pDocWindow == NULL )
		return NULL;

	return pDocWindow->GetSelection();

	END_CATCH_COM_ERROR_RET(NULL)
}

//
// Get slide show view
//
SlideShowViewPtr CPPTController::GetSlideShowView()
{
	BEGIN_CATCH_COM_ERROR

	if( !isPlayMode() )
		return NULL;

	_PresentationPtr pPresentation		= NULL;
	SlideShowWindowPtr pSlideShowWindow = NULL;

	try
	{
		pPresentation = GetCurrentPresentation();  //GetPresentation();
		if( pPresentation == NULL )
			return NULL;

		if( !isPlayMode() )
			return NULL;
		pSlideShowWindow = pPresentation->GetSlideShowWindow();
	}
	catch(...)
	{
		pPresentation = GetFrontPresentation();  //GetPresentation();
		if( pPresentation == NULL )
			return NULL;
		pSlideShowWindow = pPresentation->GetSlideShowWindow();
	}
	
	if( pSlideShowWindow == NULL )
		return NULL;

	SlideShowViewPtr pSlideShowView = pSlideShowWindow->GetView();
	if( pSlideShowView == NULL )
		return NULL;

 
	return pSlideShowView;

	END_CATCH_COM_ERROR_RET(NULL)

}

//
// Screen pixel to slide position
//
POINTF CPPTController::ScreenPixelToSlidePos(POINT pt)
{
	POINTF ptResult = {0.0f, 0.0f};

	BEGIN_CATCH_COM_ERROR

	// activate right pane
	//ActivatePane(2);

	if( m_pApplication == NULL )
		return ptResult;

	DocumentWindowPtr pDocWindow = m_pApplication->GetActiveWindow();
	if( pDocWindow == NULL )
		return ptResult;

	ViewPtr pView = pDocWindow->GetView();
	if( pView == NULL )
		return ptResult;

	float fDocWndLeft = pDocWindow->GetLeft();
	float fDocWndTop  = pDocWindow->GetTop();

	int nDocWndLeft	= pDocWindow->PointsToScreenPixelsX(fDocWndLeft);
	int nDocWndTop  = pDocWindow->PointsToScreenPixelsY(fDocWndTop);

	// scale
	int nZoom = pView->GetZoom();
	float scale = (float)nZoom / 100.0f;

	//
	float fleft = (float)(pt.x - nDocWndLeft) / scale;
	float ftop  = (float)(pt.y - nDocWndTop) / scale;

	fleft /= POUND_TO_PIXEL;
	ftop  /= POUND_TO_PIXEL;

	ptResult.x = fleft;
	ptResult.y = ftop;

	return ptResult;

	END_CATCH_COM_ERROR_RET(ptResult)

}

POINT CPPTController::SlidePosToScreenPixel( POINTF pt )
{
	CResGuard::CGuard gd(m_Lock);

	POINT ptResult = {0, 0};

	BEGIN_CATCH_COM_ERROR

		if( m_pApplication == NULL )
			return ptResult;

	DocumentWindowPtr pDocWindow = m_pApplication->GetActiveWindow();
	if( pDocWindow == NULL )
		return ptResult;

	ViewPtr pView = pDocWindow->GetView();
	if( pView == NULL )
		return ptResult;
	//float fDocWndLeft = pDocWindow->GetLeft();
	//float fDocWndTop  = pDocWindow->GetTop();

	int nDocWndLeft	= pDocWindow->PointsToScreenPixelsX(pt.x);
	int nDocWndTop  = pDocWindow->PointsToScreenPixelsY(pt.y);

	ptResult.x = nDocWndLeft;
	ptResult.y = nDocWndTop;

	return ptResult;

	END_CATCH_COM_ERROR_RET(ptResult)
}

void CPPTController::SlidePosToScreenPos(float &fWidth, float &fHeight)
{
	fWidth = fWidth<0?0:fWidth;
	fWidth = fWidth>1?1:fWidth;
	fHeight = fHeight<0?0:fHeight;
	fHeight = fHeight>1?1:fHeight;

	int nSlideWidth		= 0;
	int nSlideHeight		= 0;
	GetSlideSize(nSlideWidth, nSlideHeight);

	int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int nScreenHight = GetSystemMetrics(SM_CYSCREEN);
	
	float fPerW = nScreenWidth*1.0f/nSlideWidth;
	float fPerH = nScreenHight*1.0f/nSlideHeight;

	if (fPerW<fPerH)
	{
		int iActHeight = fPerW*nSlideHeight;
		fHeight = (nScreenHight-iActHeight)/2 + iActHeight*fHeight;
		fWidth = nScreenWidth*fWidth; 
	}
	else
	{
		int iActWidth = fPerH*nSlideWidth;
		fWidth = (nScreenWidth-iActWidth)/2 + iActWidth*fWidth;
		fHeight = nScreenHight*fHeight; 
	}
}

//
// Get active slide
//
_SlidePtr CPPTController::GetActiveSlider()
{
	BEGIN_CATCH_COM_ERROR

	if( m_pApplication == NULL )
		return NULL;

	DocumentWindowPtr pDocWnd = m_pApplication->GetActiveWindow();
	if( pDocWnd == NULL )
		return NULL;

	ViewPtr pView = pDocWnd->GetView();
	if( pView == NULL )
		return NULL;

	_SlidePtr pSlide = pView->GetSlide();

	return pSlide;

	END_CATCH_COM_ERROR_RET(NULL)
}

//
// Get active slide index
//
int CPPTController::GetActiveSlideIndex()
{
	BEGIN_CATCH_COM_ERROR

	_SlidePtr pSlide = GetActiveSlider();
	if( pSlide == NULL )
		return 0;

	int nIndex = pSlide->GetSlideIndex();

	return nIndex;

	END_CATCH_COM_ERROR_RET(0)
}


//
// Get slide width and height
//
void CPPTController::GetSlideSize(int& nWidth, int& nHeight)
{
	nWidth	= 0;
	nHeight = 0;

	BEGIN_CATCH_COM_ERROR

	if( m_pApplication == NULL )
		return;

	_PresentationPtr presentation = m_pApplication->GetActivePresentation();
	if( presentation == NULL )
		return;

	PageSetupPtr pPageSetup = presentation->GetPageSetup();
	if( pPageSetup == NULL )
		return;

	nWidth	= (int)pPageSetup->GetSlideWidth();
	nHeight = (int)pPageSetup->GetSlideHeight();

	END_CATCH_COM_ERROR
}

//------------------------------------------------------------------------------
// Invoked by user
//
int CPPTController::GetSlideCount()
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

		//if( m_pApplication == NULL )
		//	return 0;

	SlidesPtr pSlides = GetSlides();
	if( pSlides == NULL )
		return 0;

	int nCount = pSlides->GetCount();

	return nCount;

	END_CATCH_COM_ERROR_RET(0)
}

string CPPTController::GetPresentationName()
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

		if( m_pApplication == NULL )
			return "";

	_PresentationPtr pPresentation = m_pApplication->GetActivePresentation();
	if( pPresentation == NULL )
		return "";

	string strRet = pPresentation->GetName();
	return strRet;

	END_CATCH_COM_ERROR_RET("")
}


BOOL CPPTController::Open(LPCTSTR lpszPathName, BOOL bReadOnly)
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	if( m_pApplication == NULL )
		return FALSE;

	PresentationsPtr presentations = m_pApplication->GetPresentations();
	if( presentations == NULL )
		return FALSE;

	Office::MsoTriState msoReadOnly = bReadOnly ? Office::msoCTrue : Office::msoFalse;

	ShowWindow((HWND)m_pApplication->GetHWND(), SW_SHOW);
	CPPTEventHandler::m_bAppShow = TRUE;
	_PresentationPtr pPersentation = NULL;
	m_nOperationPpt		= OPE_USER_OPEN;
	Sleep(10);
	
	try
	{
		pPersentation = presentations->Open(lpszPathName, msoReadOnly, Office::msoFalse, Office::msoTrue);
	} 
	catch(_com_error& e) 
	{  
		//VARIANT_BOOL bEncrypt = pPersentation->GetPasswordEncryptionFileProperties();
		if ( MessageBox(m_MainFrmHwnd, _T("打开文件失败，是否尝试修复？【注意：修复后的文件将以只读模式打开】"), _T("101教育PPT"), MB_YESNO) == IDNO )
			return FALSE;

		pPersentation = presentations->Open2007(lpszPathName, msoReadOnly, Office::msoFalse, Office::msoCTrue, Office::msoCTrue);
	}

	Sleep(10);
	m_nOperationPpt		= OPE_POWERPOINT;
	
	m_PPTHwnd = (HWND)m_pApplication->GetHWND();

	return TRUE;

	END_CATCH_COM_ERROR

	//ShowWindow((HWND)m_pApplication->GetHWND(), SW_HIDE);
	//m_EventHandler.ShowUntitledWindow();
	m_nOperationPpt	= OPE_POWERPOINT;

	return FALSE;

}

BOOL CPPTController::Close(BOOL bBefore/* = TRUE*/)
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	_PresentationPtr pPresentation = GetPresentation(bBefore);

	if (  pPresentation != NULL )
		pPresentation->Close();

	HWND hWnd = (HWND)m_pApplication->GetHWND();

	if ( hWnd != NULL )
	{
		ShowWindow(hWnd, SW_SHOW|SW_NORMAL);
		m_pEventHandler->SetPPTHwnd(hWnd);
		SetForegroundWindow(hWnd);
	}

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

//
// update m_pPersentation
//
//void CPPTController::Update()
//{
//	CResGuard::CGuard gd(m_Lock);
//
//	BEGIN_CATCH_COM_ERROR
//
//	if( m_pApplication == NULL )
//		return;
//
//	m_pPersentation = m_pApplication->GetActivePresentation();
//	m_pMediaShape	= NULL;
//	m_bNeedUpdatePrenation = FALSE;
//
//	END_CATCH_COM_ERROR
//}


//
// Play PPT file
//
BOOL CPPTController::Play(BOOL bFromCurrentPage)
{
	//CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR
	if( m_pApplication == NULL )
		return FALSE;

	_PresentationPtr pPresentation = GetCurrentPresentation();  //GetPresentation();
	if( pPresentation == NULL )
		return FALSE;

	SlideShowSettingsPtr slideShowSetting = pPresentation->GetSlideShowSettings();
	if( slideShowSetting == NULL )
		return FALSE;

	Office::MsoTriState msoSaved = pPresentation->GetSaved();
	MSPpt::PpSlideShowType pShow = slideShowSetting->GetShowType();
	if ( pShow != ppShowTypeSpeaker )
	{
		slideShowSetting->PutShowType(ppShowTypeSpeaker);
		pPresentation->PutSaved(msoSaved);
	}
 
	if( bFromCurrentPage )
	{
		SlidesPtr pSlides = pPresentation->GetSlides();
		if( pSlides && pSlides->GetCount() )
		{
			HWND hdmain = AfxGetApp()->GetMainWnd()->GetSafeHwnd();

			if (::IsIconic(hdmain))
			{
				::ShowWindow(hdmain, SW_SHOWNORMAL);
				Sleep(200);
			}

			SetForegroundWindow(m_PPTHwnd);
			Sleep(100);

			keybd_event(VK_SHIFT, 0, 0, 0);
			keybd_event(VK_F5, 0, 0, 0);
			keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);	
			keybd_event(VK_F5, 0, KEYEVENTF_KEYUP, 0);		
		}
		else
			return FALSE;

	}
	else
	{
		//处理最终文档2007的问题
		if(GetPPTVersion() <= PPTVER_2007 && IsFinal())
		{
			SetForegroundWindow(m_PPTHwnd);
			Sleep(100);
			keybd_event(VK_F5, 0, 0, 0);
			keybd_event(VK_F5, 0, KEYEVENTF_KEYUP, 0);		
		}
		else//临时修复中文冲突ole异常导致播放阻塞问题
			if(GetPPTVersion()>=PPTVER_2013 && ((CPPTShellApp*)AfxGetApp())->m_bFixedIME)
			{
				SetForegroundWindow(m_PPTHwnd);
				Sleep(100);
				keybd_event(VK_F5, 0, 0, 0);
				keybd_event(VK_F5, 0, KEYEVENTF_KEYUP, 0);		
			}
			else
			{
				slideShowSetting->Run();
			}
	}

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)

}

// 
// Stop playing
//
BOOL CPPTController::Stop()
{
	//CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

		SlideShowViewPtr pSlideShowView = GetSlideShowView();
	if( pSlideShowView == NULL )
		return FALSE;

	//处理最终文档2007的问题
	if(GetPPTVersion() <= PPTVER_2007 && IsFinal())
	{
		PostMessage(m_PPTSlideShowHwnd, WM_CLOSE, 0, 0);
	}

	pSlideShowView->Exit();
	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}



BOOL CPPTController::IsChanged(BOOL bBefore /*= TRUE*/)
{
//	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	_PresentationPtr pPresentation = GetCurrentPresentation(); // GetPresentation(bBefore);

	if ( pPresentation == NULL )
		return FALSE;

	if ( pPresentation->GetFinal() == VARIANT_TRUE )
		return FALSE;

	if ( !pPresentation->GetSaved() )
		return TRUE;

	return FALSE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::IsReadOnly(BOOL bBefore /*= TRUE*/)
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	_PresentationPtr pPresentation = GetCurrentPresentation(); // GetPresentation(bBefore);
	if( pPresentation == NULL )
		return FALSE;

	if ( pPresentation->GetReadOnly() )
		return TRUE;

	return FALSE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

//
//
//
BOOL CPPTController::Save()
{
	//CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	_PresentationPtr pPresentation = GetCurrentPresentation(); // GetPresentation();

	if( pPresentation == NULL )
		return FALSE;

	//ClearGuidTagInfo(pPresentation);

	pPresentation->Save();
	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}


BOOL CPPTController::SaveAs( LPCTSTR szPath, BOOL bBefore /*= TRUE*/ )
{
	//CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	_PresentationPtr pPresentation = GetCurrentPresentation(); //GetPresentation(bBefore);

	if ( pPresentation == NULL )
		return FALSE;

	//ClearGuidTagInfo(pPresentation);

	pPresentation->SaveAs(szPath, ppSaveAsDefault, Office::msoTriStateMixed);

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::SaveAndClose(LPCTSTR szPath, BOOL bClose /* = TRUE*/ , BOOL bBefore/* = TRUE*/)
{
	BEGIN_CATCH_COM_ERROR

	_PresentationPtr pPresentation = GetPresentation(bBefore);

	if ( pPresentation == NULL )
		return FALSE;

	pPresentation->SaveAs(szPath, ppSaveAsDefault, Office::msoTriStateMixed);

	if ( bClose )
		pPresentation->Close();

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

void CPPTController::ClearGuidTagInfo(_PresentationPtr pPresentation)
{
	TagsPtr pTags = pPresentation->GetTags();

	if ( pTags == NULL )
		return;

	tstring strGuid = pTags->Item(PPT_GUID);

	if ( strGuid.empty() )
		return;

	VARIANT_BOOL bFinal = pPresentation->GetFinal();

	if ( bFinal )
		pPresentation->PutFinal(VARIANT_FALSE);

	pTags->Delete(PPT_GUID);

	if ( bFinal )
		pPresentation->PutFinal(VARIANT_TRUE);

}

BOOL CPPTController::SaveCopyAs(LPCTSTR szPath)
{
	//CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	_PresentationPtr pPresentation = GetPresentation();

	if( pPresentation == NULL )
		return FALSE;

	//ClearGuidTagInfo(pPresentation);

	pPresentation->SaveCopyAs(szPath, ppSaveAsPresentation, Office::msoFalse);
	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

//
// Previous slide
//
BOOL CPPTController::Prev()
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	SlideShowViewPtr pSlideShowView = GetSlideShowView();
	if( pSlideShowView == NULL )
		return FALSE;

	pSlideShowView->Previous();
	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

//
// Next slide
//
BOOL CPPTController::Next()
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	SlideShowViewPtr pSlideShowView = GetSlideShowView();
	if( pSlideShowView == NULL )
		return FALSE;
	pSlideShowView->Next();
	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

//
// Goto slide
//
BOOL CPPTController::Goto(int nSlideIdx)
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	SlideShowViewPtr pSlideShowView = GetSlideShowView();
	if( pSlideShowView == NULL )
		return FALSE;

	pSlideShowView->GotoSlide(nSlideIdx, Office::msoFalse);
	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::PresentationGoto(int iSlideIdx)
{
	
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	//BOOL isChange = IsChanged(FALSE);
	int iCurrSlideIdx = GetActiveSlideIndex();
	if (iCurrSlideIdx == iSlideIdx)
	{
		return TRUE;
	}

	SlidesPtr pSlides = GetSlides();
	if( pSlides == NULL )
		return FALSE;

	int iSlideCount = pSlides->GetCount();
	if (iSlideIdx<1 || iSlideIdx>iSlideCount)
	{
		return FALSE;
	}

	_SlidePtr pSlide = pSlides->Item(_variant_t((long)iSlideIdx));
	if( pSlide == NULL )
		return FALSE;
	
	pSlide->Select();

	//if (!isChange){
	//	SetSaved(TRUE);
	//}
	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

//
// Black screen / white screen 
//
BOOL CPPTController::SetSlideShowViewState(PpSlideShowState nState)
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	SlideShowViewPtr pSlideShowView = GetSlideShowView();
	if( pSlideShowView == NULL )
		return FALSE;

	pSlideShowView->PutState(nState);
	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

int	CPPTController::GetSlideShowViewState()
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	SlideShowViewPtr pSlideShowView = GetSlideShowView();
	if( pSlideShowView == NULL )
		return -1;

	int nState = pSlideShowView->GetState();
	return nState;

	END_CATCH_COM_ERROR_RET(-1)
}

//
// Set pointer type
//
BOOL CPPTController::SetPointerType(MSPpt::PpSlideShowPointerType nType)
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	SetForegroundWindow(m_PPTSlideShowHwnd);

	SlideShowViewPtr pSlideShowView = GetSlideShowView();
	if( pSlideShowView == NULL )
		return FALSE;

	pSlideShowView->PutPointerType(nType);
	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

//
// Get pointer type
//
PpSlideShowPointerType CPPTController::GetPointerType()
{
	BEGIN_CATCH_COM_ERROR
	
	SlideShowViewPtr pSlideShowView = GetSlideShowView();
	if( pSlideShowView == NULL )
		return ppSlideShowPointerNone;

	return pSlideShowView->GetPointerType();

	END_CATCH_COM_ERROR_RET(ppSlideShowPointerNone)
}

//
// Insert Text
//
BOOL CPPTController::InsertText(LPCTSTR szText, int nShapeIdx, int nSlideIdx /*=0*/ )
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	if(m_pApplication==NULL)
		return FALSE;

	//为0页时，新建一页
	SlidesPtr slidesPtr = GetSlides();
	_SlidePtr pSlide	= NULL;
	if (slidesPtr && slidesPtr->GetCount() == 0)
	{
		pSlide = slidesPtr->Add(1, ppLayoutTitle);
		pSlide->Select();
	}
	else
	{
		// insert to current active slide
		if ( nSlideIdx == 0 )
			nSlideIdx = GetActiveSlideIndex();

		if ( nSlideIdx == 0 )
			return PPTEC_SLIDE_NOT_SELECT;

		pSlide = slidesPtr->Item(_variant_t(long(nSlideIdx)));	
	}

	if( pSlide == NULL )
		return FALSE;

	// add text shape
	ShapesPtr pShapes = pSlide->GetShapes();
	ShapePtr pShape = NULL;

	int nSlideWidth;
	int nSlideHeight;

	GetSlideSize(nSlideWidth, nSlideHeight);

	if( nShapeIdx > pShapes->GetCount() )
		pShape = pShapes->AddShape(Office::msoShapeRectangle, 0, 0, nSlideWidth, nSlideHeight);
	else
		pShape = pShapes->Item(_variant_t((long)nShapeIdx));


	if( pShape == NULL )
		return FALSE;

	if ( pShape->GetType() == Office::msoTextEffect )
	{
		TextFramePtr textFrame = pShape->GetTextFrame();
		TextRangePtr textRange = textFrame->GetTextRange();

		textRange->PutText(szText);
	}
	else
	{
		FillFormatPtr pFillFormat = pShape->GetFill();
		MSPpt::ColorFormatPtr pColor = NULL;
		if(pFillFormat)
		{
			 pColor = pFillFormat->GetForeColor();
			 if(pColor)
				pColor->put___RGB(0xFFFFFFFF);
		}
 		TextFramePtr textFrame = pShape->GetTextFrame();
 		TextRangePtr textRange = textFrame->GetTextRange();
		MSPpt::FontPtr pFont = textRange->GetFont();
		if(pFont)
		{
			pFont->put_Size(50);
			
			pColor = pFont->GetColor();
			if(pColor)
				pColor->put___RGB(0xFF000000);
		}
		
 		textRange->PutText(szText);
		//pShapes->AddTextEffect(Office::msoTextEffect6, szText, "", 30, Office::msoTrue, Office::msoFalse, pShape->GetLeft(), pShape->GetTop());
	}

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}


//
// Insert picture
//


BOOL CPPTController::InsertPicture(LPCTSTR szPath, int& nSlideId, int& nPalceHolderId, int nLeft, int nTop, int nWidth, int nHeight, int nSlideIdx /*=0*/ )
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	//ppt是否可操作判断
	if (!IsPPTActive())	return FALSE;

	//为0页时，新建一页
	SlidesPtr slidesPtr = GetSlides();
	_SlidePtr pSlide	= NULL;
	if (slidesPtr && slidesPtr->GetCount() == 0)
	{
		pSlide = slidesPtr->Add(1, ppLayoutTitle);
		pSlide->Select();
	}
	else
	{
		// insert to current active slide
		if ( nSlideIdx == 0 )
			nSlideIdx = GetActiveSlideIndex();

		if ( nSlideIdx == 0 )
			return PPTEC_SLIDE_NOT_SELECT;

		pSlide = slidesPtr->Item(_variant_t(long(nSlideIdx)));	
	}

	if( pSlide == NULL )
		return FALSE;

	nSlideId = pSlide->GetSlideID();

	ShapesPtr pShapes = pSlide->GetShapes();
	if( pShapes == NULL )
		return FALSE;

	// slide size
	int nSlideWidth;
	int nSlideHeight;

	GetSlideSize(nSlideWidth, nSlideHeight);

	// calculate position 
	float fLeft		= 0.0f;
	float fTop		= 0.0f;
	float fWidth	= 0.0f;
	float fHeight	= 0.0f;

	if( nWidth == -1 )
		fWidth = (float)nSlideWidth / 2.0f;

	if( nHeight == -1 )
		fHeight = (float)nSlideHeight / 2.0f;

	if( nLeft != -1 && nTop != -1 )
	{
		POINT pt;
		pt.x = nLeft;
		pt.y = nTop;

		POINTF ptTemp = ScreenPixelToSlidePos(pt);
		fLeft = ptTemp.x;
		fTop = ptTemp.y;
	}

	float xDiff = 0.0f;
	float yDiff = 0.0f;

	if( nWidth != -1 && nHeight != -1 )
	{

		fWidth = (float)nWidth / POUND_TO_PIXEL;
		fHeight = (float)nHeight / POUND_TO_PIXEL;

		float fOriWidth = fWidth;
		float fOriHeight = fHeight;


		// too big
		float xscale = 1.0f;
		float yscale = 1.0f;

		if( fWidth > (float)nSlideWidth )
			xscale = (float)nSlideWidth / fWidth;

		if( fHeight > (float)nSlideHeight )
			yscale = (float)nSlideHeight / fHeight;

		float scale = xscale < yscale ? xscale : yscale;

		if( scale != 1.0f )
		{	
			scale *= 0.9f;
			fWidth *= scale;
			fHeight *= scale;

			xDiff = fOriWidth - fWidth;
			yDiff= fOriHeight - fHeight;
		}

	}

	if( nLeft == -1 )
	{
		if( nWidth != -1 )
			fLeft = ((float)nSlideWidth - fWidth) / 2.0f;
		else
			fLeft = (float)nSlideWidth / 4.0f;
	}
	else
	{
		fLeft -= fWidth / 2;
	}

	if( nTop == -1 )
	{
		if( nHeight != -1 )
			fTop = ( (float)nSlideHeight - fHeight) / 2.0f;
		else
			fTop = (float)nSlideHeight / 4.0f;
	}
	else
	{
		fTop -= fHeight / 2;
	}

	tstring strPicPath = CreateAdaptiveScreenPicture(szPath);

	ShapePtr pShape  = pShapes->AddPicture(strPicPath.c_str(), Office::msoFalse, Office::msoTrue, fLeft, fTop, fWidth, fHeight);
	
	if( pShape == NULL )
		return FALSE;

	nPalceHolderId = pShape->GetId();

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::InsertHyperLinkPicture( LPCTSTR szPath, int& nSlideId, int& nPalceHolderId, LPCTSTR szHyperlink,int nLeft /*= -1*/, int nTop /*= -1*/, int nWidth /*= -1*/, int nHeight /*= -1*/, int nSlideIdx /*= 0*/ )
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

		//ppt是否可操作判断
		if (!IsPPTActive())	return FALSE;

	//为0页时，新建一页
	SlidesPtr slidesPtr = GetSlides();
	_SlidePtr pSlide	= NULL;
	if (slidesPtr && slidesPtr->GetCount() == 0)
	{
		pSlide = slidesPtr->Add(1, ppLayoutTitle);
		pSlide->Select();
	}
	else
	{
		// insert to current active slide
		if ( nSlideIdx == 0 )
			nSlideIdx = GetActiveSlideIndex();

		if ( nSlideIdx == 0 )
			return PPTEC_SLIDE_NOT_SELECT;

		pSlide = slidesPtr->Item(_variant_t(long(nSlideIdx)));	
	}

	if( pSlide == NULL )
		return FALSE;

	nSlideId = pSlide->GetSlideID();

	ShapesPtr pShapes = pSlide->GetShapes();
	if( pShapes == NULL )
		return FALSE;

	// slide size
	int nSlideWidth;
	int nSlideHeight;

	GetSlideSize(nSlideWidth, nSlideHeight);

	// calculate position 
	float fLeft		= 0.0f;
	float fTop		= 0.0f;
	float fWidth	= 0.0f;
	float fHeight	= 0.0f;

	if( nWidth == -1 )
		fWidth = (float)nSlideWidth / 2.0f;

	if( nHeight == -1 )
		fHeight = (float)nSlideHeight / 2.0f;

	if( nLeft != -1 && nTop != -1 )
	{
		POINT pt;
		pt.x = nLeft;
		pt.y = nTop;

		POINTF ptTemp = ScreenPixelToSlidePos(pt);
		fLeft = ptTemp.x;
		fTop = ptTemp.y;
	}

	float xDiff = 0.0f;
	float yDiff = 0.0f;

	if( nWidth != -1 && nHeight != -1 )
	{

		fWidth = (float)nWidth / POUND_TO_PIXEL;
		fHeight = (float)nHeight / POUND_TO_PIXEL;

		float fOriWidth = fWidth;
		float fOriHeight = fHeight;


		// too big
		float xscale = 1.0f;
		float yscale = 1.0f;

		if( fWidth > (float)nSlideWidth )
			xscale = (float)nSlideWidth / fWidth;

		if( fHeight > (float)nSlideHeight )
			yscale = (float)nSlideHeight / fHeight;

		float scale = xscale < yscale ? xscale : yscale;

		if( scale != 1.0f )
		{	
			scale *= 0.9f;
			fWidth *= scale;
			fHeight *= scale;

			xDiff = fOriWidth - fWidth;
			yDiff= fOriHeight - fHeight;
		}

	}

	if( nLeft == -1 )
	{
		if( nWidth != -1 )
			fLeft = ((float)nSlideWidth - fWidth) / 2.0f;
		else
			fLeft = (float)nSlideWidth / 4.0f;
	}
	else
	{
		fLeft -= fWidth / 2;
	}

	if( nTop == -1 )
	{
		if( nHeight != -1 )
			fTop = ( (float)nSlideHeight - fHeight) / 2.0f;
		else
			fTop = (float)nSlideHeight / 4.0f;
	}
	else
	{
		fTop -= fHeight / 2;
	}

	tstring strPicPath = CreateAdaptiveScreenPicture(szPath);

	ShapePtr pShape  = pShapes->AddPicture(strPicPath.c_str(), Office::msoFalse, Office::msoTrue, fLeft, fTop, fWidth, fHeight);

	if( pShape == NULL )
		return FALSE;

	ActionSettingsPtr pActionSettings =  pShape->GetActionSettings();
	HyperlinkPtr pHyperlink = pActionSettings->Item(ppMouseClick)->GetHyperlink();
	pHyperlink->PutAddress(szHyperlink);

	nPalceHolderId = pShape->GetId();

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

//
// Insert Video
//
BOOL CPPTController::InsertVideo(LPCTSTR szPath, int nLeft, int nTop, int nWidth, int nHeight, int nSlideIdx /*=0*/)
{	
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	//ppt是否可操作判断
	if (!IsPPTActive())	return FALSE;

	//为0页时，新建一页
	SlidesPtr slidesPtr = GetSlides();
	_SlidePtr pSlide	= NULL;
	if (slidesPtr && slidesPtr->GetCount() == 0)
	{
		pSlide = slidesPtr->Add(1, ppLayoutTitle);
		pSlide->Select();
	}
	else
	{
		// insert to current active slide
		if ( nSlideIdx == 0 )
			nSlideIdx = GetActiveSlideIndex();

		if ( nSlideIdx == 0 )
			return PPTEC_SLIDE_NOT_SELECT;

		pSlide = slidesPtr->Item(_variant_t(long(nSlideIdx)));	
	}

	if( pSlide == NULL )
		return FALSE;

	ShapesPtr pShapes = pSlide->GetShapes();
	if( pShapes == NULL )
		return FALSE;

	// slide size
	int nSlideWidth;
	int nSlideHeight;

	GetSlideSize(nSlideWidth, nSlideHeight);

	// calculate position 
	float fLeft		= 0.0f;
	float fTop		= 0.0f;
	float fWidth	= 0.0f;
	float fHeight	= 0.0f;

	if( nWidth == -1 )
		fWidth = (float)nSlideWidth / 2.0f;

	if( nHeight == -1 )
		fHeight = (float)nSlideHeight / 2.0f;

	if( nLeft != -1 && nTop != -1 )
	{
		POINT pt;
		pt.x = nLeft;
		pt.y = nTop;

		POINTF ptTemp = ScreenPixelToSlidePos(pt);
		fLeft = ptTemp.x;
		fTop = ptTemp.y;
	}

	float xDiff = 0.0f;
	float yDiff = 0.0f;

	if( nWidth != -1 && nHeight != -1 )
	{

		fWidth = (float)nWidth / POUND_TO_PIXEL;
		fHeight = (float)nHeight / POUND_TO_PIXEL;

		float fOriWidth = fWidth;
		float fOriHeight = fHeight;


		// too big
		float xscale = 1.0f;
		float yscale = 1.0f;

		if( fWidth > (float)nSlideWidth )
			xscale = (float)nSlideWidth / fWidth;

		if( fHeight > (float)nSlideHeight )
			yscale = (float)nSlideHeight / fHeight;

		float scale = xscale < yscale ? xscale : yscale;

		if( scale != 1.0f )
		{	
			scale *= 0.9f;
			fWidth *= scale;
			fHeight *= scale;

			xDiff = fOriWidth - fWidth;
			yDiff= fOriHeight - fHeight;
		}

	}

	if( nLeft == -1 )
	{
		if( nWidth != -1 )
			fLeft = ((float)nSlideWidth - fWidth) / 2.0f;
		else
			fLeft = (float)nSlideWidth / 4.0f;
	}
	else
	{
		fLeft -= fWidth / 2;
	}

	if( nTop == -1 )
	{
		if( nHeight != -1 )
			fTop = ( (float)nSlideHeight - fHeight) / 2.0f;
		else
			fTop = (float)nSlideHeight / 4.0f;
	}
	else
	{
		fTop -= fHeight / 2;
	}

	// insert
	//ShapePtr pShape = pShapes->AddMediaObject2(szPath, Office::msoFalse, Office::msoTrue, fLeft, fTop, fWidth, fHeight);
	//if( pShape == NULL )
	//	return FALSE;

	ShapePtr pShape = pShapes->AddOLEObject(fLeft, fTop, fWidth, fHeight, _T("WMPlayer.OCX"), _T(""),
		Office::msoFalse, _T(""), 0, _T(""), Office::msoFalse);

	if ( pShape == NULL )
		return FALSE;

	OLEFormatPtr pOle = pShape->GetOLEFormat();
	IDispatchPtr iDisPtr=pOle->Object;

	CWMPPlayer4 pp;

	pp.AttachDispatch(iDisPtr, FALSE);
	pp.put_URL(szPath);
	pp.put_stretchToFit(TRUE);
 
	
	// don't auto start to play video
	CWMPSettings WmpSetting = (CWMPSettings)pp.get_settings();
	WmpSetting.put_autoStart(FALSE);

	pp.DetachDispatch();

	pShape->PutAlternativeText(PLUGIN_VIDEO);

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}



BOOL CPPTController::InsertCef(LPCTSTR szUrl, LPCTSTR szQuestionPath, int nLeft /* = -1 */, int nTop /* = -1 */, int nWidth /* = -1 */, int nHeight /* = -1 */, int nSlideIdx /* = 0 */)
{
	//CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	//ppt是否可操作判断
	if (!IsPPTActive())	
		return FALSE;

	//为0页时，新建一页
	SlidesPtr slidesPtr = GetSlides();

	m_nOperationPpt	= OPE_USER_INSERT;

	_SlidePtr pSlide;
	if (slidesPtr && slidesPtr->GetCount() == 0)
		pSlide = slidesPtr->Add(1, ppLayoutBlank);
	else
	{
		if ( nSlideIdx == 0 )
			nSlideIdx = GetActiveSlideIndex();

		if ( nSlideIdx == 0 )
			return PPTEC_SLIDE_NOT_SELECT;

		pSlide = slidesPtr->Add(++nSlideIdx, ppLayoutBlank);
	}

	m_nOperationPpt	= OPE_POWERPOINT;

	if( pSlide == NULL )
		return FALSE;

	pSlide->Select();

	//DelAllShape(pSlide);

	ShapesPtr pShapes = pSlide->GetShapes();
	if( pShapes == NULL )
		return FALSE;

	// slide size
	int nSlideWidth;
	int nSlideHeight;

	GetSlideSize(nSlideWidth, nSlideHeight);

	// calculate position 
	float fLeft		= 0.0f;
	float fTop		= 0.0f;
	float fWidth	= 0.0f;
	float fHeight	= 0.0f;

	if( nWidth == -1 )
		fWidth = (float)nSlideWidth / 2.0f;

	if( nHeight == -1 )
		fHeight = (float)nSlideHeight / 2.0f;

	if( nLeft != -1 && nTop != -1 )
	{
		POINT pt;
		pt.x = nLeft;
		pt.y = nTop;

		POINTF ptTemp = ScreenPixelToSlidePos(pt);
		fLeft = ptTemp.x;
		fTop = ptTemp.y;
	}

	float xDiff = 0.0f;
	float yDiff = 0.0f;

	if( nWidth != -1 && nHeight != -1 )
	{

		fWidth = (float)nWidth / POUND_TO_PIXEL;
		fHeight = (float)nHeight / POUND_TO_PIXEL;

		float fOriWidth = fWidth;
		float fOriHeight = fHeight;


		// too big
		float xscale = 1.0f;
		float yscale = 1.0f;

		if( fWidth > (float)nSlideWidth )
			xscale = (float)nSlideWidth / fWidth;

		if( fHeight > (float)nSlideHeight )
			yscale = (float)nSlideHeight / fHeight;

		float scale = xscale < yscale ? xscale : yscale;

		if( scale != 1.0f )
		{	
			scale *= 0.9f;
			fWidth *= scale;
			fHeight *= scale;

			xDiff = fOriWidth - fWidth;
			yDiff= fOriHeight - fHeight;
		}

	}

	if( nLeft == -1 )
	{
		if( nWidth != -1 )
			fLeft = ((float)nSlideWidth - fWidth) / 2.0f;
		else
			fLeft = (float)nSlideWidth / 4.0f;
	}
	else
	{
		fLeft -= fWidth / 2;
	}

	if( nTop == -1 )
	{
		if( nHeight != -1 )
			fTop = ( (float)nSlideHeight - fHeight) / 2.0f;
		else
			fTop = (float)nSlideHeight / 4.0f;
	}
	else
	{
		fTop -= fHeight / 2;
	}

	ShapePtr pShape = pShapes->AddOLEObject(0, 0, nSlideWidth, nSlideHeight, _T("ANDCEF.andcefCtrl.1"), _T(""),
		Office::msoFalse, _T(""), 0, _T(""), Office::msoFalse);

	if ( pShape == NULL )
		return FALSE;

	tstring strThumbnail = GenerateQuestionThumbnail(szQuestionPath);
	Sleep(50);

	OLEFormatPtr pOle = pShape->GetOLEFormat();
	IDispatchPtr iDisPtr=pOle->Object;

	CDandcef pp;
	pp.AttachDispatch(iDisPtr, FALSE);
	pp.SetUrl(szUrl);
	if (!strThumbnail.empty())
		pp.SetimgUrl(strThumbnail.c_str());
	pp.ReleaseDispatch();

	/*ShapePtr pImageShape;
	if( szThumbPath != NULL )
		pImageShape = pShapes->AddPicture(szThumbPath, Office::msoFalse, Office::msoTrue, 0.0f, 0.0f, nSlideWidth, nSlideHeight);
	else
	{
		pImageShape = pShapes->AddShape(Office::msoShapeRectangle, 0.0f, 0.0f, nSlideWidth, nSlideHeight);
		if( pImageShape == NULL )
			return FALSE;

		FillFormatPtr pFillFormat = pImageShape->GetFill();
		MSPpt::ColorFormatPtr pColor = NULL;
		if(pFillFormat)
		{
			pColor = pFillFormat->GetForeColor();
			if(pColor)
				pColor->put___RGB(0xFFFFFFFF);
		}

		TextFramePtr textFrame = pImageShape->GetTextFrame();
		TextRangePtr textRange = textFrame->GetTextRange();
		MSPpt::FontPtr pFont = textRange->GetFont();

		if(pFont)
		{
			pFont->put_Size(50);

			pColor = pFont->GetColor();
			if(pColor)
				pColor->put___RGB(0xFF000000);
		}

		textRange->PutText("习题");
	}*/

	return TRUE;
	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::UpdateCef( LPCTSTR szQuestionPath, LPCTSTR szGuid )
{
	//CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	//ppt是否可操作判断
	if (!IsPPTActive())
		return FALSE;

	_SlidePtr pSlide = GetActiveSlider();

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

			tstring strUrl = pp.GetUrl();

			if (_tcsstr(strUrl.c_str(), szGuid) != NULL )
			{
				tstring strThumbnail = GenerateQuestionThumbnail(szQuestionPath);
				Sleep(50);
				if (!strThumbnail.empty())
					pp.SetimgUrl(strThumbnail.c_str());
			}

			pp.DetachDispatch();
			return TRUE;
		}
	}

	return FALSE;
	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::Insert3D(LPCTSTR szPath, LPCTSTR szThumbPath, int nSlideIdx, LPCTSTR szSign /*= PLUGIN_3D*/)
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	//ppt是否可操作判断
	if (!IsPPTActive())	return FALSE;

	//为0页时，新建一页
	SlidesPtr slidesPtr = GetSlides();
	_SlidePtr pSlide = NULL;
	if (slidesPtr && slidesPtr->GetCount() == 0)
	{
		 pSlide = slidesPtr->Add(1, ppLayoutTitle);
	}
	else
	{
		// insert to current active slide
		if ( nSlideIdx == 0 )
			nSlideIdx = GetActiveSlideIndex();

		if ( nSlideIdx == 0 )
			return PPTEC_SLIDE_NOT_SELECT;

		pSlide = slidesPtr->Add(++nSlideIdx, ppLayoutTitle);	
	}

	if( pSlide == NULL )
		return FALSE;

	//DelAllShape(pSlide);

	pSlide->Select();

	ShapesPtr pShapes = pSlide->GetShapes();
	if( pShapes == NULL )
		return FALSE;

	// slide size
	int nSlideWidth;
	int nSlideHeight;

	GetSlideSize(nSlideWidth, nSlideHeight);

	ShapePtr pShape = pShapes->AddPicture(szThumbPath, Office::msoFalse, Office::msoTrue, 0, 0, nSlideWidth, nSlideHeight);

	if( pShape == NULL )
		return FALSE;

	tstring strInfo = szSign;
	strInfo += szPath;

	pShape->PutAlternativeText(strInfo.c_str());

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::InsertVRExe(LPCTSTR szPath,LPCTSTR szParam, LPCTSTR szThumbPath, int nSlideIdx, LPCTSTR szSign /*= PLUGIN_VR*/)
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

		//ppt是否可操作判断
		if (!IsPPTActive())	return FALSE;

	//为0页时，新建一页
	SlidesPtr slidesPtr = GetSlides();
	_SlidePtr pSlide = NULL;
	if (slidesPtr && slidesPtr->GetCount() == 0)
	{
		pSlide = slidesPtr->Add(1, ppLayoutTitle);
	}
	else
	{
		// insert to current active slide
		if ( nSlideIdx == 0 )
			nSlideIdx = GetActiveSlideIndex();

		if ( nSlideIdx == 0 )
			return PPTEC_SLIDE_NOT_SELECT;

		pSlide = slidesPtr->Add(++nSlideIdx, ppLayoutTitle);	
	}

	if( pSlide == NULL )
		return FALSE;

	//DelAllShape(pSlide);

	pSlide->Select();

	ShapesPtr pShapes = pSlide->GetShapes();
	if( pShapes == NULL )
		return FALSE;

	// slide size
	int nSlideWidth;
	int nSlideHeight;

	GetSlideSize(nSlideWidth, nSlideHeight);

	ShapePtr pShape = pShapes->AddPicture(szThumbPath, Office::msoFalse, Office::msoTrue, 0, 0, nSlideWidth, nSlideHeight);

	if( pShape == NULL )
		return FALSE;

	tstring strInfo = szSign;
	strInfo += szParam;

	pShape->PutAlternativeText(strInfo.c_str());

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}
BOOL CPPTController::InsertVRVideo(LPCTSTR szPath,LPCTSTR szParam, LPCTSTR szThumbPath, int nSlideIdx, LPCTSTR szSign /*= PLUGIN_VR*/)
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

		//ppt是否可操作判断
		if (!IsPPTActive())	return FALSE;

	//为0页时，新建一页
	SlidesPtr slidesPtr = GetSlides();
	_SlidePtr pSlide = NULL;
	if (slidesPtr && slidesPtr->GetCount() == 0)
	{
		pSlide = slidesPtr->Add(1, ppLayoutTitle);
	}
	else
	{
		// insert to current active slide
		if ( nSlideIdx == 0 )
			nSlideIdx = GetActiveSlideIndex();

		if ( nSlideIdx == 0 )
			return PPTEC_SLIDE_NOT_SELECT;

		pSlide = slidesPtr->Add(++nSlideIdx, ppLayoutTitle);	
	}

	if( pSlide == NULL )
		return FALSE;

	//DelAllShape(pSlide);

	pSlide->Select();

	ShapesPtr pShapes = pSlide->GetShapes();
	if( pShapes == NULL )
		return FALSE;

	// slide size
	int nSlideWidth;
	int nSlideHeight;

	GetSlideSize(nSlideWidth, nSlideHeight);

	/*
	ShapePtr pShape = pShapes->AddOLEObject( 0, 0, nSlideWidth, nSlideHeight, _T("WMPlayer.OCX"), _T(""),
		Office::msoFalse, _T(""), 0, _T(""), Office::msoFalse);

	if ( pShape == NULL )
		return FALSE;

	OLEFormatPtr pOle = pShape->GetOLEFormat();
	IDispatchPtr iDisPtr=pOle->Object;

	CWMPPlayer4 pp;

	pp.AttachDispatch(iDisPtr, FALSE);
	pp.put_URL(szPath);
	pp.put_stretchToFit(TRUE);
	pp.put_uiMode("NONE");//Invisible
	pp.put_fullScreen(TRUE);
	//pp.put_enableContextMenu(FALSE);

	//pShape->put_Visible(Office::msoFalse);

	HRESULT	hr = NULL;

	IConnectionPointContainer*	pConnPtContainer;
	IConnectionPoint*			pConnectionPoint;

	hr = pp.QueryInterface(IID_IConnectionPointContainer, (void**)&pConnPtContainer);
	ASSERT(!FAILED(hr));

	hr = pConnPtContainer->FindConnectionPoint(IID_IEApplication, &pConnectionPoint);
	ASSERT(!FAILED(hr));

	LPUNKNOWN pUnKown = m_pEventHandler->GetInterface(&IID_IUnknown);
	ASSERT(pUnKown);

	hr = pConnectionPoint->Advise(pUnKown, &m_pEventHandler->m_dwCookie);

	ASSERT(!FAILED(hr));

	pConnPtContainer->Release();
	*/
	// don't auto start to play video
	//CWMPSettings WmpSetting = (CWMPSettings)pp.get_settings();
	//WmpSetting.put_autoStart(FALSE);

	//pp.DetachDispatch();
	//pShape->PutAlternativeText(szParam);

	ShapePtr pPicShape = pShapes->AddPicture(szThumbPath, Office::msoFalse, Office::msoTrue, 0, 0, nSlideWidth, nSlideHeight);

	if( pPicShape == NULL )
		return FALSE;

	tstring strInfo = szSign;
	strInfo +=szParam;

	pPicShape->PutAlternativeText(strInfo.c_str());

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}
//
// Insert OLE object
//
BOOL CPPTController::InsertOLEObject(LPCTSTR szPath, int nLeft, int nTop, int nWidth, int nHeight, int nSlideIdx /*=0*/)
{	
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	//ppt是否可操作判断
	if (!IsPPTActive())	return FALSE;

	//为0页时，新建一页
	SlidesPtr slidesPtr = GetSlides();
	_SlidePtr pSlide	= NULL;
	if (slidesPtr && slidesPtr->GetCount() == 0)
	{
		pSlide = slidesPtr->Add(1, ppLayoutTitle);
		pSlide->Select();
	}
	else
	{
		// insert to current active slide
		if ( nSlideIdx == 0 )
			nSlideIdx = GetActiveSlideIndex();

		if ( nSlideIdx == 0 )
			return PPTEC_SLIDE_NOT_SELECT;

		pSlide = slidesPtr->Item(_variant_t(long(nSlideIdx)));	
	}

	if( pSlide == NULL )
		return FALSE;

	ShapesPtr pShapes = pSlide->GetShapes();
	if( pShapes == NULL )
		return FALSE;

	// slide size
	int nSlideWidth;
	int nSlideHeight;

	GetSlideSize(nSlideWidth, nSlideHeight);

	// calculate position 
	float fLeft		= 0.0f;
	float fTop		= 0.0f;
	float fWidth	= 0.0f;
	float fHeight	= 0.0f;

	if( nWidth == -1 )
		fWidth = (float)nSlideWidth / 2.0f;

	if( nHeight == -1 )
		fHeight = (float)nSlideHeight / 2.0f;

	if( nLeft != -1 && nTop != -1 )
	{
		POINT pt;
		pt.x = nLeft;
		pt.y = nTop;

		POINTF ptTemp = ScreenPixelToSlidePos(pt);
		fLeft = ptTemp.x;
		fTop = ptTemp.y;
	}

	float xDiff = 0.0f;
	float yDiff = 0.0f;

	if( nWidth != -1 && nHeight != -1 )
	{
		fWidth = (float)nWidth / POUND_TO_PIXEL;
		fHeight = (float)nHeight / POUND_TO_PIXEL;


		float fOriWidth = fWidth;
		float fOriHeight = fHeight;

		// too big
		float xscale = 1.0f;
		float yscale = 1.0f;

		if( fWidth > (float)nSlideWidth )
			xscale = (float)nSlideWidth / fWidth;

		if( fHeight > (float)nSlideHeight )
			yscale = (float)nSlideHeight / fHeight;

		float scale = xscale < yscale ? xscale : yscale;

		if( scale != 0.0f )
		{	
			scale *= 0.9f;
			fWidth *= scale;
			fHeight *= scale;

			xDiff = fOriWidth - fWidth;
			yDiff= fOriHeight - fHeight;
		}

	}

	if( nLeft == -1 )
	{
		if( nWidth != -1 )
			fLeft = ((float)nSlideWidth - fWidth) / 2.0f;
		else
			fLeft = (float)nSlideWidth / 4.0f;
	}
	else
	{
		fLeft -= fWidth / 2;
	}

	if( nTop == -1 )
	{
		if( nHeight != -1 )
			fTop = ( (float)nSlideHeight - fHeight) / 2.0f;
		else
			fTop = (float)nSlideHeight / 4.0f;
	}
	else
	{
		fTop -= fHeight / 2;
	}

	ShapePtr pShape = pShapes->AddOLEObject(fLeft, fTop, fWidth, fHeight, _T(""), szPath, Office::msoFalse, _T(""), 0, _T(""), Office::msoFalse);
	if( pShape == NULL )
		return FALSE;

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}


//
// Insert PPT
//
BOOL CPPTController::InsertPPT(LPCTSTR szPath, int nStartSlideIdx, int nEndSlideIdx, int nSlideIdx /*= 0*/, BOOL* pCancel /*= FALSE*/, NOTIFY_EVENT* pNotify /*= NULL*/)
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	//ppt是否可操作判断
	if ( !IsPPTActive() )	
	{	
		return FALSE;
	}

	SlidesPtr pSlides = GetSlides();
	if( pSlides == NULL )
		return FALSE;

	if ( nSlideIdx == 0 )
		nSlideIdx = GetActiveSlideIndex();

	if ( nSlideIdx == 0 )
		return PPTEC_SLIDE_NOT_SELECT;

	nSlideIdx++;

	int nTempSlideIdx = nSlideIdx;

	PresentationsPtr pSourcePres = m_pApplication->GetPresentations();
	if ( pSourcePres == NULL )
		return FALSE;

	m_nOperationPpt		= OPE_USER_INSERT;
	Sleep(10);
	_PresentationPtr pSourePre = NULL;

	pSourePre = pSourcePres->Open2007(szPath, Office::msoTrue, Office::msoFalse, Office::msoFalse, Office::msoCTrue);

	Sleep(10);
	m_nOperationPpt		= OPE_POWERPOINT;

	if ( pSourcePres == NULL )
		return FALSE;

	VARIANT_BOOL bFinal = pSourePre->GetFinal();

	if ( bFinal )  //fix insert final ppt result in error
		pSourePre->PutFinal(VARIANT_FALSE);

	if ( pSourePre == NULL )
		return FALSE;

	
	SlidesPtr pSourceSlides = pSourePre->GetSlides();

	if ( pSourceSlides == NULL )
	{
		pSourePre->Close();
		return FALSE;
	}


	int i = nStartSlideIdx;

	if ( nEndSlideIdx == -1 )
		nEndSlideIdx = pSourceSlides->GetCount();

	tstring strPPTFileName = GetFileName();

	TCHAR szBkPicPath[MAX_PATH] = {0};
	TCHAR szBkFolder[MAX_PATH] = {0};

	wsprintf(szBkFolder, _T("%s\\Cache\\Background"), GetLocalPath().c_str());

	// create directory if not exist
	CreateDirectory(szBkFolder, NULL);

	wsprintf(szBkPicPath, _T("%s\\bg.jpg"), szBkFolder);

	int nCount = 0;

	BOOL bInsertBk = TRUE;

	for ( i; i <= nEndSlideIdx; i++)
	{
		if ( pCancel != NULL &&  (TRUE == *pCancel) )
		{
			pSourePre->Close();
			return -1;
		}
		
		_SlidePtr pSourceSlide = pSourceSlides->Item(_variant_t(long(i)));
		if ( pSourceSlide == NULL)
			continue;

		pSourceSlide->Copy();

		int nSlideCount = pSlides->GetCount();
		if( nTempSlideIdx > nSlideCount+1 )
			nTempSlideIdx = nSlideCount;

		if( nTempSlideIdx == 0 )
			nTempSlideIdx = 1;

		SlideRangePtr pSlideRange = pSlides->Paste(nTempSlideIdx++);//m_pApplication->ActiveWindow->View->Paste();
		
		if( pSlideRange == NULL )
			continue;

		if ( bInsertBk )
		nCount++;

		bInsertBk = FALSE;

		if ( !ExportBackgroundToImages_check(szBkPicPath, _T("JPG"), pSourceSlide) )
			continue;

		InsertBackground(szBkPicPath, pSlideRange);

		bInsertBk = TRUE;
		nCount++;
	}

	pSourePre->Tags->Add(_T("close"),_T("thread"));
	pSourePre->Close();

	int nSlideCount = pSlides->GetCount();
	if( nSlideCount == 0 )
		return nCount;

	if( nSlideIdx > nSlideCount )
		nSlideIdx = nSlideCount;

	if( nSlideIdx == 0 )
		nSlideIdx = 1;


	_SlidePtr pSlide = pSlides->Item(_variant_t(long(nSlideIdx)));
	if ( pSlide != NULL )
		pSlide->Select();

	//if(pSelection!=NULL)
	//{
	//	SlideRangePtr pSlideRange = pSelection->GetSlideRange();
	//	if( pSlideRange == NULL )
	//		return TRUE;
	//	pSlideRange->MoveTo(nTempSlideIdx+1);
	//}

	return nCount;
	END_CATCH_COM_ERROR_RET(FALSE)
}

//
// Insert file link
//
BOOL CPPTController::InsertFileLink(LPCTSTR szFilePath, LPCTSTR szIconPath, LPCTSTR szTipInfo, int nLeft, int nTop, int nWidth, int nHeight, int nSlideIdx /*= 0*/)
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

		//ppt是否可操作判断
		if (!IsPPTActive())	return FALSE;

	//为0页时，新建一页
	SlidesPtr slidesPtr = GetSlides();
	_SlidePtr pSlide	= NULL;
	if (slidesPtr && slidesPtr->GetCount() == 0)
	{
		pSlide = slidesPtr->Add(1, ppLayoutTitle);
		pSlide->Select();
	}
	else
	{
		// insert to current active slide
		if ( nSlideIdx == 0 )
			nSlideIdx = GetActiveSlideIndex();

		if ( nSlideIdx == 0 )
			return PPTEC_SLIDE_NOT_SELECT;

		pSlide = slidesPtr->Item(_variant_t(long(nSlideIdx)));	
	}

	if( pSlide == NULL )
		return FALSE;

	ShapesPtr pShapes = pSlide->GetShapes();
	if( pShapes == NULL )
		return FALSE;

	// calculate position 
	POINT pt;
	pt.x = nLeft;
	pt.y = nTop;

	POINTF pos = ScreenPixelToSlidePos(pt);

	ShapePtr pShape = pShapes->AddPicture(szIconPath, Office::msoFalse, Office::msoTrue, (float)pos.x, (float)pos.y, (float)nWidth, (float)nHeight);
	if( pShape == NULL )
		return FALSE;

	// set hyper link action to this shape
	ActionSettingsPtr pActionSettings = pShape->GetActionSettings();
	if( pActionSettings == NULL )
		return FALSE;

	ActionSettingPtr pActionSetting = pActionSettings->Item(ppMouseClick);
	if( pActionSetting == NULL )
		return FALSE;

	pActionSetting->PutAction(ppActionHyperlink);

	HyperlinkPtr pHyperlink = pActionSetting->GetHyperlink();
	if( pHyperlink == NULL )
		return FALSE;

	pHyperlink->PutAddress(szFilePath);
	pHyperlink->PutScreenTip(szTipInfo);

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

//
// Insert background
//
BOOL CPPTController::InsertBackground(LPCTSTR szPath, BOOL bApplyAllSlides)
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR


		//ppt是否可操作判断
		if (!IsPPTActive())	return FALSE;

	if( bApplyAllSlides )
	{
		SlidesPtr pSlides = GetSlides();
		if( pSlides == NULL )
			return FALSE;

		for(int i= 1; i <= pSlides->GetCount(); i++)
		{
			SlideRangePtr pSlideRange = pSlides->Range(_variant_t((long)i));
			if( pSlideRange == NULL )
				continue;

			pSlideRange->PutFollowMasterBackground(Office::msoFalse);

			ShapeRangePtr pShapeRange = pSlideRange->GetBackground();
			if( pShapeRange == NULL )
				continue;

			FillFormatPtr pFillFormat = pShapeRange->GetFill();
			if( pFillFormat == NULL )
				continue;

			pFillFormat->UserPicture(szPath);

		}
	}
	else
	{
		SelectionPtr pSelection = GetSelection();
		if( pSelection == NULL )
			return FALSE;

		SlideRangePtr pSlideRange = pSelection->GetSlideRange();
		if( pSlideRange == NULL )
			return FALSE;

		pSlideRange->PutFollowMasterBackground(Office::msoFalse);

		ShapeRangePtr pShapeRange = pSlideRange->GetBackground();
		if( pShapeRange == NULL )
			return FALSE;

		FillFormatPtr pFillFormat = pShapeRange->GetFill();
		if( pFillFormat == NULL )
			return FALSE;

		pFillFormat->UserPicture(szPath);

	}

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::InsertBackground(LPCTSTR szPath, SlideRangePtr pSlideRange, bool bSetFollowMasterBG /*= true*/)
{
	BEGIN_CATCH_COM_ERROR

	//ppt是否可操作判断
	if (!IsPPTActive())
		return FALSE;

	if (bSetFollowMasterBG)
		pSlideRange->PutFollowMasterBackground(Office::msoFalse);

	ShapeRangePtr pShapeRange = pSlideRange->GetBackground();
	if( pShapeRange == NULL )
		return FALSE;

	FillFormatPtr pFillFormat = pShapeRange->GetFill();
	if( pFillFormat == NULL )
		return FALSE;

	pFillFormat->UserPicture(szPath);

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

//
//  Insert flash with special method
//
BOOL CPPTController::InsertSwf(LPCTSTR szPath, LPCTSTR szPicPath, LPCTSTR szTitle, LPCTSTR szSign, int nSlideIdx /*= 0*/)
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	//ppt是否可操作判断
	if (!IsPPTActive())	return FALSE;

	//为0页时，新建一页
	SlidesPtr slidesPtr = GetSlides();
	_SlidePtr pSlide	= NULL;
	if (slidesPtr && slidesPtr->GetCount() == 0)
	{
		pSlide = slidesPtr->Add(1, ppLayoutTitle);
		pSlide->Select();
	}
	else
	{
		// insert to current active slide
		if ( nSlideIdx == 0 )
			nSlideIdx = GetActiveSlideIndex();

		if ( nSlideIdx == 0 )
			return PPTEC_SLIDE_NOT_SELECT;

		pSlide = slidesPtr->Item(_variant_t(long(nSlideIdx)));	
	}

	if( pSlide == NULL )
		return FALSE;

	//策划支持不删除
	//DelAllShape(pSlide);

	ShapesPtr pShapes = pSlide->GetShapes();
	if( pShapes == NULL )
		return FALSE;

	// modify or add tiltle   部分文档的样式改标题会导致异常
	BEGIN_CATCH_COM_ERROR
	ShapePtr pShapeTitle = NULL;
	Office::MsoTriState msoTitle = pShapes->GetHasTitle();
	if ( msoTitle == Office::msoFalse )
	{
		PpSlideLayout pSLayout=pSlide->GetLayout();
		if( pSLayout!=ppLayoutBlank )
		{
			pShapeTitle = pShapes->AddTitle();
		}
	}
	else
	{
		pShapeTitle = pShapes->GetTitle();
	}

	if ( pShapeTitle != NULL )
	{
		TextFramePtr pTextFrame = pShapeTitle->GetTextFrame();
		if ( pTextFrame == NULL )
			return FALSE;

		TextRangePtr pTextRange = pTextFrame->GetTextRange();
		if ( pTextRange == NULL )
			return FALSE;

		if ( pTextRange->GetLength() == 0 )
			pTextRange->PutText(szTitle);
	}
	END_CATCH_COM_ERROR

	//special method

	ShapePtr pShape = NULL;
	//for ( int i = 1; i <= pShapes->GetCount(); i++)
	//{
	//	pShape = pShapes->Item(_variant_t(long(i)));

	//	if ( pShape == NULL)
	//		continue;

	//	Office::MsoShapeType msoType = pShape->GetType();

	//	if ( msoType == Office::msoShapeCan || msoType == Office::msoShapeCube )
	//	{
	//		CString strFilePath = pShape->GetAlternativeText();
	//		CString strSign		= strFilePath.Left(_tcslen(szSign));
	//	if (strSign.CompareNoCase(PLUGIN_SIGN) == 0 || strSign.CompareNoCase(PLUGIN_SIGN_HIDE) == 0)
	//		{
	//			pShape->Delete();
	//			break;
	//		}
	//	}
	//}

	// slide size
	int nSlideWidth;
	int nSlideHeight;

	GetSlideSize(nSlideWidth, nSlideHeight);

	pShape = pShapes->AddOLEObject(0, 0, nSlideWidth, nSlideHeight, _T("ShockwaveFlash.ShockwaveFlash"), _T(""),
		Office::msoFalse, _T(""), 0, _T(""), Office::msoFalse);

	if ( pShape == NULL )
		return FALSE;

	OLEFormatPtr pOle = pShape->GetOLEFormat();
	IDispatchPtr pObj = pOle->Object;
	CShockwaveFlash vFlash;

	vFlash.AttachDispatch(pObj, FALSE);
	vFlash.put_Movie(szPath);
	vFlash.put_Loop(FALSE);
	vFlash.put_EmbedMovie(TRUE);
	vFlash.put_FrameNum(0);
	vFlash.Play();

	vFlash.DetachDispatch();

	pShape->PutHeight(nSlideHeight+0.12);
	pShape->PutAlternativeText(PLUGIN_FLASH);


	/*With ActiveWindow.Selection.ShapeRange
	.Fill.Transparency = 0#
	.LockAspectRatio = msoFalse
	.Height = 540.12
	.Width = 720#
	.Left = 0#
	.Top = 0#
	End With*/

	//oleControl = pShapes.AddOLEObject(0, 0, nSlideWidth, nSlideHeight, _T("ShockwaveFlash.ShockwaveFlash"), _T(""),
	// Office::msoFalse, _T(""), 0, _T(""), Office::msoFalse).OLEFormat.Object;
	//Type oleControlType = oleControl.GetType();              /* 设置flash播放属性 */
	//oleControlType.InvokeMember("EmbedMovie", BindingFlags.SetProperty, null, oleControl, new object[] { true });
	//oleControlType.InvokeMember("Playing", BindingFlags.SetProperty, null, oleControl, new object[] { true });
	//oleControlType.InvokeMember("Movie", BindingFlags.SetProperty, null, oleControl, new object[] { fileName });// 设置Flash文件路径          
	//oleControlType.InvokeMember("Scale", BindingFlags.SetProperty, null, oleControl, new object[] { "ExactFit" });//设置显示比例为：严格匹配        
	//oleControlType.InvokeMember("ScaleMode", BindingFlags.SetProperty, null, oleControl, new object[] { 2 });

	//pShape = pShapes->AddPicture(szPicPath, Office::msoFalse, Office::msoTrue, 0, 0, nSlideWidth, nSlideHeight);

	//if ( pShape == NULL )
	//	return FALSE;

	//TCHAR szAltText[MAX_PATH*2] = {0};
	//wsprintf(szAltText, _T("%s%s"), szSign, szPath);
	//pShape->PutAlternativeText(szAltText);

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

//
// Export to images
//

//待优化内容
// 已保存的crc要判断

BOOL CPPTController::ExportToImages(const char* szFolderPath, const char* szExt, int nStartSlideIdx /*= -1*/, int nEndSlideIdx /*= -1*/, int nImageWidth /*= 0*/, int nImageHeight /*= 0*/, bool* pShouldCancel /*= NULL*/ )
{
	//BOOL isChange = IsChanged(FALSE);
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

		SlidesPtr pSlides = GetSlides();
	if( pSlides == NULL )
		return FALSE;

	// create directory if not exist
	CreateDirectory(szFolderPath, NULL);

	int nWidth = nImageWidth;
	int nHeight = nImageHeight;

	if(nWidth == 0 || nHeight == 0)
	{
		GetSlideSize(nWidth, nHeight);
		nWidth	= nWidth/2;
		nHeight = nHeight/2;
	}


	int nCount = pSlides->GetCount();

	//最终文档的导出处理
	bool bSetFinal=false;
	_PresentationPtr pPresentation=NULL;
	pPresentation = GetCurrentPresentation();
	if ( pPresentation == NULL )
		return FALSE;

	Office::MsoTriState stateBefore = pPresentation->GetSaved();

	if (GetPPTVersion()==PPTVER_2007)
	{
		VARIANT_BOOL bVariantBool = pPresentation->GetFinal();

		if(bVariantBool==VARIANT_TRUE)
		{
			pPresentation->PutFinal(VARIANT_FALSE);

			bSetFinal=true;
		}	
	}

	if( nStartSlideIdx == -1 || nEndSlideIdx == -1 )
	{
		// export all slides
		for(int i = 1; i <= nCount; i++)
		{
			if (pShouldCancel && *pShouldCancel)
			{
				break;
			}

			_SlidePtr pSlide = pSlides->Item(_variant_t((long)i));
			if( pSlide == NULL )
				return FALSE;

			char szFilePath[MAX_PATH];
			wsprintfA(szFilePath, "%s\\Slide_%d.%s", szFolderPath, i, szExt);

			pSlide->Export(szFilePath, szExt, nWidth, nHeight);
		}

	}
	else
	{
		// export indicated slides
		for(int i = nStartSlideIdx; i <= nEndSlideIdx; i++)
		{
			if (pShouldCancel && *pShouldCancel)
			{
				break;
			}

			_SlidePtr pSlide = pSlides->Item(_variant_t((long)i));
			if( pSlide == NULL )
				return FALSE;

			char szFilePath[MAX_PATH];
			wsprintfA(szFilePath, "%s\\Slide_%d.%s", szFolderPath, i, szExt);

			pSlide->Export(szFilePath, szExt, nWidth, nHeight);
		}
	}

	Office::MsoTriState statenow = pPresentation->GetSaved();
	if(stateBefore==Office::msoTrue && statenow!=stateBefore) 
	{
		if (GetPPTVersion()==PPTVER_2007)	
		{
			//2007版本office会触发PresentationSave，PresentationSave又会触发生成缩略图导致死循环，因此加这个tag
			pPresentation->Tags->Add(_T("save"),_T("ExportToImages"));
		}
		//2013版本不会，所以不用加，如果save状态因为生成缩略图而发生了变化，所有版本都要重置回来
		pPresentation->PutSaved(Office::msoTrue);
	}

	if(bSetFinal)
	{
		pPresentation->PutFinal(VARIANT_TRUE);
	}
	//if (!isChange){
	//	SetSaved(TRUE);
	//}
	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}


BOOL CPPTController::ExportToImages( char* szPPTPath, char* szFolderPath, char* szExt, bool* pShouldCancel /*= NULL*/, int* pCount/*= NULL*/, int nStartSlideIdx /*= -1*/, int nEndSlideIdx /*= -1*/, int nExistCount /*= 0*/, int nWidth /*= 0*/, int nHeight /*= 0*/ )
{
	CResGuard::CGuard gd(m_Lock);

	_PresentationPtr pSourePre = NULL;

	BEGIN_CATCH_COM_ERROR

	if (!IsPPTActive())	
		return FALSE;

	PresentationsPtr pSourcePres = m_pApplication->GetPresentations();
	if ( pSourcePres == NULL )
		return FALSE;

	m_nOperationPpt		= OPE_USER_EXPORT;
	Sleep(10);
	pSourePre = pSourcePres->Open(szPPTPath, Office::msoTrue, Office::msoFalse, Office::msoFalse);
	Sleep(10);
	m_nOperationPpt		= OPE_POWERPOINT;

	if (pSourePre == NULL)
		return FALSE;


	SlidesPtr pSlides = pSourePre->GetSlides();
	if( pSlides == NULL )
	{
		pSourePre->Close();
		return FALSE;
	}

	// create directory if not exist
	CreateDirectory(szFolderPath, NULL);

	int nImageWidth = nWidth;
	int nImageHeight = nHeight;

	if(nImageWidth == 0 || nImageHeight == 0)
		GetSlideSize(nImageWidth, nImageHeight);


	int nCount = pSlides->GetCount();
	if (pCount)
	{
		*pCount = nCount;
	}

	if (nCount == nExistCount)
	{
		pSourePre->Close();
		return TRUE;
	}

	if( nStartSlideIdx == -1 || nEndSlideIdx == -1 )
	{
		// export all slides
		for(int i = 1; i <= nCount; i++)
		{
			if (pShouldCancel && *pShouldCancel)
			{
				break;
			}

			_SlidePtr pSlide = pSlides->Item(_variant_t((long)i));
			if( pSlide == NULL )
			{
				pSourePre->Close();
				return FALSE;
			}

			char szFilePath[MAX_PATH];
			wsprintfA(szFilePath, "%s\\Slide_%d.%s", szFolderPath, i, szExt);

			pSlide->Export(szFilePath, szExt, nImageWidth, nImageHeight);
		}
	}
	else
	{
		// export indicated slides
		for(int i = nStartSlideIdx; i <= nEndSlideIdx; i++)
		{
			if (pShouldCancel && *pShouldCancel)
			{
				break;
			}

			_SlidePtr pSlide = pSlides->Item(_variant_t((long)i));
			if( pSlide == NULL )
			{
				pSourePre->Close();
				return FALSE;
			}

			char szFilePath[MAX_PATH];
			wsprintfA(szFilePath, "%s\\Slide_%d.%s", szFolderPath, i, szExt);

			pSlide->Export(szFilePath, szExt, nImageWidth, nImageHeight);
		}
	}

	pSourePre->Close();

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)

}

BOOL CPPTController::ExportBackgroundToImages(char* szFolderPath, char* szExt, int nIndex)
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	SlidesPtr pSlides = GetSlides();
	if( pSlides == NULL )
		return FALSE;

	// create directory if not exist
	CreateDirectory(szFolderPath, NULL);

	int nWidth = 0;
	int nHeight = 0;

	GetSlideSize(nWidth, nHeight);

	int nCount = pSlides->GetCount();

	if ( nIndex > nCount || nIndex < 1 )
		return FALSE;

	_SlidePtr pSlide = pSlides->Item(_variant_t((long)nIndex));
	if( pSlide == NULL )
		return FALSE;

	ShapeRangePtr pShape = pSlide->GetBackground();

	if ( pShape == NULL )
		return FALSE;

	char szFilePath[MAX_PATH];
	wsprintfA(szFilePath, "%s\\bg_%d.%s", szFolderPath, nIndex, szExt);

	pShape->Export(szFilePath, ppShapeFormatJPG, nWidth, nHeight, ppRelativeToSlide);

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::ExportBackgroundToImages_check(char* szFilePath, char* szExt, _SlidePtr pSlide)
{
	BEGIN_CATCH_COM_ERROR

	ShapeRangePtr pShapeRange = pSlide->GetBackground();

	if ( pShapeRange == NULL )
		return FALSE;

	ShapePtr pShape = pShapeRange->Item(_variant_t(long(1)));
	if ( pShape == NULL )
		return FALSE;

	FillFormatPtr pFillSource	= pShape->GetFill();
	if (pFillSource == NULL)
		return FALSE;
	
	Office::MsoFillType msoFillType = pFillSource->GetType();
	if(Office::msoFillMixed == msoFillType)
		return FALSE;

	if(Office::msoFillSolid == msoFillType)
	{
		MSPpt::ColorFormatPtr cfp = pFillSource->GetForeColor();
		Office::MsoRGBType rgb = cfp->Get__RGB();
		if ( rgb == 0x00FFFFFF )//排除白色，更好的是排除当前ppt前景色
		{
			tstring strTempName = pSlide->GetDesign()->GetName();
			if (strTempName==_T("Office 主题") || strTempName==_T("默认设计模板"))//特殊处理模板问题 
				return FALSE;
		}
	}

	BOOL bRet = DelAllShape(pSlide);
	if ( !bRet )
		return FALSE;

	pSlide->Export(szFilePath, szExt, 0, 0);

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}


BOOL CPPTController::ExportBackgroundToImages(char* szFilePath, char* szExt, _SlidePtr pSlide)
{

	BEGIN_CATCH_COM_ERROR

	ShapeRangePtr pShapeRange = pSlide->GetBackground();
	if ( pShapeRange == NULL )
		return FALSE;

	ShapePtr pShape = pShapeRange->Item(_variant_t(long(1)));
	if ( pShape == NULL )
		return FALSE;

	
	FillFormatPtr pFillSource	= pShape->GetFill();
	if (pFillSource == NULL)
		return FALSE;

	
	 
	if (pFillSource->GetForeColor()->GetType()==Office::msoColorTypeScheme)
	{
		int i=1;
	}

	Office::MsoFillType msoFillType = pFillSource->GetType();
	if(Office::msoFillMixed == msoFillType)
		return FALSE;



	if(Office::msoFillSolid == msoFillType)
	{
		MSPpt::ColorFormatPtr cfp=pFillSource->GetForeColor();
		Office::MsoRGBType rgb=cfp->Get__RGB();
		if (rgb==0x00FFFFFF)//排除白色，更好的是排除当前ppt前景色
			return FALSE;
	}

	BOOL bRet = DelAllShape(pSlide);
	if ( !bRet )
		return FALSE;

	pSlide->Export(szFilePath, szExt, 0, 0);

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::ExportBackgroundToImages(char* szPath, char* szFolderPath, char* szExt, int nIndex)
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

		if ( m_pApplication == NULL )
			return FALSE;

	PresentationsPtr pPres = m_pApplication->GetPresentations();

	if ( pPres == NULL)
		return FALSE;

	m_nOperationPpt		= OPE_USER_EXPORT;
	Sleep(10);
	_PresentationPtr pPre = pPres->Open(szPath, Office::msoFalse, Office::msoFalse, Office::msoFalse);
	Sleep(10);
	m_nOperationPpt		= OPE_POWERPOINT;

	if (pPre == NULL)
		return FALSE;

	SlidesPtr pSlides = pPre->GetSlides();

	if( pSlides == NULL )
	{
		pPre->Close();
		return FALSE;
	}

	// create directory if not exist
	CreateDirectory(szFolderPath, NULL);

	int nWidth = 0;
	int nHeight = 0;

	GetSlideSize(nWidth, nHeight);

	int nCount = pSlides->GetCount();

	if ( nIndex > nCount || nIndex < 1 )
	{
		pPre->Close();
		return FALSE;
	}

	_SlidePtr pSlide = pSlides->Item(_variant_t((long)nIndex));
	if( pSlide == NULL )
	{
		pPre->Close();
		return FALSE;
	}

	ShapeRangePtr pShape = pSlide->GetBackground();

	if ( pShape == NULL )
	{
		pPre->Close();
		return FALSE;
	}

	char szFilePath[MAX_PATH];
	wsprintfA(szFilePath, "%s\\bg_%d.%s", szFolderPath, nIndex, szExt);

	pShape->Export(szFilePath, ppShapeFormatPNG, nWidth, nHeight, ppRelativeToSlide);

	pPre->Tags->Add(_T("close"),_T("thread"));
	pPre->Close();

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

//
// Get outline
//
BOOL CPPTController::GetOutLine(vector<string> &vecOutLine,  string strPrefixIfNoExist /*= ""*/)
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	vecOutLine.clear();

	SlidesPtr pSlides = GetSlides();
	if( pSlides == NULL )
		return FALSE;

	int nCount = pSlides->GetCount();
	for(int i = 1; i <= nCount; i++)
	{
		string strTitle = "";

		_SlidePtr pSlide = pSlides->Item(_variant_t((long)i));
		if( pSlide == NULL )
			return FALSE;

		ShapesPtr pShapes = pSlide->GetShapes();
		if( pShapes != NULL )
		{
			ShapePtr pTitleShape = pShapes->GetTitle();
			if( pTitleShape != NULL )
			{
				TextFramePtr pTextFrame = pTitleShape->GetTextFrame();
				if( pTextFrame != NULL )
				{
					TextRangePtr pTextRange = pTextFrame->GetTextRange();
					if( pTextRange != NULL )
						strTitle = pTextRange->GetText();
				}
			}
		}

		// use default title
		if( strTitle == "" )
		{
			if( strPrefixIfNoExist != "" )
			{
				char szTitle[MAX_PATH];
				wsprintfA(szTitle, "%s %d", strPrefixIfNoExist.c_str(), i);
				vecOutLine.push_back(szTitle);
			}
		}
		else
			vecOutLine.push_back(strTitle);

	}

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::SetPenColor( DWORD dwColor )
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

		SetForegroundWindow(m_PPTSlideShowHwnd);

		SlideShowViewPtr	pView = GetSlideShowView();
		if ( pView == NULL )
			return FALSE;

		ColorFormatPtr pColorFormat	= pView->GetPointerColor();
		if ( pColorFormat == NULL )
			return FALSE;

		MSPpt::PpSlideShowPointerType pType = pView->GetPointerType();
		if ( pType == ppSlideShowPointerPen && GetPPTVersion() <= PPTVER_2007 )
		{
			PpSlideShowState nState = pView->GetState();

			if ( nState != ppSlideShowBlackScreen && nState != ppSlideShowWhiteScreen )
			{
			pColorFormat->Put__RGB(dwColor);
			SetForegroundWindow(m_PPTSlideShowHwnd);
			Sleep(50);
			//强制指定为画笔
			keybd_event(VK_CONTROL, 0, 0, 0);
			keybd_event('P', 0, 0, 0);
			keybd_event('P', 0, KEYEVENTF_KEYUP, 0);
			keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
			Sleep(100);
			pView->PutState(nState);
			}

			pView->PutPointerType(ppSlideShowPointerNone);
			pView->PutPointerType(ppSlideShowPointerPen);
		}
		else if ( GetPPTVersion() >= PPTVER_2010 )
		{
			pView->PutPointerType(ppSlideShowPointerNone);
			pView->PutPointerType(ppSlideShowPointerPen);
		}
		
		Sleep(10);
		pColorFormat->Put__RGB(dwColor);
		Sleep(10);
		pView->PutPointerType(ppSlideShowPointerPen);

		return TRUE;

		END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::PenDrawLine(float BeginX, float BeginY, float EndX, float EndY)
{

	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

		if( !IsPPTShowViewActive() )
		{
			return FALSE;
		}

		SlideShowViewPtr	pView = GetSlideShowView();
		if ( pView == NULL )
			return FALSE;

		pView->DrawLine(BeginX, BeginY, EndX, EndY);

		return TRUE;

		END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::PenEraseDrawing()
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

		if( !IsPPTShowViewActive() )
		{
			return FALSE;
		}

		SlideShowViewPtr	pView = GetSlideShowView();
		if ( pView == NULL )
			return FALSE;

		pView->EraseDrawing();

		return TRUE;

		END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::SetSaved( BOOL bSaved )
{
	//CResGuard::CGuard gd(m_Lock);
	BEGIN_CATCH_COM_ERROR

		if( m_pApplication == NULL )
			return NULL;

	_PresentationPtr pPerPtr = m_pApplication->GetActivePresentation();

	if ( pPerPtr == NULL )
		return FALSE;

	pPerPtr->PutSaved(bSaved ? Office::msoTrue : Office::msoFalse);

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::UnDoByCtrlZ()
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	//ppt是否可操作判断
	if (!IsPPTActive())	return FALSE;

	//先激活PPT
	DocumentWindowPtr pDocWindow = m_pApplication->GetActiveWindow();

	if (pDocWindow == NULL)
		return FALSE;

	pDocWindow->Activate();
	pDocWindow->GetPanes()->Item(2)->Activate();

	keybd_event(VK_CONTROL, 0, 0, 0);
	keybd_event('Z', 0, 0, 0);
	keybd_event('Z', 0, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);

	return TRUE;
	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::UnDo(int nCount)
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	//ppt是否可操作判断
	if (!IsPPTActive())	return FALSE;

	//先激活PPT
	DocumentWindowPtr pDocWindow = m_pApplication->GetActiveWindow();

	if (pDocWindow == NULL)
		return FALSE;

	

	//Sleep(10);
	
	//PostMessage((HWND)m_pApplication->GetHWND(), WM_UNDO, 0, 0);

	//pDocWindow->Activate();
	//pDocWindow->GetPanes()->Item(2)->Activate();

	//keybd_event(VK_CONTROL, 0, 0, 0);
	//keybd_event('Z', 0, 0, 0);
	//keybd_event('Z', 0, KEYEVENTF_KEYUP, 0);
	//keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);


	//CWnd* pView = (CWnd*)GetActiveView();
	//COleClientItem* pActiveItem = GetInPlaceActiveItem();
	//if( pActiveItem != NULL )
	//{
	//	_PresentationPtr pPresentation=	pActiveItem->m_lpObject;
	//	_ApplicationPtr app=pPresentation->GetApplication();
	//	Office::_CommandBarsPtr bar;
	//	bar =app->GetCommandBars();
	//	// Office::CommandBarControlPtr bt =bar->FindControl(vtMissing,128);
	//	Office::CommandBarControlPtr bt=pPresentation->CommandBars->FindControl(vtMissing,128);
	//	if(bt)
	//	{  
	//		BSTR p[255];
	//		bt->get_TooltipText(p);
	//		OutputDebugString((LPCSTR)p);
	//		if(bt->GetEnabled())
	//		{
	//			bt->raw_Execute();
	//			bt->PutOnAction(_("添加PPT"));
	//		}		
	//	}
	//}
	//m_pApplication->GetActiveWindow()->Activate();

	Office::_CommandBarsPtr pCommandBar;
	try 
	{
		pCommandBar = m_pApplication->GetCommandBars();
	}
	catch(_com_error& e)
	{
		int i = 0;
		for (i=0; i<nCount; i++)
		{
			UnDoByCtrlZ();
		}
		return TRUE;
	}
	

// 	for ( int i = 1; i < 400; i++)
// 	{
// 		Office::CommandBarControlPtr pCommandBarControl = pCommandBar->FindControl(vtMissing, i);
// 		if ( pCommandBarControl )
// 		{  
// 			tstring strTip = pCommandBarControl->GetTooltipText();
// 			TCHAR szNum[1024] = {0};
// 			wsprintf(szNum, _T("--%d %s\r\n"), i, strTip.c_str());
// 			OutputDebugString(szNum);
// 			WRITE_LOG_LOCAL("%s", szNum);
// 		}
// 	}


	//修复应用撤销问题
	int iWantUndo=0;
	if(nCount==1)
	{
		vector<tstring> vUndoList;
		GetUnDoList(vUndoList);
		vector<tstring>::iterator it=vUndoList.begin();
		bool bPosOne=true;	
		TCHAR szExpand2007[]=_T("展开");
		TCHAR szExpand2013[]=_T("展开??");
		TCHAR szDescript2[]=_T("最后一个");
		TCHAR szAspectRatio[]=_T("纵横比");
		TCHAR szModifySize[]=_T("调整对象大小");

		while(it!= vUndoList.end())
		{
			if(*it==szExpand2007 || *it==szExpand2013)
			{
				iWantUndo++;
			}
			else if(*it==szDescript2)
			{
				iWantUndo++;
				nCount = iWantUndo;
				break;
			}
			else
				break;

			it++;
		}
		/*
		if ( it != vUndoList.end() )
		{
			if ((++it != vUndoList.end()) && (*it == szAspectRatio || *it == szModifySize) )
				nCount++;
		}
		
		else 
		*/
		if (it == vUndoList.end() && iWantUndo>0)
		{
			nCount = iWantUndo;
			CToast::Toast(_T("部分操作无法撤销，建议修改PPT的 最多可取消操作数"), false, 3000);
		}
	}
	 
	Office::CommandBarControlPtr pCommandBarControl = pCommandBar->FindControl(vtMissing, 128);
	if ( pCommandBarControl )
	{
		for ( int i=0; i<nCount; i++)
		{
			// BSTR p;
			// pCommandBarControl->get_TooltipText(&p);
			// const char* ptrText = _com_util::ConvertBSTRToString(p); 
			// OutputDebugString(ptrText);
			// delete[] ptrText;

			if ( pCommandBarControl->GetEnabled() )
				pCommandBarControl->raw_Execute();
		}
		return TRUE;
	}

	return FALSE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::GetUnDoList(vector<tstring>& vecOperations)
{
	CResGuard::CGuard gd(m_Lock);

	vecOperations.clear();

	BEGIN_CATCH_COM_ERROR

	//ppt是否可操作判断
	if (!IsPPTActive())
		return FALSE;

	//先激活PPT
	DocumentWindowPtr pDocWindow = m_pApplication->GetActiveWindow();

	if (pDocWindow == NULL)
		return FALSE;

	pDocWindow->Activate();

	Office::_CommandBarsPtr pCommandBar;
	pCommandBar = m_pApplication->GetCommandBars();

	Office::CommandBarControlPtr pCommandBarControl = pCommandBar->FindControl(vtMissing, 128);

	if (pCommandBarControl)
	{

		int nChildCount = pCommandBarControl->GetaccChildCount();
		int i = 0;
		for (i=1; i<=nChildCount; i++)
		{
			_variant_t varIndex(i);
			_bstr_t strChildName = pCommandBarControl->GetaccName(varIndex);

			tstring strName(strChildName);
			vecOperations.push_back(strName);
		}
	}

	return TRUE;
	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::EnableNewInCommbar(BOOL bEnable)
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	//ppt是否可操作判断
	if (!IsPPTActive())	return FALSE;

	//先激活PPT
	DocumentWindowPtr pDocWindow = m_pApplication->GetActiveWindow();

	if (pDocWindow == NULL)
		return FALSE;

	Office::_CommandBarsPtr pCommandBar;
	pCommandBar = m_pApplication->GetCommandBars();
	Office::CommandBarControlPtr pCommandBarControl = pCommandBar->FindControl(vtMissing, 132);
	if ( pCommandBarControl )
	{  
		pCommandBarControl->PutEnabled(VARIANT_FALSE);
		return TRUE;
	}

	return FALSE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::InitEvent()
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	if ( m_pApplication == NULL )
		return FALSE;

	HRESULT	hr = NULL;

	IConnectionPointContainer*	pConnPtContainer;
	IConnectionPoint*			pConnectionPoint;

	hr = m_pApplication->QueryInterface(IID_IConnectionPointContainer, (void**)&pConnPtContainer);
	ASSERT(!FAILED(hr));

	if ( pConnPtContainer == NULL )
		return FALSE;

	hr = pConnPtContainer->FindConnectionPoint(IID_IEApplication, &pConnectionPoint);
	ASSERT(!FAILED(hr));

	if ( pConnectionPoint == NULL )
		return FALSE;

	LPUNKNOWN pUnKown = m_pEventHandler->GetInterface(&IID_IUnknown);
	ASSERT(pUnKown);

	hr = pConnectionPoint->Advise(pUnKown, &m_pEventHandler->m_dwCookie);

	ASSERT(!FAILED(hr));

	pConnPtContainer->Release();

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::ClearSliderInk(_SlidePtr pSlider)
{
	BEGIN_CATCH_COM_ERROR

		if( !IsPPTShowViewActive() )
		{
			return FALSE;
		}

		if ( pSlider == NULL )
			return FALSE;

		ShapesPtr	pShapes = pSlider->GetShapes();

		if ( pShapes == NULL )
			return TRUE;

		int nCount = pShapes->GetCount();

		for (int i = 1; i <= pShapes->GetCount();)
		{
			ShapePtr pShape = pShapes->Item(_variant_t(long(i)));
			if ( pShape!= NULL && pShape->Type == Office::msoShapeSun )
				pShape->Delete();
			else
				i++;
		}

		return TRUE;

		END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::DelAllShape(_SlidePtr pSlider)
{
	BEGIN_CATCH_COM_ERROR

		if ( pSlider == NULL )
			return FALSE;

	ShapesPtr	pShapes = pSlider->GetShapes();

	if ( pShapes == NULL )
		return TRUE;

	int nCount = pShapes->GetCount();

	for (int i = 1; i <= pShapes->GetCount();)
	{
		ShapePtr pShape = pShapes->Item(_variant_t(long(i)));
		if ( pShape!= NULL )
			pShape->Delete();
	}

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::DelShape(_SlidePtr pSlider, int nId)
{
	BEGIN_CATCH_COM_ERROR

	if ( pSlider == NULL )
		return FALSE;

	ShapesPtr	pShapes = pSlider->GetShapes();

	if ( pShapes == NULL )
		return TRUE;

	int nCount = pShapes->GetCount();

	for (int i = pShapes->GetCount(); i > 0; i--)
	{
		ShapePtr pShape = pShapes->Item(_variant_t(long(i)));

		if ( pShape != NULL && (pShape->GetId() == nId) )
		{
			pShape->Delete();
			return TRUE;
		}
	}

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::DelPalceHolder(int nSlideId, int nId)
{
	BEGIN_CATCH_COM_ERROR

	_SlidePtr pSlide = GetSlideById(nSlideId);

	if ( pSlide == NULL )
		return FALSE;

	return DelShape(pSlide, nId);

	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::ClearInk(BOOL bAll)
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

		if( !IsPPTShowViewActive() )
		{
			return FALSE;
		}


		if ( !bAll )
		{
			_SlidePtr pSlider = GetActiveSlider();
			return ClearSliderInk(pSlider);
		}
		else
		{
			SlidesPtr pSliders = GetSlides();

			for ( int i = 1; i <= pSliders->GetCount(); i++)
			{
				_SlidePtr pSlider = pSliders->Item(_variant_t(long(i)));
				ClearSliderInk(pSlider);
			}
			return TRUE;
		}

		END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::ClearShowViewInk(BOOL bAll)
{

	////zcs 11-07

	//BEGIN_CATCH_COM_ERROR

	//if (m_pApplication == NULL)
	//	return FALSE;

	//SlideShowWindowsPtr pSlideShowWindows = m_pApplication->GetSlideShowWindows();
	//if (pSlideShowWindows == NULL)
	//	return FALSE;

	//if ( bAll )
	//{
	//	for (int i= 1; i <= pSlideShowWindows->GetCount(); i++)
	//	{
	//		SlideShowWindowPtr pSlideWindow = pSlideShowWindows->Item(_variant_t((long)i));
	//		if (pSlideWindow == NULL)
	//			continue;
	//		pSlideWindow->Activate();
	//		SlideShowViewPtr	pSlideView = pSlideWindow->GetView();
	//		if (pSlideView == NULL)
	//			continue;
	//		pSlideView->EraseDrawing();
	//	}
	//}
	//else
	//{
	//	SlideShowViewPtr pSlideShowView = GetSlideShowView();
	//	if (pSlideShowView == NULL)
	//		return FALSE;
	//	int iIndex = pSlideShowView->GetSlide()->GetSlideIndex();
	//	
	//	SlideShowWindowPtr pSlideWindow = pSlideShowWindows->Item(_variant_t((long)iIndex));
	//	if (pSlideWindow == NULL)
	//		return FALSE;
	//	pSlideWindow->Activate();
	//	SlideShowViewPtr	pSlideView = pSlideWindow->GetView();
	//	if (pSlideView == NULL)
	//		return FALSE;

	//	pSlideView->EraseDrawing();
	//	
	//}

	//return TRUE;

	//END_CATCH_COM_ERROR_RET(FALSE)


	if(m_PPTSlideShowHwnd != NULL)
	{
		HWND hScreenWnd = FindWindowEx(m_PPTSlideShowHwnd, NULL, _T("paneClassDC"), NULL);

		if (hScreenWnd == NULL && GetVersion() >= PPTVER_2013 )
			hScreenWnd = FindWindowEx(NULL, NULL, _T("screenClass"), NULL);

		if(hScreenWnd != NULL)
		{
			
			//SetActiveWindow(hScreenWnd);
			SetForegroundWindow(hScreenWnd);
			Sleep(10);
			::PostMessage(hScreenWnd, WM_KEYDOWN, 'E', MAKELPARAM('E', WM_KEYDOWN));//e
			Sleep(10);
			::PostMessage(hScreenWnd, WM_KEYUP, 'E', MAKELPARAM('E', WM_KEYUP));
			//::PostMessage(hScreenWnd, WM_KEYDOWN, 0x45, 0x00120001);//e
			//Sleep(10);
			//::PostMessage(hScreenWnd, WM_KEYUP, 0x45, 0xC0120001);

		}else
		{
			
			//SetActiveWindow(m_PPTSlideShowHwnd);
			SetForegroundWindow(m_PPTSlideShowHwnd);
			Sleep(10);
			::PostMessage(m_PPTSlideShowHwnd, WM_KEYDOWN, 'E', MAKELPARAM('E', WM_KEYDOWN));//e
			Sleep(10);
			::PostMessage(m_PPTSlideShowHwnd, WM_KEYUP, 'E', MAKELPARAM('E', WM_KEYUP));
			//::PostMessage(m_PPTSlideShowHwnd, WM_KEYDOWN, 0x45, 0x00120001);//e
			//Sleep(10);
			//::PostMessage(m_PPTSlideShowHwnd, WM_KEYUP, 0x45, 0xC0120001);
		}
		return TRUE;
	}

	return FALSE;

	//END_CATCH_COM_ERROR_RET(FALSE)
	

		
		//消息清理，部分情况无效有动画的ppt
		//CResGuard::CGuard gd(m_Lock);
		//BEGIN_CATCH_COM_ERROR
		//	if( !IsPPTShowViewActive() )
		//	{
		//		return FALSE;
		//	}

		//	if ( !bAll )
		//	{
		//		SlideShowViewPtr ssViewPtr = GetSlideShowView();

		//		if (ssViewPtr == NULL)
		//			return FALSE;

		//		SlideShowWindowsPtr pSlideShowWindows = m_pApplication->GetSlideShowWindows();

		//		int iIndex=ssViewPtr->GetSlide()->GetSlideIndex();
		//		if(iIndex<=pSlideShowWindows->GetCount())
		//		{
		//			ssViewPtr->GotoSlide(iIndex, Office::msoFalse);
		//			SlideShowWindowPtr pSlideWindow = pSlideShowWindows->Item(iIndex);
		//			pSlideWindow->View->EraseDrawing();
		//			LPARAM lparam = MAKELPARAM(1, 1); //x坐标，y坐标
		//			m_pApplication->Activate();

		//			::PostMessage(m_PPTSlideShowHwnd, WM_MOUSEMOVE, MK_LBUTTON, lparam);
		//			//还需要激活一下

		//		}
		//		return TRUE;
		//	}
		//END_CATCH_COM_ERROR_RET(FALSE)
	
	
	
	
	//	//**注意， slide show和slide window是不同的
	//	//ssViewPtr->GotoSlide(ssViewPtr->GetSlide()->GetSlideIndex(), Office::msoFalse);
	//	//ssViewPtr->EraseDrawing();

	//	return TRUE;
	//}
	//else
	//{
	//	if (m_pApplication == NULL)
	//		return FALSE;

	//	SlideShowWindowsPtr pSlideShowWindows = m_pApplication->GetSlideShowWindows();
	//	if (pSlideShowWindows == NULL)
	//		return FALSE;
	//	
	//
	////	SetPointerType(ppSlideShowPointerPen);
	//	for (int i= 1; i <= pSlideShowWindows->GetCount(); i++)
	//	{
	//		SlideShowWindowPtr pSlideWindow = pSlideShowWindows->Item(_variant_t((long)i));
	//		if (pSlideWindow == NULL)
	//			continue;
	//		SlideShowViewPtr	pSlideView = pSlideWindow->GetView();
	//		if (pSlideView == NULL)
	//			continue;
	//		pSlideView->EraseDrawing();
	//

	//	}
	//	return TRUE;
	//}

	
}

//
// activate pane
//
BOOL CPPTController::ActivatePane(int nIndex)
{
	BEGIN_CATCH_COM_ERROR

	if ( m_pApplication == NULL )
		return FALSE;

	if ( IsPPTShowViewActive() )
		return FALSE;

	DocumentWindowPtr pDocWindow = m_pApplication->GetActiveWindow();
	if ( pDocWindow == NULL )
		return FALSE;

	pDocWindow->Activate();

	PanesPtr  pPanes = pDocWindow->GetPanes();

	if ( pPanes == NULL )
		return FALSE;

	PanePtr pPane = pPanes->Item(nIndex);

	if ( pPane == NULL )
		return FALSE;

	pPane->Activate();

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

//
// play indicated index of video
//
BOOL CPPTController::PlayVideo(int nIndex /*= -1*/)
{
	BEGIN_CATCH_COM_ERROR

	// find shape
	SlideShowViewPtr pShowView = GetSlideShowView();
	if( pShowView == NULL )
		return FALSE;

	_SlidePtr pSlide = pShowView->GetSlide();
	if( pSlide == NULL )
		return FALSE;

	int nVideoCount = 0;

	ShapesPtr pShapes = pSlide->GetShapes();
	for(int i = 1; i <= pShapes->GetCount(); i++)
	{
		ShapePtr pShape = pShapes->Item(_variant_t((long)i));
		if( pShape == NULL )
			continue;

		if( pShape->GetType() != Office::msoOLEControlObject  )
			continue;

		_bstr_t strName = pShape->GetName();
		if( _tcsstr((TCHAR*)strName, _T("WindowsMediaPlayer")) == NULL )
			continue;
	 	 
		OLEFormatPtr pOleFormat = pShape->GetOLEFormat();
		if( pOleFormat == NULL )
			continue;

		IDispatchPtr pObject = pOleFormat->Object;

		CWMPPlayer4 WmpPlayer;
		WmpPlayer.AttachDispatch(pObject, FALSE);
	
		CWMPControls WmpControl = WmpPlayer.get_controls();

		nVideoCount ++;

		// check index
		if( nVideoCount == nIndex )
		{
			WmpControl.play();
			break;
		}
		else if( nIndex == -1 )
			WmpControl.play();

		WmpPlayer.DetachDispatch();
	}
	
	END_CATCH_COM_ERROR_RET(FALSE)
	return TRUE;
}

//
// stop indicated index of video
//
BOOL CPPTController::StopVideo(int nIndex /*= -1*/)
{
	BEGIN_CATCH_COM_ERROR

	// find shape
	SlideShowViewPtr pShowView = GetSlideShowView();
	if( pShowView == NULL )
		return FALSE;

	_SlidePtr pSlide = pShowView->GetSlide();
	if( pSlide == NULL )
		return FALSE;

	int nVideoCount = 0;

	ShapesPtr pShapes = pSlide->GetShapes();
	for(int i = 1; i <= pShapes->GetCount(); i++)
	{
		ShapePtr pShape = pShapes->Item(_variant_t((long)i));
		if( pShape == NULL )
			continue;

		if( pShape->GetType() != Office::msoOLEControlObject  )
			continue;

		_bstr_t strName = pShape->GetName();
		if( _tcsstr((TCHAR*)strName, _T("WindowsMediaPlayer")) == NULL )
			continue;

		OLEFormatPtr pOleFormat = pShape->GetOLEFormat();
		if( pOleFormat == NULL )
			continue;

		IDispatchPtr pObject = pOleFormat->Object;

		CWMPPlayer4 WmpPlayer;
		WmpPlayer.AttachDispatch(pObject, FALSE);

		CWMPControls WmpControl = WmpPlayer.get_controls();

		nVideoCount ++;

		// check index
		if( nVideoCount == nIndex )
		{
			WmpControl.stop();
			break;
		}
		else if( nIndex == -1 )
			WmpControl.stop();

		WmpPlayer.DetachDispatch();
	}

	END_CATCH_COM_ERROR_RET(FALSE)
	return TRUE;

	
}

BOOL CPPTController::PlayVRVideo(int nIndex /*= -1*/)
{
	BEGIN_CATCH_COM_ERROR

		BOOL isChange = IsChanged(FALSE);
	// find shape
	SlideShowViewPtr pShowView = GetSlideShowView();
	if( pShowView == NULL )
		return FALSE;

	_SlidePtr pSlide = pShowView->GetSlide();
	if( pSlide == NULL )
		return FALSE;

	int nVideoCount = 0;

	ShapesPtr pShapes = pSlide->GetShapes();
	for(int i = 1; i <= pShapes->GetCount(); i++)
	{
		ShapePtr pShape = pShapes->Item(_variant_t((long)i));
		if( pShape == NULL )
			continue;

		//pShape->put_Visible(Office::msoFalse);

		if( pShape->GetType() != Office::msoOLEControlObject  )
			continue;

		_bstr_t strName = pShape->GetName();
		if( _tcsstr((TCHAR*)strName, _T("WindowsMediaPlayer")) == NULL )
			continue;

		//pShape->put_Visible(Office::msoTrue);
		OLEFormatPtr pOleFormat = pShape->GetOLEFormat();
		if( pOleFormat == NULL )
			continue;

		IDispatchPtr pObject = pOleFormat->Object;

		CWMPPlayer4 pp;
		pp.AttachDispatch(pObject, FALSE);

		//pp.put_uiMode("NONE");
		//pp.put_fullScreen(TRUE);
		//pp.put_enableContextMenu(FALSE);

		CWMPControls WmpControl = pp.get_controls();

		nVideoCount ++;

		// check index
		if( nVideoCount == nIndex )
		{
			WmpControl.play();
			break;
		}
		else if( nIndex == -1 )
			WmpControl.play();

		pp.DetachDispatch();
	}

	if (!isChange)
	{
		SetSaved(TRUE);
	}

	END_CATCH_COM_ERROR_RET(FALSE)
		return TRUE;
}

//
// stop indicated index of video
//
BOOL CPPTController::StopVRVideo(int nIndex /*= -1*/)
{
	BEGIN_CATCH_COM_ERROR
		BOOL isChange = IsChanged(FALSE);
	// find shape
	SlideShowViewPtr pShowView = GetSlideShowView();
	if( pShowView == NULL )
		return FALSE;

	_SlidePtr pSlide = pShowView->GetSlide();
	if( pSlide == NULL )
		return FALSE;

	int nVideoCount = 0;

	ShapesPtr pShapes = pSlide->GetShapes();
	for(int i = 1; i <= pShapes->GetCount(); i++)
	{
		ShapePtr pShape = pShapes->Item(_variant_t((long)i));
		if( pShape == NULL )
			continue;

		//pShape->put_Visible(Office::msoTrue);
		if( pShape->GetType() != Office::msoOLEControlObject  )
			continue;

		_bstr_t strName = pShape->GetName();
		if( _tcsstr((TCHAR*)strName, _T("WindowsMediaPlayer")) == NULL )
			continue;

		//pShape->put_Visible(Office::msoFalse);

		OLEFormatPtr pOleFormat = pShape->GetOLEFormat();
		if( pOleFormat == NULL )
			continue;

		IDispatchPtr pObject = pOleFormat->Object;

		CWMPPlayer4 WmpPlayer;
		WmpPlayer.AttachDispatch(pObject, FALSE);

		//WmpPlayer.put_uiMode("Invisible");

		CWMPControls WmpControl = WmpPlayer.get_controls();

		nVideoCount ++;

		// check index
		if( nVideoCount == nIndex )
		{
			WmpControl.stop();
			break;
		}
		else if( nIndex == -1 )
			WmpControl.stop();

		WmpPlayer.DetachDispatch();
	}
	if (!isChange)
	{
		SetSaved(TRUE);
	}
	END_CATCH_COM_ERROR_RET(FALSE)
		return TRUE;


}
//
// pause indicated index of video
//
BOOL CPPTController::PauseVideo(int nIndex /* = -1 */)
{
	BEGIN_CATCH_COM_ERROR

	// find shape
	SlideShowViewPtr pShowView = GetSlideShowView();
	if( pShowView == NULL )
		return FALSE;

	_SlidePtr pSlide = pShowView->GetSlide();
	if( pSlide == NULL )
		return FALSE;

	int nVideoCount = 0;

	ShapesPtr pShapes = pSlide->GetShapes();
	for(int i = 1; i <= pShapes->GetCount(); i++)
	{
		ShapePtr pShape = pShapes->Item(_variant_t((long)i));
		if( pShape == NULL )
			continue;

		if( pShape->GetType() != Office::msoOLEControlObject  )
			continue;

		_bstr_t strName = pShape->GetName();
		if( _tcsstr((TCHAR*)strName, _T("WindowsMediaPlayer")) == NULL )
			continue;

		OLEFormatPtr pOleFormat = pShape->GetOLEFormat();
		if( pOleFormat == NULL )
			continue;

		IDispatchPtr pObject = pOleFormat->Object;

		CWMPPlayer4 WmpPlayer;
		WmpPlayer.AttachDispatch(pObject, FALSE);

		CWMPControls WmpControl = WmpPlayer.get_controls();

		nVideoCount ++;

		// check index
		if( nVideoCount == nIndex )
		{
			WmpControl.pause();
			break;
		}
		else if( nIndex == -1 )
			WmpControl.pause();

		WmpPlayer.DetachDispatch();
	}

	END_CATCH_COM_ERROR_RET(FALSE)
	return TRUE;

	
}

//
// obtain video list
//
BOOL CPPTController::ObtainVideoList(vector<float>& vecVideoPostions, int& nSlideIndex )
{
	BEGIN_CATCH_COM_ERROR

	_PresentationPtr pPresentation = GetCurrentPresentation();  //GetPresentation();
	if( pPresentation == NULL )
		return NULL;

	

	if (m_pApplication==NULL || m_PPTSlideShowHwnd==NULL)
	{
		return FALSE;
	}
// 	if ( IsPPTShowViewActive() )
// 		return FALSE;

	SlideShowWindowPtr pSlideShowWindow = pPresentation->GetSlideShowWindow();
	if( pSlideShowWindow == NULL )
		return NULL;

	SlideShowViewPtr pSlideShowView = pSlideShowWindow->GetView();
	if( pSlideShowView == NULL )
		return NULL;

	PpSlideShowState pState=pSlideShowView->GetState();
	if(pState==ppSlideShowDone)
		return FALSE;

	float fWidth = pSlideShowWindow->GetWidth();
	float fHeight = pSlideShowWindow->GetHeight();


	_SlidePtr pSlide = pSlideShowView->GetSlide();
	if( pSlide == NULL )
		return FALSE;

	nSlideIndex = pSlide->GetSlideIndex();

	//
	int nSlideWidth = 1;
	int nSlideHeight = 1;
	GetSlideSize(nSlideWidth, nSlideHeight);
 
	ShapesPtr pShapes = pSlide->GetShapes();
	for(int i = 1; i <= pShapes->GetCount(); i++)
	{
		ShapePtr pShape = pShapes->Item(_variant_t((long)i));
		if( pShape == NULL )
			continue;

		if( pShape->GetType() != Office::msoOLEControlObject  )
			continue;

		_bstr_t strName = pShape->GetName();
		if( _tcsstr((TCHAR*)strName, _T("WindowsMediaPlayer")) == NULL )
			continue;

		// get position of this video
		float left		= pShape->GetLeft();
		float top		= pShape->GetTop();
		float width		= pShape->GetWidth();
		float height	= pShape->GetHeight();

// 		if (left<0 || top<0 || width<0 || height<0 || nSlideWidth<0 || nSlideHeight<0)
// 		{
// 			WRITE_LOG_LOCAL("CPPTController::ObtainVideoList, file:%s,left:%f, top:%f, width:%f, height:%f, slidewidth:%d, slideheight:%d", GetFilePath().c_str(), left, top, width, height, nSlideWidth, nSlideHeight);
// 		}

		left	/= (float)nSlideWidth;
		top		/= (float)nSlideHeight;
		width	/= (float)nSlideWidth;
		height  /= (float)nSlideHeight;
	
		vecVideoPostions.push_back(left);
		vecVideoPostions.push_back(top);
		vecVideoPostions.push_back(width);
		vecVideoPostions.push_back(height);

	}

	END_CATCH_COM_ERROR_RET(FALSE)
	return TRUE;
}

BOOL CPPTController::RestoreFlashFrameNum( int nSlideIdx )
{
	BEGIN_CATCH_COM_ERROR

	// find shape
	SlideShowViewPtr pShowView = GetSlideShowView();
	if( pShowView == NULL )
		return FALSE;

	PpSlideShowState pState=pShowView->GetState();
	if(pState==ppSlideShowDone)
		return FALSE;

	_SlidePtr pSlide = pShowView->GetSlide();
	if( pSlide == NULL )
		return FALSE;

	ShapesPtr pShapes = pSlide->GetShapes();

	for(int i = 1; i <= pShapes->GetCount(); i++)
	{
		ShapePtr pShape = pShapes->Item(_variant_t((long)i));
		if( pShape == NULL )
			continue;

		if( pShape->GetType() != Office::msoOLEControlObject  )
			continue;

		tstring strName = pShape->GetName();

		if ( strName != _T("ShockwaveFlash1") )
			continue;

		OLEFormatPtr pOleFormat = pShape->GetOLEFormat();
		if( pOleFormat == NULL )
			continue;

		IDispatchPtr pObject = pOleFormat->Object;

		CShockwaveFlash SwFlash;
		SwFlash.AttachDispatch(pObject, FALSE);

		SwFlash.put_FrameNum(0);
		//SwFlash.Play();

		SwFlash.DetachDispatch();

	}

	END_CATCH_COM_ERROR_RET(FALSE)
		return TRUE;
}


BOOL CPPTController::Pptx2Ppt( LPCTSTR szPath )
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

		if ( m_pApplication == NULL )
			return FALSE;

	PresentationsPtr pPres = m_pApplication->GetPresentations();

	if ( pPres == NULL)
		return FALSE;

	m_nOperationPpt		= OPE_USER_EXPORT;
	Sleep(10);
	_PresentationPtr pPre = pPres->Open(szPath, Office::msoFalse, Office::msoFalse, Office::msoFalse);
	Sleep(10);
	m_nOperationPpt		= OPE_POWERPOINT;

	tstring strPptPath = szPath;
	strPptPath = strPptPath.substr(0, strPptPath.length() - 1);

	pPre->SaveCopyAs(strPptPath.c_str(), ppSaveAsDefault, Office::msoFalse);
	pPre->Tags->Add(_T("close"),_T("thread"));
	pPre->Close();

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::ShowMediaShape()
{
	BEGIN_CATCH_COM_ERROR

		if ( m_pMediaShape != NULL )
			m_pMediaShape->PutVisible(Office::msoTrue);

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

tstring CPPTController::GetMediaPath( int& nIndex )
{
	BEGIN_CATCH_COM_ERROR
	
	tstring strPath = _T("");
	nIndex	= 0;

	_PresentationPtr pPresentation = GetCurrentPresentation();//GetPresentation();
	if ( pPresentation == NULL )
		return strPath;

	SlideShowViewPtr pShowView = GetSlideShowView();

	if ( pShowView == NULL )
		return strPath;

	PpSlideShowState state=pShowView->GetState();
	if(state==ppSlideShowDone)
		return strPath;

	_SlidePtr pSlide = pShowView->GetSlide();

	if ( pSlide == NULL )
		return strPath;

	nIndex = pSlide->GetSlideIndex();

	ShapesPtr pShapes = pSlide->GetShapes();

	if ( pShapes == NULL )
		return strPath;

	ShapePtr pShape;

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
			else if ( strSign.compare(PLUGIN_SIGN_HIDE) == 0 )
			{
				m_pMediaShape = pShape;
				m_pMediaShape->PutVisible(Office::msoFalse);
				strPath = strFilePath.substr(_tcslen(PLUGIN_SIGN_HIDE));
				break;
			}
		}
	}

	return strPath;

	END_CATCH_COM_ERROR_RET(_T(""))
}

tstring CPPTController::GetFilePath(BOOL bBefore)
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	_PresentationPtr pPresentation = GetPresentation(bBefore);
	if ( pPresentation == NULL )
			return _T("");

	tstring strPath = pPresentation->GetPath();
	if ( strPath.empty() )
		return _T("");

	tstring strFullPath = pPresentation->GetFullName();

	return strFullPath;

	END_CATCH_COM_ERROR_RET(_T(""))
}

tstring CPPTController::GetFileName(BOOL bBefore)
{
//	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	_PresentationPtr pPresentation = GetPresentation(bBefore);
	if ( pPresentation == NULL )
		return _T("");

	tstring strName = pPresentation->GetName();
	return strName;

	END_CATCH_COM_ERROR_RET(_T(""))
}

// 可能需要判断2013的菜单
BOOL CPPTController::IsPPTActive()
{
	if(m_PPTHwnd != NULL)
	{
		HWND hPop = GetLastActivePopup(m_PPTHwnd);
		if (hPop && m_PPTHwnd != hPop) {
			return FALSE;
		}else
			return TRUE;
	}
	return TRUE;
}

BOOL CPPTController::isPlayMode()
{
	if(m_PPTSlideShowHwnd != NULL)
	{
		if( IsWindow(m_PPTSlideShowHwnd) )
			return TRUE;
		else
		{
			return FALSE;
		}
	}else
		return FALSE;
}

BOOL CPPTController::IsPPTShowViewActive()
{
	BEGIN_CATCH_COM_ERROR

	if(m_pApplication == NULL)
			return FALSE;

	if(m_PPTSlideShowHwnd != NULL)
	{
		HWND hPop = GetLastActivePopup(m_PPTSlideShowHwnd);
		if (hPop && m_PPTSlideShowHwnd != hPop) {
			return FALSE;
		}else
			return TRUE;
	}
	return FALSE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::IfPPTSlideShowView()
{
	if (m_PPTSlideShowHwnd == NULL)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CPPTController::OnClosePPT()
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

		if(m_pApplication == NULL)
			return FALSE;


	PresentationsPtr pPres = m_pApplication->GetPresentations();

	if ( pPres->GetCount() > 1 )
	{
		//m_pPersentation = m_pApplication->GetActivePresentation();
		return TRUE;
	}

	return FALSE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

void CPPTController::TopShowSlideView()
{
	if ( m_PPTSlideShowHwnd != NULL )
		::SetForegroundWindow(m_PPTSlideShowHwnd);
}

// office2013 版本号——15
// office2010 版本号——14
// office2007 版本号——12
// office2003 版本号——11
int CPPTController::GetPPTVersion()
{
	if ( m_fPPTVersion < 13.0)
		return PPTVER_2007;

	if ( m_fPPTVersion <= 14.0)
		return PPTVER_2010;

	if ( m_fPPTVersion < 16.0)
		return PPTVER_2013;

	return PPTVER_UNKONW;
}

void CPPTController::RepairPPTBugVideoThumb()
{
	HWND h=::FindWindowEx(GetPPTHwnd(), NULL, _T("MDIClient"),NULL);
	if(h)
	{
		HWND h2=::FindWindowEx(h, NULL,  _T("mdiClass"),NULL);
		if(h2)
		{
			HWND h3=::FindWindowEx(h2, NULL,  _T("paneClassDC"),_T("Thumbnails"));
			if(h3)
			{
				CRect ct;
				::GetWindowRect(h3, &ct);
				if (ct.Width()%2 == 0)
					ct.right=ct.right-3;
				else
					ct.right=ct.right+3;
				//调整窗口大小触发系统的重刷事件
				::SetWindowPos(h3,NULL, 0, 0, ct.Width(), ct.Height(),SWP_NOMOVE|SWP_SHOWWINDOW);
			}
		}
	}
}

BOOL CPPTController::PresentationToImage(_PresentationPtr Pres, char* szDir)
{
	CResGuard::CGuard gd(m_Lock);
	BEGIN_CATCH_COM_ERROR
	return Pres->SaveAs(szDir, ppSaveAsJPG, Office::msoFalse);
	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::DeleteSlide()
{	
	CResGuard::CGuard gd(m_Lock);
	BEGIN_CATCH_COM_ERROR
	if( m_pApplication == NULL )
		return FALSE;

	_PresentationPtr pPresentation = GetCurrentPresentation();
	if ( pPresentation == NULL )
		return FALSE;

	VARIANT_BOOL bFinal = pPresentation->GetFinal();
	if ( bFinal )
		return FALSE;

	DocumentWindowPtr pDocWindow = m_pApplication->GetActiveWindow();
	if( pDocWindow == NULL )
		return FALSE;

	SelectionPtr pSelection = pDocWindow->GetSelection();
	if ( pSelection == NULL )
		return FALSE;

	SlideRangePtr pSlideRange = pSelection->GetSlideRange();
	if ( pSlideRange == NULL )
		return FALSE;

	pSlideRange->Delete();

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

void CPPTController::ReSetInit( )
{	
	m_bPPTWndInit = false;
	m_pEventHandler->TerminateMainMoniterThread();
}

void CPPTController::ActivePPTWnd()
{
	if(IsWindowVisible(m_PPTHwnd))
	{
		m_pApplication->Activate();
		SetForegroundWindow(m_PPTHwnd);
	}
}


int CPPTController::GetOffsetWithForbidenDrag()
{

	int nTopOffset = 0;

	if (GetPPTVersion() <= PPTVER_2007)
	{
		nTopOffset = 8;//8个像素
	}
	else if (GetPPTVersion() <= PPTVER_2013)
	{
		nTopOffset = 4;
	}

	return nTopOffset;
}

void CPPTController::OnSize( UINT nType, int cx, int cy )
{
	//if ( m_mapHwnd.size() <= 0 )
	//	return;

	//map<HWND, BOOL>::iterator iter = m_mapHwnd.begin();
	//for ( iter; iter != m_mapHwnd.end(); iter++ )
	//{
	//	::MoveWindow(iter->first, 0, -GetOffsetWithForbidenDrag(), cx, cy + GetOffsetWithForbidenDrag(), TRUE);
	//}
	
	if (m_PPTHwnd == NULL )
		return;
	::MoveWindow(m_PPTHwnd, 0, -GetOffsetWithForbidenDrag(), cx, cy + GetOffsetWithForbidenDrag(), TRUE);
}

HWND CPPTController::GetPPTMenuHwnd()
{
	//if (GetPPTVersion()<PPTVER_2013) return NULL;
	return ::FindWindowEx(m_MainFrmHwnd, NULL, _T("FullpageUIHost"), NULL);
}

BOOL CPPTController::AutoClosePPTMenu()
{
	HWND hwnd = GetPPTMenuHwnd();
	if (hwnd != NULL)
	{
		HWND hwndSub = FindWindowEx(hwnd, NULL, _T("NetUIHWND"), NULL);
		if(hwndSub)
		{
			SetForegroundWindow(hwndSub);
			PostMessage(hwndSub, WM_KEYDOWN, VK_ESCAPE, 0x00010001);
			Sleep(50);
			PostMessage(hwndSub, WM_CHAR, VK_ESCAPE, 0x00010001);
			Sleep(50);
			PostMessage(hwndSub, WM_KEYUP, VK_ESCAPE, 0xC0010001);
			Sleep(10);
		}
		keybd_event(VK_ESCAPE, 0, 0, 0);
		keybd_event(VK_ESCAPE, 0, KEYEVENTF_KEYUP, 0);
		return TRUE;
	}
	return FALSE;
}

int CPPTController::GetPresentationCount()
{
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR
	
	if ( m_pApplication == NULL )
		return 0;
	
	PresentationsPtr pPresentations = m_pApplication->GetPresentations();
	if ( pPresentations == NULL )
		return 0;

	return pPresentations->GetCount();

	END_CATCH_COM_ERROR_RET(-1)
}

bool CPPTController::OnApplicationClose()
{
	if (GetPPTVersion() >= PPTVER_2010)
	{
		HWND hMenu = ::FindWindowEx(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), NULL, _T("FullpageUIHost"), NULL);
		if (hMenu)
		{
			HWND hwndSub = ::FindWindowEx(hMenu, NULL, _T("NetUIHWND"), NULL);
			if(hwndSub)
			{
				::PostMessage(hwndSub, WM_KEYDOWN, VK_ESCAPE, 0x00010001);
				::PostMessage(hwndSub, WM_CHAR, VK_ESCAPE, 0x00010001);
				::PostMessage(hwndSub, WM_KEYUP, VK_ESCAPE, 0xC0010001);

				return false;
			}
		}
	}

	if (g_hMonitoringMenuThread)
	{
		TerminateThread(g_hMonitoringMenuThread, 0);
		g_hMonitoringMenuThread=NULL;
	}

	return true;
}

BOOL CPPTController::IsFinal()
{
	BEGIN_CATCH_COM_ERROR
	
	_PresentationPtr pPresentation = GetPresentation(FALSE);

	if ( pPresentation == NULL )
		return FALSE;

	return pPresentation->GetFinal();

	END_CATCH_COM_ERROR_RET(FALSE)
}

//void CPPTController::SetUpdatePresentationStatus(BOOL bNeedUpdate)
//{
//	m_bNeedUpdatePrenation = bNeedUpdate;
//}
//
//BOOL CPPTController::IsNeedUpdatePresentation()
//{
//	return m_bNeedUpdatePrenation;
//}


//
// Pack
//
DWORD CPPTController::PackAllResource(HWND hwnd, void* pNotify, char* szPath)
{
	CResGuard::CGuard gd(m_Lock);
	BEGIN_CATCH_COM_ERROR

	PackNotify* pPackNotify = (PackNotify*)pNotify; 

	TCHAR szSrcDrive[8]			= {0};
	TCHAR szSrcDir[MAX_PATH*4]		= {0};
	TCHAR szSrcFileName[MAX_PATH]	= {0};
	TCHAR szSrcExt[8]				= {0};
	if (_tsplitpath_s(szPath, szSrcDrive, szSrcDir, szSrcFileName, szSrcExt))
	{
		WRITE_LOG_LOCAL(_T("Pack Fail, Path:%s, error:%d"), szPath, GetLastError());
		return 1;
	}

	TCHAR szPackDir[MAX_PATH * 4];
	//创建临时目录
	//////////////////////////////////////////////////////////////////////////
//	DWORD dwCrc = CalcFileCRC(szPath);
	_sntprintf_s(szPackDir, sizeof(szPackDir) - 1,  _T("%s%s%s_打包文件"),szSrcDrive, szSrcDir, szSrcFileName/*, dwCrc*/);

	DeleteDir(szPackDir);
	CreateDirectory(szPackDir, NULL);

	//////////////////////////////////////////////////////////////////////////


	//拷贝ppt
	tstring strCopyFile = szPackDir;
	strCopyFile += _T("\\");
	strCopyFile += szSrcFileName;
	strCopyFile += szSrcExt;
	if(CopyFile(szPath, strCopyFile.c_str(), FALSE) == FALSE)
	{
		WRITE_LOG_LOCAL(_T("CopyFile Fail, Path:%s, DestPath:%s, error:%d"), szPath, strCopyFile.c_str(), GetLastError());
		return 2;
	}

	TCHAR szDestDrive[8]			= {0};
	TCHAR szDestDir[MAX_PATH * 4]		= {0};
	TCHAR szDestFileName[MAX_PATH]	= {0};
	TCHAR szDestExt[8]				= {0};
	if (_tsplitpath_s(strCopyFile.c_str(), szDestDrive, szDestDir, szDestFileName, szDestExt))
	{
		WRITE_LOG_LOCAL(_T("File split Fail, Path:%s, error:%d"), strCopyFile.c_str(),GetLastError());
		return 1;
	}

	int nRet = 0;
	if ( m_pApplication == NULL )
	{
		WRITE_LOG_LOCAL(_T("Application NULL"));
		return 3;
	}
	

	PresentationsPtr pPres = m_pApplication->GetPresentations();

	if ( pPres == NULL)
	{
		WRITE_LOG_LOCAL(_T("Presentations NULL"));
		return 4;
	}

	m_nOperationPpt		= OPE_USER_PACK;
	Sleep(10);
	_PresentationPtr pPre = pPres->Open(strCopyFile.c_str(), Office::msoFalse, Office::msoFalse, Office::msoFalse);
	Sleep(10);
	m_nOperationPpt		= OPE_POWERPOINT;

	if (pPre == NULL)
	{
		WRITE_LOG_LOCAL(_T("Presentation NULL"));
		return 5;
	}

	SlidesPtr pSlides = pPre->GetSlides();
	if( pSlides == NULL )
	{
		WRITE_LOG_LOCAL(_T("Slides NULL"));
		pPre->Close();
		return 6;
	}

	ShapesPtr pShapes;
	ShapePtr pShape;

	int nSlidesCount = pSlides->GetCount();
	pPackNotify->nPackStep = PACK_PPT_START;
	pPackNotify->dwRet = nSlidesCount;
	::SendMessage(hwnd, WM_USER_PACK_PPT, (WPARAM)pPackNotify, (LPARAM)NULL);

	int nShapesCount;

	OLEFormatPtr pOle = NULL;
	IDispatchPtr pObj = NULL;
	CWMPPlayer4 Player;
//	CShockwaveFlash* pFlash = new CShockwaveFlash;
//	CDandcef* pDandCef = new CDandcef;
	CDandcef DandCef;
	CShockwaveFlash* pFlash = NULL;

	TCHAR szPackDestPath[MAX_PATH * 4];
	TCHAR szPackDestFile[MAX_PATH * 4];
	TCHAR szPackSrcPath[MAX_PATH * 4];
	TCHAR szPackSrcFile[MAX_PATH * 4];
	TCHAR szPackRelativePath[MAX_PATH * 4];

	//
	TCHAR szResDrive[8]			= {0};
	TCHAR szResDir[MAX_PATH * 4]		= {0};
	TCHAR szResFileName[MAX_PATH]	= {0};
	TCHAR szResExt[8]				= {0};

	pPackNotify->nPackStep = PACK_PPT_SLIDER;
	for(int i = 1; i <= nSlidesCount && !pPackNotify->bCancel; i++)
	{
		::SendMessage(hwnd, WM_USER_PACK_PPT, (WPARAM)pPackNotify, (LPARAM)NULL);
		Sleep(100);
		_SlidePtr pSlide = pSlides->Item(_variant_t((long)i));
		if( pSlide == NULL )
			continue;
		
		pShapes = pSlide->GetShapes();
		nShapesCount = pShapes->GetCount();
		for (int j = 1; j <= nShapesCount; j++)
		{
			pShape = pShapes->Item(_variant_t(long(j)));
			if ( pShape == NULL )
				continue;

			_bstr_t strName = pShape->GetName();
			TCHAR * szName = strName;
			if (_tcsstr(szName, _T("WindowsMediaPlayer")) )
			{
				OLEFormatPtr pOle = pShape->GetOLEFormat();
				IDispatchPtr pObj = pOle->Object;
				
				Player.AttachDispatch(pObj, FALSE);
				tstring strUrl = Player.get_URL();
				if(strUrl.empty())
				{
					Player.DetachDispatch();
					continue;
				}

				TCHAR szUrlDrive[8]			= {0};
				TCHAR szUrlDir[MAX_PATH * 4]		= {0};
				TCHAR szUrlFileName[MAX_PATH]	= {0};
				TCHAR szUrlExt[8]				= {0};

				std::tr1::regex szUrlRule(_T("^(http[s]{0,1})://[a-zA-Z0-9_-]+$"));
				std::tr1::smatch sResult;
				if(std::tr1::regex_search(strUrl, sResult, szUrlRule))
				{
					Player.DetachDispatch();
					//url
					continue;
				}
				else if (!_tsplitpath_s(strUrl.c_str(), szResDrive, szResDir, szResFileName, szResExt))
				{
					if(_tcslen(szResDrive) > 0)
					{
						//绝对路径
						_sntprintf_s(szPackRelativePath, sizeof(szPackRelativePath) - 1,  _T("Video\\Slider%d\\%s%s"), i, szResFileName, szResExt);
						_sntprintf_s(szPackDestFile, sizeof(szPackDestFile) - 1,  _T("%s\\%s"),szPackDir, szPackRelativePath);
						//拷贝视频和音频文件
						createDirWithFullPath(szPackDestFile);

						BOOL bRet = CopyFile(strUrl.c_str(), szPackDestFile, FALSE);
						if(!bRet)
						{
							Player.DetachDispatch();
							pPackNotify->strFailPath = strUrl;
							pPre->Close();
							return 50;
						}
					}
					else
					{
						//相对路径
						_sntprintf_s(szPackRelativePath, sizeof(szPackRelativePath) - 1,  _T("Video\\Slider%d\\%s%s"), i, szResFileName, szResExt);
						_sntprintf_s(szPackDestFile, sizeof(szPackDestFile) - 1,  _T("%s\\%s"),szPackDir, szPackRelativePath);

						if (_tcsnicmp(szResDir, _T("\\\\"), 2) == 0 ) // 共享盘
							_sntprintf_s(szPackSrcFile, sizeof(szPackSrcFile) - 1,  _T("%s"), strUrl.c_str());
						else
							_sntprintf_s(szPackSrcFile, sizeof(szPackSrcFile) - 1,  _T("%s%s%s"), szSrcDrive, szSrcDir, strUrl.c_str());
						//拷贝视频和音频文件
						createDirWithFullPath(szPackDestFile);

						BOOL bRet =CopyFile(szPackSrcFile, szPackDestFile, FALSE);
						if(!bRet)
						{
							Player.DetachDispatch();
							pPackNotify->strFailPath = szPackSrcFile;
							pPre->Close();
							return 50;
						}
					}

					//
					Player.put_URL(szPackRelativePath);
				}

				Player.DetachDispatch();
			}
			else if (_tcsstr(szName, _T("andcef")) )//习题
			{
				
				OLEFormatPtr pOle = pShape->GetOLEFormat();
				IDispatchPtr pObj = pOle->Object;

				DandCef.AttachDispatch(pObj, FALSE);
				CString cstrUrl = DandCef.GetUrl();

				if(cstrUrl.GetLength() == 0)
				{
					DandCef.DetachDispatch();
					continue;
				}
				//d:\NDCloud\Package\nodejs\app\player\index.html?main=/d:/NDCloud/Question/ddaac079-3967-4ab0-b725-b6ba1713d3a3_default/main.xml?sys=pptshell
				CString cstrPath,cstrId;
				int nPos = cstrUrl.Find(_T("main=/"));
				int nPosEnd = cstrUrl.Find(_T("/main.xml"));

				int nStart = 0;
				if(nPos != tstring::npos)
					nStart = nPos + _tcslen(_T("main=/"));
				int nCount = nPosEnd - nStart ;
				cstrPath = cstrUrl.Mid(nStart , nCount);

				int nPosEnd1 = cstrPath.ReverseFind('/');
				cstrId = cstrPath.Mid(nPosEnd1 + 1);

				std::tr1::regex szUrlRule(_T("^(http[s]{0,1})://[a-zA-Z0-9_-]+$"));
				std::tr1::smatch sResult;

				cstrPath.Replace(_T("/"),_T("\\"));
				tstring strPath = cstrPath.GetString();
				
				if(std::tr1::regex_search(strPath, sResult, szUrlRule))
				{
					//url
					DandCef.DetachDispatch();
					continue;
				}
				else if (!_tsplitpath_s(strPath.c_str(), szResDrive, szResDir, szResFileName, szResExt))
				{
					if(_tcslen(szResDrive) > 0)
					{
						//绝对路径
						_sntprintf_s(szPackRelativePath, sizeof(szPackRelativePath) - 1,  _T("Question\\%s\\"), cstrId.GetString());
						_sntprintf_s(szPackDestPath, sizeof(szPackDestPath) - 1,  _T("%s\\%s"),szPackDir, szPackRelativePath);
						_sntprintf_s(szPackSrcPath, sizeof(szPackSrcPath) - 1,  _T("%s%s%s\\"), szResDrive, szResDir, cstrId.GetString());

						//拷贝系统完整目录
						createDirWithFullPath(szPackDestPath);
						TCHAR szFailPath[MAX_PATH * 4] = {0};
						BOOL bRet =CopyDir(szPackSrcPath, szPackDestPath, szFailPath);
						if(!bRet)
						{
							DandCef.DetachDispatch();
							pPackNotify->strFailPath = szFailPath;
							pPre->Close();
							return 50;
						}
					}
					else
					{
						//相对路径
						_sntprintf_s(szPackRelativePath, sizeof(szPackRelativePath) - 1,  _T("Question\\%s\\"), cstrId.GetString());
						_sntprintf_s(szPackDestPath, sizeof(szPackDestPath) - 1,  _T("%s\\%s"), szPackDir, szPackRelativePath);
						_sntprintf_s(szPackSrcPath, sizeof(szPackSrcPath) - 1,  _T("%s%s%s%s\\"), szSrcDrive, szSrcDir, szResDir, cstrId.GetString());

						//拷贝系统完整目录
						createDirWithFullPath(szPackDestPath);
						TCHAR szFailPath[MAX_PATH * 4] = {0};
						BOOL bRet = CopyDir(szPackSrcPath, szPackDestPath, szFailPath);
						if(!bRet)
						{
							DandCef.DetachDispatch();
							pPackNotify->strFailPath = szFailPath;
							pPre->Close();
							return 50;
						}
					}

					//修改配置
					CString cstrParam = cstrUrl.Mid(nPosEnd + 1);
 					CString cstrPackRelativePath = szPackRelativePath;
 					cstrPackRelativePath += cstrParam;

					cstrPackRelativePath.Replace(_T("\\\\"),_T("\\"));
					cstrPackRelativePath.Replace(_T("\\"),_T("/"));
					DandCef.SetUrl(cstrPackRelativePath.GetString());


// 
// 					TiXmlNode* pChild = pPathElement->FirstChild();
// 					if(!pChild)
// 						continue;
// 					pChild->SetValue(strPackRelativePath.c_str());
// 					TiXmlPrinter printer;
// 					doc.Accept(&printer);
// 					tstring xml = printer.CStr();
// 					pShape->PutAlternativeText(xml.c_str());
					//

				}
				DandCef.DetachDispatch();
			}
		}
		//
	}
// 
// 	if(pPlayer != NULL)
// 	{
// 		delete pPlayer;
// 		pPlayer = NULL;
// 	}
// 
// 	if(pFlash != NULL)
// 	{
// 		delete pFlash;
// 		pFlash = NULL;
// 	}

// 	if(pDandCef != NULL)
// 	{
// 		delete pDandCef;
// 		pFlash = NULL;
// 	}

	m_nOperationPpt		= OPE_USER_PACK;
	int nPos = strCopyFile.rfind(_T(".pptx"));
	if ( nPos == tstring::npos )
	{
		nPos = strCopyFile.rfind(_T(".ppt"));
		tstring strSaveFile = strCopyFile.substr(0, nPos);
		strSaveFile += _T(".pptx");
		pPre->SaveAs(strSaveFile.c_str(), ppSaveAsDefault, Office::msoTriStateMixed);
		pPackNotify->strPath = strSaveFile;
		DeleteFile(strCopyFile.c_str());
	}
	else
	{
		pPre->SaveAs(strCopyFile.c_str(), ppSaveAsDefault, Office::msoTriStateMixed);
	}
	m_nOperationPpt		= OPE_POWERPOINT;
	pPre->Close();

	if(pPackNotify->bCancel)
	{
		return 100;
	}

	//打包
	pPackNotify->nPackStep = PACK_PPT_ZIP;
	::SendMessage(hwnd, WM_USER_PACK_PPT, (WPARAM)pPackNotify, (LPARAM)NULL);
	if(ZipPackDir(szPackDir, _T("")) == FALSE)
	{
		WRITE_LOG_LOCAL("ZipPackDir Fail, PackDir:%s, error:%d", szPackDir, GetLastError());
		return 7;
	}
	//

	if(pPackNotify->bCancel)
	{
		return 100;
	}

	//修改名称
	tstring strPPTZipFile = szPackDir;
	tstring strNdPPTFile = strPPTZipFile;
	strPPTZipFile += _T(".zip");
	strNdPPTFile += _T(".ndpx");
	if(::MoveFileEx(strPPTZipFile.c_str(), strNdPPTFile.c_str(), MOVEFILE_COPY_ALLOWED|MOVEFILE_REPLACE_EXISTING) == FALSE)
	{
		WRITE_LOG_LOCAL("MoveFileEx Fail, PPTZipFile:%s, NdPPTFile,%s, error:%d", strPPTZipFile.c_str(), strNdPPTFile.c_str(), GetLastError());
		return 8;
	}
	//

	//删除PPT
	DeleteDir(szPackDir);
	//DeleteFile(szPath);
	//
	pPackNotify->strPath = strNdPPTFile;
	return -1;

	END_CATCH_COM_ERROR_RET(FALSE)
}

//
// UnPack
//
DWORD CPPTController::UnPackAllResource(HWND hwnd, void* pNotify, char* szPath)
{
	CResGuard::CGuard gd(m_Lock);
	BEGIN_CATCH_COM_ERROR

	PackNotify* pPackNotify = (PackNotify*)pNotify; 

	TCHAR szSrcDrive[8]			= {0};
	TCHAR szSrcDir[MAX_PATH * 4]		= {0};
	TCHAR szSrcFileName[MAX_PATH]	= {0};
	TCHAR szSrcExt[8]				= {0};
	if (_tsplitpath_s(szPath, szSrcDrive, szSrcDir, szSrcFileName, szSrcExt))
	{
		WRITE_LOG_LOCAL(_T("UnPack Fail, Path:%s, error:%d"), szPath, GetLastError());
		return 1;
	}

	tstring strPPTFileName = szSrcFileName;

	TCHAR szUnPackDir[MAX_PATH * 4];
	tstring strMd5 = CalcFileMD5(szPath);
	tstring strNDCloudDirectory = NDCloudFileManager::GetInstance()->GetNDCloudDirectory();
	_stprintf(szUnPackDir, _T("%s\\PPTTemp\\%s"), strNDCloudDirectory.c_str(), strMd5.c_str());

	if(UnZipPackDir(szPath, szUnPackDir) == FALSE)
	{
		WRITE_LOG_LOCAL(_T("UnZipPackDir Fail, ZipPath:%s, DesPath:%s, error:%d"), szPath, szUnPackDir, GetLastError());
		return 2;
	}

 	tstring strUnpackFile ;
	tstring strCloudFilePPT = szUnPackDir;

	strCloudFilePPT += _T("\\*.*");
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = ::FindFirstFile(strCloudFilePPT.c_str(), &FindFileData);
	if(hFind != INVALID_HANDLE_VALUE)
	{
		do 
		{
			tstring strFileName = FindFileData.cFileName;
			WRITE_LOG_LOCAL(_T("FindFirstFile Path:%s"), strFileName.c_str());
			if(strFileName.find(_T("~$")) == tstring::npos && strFileName.find(_T(".ppt")) != tstring::npos )
			{
				strUnpackFile = szUnPackDir;
				strUnpackFile += _T("\\");
				strUnpackFile += FindFileData.cFileName;
				break;
			}
		}while (::FindNextFile(hFind, &FindFileData));
		FindClose(hFind);
	}

	if(strUnpackFile.length() == 0)
	{
		WRITE_LOG_LOCAL(_T("UnpackFile Not Find, Dir:%s, error:%d"), szUnPackDir, GetLastError());
		return 3;
	}

	if ( m_pApplication == NULL )
	{
		WRITE_LOG_LOCAL(_T("Application NULL"));
		return 4;
	}

	PresentationsPtr pPres = m_pApplication->GetPresentations();

	if ( pPres == NULL)
	{
		WRITE_LOG_LOCAL(_T("Presentations NULL"));
		return 5;
	}

	//判断是否已经打开
	_PresentationPtr pPre	= NULL;
	for(int i = 1 ; i <= pPres->GetCount(); i++)
	{
		pPre = pPres->Item(_variant_t((LONG)i));
		if(pPre)
		{
			tstring strFullName = pPre->GetFullName();
			WRITE_LOG_LOCAL(_T("File exsit Name :%s"), strFullName.c_str());
			if(strFullName.find(strUnpackFile) != tstring::npos)
			{
				return 51;
			}
		}
	}
	
	//

	pPackNotify->strPath = strUnpackFile;

	WRITE_LOG_LOCAL(_T("Open PPT %s"), strUnpackFile.c_str());
	m_nOperationPpt		= OPE_USER_PACK;
	Sleep(10);
	pPre = pPres->Open(strUnpackFile.c_str(), Office::msoFalse, Office::msoFalse, Office::msoFalse);
	Sleep(10);
	m_nOperationPpt		= OPE_POWERPOINT;

	WRITE_LOG_LOCAL(_T("Open PPT %s success"), strUnpackFile.c_str());
	if (pPre == NULL)
	{
		WRITE_LOG_LOCAL(_T("Presentation NULL"));
		return 6;
	}

	SlidesPtr pSlides = pPre->GetSlides();
	if( pSlides == NULL )
	{
		WRITE_LOG_LOCAL(_T("Slides NULL"));
		pPre->Close();
		return 7;
	}

	ShapesPtr pShapes;
	ShapePtr pShape;

	int nSlidesCount = pSlides->GetCount();
	pPackNotify->nPackStep = PACK_PPT_START;
	pPackNotify->dwRet = nSlidesCount;
	::SendMessage(hwnd, WM_USER_PACK_PPT, (WPARAM)pPackNotify, (LPARAM)NULL);

	int nShapesCount;

	OLEFormatPtr pOle = NULL;
	IDispatchPtr pObj;
	CWMPPlayer4 Player;
	CDandcef DandCef;
	CShockwaveFlash* pFlash = NULL;

	TCHAR szPackAbsolutePath[MAX_PATH * 4];

	//
	TCHAR szResDrive[8]				= {0};
	TCHAR szResDir[MAX_PATH * 4]	= {0};
	TCHAR szResFileName[MAX_PATH]	= {0};
	TCHAR szResExt[8]				= {0};

	pPackNotify->nPackStep = PACK_PPT_SLIDER;
	for(int i = 1; i <= nSlidesCount && !pPackNotify->bCancel; i++)
	{
		::SendMessage(hwnd, WM_USER_PACK_PPT, (WPARAM)pPackNotify, (LPARAM)NULL);
		_SlidePtr pSlide = pSlides->Item(_variant_t((long)i));
		if( pSlide == NULL )
			continue;

		pShapes = pSlide->GetShapes();
		nShapesCount = pShapes->GetCount();
		for (int j = 1; j <= nShapesCount; j++)
		{
			pShape = pShapes->Item(_variant_t(long(j)));
			if ( pShape == NULL )
				continue;

			//_bstr_t bstrName = pShape->GetName();
			tstring strName = pShape->GetName();
			transform(strName.begin(), strName.end(), strName.begin(), ::tolower);
			if (strName.find(_T("windowsmediaplayer")) != tstring::npos)
			{
				OLEFormatPtr pOle = pShape->GetOLEFormat();
				IDispatchPtr pObj = pOle->Object;

				Player.AttachDispatch(pObj, FALSE);
				tstring strUrl = Player.get_URL();
				if(strUrl.empty())
				{
					Player.DetachDispatch();
					continue;
				}

				std::tr1::regex szUrlRule(_T("^(http[s]{0,1})://[a-zA-Z0-9_-]+$"));
				std::tr1::smatch sResult;
				if(std::tr1::regex_search(strUrl, sResult, szUrlRule))
				{
					Player.DetachDispatch();
					//url
					continue;
				}
				else if (!_tsplitpath_s(strUrl.c_str(), szResDrive, szResDir, szResFileName, szResExt))
				{
					if(_tcslen(szResDrive) == 0)
					{
						//相对路径
						_sntprintf_s(szPackAbsolutePath, sizeof(szPackAbsolutePath) - 1,  _T("%s\\Video\\Slider%d\\%s%s"),szUnPackDir, i, szResFileName, szResExt);
					}

					//
					Player.put_URL(szPackAbsolutePath);
				}

				Player.DetachDispatch();
			}
			else if (strName.find(_T("andcef")) != tstring::npos)//习题
			{
				OLEFormatPtr pOle = pShape->GetOLEFormat();
				IDispatchPtr pObj = pOle->Object;

				DandCef.AttachDispatch(pObj, FALSE);
				CString cstrUrl = DandCef.GetUrl();

				if(cstrUrl.GetLength() == 0)
				{
					DandCef.DetachDispatch();
					continue;
				}

				CString cstrPath,cstrId;
				int nPos = cstrUrl.Find(_T("main=/"));
				int nPosEnd = cstrUrl.Find(_T("/main.xml"));

				int nStart = 0;
				if(nPos != tstring::npos)
					nStart = nPos + _tcslen(_T("main=/"));
				int nCount = nPosEnd - nStart ;
				cstrPath = cstrUrl.Mid(nStart , nCount);

				int nPosEnd1 = cstrPath.ReverseFind('/');
				cstrId = cstrPath.Mid(nPosEnd1 + 1);

				std::tr1::regex szUrlRule(_T("^(http[s]{0,1})://[a-zA-Z0-9_-]+$"));
				std::tr1::smatch sResult;

				cstrPath.Replace(_T("/"),_T("\\"));
				tstring strPath = cstrPath.GetString();
				if(std::tr1::regex_search(strPath, sResult, szUrlRule))
				{
					DandCef.DetachDispatch();
					//url
					continue;
				}
				else if (!_tsplitpath_s(strPath.c_str(), szResDrive, szResDir, szResFileName, szResExt))
				{
					if(_tcslen(szResDrive) == 0)
					{
						//相对路径
						_sntprintf_s(szPackAbsolutePath, sizeof(szPackAbsolutePath) - 1,  _T("%s\\Question\\%s\\"), szUnPackDir, cstrId.GetString());
					}

					//修改配置
					CString cstrPackAbsolutePath = szPackAbsolutePath;
					cstrPackAbsolutePath.Replace(_T("\\"),_T("/"));
					CString cstrParam = cstrUrl.Mid(nPosEnd + 1);
					CString cstrPackRelativePath = GetLocalPath().c_str();
					cstrPackRelativePath += _T("\\Package\\nodejs\\app\\player\\index.html?main=/");
					cstrPackRelativePath += cstrPackAbsolutePath;
					cstrPackRelativePath += cstrParam;

					cstrPackRelativePath.Replace(_T("\\\\"),_T("\\"));
					DandCef.SetUrl(cstrPackRelativePath.GetString());

					CString cstrImagePath = szPackAbsolutePath;
					cstrImagePath += _T("main.xml.jpg");
					DandCef.SetimgUrl(cstrImagePath.GetString());

				}
				DandCef.DetachDispatch();
			}
		}
	}

	m_nOperationPpt		= OPE_USER_PACK;

	BOOL bDel = FALSE;

	WRITE_LOG_LOCAL(_T("SaveAs PPT %s"), strUnpackFile.c_str());
	//if ( pPre->GetSaved() == Office::msoTrue )
	{
		int nPos = strUnpackFile.rfind(_T(".pptx"));
		if ( nPos == tstring::npos )
		{
			nPos = strUnpackFile.rfind(_T(".ppt"));
			tstring strSaveFile = strUnpackFile.substr(0, nPos);
			strSaveFile += _T(".pptx");
			pPre->SaveAs(strSaveFile.c_str(), ppSaveAsDefault, Office::msoTriStateMixed);
			pPackNotify->strPath = strSaveFile;

			bDel = TRUE;
		}
		else
		{
			pPre->SaveAs(strUnpackFile.c_str(), ppSaveAsDefault, Office::msoTriStateMixed);
		}
	}

	WRITE_LOG_LOCAL(_T("SaveAs PPT %s success"), strUnpackFile.c_str());
	m_nOperationPpt		= OPE_POWERPOINT;
	pPre->Close();

	if ( bDel )
		DeleteFile(strUnpackFile.c_str());
	return -1;

	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::KillCrashPowerPoint()
{
	BEGIN_CATCH_COM_ERROR

	if ( m_pApplication == NULL )
		return FALSE;
	
	HWND hWnd = (HWND)m_pApplication->GetHWND();

	if ( hWnd == NULL )
		return FALSE;

	CRect rtPPT = CRect(-10, -10, -10, -10);
	GetClientRect(hWnd, &rtPPT);

	if (rtPPT.Width() == 0 && rtPPT.Height() == 0 
		&& -10 == rtPPT.left && -10 == rtPPT.top 
		&& -10 == rtPPT.right && -10 == rtPPT.bottom)
	{
		PPTControllerMgrUpdate();
		KillExeCheckParentPid(PPT_EXE_NAME);
		Sleep(500);
		initApp();
		return TRUE;
	}

	return FALSE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::AddSlide()
{
	BEGIN_CATCH_COM_ERROR

	if( m_pApplication == NULL )
		return FALSE;
	
	_PresentationPtr prePtr = GetCurrentPresentation();
	if ( prePtr == NULL )
		return FALSE;

	SlidesPtr slidesPtr = prePtr->GetSlides();
	
	if ( slidesPtr == NULL )
		return FALSE;

	int nCurIndex = GetActiveSlideIndex();

	if ( nCurIndex == 0 )
	{
		nCurIndex = slidesPtr->GetCount();

		if ( nCurIndex != 0 )
			return PPTEC_SLIDE_NOT_SELECT;
	}

	_SlidePtr slidePtr = slidesPtr->Add(++nCurIndex, ppLayoutTitle);

	if ( slidePtr != NULL )
	{
		BEGIN_CATCH_COM_ERROR
		slidePtr->Select();
		END_CATCH_COM_ERROR
	}

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

//
// obtain question list
//
BOOL CPPTController::ObtainQuestionList(std::vector<tstring> &vecQuestionList)
{
	BEGIN_CATCH_COM_ERROR

	SlidesPtr pSlides = GetSlides();
	
	for(int i = 1; i <= pSlides->GetCount(); i++)
	{
		_SlidePtr pSlide = pSlides->Item(_variant_t((long)i));
		if( pSlide == NULL )
			return FALSE;

		ShapesPtr pShapes = pSlide->GetShapes();
		int nIndex = pSlide->GetSlideIndex();

		CString strUrl = _T("");
		for(int i = 1; i <= pShapes->GetCount(); i++)
		{
			ShapePtr pShape = pShapes->Item(_variant_t((long)i));
			if( pShape == NULL )
				continue;

			if( pShape->GetType() != Office::msoOLEControlObject  )
				continue;

			_bstr_t strName = pShape->GetName();
			if( _tcsstr((TCHAR*)strName, _T("andcef")) == NULL )
				continue;

			OLEFormatPtr pOleFormat = pShape->GetOLEFormat();
			IDispatchPtr iDisPtr = pOleFormat->Object;

			CDandcef pp;
			pp.AttachDispatch(iDisPtr, FALSE);

			strUrl = pp.GetUrl();
			pp.DetachDispatch();

			//pp.SetSlideIndex(nIndex);
			break;
		}

		vecQuestionList.push_back(strUrl.GetString());
	}

	return TRUE;

	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::HasQuestionAtPage(int nIndex)
{
	BEGIN_CATCH_COM_ERROR
	SlidesPtr pSlides	= GetSlides();
	_SlidePtr pSlide	= pSlides->Item(_variant_t((long)nIndex));
	if( pSlide == NULL )
		return FALSE;

	ShapesPtr pShapes = pSlide->GetShapes();
	for(int i = 1; i <= pShapes->GetCount(); i++)
	{
		ShapePtr pShape = pShapes->Item(_variant_t((long)i));
		if( pShape == NULL )
			continue;

		if( pShape->GetType() != Office::msoOLEControlObject  )
			continue;

		_bstr_t strName = pShape->GetName();
		if( _tcsstr((TCHAR*)strName, _T("andcef")) != NULL )
		{
			return TRUE;
		}
	}



	END_CATCH_COM_ERROR_RET(FALSE)
		return FALSE;
}

BOOL CPPTController::GetSlideAllShapesInfo( int nIndex, CStream* pStream )
{
	int		nCount		= 0;

	pStream->ResetCursor();
	pStream->WriteInt(nIndex);
	pStream->WriteInt(0);

	BEGIN_CATCH_COM_ERROR

	SlidesPtr pSlides	= GetSlides();
	_SlidePtr pSlide	= pSlides->Item(_variant_t((long)nIndex));
	if( pSlide == NULL )
		return FALSE;

	tstring strPath		= _T("");
	ShapesPtr pShapes	= pSlide->GetShapes();
	for(int i = 1; i <= pShapes->GetCount(); i++)
	{
		ShapePtr pShape = pShapes->Item(_variant_t((long)i));
		if( pShape == NULL )
			continue;

		int nType = pShape->GetType();
		if ( nType == Office::msoPicture || nType == Office::msoPlaceholder )
		{
			tstring strFilePath = pShape->GetAlternativeText();
			tstring strSign = strFilePath.substr(0, _tcslen(PLUGIN_VR));
			if ( strSign.compare(PLUGIN_VR) == 0 )
			{
				strPath = strFilePath.substr(_tcslen(PLUGIN_VR));
				pStream->WriteInt(SHAPE_VR);
				pStream->WriteString(strPath);
				nCount++;
			}
		}
		else if( pShape->GetType() == Office::msoOLEControlObject )
		{
			_bstr_t strName = pShape->GetName();
			if( _tcsstr((TCHAR*)strName, _T("andcef")) != NULL )
			{
				pStream->WriteInt(SHAPE_QUESTION);
				pStream->WriteString(_T(""));
				nCount++;
			}
		}
	}

	END_CATCH_COM_ERROR_RET(FALSE)

	pStream->ResetCursor();
	pStream->WriteInt(nIndex);
	pStream->WriteInt(nCount);
	pStream->ResetCursor();

	return TRUE;
}

BOOL CPPTController::SetFocusToCef(BOOL nReLoad)
{
	BEGIN_CATCH_COM_ERROR

	SlideShowViewPtr pSlideShow = GetSlideShowView();
	if( pSlideShow == NULL )
		return FALSE;

	_SlidePtr pSlide = pSlideShow->GetSlide();
	if( pSlide == NULL )
		return FALSE;

	ShapesPtr pShapes = pSlide->GetShapes();
	int nIndex = pSlide->GetSlideIndex();
	for(int i = 1; i <= pShapes->GetCount(); i++)
	{
		ShapePtr pShape = pShapes->Item(_variant_t((long)i));
		if( pShape == NULL )
			continue;

		if( pShape->GetType() != Office::msoOLEControlObject  )
			continue;

		_bstr_t strName = pShape->GetName();
		if( _tcsstr((TCHAR*)strName, _T("andcef")) == NULL )
			continue;

		OLEFormatPtr pOleFormat = pShape->GetOLEFormat();
		IDispatchPtr iDisPtr = pOleFormat->Object;

		CDandcef pp;
		pp.AttachDispatch(iDisPtr, FALSE);
		if (nReLoad){
			pp.SetFocus(nIndex);
		}else{
			pp.SetFocus(-1);
		}
		pp.ReleaseDispatch();


	}

	END_CATCH_COM_ERROR_RET(FALSE)
		return TRUE;
}
//
// send event to cef
//
BOOL CPPTController::SendEventToCef(LPCTSTR szEventName, LPCTSTR szEventData)
{
	BEGIN_CATCH_COM_ERROR

	SlideShowViewPtr pSlideShow = GetSlideShowView();
	if( pSlideShow == NULL )
		return FALSE;

	_SlidePtr pSlide = pSlideShow->GetSlide();
	if( pSlide == NULL )
		return FALSE;

	ShapesPtr pShapes = pSlide->GetShapes();
	for(int i = 1; i <= pShapes->GetCount(); i++)
	{
		ShapePtr pShape = pShapes->Item(_variant_t((long)i));
		if( pShape == NULL )
			continue;

		if( pShape->GetType() != Office::msoOLEControlObject  )
			continue;

		_bstr_t strName = pShape->GetName();
		if( _tcsstr((TCHAR*)strName, _T("andcef")) == NULL )
			continue;

		OLEFormatPtr pOleFormat = pShape->GetOLEFormat();
		IDispatchPtr iDisPtr = pOleFormat->Object;

		CDandcef pp;
		pp.AttachDispatch(iDisPtr, FALSE);

		pp.SendCallback2H5(szEventName, szEventData);
		pp.ReleaseDispatch();

	}

	END_CATCH_COM_ERROR_RET(FALSE)
	return TRUE;
}


//  insert by slide id

BOOL CPPTController::InsertPPTBySlideId( LPCTSTR szPath, int nStartSlideIdx, int nEndSlideIdx, int nSlideId, int nPlaceHolderId, BOOL* pCancel /*= NULL*/, NOTIFY_EVENT* pNotify /*= NULL*/ )
{
	BEGIN_CATCH_COM_ERROR

	_SlidePtr pSlide = GetSlideById(nSlideId);

	if ( pSlide == NULL )
		return -2;

	DelShape(pSlide, nPlaceHolderId);

	return InsertPPT(szPath, nStartSlideIdx, nEndSlideIdx, pSlide->GetSlideIndex(), pCancel, pNotify);

	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::InsertPictureBySlideId( LPCTSTR szPath, int nSlideId, int nPlaceHolderId, int nLeft /*= -1*/, int nTop /*= -1*/, int nWidth /*= -1*/, int nHeight /*= -1*/ )
{
	BEGIN_CATCH_COM_ERROR

	_SlidePtr pSlide = GetSlideById(nSlideId);

	if ( pSlide == NULL )
		return -1;

	DelShape(pSlide, nPlaceHolderId);

	return InsertPicture(szPath, nSlideId, nPlaceHolderId, nLeft, nTop, nWidth, nHeight, pSlide->GetSlideIndex());

	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::InsertHyperLinkPictureBySlideId( LPCTSTR szPath, int nSlideId, int nPlaceHolderId, LPCTSTR szHyperlink, int nLeft /*= -1*/, int nTop /*= -1*/, int nWidth /*= -1*/, int nHeight /*= -1 */ )
{
	BEGIN_CATCH_COM_ERROR

	_SlidePtr pSlide = GetSlideById(nSlideId);

	if ( pSlide == NULL )
		return -1;

	DelShape(pSlide, nPlaceHolderId);

	return InsertHyperLinkPicture(szPath, nSlideId, nPlaceHolderId, szHyperlink, nLeft, nTop, nWidth, nHeight, pSlide->GetSlideIndex());

	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::InsertVideoBySlideId( LPCTSTR szPath, int nSlideId, int nPlaceHolderId, int nLeft /*= -1*/, int nTop /*= -1*/, int nWidth /*= -1*/, int nHeight /*= -1*/ )
{
	BEGIN_CATCH_COM_ERROR

	_SlidePtr pSlide = GetSlideById(nSlideId);

	if ( pSlide == NULL )
		return -1;

	DelShape(pSlide, nPlaceHolderId);

	return InsertVideo(szPath, nLeft, nTop, nWidth, nHeight, pSlide->GetSlideIndex());

	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::InsertSwfBySlideId( LPCTSTR szPath, LPCTSTR strPicPath, LPCTSTR szTitle, int nSlideId, int nPlaceHolderId, LPCTSTR szSign /*= PLUGIN_SIGN*/ )
{
	BEGIN_CATCH_COM_ERROR

	_SlidePtr pSlide = GetSlideById(nSlideId);

	if ( pSlide == NULL )
		return -1;

	DelShape(pSlide, nPlaceHolderId);

	return InsertSwf(szPath, strPicPath, szTitle, szSign, pSlide->GetSlideIndex());

	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::InsertCefBySlideId( LPCTSTR szUrl, LPCTSTR szQuestionPath, int nSlideId, int nPlaceHolderId, int nLeft /*= -1*/, int nTop /*= -1*/, int nWidth /*= -1*/, int nHeight /*= -1*/ )
{
	BEGIN_CATCH_COM_ERROR

	_SlidePtr pSlide = GetSlideById(nSlideId);

	if ( pSlide == NULL )
		return -1;

	DelShape(pSlide, nPlaceHolderId);

	return InsertCef(szUrl, szQuestionPath, nLeft, nTop, nWidth, nHeight, pSlide->GetSlideIndex());

	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::InsertOLEObjectBySlideId( LPCTSTR szPath, int nSlideId, int nPlaceHolderId, int nLeft /*= -1*/, int nTop /*= -1*/, int nWidth /*= -1*/, int nHeight /*= -1*/ )
{
	BEGIN_CATCH_COM_ERROR

	_SlidePtr pSlide = GetSlideById(nSlideId);

	if ( pSlide == NULL )
		return -1;

	DelShape(pSlide, nPlaceHolderId);

	return InsertOLEObject(szPath, nLeft, nTop, nWidth, nHeight, pSlide->GetSlideIndex());

	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::Insert3DBySlideId(LPCTSTR szPath, LPCTSTR szThumbPath, int nSlideId, int nPlaceHolderId, LPCTSTR szSign /*= PLUGIN_3D*/)
{
	BEGIN_CATCH_COM_ERROR

	_SlidePtr pSlide = GetSlideById(nSlideId);

	if ( pSlide == NULL )
		return -1;

	DelShape(pSlide, nPlaceHolderId);

	return Insert3D(szPath, szThumbPath, pSlide->GetSlideIndex());

	END_CATCH_COM_ERROR_RET(FALSE)
}

BOOL CPPTController::InsertVRVideoBySlideId(LPCTSTR szPath, LPCTSTR szParam,LPCTSTR szThumbPath, int nSlideId, int nPlaceHolderId, LPCTSTR szSign /*= PLUGIN_VR*/)
{
	BEGIN_CATCH_COM_ERROR

	_SlidePtr pSlide = GetSlideById(nSlideId);

	if ( pSlide == NULL )
		return -1;

	DelShape(pSlide, nPlaceHolderId);

	return InsertVRVideo(szPath,szParam, szThumbPath, pSlide->GetSlideIndex());

	END_CATCH_COM_ERROR_RET(FALSE)
}
BOOL CPPTController::InsertVRExeBySlideId(LPCTSTR szPath, LPCTSTR szParam,LPCTSTR szThumbPath, int nSlideId, int nPlaceHolderId, LPCTSTR szSign /*= PLUGIN_VR*/)
{
	BEGIN_CATCH_COM_ERROR

		_SlidePtr pSlide = GetSlideById(nSlideId);

	if ( pSlide == NULL )
		return -1;

	DelShape(pSlide, nPlaceHolderId);

	return InsertVRExe(szPath,szParam, szThumbPath, pSlide->GetSlideIndex());

	END_CATCH_COM_ERROR_RET(FALSE)
}

string CPPTController::GetPPTPath()
{
	if (!m_pApplication)
		return "";

	string strPath = m_pApplication->GetPath();
	strPath += "\\";
	strPath += PPT_EXE_NAME;

	return strPath;
}



void CPPTController::GetNewApplication()
{
	BEGIN_CATCH_COM_ERROR
	m_pApplication.Release();
	m_pApplication = NULL;
	HRESULT hr = m_pApplication.CreateInstance(__uuidof(MSPpt::Application));
	END_CATCH_COM_ERROR
	
}

void CPPTController::CheckPPTRunTime()
{
	CResGuard::CGuard gd(m_Lock);

	if (CPPTController::m_bEverTryRepair)
	{
		return;
	}

	CPPTController::m_bEverTryRepair = true;
	Sleep(100);

	_ApplicationPtr ptrApplication;
	HRESULT hr = ptrApplication.CreateInstance(__uuidof(MSPpt::Application));

	if( hr != S_OK )
	{
		WRITE_LOG_LOCAL("_ApplicationPtr.CreateInstance fail. HRESULT:%x", (unsigned long int)hr);
		// 提示用户修复
		PPTVERSION pptVersion = GetSystemPPTVersion();
		if (pptVersion == PPTVER_UNKONW)
		{
			return;
		}

		DoPPTRunTimeRepaire(pptVersion);
		hr = ptrApplication.CreateInstance(__uuidof(MSPpt::Application));
	}

	if( hr != S_OK )
	{
		return;		// 无法修复
	}

	try 
	{
		if(ptrApplication != NULL )
		{
			PresentationsPtr  pPresentations	= ptrApplication->GetPresentations();
			if ( pPresentations == NULL )
			{
				return;
			}

			tstring  strVer = ptrApplication->GetVersion();
			CPPTController::m_fPPTVersion = (float)_tstof(strVer.c_str());
		}

		Office::_CommandBarsPtr pCommandBar;
		pCommandBar = ptrApplication->GetCommandBars();
	}
	catch(_com_error& e)
	{
		DoPPTRunTimeRepaire((PPTVERSION)GetPPTVersion());
	}

}

void CPPTController::DoPPTRunTimeRepaire(PPTVERSION pptVersion)
{
	TCHAR tszPath[MAX_PATH + 1] = { 0 };
	::GetCurrentDirectory(MAX_PATH, tszPath);

	TCHAR szCmd[1024] = {0};

	if (pptVersion == PPTVER_2007)
	{
		wsprintf(szCmd, _T("reg import \"%s\\bin\\office\\Office2007修复.dat\""), tszPath);
	}
	else if (pptVersion == PPTVER_2010)
	{
		wsprintf(szCmd, _T("reg import \"%s\\bin\\office\\Office2010修复.dat\""), tszPath);
	}
	else if (pptVersion == PPTVER_2013)
	{
		wsprintf(szCmd, _T("reg import \"%s\\bin\\office\\Office2013修复.dat\""), tszPath);
	}

	if (szCmd[0] != 0)
	{
		system(szCmd);
	}
}

PPTVERSION CPPTController::GetSystemPPTVersion()
{
	int nRet = UIMessageBox(NULL, _T("加载Microsoft Office组件时出错，请选择当前已安装的Office版本进行修复！"), _T("101教育PPT"), _T("2007, 2010, 2013,忽略"), CMessageBoxUI::enMessageBoxTypeError, IDCANCEL);
	if (nRet == ID_MSGBOX_BTN)
	{
		return PPTVER_2007;
	}
	else if (nRet == ID_MSGBOX_BTN+1)
	{
		return PPTVER_2010;
	}
	else if (nRet == ID_MSGBOX_BTN+2)
	{
		return PPTVER_2013;
	}

	return PPTVER_UNKONW;
}

//设置文字的超链接
BOOL CPPTController::SetTextHyperlink( tstring strDesTxt )
{
	BEGIN_CATCH_COM_ERROR
	if ( m_pApplication == NULL )
		return FALSE;
	
	_SlidePtr pSlide = GetActiveSlider();
	if ( pSlide == NULL )
		return FALSE;

	ShapesPtr pShapes = pSlide->GetShapes();
	if ( pShapes == NULL )
		return FALSE;

	BOOL bFind	= FALSE;
	ShapePtr pShape = NULL;
	for ( int i=1; i<=pShapes->GetCount(); i++)
	{
		pShape = pShapes->Item(_variant_t(long(i)));

		if  (pShape == NULL || pShape->GetHasTextFrame() == Office::msoFalse )
			continue;

		TextFramePtr pTextFrame = pShape->GetTextFrame();

		if ( pTextFrame == NULL || pTextFrame->GetHasText() == Office::msoFalse )
			continue;

		TextRangePtr pTextRange = pTextFrame->GetTextRange();

		TextRangePtr pTextFindRange = pTextRange->Find(strDesTxt.c_str(), 0, Office::msoFalse, Office::msoFalse);

		if ( pTextFindRange == NULL )
			continue;

		HyperlinkPtr pHyperlink = NULL;

		bFind = TRUE;

		tstring strTip = _T("生字卡:");
		strTip += strDesTxt;

		while ( pTextFindRange != NULL )
		{
			ActionSettingsPtr pActionSettings =  pTextFindRange->GetActionSettings();

			pHyperlink = pActionSettings->Item(ppMouseClick)->GetHyperlink();

			pHyperlink->PutAddress(_T("outlookfeeds://123123/"));
			pHyperlink->PutScreenTip(strTip.c_str());
			
			pTextFindRange = pTextRange->Find(strDesTxt.c_str(), pTextFindRange->GetStart() + pTextFindRange->GetLength() - 1, Office::msoFalse, Office::msoFalse);

		}
	}

	return bFind;

	END_CATCH_COM_ERROR_RET(FALSE)
}

// 获取SlideShowView的当前显示页号
int CPPTController::GetCurrentShowPosition()
{
	int nShowPage = -1;
	CResGuard::CGuard gd(m_Lock);

	BEGIN_CATCH_COM_ERROR

	SlideShowViewPtr pSlideShowView = GetSlideShowView();
	if( pSlideShowView == NULL )
		return FALSE;
	nShowPage = pSlideShowView->CurrentShowPosition;
	return nShowPage;

	END_CATCH_COM_ERROR_RET(FALSE)

}

void CPPTController::UpdatePPTHwnd( HWND hWnd )
{	  
	::ShowWindow(hWnd, SW_SHOW);
	if ( m_PPTHwnd != hWnd )
		::ShowWindow(m_PPTHwnd, SW_HIDE);

	m_PPTHwnd		= hWnd;		
	m_mapHwnd[hWnd] = TRUE;	
	
	CPanelInnerDlg::AdjustMainPanel();
}
