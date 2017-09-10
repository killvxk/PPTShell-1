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
#include "EventCenter/EventDefine.h"
#include "NDCloud/NDCloudUser.h"


#define AbtainSessionUrlFormat			_T("/v0.6/assets/%s/downloadurl?uid=%d&key=%s")
#define QueryPermissionUrlFormat		_T("/101ppt/permission.php?user=%s&plat=%s")

TCHAR* g_szPlatformType[] = 
{
	_T("uc"),
	_T("sina"),
	_T("qq"),
	_T("99u"),
};

CVRDownloader::CVRDownloader()
{
	m_dwSessionId		= 0;
	m_dwPermissionId	= 0;

	OnEvent(EVT_LOGIN, MakeEventDelegate(this, &CVRDownloader::OnUserLogin));
	OnEvent(EVT_LOGOUT, MakeEventDelegate(this, &CVRDownloader::OnUserLogout));
	
}


CVRDownloader::~CVRDownloader()
{
	CancelEvent(EVT_LOGIN, MakeEventDelegate(this, &CVRDownloader::OnUserLogin));
	CancelEvent(EVT_LOGOUT, MakeEventDelegate(this, &CVRDownloader::OnUserLogout));
	
}

bool CVRDownloader::Transfer()
{
	// check permission
	DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();
	int nLoginType = NDCloudUser::GetInstance()->GetLoginType();
	tstring strUserName = NDCloudUser::GetInstance()->GetUserName();

	m_dwUserId = dwUserId;

	if( dwUserId == 0 
		|| nLoginType == -1 )
	{
		THttpNotify notify;
		notify.dwErrorCode	= Error_Not_LoggedIn;
		notify.pUserData	= __super::m_pParamer;
		__super::OnDownloadResourceCompleted(&notify);
		return false;
	}


	TCHAR szUrl[1024] = {0};
	_stprintf_s(szUrl, QueryPermissionUrlFormat, strUserName.c_str(), g_szPlatformType[nLoginType]);

	m_dwPermissionId = HttpDownloadManager::GetInstance()->AddTask(_T("p.101.com"),
		szUrl,
		_T(""), 
		_T("GET"),
		"", 
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
		notify.dwErrorCode	= Error_Net_Disconnected;
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
		notify.dwErrorCode	= pNotify->dwErrorCode;
		notify.pUserData	= __super::m_pParamer;
		__super::OnDownloadResourceCompleted(&notify);

		return true;
	}

	// check permission
	pNotify->pData[pNotify->nDataSize] = '\0';
	if( 0 == strcmpi(pNotify->pData, "0") )
	{
		TCHAR szUrl[1024];
		_stprintf_s(szUrl, AbtainSessionUrlFormat, __super::m_pGuid, 0, _T("source"));

		m_dwSessionId = NDCloudDownload(szUrl, MakeHttpDelegate(this, &CVRDownloader::OnSessionObtained));
	}
	else
	{
		THttpNotify notify;
		notify.dwErrorCode	= Error_Access_Denied;
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
			notify.dwErrorCode	= Error_Net_Disconnected;
			notify.pUserData	= __super::m_pParamer;
			__super::OnDownloadResourceCompleted(&notify);
		}

		return __super::m_dwTransferId != 0;

	} while (false);

	THttpNotify notify;
	notify.dwErrorCode	= Error_Session_Get_Failed;
	notify.pUserData	= __super::m_pParamer;
	__super::OnDownloadResourceCompleted(&notify);

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
		NotifyDownloadInterpose(eInterpose_Pause);
		NDCloudDownloadPause(__super::m_dwTransferId, &MakeHttpDelegate(this, &CResourceDownloader::OnDownloadResourcePause));
	}

	if (m_dwSessionId != 0)
	{
		NotifyDownloadInterpose(eInterpose_Pause);
		NDCloudDownloadPause(m_dwSessionId, &MakeHttpDelegate(this, &CResourceDownloader::OnDownloadResourcePause));
	}

	if (m_dwPermissionId != 0)
	{
		NotifyDownloadInterpose(eInterpose_Pause);
		NDCloudDownloadPause(m_dwPermissionId, &MakeHttpDelegate(this, &CResourceDownloader::OnDownloadResourcePause));
	}

}

void CVRDownloader::Resume()
{
	// check user whether it is changed
	DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();
	int nLoginType = NDCloudUser::GetInstance()->GetLoginType();
	tstring strUserName = NDCloudUser::GetInstance()->GetUserName();

	if( dwUserId == 0 
		|| nLoginType == -1 )
	{
		THttpNotify notify;
		notify.dwErrorCode	= Error_Not_LoggedIn;
		notify.pUserData	= __super::m_pParamer;
		__super::OnDownloadResourceCompleted(&notify);
		return;
	}

	if( dwUserId == m_dwUserId )
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

		return;
	}
	
	// check permission because user is changed
	TCHAR szUrl[1024] = {0};
	_stprintf_s(szUrl, QueryPermissionUrlFormat, strUserName.c_str(), g_szPlatformType[nLoginType]);

	m_dwPermissionId = HttpDownloadManager::GetInstance()->AddTask(_T("p.101.com"),
		szUrl,
		_T(""), 
		_T("GET"),
		"", 
		INTERNET_DEFAULT_HTTP_PORT, 
		MakeHttpDelegate(this, &CVRDownloader::OnResumePermissionObtained),
		MakeHttpDelegate(NULL), 
		MakeHttpDelegate(NULL),
		TRUE,
		FALSE,
		0,
		__super::m_pParamer);

}

bool CVRDownloader::OnResumePermissionObtained(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	if( pNotify->dwErrorCode != 0 )
	{
		THttpNotify notify;
		notify.dwErrorCode	= pNotify->dwErrorCode;
		notify.pUserData	= __super::m_pParamer;
		__super::OnDownloadResourceCompleted(&notify);

		return true;
	}

	// check permission
	pNotify->pData[pNotify->nDataSize] = '\0';
	if( 0 == strcmpi(pNotify->pData, "0") )
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
	else
	{
		THttpNotify notify;
		notify.dwErrorCode	= Error_Access_Denied;
		notify.pUserData	= __super::m_pParamer;
		__super::OnDownloadResourceCompleted(&notify);
	}


	return true;
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

bool CVRDownloader::OnUserLogin(void* param)
{
	DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();
	if( dwUserId != m_dwUserId )
		Pause();

	return true;
}

bool CVRDownloader::OnUserLogout(void* param)
{
	Pause();
	return true;
}

