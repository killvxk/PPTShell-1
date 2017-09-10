#include "StdAfx.h"
#include "ImageCtrlDialogUI.h"
#include "Util/Util.h"
#include "EventCenter/EventDefine.h"
#include "PacketProcess/Packet.h"


CImageCtrlDialogUI::CImageCtrlDialogUI(void)
{
	m_bClosed = true;
	m_hWnd = NULL;
	::OnEvent(EVT_IMAGE_CONTROL, MakeEventDelegate(this, &CImageCtrlDialogUI::OnImageControl));
}

CImageCtrlDialogUI::~CImageCtrlDialogUI(void)
{
	::CancelEvent(EVT_IMAGE_CONTROL, MakeEventDelegate(this, &CImageCtrlDialogUI::OnImageControl));
}

LPCTSTR CImageCtrlDialogUI::GetWindowClassName( void ) const
{
	return _T("ImageCtrlDialog");
}

DuiLib::CDuiString CImageCtrlDialogUI::GetSkinFile()
{
	return _T("ImageCtrl\\ImageCtrl.xml");
}

DuiLib::CDuiString CImageCtrlDialogUI::GetSkinFolder()
{
	return _T("skins");
}


void CImageCtrlDialogUI::InitWindow()
{
	CLabelUI* pImageLabel	= static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("DisplayImage")));
	pImageLabel->SetBkImage(m_strImagePath);

	GetWindowRect(m_hWnd, &m_rtOriginal);
}
 
void CImageCtrlDialogUI::OpenDialog(LPCTSTR strImagePath)
{
	m_strImagePath = strImagePath;
	BroadcastEvent(EVT_IMAGE_CONTROL, IMGCTL_OPEN, 0, NULL);
}

void CImageCtrlDialogUI::CloseDialog()
{
	BroadcastEvent(EVT_IMAGE_CONTROL, IMGCTL_CLOSE, 0, NULL);
}

void CImageCtrlDialogUI::ZoomIn(float scale)
{
	BroadcastEvent(EVT_IMAGE_CONTROL, IMGCTL_ZOOMIN, (LPARAM)(scale*100), NULL);
}

void CImageCtrlDialogUI::ZoomOut(float scale)
{
	BroadcastEvent(EVT_IMAGE_CONTROL, IMGCTL_ZOOMOUT, (LPARAM)(scale*100), NULL);
}

void CImageCtrlDialogUI::Normal()
{
	BroadcastEvent(EVT_IMAGE_CONTROL, IMGCTL_NORMAL, (LPARAM)100.0f, NULL);
}

bool CImageCtrlDialogUI::OnImageControl(void *param)
{
	TEventNotify* pNotify = (TEventNotify*)param;

	if( pNotify->wParam == IMGCTL_OPEN )
	{
		if( !::IsWindow(m_hWnd) )
		{
			//HWND hParent = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
			Create(NULL, _T("ImageCtrlWindow"), WS_VISIBLE, 0);
		}
		else
		{
			CLabelUI* pImageLabel	= static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("DisplayImage")));
			pImageLabel->SetBkImage(m_strImagePath);
		}

		// get file size
		Gdiplus::Image* pImage = new Image(Str2Unicode(m_strImagePath.GetData()).c_str());
		int nWidth = pImage->GetWidth();
		int nHeight = pImage->GetHeight();
		delete pImage;

		ResizeWindow(nWidth, nHeight);
		ShowWindow(true, true);
		GetWindowRect(m_hWnd, &m_rtOriginal);

		SetWindowClosed(false);
	}
	else if( pNotify->wParam == IMGCTL_CLOSE )
	{
		Close();
		SetWindowClosed();
	}
	else if( pNotify->wParam == IMGCTL_ZOOMIN || pNotify->wParam == IMGCTL_ZOOMOUT )
	{
		// enlarge
		float fScale = (float)pNotify->lParam / 100.0f;
		float fWidth = (float)(m_rtOriginal.right - m_rtOriginal.left) * fScale;
		float fHeight = (float)(m_rtOriginal.bottom - m_rtOriginal.top) * fScale;
		
		int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

		int nWidth = (int)fWidth;
		int nHeight = (int)fHeight;

		int x = (nScreenWidth - nWidth) / 2;
		int y = (nScreenHeight - nHeight) / 2;

		if( x < 0 )
			x = 0;

		if( y < 0 )
			y = 0;


		MoveWindow(m_hWnd, x, y, nWidth, nHeight, TRUE); 
	}
	

	return true;
}

LRESULT CImageCtrlDialogUI::OnKeyDown(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if( wParam == VK_ESCAPE )
	{
		Close();
		bHandled = TRUE;
		SetWindowClosed();
	}

	return 0;
}
void CImageCtrlDialogUI::SetWindowClosed(bool bClosed)
{
	m_bClosed = bClosed;
}
bool CImageCtrlDialogUI::IsWindowClosed()
{
	return m_bClosed;
}
void CImageCtrlDialogUI::OnBtnCloseClick(TNotifyUI& msg)
{
	CloseDialog();
}
void CImageCtrlDialogUI::ResizeWindow(int nWidth, int nHeight)
{
	const float fDisplayRectMax = 4.0/5.0;	// 最大显示区域为屏幕的4/5
	int nDstWidth, nDstHeight, nDisplayRect;
	nDstWidth = nWidth;
	nDstHeight = nHeight;

	// 将图片超过显示区域大小时按比例缩放显示窗口
	int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	nDisplayRect = nScreenWidth * fDisplayRectMax;
	// 计算宽度
	if(nWidth > nDisplayRect)
	{
		nDstWidth = nDisplayRect;
		nDstHeight = nDstWidth * nHeight / nWidth;
	}

	// 计算高度，如果高度超过大小，则重新计算宽度
	nDisplayRect = nScreenHeight * fDisplayRectMax;
	if(nDstHeight > nDisplayRect)
	{
		nDstHeight = nDisplayRect;
		nDstWidth = nDstHeight * nWidth / nHeight;
	}

	int x = (nScreenWidth - nDstWidth) / 2;
	int y = (nScreenHeight - nDstHeight) / 2;

	if( x < 0 )
		x = 0;

	if( y < 0 )
		y = 0;

	::SetWindowPos(m_hWnd, HWND_TOPMOST, x, y, nDstWidth, nDstHeight, SWP_SHOWWINDOW | SWP_NOACTIVATE);
	//MoveWindow(m_hWnd, x, y, nDstWidth, nDstHeight, TRUE); 
}

