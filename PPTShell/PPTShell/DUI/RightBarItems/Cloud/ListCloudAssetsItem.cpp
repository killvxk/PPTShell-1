#include "stdafx.h"
#include "ListCloudAssetsItem.h"
#include "NDCloud/NDCloudFile.h"
#include "NDCloud/NDCloudAPI.h"
#include "DUI/GroupExplorer.h"

CListCloudAssetsItem::CListCloudAssetsItem()
{
	m_dwCurImageDownloadThreadId = -1;
	m_dwCurFlashDownloadThreadId = -1;
	m_dwCurVideoDownloadThreadId = -1;
	m_dwCurVolumeDownloadThreadId = -1;

	m_dwTotalImageDownloadThreadId = -1;
	m_dwTotalFlashDownloadThreadId = -1;
	m_dwTotalVideoDownloadThreadId = -1;
	m_dwTotalVolumeDownloadThreadId = -1;
}

CListCloudAssetsItem::~CListCloudAssetsItem()
{
	if(m_dwCurImageDownloadThreadId != -1)
	{
		NDCloudDownloadCancel(m_dwCurImageDownloadThreadId);
	}
	if(m_dwCurFlashDownloadThreadId != -1)
	{
		NDCloudDownloadCancel(m_dwCurFlashDownloadThreadId);
	}
	if(m_dwCurVideoDownloadThreadId != -1)
	{
		NDCloudDownloadCancel(m_dwCurVideoDownloadThreadId);
	}
	if(m_dwCurVolumeDownloadThreadId != -1)
	{
		NDCloudDownloadCancel(m_dwCurVolumeDownloadThreadId);
	}

	if(m_dwTotalImageDownloadThreadId != -1)
	{
		NDCloudDownloadCancel(m_dwTotalImageDownloadThreadId);
	}
	if(m_dwTotalFlashDownloadThreadId != -1)
	{
		NDCloudDownloadCancel(m_dwTotalFlashDownloadThreadId);
	}
	if(m_dwTotalVideoDownloadThreadId != -1)
	{
		NDCloudDownloadCancel(m_dwTotalVideoDownloadThreadId);
	}
	if(m_dwTotalVolumeDownloadThreadId != -1)
	{
		NDCloudDownloadCancel(m_dwTotalVolumeDownloadThreadId);
	}
}

void CListCloudAssetsItem::DoInit()
{
	__super::DoInit();

	m_nType = CloudFileAssets;

	//增加本地统计
	CTime tm = CTime::GetCurrentTime();
	DWORD dwTime = tm.GetTime();

	tstring strConfinFile = GetLocalPath();
	strConfinFile += _T("\\Setting\\Config.ini");

	TCHAR szTime[MAX_PATH * 2 + 1];
	GetPrivateProfileString(_T("Data"), _T("Date"), _T(""), szTime, MAX_PATH * 2, strConfinFile.c_str());
	if( _tcslen(szTime) > 0)
	{
		DWORD dwLocalTime = _tcstoul(szTime, NULL , 10);

		if(dwTime - dwLocalTime < 12 * 60 * 60)
		{
			TCHAR szCount[MAX_PATH * 2 + 1];
			GetPrivateProfileString(_T("Data"), GetUserData(), _T(""), szCount, MAX_PATH * 2, strConfinFile.c_str());
			if( _tcslen(szCount) > 0)
			{
				int nCount = _ttoi(szCount);

				m_pTotalLoadGif->StopGif();
				m_pTotalLoadGif->SetVisible(false);


				TCHAR szBuff[128] ={0};
				_stprintf(szBuff,_T("%d"), nCount);
				
				SIZE textSize = CRenderEngine::GetTextSize(GetManager()->GetPaintDC(), GetManager(), 
					szBuff, m_pTextTotalCount->GetFont(), DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT| DT_NOPREFIX);

				
				m_pTextTotalCount->SetFixedWidth(textSize.cx);
				m_pTextTotalCount->SetVisible(true);
				m_pTextTotalCount->SetText(szBuff);

				m_bTotalNetLess = false;

				return;
			}
		}

	}

	GetTotalCountInterface();
}

bool CListCloudAssetsItem::OnChapterChanged( void* pObj )
{
	TEventNotify* pEventNotify = (TEventNotify*)pObj;
	CStream* pStream = (CStream*)pEventNotify->wParam;
	pStream->ResetCursor();
	tstring strGuid = pStream->ReadString();

	GetCurCountInterface(strGuid);
	return true;
}

bool CListCloudAssetsItem::OnDownloadDecodeList( void* pObj )
{
	return true;
}

void CListCloudAssetsItem::GetCurCountInterface( tstring strGuid )
{
	tstring strImageUrl = NDCloudComposeUrlPictureInfo(strGuid, _T(""), 0, 1);
	tstring strVolumeUrl = NDCloudComposeUrlVolumeInfo(strGuid, _T(""), 0, 1);
	tstring strVideoUrl = NDCloudComposeUrlVideoInfo(strGuid, _T(""), 0, 1);
	tstring strFlashUrl = NDCloudComposeUrlFlashInfo(strGuid, _T(""), 0, 1);

	if(m_pStream)
	{
		delete m_pStream;
		m_pStream = NULL; 
	}

	if(m_dwCurImageDownloadThreadId != -1)
	{
		NDCloudDownloadCancel(m_dwCurImageDownloadThreadId);
	}
	if(m_dwCurFlashDownloadThreadId != -1)
	{
		NDCloudDownloadCancel(m_dwCurFlashDownloadThreadId);
	}
	if(m_dwCurVideoDownloadThreadId != -1)
	{
		NDCloudDownloadCancel(m_dwCurVideoDownloadThreadId);
	}
	if(m_dwCurVolumeDownloadThreadId != -1)
	{
		NDCloudDownloadCancel(m_dwCurVolumeDownloadThreadId);
	}

	m_pTextCurCount->SetVisible(false);

	m_pCurLoadGif->SetVisible(true);
	m_pCurLoadGif->PlayGif();

	m_dwCurThreadFinishCount	= 0;
	m_dwCurRequestSuccess		= 0;
	m_nGetCurAssetsCount	= 0;
	m_dwCurImageDownloadThreadId = NDCloudDownload(strImageUrl.c_str(), MakeHttpDelegate(this, &CListCloudAssetsItem::OnGetAssetsCurCount));
	m_dwCurFlashDownloadThreadId = NDCloudDownload(strFlashUrl.c_str(), MakeHttpDelegate(this, &CListCloudAssetsItem::OnGetAssetsCurCount));
	m_dwCurVideoDownloadThreadId = NDCloudDownload(strVideoUrl.c_str(), MakeHttpDelegate(this, &CListCloudAssetsItem::OnGetAssetsCurCount));
	m_dwCurVolumeDownloadThreadId = NDCloudDownload(strVolumeUrl.c_str(), MakeHttpDelegate(this, &CListCloudAssetsItem::OnGetAssetsCurCount));
}

bool CListCloudAssetsItem::OnGetAssetsCurCount( void * pParam )
{
	m_dwCurThreadFinishCount++;

	THttpNotify * pNotify = (THttpNotify*)pParam;

	if(m_dwCurThreadFinishCount == 4)
	{
		m_pCurLoadGif->StopGif();
		m_pCurLoadGif->SetVisible(false);
	}

	if(pNotify->dwErrorCode != 0)
	{
		m_bCurNetLess = true;
		return true;
	}
	m_dwCurRequestSuccess++;

	int nCount = NDCloudDecodeCount(pNotify->pData, pNotify->nDataSize);

	m_nGetCurAssetsCount += nCount;
	if(m_pTextCurCount && m_dwCurThreadFinishCount == 4)
	{
		TCHAR szBuff[128] ={0};
		_stprintf(szBuff,_T("%d"), m_nGetCurAssetsCount);
		
		SIZE textSize = CRenderEngine::GetTextSize(GetManager()->GetPaintDC(), GetManager(), 
			szBuff, m_pTextCurCount->GetFont(), DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT| DT_NOPREFIX);

		m_pTextCurCount->SetFixedWidth(textSize.cx);
		m_pTextCurCount->SetVisible(true);
		m_pTextCurCount->SetText(szBuff);

		if(m_dwCurRequestSuccess == 4)
			m_bCurNetLess = false;
	}

	return true;
}

void CListCloudAssetsItem::GetTotalCountInterface()
{
	//增加本地统计
	CTime tm = CTime::GetCurrentTime();
	DWORD dwTime = tm.GetTime();

	tstring strConfinFile = GetLocalPath();
	strConfinFile += _T("\\Setting\\Config.ini");

	TCHAR szTime[MAX_PATH * 2 + 1];
	GetPrivateProfileString(_T("Data"), _T("Date"), _T(""), szTime, MAX_PATH * 2, strConfinFile.c_str());
	if( _tcslen(szTime) > 0)
	{
		DWORD dwLocalTime = _tcstoul(szTime, NULL , 10);

		if(dwTime - dwLocalTime < 12 * 60 * 60)
		{
			TCHAR szCount[MAX_PATH * 2 + 1];
			GetPrivateProfileString(_T("Data"), GetUserData(), _T(""), szCount, MAX_PATH * 2, strConfinFile.c_str());
			if( _tcslen(szCount) > 0)
			{
				int nCount = _ttoi(szCount);
				if(nCount != 0)
				{
					m_pTotalLoadGif->StopGif();
					m_pTotalLoadGif->SetVisible(false);


					TCHAR szBuff[128] ={0};
					_stprintf(szBuff,_T("%d"), nCount);
					
					SIZE textSize = CRenderEngine::GetTextSize(GetManager()->GetPaintDC(), GetManager(), 
						szBuff, m_pTextTotalCount->GetFont(), DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT| DT_NOPREFIX);

					m_pTextTotalCount->SetFixedWidth(textSize.cx);
					m_pTextTotalCount->SetVisible(true);
					m_pTextTotalCount->SetText(szBuff);

					m_bTotalNetLess = false;

					return;
				}

			}
		}

	}

	tstring strImageUrl = NDCloudComposeUrlPictureInfo(_T(""), _T(""), 0, 1);
	tstring strVolumeUrl = NDCloudComposeUrlVolumeInfo(_T(""), _T(""), 0, 1);
	tstring strVideoUrl = NDCloudComposeUrlVideoInfo(_T(""), _T(""), 0, 1);
	tstring strFlashUrl = NDCloudComposeUrlFlashInfo(_T(""), _T(""), 0, 1);

	if(m_dwTotalImageDownloadThreadId != -1)
	{
		NDCloudDownloadCancel(m_dwTotalImageDownloadThreadId);
	}
	if(m_dwTotalFlashDownloadThreadId != -1)
	{
		NDCloudDownloadCancel(m_dwTotalFlashDownloadThreadId);
	}
	if(m_dwTotalVideoDownloadThreadId != -1)
	{
		NDCloudDownloadCancel(m_dwTotalVideoDownloadThreadId);
	}
	if(m_dwTotalVolumeDownloadThreadId != -1)
	{
		NDCloudDownloadCancel(m_dwTotalVolumeDownloadThreadId);
	}

	m_pTextTotalCount->SetVisible(false);

	m_pTotalLoadGif->SetVisible(true);
	m_pTotalLoadGif->PlayGif();

	m_dwTotalThreadFinishCount	= 0;
	m_dwTotalRequestSuccess		= 0;
	m_nGetTotalAssetsCount	= 0;
	m_dwTotalImageDownloadThreadId = NDCloudDownload(strImageUrl.c_str(), MakeHttpDelegate(this, &CListCloudAssetsItem::OnGetAssetsTotalCount));
	m_dwTotalFlashDownloadThreadId = NDCloudDownload(strFlashUrl.c_str(), MakeHttpDelegate(this, &CListCloudAssetsItem::OnGetAssetsTotalCount));
	m_dwTotalVideoDownloadThreadId = NDCloudDownload(strVideoUrl.c_str(), MakeHttpDelegate(this, &CListCloudAssetsItem::OnGetAssetsTotalCount));
	m_dwTotalVolumeDownloadThreadId = NDCloudDownload(strVolumeUrl.c_str(), MakeHttpDelegate(this, &CListCloudAssetsItem::OnGetAssetsTotalCount));
}

bool CListCloudAssetsItem::OnGetAssetsTotalCount( void * pParam )
{
	m_dwTotalThreadFinishCount++;

	THttpNotify * pNotify = (THttpNotify*)pParam;

	if(m_dwTotalThreadFinishCount == 4)
	{
		m_pTotalLoadGif->StopGif();
		m_pTotalLoadGif->SetVisible(false);
	}

	if(pNotify->dwErrorCode != 0)
	{
		m_bTotalNetLess = true;
		return true;
	}
	m_dwTotalRequestSuccess++;

	int nCount = NDCloudDecodeCount(pNotify->pData, pNotify->nDataSize);

	m_nGetTotalAssetsCount += nCount;
	if(m_pTextTotalCount && m_dwTotalThreadFinishCount == 4)
	{
		TCHAR szBuff[128] ={0};
		_stprintf(szBuff,_T("%d"), m_nGetTotalAssetsCount);
		
		SIZE textSize = CRenderEngine::GetTextSize(GetManager()->GetPaintDC(), GetManager(), 
			szBuff, m_pTextTotalCount->GetFont(), DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT| DT_NOPREFIX);

		m_pTextTotalCount->SetFixedWidth(textSize.cx);
		m_pTextTotalCount->SetVisible(true);
		m_pTextTotalCount->SetText(szBuff);

		if(m_dwTotalRequestSuccess == 4)
			m_bTotalNetLess = false;

		//增加本地统计
		CTime tm = CTime::GetCurrentTime();
		DWORD dwTime = tm.GetTime();

		tstring strConfinFile = GetLocalPath();
		strConfinFile += _T("\\Setting\\Config.ini");

		TCHAR szTime[MAX_PATH * 2 + 1];
		_stprintf(szTime, _T("%d"), dwTime);
		WritePrivateProfileString(_T("Data"), _T("Date"), szTime, strConfinFile.c_str());

		TCHAR szCount[MAX_PATH * 2 + 1];
		_stprintf(szCount, _T("%d"), m_nGetTotalAssetsCount);
		WritePrivateProfileString(_T("Data"), GetUserData(), szCount, strConfinFile.c_str());
	}

	return true;
}

void CListCloudAssetsItem::DoClick( TNotifyUI* pNotify )
{
	CGroupExplorerUI * pGroupExplorer = CGroupExplorerUI::GetInstance();

	pGroupExplorer->ShowWindow(true);
	pGroupExplorer->ShowCloudAssetsUI();

	if(m_bCurNetLess)
		GetCurCountInterface(NDCloudGetChapterGUID());

	if(m_OnDoClickCallBack)
		m_OnDoClickCallBack(pNotify);
}
