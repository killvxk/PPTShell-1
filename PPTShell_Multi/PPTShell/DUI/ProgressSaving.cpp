#include "StdAfx.h"
#include "ProgressSaving.h"
#include "Util/Util.h"
#include "DUI/GroupExplorer.h"
#include "NDCloud/NDCloudAPI.h"
#include "NDCloud/NDCloudUser.h"
#include "NDCloud/NDCloudResModel.h"
#include "NDCloud/NDCloudUpload.h"
#include "json/json.h"

#define TimerId_ShowState 12

CProgressSavingUI::CProgressSavingUI(void)
{
	m_dwUploadId				= 0;
	m_dwDownloadServerPathId	= 0;
	m_dwCommitId				= 0;
	m_nPresentCount				= 0;
	m_bSuccess					= false;
}

CProgressSavingUI::~CProgressSavingUI(void)
{

	Stop();
}

DuiLib::CDuiString CProgressSavingUI::GetSkinFolder()
{
	return CDuiString(_T("skins"));
}

DuiLib::CDuiString CProgressSavingUI::GetSkinFile()
{
	return _T("ProgressMasker\\saving.xml");
}

void CProgressSavingUI::InitWindow()
{
	__super::InitWindow();

//	m_wndShadow.Create(GetHWND(), AfxGetApp()->m_pMainWnd->GetSafeHwnd());

	m_laySaving		= dynamic_cast<CContainerUI*>(m_PaintManager.FindControl(_T("saving")));
	m_laySuccess	= dynamic_cast<CContainerUI*>(m_PaintManager.FindControl(_T("success")));
	m_layFail		= dynamic_cast<CContainerUI*>(m_PaintManager.FindControl(_T("fail")));
	m_proDownload	= dynamic_cast<CProgressUI*>(m_PaintManager.FindControl(_T("progress")));


	m_laySaving->OnEvent += MakeDelegate(this, &CProgressSavingUI::OnEvent);

	

}

LPCTSTR CProgressSavingUI::GetWindowClassName( void ) const
{
	return _T("CProgressSavingUI");
}

void CProgressSavingUI::ShowWindow( bool bShow /*= true*/, bool bTakeFocus /*= true */ )
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

void CProgressSavingUI::OnBtnClose( TNotifyUI& msg )
{

}

bool CProgressSavingUI::Start( LPCTSTR lptcsPath, CDelegateBase* OnCloseDelegate /*= NULL*/, int nPresentCount/* = 0 */)
{
	if (!lptcsPath)
	{
		return false;
	}

	m_nPresentCount	= nPresentCount;

	//get file name
	TCHAR szDrive[8]				= {0};
	TCHAR szDir[MAX_PATH * 2]		= {0};
	TCHAR szFileName[MAX_PATH * 2]	= {0};
	TCHAR szExt[MAX_PATH * 2]		= {0};
	if (_tsplitpath_s(lptcsPath, szDrive, szDir, szFileName, szExt))
	{
		return false;
	}

	_tcsncpy_s(szFileName, szFileName, 110);

	m_strTitle		= szFileName;
	m_strLocalPath	= lptcsPath;

	m_OnClose.clear();
	if (OnCloseDelegate)
	{
		m_OnClose		+=*OnCloseDelegate;
	}
	
	CHttpDownloadManager* pHttpManager = HttpDownloadManager::GetInstance();
	if( pHttpManager == NULL )
		return false;

	// query title whether it is exist
	DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();
	tstring strChapterGuid = NDCloudGetChapterGUID();

	tstring strUrl = NDCloudComposeUrlCourseInfo(strChapterGuid, m_strTitle, 0, 10, dwUserId);
	DWORD dwTaskId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CProgressSavingUI::OnTitleQueried));
	
	if( dwTaskId == 0 )
		return false;

	return true;
}

void CProgressSavingUI::Stop()
{
	NDCloudDownloadCancel(m_dwDownloadServerPathId);
	NDCloudDownloadCancel(m_dwCommitId);
	HttpUploadManager::GetInstance()->CancelTask(m_dwUploadId);
}

bool CProgressSavingUI::OnTitleQueried(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;

	if( pNotify->dwErrorCode != 0 )
		return false;

	tstring strServerGuid;
	tstring strServerFilePath;

	CStream stream(1024);
	NDCloudDecodeCourseList(pNotify->pData, pNotify->nDataSize, &stream);
	
	m_bCover = false;//覆盖

	int nCount = stream.ReadInt();

	int pos = m_strLocalPath.rfind('.');
	if( pos == -1 )
		return false;

	//增加判断类型
	bool bSameTitle = false;
	bool bSameFormat = false;

	tstring strFormat = m_strLocalPath.substr(pos+1);

	CNDCloudResourceModel model;
	tstring strLocalFormat = model.GetFileFormat(strFormat);

	tstring strServerFormat;
	for(int i = 0 ; i < nCount ; i++)
	{
		bSameTitle = false;
		bSameFormat = false;

		int nNdpType = stream.ReadInt();
		if(nNdpType == 0 )
			strServerFormat = _T("application/vnd.ms-powerpoint");
		else if( nNdpType == 1 )
			strServerFormat = _T("ndp");
		else
			strServerFormat = _T("ndf/cw-x");

		if(strServerFormat == strLocalFormat)
		{
			bSameFormat = true;
		}

		tstring strTitle = stream.ReadString();
		if(m_strTitle == strTitle)
		{
			bSameTitle = true;
		}

		if(bSameTitle && bSameFormat)
		{
			break;
		}

		stream.ReadString();
		stream.ReadString();
		stream.ReadString();
		int nPreviewsCount	= stream.ReadInt();

		if(nPreviewsCount)
		{
			stream.ReadString();
		}

		stream.ReadString();
	}
	
	//
	if( bSameTitle && bSameFormat )
	{
		// notify user whether there it is a file with same title
		int nRet = UIMessageBox(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), _T("网盘上存在同名文件,是否覆盖?"), _T("101教育PPT"), _T("是,否"), CMessageBoxUI::enMessageBoxTypeQuestion, IDCANCEL); 
		if (nRet == ID_MSGBOX_BTN)
		{
			//BOOL bIsNdp			= stream.ReadDWORD();
			//tstring strTitle	= stream.ReadString();
			tstring strGuid		= stream.ReadString();
			tstring strPPTUrl	= stream.ReadString();

			strServerGuid = strGuid;
			strServerFilePath = strPPTUrl;

			m_bCover = true;
		}
		else
		{
			// a new upload but use different title
			TCHAR szTime[MAX_PATH];

			SYSTEMTIME systemTime;
			GetSystemTime(&systemTime);
		
			_stprintf_s(szTime, _T("_%04d%02d%02d_%02d%02d%02d"), systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour+8, systemTime.wMinute, systemTime.wSecond);
			m_strTitle += szTime;
		}	
	}
	stream.ResetCursor();

	// upload a new file
	DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();
	NDCloudUploadManager::GetInstance()->UploadFile(UPLOAD_COURSEWARE, 
													dwUserId,												
													m_strLocalPath, 
													MakeHttpDelegate(this, &CProgressSavingUI::OnSaveCompleted),
													MakeHttpDelegate(this, &CProgressSavingUI::OnSaveProgress),
													strServerGuid,
													strServerFilePath);
													

	return true;
}

bool CProgressSavingUI::OnSaveProgress( void* pObj )
{
	THttpNotify* pHttpNotify = (THttpNotify*)pObj;
	int nPos = (int)(pHttpNotify->fPercent * m_proDownload->GetMaxValue());
	this->SetProgress(nPos);

	TCHAR szTip[128]	= {0};
	TCHAR szSpeed[16]	= {0};
	TCHAR szRemain[32]	= {0};
	TCHAR szElapse[32]	= {0};
	TCHAR szSize[32]	= {0};
	int		nTemp		= 0;

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


	_stprintf_s(szTip, _T("上传速度：%s<n>上传用时：%s<n>剩余时间：%s<n>文件大小：%s<n>"), 
		szSpeed,
		szElapse,
		szRemain,
		szSize);

	m_proDownload->SetToolTip(szTip);

	return true;
}

bool CProgressSavingUI::OnSaveCompleted( void* pObj )
{
	THttpNotify* pHttpNotify = (THttpNotify*)pObj;
	tstring strFullFileServerPath;
	if (pHttpNotify->dwErrorCode==0)
	{
		pHttpNotify->pData[pHttpNotify->nDataSize] = '\0';
		tstring str = pHttpNotify->pData;
		Json::Reader reader;
		Json::Value root;

		bool res = reader.parse(str, root);
		if(!res||root.get("dentry_id", Json::Value()).isNull())
		{
			pHttpNotify->dwErrorCode=5;
		}

		if(res && !root.get("path", Json::Value()).isNull())
		{
			strFullFileServerPath= Ansi2Str(root["path"].asCString());
		}
	}
	if (pHttpNotify->dwErrorCode != 0)
	{
		this->ShowProgress(false);
		this->SetProgress(0);

		SwitchTo(false);
		return true;
	}

	this->SetProgress(m_proDownload->GetMaxValue() - 1);
	m_proDownload->SetToolTip(_T(""));

	CNDCloudResourceModel ResModel;
	DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();

	tstring strChapterGuid = NDCloudGetChapterGUID();

	TCHAR szUserId[MAX_PATH];
	_stprintf_s(szUserId, _T("%d"), dwUserId);

	// detail data
	CStream* pStream = (CStream*)pHttpNotify->pDetailData;
	tstring strUploadGuid = pStream->ReadString();
	tstring strServerPath = pStream->ReadString();

	ResModel.SetCover(m_bCover);
	ResModel.SetGuid(strUploadGuid);
	string strTmp = m_strTitle;
	if(strTmp.length()>200)
	{
		strTmp = strTmp.substr(0,200);
	}
	ResModel.SetBasicInfo(strTmp);
	ResModel.SetPreview();
	ResModel.SetTechInfo(m_strLocalPath, strServerPath, strFullFileServerPath);
	ResModel.SetLifeCycleInfo();
	ResModel.AddCoverage(_T("User"), szUserId, _T(""), _T("OWNER") );
	ResModel.AddRelation(strChapterGuid);
	ResModel.AddRequirement(_T("SOFTWARE"), _T("editor"), _T("2007"), _T("2013"), _T("microsoft office 2007+"));
	ResModel.AddRequirement(_T("SOFTWARE"), _T("player"), _T("2007"), _T("2013"), _T("microsoft office 2007+"));

	CCategoryTree* pCategory;
	NDCloudGetCategoryTree(pCategory);
	ResModel.SetCategoryInfo(pCategory);


	// Commit to database
	m_dwCommitId = ResModel.CommitResourceModel(MakeHttpDelegate(this, &CProgressSavingUI::OnCommitCompleted));

	return true;
}

bool CProgressSavingUI::OnCommitCompleted( void * pParam )
{
	THttpNotify* pHttpNotify = (THttpNotify*)pParam;

	this->SetProgress(m_proDownload->GetMaxValue());
	this->ShowProgress(false);
	this->SetProgress(0);

	if (pHttpNotify->dwErrorCode != 0)
	{
		// fail
		SwitchTo(false);
		return true;
	}

	Json::Reader	reader;
	Json::Value		result;
	if (!reader.parse(pHttpNotify->pData, pHttpNotify->pData + pHttpNotify->nDataSize, result))
	{
		// fail
		SwitchTo(false);
		return true;
	}

	if (!result.get(_T("code"), Json::Value()).isNull())
	{
		// fail
		SwitchTo(false);
		return true;
	}

	SwitchTo(true);
	return true;
}

DWORD CProgressSavingUI::UploadFile(LPCTSTR lptcsUrl, LPCTSTR lptcsSessionId, LPCTSTR lptcsUploadGuid, LPCTSTR lptcsServerFilePath)
{
	TCHAR szHost[128];
	TCHAR szUrlPath[1024];
	if(sscanf_s(lptcsUrl, _T("http://%[^/]%s"), szHost, sizeof(szHost) - 1, szUrlPath , sizeof(szUrlPath) - 1) != 2)
	{
		return 0;
	}
	tstring strHost = szHost;
	tstring strUrlPath = szUrlPath;
	strUrlPath += _T("?session=");
	strUrlPath += lptcsSessionId;

	return HttpUploadManager::GetInstance()->AddUploadTask(strHost.c_str(), 
		strUrlPath.c_str(),
		m_strLocalPath.c_str(),
		lptcsServerFilePath,
		_T("POST"), 
		80,
		MakeHttpDelegate(this, &CProgressSavingUI::OnSaveCompleted),
		MakeHttpDelegate(this, &CProgressSavingUI::OnSaveProgress));

}

void CProgressSavingUI::SetProgress( int nPos )
{
	m_proDownload->SetValue(nPos);

// 	TCHAR szText[32] = {0};
// 	_stprintf_s(szText, _T("%d%%"), nPos);
// 
// 	m_proDownload->SetText(szText);
}

void CProgressSavingUI::ShowProgress( bool bVisible )
{
	m_proDownload->SetVisible(bVisible);
}

void CProgressSavingUI::SwitchTo( bool bSuccess )
{
	m_laySaving->SetVisible(false);

	if (bSuccess)
	{
		m_laySuccess->SetVisible(true);
		m_layFail->SetVisible(false);
	}
	else
	{
		m_layFail->SetVisible(true);
		m_laySuccess->SetVisible(false);
	}

	m_bSuccess = bSuccess;
	m_PaintManager.SetTimer(m_laySaving, TimerId_ShowState, 2000);
}

bool CProgressSavingUI::OnEvent( void * pParam )
{
	TEventUI* pEvent =(TEventUI*)pParam;
	if (pEvent->Type == UIEVENT_TIMER)
	{
		if (pEvent->wParam == TimerId_ShowState)
		{
			m_PaintManager.KillTimer(m_laySaving, TimerId_ShowState);

			Close();

			TEventNotify event = {0};
			event.wParam = m_bSuccess ? 0 : 1;
			event.lParam = m_nPresentCount;

			if(m_OnClose)
				m_OnClose(&event);
			
		}
	}

	return true;
}

