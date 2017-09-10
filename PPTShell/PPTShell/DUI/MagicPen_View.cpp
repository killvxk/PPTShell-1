#include "StdAfx.h" 
#include "MagicPen_View.h"
#include "Common.h" 
#include "MagicPen_Main.h"


CMagicPen_View::CMagicPen_View(void)
{ 
	m_pbmpCopyScreen = NULL;
	m_nCx  = GetSystemMetrics(SM_CXSCREEN);
	m_nCy = GetSystemMetrics(SM_CYSCREEN);
	m_hParentHandle = NULL;
}

CMagicPen_View::~CMagicPen_View(void)
{
}

void CMagicPen_View::InitWindow()
{
	__super::InitWindow();
	m_pCloseBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnClose")));
	m_pCopyBmplbl = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("CopyBmpLabel"))); 
}

LPCTSTR CMagicPen_View::GetWindowClassName() const
{
	return _T("MagicPen_View");
}

DuiLib::CDuiString CMagicPen_View::GetSkinFile()
{
	return _T("MagicPen\\MagicPen_View.xml");
}

DuiLib::CDuiString CMagicPen_View::GetSkinFolder()
{
	return _T("skins");
}
 

LRESULT CMagicPen_View::OnLButtonDown( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	CPoint point;

	point.x=GET_X_LPARAM(lParam);  
	point.y=GET_Y_LPARAM(lParam); 
	 
	RECT lClosePos = m_pCloseBtn->GetPos();
	
	
	RECT lTmp =m_pCloseBtn->GetRelativePos();
 
	
	if(!PtInRect(&lClosePos, point))
	{
		PostMessage(WM_NCLBUTTONDOWN,   HTCAPTION,   lParam);  
		return FALSE;
	}
	return  __super::OnLButtonDown(uMsg, wParam, lParam, bHandled);
} 
 
void CMagicPen_View::InitPenZoom( int nL, int nT, int nW, int nH, CBitmap* pBmp , HWND hHandle )
{ 
	m_nLeft = nL; 
	m_nTop = nT;
	m_nWidth = nW;
	m_nHeight = nH;
	m_pbmpCopyScreen = pBmp; 
	m_hParentHandle = hHandle;

	m_pCopyBmplbl->SetBkImage(_T("file=\'c:\\MagicPenTmp.bmp\'")); 
	RECT lRect;
	lRect.left = 0;
	lRect.top = 0;
	lRect.right = m_nWidth*3;
	lRect.bottom = m_nHeight * 3;
	m_pCopyBmplbl->SetPos(lRect);  
}   

void CMagicPen_View::OnCloseBtn( TNotifyUI& msg )
 {	
	Close(); 
	MagicPen_MainUI::GetInstance()->CloseLocalWindows();
	//::SendMessage(m_hParentHandle, WM_CLOSE, 0,0);//关闭上级窗口
}

LRESULT CMagicPen_View::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( uMsg == WM_SYSKEYDOWN && wParam == VK_F4 )
	{
		return FALSE;
	}
	else if (uMsg == WM_KEYDOWN)
	{ 
		if (wParam == VK_ESCAPE)
		{ 
			Close();
			MagicPen_MainUI::GetInstance()->CloseLocalWindows();
			//::SendMessage(m_hParentHandle, WM_CLOSE, 0,0);//关闭上级窗口
			return TRUE;
		}
	}

	return __super::HandleMessage(uMsg,wParam, lParam);
} 

