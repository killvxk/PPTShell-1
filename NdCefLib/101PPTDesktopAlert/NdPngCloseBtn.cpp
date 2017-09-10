#include "StdAfx.h"
#include "NdPngCloseBtn.h"

CGdiPlusBitmapResource* CNdPngCloseBtn::pbgImage = NULL;
CGdiPlusBitmapResource* CNdPngCloseBtn::pbgImageClose = NULL;
CGdiPlusBitmapResource* CNdPngCloseBtn::pbgImageOver = NULL;
bool CNdPngCloseBtn::m_bIsMouseOver = false;

CNdPngCloseBtn::CNdPngCloseBtn(void)
{
}

CNdPngCloseBtn::~CNdPngCloseBtn(void)
{
}

ATOM CNdPngCloseBtn::NdBtnRegisterClass(HINSTANCE hInstance){
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= NdBtnCloseProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= _T("NDBTNCLOSECLASS");
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

BOOL CNdPngCloseBtn::NdBtnInitInstance(HINSTANCE hInstance,HWND hWndParent, int nCmdShow){
	HWND hWnd;
	NdBtnRegisterClass(hInstance);
	hWnd = CreateWindow(_T("NDBTNCLOSECLASS"), L"", WS_CHILD|WS_VISIBLE,
		330,13 , 28, 28, hWndParent, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	return TRUE;
}
void  CNdPngCloseBtn::NdDrawImage(HWND hWnd,HDC hdc)
{
	//加载图像
	if (m_bIsMouseOver){
		if (pbgImageClose==NULL){
			pbgImageClose = new CGdiPlusBitmapResource;
		}
		if (pbgImageClose){
			if(pbgImageClose->Load(IDB_CLOSEOVER, L"PNG")){
				Graphics graphics(hdc);
				RECT rcClient ;  
				SolidBrush brush(Color(17, 176, 182));  
				GetClientRect(hWnd,&rcClient);  
				graphics.FillRectangle(&brush,RectF(0, 0, rcClient.right-rcClient.left,rcClient.bottom-rcClient.top));
				graphics.DrawImage(*pbgImageClose,0,0);
			}
		}
	}else{
		if (pbgImage==NULL){
			pbgImage = new CGdiPlusBitmapResource;
		}
		if (pbgImage){
			if(pbgImage->Load(IDB_CLOSE, L"PNG")){

				//绘图
				Graphics graphics(hdc);
				RECT rcClient ;  
				SolidBrush brush(Color(17, 176, 182));  
				GetClientRect(hWnd,&rcClient);  
				graphics.FillRectangle(&brush,RectF(0, 0, rcClient.right-rcClient.left,rcClient.bottom-rcClient.top));
				graphics.DrawImage(*pbgImage,0,0);
			}
		}
	}
	//if(pbgImage) delete pbgImage;
	return;
}
LRESULT CALLBACK CNdPngCloseBtn::NdBtnCloseProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CREATE:
		{
		}
		break;
	case WM_MOUSEMOVE:
		if (!m_bIsMouseOver){
			TRACKMOUSEEVENT csTME;
			csTME.cbSize = sizeof (csTME);
			csTME.dwFlags = TME_LEAVE|TME_HOVER;
			csTME.hwndTrack = hWnd ;// 指定要 追踪 的窗口 
			csTME.dwHoverTime = 10;  // 鼠标在按钮上停留超过 10ms ，才认为状态为 HOVER
			::TrackMouseEvent(&csTME); // 开启 Windows 的 WM_MOUSELEAVE ， WM_MOUSEHOVER 事件支持
			m_bIsMouseOver   =   true; 
			RECT rcClient ;  
			GetClientRect(hWnd,&rcClient);  
			InvalidateRect(hWnd,&rcClient,true);
		}
		break;
	case WM_MOUSELEAVE:
		if (m_bIsMouseOver){
			m_bIsMouseOver   =   false; 
			RECT rcClient ;  
			GetClientRect(hWnd,&rcClient); 
			InvalidateRect(hWnd,&rcClient,true);
		} 
		break;
	case WM_LBUTTONDOWN:
		{
			HWND hPWnd = GetParent(hWnd);
			SendMessage(hPWnd,WM_DESTROY,0,0);
		}
		break;
	case WM_CLOSE:
		break;
	case WM_ERASEBKGND:
		{
			HDC hdc = (HDC)wParam;
			CNdPngCloseBtn::NdDrawImage(hWnd,hdc);
		}
		return 0;
	case WM_PRINTCLIENT:
		{
			HDC hdc = (HDC)wParam;
			CNdPngCloseBtn::NdDrawImage(hWnd,hdc);
		}
		return 0;
	case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);
			CNdPngCloseBtn::NdDrawImage(hWnd,hdc);
			EndPaint(hWnd, &ps);
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}