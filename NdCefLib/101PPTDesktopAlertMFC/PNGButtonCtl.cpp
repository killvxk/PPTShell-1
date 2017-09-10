#include "StdAfx.h"
#include "PNGButtonCtl.h"
#include "Resource.h"
//#include "MyUtil.cpp"


PNGButton::PNGButton(void)
{ 
	m_bIsChecked = false;
	m_bIsMouseHover = false;
	m_bIsMouseOver	= false; 
	m_bIsTracked = false;
	m_bIsEnable = true;
	m_bg = NULL;
	m_hoverBg = NULL;
	m_clickBg = NULL;
	m_DisableBg = NULL;
} 
PNGButton::~PNGButton(void) { }
IMPLEMENT_DYNCREATE(PNGButton, CWnd)  
BEGIN_MESSAGE_MAP(PNGButton, CWnd)  
	//{{AFX_MSG_MAP(MySelfWnd)  
//	ON_WM_LBUTTONDOWN()

	//}}AFX_MSG_MAP  
	ON_WM_PAINT()
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()  
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()  



static bool ImageFromIDResource(UINT nID, LPCTSTR sTR,Image * &pImg)
{
	bool ret = false;
	HINSTANCE hInst = AfxGetResourceHandle();
	HRSRC hRsrc = ::FindResource (hInst,MAKEINTRESOURCE(nID),sTR); // sTR:type
	if (!hRsrc)
		return FALSE;
	// load resource into memory
	DWORD len = SizeofResource(hInst, hRsrc);
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);
	//HGLOBAL lpRsrc = LoadResource(hInst, hRsrc);
	if (!lpRsrc)
		return FALSE;
	//add
	LPVOID pBuffer = LockResource(lpRsrc);
	if ( !pBuffer )
		return false;
	// Allocate global memory on which to create stream
	//HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, len);
	HGLOBAL m_hMem = GlobalAlloc(GMEM_ZEROINIT, len);
	BYTE* pmem = (BYTE*)GlobalLock(m_hMem);
	//HGLOBAL pmem = GlobalLock(m_hMem);
	memcpy(pmem,lpRsrc,len);
	IStream* pstm;
	if ( S_OK == CreateStreamOnHGlobal(m_hMem,FALSE,&pstm) )
	{
		// load from stream
		pImg = Gdiplus::Image::FromStream(pstm);
		if ( pImg )
			ret = true;
	}
	
	// free/release stuff
	GlobalUnlock(m_hMem);
	pstm->Release();
	FreeResource(lpRsrc);
	//GlobalFree()
	return ret;
}

//void PNGButton::OnLButtonDown(UINT nFlags, CPoint point)   
//{   
//	CWnd::OnLButtonDown(nFlags, point);  
//}  
//BOOL PNGButton::Create( UINT x,UINT y, CWnd* pParentWnd, UINT nID, UINT PngID,Gdiplus::Image* m_hoverBg, CCreateContext* pContext)   
//{  
//	// TODO: Add your specialized code here and/or call the base class  
//	// 重新注册窗口类,  
//	LPCTSTR lpszClassName=AfxRegisterWndClass( CS_HREDRAW|CS_VREDRAW ,  AfxGetApp()->LoadStandardCursor(IDC_ARROW), 
//		(HBRUSH)GetStockObject(TRANSPARENT), NULL) ;   
//	ImageFromIDResource(PngID,L"PNG",this->m_bg);
//	this->m_hoverBg=m_hoverBg;
//	
//	m_Width=m_bg->GetWidth();
//	m_Height=m_bg->GetHeight();
//
//	BOOL OK=CWnd::Create(NULL,NULL,WS_CHILDWINDOW|WS_VISIBLE, CRect(x,y,x+m_Width,m_Height+y),pParentWnd, nID, pContext); 
//	//|BS_OWNERDRAW
//	ModifyStyleEx(0, WS_EX_TRANSPARENT);// WS_EX_LAYERED||WS_EX_TRANSPARENT  
//	//SetLayeredWindowAttributes(TRANSPARENT, (byte)(255 * 1), LWA_COLORKEY); //LWA_ALPHA LWA_COLORKEY
//	
//	return OK;
//}


BOOL PNGButton::Create(UINT x,UINT y, CWnd* pParentWnd, UINT nID, UINT PngID,UINT PngID_hOver, UINT PngID_hDisable, UINT PngID_hClick, CCreateContext* pContext)
{
	LPCTSTR lpszClassName=AfxRegisterWndClass( CS_HREDRAW|CS_VREDRAW ,  AfxGetApp()->LoadStandardCursor(IDC_ARROW), (HBRUSH)GetStockObject(TRANSPARENT), NULL);
	ImageFromIDResource(PngID,_T("PNG"),this->m_bg);
	ImageFromIDResource(PngID_hOver,L"PNG",this->m_hoverBg);
	if ( 0 == PngID_hClick)
		ImageFromIDResource(PngID,L"PNG", this->m_clickBg);
	else
		ImageFromIDResource(PngID_hClick,L"PNG", this->m_clickBg);
	ImageFromIDResource(PngID_hDisable,L"PNG" ,this->m_DisableBg);
	if ( !m_bg || !m_hoverBg || !m_clickBg ||!m_DisableBg)
	{
		return false;
	}
	m_Width=m_bg->GetWidth();
	m_Height=m_bg->GetHeight(); 
	BOOL OK=CWnd::Create(NULL,NULL,WS_CHILDWINDOW|WS_VISIBLE, CRect(x,y,x+m_Width,m_Height+y),pParentWnd, nID, pContext);
	ModifyStyleEx(0, WS_EX_TRANSPARENT);// WS_EX_LAYERED||WS_EX_TRANSPARENT  
	//SetLayeredWindowAttributes(TRANSPARENT, (byte)(255 * 1), LWA_COLORKEY); //LWA_ALPHA LWA_COLORKEY
	
	return OK;
}


void PNGButton::OnPaint()
{  
	CPaintDC dc(this);    
	Graphics g(dc.m_hDC);   
	
	Bitmap bmp(this->m_Width,this->m_Height);
	Graphics* gBuf=Graphics::FromImage(&bmp);


	gBuf->DrawImage(this->m_bg,0,0); 
	//不同状态的显示 
	if ( !m_bIsEnable )
	{
		gBuf->DrawImage(m_DisableBg,0,0,0,0,m_Width,m_Height,Gdiplus::UnitPixel); 
		TRACE(L"m_bIsDisable\r\n"); 
	}
	else
	{
		if( m_bIsMouseHover )
		{
			gBuf->DrawImage(m_hoverBg,0,0,0,0,m_Width,m_Height,Gdiplus::UnitPixel); 
			TRACE(L"m_bIsMouseHover\r\n");   
		}
		if ( m_bIsChecked )
		{
			gBuf->DrawImage(m_clickBg,0,0,0,0,m_Width,m_Height,Gdiplus::UnitPixel); 
			TRACE(L"m_bIsMouseClick\r\n");  
		}
	}
	

	delete gBuf;
	g.DrawImage(&bmp,0,0);
	g.ReleaseHDC(dc.m_hDC);
	 
}
void PNGButton::PaintParent()
{  
	CRect   rect; 
	GetWindowRect(&rect); 
	GetParent()-> ScreenToClient(&rect); 
	GetParent()-> InvalidateRect(&rect,true);
}

LRESULT PNGButton::OnMouseHover(WPARAM wparam, LPARAM lparam)
{    
	m_bIsMouseHover=true;
	PaintParent(); 
	TRACE(L"OnMouseHover\r\n");
	return 0L;
}


LRESULT PNGButton::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{    
	m_bIsMouseOver	=   false; 
	m_bIsTracked	=   false;  
	m_bIsMouseHover =	false;
	m_bIsChecked	=	false;

	PaintParent(); 
	TRACE(L"OnMouseLeave\r\n");
	//add
	Invalidate();
	//CWnd::OnMouseLeave();
	return 0L;
}


void PNGButton::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_bIsMouseOver   =   true;  
	//TRACE(L"OnMouseMove\r\n");
	if(!m_bIsTracked) 
	{ 
		TRACKMOUSEEVENT   tme; 
		tme.cbSize		=   sizeof(TRACKMOUSEEVENT); 
		tme.dwFlags		=   TME_LEAVE|TME_HOVER; //
		tme.hwndTrack   =   GetSafeHwnd(); 
		tme.dwHoverTime	=   80; 
		m_bIsTracked = _TrackMouseEvent(&tme);    
	}   
	CWnd::OnMouseMove(nFlags, point);
}


void PNGButton::EnableWindow(bool enable)
{
	m_bIsEnable = enable;
	CWnd::EnableWindow(m_bIsEnable);
	PaintParent();
}
 
void PNGButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_bIsChecked = false;
	TRACE(L"ClickUp!\n"); 
	GetParent()->SendMessage(WM_COMMAND,GetDlgCtrlID()|0,(LONG)GetSafeHwnd());
	CWnd::OnLButtonUp(nFlags, point);
	PaintParent();
}


void PNGButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bIsChecked = true;
	TRACE(L"ClickDown!\n"); 
	GetParent()->SendMessage(WM_COMMAND,GetDlgCtrlID()|0,(LONG)GetSafeHwnd());
	CWnd::OnLButtonDown(nFlags, point);
	PaintParent();
}