
// PPTShellDoc.cpp : implementation of the CPPTShellDoc class
//

#include "stdafx.h"
#include "PPTShell.h"
#include "PPTShellDoc.h"
#include "PPTShellView.h"
#include "MainFrm.h"
#include "PPTControl/PPTControllerManager.h"
#include "EventCenter/EventDefine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPPTShellDoc

IMPLEMENT_DYNCREATE(CPPTShellDoc, CDocument)

BEGIN_MESSAGE_MAP(CPPTShellDoc, CDocument)
END_MESSAGE_MAP()


// CPPTShellDoc construction/destruction

CPPTShellDoc::CPPTShellDoc()
{
	// TODO: add one-time construction code here

}

CPPTShellDoc::~CPPTShellDoc()
{
	//不能再view或者frame中杀，要等待pptcontrol析构完成后再杀，否则概率bug
	KillExeCheckParentPid(PPT_EXE_NAME);
}

BOOL CPPTShellDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// Close
	CMainFrame* pMainFrame	= (CMainFrame*)AfxGetMainWnd();
	CPPTShellView* pView	= (CPPTShellView*)pMainFrame->GetActiveView();

	if ( pView == NULL ) return TRUE;

	GetPPTController()->ShowPPTWnd(pMainFrame);
	NewPPTByThread();
	pView->ShowUntitledWindow(FALSE);

	pMainFrame->GenarateOperationer();
	return TRUE;
}

BOOL CPPTShellDoc::OnOpenDocument(LPCTSTR lpszPathName, BOOL bReadOnly)
{
	// Close
	CMainFrame* pMainFrame	= (CMainFrame*)AfxGetMainWnd();
	CPPTShellView* pView	= (CPPTShellView*)pMainFrame->GetActiveView();
	//COleClientItem* pActiveItem = GetInPlaceActiveItem((CWnd*)pView);

	pMainFrame->EnablePPTOperationButton(TRUE);

	try
	{
		GetPPTController()->ShowPPTWnd(pMainFrame);

		OpenPPTByThread(lpszPathName, bReadOnly);

		pView->ShowUntitledWindow(FALSE);

		SetModifiedFlag(FALSE);
		//CDocument::SetPathName(lpszPathName);

		CPPTShellApp* pApp	= (CPPTShellApp*)AfxGetApp();
		pApp->SetCurrentPPTFileName(CDocument::GetTitle());

		pMainFrame->GenarateOperationer();
	}
	catch (...)
	{
	}

	return TRUE;
}

BOOL CPPTShellDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	//CMainFrame* pMainFrame		= (CMainFrame*)AfxGetMainWnd();
	//BOOL bSaveSuccess = SavePPTByThread(lpszPathName);
	//bSaveSuccess = pMainFrame->GetPPTController()->SaveAs(lpszPathName);
	//CPPTShellView* pView		= (CPPTShellView*)pMainFrame->GetActiveView();
	//COleClientItem* pActiveItem = GetInPlaceActiveItem((CWnd*)pView);

	//if ( pActiveItem != NULL )
	//{
	//	_PresentationPtr pPrePtr = pActiveItem->m_lpObject;
	//	pPrePtr->SaveAs(lpszPathName, ppSaveAsDefault, Office::msoTriStateMixed);
	//}

	return TRUE;//2015.10.22 cws
}

void CPPTShellDoc::OnCloseDocument()
{
	//CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	//pMainFrame->GetPPTController()->Close();
	//CPPTShellView* pView = (CPPTShellView*)pMainFrame->GetActiveView();

	//COleClientItem* pActiveItem = GetInPlaceActiveItem((CWnd*)pView);
	//if( pActiveItem != NULL )
	//{
	//	_PresentationPtr pPrePtr = pActiveItem->m_lpObject;
	//	pPrePtr->Close();
	//	pActiveItem->Close();
	//}

	CDocument::OnCloseDocument();
	//ExitProcess(0);
}


// CPPTShellDoc serialization

void CPPTShellDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

BOOL CPPTShellDoc::NewDocument()
{
	return CDocument::OnNewDocument();
}


// CPPTShellDoc diagnostics

#ifdef _DEBUG
void CPPTShellDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPPTShellDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}

#endif //_DEBUG


// CPPTShellDoc commands
