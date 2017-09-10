#include "StdAfx.h"
#include "DUI/ITransfer.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IDownloadListener.h"
#include "DUI/IVisitor.h"
#include "DUI/BaseParamer.h"
#include "DUI/ResourceDownloader.h"

#include "DUI/ITransfer.h"
#include "DUI/IDownloaderListener.h"
#include "DUI/ResourceDownloaderManager.h"

CResourceDownloader::CResourceDownloader()
{
	m_pParamer		= NULL;
	m_pCopyParamer	= NULL;
	m_bSelfFree		= true;

}


CResourceDownloader::~CResourceDownloader()
{
	ClearListener();
	if (m_pCopyParamer)
	{
		delete m_pCopyParamer;
		m_pCopyParamer = NULL;
	}
}

void CResourceDownloader::AddListener( IDownloadListener* pListener )
{
	if (!pListener)
	{
		return;
	}

	m_mapListener[pListener] = pListener;
}

void CResourceDownloader::AddListener( vector<IDownloadListener*>* pListeners )
{
	if (!pListeners)
	{
		return;
	}

	for (size_t i = 0; i < pListeners->size(); ++i)
	{
		IDownloadListener* pDownloadListener =  pListeners->at(i);
		AddListener(pDownloadListener);
	}
	
}

void CResourceDownloader::RemoveListener( IDownloadListener* pListener )
{
	map<IDownloadListener*, IDownloadListener*>::iterator itor = m_mapListener.find(pListener);
	if (itor != m_mapListener.end())
	{
		m_mapListener.erase(itor);
	}

}

void CResourceDownloader::ClearListener()
{
	m_mapListener.clear();
}


bool CResourceDownloader::OnDownloadResourceProgress( void* pObj )
{
	THttpNotify* pHttpNotify	= (THttpNotify*)pObj;
	pHttpNotify->pDetailData	= this;
	for (map<IDownloadListener*, IDownloadListener*>::iterator itor = m_mapListener.begin(); itor != m_mapListener.end(); ++itor)
	{
		itor->second->OnDownloadProgress(pHttpNotify);
	}
	return true;
}

bool CResourceDownloader::OnDownloadResourcePause( void* pObj )
{
	NotifyDownloadInterpose(eInterpose_Pause);
	return true;
}

void CResourceDownloader::NotifyDownloadInterpose( int nInterpose )
{
	THttpNotify httpNotify;
	httpNotify.pUserData	= m_pParamer;
	httpNotify.pDetailData	= this;
	httpNotify.dwErrorCode	= nInterpose;

	//copy listeners avoid to remove self on callback
	vector<IDownloadListener*> vctrListener;
	for (map<IDownloadListener*, IDownloadListener*>::iterator itor = m_mapListener.begin(); itor != m_mapListener.end(); ++itor)
	{
		vctrListener.push_back(itor->second);
	}

	for (int i = 0; i < vctrListener.size(); ++i)
	{
		vctrListener.at(i)->OnDownloadInterpose(&httpNotify);
	}
}


bool CResourceDownloader::OnDownloadResourceCompleted( void* pObj )
{
	THttpNotify* pHttpNotify	= (THttpNotify*)pObj;
	pHttpNotify->pDetailData	= this;

	//for retry
	if (pHttpNotify->dwErrorCode != 0)
	{
		if (m_pCopyParamer)
		{
			delete m_pCopyParamer;
			m_pCopyParamer = NULL;
		}

		if (m_pParamer)
		{
			m_pCopyParamer = m_pParamer->Copy();
		}
		
	}

	//copy listeners avoid to remove self on callback
	vector<IDownloadListener*> vctrListener;
	for (map<IDownloadListener*, IDownloadListener*>::iterator itor = m_mapListener.begin(); itor != m_mapListener.end(); ++itor)
	{
		vctrListener.push_back(itor->second);
	}

	for (int i = 0; i < vctrListener.size(); ++i)
	{
		vctrListener.at(i)->OnDownloadCompleted(pHttpNotify);
	}

	if(m_bSelfFree)
	{
		delete this;
	}
	return true;
}

void CResourceDownloader::SetParamer( IBaseParamer* pParamer )
{
	m_pParamer = pParamer;
}

void CResourceDownloader::SetIsSelfFree( bool isSelfFree )
{
	m_bSelfFree = isSelfFree;
}
