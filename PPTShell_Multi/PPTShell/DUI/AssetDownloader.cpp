#include "StdAfx.h"
#include "DUI/ITransfer.h"
#include "DUI/IVisitor.h"
#include "DUI/BaseParamer.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IDownloadListener.h"
#include "DUI/ResourceDownloader.h"
#include "DUI/AssetDownloader.h"

#include "NDCloud/NDCloudAPI.h"

CAssetDownloader::CAssetDownloader()
{
	m_pGuid				=  NULL;
	m_pUrl				=  NULL;
	m_pTitle			=  NULL;
	m_pMD5				=  NULL;
	m_nAssetType		= -1;
	m_nThumbnailSize	= 0;
	m_dwTransferId		= 0;
}


CAssetDownloader::~CAssetDownloader()
{

}

bool CAssetDownloader::Transfer()
{
	tstring strMD5;
	if( m_pMD5 != NULL )
		strMD5 = m_pMD5;

	tstring strUrl = m_pUrl;
	_tcslwr((char *)strUrl.c_str());
	if(_tcsstr(strUrl.c_str(), _T("http://")))
	{
		m_dwTransferId  = NDCloudFileManager::GetInstance()->DownloadFileSearchPlatform(m_pUrl,
			m_pGuid,
			m_pTitle,
			m_nAssetType,
			MakeHttpDelegate(this, &CResourceDownloader::OnDownloadResourceCompleted),
			MakeHttpDelegate(this, &CResourceDownloader::OnDownloadResourceProgress),
			__super::m_pParamer, strMD5);
	}
	else
	{
		m_dwTransferId  = NDCloudDownloadFile(m_pUrl,
			m_pGuid,
			m_pTitle,
			m_nAssetType,
			m_nThumbnailSize,
			MakeHttpDelegate(this, &CResourceDownloader::OnDownloadResourceCompleted),
			MakeHttpDelegate(this, &CResourceDownloader::OnDownloadResourceProgress),
			__super::m_pParamer, strMD5);
	}
	


	if (m_dwTransferId == 0)
	{
		THttpNotify notify;
		notify.dwErrorCode	= 10;
		notify.pUserData	= __super::m_pParamer;
		__super::OnDownloadResourceCompleted(&notify);
	}

	return m_dwTransferId != 0;
}

void CAssetDownloader::SetAssetGuid( LPCTSTR lptcstr )
{
	m_pGuid = lptcstr;
}

void CAssetDownloader::SetAssetType( int nType )
{
	m_nAssetType = nType;
}

void CAssetDownloader::SetAssetUrl( LPCTSTR lptcstr )
{
	m_pUrl = lptcstr;
}

void CAssetDownloader::SetAssetTitle( LPCTSTR lptcstr )
{
	m_pTitle = lptcstr;
}

void CAssetDownloader::SetThumbnailSize( int nSize )
{
	m_nThumbnailSize = nSize;
}

void CAssetDownloader::SetAssetMD5(LPCTSTR lptcstr)
{
	m_pMD5 = lptcstr;
}

void CAssetDownloader::Cancel()
{
	if (!m_dwTransferId)
	{
		return;
	}
	NDCloudDownloadCancel(m_dwTransferId);
	NotifyDownloadInterpose(eInterpose_Cancel);
}

void CAssetDownloader::Pause()
{
	if (!m_dwTransferId)
	{
		return;
	}
	NDCloudDownloadPause(m_dwTransferId, &MakeHttpDelegate(this, &CResourceDownloader::OnDownloadResourcePause));
}

void CAssetDownloader::Resume()
{
	if (!m_dwTransferId)
	{
		return;
	}
	NDCloudDownloadResume(m_dwTransferId);
	NotifyDownloadInterpose(eInterpose_Resume);
}

ITransfer* CAssetDownloader::Copy()
{
	CAssetDownloader* pDownloader = new CAssetDownloader;
	pDownloader->m_pParamer			= __super::m_pCopyParamer->Copy();
//	pDownloader->m_mapListener		= __super::m_mapListener;

	pDownloader->m_nAssetType		= this->m_nAssetType;
	pDownloader->m_nThumbnailSize	= this->m_nThumbnailSize;
	pDownloader->m_pGuid			= this->m_pGuid;
	pDownloader->m_pMD5				= this->m_pMD5;
	pDownloader->m_pTitle			= this->m_pTitle;
	pDownloader->m_pUrl				= this->m_pUrl;
	

	return pDownloader;
}

int CAssetDownloader::GetResourceType()
{
	return m_nAssetType;
}

LPCTSTR CAssetDownloader::GetResourceTitle()
{
	return m_pTitle;
}

LPCTSTR CAssetDownloader::GetResourceGuid()
{
	return m_pGuid;
}
