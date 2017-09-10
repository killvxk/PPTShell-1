#include "StdAfx.h" 
#include "MagicPen_Main.h"
#include "Common.h" 
#include <winuser.h>
#include <afxwin.h>
#include <windef.h>
#include "MagicPen_Altha.h"
#include "PPTControl\PPTController.h"
#include "PacketProcess\Packet.h"
#include "PacketProcess\PacketStream.h"

class CPPTShellApp;
 
CMagicPen_Main::CMagicPen_Main(void)
{
	m_hMagicPenAltha = NULL;
	m_pbmpScreen = NULL;
	m_pbmpCopyScreen = NULL;
	m_pbmpToolTip = NULL;
	m_pRoundRect = NULL;
	m_pShowToolTip = NULL;
	m_nCx  = GetSystemMetrics(SM_CXSCREEN);
	m_nCy = GetSystemMetrics(SM_CYSCREEN);  
	
	m_bHaveShow = false;
	m_ptStart.x = 0;
	m_ptStart.y = 0;
	m_ptEnd.x = 0;
	m_ptEnd.y = 0; 
	m_nControlMode = 2;
	m_bIsShowing = false;
}

CMagicPen_Main::~CMagicPen_Main(void)
{
	if (IsWindow(m_hMagicPenAltha))
	{
		DestroyWindow(m_hMagicPenAltha);
	}
	m_hMagicPenAltha = NULL;
	m_pbmpScreen = NULL;
	m_pbmpCopyScreen = NULL;
	m_pbmpToolTip = NULL;
	m_pShowToolTip->SetVisible(false);
	m_pRoundRect->SetVisible(false);
}

void CMagicPen_Main::InitWindow()
{ 
	m_pRoundRect  =	static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("RoundRect")));	
	m_pShowToolTip  =	static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("ShowToolTip")));
	RECT lRect;
	lRect.left = m_nCx / 2;
	lRect.top = m_nCy / 2;
	lRect.right = lRect.left + 200;
	lRect.bottom = lRect.top + 60;
	m_pShowToolTip->SetPos(lRect); 
	m_pRoundRect->SetVisible(false);
}

LPCTSTR CMagicPen_Main::GetWindowClassName() const
{
	return _T("CMagicPen_Main");
}

DuiLib::CDuiString CMagicPen_Main::GetSkinFile()
{
	return _T("MagicPen\\MagicPen_Main.xml");
}

DuiLib::CDuiString CMagicPen_Main::GetSkinFolder()
{
	return _T("skins");
} 


int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)  
{  
	UINT  num = 0;          // number of image encoders  
	UINT  size = 0;         // size of the image encoder array in bytes  

	ImageCodecInfo* pImageCodecInfo = NULL;  

	Gdiplus::GetImageEncodersSize(&num, &size);  
	if(size == 0)  
		return -1;  // Failure  

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));  
	if(pImageCodecInfo == NULL)  
		return -1;  // Failure  

	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);  

	for(UINT j = 0; j < num; ++j)  
	{  
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )  
		{  
			*pClsid = pImageCodecInfo[j].Clsid;  
			free(pImageCodecInfo);  
			return j;  // Success  
		}      
	}  

	free(pImageCodecInfo);  
	return -1;  // Failure  
}  


LRESULT CMagicPen_Main::OnLButtonDown( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{ 
	if (m_nControlMode == 1 || m_nControlMode == 2)
	{
		m_nControlMode = 1;
		StartDrawRoundRect(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}
	return __super::OnLButtonDown(uMsg, wParam, lParam, bHandled);
} 

LRESULT CMagicPen_Main::OnLButtonUp( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{	
	if (m_nControlMode == 1)
	{
		EndDrawRoundRect(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		m_nControlMode = 2;
	}
	return __super::OnLButtonUp(uMsg, wParam, lParam, bHandled);
}

LRESULT CMagicPen_Main::OnMouseMove( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if (m_nControlMode == 1)
	{
		DrawingRoundRect(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}
	return __super::OnMouseMove(uMsg, wParam, lParam, bHandled);
}

void CMagicPen_Main::ShowLocalWindows()
{ 
	if (!GetHWND() || !::IsWindow(GetHWND()))
	{	
		Create(NULL, _T("CMagicPen_Main"), WS_POPUP|WS_VISIBLE,SWP_NOACTIVATE, 0,0, m_nCx ,m_nCy);  
	} 
	m_hMagicPenAltha = NULL;
	m_pbmpScreen = NULL;
	m_pbmpCopyScreen = NULL;
	m_pbmpToolTip = NULL;
	m_pRoundRect->SetVisible(false);
	m_nStartTimeTick = GetTickCount();

	CRect lRect;
	lRect.left = m_nCx / 2;
	lRect.top = m_nCy / 2;
	lRect.right = lRect.left + 200;
	lRect.bottom = lRect.top + 60;
	//创建提示信息的位图
	if (m_pbmpToolTip == NULL)
	{ 
		//创建一个内存上下文
		CDC dc;
		HDC hdc = ::GetWindowDC(CWnd::GetDesktopWindow()->m_hWnd);
		dc.Attach(hdc); 
		CDC memDC;
		memDC.CreateCompatibleDC(&dc);
		m_pbmpToolTip = new CBitmap();
		m_pbmpToolTip->CreateCompatibleBitmap(&dc, lRect.Width(), lRect.Height());	
		CBitmap * oldbm = memDC.SelectObject(m_pbmpToolTip);
		memDC.BitBlt(0, 0, lRect.Width(), lRect.Height(), &dc, lRect.left, lRect.top, SRCCOPY);
		
		if (0)
		{
			//保存图片
			CLSID pngClsid;  
			GetEncoderClsid(L"image/png", &pngClsid);//此处以BMP为例，其它格式选择对应的类型，如JPG用L"image/jpeg"   

			Gdiplus::Bitmap *pbmSrc = Gdiplus::Bitmap::FromHBITMAP((HBITMAP)m_pbmpToolTip->GetSafeHandle(), NULL);  
			if( pbmSrc->Save(L"c:\\ScreenTmp.bmp", &pngClsid) == Ok)  
			{   
			}  
			delete pbmSrc; 
		}
		memDC.SelectObject(oldbm); 
		memDC.DeleteDC();
	}

	m_pShowToolTip->SetVisible(true);

	m_bHaveShow = false;
	m_ptStart.x = 0;
	m_ptStart.y = 0;
	m_ptEnd.x = 0;
	m_ptEnd.y = 0; 
	::SetWindowPos(GetHWND(), HWND_TOPMOST, 0, 0, m_nCx, m_nCy, SWP_NOACTIVATE|SWP_SHOWWINDOW); 

	if(m_nControlMode != 0)
	{
		// 打开要通知手机端
		BroadcastPostedEvent(EVT_MAGICPEN_OPERATION, MAGNIFIER_OPEN, 0, 0);
	}

	m_PaintManager.SetTimer(m_pShowToolTip, MAGICPEN_TIMER_ID, 500);
	m_pShowToolTip->OnEvent += MakeDelegate(this, &CMagicPen_Main::OnCloseWndEvent);
	m_bIsShowing = true;
}  

bool CMagicPen_Main::OnCloseWndEvent(void* pObj)
{
	TEventUI* pEvent = (TEventUI*)pObj;
	if (pEvent->Type == UIEVENT_TIMER)
	{
		if (pEvent->wParam == MAGICPEN_TIMER_ID)
		{			 
			int lStartTick = GetTickCount();
			if ((lStartTick - m_nStartTimeTick) > 2000)
			{
				m_PaintManager.KillTimer(m_pShowToolTip, MAGICPEN_TIMER_ID);	

				m_pShowToolTip->SetVisible(false);
			} 
		} 
	}
	return true;
} 

void CMagicPen_Main::StartDrawRoundRect(int nX, int nY)
{
	if (m_bHaveShow == true)
	{
		m_pbmpToolTip = NULL;
		m_pbmpScreen = NULL;
		m_pbmpCopyScreen = NULL;
		m_bMagicPenMode = false; 
		m_bHaveShow = false;
		m_pShowToolTip->SetVisible(false);		
		ShowWindow(SW_HIDE); 
		return;
	}
	m_pShowToolTip->SetVisible(false);

	if (m_bMagicPenMode == false)
	{
		m_bMagicPenMode = true;
	}
	if (this->IsMagicPenMode())
	{ 
		m_pRoundRect->SetVisible(true);
		CPoint point;
		point.x= nX;//GET_X_LPARAM(lParam);  
		point.y= nY;//GET_Y_LPARAM(lParam); 

		m_ptStart.x = point.x;
		m_ptStart.y = point.y;

		//OutputDebugString(_T("mousedown first\n"));
		//创建设备上下文的位图
		if (m_pbmpScreen == NULL)
		{ 
			//创建一个内存上下文
			CDC dc;
			HDC hdc = ::GetWindowDC(CWnd::GetDesktopWindow()->m_hWnd);
			dc.Attach(hdc); 
			CDC memDC;
			memDC.CreateCompatibleDC(&dc);
			m_pbmpScreen = new CBitmap();
			m_pbmpScreen->CreateCompatibleBitmap(&dc, m_nCx, m_nCy);	
			CBitmap * oldbm = memDC.SelectObject(m_pbmpScreen);
			memDC.BitBlt(0, 0, m_nCx, m_nCy, &dc, 0, 0, SRCCOPY);

			if (0)
			{
				//保存图片
				CLSID pngClsid;  
				GetEncoderClsid(L"image/png", &pngClsid);//此处以BMP为例，其它格式选择对应的类型，如JPG用L"image/jpeg"   

				Gdiplus::Bitmap *pbmSrc = Gdiplus::Bitmap::FromHBITMAP((HBITMAP)m_pbmpScreen->GetSafeHandle(), NULL);  
				if( pbmSrc->Save(L"c:\\ScreenTmp.bmp", &pngClsid) == Ok)  
				{   
				}  
				delete pbmSrc; 
			}
			memDC.SelectObject(oldbm);
			memDC.DeleteDC();
		} 

	}
}

void CMagicPen_Main::EndDrawRoundRect(int nX, int nY)
{
	if (this->IsMagicPenMode())
	{

		//OutputDebugString(_T("1\n")); 
		m_bMagicPenMode = false;

		if (m_pbmpCopyScreen == NULL)
		{
			m_pbmpCopyScreen = new CBitmap(); 	 
			if (::IsWindow(m_hMagicPenAltha))
			{
				::DestroyWindow(m_hMagicPenAltha);
			}
			///////////////////////////////////////////////////////
			CPoint point;
			CPoint lStart, lEnd;
			point.x=nX;//GET_X_LPARAM(lParam);  
			point.y=nY;//GET_Y_LPARAM(lParam);  

			if ((point.x >= m_ptStart.x) &&(point.y >= m_ptStart.y))
			{
				lStart.x = m_ptStart.x;
				lStart.y = m_ptStart.y;
				lEnd.x = point.x;
				lEnd.y = point.y;
			}
			else if ((point.x > m_ptStart.x) &&(point.y < m_ptStart.y))
			{ 
				lStart.x = m_ptStart.x;
				lStart.y = point.y;
				lEnd.x = point.x;
				lEnd.y = m_ptStart.y;
			}
			else if ((point.x < m_ptStart.x) &&(point.y < m_ptStart.y))
			{ 
				lStart.x = point.x;
				lStart.y = point.y;
				lEnd.x = m_ptStart.x;
				lEnd.y = m_ptStart.y;
			}
			else if ((point.x < m_ptStart.x) &&(point.y > m_ptStart.y))
			{  
				lStart.x = point.x;
				lStart.y = m_ptStart.y;
				lEnd.x = m_ptStart.x;
				lEnd.y = point.y;
			}
			else
			{ 
				lStart.x = m_ptStart.x;
				lStart.y = m_ptStart.y;
				lEnd.x = point.x;
				lEnd.y = point.y;
			}

			int nWidth = lEnd.x - lStart.x;
			int nHeight = lEnd.y -  lStart.y;
			int nLeft = lStart.x;// (m_nCx / 2 - nWidth * 3 / 2 );
			int nTop = lStart.y;//(m_nCy / 2 - nHeight * 3 / 2);
			CMagicPen_Altha* pMagicPen_Altha = NULL;  
			if ((nWidth == 0) || (nHeight == 0))//避免点击就弹起鼠标造成没有截图
			{
				nWidth = 20;
				nHeight = 20;
			} 

			//创建一个内存上下文
			CDC dc;
			HDC hdc = ::GetWindowDC(CWnd::GetDesktopWindow()->m_hWnd);
			dc.Attach(hdc);  

			CDC MemDCTmp;  
			MemDCTmp.CreateCompatibleDC(NULL); 
			//背景重绘
			CBitmap * oldSourcebm = MemDCTmp.SelectObject(m_pbmpScreen);
			dc.BitBlt(0, 0, m_nCx, m_nCy, &MemDCTmp, 0, 0, SRCCOPY);
			dc.SelectObject(oldSourcebm);   


			//提示符重绘
			CDC SourceMemDC; 
			SourceMemDC.CreateCompatibleDC(&dc);
			SourceMemDC.SelectObject(m_pbmpToolTip);
			CRect lRect;
			lRect.left = m_nCx / 2;
			lRect.top = m_nCy / 2;
			lRect.right = lRect.left + 200;
			lRect.bottom = lRect.top + 60;
			MemDCTmp.BitBlt(lRect.left,lRect.top , lRect.Width(), lRect.Height(), &SourceMemDC,0,0 , SRCCOPY); 

			if(0)
			{
				//保存图片
				CLSID pngClsid;  
				GetEncoderClsid(L"image/png", &pngClsid);//此处以BMP为例，其它格式选择对应的类型，如JPG用L"image/jpeg"   

				Gdiplus::Bitmap *pbmSrc = Gdiplus::Bitmap::FromHBITMAP((HBITMAP)m_pbmpScreen->GetSafeHandle(), NULL);  
				if( pbmSrc->Save(L"c:\\ScreenAltTmp.bmp", &pngClsid) == Ok)  
				{   
				}  
				delete pbmSrc;

			}


			//要复制的图像块
			CDC memDC;
			memDC.CreateCompatibleDC(&dc); 
			m_pbmpCopyScreen->CreateCompatibleBitmap(&MemDCTmp, nWidth, nHeight);	
			CBitmap * oldbm = memDC.SelectObject(m_pbmpCopyScreen);
			memDC.BitBlt(0, 0, nWidth, nHeight, &MemDCTmp, nLeft, nTop, SRCCOPY);
			memDC.SelectObject(oldbm); 
			//保存成文件  
			{  
				//L"image/bmp" L"image/jpeg"  L"image/gif" L"image/tiff" L"image/png"  
				CLSID pngClsid1;  
				GetEncoderClsid(L"image/png", &pngClsid1);//此处以BMP为例，其它格式选择对应的类型，如JPG用L"image/jpeg"   

				Gdiplus::Bitmap *pbmSrc = Gdiplus::Bitmap::FromHBITMAP((HBITMAP)m_pbmpCopyScreen->GetSafeHandle(), NULL);  
				if( pbmSrc->Save(L"c:\\MagicPenTmp.bmp", &pngClsid1) == Ok)  
				{   
				}  
				delete pbmSrc;  
			}  

			memDC.DeleteDC();
			oldbm->DeleteObject(); 
			MemDCTmp.DeleteDC();
			SourceMemDC.DeleteDC();


			if (!m_hMagicPenAltha || !::IsWindow(m_hMagicPenAltha))
			{
				//当前窗口隐藏
				m_pbmpCopyScreen = NULL;
				m_pbmpScreen = NULL;
				ShowWindow(SW_HIDE); 

				//弹出窗口
				pMagicPen_Altha = new CMagicPen_Altha(); 
				pMagicPen_Altha->Create(this->GetHWND(), _T("CMagicPen_Altha"), WS_POPUP | WS_VISIBLE,  WS_EX_TOOLWINDOW | WS_EX_TOPMOST);
				::SetWindowPos(pMagicPen_Altha->GetHWND(), HWND_DESKTOP, 0, 0, m_nCx, m_nCy, SWP_NOMOVE | SWP_NOACTIVATE); 
				pMagicPen_Altha->InitPenZoom(nLeft, nTop, nWidth, nHeight, m_pbmpCopyScreen);
				m_hMagicPenAltha	= pMagicPen_Altha->GetHWND();

			}
			if(m_nControlMode != 0)
			{
				// 放大要通知手机端
				CStream* pStream = new CStream(256);
				pStream->WriteDWORD(nLeft);
				pStream->WriteDWORD(nTop);
				pStream->WriteDWORD(nLeft + nWidth);
				pStream->WriteDWORD(nTop + nHeight);

				WRITE_LOG_LOCAL(_T("EndDrawRoundRect - LTRB:%d, %d, %d, %d"), nLeft, nTop, nLeft + nWidth, nTop + nHeight);
				
				BroadcastPostedEvent(EVT_MAGICPEN_OPERATION, MAGNIFIER_P2M_RECT, (LPARAM)pStream, 0);
				//	CPacketStream packStream;
				//	packStream.PackThreeCommand(ACTION_PUSH_DATA, PUSH_P2M_MAGNIFIER, MAGNIFIER_P2M_RECT);
				//	// 获取坐标百分比
				//	CPPTShellApp* pApp = (CPPTShellApp*)AfxGetApp();
				//	pApp->SendStreamPacket(packStream);
			}

		}  

		m_bHaveShow = TRUE; 
	}
}

void CMagicPen_Main::DrawingRoundRect(int nX, int nY)
{
	if (this->IsMagicPenMode())
	{    
		//OutputDebugString(_T("2\n"));
		CPoint point;

		point.x=nX;//GET_X_LPARAM(lParam);  
		point.y=nY;//GET_Y_LPARAM(lParam); 


		char szPos[128];

		m_ptEnd.x = point.x;
		m_ptEnd.y = point.y;
		//m_pRoundRect->SetFloat();
		RECT lRect;
		if ((point.x >= m_ptStart.x) &&(point.y >= m_ptStart.y))
		{
			lRect.left = m_ptStart.x;
			lRect.top = m_ptStart.y;
			lRect.right = point.x;
			lRect.bottom = point.y;
			sprintf(szPos, "1 pos:%d,%d, %d,%d\n", lRect.left, lRect.top, lRect.right, lRect.bottom);
		}
		else if ((point.x > m_ptStart.x) &&(point.y < m_ptStart.y))
		{
			lRect.left = m_ptStart.x;
			lRect.top = point.y;
			lRect.right = point.x;
			lRect.bottom = m_ptStart.y;
			sprintf(szPos, "2 pos:%d,%d, %d,%d\n", lRect.left, lRect.top, lRect.right, lRect.bottom);
		}
		else if ((point.x < m_ptStart.x) &&(point.y < m_ptStart.y))
		{
			lRect.left =point.x;
			lRect.top = point.y;
			lRect.right =  m_ptStart.x;
			lRect.bottom = m_ptStart.y;
			sprintf(szPos, "3 pos:%d,%d, %d,%d\n", lRect.left, lRect.top, lRect.right, lRect.bottom);
		}
		else if ((point.x < m_ptStart.x) &&(point.y > m_ptStart.y))
		{
			lRect.left =point.x;
			lRect.top =m_ptStart.y;
			lRect.right =  m_ptStart.x;
			lRect.bottom =  point.y;
			sprintf(szPos, "4 pos:%d,%d, %d,%d\n", lRect.left, lRect.top, lRect.right, lRect.bottom);
		}
		else
		{
			lRect.left = m_ptStart.x;
			lRect.top = m_ptStart.y;
			lRect.right = point.x;
			lRect.bottom = point.y;
			sprintf(szPos, "5 pos:%d,%d, %d,%d\n", lRect.left, lRect.top, lRect.right, lRect.bottom);
		}
		//OutputDebugString(_T(szPos));
		m_pRoundRect->SetPos(lRect, true);
		//OutputDebugString(_T("3\n"));

	}
}

void CMagicPen_Main::DrawRoundRect( int nState, int nX, int nY )
{
	if (nState == DRAWRECT_START)
	{
		StartDrawRoundRect(nX, nY);
	}
	else if (nState == DRAWRECT_DRAWING)
	{
		DrawingRoundRect(nX, nY);
	} 
	else
	{
		EndDrawRoundRect(nX, nY);
	}
}

void CMagicPen_Main::CloseLocalWindows()
{	
	if (m_bIsShowing)
	{ 
		if (GetHWND() && IsWindow(GetHWND()))
		{		 
			::DestroyWindow(GetHWND());		 
		} 
	}

	//if(m_nControlMode != 0)	// 通过手机端来关闭时，要通知其他手机端关闭放大镜
	{
		// 关闭要通知手机端
		BroadcastPostedEvent(EVT_MAGICPEN_OPERATION, MAGNIFIER_TOGGLE, 0, 0);
	}

	m_bIsShowing = false;
}

void CMagicPen_Main::TopMostView()
{
	::SetWindowPos(GetHWND(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE|SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE); 
}
