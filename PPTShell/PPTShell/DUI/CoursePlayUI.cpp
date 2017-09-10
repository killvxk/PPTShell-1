#include "stdafx.h"
#include "CoursePlayUI.h"
#include "AddQuestionWindow.h"
#include "CoursePlayerShadow.h"

#define MSG_URLCHANGE  30001	
#define MSG_URLFAILED  30002
#define MSG_URLPOPUP   30003
#define MSG_PLAYERLOADED 30004

CCoursePlayUI::CCoursePlayUI()
{
	m_strWndClassName = _T("CoursePlayWebUI");
	m_dwStartTickTime = GetTickCount();
	m_dwDBClickTime  = GetTickCount();
	m_dwDBSizeTime = GetTickCount();
	m_dwDBClickCount = 0;
	m_MainRect = CRect(0);
	m_bSubTitle = FALSE;
	m_nHiddenFlag = TRUE;
	m_dwDBClickFlag = true;
	m_nErrorCount = 0;
}

CCoursePlayUI::~CCoursePlayUI()
{

}

LPCTSTR CCoursePlayUI::GetWindowClassName() const
{
	return _T("CoursePlayUI");
}

bool CCoursePlayUI::Initialize( HINSTANCE hInstance )
{
	m_hInstance = hInstance;
	// Register window class for shadow window
	WNDCLASSEX wcex;

	memset(&wcex, 0, sizeof(wcex));

	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW |CS_DBLCLKS ;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= m_strWndClassName.c_str();
	wcex.hIconSm		= NULL;

	RegisterClassEx(&wcex);
	return true;
}
void CCoursePlayUI::SetClientRect(){
	if (m_MainRect==CRect(0)){
		HWND hwnd = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
		::GetWindowRect(hwnd, &m_MainRect);
	}
}
LRESULT CCoursePlayUI::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam){
	switch(uMsg)
	{
	case WM_SYSKEYDOWN:
		if (wParam == VK_F4)
		{
			HideAddQuestionWindow();
			HideCoursePlayerShadow();
			return false;
		}
		break;
	case WM_WINDOWPOSCHANGING:
		if (::IsIconic(GetHWND())){
			HideAddQuestionWindow();
		}else{
			ShowAddQuestionWindow();
		}
		break;
	case WM_SHOWWINDOW:
	
		break;
	case WM_NCHITTEST:
		{
			POINT pt; 
			pt.x = GET_X_LPARAM(lParam); 
			pt.y = GET_Y_LPARAM(lParam);
			::ScreenToClient(GetHWND(),&pt);

			RECT rcClient;
			::GetClientRect(GetHWND(), &rcClient);
			int nBorderSize = 3;
			if (!::IsZoomed(GetHWND())){
				if (pt.x<rcClient.left+nBorderSize*2&&pt.y<rcClient.top+nBorderSize*2)//左上角,判断是不是在左上角，就是看当前坐标是不是即在左边拖动的范围内，又在上边拖动的范围内，其它角判断方法类似
				{
					return HTTOPLEFT;
				}else if (pt.x>rcClient.right-nBorderSize*2 && pt.y<rcClient.top+nBorderSize*2)//右上角
				{
					return HTTOPRIGHT;
				}else if (pt.x<rcClient.left+nBorderSize*2 && pt.y>rcClient.bottom-nBorderSize*2)//左下角
				{
					return HTBOTTOMLEFT;
				}else if (pt.x>rcClient.right-nBorderSize*2 && pt.y>rcClient.bottom-nBorderSize*2)//右下角
				{
					return HTBOTTOMRIGHT;
				}else if (pt.x<rcClient.left+nBorderSize)
				{
					return HTLEFT;
				}else if (pt.x>rcClient.right-nBorderSize)
				{
					return HTRIGHT;
				}else if (pt.y<rcClient.top+nBorderSize)
				{
					return HTTOP;
				}if (pt.y>rcClient.bottom-nBorderSize)
				{
					return HTBOTTOM;          //以上这四个是上、下、左、右四个边
				}
			}
			UINT nHitTest;
			nHitTest = ::DefWindowProc(GetHWND(),uMsg,wParam,lParam);
			SHORT nKeyState = ::GetAsyncKeyState(MK_LBUTTON);
			int nKeyDown = (nKeyState & 0x8000) ? 1 : 0;
			int nKeyUP = (nKeyState & 0x8000) ? 0 : 1;
			//如果鼠标左键按下, GetAsyncKeyState 的返回值小于0
			DWORD dwStart = GetTickCount();//判断双击全屏
			if (dwStart-m_dwDBClickTime>200){
				m_dwDBClickCount = 0;
				m_dwDBClickFlag = FALSE;
			}
			if (nKeyDown==1){
				if (!m_dwDBClickFlag){
					m_dwDBClickFlag = TRUE;
					m_dwDBClickCount++;
					m_dwDBClickTime = GetTickCount();
				}
			}else if (nKeyUP==1){
				if (m_dwDBClickFlag){
					m_dwDBClickFlag = FALSE;
					m_dwDBClickCount++;
					m_dwDBClickTime = GetTickCount();
					if (m_dwDBClickCount==4){
						m_dwDBClickCount = 0;
						m_dwDBClickFlag = FALSE;
						OnZoomDBClick();
						return 0;
					}
				}
			}
			if(nHitTest == HTCLIENT &&  nKeyState< 0)
			{
				if (m_pCloseBtn){
					CRect rtButton(m_pCloseBtn->GetRelativePos());
					if(rtButton.PtInRect(pt)){ 
						if (nKeyUP){
							ShowWindow(false);
						}
					}else{
						nHitTest = HTCAPTION;
						return nHitTest;
					}
				}
			}
		}
		break;
	case WM_SIZE:{
			DWORD dwStart = GetTickCount();
			if (dwStart-m_dwStartTickTime>200){
				m_dwStartTickTime = dwStart;
				SizeMessage();
			}
			ShowAddQuestionWindow();

		}
		break;					 
	case WM_EXITSIZEMOVE:{
		SizeMessage();
		break;
	case WM_TIMER:
		::KillTimer(GetHWND(),1);
		SetWindowPos(GetHWND(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE); 
		SetWindowPos(GetHWND(), HWND_TOP, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE); 
		break;
	 }
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}
void CCoursePlayUI::SizeMessage(){
	RECT rectclient;
	GetClientRect(GetHWND(), &rectclient);

	MoveWindow(m_hWebWindow, rectclient.left+1,50, rectclient.right - rectclient.left-2, rectclient.bottom - rectclient.top-51, false);	
	CCefObject* pObejct = CefObject::GetInstance();
	pObejct->SizeChange(m_hWebWindow);
	ShowAddQuestionWindow();
}

LRESULT CALLBACK CCoursePlayUI::WndProc( HWND hwnd, /* handle to window */ UINT uMsg, /* message identifier */ WPARAM wParam, /* first message parameter */ LPARAM lParam /* second message parameter */ )
{
	switch(uMsg)
	{
	case WM_SYSKEYDOWN:
		if (wParam == VK_F4)
		{
			//HideAddQuestionWindow();
			//HideCoursePlayerShadow();
			return false;
		}
		break;
	case WM_SIZE:
		{
			return false;
		}
		break;
	case WM_COPYDATA:
		{
			COPYDATASTRUCT* pCopyDataStruct = (COPYDATASTRUCT*)lParam;

			switch (pCopyDataStruct->dwData)
			{ 	
			case MSG_PLAYERLOADED:
				{
					return false;
				}
				break;
			case MSG_CEF_ICRINVOKENATIVE:
				break;
			case MSG_CEFINFO:
				break;
			case MSG_URLCHANGE:
				{
					TCHAR * pszUrl = new TCHAR[pCopyDataStruct->cbData + 1];
					pszUrl[pCopyDataStruct->cbData] = 0;
					memcpy(pszUrl, (const char *)pCopyDataStruct->lpData, pCopyDataStruct->cbData);
					tstring strUrl = pszUrl;
					delete pszUrl;

					CCoursePlayUI * pDlg = CoursePlayUI::GetInstance();
					pDlg->GetQuestionResType(strUrl);

				}
				break;
			case MSG_URLFAILED:
				{
					CCoursePlayUI * pDlg = CoursePlayUI::GetInstance();
					pDlg->m_nErrorCount++;
					if (pDlg->m_nErrorCount<3){
						tstring strWorkDirectory = GetLocalPath();
						strWorkDirectory += _T("\\Package\\nodejs");

						tstring strExePath = strWorkDirectory;
						strExePath += _T("\\node.exe");

						ShellExecute(NULL, _T("open"), strExePath.c_str(), _T("app.js"), strWorkDirectory.c_str(), SW_HIDE);

						
						CCefObject* pObejct = CefObject::GetInstance();
						pObejct->Init();

						pObejct->CoursePlayDui(pDlg->m_hWebWindow,(WCHAR *) pDlg->m_wstrUrl.c_str());
					}else{
					}
					
				}
				break;
			}
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
TCHAR* CCoursePlayUI::GetQuestionResType( tstring szQuestionResDescription )
{
	tstring nTitle = NDCloudQuestionManager::GetInstance()->FindQuestionResType(m_nQuestType,szQuestionResDescription);
	if (nTitle.length()>0){
		SetTitle(nTitle);
	}
	return _T("");
}
void CCoursePlayUI::InitWindow()
{
	RECT rectclient;
	GetClientRect(GetHWND(), &rectclient);
	m_hWebWindow = CreateWindow(m_strWndClassName.c_str(), _T(""), WS_VISIBLE | WS_CHILD,  rectclient.left+1,50, rectclient.right - rectclient.left-2, rectclient.bottom - rectclient.top-51 , GetHWND() , NULL, m_hInstance, NULL);

	m_pCloseBtn	= static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("close")));
	m_pTitleLabel = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(_T("title")));
	m_pSubTitleLabel  = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(_T("subtitle")));
	m_pTopTitleLabel = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(_T("toptitle")));
	m_pTitleLabel->SetText(_T("编辑"));
	m_pTopTitleLabel->SetText(_T("101教育PPT>"));

	
}

void CCoursePlayUI::Init(wstring wstrUrl, CoursePlay_Type nType, BOOL nShow )
{
	HWND hwnd = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
	tstring nTitle = _T("");
	int nQuestionType = 0;
	m_wstrUrl = wstrUrl;
	
	switch(nType){
	case COURSEPLAY_ADD://显示习题类型和不显示插入
		m_nHiddenFlag = true;
		m_bSubTitle = true;
		nTitle = _T("新建习题");
		m_nQuestType = QUESTIONTYPE_NOEXIST;
		break;
	case COURSEPLAY_EDITOR:   //显示习题类型和不显示插入
		m_nHiddenFlag = true;
		m_bSubTitle = true;
		nTitle = _T("编辑习题");
		nQuestionType = 1;
		m_nQuestType = QUESTIONTYPE_EXIST;
		break;
	case COURSEPLAY_PREVIEW:  //不显示习题类型和显示插入
		m_nHiddenFlag = false;
		m_bSubTitle = false;
		nTitle = _T("预览");
		m_nQuestType = QUESTIONTYPE_EXIST;
		break;
	case COURSEPLAY_PLAYER:  //不显示习题类型和不显示插入
		m_nHiddenFlag = true;
		m_bSubTitle = false;
		nTitle = _T("播放");
		m_nQuestType = QUESTIONTYPE_EXIST;
		break;
	}

	ShowCoursePlayerShadow();

	RECT rectclient;
	GetClientRect(hwnd, &rectclient);
	if(GetHWND() == NULL)
	{
		int nWidth = 983; 
		int nHeight = 653; 

		if (rectclient.right-rectclient.left>=1254){
			nWidth = 1260; 
			nHeight = 737; 
		}
		Create(hwnd, _T(""), WS_POPUP| WS_SIZEBOX|WS_VISIBLE , NULL,0,0, nWidth, nHeight);
	}

	Sleep(1);
	CenterWindow();

	CCefObject* pObejct = CefObject::GetInstance();
	pObejct->Init();

	m_pSubTitleLabel->SetText(_T(""));
	m_pTitleLabel->SetText(nTitle.c_str());
	m_nErrorCount = 0;
	pObejct->SetQuestionType(m_hWebWindow,nQuestionType);
	pObejct->CoursePlayDui(m_hWebWindow, (WCHAR*)wstrUrl.c_str());
	
	if (nShow)
	{
		Sleep(10);
		ShowWindow(true);
		SetWindowPos(GetHWND(), HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE); 
		::SetTimer(GetHWND(),1,100,NULL);
	}
	
}

DuiLib::CDuiString CCoursePlayUI::GetSkinFile()
{
	return _T("CoursePlayer\\CoursePlayer.xml");
}

DuiLib::CDuiString CCoursePlayUI::GetSkinFolder()
{
	return CDuiString(_T("skins"));
}

CControlUI* CCoursePlayUI::CreateControl( LPCTSTR pstrClass )
{
	return NULL;
}

void CCoursePlayUI::Hide(){
	m_nHiddenFlag = true;
	HideAddQuestionWindow();
	HideCoursePlayerShadow();
	ShowWindow(false);
}
void CCoursePlayUI::SetTitle(tstring nTitle){
	if (m_bSubTitle){
		tstring nTitletemp = _T(">")+nTitle;
		m_pSubTitleLabel->SetText(nTitletemp.c_str());
	}
}
void CCoursePlayUI::OnCloseBtn( TNotifyUI& msg )
{
	m_nHiddenFlag = true;
	HideAddQuestionWindow();
	HideCoursePlayerShadow();
	CCefObject* pObejct = CefObject::GetInstance();
	if(pObejct)
	{
		pObejct->Init();
		pObejct->CoursePlayDui(m_hWebWindow,L"about:blank");
	}
	ShowWindow(false);
}
void CCoursePlayUI::OnZoomDBClick()
{
	m_dwDBClickCount = 0;
	m_dwDBClickFlag = FALSE;
	m_dwDBClickTime = 0;
	DWORD dwDBSizeTime =  GetTickCount();

	if (dwDBSizeTime - m_dwDBSizeTime>1000){
		if (::IsZoomed(GetHWND()))
		{	
			m_dwDBSizeTime =  GetTickCount();
			::ShowWindow(GetHWND(),SW_RESTORE);
		}
		else
		{
			m_dwDBSizeTime =  GetTickCount();
			::ShowWindow(GetHWND(),SW_MAXIMIZE);
		}
	}
}
BOOL CCoursePlayUI::HideAddQuestionWindow(){
	m_nHiddenFlag = TRUE;
	CCefObject* pObejct = CefObject::GetInstance();
	pObejct->Init();
	pObejct->CoursePlayDui(m_hWebWindow,L"about:blank");
	CAddQuestionWindow* pAddQuestionWindow = AddQuestionUI::GetInstance();

	if ( pAddQuestionWindow == NULL )
		return FALSE;

	if(IsWindow(pAddQuestionWindow->GetHWND()))
		pAddQuestionWindow->Hide();
		
	return TRUE;
}
void CCoursePlayUI::SetQuestionPath(tstring strQuestionPath){
	 m_strQuestionPath = strQuestionPath;
}
tstring CCoursePlayUI::GetQuestionPath(){
	return m_strQuestionPath;
}
BOOL CCoursePlayUI::SetQuestionInfo( tstring szPath, tstring szQuestionType, tstring szGuid){
	CAddQuestionWindow* pAddQuestionWindow = AddQuestionUI::GetInstance();

	if ( pAddQuestionWindow == NULL )
		return FALSE;
	pAddQuestionWindow->SetQuestionInfo(szPath,szQuestionType,szGuid);
	return TRUE;
}
BOOL CCoursePlayUI::HideCoursePlayerShadow(){
	CCoursePlayerShadow* pCoursePlayerShadow = CoursePlayerShadow::GetInstance();

	if ( pCoursePlayerShadow == NULL )
		return FALSE;

	if(IsWindow(pCoursePlayerShadow->GetHWND()))
		pCoursePlayerShadow->ShowWindow(SW_HIDE);

	::EnableWindow(pCoursePlayerShadow->GetHWND(), TRUE);
	return TRUE;
}

BOOL CCoursePlayUI::ShowCoursePlayerShadow()
{
	//增加提示
	CCoursePlayerShadow* pCoursePlayerShadow = CoursePlayerShadow::GetInstance();

	if ( pCoursePlayerShadow == NULL )
		return FALSE;

	if(pCoursePlayerShadow->GetHWND() == NULL){
		pCoursePlayerShadow->Create(AfxGetMainWnd()->GetSafeHwnd(), _T("CoursePlayerShadow"), WS_POPUP , 0, 0, 0, 0, 0);
	}

	RECT rectclient;
	::GetWindowRect(AfxGetMainWnd()->GetSafeHwnd(), &rectclient);
	//

	::MoveWindow(pCoursePlayerShadow->GetHWND(), rectclient.left, rectclient.top, rectclient.right-rectclient.left,rectclient.bottom-rectclient.top, false);
	
	Sleep(1);
	pCoursePlayerShadow->ShowWindow(SW_SHOW);

	pCoursePlayerShadow->SetShadowLayoutSize(rectclient);

	::EnableWindow(pCoursePlayerShadow->GetHWND(), FALSE); 
	SetWindowPos(GetHWND(), HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE); 
	return TRUE;
}
BOOL CCoursePlayUI::ShowAddQuestionWindow()
{
	if (m_nHiddenFlag){
		return TRUE;
	}
	//增加提示
	CAddQuestionWindow* pAddQuestionWindow = AddQuestionUI::GetInstance();

	if ( pAddQuestionWindow == NULL )
		return FALSE;

	if(pAddQuestionWindow->GetHWND() == NULL)
		pAddQuestionWindow->Create(GetHWND(), _T("AddSlideWindow"), WS_POPUP , 0, 0, 0, 0, 0);

	CRect rtMain;
	::GetWindowRect(GetHWND(), &rtMain);

	int nPosX = rtMain.right-150;	
	int nPosY = rtMain.bottom-70;

	if ( ::IsZoomed(GetHWND()) )
	{
		nPosX += 6;
		nPosY -= 6;
	}

	::MoveWindow(pAddQuestionWindow->GetHWND(), nPosX, nPosY, 86, 38, FALSE);
	if (::IsIconic(AfxGetMainWnd()->GetSafeHwnd()) )
	{
		pAddQuestionWindow->ShowWindow(SW_HIDE);
	}else{
		pAddQuestionWindow->ShowWindow(SW_SHOW);
	}

	return TRUE;
}