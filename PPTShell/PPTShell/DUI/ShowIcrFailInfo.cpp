#include "StdAfx.h" 
#include "ShowIcrFailInfo.h"
#include "Common.h"
#include "NDCloud\NDCloudUser.h" 
#include "Util\Util.h"

CShowIcrFailInfo::CShowIcrFailInfo(void)
{
	m_pFailInfo = NULL; 
	m_nCx  = GetSystemMetrics(SM_CXSCREEN);
	m_nCy = GetSystemMetrics(SM_CYSCREEN); 
}

CShowIcrFailInfo::~CShowIcrFailInfo(void)
{
}

LPCTSTR CShowIcrFailInfo::GetWindowClassName() const
{
	return _T("CShowIcrFailInfo");
}

DuiLib::CDuiString CShowIcrFailInfo::GetSkinFile()
{
	return _T("ScreenTool\\ShowIcrFailInfo.xml");
}

DuiLib::CDuiString CShowIcrFailInfo::GetSkinFolder()
{
	return _T("skins");
}

void CShowIcrFailInfo::InitWindow()
{
	__super::InitWindow(); 
	m_pFailInfo	= static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("LoadingFail"))); 

}

void CShowIcrFailInfo::ShowLocalWindows(HWND hParent )
{ 
	if (!GetHWND() || !::IsWindow(GetHWND()))
	{	
		Create(hParent, _T("CShowIcrFailInfo"), WS_POPUP,WS_EX_TOPMOST|SWP_NOACTIVATE);  
	} 

	::SetWindowPos(GetHWND(), HWND_TOPMOST, (m_nCx - 450) / 2, (m_nCy - 150) / 2, 450, 150, SWP_NOACTIVATE); 
 
	m_nStartTime = GetTickCount();
	m_PaintManager.SetTimer(m_pFailInfo, FAIL_TIMER_ID, 500);
	m_pFailInfo->OnEvent += MakeDelegate(this, &CShowIcrFailInfo::OnCloseWndEvent);
	this->ShowModal();
} 

bool CShowIcrFailInfo::OnCloseWndEvent(void* pObj)
{
	TEventUI* pEvent = (TEventUI*)pObj;
	if (pEvent->Type == UIEVENT_TIMER)
	{
		if (pEvent->wParam == FAIL_TIMER_ID)
		{ 
			DWORD lEndTime= GetTickCount();
			if (lEndTime - m_nStartTime > 60000)//如果超过1分钟还未加载完毕，则自动关闭窗口
			{ 				
				m_PaintManager.KillTimer(m_pFailInfo, FAIL_TIMER_ID);				 
				if (this->GetHWND())
				{
					Close();
				}
				return false;
			}
			 
			m_pFailInfo->SetText(NDCloudUser::GetInstance()->GetLoadALLTeacherTip().c_str());
			if(NDCloudUser::GetInstance()->GetIsLoadAllTeacherInfo())
			{
				NDCloudUser::GetInstance()->IcrStart();//加载完成自动关闭窗口后调用启动101pad
				m_PaintManager.KillTimer(m_pFailInfo, FAIL_TIMER_ID);				 
				if (this->GetHWND())
				{
					Close();
				}
			}
			 
		} 
	}
	return true;
}

void CShowIcrFailInfo::OnBtnCloseClick( TNotifyUI& msg )
{
	m_PaintManager.KillTimer(m_pFailInfo, FAIL_TIMER_ID);				 
	if (this->GetHWND())
	{
		Close();
	}
}
 