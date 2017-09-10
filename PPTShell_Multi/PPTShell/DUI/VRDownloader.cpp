#include "StdAfx.h"
#include "DUI/ITransfer.h"
#include "DUI/IVisitor.h"
#include "DUI/BaseParamer.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IDownloadListener.h"
#include "DUI/ResourceDownloader.h"
#include "DUI/AssetDownloader.h"
#include "DUI/VRDownloader.h"
#include "NDCloud/NDCloudAPI.h"
#include "Util/Util.h"

#include "NDCloud/NDCloudUser.h"


#define AssetsDownloadUrl			_T("/v0.6/assets/%s/downloadurl?uid=%d&key=%s")

CVRDownloader::CVRDownloader()
{
	m_dwSessionId		= 0;
	m_dwPermissionId	= 0;
}


CVRDownloader::~CVRDownloader()
{

}

bool CVRDownloader::Transfer()
{
	TCHAR* szPlatformType[] = 
	{
		_T("uc"),
		_T("sina"),
		_T("qq"),
		_T("99u"),
	};

	// check permission
	DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();
	int nLoginType = NDCloudUser::GetInstance()->GetLoginType();
	tstring strUserName = NDCloudUser::GetInstance()->GetUserName();

	if( dwUserId == 0 || nLoginType == -1 )
	{
		CToast::Toast(_T("请先登录"));
		THttpNotify notify;
		notify.dwErrorCode	= 10;
		notify.pUserData	= __super::m_pParamer;
		__super::OnDownloadResourceCompleted(&notify);
		return false;
	}


	TCHAR szUrl[1024];
	_stprintf_s(szUrl, _T("/101ppt/permission.php?user=%s&plat=%s"), strUserName.c_str(), szPlatformType[nLoginType]);

	m_dwPermissionId = HttpDownloadManager::GetInstance()->AddTask(_T("p.101.com"),
		szUrl,
		_T(""), 
		_T("GET"),
		_T(""), 
		INTERNET_DEFAULT_HTTP_PORT, 
		MakeHttpDelegate(this, &CVRDownloader::OnPermissionObtained),
		MakeHttpDelegate(NULL), 
		MakeHttpDelegate(NULL),
		TRUE,
		FALSE,
		0,
		__super::m_pParamer);

	if (!m_dwPermissionId)
	{
		THttpNotify notify;
		notify.dwErrorCode	= 10;
		notify.pUserData	= __super::m_pParamer;
		__super::OnDownloadResourceCompleted(&notify);
	}
	return m_dwPermissionId != 0;
}

bool CVRDownloader::OnPermissionObtained( void* param )
{
	m_dwPermissionId = 0;

	THttpNotify* pNotify = (THttpNotify*)param;
	if( pNotify->dwErrorCode != 0 )
	{
		THttpNotify notify;
		notify.dwErrorCode	= 10;
		notify.pUserData	= __super::m_pParamer;
		__super::OnDownloadResourceCompleted(&notify);

		return true;
	}

	// check permission
	pNotify->pData[pNotify->nDataSize] = '\0';
	if( 0 == strcmpi(pNotify->pData, "0") )
	{
		TCHAR szUrl[1024];
		_stprintf_s(szUrl, AssetsDownloadUrl, __super::m_pGuid, 0, _T("source"));

		m_dwSessionId = NDCloudDownload(szUrl, MakeHttpDelegate(this, &CVRDownloader::OnSessionObtained));
	}
	else
	{
		UIMessageBox(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), _STR_NO_PERMISSION, _T("提示"), _T("确定"), CMessageBoxUI::enMessageBoxTypeInfo);

		THttpNotify notify;
		notify.dwErrorCode	= 11;
		notify.pUserData	= __super::m_pParamer;
		__super::OnDownloadResourceCompleted(&notify);
	}

	return true;
}

bool CVRDownloader::OnSessionObtained( void* param )
{
	m_dwSessionId = 0;

	THttpNotify* pNotify = (THttpNotify*)param;

	do 
	{
		if (pNotify->dwErrorCode != 0)
		{
			break;
		}
		// download url and session
		pNotify->pData[pNotify->nDataSize] = '\0';

		string str = Utf8ToAnsi(pNotify->pData);

		Json::Reader reader;
		Json::Value root;

		bool res = reader.parse(str, root);
		if( !res )
		{
			break;
		}

		// access_url
		if( root["access_url"].isNull() )
		{
			break;
		}

		tstring strUrl = Ansi2Str(root["access_url"].asCString());

		tstring strSessionId = Ansi2Str(root["session_id"].asCString());

		strUrl += _T("&session=");
		strUrl += strSessionId;

		
		__super::m_dwTransferId  = NDCloudDownloadFile(strUrl.c_str(),
			m_pGuid,
			m_pTitle,
			m_nAssetType,
			0,
			MakeHttpDelegate(this, &CResourceDownloader::OnDownloadResourceCompleted),
			MakeHttpDelegate(this, &CResourceDownloader::OnDownloadResourceProgress),
			__super::m_pParamer, m_pMD5);


		if (__super::m_dwTransferId == 0)
		{
			THttpNotify notify;
			notify.dwErrorCode	= 10;
			notify.pUserData	= __super::m_pParamer;
			__super::OnDownloadResourceCompleted(&notify);
		}

		return __super::m_dwTransferId != 0;

	} while (false);

	THttpNotify notify;
	notify.dwErrorCode	= 10;
	notify.pUserData	= __super::m_pParamer;
	__super::OnDownloadResourceCompleted(&notify);
	CToast::Toast(_T("文件Session获取失败"));

	return true;
}

void CVRDownloader::Cancel()
{
	if (__super::m_dwTransferId != 0)
	{
		NDCloudDownloadCancel(__super::m_dwTransferId);
	}

	if (m_dwSessionId != 0)
	{
		NDCloudDownloadCancel(m_dwSessionId);
	}

	if (m_dwPermissionId != 0)
	{
		NDCloudDownloadCancel(m_dwPermissionId);
	}

	NotifyDownloadInterpose(eInterpose_Cancel);
}

void CVRDownloader::Pause()
{
	if (__super::m_dwTransferId != 0)
	{
		NDCloudDownloadPause(__super::m_dwTransferId, &MakeHttpDelegate(this, &CResourceDownloader::OnDownloadResourcePause));
	}

	if (m_dwSessionId != 0)
	{
		NDCloudDownloadPause(m_dwSessionId, &MakeHttpDelegate(this, &CResourceDownloader::OnDownloadResourcePause));
	}

	if (m_dwPermissionId != 0)
	{
		NDCloudDownloadPause(m_dwPermissionId, &MakeHttpDelegate(this, &CResourceDownloader::OnDownloadResourcePause));
	}

}

void CVRDownloader::Resume()
{
	if (__super::m_dwTransferId != 0)
	{
		NDCloudDownloadResume(__super::m_dwTransferId);
	}

	if (m_dwSessionId != 0)
	{
		NDCloudDownloadResume(m_dwSessionId);
	}

	if (m_dwPermissionId != 0)
	{
		NDCloudDownloadResume(m_dwPermissionId);
	}

	NotifyDownloadInterpose(eInterpose_Resume);
}


ITransfer* CVRDownloader::Copy()
{
	CVRDownloader* pDownloader		= new CVRDownloader;
	pDownloader->m_pParamer			= __super::m_pCopyParamer->Copy();

	pDownloader->m_nAssetType		= this->m_nAssetType;
	pDownloader->m_pGuid			= this->m_pGuid;
	pDownloader->m_pMD5				= this->m_pMD5;
	pDownloader->m_pTitle			= this->m_pTitle;
	pDownloader->m_pUrl				= this->m_pUrl;


	return pDownloader;
}