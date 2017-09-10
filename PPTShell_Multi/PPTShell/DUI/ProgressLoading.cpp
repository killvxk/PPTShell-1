#include "StdAfx.h"
#include "ProgressLoading.h"
#include "Util/Util.h"
#include "DUI/GroupExplorer.h"
#include "json/json.h"

#define TimerId_ShowState 12
#define EduPlatformHostLC							_T("esp-lifecycle.web.sdp.101.com")
#define EduPlatformLCDownloadCoursewares			_T("/v0.6/coursewares/%s/downloadurl?uid=%u&key=source")

CProgressLoadingUI::CProgressLoadingUI(void)
{

}

CProgressLoadingUI::~CProgressLoadingUI(void)
{

}

DuiLib::CDuiString CProgressLoadingUI::GetSkinFolder()
{
	return CDuiString(_T("skins"));
}

DuiLib::CDuiString CProgressLoadingUI::GetSkinFile()
{
	return _T("ProgressMasker\\loading.xml");
}

void CProgressLoadingUI::InitWindow()
{
	__super::InitWindow();

	m_laySaving		= dynamic_cast<CContainerUI*>(m_PaintManager.FindControl(_T("loading")));
	m_laySuccess	= dynamic_cast<CContainerUI*>(m_PaintManager.FindControl(_T("success")));
	m_layFail		= dynamic_cast<CContainerUI*>(m_PaintManager.FindControl(_T("fail")));
	m_proDownload	= dynamic_cast<CProgressUI*>(m_PaintManager.FindControl(_T("progress")));
	m_pTitle		= dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(_T("title")));

	m_laySaving->OnEvent += MakeDelegate(this, &CProgressLoadingUI::OnEvent);
}

LPCTSTR CProgressLoadingUI::GetWindowClassName( void ) const
{
	return _T("CProgressLoadingUI");
}

void CProgressLoadingUI::ShowWindow( bool bShow /*= true*/, bool bTakeFocus /*= true */ )
{
	if (! GetHWND()
		|| !IsWindow(GetHWND()))
	{
		HWND hwnd = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
		this->Create(hwnd, this->GetWindowClassName(), WS_POPUP, 0);
		CRect rect;
		::GetWindowRect(hwnd, &rect);
		MoveWindow(GetHWND(), rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
		//this->CenterWindow();
	}

	__super::ShowWindow(bShow, bTakeFocus);
}

void CProgressLoadingUI::OnBtnClose( TNotifyUI& msg )
{

}


void CProgressLoadingUI::SetProgress( int nPos )
{
	m_proDownload->SetValue(nPos);
}

void CProgressLoadingUI::ShowProgress( bool bVisible )
{
	m_proDownload->SetVisible(bVisible);
}

//
// start
//
BOOL CProgressLoadingUI::Start(tstring strCommandLine, CHttpDelegateBase &OnCompleteDelegate, CHttpDelegateBase &OnProgressDelegate)
{
	//AfxMessageBox(strCommandLine.c_str());

	m_pTitle->SetText(_T("解析启动命令中..."));

	// parse command line
	BOOL res = ParseCommandLine(strCommandLine);
	if( !res )
	{
		ShowWindow(false);
		Close();

		CToast::Toast(_T("启动命令错误!"));
		WRITE_LOG_LOCAL(_T("启动命令错误: %s"), strCommandLine.c_str());
		return FALSE;
	}

	// delete file 
	tstring strNDCloudDir = NDCloudFileManager::GetInstance()->GetNDCloudDirectory();
	strNDCloudDir += _T("\\Course\\");
	strNDCloudDir += m_strCourseGuid;

	DeleteDir(strNDCloudDir.c_str());

	m_pTitle->SetText(_T("账号登录中..."));

	// login user by token
	NDCloudUser::GetInstance()->TokenLogin(_T("class.101.com"),
		m_strCourseGuid,
		_T("pptshell"), 
		m_strMac, 
		m_strNonce,
		m_strAccessToken,
		MakeHttpDelegate(this, &CProgressLoadingUI::OnUserLogined));


	return TRUE;
}

bool CProgressLoadingUI::OnUserLogined(void *param)
{
	CNDCloudUser* pUser = NDCloudUser::GetInstance();
	int nStep = pUser->GetCurStep();

	if( nStep == UCSTEP_LOGIN )
	{
		BOOL res = NDCloudUser::GetInstance()->IsSuccess();
		if( !res )
		{
			ShowWindow(false);
			Close();

			CToast::Toast(_T("账号登录失败!"));
			return false;
		}

		DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();

		// download course file
		TCHAR szUrl[1024];
		_stprintf_s(szUrl, EduPlatformLCDownloadCoursewares, m_strCourseGuid.c_str(), dwUserId);

		string strAuthorization = NDCloudUser::GetInstance()->GetAuthorizationHeader(EduPlatformHostLC, szUrl, "GET");

		tstring strHeader = _T("Content-Type: application/json");
		strHeader += _T("\r\n");
		strHeader += Ansi2Str(strAuthorization);


		m_pTitle->SetText(_T("课件下载地址获取中..."));


		HttpDownloadManager::GetInstance()->AddTask(EduPlatformHostLC, 
			szUrl, 
			strHeader.c_str(),
			_T("GET"),
			_T(""), 
			INTERNET_DEFAULT_HTTP_PORT, 
			MakeHttpDelegate(this, &CProgressLoadingUI::OnDownloadPathObtained), 
			MakeHttpDelegate(NULL), 
			MakeHttpDelegate(NULL), 
			FALSE);

		BroadcastEvent(EVT_LOGIN, 0 ,0 ,0);
	}


	return true;
}


bool CProgressLoadingUI::OnDownloadPathObtained( void * pParam )
{
	THttpNotify* pNotify = (THttpNotify*)pParam;

	pNotify->pData[pNotify->nDataSize] = '\0';
	pNotify->pUserData = this;

	string str = Utf8ToAnsi(pNotify->pData);

	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(str, root);
	if( !res )
		return false;

	tstring strUrl;
	tstring strGuid;

	if( !root["access_url"].isNull() )
		strUrl = Ansi2Str(root["access_url"].asCString());

	if( !root["uuid"].isNull() )
		strGuid = Ansi2Str(root["uuid"].asCString());

	m_pTitle->SetText(_T("课件下载中..."));

	NDCloudDownloadCourseFile(strUrl, strGuid, _T(""), CourseFilePPT, 
		MakeHttpDelegate(this, &CProgressLoadingUI::OnCourseFileDownloaded), MakeHttpDelegate(this, &CProgressLoadingUI::OnCourseFileDownloading));

	return true;
}

bool CProgressLoadingUI::OnCourseFileDownloading(void* param)
{
	THttpNotify* pHttpNotify = (THttpNotify*)param;

	int nPos = (int)(pHttpNotify->fPercent * m_proDownload->GetMaxValue());
	this->SetProgress(nPos);

	TCHAR szTip[MAX_PATH]		= {0};
	TCHAR szSpeed[MAX_PATH]		= {0};
	TCHAR szRemain[MAX_PATH]	= {0};
	TCHAR szElapse[MAX_PATH]	= {0};
	TCHAR szSize[MAX_PATH]		= {0};
	int		nTemp				= 0;

	_stprintf_s(szSpeed, _T("%.1f %s"), 
		pHttpNotify->nSpeed < 1000 ? pHttpNotify->nSpeed : (pHttpNotify->nSpeed  * 1.0f / 1024),
		pHttpNotify->nSpeed < 1000 ? _T("KB/S") : _T("MB/S"));

	nTemp = pHttpNotify->nElapseTime;
	_stprintf_s(szElapse, _T("%02d:%02d:%02d"), 
		pHttpNotify->nElapseTime / 3600,
		(nTemp %= 3600, nTemp / 60),
		pHttpNotify->nElapseTime % 60);

	nTemp = pHttpNotify->nRemainTime;
	_stprintf_s(szRemain, _T("%02d:%02d:%02d"), 
		pHttpNotify->nRemainTime / 3600,
		(nTemp %= 3600, nTemp / 60),
		pHttpNotify->nRemainTime % 60);


	if (pHttpNotify->nTotalSize < 1000 )
	{
		_stprintf_s(szSize, _T("%.2f B"), 
			(pHttpNotify->nTotalSize  * 1.0f));
	}
	else if (pHttpNotify->nTotalSize < 1000 * 1000 )
	{
		_stprintf_s(szSize, _T("%.2f KB"), 
			(pHttpNotify->nTotalSize  * 1.0f / 1024));
	}
	else if (pHttpNotify->nTotalSize < 1000 * 1000 * 1000 )
	{
		_stprintf_s(szSize, _T("%.2f MB"), 
			(pHttpNotify->nTotalSize  * 1.0f / (1024 * 1024)));
	}

	_stprintf_s(szTip, _T("下载速度：%s<n>下载用时：%s<n>剩余时间：%s<n>文件大小：%s<n>"), 
		szSpeed,
		szElapse,
		szRemain,
		szSize);


	m_proDownload->SetToolTip(szTip);
	return true;
}

bool CProgressLoadingUI::OnCourseFileDownloaded(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;

	ShowWindow(false);
	Close();

	COPYDATASTRUCT copyData;
	copyData.dwData = MSG_OPEN_FILE;
	copyData.cbData = pNotify->strFilePath.length();
	copyData.lpData = (LPVOID)pNotify->strFilePath.c_str();

	::SendMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), WM_COPYDATA, MSG_OPEN_FILE, (LPARAM)&copyData);

	return true;
}


//
// pptshell:
// id=394a1f46-b263-4ba5-8869-ebc7595eeae7,
// chapter_id=90a3732d-d9c4-4151-ab1f-61f7740fa31f,
// mode=edit,
// auth=TUFDIGlkPSc2NjZkYTM3Yy1iMzIxLTQ2NDYtYTBlNS1hMTdkNjVjYmJkMTcnLG5vbmNlPScxNDIwMzU3MDQ5MTg1OmFmZGdmNDU0JyxtYWM9J3UwSGxSZXhCN3oyTkJKWTlCcWJ6MUVwVmRiM2NJVHBUMENyK25GREVXOE09Jw==
//
BOOL CProgressLoadingUI::ParseCommandLine(tstring strCommandLine)
{
	int pos = strCommandLine.find("pptshell:");
	if( pos == -1 )
		return FALSE;

	strCommandLine = strCommandLine.substr(pos+_tcslen("pptshell:"));

	vector<tstring> vecStrings = SplitString(strCommandLine, strCommandLine.length(), _T(','), false);
	if( vecStrings.size() < 4 )
		return FALSE;

	tstring strCourseGuid	= vecStrings[0];
	tstring strChapterGuid	= vecStrings[1];
	tstring strMode			= vecStrings[2];
	tstring strAuth			= vecStrings[3];

	// course guid
	vecStrings = SplitString(strCourseGuid, strCourseGuid.length(), _T('='), false);
	if( vecStrings.size() == 2 )
		m_strCourseGuid = vecStrings[1];

	// chapter guid
	vecStrings = SplitString(strChapterGuid, strChapterGuid.length(), _T('='), false);
	if( vecStrings.size() == 2 )
		m_strChapterGuid = vecStrings[1];

	// mode
	vecStrings = SplitString(strMode, strMode.length(), _T('='), false);
	if( vecStrings.size() < 2 )
		return FALSE;

	m_strMode = vecStrings[1];

	// auth
	pos = strAuth.find(_T("auth="));
	if( pos == -1 )
		return FALSE;

	m_strAuth = strAuth.substr(pos+_tcslen(_T("auth=")));

	// user login by token
	char szMacToken[MAX_PATH] = {0};
	int len = base64_decode((char*)m_strAuth.c_str(), m_strAuth.length(), szMacToken, MAX_PATH);
	if( len == 0 )
		return FALSE;

	// MAC id='666da37c-b321-4646-a0e5-a17d65cbbd17',nonce='1420357049185:afdgf454',mac='u0HlRexB7z2NBJY9Bqbz1EpVdb3cITpT0Cr+nFDEW8M='
	vecStrings = SplitString(Ansi2Str(szMacToken), strlen(szMacToken), _T(','), false);

	tstring strAccessToken	= vecStrings[0];
	tstring strNonce		= vecStrings[1];
	tstring strMac			= vecStrings[2];

	// access token
	int pos1 = strAccessToken.find('\"');
	if( pos1 == -1 )
		return FALSE;

	int pos2 = strAccessToken.find('\"', pos1+1);
	if( pos2 == -1 )
		return FALSE;

	m_strAccessToken = strAccessToken.substr(pos1+1, pos2-pos1-1);

	// nonce
	pos1 = strNonce.find('\"');
	if( pos1 == -1 )
		return FALSE;

	pos2 = strNonce.find('\"', pos1+1);
	if( pos2 == -1 )
		return FALSE;

	m_strNonce = strNonce.substr(pos1+1, pos2-pos1-1);

	// mac
	pos1 = strMac.find('\"');
	if( pos1 == -1 )
		return FALSE;

	pos2 = strMac.find('\"', pos1+1);
	if( pos2 == -1 )
		return FALSE;

	m_strMac = strMac.substr(pos1+1, pos2-pos1-1);
	return TRUE;
}


bool CProgressLoadingUI::OnEvent( void * pParam )
{
	TEventUI* pEvent =(TEventUI*)pParam;
	if (pEvent->Type == UIEVENT_TIMER)
	{
		if (pEvent->wParam == TimerId_ShowState)
		{
			m_PaintManager.KillTimer(m_laySaving, TimerId_ShowState);

			Close();

			TEventNotify event = {0};
			m_OnClose(&event);

		}
	}

	return true;
}