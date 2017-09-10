#include "StdAfx.h" 
#include "LoginToolTip.h"
#include "Common.h"
#include "NDCloud\NDCloudUser.h"
#include "GroupExplorer.h"
#include "ItemExplorer.h"
#include "EventCenter\EventDefine.h"

CLoginToolTip::CLoginToolTip(void)
{
	m_pTeacherName = NULL;
	m_pTeacherGradeName = NULL;
}

CLoginToolTip::~CLoginToolTip(void)
{
}

LPCTSTR CLoginToolTip::GetWindowClassName() const
{
	return _T("CLoginToolTip");
}

DuiLib::CDuiString CLoginToolTip::GetSkinFile()
{
	return _T("ChangeGrade\\LoginToolTip.xml");
}

DuiLib::CDuiString CLoginToolTip::GetSkinFolder()
{
	return _T("skins");
}

void CLoginToolTip::InitWindow()
{
	m_pTeacherName				= static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lbl_TeacherName")));
	m_pTeacherGradeName			= static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lbl_GradeName"))); 
}

void CLoginToolTip::ShowLocalWindows(POINT ptInfo,tstring sTeacherName, tstring sGradeName)
{ 
	if (!GetHWND() || !::IsWindow(GetHWND()))
	{	
		Create(NULL, _T("CLoginToolTip"), WS_POPUP | WS_VISIBLE,  WS_EX_TOOLWINDOW| WS_EX_TOPMOST);  
	} 

	m_lStartTick = GetTickCount();
	m_ptPos.x = ptInfo.x;
	m_ptPos.y = ptInfo.y;
	::SetWindowPos(GetHWND(), HWND_TOPMOST, ptInfo.x, ptInfo.y, 240, 110, SWP_NOACTIVATE|SWP_SHOWWINDOW); 
	m_PaintManager.SetTimer(m_pTeacherName, TEACHER_TIMER_ID, 500);
	m_pTeacherName->OnEvent += MakeDelegate(this, &CLoginToolTip::OnCloseWndEvent);

	TCHAR szForamt[]	= _T("{f 140100}{c #11B0B6}%s{/c}{/f} {f 140100}{c #000000}%s{/c} {/f}");
	TCHAR szText[260]	= {0};
	_stprintf_s(szText, szForamt, _T(sTeacherName.c_str()), _T("老师,欢迎您！"));
	m_pTeacherName->SetShowHtml();
	m_pTeacherName->SetText(szText);

	tstring strGradeName = _T(sGradeName.c_str());
	if (sGradeName == _T(""))
	{
		strGradeName = _T("无");
	}  

	TCHAR szGradeForamt[]	= _T("{f 140100}{c #000000}%s{/c}{/f} {f 140100}{c #11B0B6}%s{/c}{/f}"); 
	_stprintf_s(szText, szGradeForamt, _T("当前班级："), _T(strGradeName.c_str()));
	m_pTeacherGradeName->SetShowHtml();
	m_pTeacherGradeName->SetText(_T(szText));
}


bool CLoginToolTip::OnCloseWndEvent(void* pObj)
{
	TEventUI* pEvent = (TEventUI*)pObj;
	if (pEvent->Type == UIEVENT_TIMER)
	{
		if (pEvent->wParam == TEACHER_TIMER_ID)
		{
			POINT pt; 
			GetCursorPos(&pt); 
			CRect lRect;
			GetWindowRect(this->GetHWND(), &lRect);
			char szPos[128];
			sprintf(szPos, "curpos:%d,%d,pos:%d,%d, %d,%d\n", pt.x, pt.y, m_ptPos.x, m_ptPos.y, lRect.Width(), lRect.Height());
			OutputDebugString(_T(szPos));
			if((pt.x > m_ptPos.x) && (pt.x < m_ptPos.x + lRect.Width())  && ( pt.y > m_ptPos.y) && (pt.y < m_ptPos.y + lRect.Height()) )
			{
				//在区域范围内的不处理
			}
			else
			{
				int lStartTick = GetTickCount();
				if ((lStartTick - m_lStartTick) > 3000)
				{
					m_PaintManager.KillTimer(m_pTeacherName, TEACHER_TIMER_ID);				 
					if (this->GetHWND())
					{
						Close();
					}
				}
			}
		} 
	}
	return true;
} 

void CLoginToolTip::OnBtnCloseClick( TNotifyUI& msg )
{
	m_PaintManager.KillTimer(m_pTeacherName, TEACHER_TIMER_ID);				 
	if (this->GetHWND())
	{
		Close();
	}
} 

 
