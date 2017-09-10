
// PPTShellView.cpp : implementation of the CPPTShellView class
//

#include "stdafx.h"
#include "PPTShell.h"
#include "CntrItem.h"
#include "MainFrm.h"
#include "PPTShellDoc.h"
#include "PPTShellView.h"
#include "NDCloud/NDCloudAPI.h"
#include "DUI/Introduction.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "Config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPPTShellView

IMPLEMENT_DYNCREATE(CPPTShellView, CView)

BEGIN_MESSAGE_MAP(CPPTShellView, CView)
	ON_WM_SIZE()
	ON_MESSAGE(WM_INITPPT, &CPPTShellView::OnCreateOleInView)
	ON_MESSAGE(WM_INITSHOWGUIDE, &CPPTShellView::OnShowGuide)
END_MESSAGE_MAP()

// CPPTShellView construction/destruction

CPPTShellView::CPPTShellView()
{
	// TODO: add construction code here
	m_pUntitledWnd			= NULL;
	m_bFirst				= TRUE;
	m_bUntitledWindowShow	= FALSE;
}

CPPTShellView::~CPPTShellView()
{
}

BOOL CPPTShellView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CPPTShellView drawing
void CPPTShellView::OnDraw(CDC* /*pDC*/)
{
	CPPTShellDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}

void CPPTShellView::CreateUntitleWindow()
{
	if(m_pUntitledWnd == NULL)
	{
		m_pUntitledWnd = new CUntitledWindow;
		m_pUntitledWnd->CreateDuiWindow(AfxGetMainWnd()->GetSafeHwnd(), _T("UntitledWindow"), UI_WNDSTYLE_CHILD);
	}else
	{
		m_pUntitledWnd->ShowWindow(true);
		m_pUntitledWnd->SetNewButtonEnabled(true);
	}

	m_bUntitledWindowShow = TRUE;
	
	::SetWindowText(AfxGetMainWnd()->m_hWnd, _T("101教育PPT"));

	CRect rt;
	GetClientRect(rt);
	::SetWindowPos(m_pUntitledWnd->GetHWND(), NULL, 0, 0, rt.Width()+3, rt.Height()+3, SWP_DRAWFRAME);
	::SetWindowPos(AfxGetMainWnd()->GetSafeHwnd(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	SetActiveWindow();
	//BringWindowToTop();

}
//创建ole
LRESULT CPPTShellView::OnCreateOleInView(WPARAM wParam, LPARAM lParam)
{
	CPPTShellDoc* pDoc = GetDocument();

	if (pDoc == NULL) 
		return FALSE;

//	CPPTShellCntrItem* pItem = new CPPTShellCntrItem(pDoc);
	BeginWaitCursor();
	//GUID clsid =  {0xcf4f55f4,0x8f87,0x4d47,0x80,0xbb,0x58,0x08,0x16,0x4b,0xb3,0xf8};
	GUID clsid =  {0xdc020317,0xe6e2,0x4a62, 0xb9,0xfa,0xb3,0xef,0xe1,0x66,0x26,0xf4};
	TRY
	{
		if ( CPPTController::IsInitFailed() )
			CPPTController::CheckPPTRunTime();

		CPPTShellApp* pApp = (CPPTShellApp*)AfxGetApp();
		tstring strPPTPath = pApp->GetCurrentPPTPath();
		theApp.SetCurrentPPTPath(_T(""));
		
		//BOOL bOPen = FALSE;
		if ( ! strPPTPath.empty() )
		{
			((CMainFrame*)pApp->m_pMainWnd)->OpenDoc(strPPTPath);
			//if ( ((CMainFrame*)pApp->m_pMainWnd)->IsNdpx(strPPTPath) )
			//{
			//	GetPPTController()->ShowPPTWnd(pApp->GetMainWnd());
			//	((CMainFrame*)pApp->m_pMainWnd)->OpenNdpx(strPPTPath);
			//}
			//else
			//{
			//	((CMainFrame*)pApp->m_pMainWnd)->OpenDoc(strPPTPath);
			//	//pDoc->OnOpenDocument(strPPTPath.c_str());
			//}
				
		}
		else
		{
			if ( wParam == 0  && !pApp->IsFromNetwork() )
				pDoc->OnNewDocument();
			else
			{
				BOOL bRet = GetPPTController()->ShowPPTWnd(pApp->GetMainWnd());
			}
				
			//CreateUntitleWindow();  //zcs   2015-11-24
		}
		//else if ( pItem->CreateNewItem(clsid) )
		//{
		//	pItem->DoVerb(OLEIVERB_SHOW, this);
		//	pDoc->UpdateAllViews(NULL);
		//	pDoc->SetModifiedFlag(FALSE);

		//	Sleep(100);
		//	CPPTController pPPTControl;
		//	pPPTControl.SetSaved(TRUE);
		//	
		//}

	}
	CATCH(CException, e)
	{
	}
	END_CATCH

	EndWaitCursor();

	return 1;
}

LRESULT CPPTShellView::OnShowGuide(WPARAM wParam, LPARAM lParam)
{
	//
	if( m_bFirst && NDCloudLoadTrees() == FALSE )
	{
		::PostMessage( ((CMainFrame *)AfxGetMainWnd())->getMainPane()->GetMainControlDlg()->GetSafeHwnd(), WM_CREATE_GUIDE,GUIDE_REMOTE_OPEN,NULL);
	}

	//增加更新完成界面
	if(g_Config::GetInstance()->GetUpdatedStatus() == eUpdateType_Update_Success)
	{
		tstring strPath = GetLocalPath();
		strPath += _T("\\Introduction\\");
		tstring strListPath = strPath ;
		strListPath += _T("list.txt");
		tstring strDefaultImagePath = GetLocalPath() ;
		strDefaultImagePath += _T("\\Skins\\Introduction\\Default.png");

		vector<tstring> vecImagesList;

		CStdioFile File;
		//TCHAR szLine[MAX_PATH] ;
		CString szLine;
		TCHAR szFile[MAX_PATH] ;
		if(File.Open(strListPath.c_str(), CFile::modeRead))
		{
			while(File.ReadString(szLine))
			{
				_stprintf(szFile, _T("%s%s"), strPath.c_str(), szLine.GetString());
				if(GetFileAttributes(szFile) != INVALID_FILE_ATTRIBUTES)
					vecImagesList.push_back(szFile);
			}
			File.Close();
		}

		

		if(!vecImagesList.empty())
		{
			CIntroductionDialogUI *pDialogUI = IntroductionDialogUI::GetInstance();
			if(pDialogUI && pDialogUI->GetHWND() == NULL)
			{
				pDialogUI->Create(GetSafeHwnd(), _T("GuideWindow"), WS_POPUP, 0, 0, 0, 0, 0);
				CRect rect;
				AfxGetApp()->m_pMainWnd->GetWindowRect(rect);
				pDialogUI->LoadImages(vecImagesList);
				pDialogUI->Init(rect);
				pDialogUI->StartAnim();
				pDialogUI->ShowWindow(true);
			}
		}
		else{
			if(GetFileAttributes(strDefaultImagePath.c_str()) != INVALID_FILE_ATTRIBUTES)
			{
				CIntroductionDialogUI *pDialogUI = IntroductionDialogUI::GetInstance();
				if(pDialogUI && pDialogUI->GetHWND() == NULL)
				{
					pDialogUI->Create(GetSafeHwnd(), _T("GuideWindow"), WS_POPUP, 0, 0, 0, 0, 0);
					CRect rect;
					AfxGetApp()->m_pMainWnd->GetWindowRect(rect);
					pDialogUI->LoadImage(strDefaultImagePath);
					pDialogUI->Init(rect);
					pDialogUI->StartAnim();
					pDialogUI->ShowWindow(true);
				}
			}
		}
	}
	
	

	m_bFirst = FALSE;

	return 1;
}

// CPPTShellView diagnostics

#ifdef _DEBUG
void CPPTShellView::AssertValid() const
{
	CView::AssertValid();
}

void CPPTShellView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPPTShellDoc* CPPTShellView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPPTShellDoc)));
	return (CPPTShellDoc*)m_pDocument;
}
#endif //_DEBUG


// CPPTShellView message handlers

BOOL CPPTShellView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class
	return CView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void CPPTShellView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	// TODO: Add your specialized code here and/or call the base class

	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CPPTShellView::OnActivateFrame(UINT nState, CFrameWnd* pDeactivateFrame)
{
	// TODO: Add your specialized code here and/or call the base class

	CView::OnActivateFrame(nState, pDeactivateFrame);
}

void CPPTShellView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)InitThread, this, 0, NULL);
	//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ClosePPT2013TipDialogThread, this, 0, NULL);
}

void CPPTShellView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	//COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	//if (pActiveItem != NULL)
	//{
	//	pActiveItem->SetItemRects();
	//}
	// TODO: Add your message handler code here
	if ( m_pUntitledWnd != NULL )
	{
		CRect rt;
		GetClientRect(rt);
		::SetWindowPos(m_pUntitledWnd->GetHWND(), NULL, 0, 0, rt.Width()+5, rt.Height()+5, SWP_DRAWFRAME);
	}
}

DWORD WINAPI CPPTShellView::InitThread(LPARAM lParam)
{
	CPPTShellView* pThis = (CPPTShellView*)lParam;

	int nCount = 0;
	CPPTController* pController = GetPPTController();
	//BOOL bKill = pController->KillCrashPowerPoint();

	//if ( !bKill )
	//	 nCount = pController->GetPresentationCount();

	Sleep(100);
	::PostMessage(pThis->GetSafeHwnd(), WM_INITPPT, nCount, 0);
	Sleep(300);
	::PostMessage(pThis->GetSafeHwnd(), WM_INITSHOWGUIDE, 0, 0);
	::SetWindowPos(AfxGetMainWnd()->GetSafeHwnd(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	
	if ( CPPTController::GetPPTVersion() >= PPTVER_2013 )
		pThis->ClosePPT2013TipDialogThread();

	return 0;
}

BOOL CPPTShellView::ClosePPT2013TipDialogThread()
{
	int nCount = 0;
	while ( nCount++ < 60)
	{
		Sleep(100);
		HWND hWndTipDialog = ::FindWindow(NULL, _T("Microsoft PowerPoint"));
		if ( hWndTipDialog != NULL )
		{
			HWND hCheckWnd = ::FindWindowEx(hWndTipDialog, NULL, _T("Button"), _T("不再显示此消息(&D)"));
			if ( hCheckWnd )
			{
				::PostMessage(hCheckWnd, BM_CLICK, 0, 0);
			}

			HWND hNoWnd = ::FindWindowEx(hWndTipDialog, NULL, _T("Button"), _T("否(&N)"));
			if ( hNoWnd )
			{
				::PostMessage(hNoWnd, BM_CLICK, 0, 0);
			}

			return TRUE;
		}
	}
	return FALSE;
}

void CPPTShellView::ShowUntitledWindow( bool bShow )
{
	if (m_pUntitledWnd != NULL)
	{
		m_pUntitledWnd->ShowWindow(bShow);
		m_pUntitledWnd->SetNewButtonEnabled(true);
		m_bUntitledWindowShow = bShow;
	}
}
