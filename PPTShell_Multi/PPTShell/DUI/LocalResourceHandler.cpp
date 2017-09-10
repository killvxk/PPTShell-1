#include "StdAfx.h"
#include "DUI/IItemHandler.h"
#include "DUI/INotifyHandler.h"
#include "Util/Stream.h"
#include "DUI/IStreamReader.h"
#include "DUI/IVisitor.h"
#include "DUI/ItemHandler.h"
#include "DUI/INotifyHandler.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "NDCloud/NDCloudAPI.h"
#include "DUI/IUploadListener.h"
#include "DUI/IThumbnailListener.h"
#include "DUI/LocalResourceHandler.h"

#include "NDCloud/NDCloudUser.h"
#include "NDCloud/NDCloudResModel.h"
#include "NDCloud/NDCloudUpload.h"
#include "json/json.h"
#include "Util/Util.h"

CLocalResourceHandler::CLocalResourceHandler()
{
	m_pUploadListener		= NULL;
	m_pThumbnailListener	= NULL;
	m_bIsDownloading		= false;
}

CLocalResourceHandler::~CLocalResourceHandler()
{

}

void CLocalResourceHandler::DoClick(TNotifyUI*	pNotify)
{
	__super::DoClick(pNotify);
}

void CLocalResourceHandler::DoButtonClick(TNotifyUI*	pNotify)
{
	__super::DoButtonClick(pNotify);
}

void CLocalResourceHandler::DoMenuClick(TNotifyUI*	pNotify)
{
	__super::DoMenuClick(pNotify);
}

void CLocalResourceHandler::DoRClick( TNotifyUI* pNotify )
{
	__super::DoRClick(pNotify);
}

LPCTSTR CLocalResourceHandler::GetResource()
{
	return m_strPath.c_str();
}

LPCTSTR CLocalResourceHandler::GetTitle()
{
	return m_strTitle.c_str();
}


void CLocalResourceHandler::ReadStream( CStream* pStream )
{
	m_strTitle		= pStream->ReadString();
	m_strPath		= pStream->ReadString();
}

bool CLocalResourceHandler::UploadResource( LPCTSTR lptcsServer )
{
	SetUploading(true);
	if (GetUploadListener())
	{
		GetUploadListener()->OnUploadBefore(NULL);
	}

	TCHAR szGet[2048] = {0};
	_stprintf_s(szGet, lptcsServer, NDCloudUser::GetInstance()->GetUserId());

	DWORD dwDownloadServerPathId = HttpDownloadManager::GetInstance()->AddTask(
		_T("esp-lifecycle.web.sdp.101.com"),
		szGet,
		_T(""),
		_T("GET"),
		_T(""),
		80, 
		MakeHttpDelegate(this, &CLocalResourceHandler::OnUploadResourceRequestCompleted),
		MakeHttpDelegate(NULL),
		MakeHttpDelegate(NULL));

	if (dwDownloadServerPathId == 0)
	{
		if (GetUploadListener())
		{
			GetUploadListener()->OnUploadCompleted(NULL);
		}

		SetUploading(false);
	}

	return true;
}


bool CLocalResourceHandler::OnUploadResourceRequestCompleted( void* pObj )
{
	THttpNotify* pNotify = static_cast<THttpNotify*>(pObj);
	do 
	{
		if (pNotify->dwErrorCode != 0)
		{
			break;
		}

		pNotify->pData[pNotify->nDataSize] = '\0';
		Json::Reader	reader;
		Json::Value		root;
		bool res = reader.parse(pNotify->pData, root);
		if( !res )
		{
			break;
		}

		if( root["access_url"].isNull() 
			|| root["uuid"].isNull()
			|| root["session_id"].isNull()
			|| root["dist_path"].isNull())
		{
			break;
		}

		DWORD dwUploadId = UploadResourceInternal(root["access_url"].asCString(),
								root["session_id"].asCString(),
								root["uuid"].asCString(),
								root["dist_path"].asCString());
		if(dwUploadId == 0)
		{
			break;
		}
		return true;

	} while (FALSE);

	OnUploadResourceCompleted(NULL);
	SetUploading(false);
	return true;
}


DWORD CLocalResourceHandler::UploadResourceInternal( LPCSTR lptcsUrl, LPCSTR lptcsSessionId, LPCSTR lptcsUploadGuid, LPCSTR lptcsServerFilePath )
{
	CStream* pStream = new CStream(1024);
	pStream->WriteString(lptcsSessionId);
	pStream->WriteString(lptcsUploadGuid);
	pStream->WriteString(lptcsServerFilePath);


	CHAR szHost[128];
	CHAR szUrlPath[1024];
	if(sscanf_s(lptcsUrl, "http://%[^/]%s", szHost, sizeof(szHost) - 1, szUrlPath , sizeof(szUrlPath) - 1) != 2)
	{
		return 0;
	}

	tstring strHost		= szHost;
	tstring strUrlPath	= szUrlPath;
	strUrlPath += _T("?session=");
	strUrlPath += lptcsSessionId;

	return HttpUploadManager::GetInstance()->AddUploadTask(strHost.c_str(), 
		strUrlPath.c_str(),
		GetResource(),
		lptcsServerFilePath,
		_T("POST"), 
		80,
		MakeHttpDelegate(this, &CLocalResourceHandler::OnUploadResourceCompleted),
		MakeHttpDelegate(this, &CLocalResourceHandler::OnUploadResourceProgress),
		TRUE,
		FALSE,
		pStream);

}


bool CLocalResourceHandler::OnUploadResourceProgress( void* pObj )
{
	if (GetUploadListener())
	{
		GetUploadListener()->OnUploadProgress((THttpNotify*)pObj);
	}
	return true;
}

bool CLocalResourceHandler::OnUploadResourceCompleted( void* pObj )
{
	THttpNotify*	pHttpNotify = (THttpNotify*)pObj;
	CStream*		pStream		= (CStream*)pHttpNotify->pUserData;

	tstring strFullFileServerPath;
	if (pHttpNotify->dwErrorCode == 0)
	{
		pHttpNotify->pData[pHttpNotify->nDataSize] = '\0';
		tstring str = pHttpNotify->pData;
		Json::Reader reader;
		Json::Value root;

		bool res = reader.parse(str, root);
		if(!res||root.get("dentry_id", Json::Value()).isNull())
		{
			pHttpNotify->dwErrorCode = 5;
		}

		if(!res|| !root.get("path", Json::Value()).isNull())
		{
			strFullFileServerPath= Ansi2Str(root["path"].asCString());
		}
	}

	if (pHttpNotify->dwErrorCode != 0)
	{
		if (GetUploadListener())
		{
			GetUploadListener()->OnUploadCompleted(pHttpNotify);
		}
		delete pStream;
		SetUploading(false);
		return true;
	}

	if (GetUploadListener())
	{
		pHttpNotify->fPercent = 0.99f;
		GetUploadListener()->OnUploadCompleted(pHttpNotify);
	}

	pStream->ResetCursor();
	string strSessionId = pStream->ReadString();
	string strGuid		= pStream->ReadString();
	string strSrvPath	= pStream->ReadString();
	delete pStream;

	CNDCloudResourceModel ResModel;
	DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();

	tstring strChapterGuid = NDCloudGetChapterGUID();

	TCHAR szUserId[MAX_PATH];
	_stprintf_s(szUserId, _T("%d"), dwUserId);

	ResModel.SetGuid(strGuid.c_str());
	string strTmp = GetTitle();
	if(strTmp.length()>200)
	{
		strTmp = strTmp.substr(0,200);
	}
	ResModel.SetBasicInfo(strTmp);
	ResModel.SetPreview();
	ResModel.SetTechInfo(GetResource(), strSrvPath.c_str(), strFullFileServerPath);
	ResModel.SetLifeCycleInfo();
	ResModel.AddCoverage(_T("User"), szUserId, _T(""), _T("OWNER") );
	ResModel.AddRelation(NDCloudGetChapterGUID().c_str());

	CCategoryTree* pCategory;
	NDCloudGetCategoryTree(pCategory);
	ResModel.SetCategoryInfo(pCategory);

	// Commit to database
	DWORD dwCommitId = ResModel.CommitResourceModel(MakeHttpDelegate(this, &CLocalResourceHandler::OnUploadResourceCommitted));

	return true;
}


bool CLocalResourceHandler::OnUploadResourceCommitted( void* pObj )
{
	THttpNotify*	pHttpNotify = (THttpNotify*)pObj;

	pHttpNotify->fPercent = 1.0f;
	do 
	{
		if (pHttpNotify->dwErrorCode != 0)
		{
			// fail
			break;
		}

		Json::Reader	reader;
		Json::Value		result;
		if (!reader.parse(pHttpNotify->pData, pHttpNotify->pData + pHttpNotify->nDataSize, result))
		{
			// fail
			break;
		}

		if (!result.get(_T("code"), Json::Value()).isNull())
		{
			// fail
			break;
		}

		//success
		if (GetUploadListener())
		{
			GetUploadListener()->OnUploadCompleted(pHttpNotify);
		}

		SetUploading(false);
		return true;

	} while (false);
	
	// fail
	if (GetUploadListener())
	{
		if (pHttpNotify->dwErrorCode == 0)
		{
			pHttpNotify->dwErrorCode = 5;
		}
		GetUploadListener()->OnUploadCompleted(pHttpNotify);
	}

	SetUploading(false);
	return true;
}

IUploadListener* CLocalResourceHandler::GetUploadListener()
{
	return m_pUploadListener;
}

IThumbnailListener* CLocalResourceHandler::GetThumbnailListener()
{
	return m_pThumbnailListener;
}

void CLocalResourceHandler::SetUploading( bool bIsDownloading )
{
	m_bIsDownloading = bIsDownloading;
}

bool CLocalResourceHandler::IsUploading()
{
	return m_bIsDownloading;
}
