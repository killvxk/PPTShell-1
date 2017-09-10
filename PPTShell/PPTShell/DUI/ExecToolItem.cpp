#include "StdAfx.h"
#include "ExecToolItem.h"
#include "Util/Util.h"
#include "Statistics/Statistics.h"
#include "InstrumentView.h"
#include "DUI/InstrumentItem.h"


CExecToolItemUI::CExecToolItemUI(void)
{

}

CExecToolItemUI::~CExecToolItemUI(void)
{

}

void CExecToolItemUI::OnClick( CContainerUI* pParent )
{
	if (trim(m_strResource).empty())
	{
		return;
	}

	//get file name
	LPCTSTR lpBackSlant = _tcsrchr(m_strResource.c_str(), _T('\\'));
	if (lpBackSlant)
	{
		lpBackSlant++;
	}
	else
	{
		lpBackSlant = m_strResource.c_str();
	}

	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	memset( &si, 0, sizeof(STARTUPINFO) );
	si.cb = sizeof(STARTUPINFO);  
	
	HWND	hWnd		= FindWindow(_T("CalcFrame"), NULL);
	if (!hWnd)
	{ 
		hWnd = FindWindow(NULL, _T("¼ÆËãÆ÷"));//
	}

	if (!HasProcess() || !hWnd)
	{
		
		if (CreateProcess(NULL, (TCHAR *)lpBackSlant, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi ))
		{
			SetProcess(pi.hProcess);
			CloseHandle(pi.hThread);
			Sleep(100);
		}

		//if had problem later, we need detact hwnd on timer
		int nSleepCount = 2;
		while(!hWnd && nSleepCount > 0)
		{
			hWnd = FindWindow(_T("CalcFrame"), NULL);
			if (!hWnd)
			{ 
				hWnd = FindWindow(NULL, _T("¼ÆËãÆ÷"));//
			}
			nSleepCount --;
			Sleep(100);
		}
	}

	CInstrumentView* pParentView = dynamic_cast<CInstrumentView*>(pParent);
	bool	bLeftSide	= pParentView->GetHoldInstrumentItem()->IsLeftSide();		
	RECT	rt			= pParentView->GetHoldInstrumentItem()->GetPos();
	
	if( hWnd )
	{

		RECT	calcRect;
		if (!::IsIconic(hWnd))
		{
			GetWindowRect(hWnd, &calcRect);
			m_calcRect = calcRect;
		}
		else
		{
			::ShowWindow(hWnd, SW_SHOWNORMAL);
			calcRect = m_calcRect;
		}

		if (bLeftSide)
		{
			::SetWindowPos(hWnd, HWND_TOPMOST, rt.right, rt.top, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE );
		}
		else
		{
			::SetWindowPos(hWnd, HWND_TOPMOST, rt.left - (calcRect.right - calcRect.left), rt.top,0, 0, SWP_NOACTIVATE | SWP_NOSIZE );
		}

		
	}

	Statistics::GetInstance()->Report(STAT_EXEC_CALC);
}