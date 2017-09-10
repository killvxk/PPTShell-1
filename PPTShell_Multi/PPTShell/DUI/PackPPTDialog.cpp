#include "stdafx.h"
#include "PackPPTDialog.h"
#include "PPTControl/PPTPack.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "DUI/ProgressSaving.h"
#include "NDCloud/NDCloudUser.h"
#include "GUI/MainFrm.h"

CPackPPTDialogUI::CPackPPTDialogUI()
{
}


CPackPPTDialogUI::~CPackPPTDialogUI()
{
	
}

LPCTSTR CPackPPTDialogUI::GetWindowClassName() const
{
	return _T("PackPPTDialog");
}

CDuiString CPackPPTDialogUI::GetSkinFile()
{
	return _T("TopBar\\PackPPTUI.xml");
}

CDuiString CPackPPTDialogUI::GetSkinFolder()
{
	return CDuiString(_T("skins"));
}

void CPackPPTDialogUI::InitWindow()
{
	m_pLabel = dynamic_cast<CLabelUI *>(m_PaintManager.FindControl(_T("packStatus")));
	m_pProgress = dynamic_cast<CProgressUI *>(m_PaintManager.FindControl(_T("packProgress")));

//	m_WndShadow.Create(m_hWnd);
//	m_pLabel->OnDestroy += MakeDelegate(this, &CPackPPTDialogUI::OnWindowDestroy);
//	
}

void CPackPPTDialogUI::Pack( tstring strFile )
{
	m_nProgressCount = 0;
	m_nTotal = 100;

	m_pProgress->SetValue(m_nProgressCount);
	m_pLabel->SetText(_T("正在打包中..."));
	m_dwTaskId = PPTPack::GetInstance()->Packing(strFile.c_str(), MakeDelegate(this, &CPackPPTDialogUI::OnSetValue),
		MakeDelegate(this, &CPackPPTDialogUI::OnPackProgress), MakeDelegate(this, &CPackPPTDialogUI::OnPackCompleted));
}

void CPackPPTDialogUI::UnPack( tstring strFile )
{
	m_nProgressCount = 0;
	m_nTotal = 100;

	m_pProgress->SetValue(m_nProgressCount);
	m_pLabel->SetText(_T("正在初始化，请稍后..."));
	m_dwTaskId = PPTPack::GetInstance()->UnPacking(strFile.c_str(), MakeDelegate(this, &CPackPPTDialogUI::OnSetValue),
		MakeDelegate(this, &CPackPPTDialogUI::OnPackProgress), MakeDelegate(this, &CPackPPTDialogUI::OnUnPackCompleted));
}

bool CPackPPTDialogUI::OnPackProgress( void * pParam )
{
	//PackNotify* pPackNotify =  (PackNotify*)pParam;
	
	m_nProgressCount++;
	m_pProgress->SetValue(m_nProgressCount);
	return true;
}

bool CPackPPTDialogUI::OnSetValue( void * pParam )
{
	PackNotify* pPackNotify =  (PackNotify*)pParam;
	DWORD dwRet = pPackNotify->dwRet;
	m_nTotal = dwRet + 1;
	m_pProgress->SetMaxValue(m_nTotal);

	return true;
}

bool CPackPPTDialogUI::OnPackCompleted( void * pParam )
{
	m_pProgress->SetValue(m_nTotal);

	PackNotify* pPackNotify =  (PackNotify*)pParam;
	DWORD dwRet = pPackNotify->dwRet;
	if(dwRet != -1)
	{
//		m_pLabel->SetText(_T("打包失败"));
		m_dwRet = dwRet;
		m_strFailPath = pPackNotify->strFailPath;

		SetTimer(GetHWND(), 100, 100, (TIMERPROC)&CPackPPTDialogUI::TimerProcComplete);
	}
	else
	{
		m_pLabel->SetText(_T("打包完成"));

		m_strFilePath = pPackNotify->strPath;
		SetWindowLongPtr(m_hWnd, GWL_USERDATA, (LONG_PTR)this);

		SetTimer(GetHWND(), 101, 3000, (TIMERPROC)&CPackPPTDialogUI::TimerProcComplete);
	}
	
	
	return true;
}


bool CPackPPTDialogUI::OnUnPackCompleted( void * pParam )
{
	PackNotify* pPackNotify =  (PackNotify*)pParam;
	DWORD dwRet = pPackNotify->dwRet;
	if(dwRet == 51)
	{
		m_pLabel->SetText(_T("文件已打开"));
		m_pProgress->SetValue(m_nTotal);
		SetTimer(GetHWND(), 102, 3000, (TIMERPROC)&CPackPPTDialogUI::TimerProcComplete);
	}
	else if(dwRet != -1)
	{
		m_pLabel->SetText(_T("文件打开失败"));
		m_pProgress->SetValue(m_nTotal);
		SetTimer(GetHWND(), 102, 3000, (TIMERPROC)&CPackPPTDialogUI::TimerProcComplete);
	}
	else
	{
		tstring strPath = pPackNotify->strPath;
		OpenPPTByThread(strPath.c_str(), FALSE);
		ShowWindow(false);
	}

	return true;
}


void CPackPPTDialogUI::Init( CRect rect )
{
//	int nMainWindowWidth= 488;
//	int nMainWindowHeight = 35;
//	::MoveWindow(GetHWND(), rect.left + (rect.Width() - nMainWindowWidth)/2, rect.top + (rect.Height() - nMainWindowHeight)/2, nMainWindowWidth, nMainWindowHeight, TRUE);
	MoveWindow(GetHWND(), rect.left, rect.top, rect.Width(), rect.Height(), FALSE);
//	m_rcRect = rect;

//	m_WndShadow.SetMaskSize(m_rcRect);
}

void CPackPPTDialogUI::TimerProcComplete( HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime )
{
	CPackPPTDialogUI* pDlg = (CPackPPTDialogUI*)GetWindowLongPtr(hwnd, GWL_USERDATA);

	if(idEvent == 100)
	{
		KillTimer(hwnd, 100);
		//pDlg->ShowWindow(false);
		pDlg->Close();

		
		TCHAR szTip[1024];
		int nRet;
		if(pDlg->m_dwRet == 50)
		{
			sprintf(szTip, _T("%s\r\n%s"), _STR_PACK_FAIL_KNOWN, pDlg->m_strFailPath.c_str());
			nRet = UIMessageBox(NULL, _STR_PACK_FAIL_SAVE_KNOWN, 0xFF10B0B6, 190100, szTip, 0xFF000001, 120000, _T("101教育PPT"), _T("重试,取消"), CMessageBoxUI::enMessageBoxTypeError, IDCANCEL);
		}
		else
			nRet = UIMessageBox(NULL, _STR_PACK_FAIL_SAVE_UNKNOWN, 0xFF10B0B6, 190100, _STR_PACK_FAIL_UNKNOWN, 0xFF000001, 120000, _T("101教育PPT"), _T("重试,取消"), CMessageBoxUI::enMessageBoxTypeError, IDCANCEL);
		
		if( nRet == ID_MSGBOX_BTN )
		{
			if ( GetPPTController()->IsInit() )
				IsPPTChangedReadOnlyByThread(FALSE, PPTC_PACKAGE);
		}
	}
	else if(idEvent == 101)
	{

		tstring strPath = pDlg->GetPath();
		if(!strPath.empty())
		{
			TCHAR szParam[MAX_PATH * 4] = {0};
			_tcscpy(szParam, _T("/e,/select,\""));
			_tcscat(szParam, strPath.c_str());
			_tcscat(szParam, _T("\""));
			ShellExecute(NULL,_T("open"),_T("explorer"),szParam,NULL,SW_SHOW );
		}
		
		KillTimer(hwnd, 101);
		pDlg->Close();

		if ( NDCloudUser::GetInstance()->GetUserId() != 0 )
		{
			int nRet = UIMessageBox(NULL, _T("是否保存当前课件到“我的网盘”?"), _T("101教育PPT"), _T("是,否"), CMessageBoxUI::enMessageBoxTypeInfo, IDNO);

			if ( nRet == ID_MSGBOX_BTN )
			{
				
				((CMainFrame*)(AfxGetApp()->m_pMainWnd))->m_bUploadingOnClose = TRUE;
				CProgressSavingUI* pSaveDialog = new CProgressSavingUI;
				pSaveDialog->ShowWindow();
				pSaveDialog->Start(strPath.c_str(), &MakeDelegate(pDlg, &CPackPPTDialogUI::OnSaveToDBankCompleted));
			}
		}
		//pDlg->ShowWindow(false);
	}
	else if(idEvent == 102)
	{
		KillTimer(hwnd, 100);
		//pDlg->ShowWindow(false);
		pDlg->Close();
	}
}

LRESULT CPackPPTDialogUI::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if (WM_SYSKEYDOWN == uMsg && VK_F4 == wParam)
	{
		PPTPack::GetInstance()->CancelPack(m_dwTaskId);
		KillTimer(GetHWND(), 100);
		ShowWindow(false);
		return S_FALSE;
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

bool CPackPPTDialogUI::OnSaveToDBankCompleted( void* param )
{
	((CMainFrame*)(AfxGetApp()->m_pMainWnd))->m_bUploadingOnClose = FALSE;

	return true;
}

// 
// bool CPackPPTDialogUI::OnWindowDestroy( void * Param )
// {
// 	
// 	return true;
// }
