#include "StdAfx.h" 
#include "MagicPen_Altha.h"
#include "Common.h" 
#include "MagicPen_View.h"

CMagicPen_Altha::CMagicPen_Altha(void)
{
	m_hPenZoom = NULL;
	m_pbmpTmp = NULL;
	m_nCx  = GetSystemMetrics(SM_CXSCREEN);
	m_nCy = GetSystemMetrics(SM_CYSCREEN); 
}

CMagicPen_Altha::~CMagicPen_Altha(void)
{
}

LPCTSTR CMagicPen_Altha::GetWindowClassName() const
{
	return _T("MagicPen_Altha");
}

DuiLib::CDuiString CMagicPen_Altha::GetSkinFile()
{
	return _T("MagicPen\\MagicPen_Altha.xml");
}

DuiLib::CDuiString CMagicPen_Altha::GetSkinFolder()
{
	return _T("skins");
} 


void CMagicPen_Altha::InitPenZoom( int nL, int nT, int nW, int nH,CBitmap* pBmp)
{   
	int nClosePicWidth = 60;

	int nZoomTimes = 3;//Ëõ·Å±¶ÂÊ
	int nWidth = (nW*nZoomTimes + nClosePicWidth) > m_nCx? m_nCx:(nW*nZoomTimes + nClosePicWidth);
	int nHeight = nH*nZoomTimes > m_nCy? m_nCy:nH*nZoomTimes;
	int nLeft = (m_nCx / 2 - nWidth / 2 );
	int nTop = (m_nCy / 2 - nHeight / 2); 

	m_nLeft = nLeft + nClosePicWidth;
	m_nTop = nTop;
	m_nWidth = nWidth;
	m_nHeight = nHeight; 

	//Ô¤ÀÀ´°¿Ú
	if (::IsWindow(m_hPenZoom))
	{
		::DestroyWindow(m_hPenZoom);
	} 
	CMagicPen_View*	wndPenView = NULL;
	if (!m_hPenZoom || !::IsWindow(m_hPenZoom))
	{
		wndPenView = new CMagicPen_View();  
		wndPenView->Create(this->GetHWND(), _T("CMagicPen_View"), WS_EX_TOOLWINDOW | WS_VISIBLE,WS_EX_TOPMOST);
		wndPenView->InitPenZoom( nL, nT, nW, nH,pBmp, this->GetHWND() );
		::SetWindowPos(wndPenView->GetHWND(),HWND_DESKTOP,nLeft, nTop, nWidth, nHeight, SWP_NOACTIVATE);

		m_hPenZoom	= wndPenView->GetHWND(); 
	} 
} 

LRESULT CMagicPen_Altha::OnLButtonDown( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if (::IsWindow(m_hPenZoom))
	{
		RECT lRect;
		GetWindowRect(m_hPenZoom, &lRect);
		::SetWindowPos(m_hPenZoom, HWND_DESKTOP,lRect.left, lRect.top, 0, 0, SWP_SHOWWINDOW| SWP_NOSIZE);
	}
	return FALSE;
}
