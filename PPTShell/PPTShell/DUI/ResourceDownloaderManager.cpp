#include "StdAfx.h"
#include "DUI/ITransfer.h"
#include "DUI/IDownloaderListener.h"
#include "DUI/ResourceDownloaderManager.h"



CResourceDownloaderManager::CResourceDownloaderManager()
{

}


CResourceDownloaderManager::~CResourceDownloaderManager()
{

}

CResourceDownloaderManager* CResourceDownloaderManager::m_pThis = NULL;
CResourceDownloaderManager* CResourceDownloaderManager::GetInstance()
{
	if (!m_pThis)
	{
		m_pThis = new CResourceDownloaderManager();
	}

	return m_pThis;
}

void CResourceDownloaderManager::AddListener( IDownloaderListener* pListener )
{
	if (!pListener)
	{
		return;
	}
	m_mapListener[pListener] = pListener;
}

void CResourceDownloaderManager::AddListener( vector<IDownloaderListener*>* pListeners )
{
	if (!pListeners)
	{
		return;
	}

	for (size_t i = 0; i < pListeners->size(); ++i)
	{
		IDownloaderListener* pDownloadListener =  pListeners->at(i);
		if (!pDownloadListener)
		{
			continue;
		}

		m_mapListener[pDownloadListener] = pDownloadListener;
	}
	
}

void CResourceDownloaderManager::RemoveListener( IDownloaderListener* pListener )
{
	map<IDownloaderListener*, IDownloaderListener*>::iterator itor = m_mapListener.find(pListener);
	if (itor != m_mapListener.end())
	{
		m_mapListener.erase(itor);
	}
}

void CResourceDownloaderManager::NotifyDownloaderCreate( CResourceDownloader* pDownloader, CItemHandler* pItemHandler, bool bNotify /*= true*/ )
{
#ifdef DEVELOP_VERSION
	m_mapDownloader[pDownloader] = pItemHandler;
	if (!bNotify)
	{
		return;
	}
	for (map<IDownloaderListener*, IDownloaderListener*>::iterator itor = m_mapListener.begin(); itor != m_mapListener.end(); ++itor)
	{
		itor->second->OnDownloaderCreate(pDownloader, pItemHandler);
	}
#endif
}


void CResourceDownloaderManager::NotifyDownloaderDestroy( CResourceDownloader* pDownloader , int nResult)
{
#ifdef DEVELOP_VERSION
	if (pDownloader)
	{
		m_mapDownloader.erase(pDownloader);
	}

	if (nResult == eResult_Error)
	{
		return;
	}

	for (map<IDownloaderListener*, IDownloaderListener*>::iterator itor = m_mapListener.begin(); itor != m_mapListener.end(); ++itor)
	{
		itor->second->OnDownloaderDestroy(pDownloader, nResult);
	}
#endif
}

map<CResourceDownloader*, CItemHandler*>* CResourceDownloaderManager::GetDownloaders()
{
	return &m_mapDownloader;
}

bool CResourceDownloaderManager::IsDownloaderExists( CResourceDownloader* pDownloader )
{
	map<CResourceDownloader*, CItemHandler*>::iterator itor = m_mapDownloader.find(pDownloader);
	if (itor != m_mapDownloader.end())
	{
		return true;
	}

	return false;
}




