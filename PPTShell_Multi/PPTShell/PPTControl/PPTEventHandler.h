//===========================================================================
// FileName:				PPTEventHandler.h
// 
// Desc:				 
//============================================================================
#pragma once
#include "Util/Singleton.h"

class CPPTEventHandler : public CCmdTarget
{
	DECLARE_DYNAMIC(CPPTEventHandler)

public:
	CPPTEventHandler();
	virtual ~CPPTEventHandler();

	virtual void OnFinalRelease();

protected:
	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	//DECLARE_INTERFACE_MAP()

public:
	void StartMainWindowMoniter(LPDISPATCH Pres);
	void ShowUntitledWindow();
	void MainWindowMoniter();
	void DealAbnormalMsgBox();
	static DWORD WINAPI MainWindowMoniterThread(LPVOID lparam);
	void AttatchPPTWindow(HWND hPptWnd, CWnd* pParent);
	BOOL IsPPTActive();

	void WindowSelectionChange(LPDISPATCH Pres);
	void WindowBeforeRightClick(LPDISPATCH Pres, VARIANT_BOOL* Cancel);
	void WindowBeforeDoubleClick(LPDISPATCH Pres, VARIANT_BOOL* Cancel);
	void PresentationClose ( LPDISPATCH Pres);
	void PresentationSave( LPDISPATCH Pres);
	void PresentationOpen( LPDISPATCH Pres);
	void NewPresentation( LPDISPATCH Pres);
	void PresentationNewSlide( LPDISPATCH Pres);
	void WindowActivate( LPDISPATCH Pres,LPDISPATCH Wn);
	void WindowDeactivate(LPDISPATCH Pres, LPDISPATCH Wn);
	void SlideShowBegin(LPDISPATCH Wn);
	void SlideShowNextBuild( LPDISPATCH Wn);
	void SlideShowNextSlide( LPDISPATCH Wn);
	void SlideShowEnd( LPDISPATCH Pres);
	void PresentationPrint(LPDISPATCH Pres);
	void SlideSelectionChanged(LPDISPATCH SldRange);
	// 	void ColorSchemeChanged(LPDISPATCH SldRange);
	void PresentationBeforeSave(LPDISPATCH Pres, VARIANT_BOOL * Cancel);
	// 	void SlideShowNextClick(LPDISPATCH Wn, LPDISPATCH nEffect);
	void AfterNewPresentation(LPDISPATCH Pres);
	void AfterPresentationOpen(LPDISPATCH Pres);
	// 	void PresentationSync(LPDISPATCH Pres, long SyncEventType);
	void SlideShowOnNext(LPDISPATCH Wn);
	//void SlideShowOnPrevious(LPDISPATCH Wn);
	//void PresentationPrint(LPDISPATCH Pres);

	void	TerminateMainMoniterThread();
	tstring GenerateGuid();
	tstring Get3DPath( LPDISPATCH pSlide );
	tstring GetVRPath( LPDISPATCH pSlide );
	tstring GetExercisesPath( LPDISPATCH pSlide );
	BOOL	SetFocuesCef( LPDISPATCH pSlide);
	void	SetPPTHwnd(HWND hWnd) {	m_PPTHwnd = hWnd;	}

public:
	DWORD		m_dwCookie;	
	bool		m_bNeedCheck;
	static bool	m_bAppShow;
	static int	m_nPPTCount;

protected:
	static DWORD WINAPI WaitRecoverySaveDialogThread(LPVOID lParam);
	void				WaitForRecoverySaveDialogClose();
	void				AddPPTCount();
	
protected:
	HANDLE			m_MoniterThread;
	HANDLE			m_hWaitRecoveryThread;

	HWND			m_PPTHwnd;

	BOOL			m_bIcrPlayer;

	BOOL			m_bInSidePPTPlay;
	BOOL			m_bRecovery;
	BOOL			m_bNewPPT;

	tstring			m_strPPTTitleName;

};

typedef Singleton<CPPTEventHandler>	PPTEventHandler;