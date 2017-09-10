#include "StdAfx.h"
#include "QuestionPreviewFullScreenUI.h"
#include "cef\cefobj.h"
#include "Util\Util.h"

#define MSG_URLCHANGE  30001	
#define MSG_URLFAILED  30002
#define MSG_PLAYERLOADED 30004
static CQuestionPreviewFullScreenUI* m_pInstance = NULL;

CQuestionPreviewFullScreenUI::CQuestionPreviewFullScreenUI(void):m_pPreviewDlg(NULL),m_pLayoutQuestion(NULL),m_pGifLoading(NULL),m_pBtnClose(NULL),m_hWebWindow(NULL)
{
	m_sCurrentUrl = _T("");
	m_nCurrentIndex = 0;
	m_hThread = NULL;
}

CQuestionPreviewFullScreenUI::~CQuestionPreviewFullScreenUI(void)
{
	if ( m_hThread != NULL )
	{
		TerminateThread(m_hThread, 0);
		m_hThread = NULL;
	}
}

CQuestionPreviewFullScreenUI* CQuestionPreviewFullScreenUI::GetInstance()
{
	if(m_pInstance==NULL)
	{
		m_pInstance = new CQuestionPreviewFullScreenUI;
	}
	return m_pInstance;
}

bool CQuestionPreviewFullScreenUI::Initialize( HINSTANCE hInstance )
{
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
	wcex.lpszClassName	= _T("CoursePlayWebFullScreen");
	wcex.hIconSm		= NULL;

	RegisterClassEx(&wcex);
	return true;
}

LRESULT CALLBACK CQuestionPreviewFullScreenUI::WndProc( HWND hwnd, /* handle to window */ UINT uMsg, /* message identifier */ WPARAM wParam, /* first message parameter */ LPARAM lParam /* second message parameter */ )
{
switch(uMsg)
	{
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
			case MSG_CEF_ICRINVOKENATIVE:
				break;
			case MSG_CEFINFO:
				break;
			case MSG_ESC:
				{
					CQuestionPreviewFullScreenUI * pDlg = CQuestionPreviewFullScreenUI::GetInstance();
					if(pDlg)
					{
						pDlg->HideWindow();
					}
				}
				break;
			case MSG_PLAYERLOADED:
				{
					CQuestionPreviewFullScreenUI * pDlg = CQuestionPreviewFullScreenUI::GetInstance();
					if(pDlg)
					{
						pDlg->ShowWebWindow();
					}
				}
				break;
			case MSG_URLCHANGE:
				{
					TCHAR * pszUrl = new TCHAR[pCopyDataStruct->cbData + 1];
					pszUrl[pCopyDataStruct->cbData] = 0;
					memcpy(pszUrl, (const char *)pCopyDataStruct->lpData, pCopyDataStruct->cbData);
					tstring strUrl = pszUrl;
					delete pszUrl;
					CQuestionPreviewFullScreenUI * pDlg = CQuestionPreviewFullScreenUI::GetInstance();
					if(pDlg)
					{
						//pDlg->ShowQuestion(AnsiToUnicode(strUrl));
						/*CCefObject* pObejct = CefObject::GetInstance();
						pObejct->Init();
						pObejct->CoursePreview(pDlg->m_hWebWindow,(WCHAR *)strUrl.c_str());*/
					}
				}
				break;
			case MSG_URLFAILED:
				{
					CQuestionPreviewFullScreenUI* pDlg = CQuestionPreviewFullScreenUI::GetInstance();
					if(pDlg)
					{
						pDlg->AddRetryCount();
						if (pDlg->GetRetryCount()<3){
							tstring strUrl = pDlg->GetUrl();
							if(!strUrl.empty())
							{
								tstring strWorkDirectory = GetLocalPath();
								strWorkDirectory += _T("\\Package\\nodejs");
								tstring strExePath = strWorkDirectory;
								strExePath += _T("\\node.exe");
								ShellExecute(NULL, _T("open"), strExePath.c_str(), _T("app.js"), strWorkDirectory.c_str(), SW_HIDE);
								pDlg->ShowQuestion(strUrl);
								/*CCefObject* pObejct = CefObject::GetInstance();
								pObejct->Init();
								pObejct->CoursePreview(pDlg->m_hWebWindow,(WCHAR *)strUrl.c_str());*/
							}
						}
					}
				}
				break;
			}
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LPCTSTR CQuestionPreviewFullScreenUI::GetWindowClassName() const
{
	return _T("QuestionPreviewFullScreen");
}

void CQuestionPreviewFullScreenUI::InitWindow()
{
	m_pLayoutQuestion = dynamic_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("layoutQuestion")));
	m_pGifLoading = dynamic_cast<CGifAnimUI*>(m_PaintManager.FindControl(_T("gifLoading")));
	m_pBtnClose = dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnClose")));
}

DuiLib::CDuiString CQuestionPreviewFullScreenUI::GetSkinFile()
{
	return _T("Preview\\QuestionPreviewFullScreen.xml");
}

DuiLib::CDuiString CQuestionPreviewFullScreenUI::GetSkinFolder()
{
	return _T("skins");
}

CControlUI* CQuestionPreviewFullScreenUI::CreateControl( LPCTSTR pstrClass )
{
	if( _tcscmp(pstrClass, _T("GifAnim")) == 0 )
		return new CGifAnimUI;
	return __super::CreateControl(pstrClass);
}

void CQuestionPreviewFullScreenUI::OnShown()
{
	CRect rect;
	::GetWindowRect(this->GetHWND(), &rect);
	if(m_hWebWindow==NULL)
	{
		if(m_pLayoutQuestion)
		{
			RECT rectWeb = {0};
			rectWeb = m_pLayoutQuestion->GetPos();
			if(m_hWebWindow==NULL)
			{
				int x = 46;
				int y = rectWeb.top;
				int width = rectWeb.right - rectWeb.left - 92;
				int height = rectWeb.bottom - rectWeb.top;
				m_hWebWindow = CreateWindow(_T("CoursePlayWebFullScreen"), _T(""), WS_POPUP | WS_VISIBLE | WS_CHILD,  x,y,width,height , GetHWND() , NULL, AfxGetApp()->m_hInstance, NULL);
			}
			ShowQuestion(m_sCurrentUrl);
		}
	}
	if(m_pBtnClose)
	{
		int width = m_pBtnClose->GetFixedWidth();
		int heigth = m_pBtnClose->GetFixedHeight();
		RECT rectBtnClose  = {0};
		rectBtnClose.top = rect.top + 10;
		rectBtnClose.bottom = rectBtnClose.top + heigth;
		rectBtnClose.right = rect.right - 10;
		rectBtnClose.left = rectBtnClose.right - width;
		m_pBtnClose->SetVisible(true);
		m_pBtnClose->SetFloat(true);
		m_pBtnClose->SetPos(rectBtnClose);
	}
}

LRESULT CQuestionPreviewFullScreenUI::OnKeyDown( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if( uMsg == WM_KEYDOWN )
	{
		TNotifyUI msg;
		if ((wParam == VK_UP || wParam == VK_LEFT))
		{
			HideQuestion();
			m_pPreviewDlg->OnBtnQuestionPreviewClick(msg);

		}
		else if (wParam == VK_DOWN || wParam == VK_RIGHT)
		{
			HideQuestion();
			m_pPreviewDlg->OnBtnQuestionNextClick(msg);
		}
		else if(wParam == VK_RETURN || wParam == VK_ESCAPE)
		{
			HideWindow();
		}
	}
	return TRUE;
}

void CQuestionPreviewFullScreenUI::OnBtnCloseClick( TNotifyUI& msg )
{
	HideWindow();
}

void CQuestionPreviewFullScreenUI::ShowQuestion( tstring url )
{
	m_sCurrentUrl = url;
	CCefObject* pObejct = CefObject::GetInstance();
	if(pObejct&&m_hWebWindow)
	{
		pObejct->Init();
		pObejct->SetQuestionType(m_hWebWindow,0);
		pObejct->CourseFullScreenPreview(m_hWebWindow,(WCHAR*)Str2Unicode(url).c_str());
		if ( m_hThread != NULL )
		{
			TerminateThread(m_hThread, 0);
			m_hThread = NULL;
		}
		m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScanThread, this, 0, NULL);
	}
	SetFocus(this->GetHWND());
}

void CQuestionPreviewFullScreenUI::HideQuestion()
{
	if(m_pGifLoading)
	{
		m_pGifLoading->PlayGif();
	}
	if(m_hWebWindow)
	{
		::ShowWindow(m_hWebWindow, SW_HIDE);
	}
}

tstring CQuestionPreviewFullScreenUI::GetUrl()
{
	return m_sCurrentUrl;
}

void CQuestionPreviewFullScreenUI::AddRetryCount()
{
	m_nRetryCount++;
}

int CQuestionPreviewFullScreenUI::GetRetryCount()
{
	return m_nRetryCount;
}

void CQuestionPreviewFullScreenUI::InitData(CStream* pStream)
{
	if(pStream)
	{
		pStream->ResetCursor();
		m_sCurrentUrl = pStream->ReadString();
		ShowQuestion(m_sCurrentUrl);
	}
}

void CQuestionPreviewFullScreenUI::SetPreviewDlg( CQuestionPreviewDialogUI* pDlg )
{
	if (!m_pInstance->GetHWND() || !IsWindow(m_pInstance->GetHWND()))
	{
		m_pInstance->Create(pDlg->GetHWND(), _T("QuestionPreviewFullScreen"), WS_POPUP, 0, 0, 0, 0, 0);
		MONITORINFO oMonitor = {0};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(this->GetHWND(), MONITOR_DEFAULTTONEAREST), &oMonitor);
		int m_nScreenWidth = oMonitor.rcMonitor.right - oMonitor.rcMonitor.left;
		int m_nScreenHeight	= oMonitor.rcMonitor.bottom - oMonitor.rcMonitor.top;
		::MoveWindow(m_hWnd, 0,0, m_nScreenWidth, m_nScreenHeight, false);
	}
	m_pInstance->m_pPreviewDlg = pDlg;
}

void CQuestionPreviewFullScreenUI::ShowWebWindow()
{
	if(m_hWebWindow&&IsWindowVisible(this->GetHWND()))
	{
		::ShowWindow(m_hWebWindow, SW_SHOW);
	}
	if(m_pGifLoading)
	{
		m_pGifLoading->StopGif();
	}
	if ( m_hThread != NULL )
	{
		TerminateThread(m_hThread, 0);
		m_hThread = NULL;
	}
	::SetFocus(this->GetHWND());
}

void CQuestionPreviewFullScreenUI::HideWindow()
{
	this->ShowQuestion(_T("about:blank"));
	HideQuestion();
	this->ShowWindow(false);
	if ( m_hThread != NULL )
	{
		TerminateThread(m_hThread, 0);
		m_hThread = NULL;
	}
	if(m_pPreviewDlg->GetIsWebWindowShown())
	{
		m_pPreviewDlg->ShowWebWindow();
	}
}

DWORD  WINAPI CQuestionPreviewFullScreenUI::ScanThread( LPARAM lParam )
{
	Sleep(10*1000);
	CQuestionPreviewFullScreenUI* pDlg = CQuestionPreviewFullScreenUI::GetInstance();
	if(pDlg)
	{
		pDlg->ShowWebWindow();
	}
	return 1;
}
