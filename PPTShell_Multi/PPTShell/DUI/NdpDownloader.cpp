#include "StdAfx.h"
#include "DUI/ITransfer.h"
#include "DUI/IVisitor.h"
#include "DUI/BaseParamer.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IDownloadListener.h"
#include "DUI/ResourceDownloader.h"
#include "DUI/NdpDownloader.h"

#include "NDCloud/NDCloudAPI.h"
#include "NDCloud/NDCloudQuestion.h"

#include "Util/Util.h"

CNdpDownloader::CNdpDownloader()
{
	m_dwGetNdpDownloadUrlId = 0;
	m_dwTransferId			= 0;
	m_pTitle				= NULL;
	m_pGuid					= NULL;
}


CNdpDownloader::~CNdpDownloader()
{

}

bool CNdpDownloader::Transfer()
{
	return GetNdpDownloadUrl();
}

bool CNdpDownloader::OnGetNdpDownloadUrl(void* pObj)
{
	THttpNotify* pNotify = static_cast<THttpNotify*>(pObj);

	if(pNotify->dwErrorCode != 0)
	{
		THttpNotify notify;
		notify.dwErrorCode	= 10;
		notify.pUserData	= __super::m_pParamer;
		__super::OnDownloadResourceCompleted(&notify);
		return false;
	}

	pNotify->pData[pNotify->nDataSize] = 0;
	string str = Utf8ToAnsi(pNotify->pData);
	Json::Reader	reader;
	Json::Value		result;
	if (reader.parse(str.c_str(), result))
	{
		if (!result.get("md5", Json::Value()).isNull() && !result.get("url", Json::Value()).isNull())
		{
			string strMd5 = result["md5"].asCString();
			string strDownloadUrl = result["url"].asCString();

			tstring strNdpFilePath = NDCloudFileManager::GetInstance()->GetNDCloudDirectory();
			strNdpFilePath += _T("\\NdpCourse\\");
			strNdpFilePath += m_pGuid;
			strNdpFilePath += _T("_default_webp.zip");

			tstring strFileMD5 = CalcFileMD5(strNdpFilePath);
			if(strFileMD5 != strMd5)
				DeleteFile(strNdpFilePath.c_str());
			m_dwTransferId = NDCloudDownloadFile(strDownloadUrl,
				_T(""),
				_T(""),
				CloudFileNdpCourse,
				0,
				MakeHttpDelegate(this, &CResourceDownloader::OnDownloadResourceCompleted),
				MakeHttpDelegate(this, &CResourceDownloader::OnDownloadResourceProgress),
				__super::m_pParamer);	
			return true;
		}

		if (!result.get("code", Json::Value()).isNull())
		{
			tstring strCode = result["code"].asCString();
			if(strCode == _T("LC/WAIT") )
			{
				return GetNdpDownloadUrl();
			}
		}
	}

	return true;
}
void CNdpDownloader::SetNdpGuid( LPCTSTR lptcstr )
{
	m_pGuid = lptcstr;
}

void CNdpDownloader::SetNdpTitle( LPCTSTR lptcstr )
{
	m_pTitle = lptcstr;
}

bool CNdpDownloader::GetNdpDownloadUrl()
{
	CHttpDownloadManager* pHttpDownloadManager = HttpDownloadManager::GetInstance();

	TCHAR szPost[1024];
	_stprintf(szPost, _T("/v1.1/coursewares/%s/actions/download"), m_pGuid);

	m_dwGetNdpDownloadUrlId = pHttpDownloadManager->AddTask(_T("mooc-lms.web.sdp.101.com"),
		szPost,
		_T(""), 
		_T("GET"),
		_T(""),
		80,
		MakeHttpDelegate(this, &CNdpDownloader::OnGetNdpDownloadUrl),
		MakeHttpDelegate(NULL),
		MakeHttpDelegate(NULL));


	if (m_dwGetNdpDownloadUrlId == 0)
	{
		THttpNotify notify;
		notify.dwErrorCode	= 10;
		notify.pUserData	= __super::m_pParamer;
		__super::OnDownloadResourceCompleted(&notify);
	}

	return m_dwGetNdpDownloadUrlId != 0;
}

void CNdpDownloader::Cancel()
{
	if (m_dwTransferId)
	{
		NDCloudDownloadCancel(m_dwTransferId);
	}

	if (m_dwGetNdpDownloadUrlId)
	{
		NDCloudDownloadCancel(m_dwGetNdpDownloadUrlId);
	}

	NotifyDownloadInterpose(eInterpose_Cancel);
}

int CNdpDownloader::GetResourceType()
{
	return CloudFileNdpCourse;

}

LPCTSTR CNdpDownloader::GetResourceTitle()
{
	return m_pTitle;
}

void CNdpDownloader::Pause()
{
	if (m_dwTransferId)
	{
		NDCloudDownloadPause(m_dwTransferId, &MakeHttpDelegate(this, &CResourceDownloader::OnDownloadResourcePause));
	}

	if (m_dwGetNdpDownloadUrlId)
	{
		NDCloudDownloadPause(m_dwGetNdpDownloadUrlId, &MakeHttpDelegate(this, &CResourceDownloader::OnDownloadResourcePause));
	}
}

void CNdpDownloader::Resume()
{
	if (m_dwTransferId)
	{
		NDCloudDownloadResume(m_dwTransferId);
	}

	if (m_dwGetNdpDownloadUrlId)
	{
		NDCloudDownloadResume(m_dwGetNdpDownloadUrlId);
	}

	NotifyDownloadInterpose(eInterpose_Resume);
}

ITransfer* CNdpDownloader::Copy()
{
	CNdpDownloader* pDownloader	= new CNdpDownloader;
	pDownloader->m_pParamer		= __super::m_pCopyParamer->Copy();

	pDownloader->m_pGuid		= this->m_pGuid;
	pDownloader->m_pTitle		= this->m_pTitle;

	return pDownloader;
}

LPCTSTR CNdpDownloader::GetResourceGuid()
{
	return m_pGuid;
}
