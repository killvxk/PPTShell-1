#include "StdAfx.h"
#include "DUI/ITransfer.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IDownloadListener.h"
#include "DUI/ResourceDownloader.h"
#include "DUI/AssetDownloader.h"
#include "DUI/CourseDownloader.h"

#include "NDCloud/NDCloudAPI.h"

CCourseDownloader::CCourseDownloader()
{

}


CCourseDownloader::~CCourseDownloader()
{

}

bool CCourseDownloader::Transfer()
{
	tstring strMD5;
	if( m_pMD5 != NULL )
		strMD5 = m_pMD5;

	tstring strUrl = m_pUrl;
 	_tcslwr((TCHAR*)strUrl.c_str());
	if(_tcsstr(strUrl.c_str(), _T("http://")))
	{
		__super::m_dwTransferId = NDCloudFileManager::GetInstance()->DownloadFileSearchPlatform(m_pUrl,
			m_pGuid,
			m_pTitle,
			m_nAssetType,
			MakeHttpDelegate(this, &CResourceDownloader::OnDownloadResourceCompleted),
			MakeHttpDelegate(this, &CResourceDownloader::OnDownloadResourceProgress),
			__super::m_pParamer, strMD5);
	}
	else
	{
		__super::m_dwTransferId = NDCloudDownloadCourseFile(m_pUrl,
			m_pGuid,
			m_pTitle,
			m_nAssetType,
			MakeHttpDelegate(this, &CResourceDownloader::OnDownloadResourceCompleted),
			MakeHttpDelegate(this, &CResourceDownloader::OnDownloadResourceProgress),
			__super::m_pParamer,
			strMD5);
	}
	

	if (__super::m_dwTransferId == 0)
	{
		THttpNotify notify;
		notify.dwErrorCode	= 10;
		notify.pUserData	= __super::m_pParamer;
		__super::OnDownloadResourceCompleted(&notify);
	}

	return __super::m_dwTransferId != 0;
}

int CCourseDownloader::GetResourceType()
{
	if (m_nAssetType == CourseFilePPT)
	{
		return CloudFileCourse;
	}
	else if (m_nAssetType == CourseFileThumb)
	{
		return CloudFileImage;
	}

	return m_nAssetType;
}
