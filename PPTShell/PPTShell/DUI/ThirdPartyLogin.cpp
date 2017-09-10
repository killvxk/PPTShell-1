#include "stdafx.h"
#include "ThirdPartyLogin.h"
#include "NDCloud/NDCloudUser.h"
#include "DUI/GroupExplorer.h"
#include "Statistics/Statistics.h"

#define MSG_URLCHANGE  30001	

#define ThirdUrl L"http://p.101.com/redirect_third/?sso_host"
#define WeiboUrl L"https://api.weibo.com/oauth2/authorize?client_id=181994916&forcelogin=true&redirect_uri=http%3A%2F%2Fesp-sso.edu.web.sdp.101.com%2Fredirect_third%3Fsso_host%3Desp-sso.edu.web.sdp.101.com%26type%3Dsinawb%26fromway%3Dhttp%25253A%25252F%25252Fp.101.com"
#define QQUrl L"https://graph.qq.com/oauth2.0/authorize?response_type=code&client_id=101249669&redirect_uri=http%3A%2F%2Fesp-sso.edu.web.sdp.101.com%2Fredirect_third%3Fsso_host%3Desp-sso.edu.web.sdp.101.com%26type%3Dqq%26fromway%3Dhttp%25253A%25252F%25252Fp.101.com"
#define Account99Url L"https://openapi.99.com/Oauth2/Authorize?response_type=code&client_id=2852836&redirect_uri=http%3A%2F%2Fesp-sso.edu.web.sdp.101.com%2Fredirect_third%3Fsso_host%3Desp-sso.edu.web.sdp.101.com%26type%3Dnd99%26fromway%3Dhttp%25253A%25252F%25252Fp.101.com"


CThirdPartyLoginUI::CThirdPartyLoginUI()
{
	m_strWndClassName = _T("PPTShellWebWindow");
}

CThirdPartyLoginUI::~CThirdPartyLoginUI()
{
	
}

LPCTSTR CThirdPartyLoginUI::GetWindowClassName() const
{
	return _T("ThirdPartyLoginUI");
}

bool CThirdPartyLoginUI::Initialize( HINSTANCE hInstance )
{
	m_hInstance = hInstance;
	// Register window class for shadow window
	WNDCLASSEX wcex;

	memset(&wcex, 0, sizeof(wcex));

	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
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

LRESULT CALLBACK CThirdPartyLoginUI::WndProc( HWND hwnd, /* handle to window */ UINT uMsg, /* message identifier */ WPARAM wParam, /* first message parameter */ LPARAM lParam /* second message parameter */ )
{
	
	if (uMsg == WM_COPYDATA)
	{
		COPYDATASTRUCT* pCopyDataStruct = (COPYDATASTRUCT*)lParam;

		switch (pCopyDataStruct->dwData)
		{ 	
		case MSG_URLCHANGE:
			{
				TCHAR * pszUrl = new TCHAR[pCopyDataStruct->cbData + 1];
				pszUrl[pCopyDataStruct->cbData] = 0;
				memcpy(pszUrl, (const char *)pCopyDataStruct->lpData, pCopyDataStruct->cbData);
				tstring strUrl = pszUrl;
				delete pszUrl;

				tstring strFindString = _T("http://p.101.com/$sso/login_callback/?session_id=");
				tstring strErrorString =  _T("http://esp-sso.edu.web.sdp.101.com/?fromway=http%3A%2F%2Fp.101.com#/login");
				tstring strErrorString1 =  _T("error_code=21330");

				if(strUrl.find(strFindString) != tstring::npos )
				{
					int nPos = strUrl.find(_T('&'));
					if(nPos != tstring::npos)
					{
						
						tstring strSessionId = strUrl.substr(strFindString.length() , nPos - strFindString.length() );
						CThirdPartyLoginUI * pDlg = ThirdPartyLoginUI::GetInstance();
						pDlg->ShowWindow(false);
						NDCloudUser::GetInstance()->BearerTokensLogin(strSessionId, MakeHttpDelegate(pDlg, &CThirdPartyLoginUI::OnThirdLogin));
					}
					
				}else if (strErrorString==strUrl){
						CThirdPartyLoginUI * pDlg = ThirdPartyLoginUI::GetInstance();
						pDlg->ShowWindow(false);
						pDlg->ResetUrl();
				}else if (strUrl.find(strErrorString1) != tstring::npos ){
					CThirdPartyLoginUI * pDlg = ThirdPartyLoginUI::GetInstance();
					pDlg->ShowWindow(false);
					pDlg->ResetUrl();
				}
			}
			break;
		}
	}
	
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void CThirdPartyLoginUI::InitWindow()
{
// 	m_pWebBrowser = dynamic_cast<CWebBrowserUI*>(m_PaintManager.FindControl(_T("WebBrowser")));
// 
// 	m_pWebBrowser->SetDelayCreate(false);
// 	CCustomWebEventHandler *pWebHandle = new CCustomWebEventHandler;
// 	m_pWebBrowser->SetWebBrowserEventHandler(pWebHandle);

	//m_hWebWindow = CreateWindow(m_strWndClassName.c_str(), _T(""), WS_VISIBLE | WS_CHILD, 0, 0/*30*/, 0/*800*/, 0/*570*/ , GetHWND() , NULL, m_hInstance, NULL);
	m_hWebWindow = CreateWindow(m_strWndClassName.c_str(), _T(""), WS_VISIBLE | WS_CHILD, 0, 30, 800, 570 , GetHWND() , NULL, m_hInstance, NULL);

	m_pTitleLabel = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(_T("title")));
}

DuiLib::CDuiString CThirdPartyLoginUI::GetSkinFile()
{
	return _T("RightBar\\ThirdPartyLogin.xml");
}

DuiLib::CDuiString CThirdPartyLoginUI::GetSkinFolder()
{
	return CDuiString(_T("skins"));
}

CControlUI* CThirdPartyLoginUI::CreateControl( LPCTSTR pstrClass )
{
	return NULL;
}
void CThirdPartyLoginUI::ResetUrl(){
	CCefObject* pObejct = CefObject::GetInstance();
	pObejct->Init();
	pObejct->ShowFromDui(m_hWebWindow,L"about:blank");
}
void CThirdPartyLoginUI::Init( int nType )
{
	CCefObject* pObejct = CefObject::GetInstance();
	pObejct->Init();
// 	CRect rect;
// 	GetClientRect(GetHWND(), &rect);
// 	MoveWindow(m_hWebWindow, 0, 30 , rect.Width(), rect.Height(), TRUE);
	pObejct->ClearCookie(L"",L"");
	if(nType == 1)
	{
		m_pTitleLabel->SetText(_T("新浪微博登录"));
		pObejct->ShowFromDui(m_hWebWindow, WeiboUrl);
	}
	else if(nType == 2)
	{
		m_pTitleLabel->SetText(_T("QQ登录"));
		pObejct->ShowFromDui(m_hWebWindow,QQUrl);
	}
	else if(nType == 3)
	{
		m_pTitleLabel->SetText(_T("99通行证登录"));
		pObejct->ShowFromDui(m_hWebWindow,Account99Url);
	}

	m_nType = nType;
}

void CThirdPartyLoginUI::OnCloseBtn( TNotifyUI& msg )
{
	CCefObject* pObejct = CefObject::GetInstance();
	pObejct->Destroy(m_hWebWindow);
	ShowWindow(false);
	//Close();
}

bool CThirdPartyLoginUI::OnThirdLogin( void* pParam )
{
	THttpNotify* pNotify = static_cast<THttpNotify*>(pParam);

	if(pNotify->dwErrorCode != 0)
	{
//		ShowLoginTip(2, _T("当前网络不太好，无法登录，请检查网络连接"));
		return false;
	}
	CNDCloudUser* pUser = NDCloudUser::GetInstance();
	if( pUser->IsSuccess() )
	{
		HWND hwnd = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
//		SetTimer(hwnd, WM_LOGIN_COMPLETE, 1000, (TIMERPROC)CGroupExplorerUI::TimerProcComplete);
		if(  ::IsWindowVisible(CGroupExplorerUI::GetInstance()->GetHWND()))
				CGroupExplorerUI::GetInstance()->ShowWindow(false);
		NDCloudUser::GetInstance()->SetLoginType(m_nType);
		Statistics::GetInstance()->SetUserId(NDCloudUser::GetInstance()->GetUserId());
		NDCloudUser::GetInstance()->SetLoginComplete(true);//设置登陆完成 2016.01.27
		BroadcastEvent(EVT_LOGIN, m_nType ,0 ,0);
	}
	else
	{
		NDCloudUser::GetInstance()->SetLoginType(-1);
		tstring strErrorCode = pUser->GetErrorCode();
		tstring strErrorMessage = pUser->GetErrorMessage();
//		CToast::Toast(strErrorMessage.c_str());
	}

	return true;
}
