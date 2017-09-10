#include "StdAfx.h"
#include "Util/Stream.h"
#include "MobilePenView.h"
#include "PanelView.h"
#include "PPTControl/PPTController.h"
#include "Util/Util.h"
#include "DUI/Shape.h" 

CMobilePenViewUI::CMobilePenViewUI(void)
{
	m_pMobilePen = NULL;   
	//设置默认颜色 
	m_dwCurrentColor = RGB(234,55,68);
	m_dwCurrentColor  &= 0x00FFFFFF;
	if(m_dwCurrentColor == 0)
	{
		m_dwCurrentColor = 0x1;
	}
	m_dwCurrentColor = RGB(GetBValue(m_dwCurrentColor), GetGValue(m_dwCurrentColor), GetRValue(m_dwCurrentColor));
	//设置penview默认类型
	m_nCurrentPenViewType = ePoiner_Pen;
}

CMobilePenViewUI::~CMobilePenViewUI(void)
{
}
  
DuiLib::CDuiString CMobilePenViewUI::GetSkinFolder()
{
	return _T("skins");
}

DuiLib::CDuiString CMobilePenViewUI::GetSkinFile()
{
	return _T("ScreenTool\\MobilePenView.xml");
}

LPCTSTR CMobilePenViewUI::GetWindowClassName( void ) const
{
	return _T("CMobilePenViewUI");
}
 
void CMobilePenViewUI::InitWindow()
{ 
	m_pLayoutBG = (CVerticalLayoutUI*)m_PaintManager.FindControl(_T("layoutBG"));  

	m_pMobilePen = new CPanelViewUI;
	m_pMobilePen->SetName(_T("mobilepen"));	
	m_pMobilePen->SetReferenceBackground(true);
	m_pMobilePen->SetShapeColor(m_dwCurrentColor);
	m_pMobilePen->SetPointer(m_nCurrentPenViewType);  
	m_pLayoutBG->Add(m_pMobilePen);
}
 

CControlUI* CMobilePenViewUI::CreateControl( LPCTSTR pstrClass )
{
	if (_tcsicmp(_T("PanelView"), pstrClass) == 0)
	{
		return new CPanelViewUI;
	}
	return NULL;
} 

void CMobilePenViewUI::ShowMobilePenBoard()
{
	if (!GetHWND() || !::IsWindow(GetHWND()))
	{ 
		HWND hwnd = CPPTController::GetSlideShowViewHwnd();
		Create(CPPTController::GetSlideShowViewHwnd(), _T("CMobilePenViewUI"), WS_POPUP| WS_VISIBLE, WS_EX_TOPMOST);  
		MONITORINFO oMonitor = {0};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(this->GetHWND(), MONITOR_DEFAULTTONEAREST), &oMonitor);
		m_nScreenWidth = oMonitor.rcMonitor.right - oMonitor.rcMonitor.left;
		m_nScreenHeight	= oMonitor.rcMonitor.bottom - oMonitor.rcMonitor.top;
	} 

	::SetWindowPos(GetHWND(), HWND_TOPMOST, 0, 0, m_nScreenWidth , m_nScreenHeight, SWP_NOACTIVATE| SWP_SHOWWINDOW);   
}
 

void CMobilePenViewUI::CloseMobilePenBoard()
{  
	if (GetHWND())
	{
		::DestroyWindow(this->GetHWND()); 
	}
}

void CMobilePenViewUI::OnBtnCloseClick( TNotifyUI& msg )
{
	CloseMobilePenBoard();
}   
 

void CMobilePenViewUI::ClearPanel()
{ 
	if(m_pMobilePen)
	{
		m_pMobilePen->ClearShapes();
	} 
}   

void CMobilePenViewUI::AddShapes( IShape* pShape )
{
	m_pMobilePen->AddShapes(pShape);

}
 