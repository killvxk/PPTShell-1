#include "stdafx.h"
#include "UpdateDialog.h"
#include "Update/UpdateOperation.h"
#include "Config.h"
#include "EventCenter/EventDefine.h"

#define	TIMER_EVENT_TIMINGUPDATE	200


CUpdateDialogUI::CUpdateDialogUI()
{
	m_nTimingUpdateCount = 0;
	m_pUpdateInfoLabel = NULL;
}
// 
// void CUpdateDialogUI::SetRect( CRect &rect )
// {
// 	m_mainRect		= rect;
// }

CUpdateDialogUI::~CUpdateDialogUI()
{
	KillTimer(this->GetHWND(), TIMER_EVENT_TIMINGUPDATE);
}

LPCTSTR CUpdateDialogUI::GetWindowClassName() const
{
	return _T("UpdateDialog");
}

CDuiString CUpdateDialogUI::GetSkinFile()
{
	return _T("TopBar\\update.xml");
}

CDuiString CUpdateDialogUI::GetSkinFolder()
{
	return CDuiString(_T("skins"));
}


void CUpdateDialogUI::InitWindow()
{
	// enlarge window

	m_pUpdateInfoLayout = static_cast<CVerticalLayoutUI *>(m_PaintManager.FindControl(_T("updateInfoLayout")));
	m_pUpdateInfoTitle	 = static_cast<CLabelUI *>(m_PaintManager.FindControl(_T("updateInfoTitle")));
	m_pAllFilesSizeLabel = static_cast<CLabelUI *>(m_PaintManager.FindControl(_T("allFilesSize")));
// 
// 	m_WndShadow.SetMaskSize(m_mainRect);
// 	m_WndShadow.Create(m_hWnd);
	m_pMainLayoutUI = dynamic_cast<CVerticalLayoutUI *>(m_PaintManager.FindControl(_T("mainLayout")));
	m_pMainWindowLayoutUI = dynamic_cast<CVerticalLayoutUI *>(m_PaintManager.FindControl(_T("mainWindowLayout")));

	m_pHasUpdateLayout = static_cast<CVerticalLayoutUI *>(m_PaintManager.FindControl(_T("hasUpdateLayout")));
	m_pNewestLayout = static_cast<CVerticalLayoutUI *>(m_PaintManager.FindControl(_T("newestLayout")));
	m_pUpdateProcessingLayout = static_cast<CVerticalLayoutUI *>(m_PaintManager.FindControl(_T("updateProcessingLayout")));

	m_pLocalVersion		 = static_cast<CLabelUI *>(m_PaintManager.FindControl(_T("LocalVersion")));

	m_pPlaynewBtn = dynamic_cast<CButtonUI *>(m_PaintManager.FindControl(_T("playnewBtn")));

	m_pUpdateInfoLabel	 = new CLabelMultiUI;
	m_pUpdateInfoLayout->Add(m_pUpdateInfoLabel);

	m_pCurrentProgress = static_cast<CProgressUI *>(m_PaintManager.FindControl(_T("currentProgress")));
	m_pTotalProgress = static_cast<CProgressUI *>(m_PaintManager.FindControl(_T("totalProgress")));
	m_pUpdateInfoParamLabel = static_cast<CLabelUI *>(m_PaintManager.FindControl(_T("updateInfoParamLabel")));
	m_pUpdateProcessingLabel = static_cast<CLabelUI *>(m_PaintManager.FindControl(_T("updateProcessingLabel"))); 
}

CControlUI* CUpdateDialogUI::CreateControl(LPCTSTR pstrClass)
{
	return NULL;
}

void CUpdateDialogUI::setDlgHwnd(HWND Hwnd)
{
	m_mainHwnd = Hwnd;
}

void CUpdateDialogUI::setUpdateLog(LPCTSTR szUpdateLog)
{
	CRect rc(0,0,550,25);

	int nFont = 140000;
	CRenderEngine::DrawText(::GetDC(m_hWnd), &m_PaintManager, rc, szUpdateLog, 0, nFont, DT_CALCRECT|DT_EDITCONTROL|DT_WORDBREAK);
	
	m_pUpdateInfoLabel->SetFixedHeight(rc.bottom - rc.top);
	m_pUpdateInfoLabel->SetText(szUpdateLog);
	m_pUpdateInfoLabel->SetAttribute(_T("multiline"),_T("true"));
	m_pUpdateInfoLabel->SetTextColor(0xFF666666);
	m_pUpdateInfoLabel->SetFont(nFont);

}

void CUpdateDialogUI::setUpdateVersion(LPCTSTR szUpdateVersion)
{
	m_strUpdateVersion = szUpdateVersion;
	m_pUpdateInfoTitle->SetText(m_strUpdateVersion.c_str());
}

void CUpdateDialogUI::setAllFilesSize( LPCTSTR szFilesSize )
{
	m_strAllFilesSize = szFilesSize;
	m_pAllFilesSizeLabel->SetText(m_strAllFilesSize.c_str());
}

void CUpdateDialogUI::setUpdate( BOOL bUpdate )
{
	m_bUpdate	= bUpdate;
	if(m_bUpdate == eUpdateType_Has_Update)
	{
		m_pPlaynewBtn->SetText(_T("启动新版本"));
		m_pMainLayoutUI->SetFixedHeight(430);
		m_pMainWindowLayoutUI->SetFixedWidth(610);
//		ResizeClient(610,430);
		m_pHasUpdateLayout->SetVisible(true);
		m_pNewestLayout->SetVisible(false);
		m_pUpdateProcessingLayout->SetVisible(false);
	}
	else if(m_bUpdate == eUpdateType_Update_Processing)
	{
		m_pMainLayoutUI->SetFixedHeight(430);
		m_pMainWindowLayoutUI->SetFixedWidth(610);
		m_pUpdateProcessingLayout->SetVisible(true);
		m_pHasUpdateLayout->SetVisible(false);
		m_pNewestLayout->SetVisible(false);
	}
	else
	{
		tstring str = getLocalVersion();
		m_pLocalVersion->SetText(str.c_str());

		m_pMainLayoutUI->SetFixedHeight(360);
		m_pMainWindowLayoutUI->SetFixedWidth(540);

//		ResizeClient(540,360);
		m_pNewestLayout->SetVisible(true);
		m_pHasUpdateLayout->SetVisible(false);
		m_pUpdateProcessingLayout->SetVisible(false);
	}
}

tstring CUpdateDialogUI::getLocalVersion()
{
	tstring str = _T("当前版本: ");
	return str += g_Config::GetInstance()->GetVersion();
}

void CUpdateDialogUI::TimerProcComplete(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	CUpdateDialogUI* pDialog = UpdateDialogUI::GetInstance();	
	if(idEvent == TIMER_EVENT_TIMINGUPDATE)
	{
		int nCount = pDialog->GetTimingUpdateCount();
		if(nCount > 1)
		{
			nCount--;
			pDialog->SetTimingUpdateCount(nCount);
			TCHAR szBuff[MAX_PATH];
			_stprintf(szBuff, _T("启动新版本 (%d)"), pDialog->m_nTimingUpdateCount);
			pDialog->m_pPlaynewBtn->SetText(szBuff);
		}
		else
		{
			KillTimer(hwnd, TIMER_EVENT_TIMINGUPDATE);
			pDialog->ShowWindow(false);
			//开始强更
			BroadcastEvent(EVT_ENV_RELEASE, 1, 0, NULL);
			//
		}
	}
}


void CUpdateDialogUI::OnConfirmBtn(TNotifyUI& msg)
{
	//ShowWindow(false);
	Close();
}

void CUpdateDialogUI::OnCloseBtn(TNotifyUI& msg)
{
	KillTimer(this->GetHWND(), TIMER_EVENT_TIMINGUPDATE);
	//ShowWindow(false);
	Close();
}

void CUpdateDialogUI::OnPlayNewBtn( TNotifyUI& msg )
{
	KillTimer(this->GetHWND(), TIMER_EVENT_TIMINGUPDATE);
	ShowWindow(false);
	BroadcastEvent(EVT_ENV_RELEASE, 0, 0, NULL);
}

void CUpdateDialogUI::TimingUpdate()
{
	m_nTimingUpdateCount = 60;
	m_pPlaynewBtn->SetText(_T("启动新版本 (60)"));
	SetTimer(this->GetHWND(), TIMER_EVENT_TIMINGUPDATE, 1000, (TIMERPROC)CUpdateDialogUI::TimerProcComplete);
}

int CUpdateDialogUI::GetTimingUpdateCount()
{
	return m_nTimingUpdateCount;
}

void CUpdateDialogUI::SetTimingUpdateCount( int nCount )
{
	m_nTimingUpdateCount = nCount;
}

void CUpdateDialogUI::Init( CRect rect )
{
	::MoveWindow(GetHWND(), rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
}

void CUpdateDialogUI::ShowUpdateInfo( UpdateInfo_Param * pParam )
{
	if(pParam->nType == eUpdateType_Update_Processing)
	{
		setUpdate(eUpdateType_Update_Processing);
		TCHAR szSpeedBuff[128];
		if(pParam->fDownloadSpeed > 1024 * 1024)
		{
			_stprintf(szSpeedBuff, _T("%0.2fM/s"), pParam->fDownloadSpeed/(1024 * 1024));
		}
		else if(pParam->fDownloadSpeed > 1024)
			_stprintf(szSpeedBuff, _T("%0.2fK/s"), pParam->fDownloadSpeed/1024);
		else
			_stprintf(szSpeedBuff, _T("%0.2fB/s"), pParam->fDownloadSpeed);

		TCHAR szBuff[1024];
		_stprintf(szBuff, _T("正在下载 %s %s"), pParam->szFileName, szSpeedBuff);
		m_pUpdateInfoParamLabel->SetText(szBuff);
		m_pUpdateInfoParamLabel->SetToolTip(szBuff);
		m_pCurrentProgress->SetValue(pParam->dwCurrentProgress);
		if(pParam->dwTotalProgress > 90)
		{
			pParam->dwTotalProgress = 90;
		}
		m_pTotalProgress->SetValue(pParam->dwTotalProgress);
	}
	else if(pParam->nType == eUpdateType_Failed)
	{
		m_pUpdateProcessingLabel->SetText("更新下载失败");
	}
}


LRESULT CUpdateDialogUI::HandleCustomMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if (uMsg == WM_SHOWWINDOW)
	{
		CRect rect;
		::GetWindowRect(AfxGetMainWnd()->m_hWnd,&rect);
		Init(rect);
		ShowWindow();
	}
	
	return __super::HandleCustomMessage( uMsg, wParam, lParam, bHandled );
}
