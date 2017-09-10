#include "StdAfx.h"
#include "NdUI.h"
#include "NdPngBtn.h"
#include "NdPngCloseBtn.h"

namespace NdUI{

	std::wstring CNdUI::szFile = L"";
	int CNdUI::m_iTimeClose = 0;
	HINSTANCE CNdUI::m_hInst = NULL;
	CGdiPlusBitmapResource* CNdUI::pbgImage = NULL;
	bool CNdUI::m_bIsMouseOver = false;

	CNdUI::CNdUI(void)
	{
	}

	CNdUI::~CNdUI(void)
	{

	}
	void CNdUI::ClearCloseTime(){
		m_iTimeClose = 0;
	}
	void CNdUI::AddCloseTime(){
		m_iTimeClose++;
	}
	BOOL CNdUI::IsCloseTime(){
		BOOL nResult = false;
		if (m_iTimeClose>=3){
			nResult = true;
		}
		return nResult;
	}
	

	void  CNdUI::NdDrawImage(HDC hdc)
	{
		//加载图像
		if (pbgImage==NULL){
			pbgImage = new CGdiPlusBitmapResource;
		}
		if (pbgImage){
			if(pbgImage->Load(IDB_ALERT, L"PNG")){

				//绘图
				Graphics graphics(hdc);
				graphics.DrawImage(*pbgImage,0,0);
			}
		}
		//if(pbgImage) delete pbgImage;
		return;
	}
	LRESULT CALLBACK CNdUI::NdWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		PAINTSTRUCT ps;
		HDC hdc;

		switch (message)
		{
		case WM_CREATE:
			{
				CNdPngCloseBtn nCloseBtn;
				CNdPngBtn nLookBtn;
				nLookBtn.NdBtnInitInstance(CNdUI::m_hInst,hWnd,SW_SHOW);
				nCloseBtn.NdBtnInitInstance(CNdUI::m_hInst,hWnd,SW_SHOW);
				RECT rc;
				GetClientRect(hWnd,&rc);
				int nWidth=rc.right-rc.left;
				int nHeight=rc.bottom-rc.top;
				//取出桌面工作区
				SystemParametersInfo(SPI_GETWORKAREA,NULL,&rc,NULL);
				SetWindowLong(hWnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW|WS_EX_LAYERED);
				SetWindowPos(hWnd,0,rc.right-nWidth,rc.bottom-nHeight,0,0,SWP_NOZORDER|SWP_NOSIZE|SWP_NOREDRAW);
				//SetWindowLong(hWnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW|WS_EX_LAYERED|WS_EX_TRANSPARENT);
				SetWindowPos(hWnd, HWND_TOPMOST,0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );
				SetLayeredWindowAttributes(hWnd,RGB(0,0,0),0,LWA_COLORKEY);
				AnimateWindow(hWnd,1000,AW_SLIDE|AW_VER_NEGATIVE);
				SetTimer(hWnd, 1, 1000, NULL);  
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
				KillTimer(hWnd,1);
				ClearCloseTime();
			}
			break;
		case WM_MOUSELEAVE:
			if (m_bIsMouseOver){
				m_bIsMouseOver   =   false; 
				SetTimer(hWnd, 1, 1000, NULL); 
			} 
			break;
		case WM_TIMER:
			AddCloseTime();
			if (IsCloseTime()){
				KillTimer(hWnd, 1); 
				PostMessage(hWnd,WM_CLOSE,0,0);
			}
			break;
		case WM_LBUTTONDOWN:
			{
				int a = 0;
			}
			break;
		case WM_CLOSE:
			AnimateWindow(hWnd,1000,AW_HIDE|AW_VER_POSITIVE);
			return DefWindowProc(hWnd, message, wParam, lParam);
		case WM_ERASEBKGND:
			{
				HDC hdc = (HDC)wParam;
				CNdUI::NdDrawImage(hdc);
			}
			return 0;
		case WM_PRINTCLIENT:
			{
				HDC hdc = (HDC)wParam;
				CNdUI::NdDrawImage(hdc);
			}
			return 0;
		case WM_PAINT:
			{
				hdc = BeginPaint(hWnd, &ps);
				CNdUI::NdDrawImage(hdc);
				EndPaint(hWnd, &ps);
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}

	ATOM CNdUI::NdRegisterClass(HINSTANCE hInstance)
	{
		WNDCLASSEX wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= NdWndProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= hInstance;
		wcex.hIcon			= NULL;
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= NULL;
		wcex.lpszMenuName	= NULL;
		wcex.lpszClassName	= szWindowClass;
		wcex.hIconSm		= NULL;

		return RegisterClassEx(&wcex);
	}

	BOOL CNdUI::NdInitInstance(HINSTANCE hInstance, int nCmdShow)
	{
		HWND hWnd;

		m_hInst = hInstance; // Store instance handle in our global variable

		hWnd = CreateWindow(szWindowClass, szTitle, WS_POPUP|WS_VISIBLE,
			0, 0, 370, 260, NULL, NULL, hInstance, NULL);

		if (!hWnd)
		{
			return FALSE;
		}
		ShowWindow(hWnd, SW_SHOW);
		UpdateWindow(hWnd);
		return TRUE;
	}

	BOOL CNdUI::NdMain(HINSTANCE hInstance,int nCmdShow){
		MSG msg;

		ULONG_PTR gdipludToken;
		GdiplusStartupInput gdiplusInput;
		GdiplusStartup(&gdipludToken,&gdiplusInput,NULL);

		LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
		LoadString(hInstance, IDC_MY101PPTDESKTOPALERT, szWindowClass, MAX_LOADSTRING);
		NdRegisterClass(hInstance);

		if (!NdInitInstance (hInstance, nCmdShow))
		{
			return FALSE;
		}

		while (GetMessage(&msg, NULL, 0, 0))
		{
			if (!TranslateAccelerator(msg.hwnd, NULL, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	
		GdiplusShutdown(gdipludToken);
		return (int) msg.wParam;
	}
}