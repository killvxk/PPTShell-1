#include "StdAfx.h"
#include "ForceUpdate.h"
#include "Util/Util.h"
#include "EventCenter/EventDefine.h"

CForceUpdate * pForceUpdate = NULL;
CForceUpdate::CForceUpdate(void)
{
}

CForceUpdate::~CForceUpdate(void)
{
	m_update.CancelUpdate();
//	CancelEvent(EVT_CHECK_UPDATE, MakeEventDelegate(this, &CForceUpdate::OnCheckUpdate));
}

void CForceUpdate::InitWindow()
{
	pForceUpdate = this;
	m_pLabel	= static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("text")));

//	::OnEvent(EVT_CHECK_UPDATE, MakeEventDelegate(this, &CForceUpdate::OnCheckUpdate));

	pForceUpdate->m_update.m_OnNotify += MakeDelegate(pForceUpdate, &CForceUpdate::OnCheckUpdate);
	m_nTimer = 6;
	SetTimer(m_hWnd, UPDATE_REFRESH_TEXT, 1000, (TIMERPROC)TimerProcComplete);
}


void CForceUpdate::TimerProcComplete(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime)
{
	if(idEvent == UPDATE_REFRESH_TEXT)
	{
		pForceUpdate->m_nTimer--;
		if(pForceUpdate->m_nTimer == 0)
		{
			KillTimer(hwnd,UPDATE_REFRESH_TEXT);
			pForceUpdate->PostMessage(WM_COMMAND, MAKEWPARAM(IDOK, BN_CLICKED), NULL);

			pForceUpdate->UpdateConfig();
			return;
		}

		if(pForceUpdate->m_nTimer == 4)
		{
			pForceUpdate->m_update.CheckUpdate();
		}

		TCHAR szText[128];
		tstring strText;
		for(int i = 6 ; i> pForceUpdate->m_nTimer ; i--)
		{
			strText += _T(".");
		}
		_stprintf(szText, _T("正在启动%s"), strText.c_str());
		pForceUpdate->m_pLabel->SetText(szText);
	}
}

bool CForceUpdate::OnCheckUpdate( void* pObj )
{
	TEventNotify*  pENotify	= (TEventNotify*) pObj;
	int nType			= pENotify->nEventType;

	::PostMessage(GetHWND(),WM_CHECK_UPDATE, (WPARAM)nType, NULL);

	return true;
}

void CForceUpdate::UpdateConfig()
{
	//修改本地配置时间
	tstring strPath = GetLocalPath();
	strPath += _T("\\Setting\\Config.ini");

	CTime tCurrentDate = CTime::GetCurrentTime();
	TCHAR szUpdateDate[60] = {0};
	_stprintf(szUpdateDate,_T("%04d-%02d-%02d %02d:%02d:%02d"),
		tCurrentDate.GetYear(),
		tCurrentDate.GetMonth(),
		tCurrentDate.GetDay(),
		tCurrentDate.GetHour(),
		tCurrentDate.GetMinute(),
		tCurrentDate.GetSecond()
		);
	WritePrivateProfileString(_T("config"), _T("UpdateDate"), szUpdateDate, strPath.c_str());
	//
}

LRESULT CForceUpdate::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_CHECK_UPDATE:
		{
			KillTimer(GetHWND(),UPDATE_REFRESH_TEXT);

			if(wParam == eUpdateType_Has_Update)
			{
				PostMessage(WM_COMMAND, MAKEWPARAM(IDCANCEL, BN_CLICKED), NULL);
				m_update.StartCover();

			}
			else
				PostMessage(WM_COMMAND, MAKEWPARAM(IDOK, BN_CLICKED), NULL);
			
			UpdateConfig();
		}
		break;
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}