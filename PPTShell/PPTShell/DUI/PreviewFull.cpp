#include "StdAfx.h"
#include "PreviewFull.h"


CPreviewFullUI* CPreviewFullUI::m_pInstance = NULL;


CPreviewFullUI::CPreviewFullUI(void)
{
	m_nHeight = 0;
	m_nWidth = 0;
	m_rImgeSize.cx = 0; 
	m_rImgeSize.cy = 0;
	m_bHaveWindow=false;
}

CPreviewFullUI::~CPreviewFullUI(void)
{
}

LPCTSTR CPreviewFullUI::GetWindowClassName( void ) const
{
	return _T("AboutDialog");
}

DuiLib::CDuiString CPreviewFullUI::GetSkinFile()
{
	return _T("Preview\\PreviewFullScreen.xml");
}

DuiLib::CDuiString CPreviewFullUI::GetSkinFolder()
{
	return _T("skins");
}

void CPreviewFullUI::InitFullWnd(LPCTSTR lpszImgFile, EPLUS_WND_TYPE eWndType)
{
	m_eCurrentType = eWndType;

	if (E_WND_COURSE == m_eCurrentType)
	{
		m_pVerticalLayout->SetVisible(false);
		int nLeftWidth = (m_nWidth/2) - (PIC_PREVIEW_WIDTH / 2);
		int nTopHeight = (m_nHeight/2) - (PIC_PREVIEW_HEIGHT / 2);
		CDuiRect rt;
		rt.left = nLeftWidth;
		rt.right = PIC_PREVIEW_WIDTH + nLeftWidth;
		rt.top = nTopHeight;
		rt.bottom = PIC_PREVIEW_HEIGHT + nTopHeight;
		m_pPreviewBig->SetPos(rt);
		m_pPreviewBig->SetBkImage(lpszImgFile);
		m_pVerticalLayout->SetVisible(true);
		m_pPreviewBig->Invalidate();
		
		
	}
	else if (E_WND_PICTURE == m_eCurrentType)
	{
		SIZE szImg = GetImageFileSize(lpszImgFile);
		
		if (szImg.cx > m_nWidth )
		{
			szImg.cx = m_nWidth ;
		}
		if (szImg.cy > m_nHeight)
		{
			szImg.cy = m_nHeight ;
		}

		if (szImg.cx == 0 || szImg.cy == 0)
		{
			
			m_pPreviewBig->SetVisible(false);
			m_pPreviewError->SetVisible(true);
			m_pVerticalLayout->SetVisible(false);
			int nLeftWidth = (m_nWidth/2) - (szImg.cx / 2);
			int nTopHeight = 50;
			CDuiRect rt;
			rt.left = nLeftWidth;
			rt.right = 90 + nLeftWidth;
			rt.top = nTopHeight;
			rt.bottom = 70 + nTopHeight;
			
			m_pVerticalLayout->SetVisible(true);
			m_pPreviewError->SetPos(rt);
		
			m_pPreviewError->Invalidate();
			m_pVerticalLayout->Invalidate();
			return;

		}

		m_pVerticalLayout->SetVisible(false);
		int nLeftWidth = (m_nWidth/2) - (szImg.cx / 2);
		int nTopHeight = (m_nHeight/2) - (szImg.cy / 2);
		CDuiRect rt;
		rt.left = nLeftWidth;
		rt.right = szImg.cx + nLeftWidth -50;
		rt.top = nTopHeight-50;
		rt.bottom = szImg.cy + nTopHeight;
		m_pPreviewBig->SetPos(rt);
		m_pPreviewBig->SetBkImage(lpszImgFile);
		m_pVerticalLayout->SetVisible(true);
		m_pPreviewBig->Invalidate();
	}
	else if (E_WND_FLASH == m_eCurrentType)
	{
	}
		ShowWindow(SW_SHOW);
}

void CPreviewFullUI::InitWindow()
{
	m_pVerticalLayout = static_cast<CVerticalLayoutUI *>(m_PaintManager.FindControl(_T("PreviewFullDlg")));
	m_pPreviewBig	  =	static_cast<CControlUI*>(m_pVerticalLayout->FindSubControl(_T("PreviewBig")));
	m_pPreviewError	  = static_cast<CControlUI*>(m_pVerticalLayout->FindSubControl(_T("PreviewBigError")));


	POINT pt;
	::GetCursorPos(&pt);
	::ClientToScreen(GetHWND(), &pt);
	HMONITOR hMonitor = ::MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
	MONITORINFO monitorInfo;
	monitorInfo.cbSize = sizeof(MONITORINFO);
	::GetMonitorInfo(hMonitor, &monitorInfo);

	m_nWidth = monitorInfo.rcMonitor.right-monitorInfo.rcMonitor.left;
	m_nHeight =  monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
	
	//SetWindowPos(m_hWnd, HWND_DESKTOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW);
	//SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);

// 	::SetWindowLong(m_hWnd, GWL_EXSTYLE, WS_EX_TOPMOST); 
// 	UpdateWindow(GetHWND()); 

	::MoveWindow(m_hWnd, 0,0, m_nWidth, m_nHeight, false);

	//::SetWindowLong(m_hWnd, GWL_EXSTYLE, WS_EX_TOPMOST); 

	//HideTaskBar(true);

}

void CPreviewFullUI::OnBtnClose(TNotifyUI& msg)
{
	Close();
}

void CPreviewFullUI::ShowPreviewFull(HWND hwnd, EPLUS_WND_TYPE eWndType, LPCTSTR lpszImgFile)
 {
	 m_pInstance->Create(hwnd, _T("PreviewFull"), WS_POPUP, 0, 0, 0, 0, 0);
	 m_pInstance->InitFullWnd(lpszImgFile, eWndType);
	 m_pInstance->m_bHaveWindow=true;
 }

CPreviewFullUI* CPreviewFullUI::GetInstance(HWND hwnd, EPLUS_WND_TYPE eWndType, LPCTSTR lpszImgFile)
{
	if( m_pInstance == NULL )
	{
		m_pInstance = new CPreviewFullUI;
		ShowPreviewFull(hwnd, eWndType, lpszImgFile);
	}else
	{
		if( m_pInstance->m_bHaveWindow==false)
		{
			ShowPreviewFull(hwnd, eWndType, lpszImgFile);
		}else
		{
			 m_pInstance->InitFullWnd(lpszImgFile, eWndType);
			 m_pInstance->ShowWindow(SW_SHOWNORMAL);
		}
	}
	return m_pInstance;
}

void CPreviewFullUI::ReleaseInstance()
{
	if (m_pInstance != NULL)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

SIZE CPreviewFullUI::GetImageFileSize(LPCTSTR lpszFile)
{
	const TImageInfo* pImageInfo = NULL;
	
	pImageInfo = m_PaintManager.GetImageEx((LPCTSTR)lpszFile);

	if( pImageInfo ) 
	{
		m_rImgeSize.cx = pImageInfo->nX;
		m_rImgeSize.cy = pImageInfo->nY;
	}
	return m_rImgeSize;
	



}


LRESULT CPreviewFullUI::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	
	 m_bHaveWindow=false;
	__super::OnClose(uMsg, wParam ,lParam, bHandled);
	return 0;
}