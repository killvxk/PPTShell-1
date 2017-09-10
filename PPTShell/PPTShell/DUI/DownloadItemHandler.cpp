#include "StdAfx.h"
#include "DUI/ITransfer.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IDownloadListener.h"
#include "DUI/ResourceDownloader.h"

#include "Util/Stream.h"
#include "DUI/ItemHandler.h"
#include "DUI/DownloadItemHandler.h"
#include "DUI/IButtonTag.h"

#include "DUI/ResourceDownloaderManager.h"

CDownloadItemHandler::CDownloadItemHandler()
{
	m_pDownloader			= NULL;
	m_pRetryTransfer		= NULL;
	m_pOriginalItemHandler	= NULL;
}	


CDownloadItemHandler::~CDownloadItemHandler()
{

}

void CDownloadItemHandler::DoInit( CControlUI* pHolder )
{
	
}

void CDownloadItemHandler::DoDestroy( CControlUI* pHolder )
{
	RemoveHolder(pHolder);

	delete this;
}

void CDownloadItemHandler::DoButtonClick(TNotifyUI*	pNotify)
{
	__super::DoButtonClick(pNotify);

	if (pNotify->wParam == eClickFor_Start)
	{
		if (GetDownloader())
		{
			GetDownloader()->Resume();
		}
	}
	else if (pNotify->wParam == eClickFor_Pause)
	{	
		if (GetDownloader())
		{
			GetDownloader()->Pause();
		}
	}
	else if (pNotify->wParam == eClickFor_Retry)
	{
		if (m_pRetryTransfer)
		{
			CResourceDownloader* pDownloader = (CResourceDownloader*)m_pRetryTransfer;
			pDownloader->AddListener(this);

			SetDownloader(pDownloader);

 			IDownloaderRefresher* pDownloaderRefresher = dynamic_cast<IDownloaderRefresher*>(m_pOriginalItemHandler);
 			if (pDownloaderRefresher)
 			{
 				pDownloaderRefresher->RefreshDownloader(pDownloader);
 			}

			m_pRetryTransfer->Transfer();
			
		}
	}
	else if (pNotify->wParam == eClickFor_Close)
	{
		if (GetDownloader())
		{
			GetDownloader()->Cancel();
		}
		else
		{
			//remove item
			if (GetTrigger())
			{
				GetTrigger()->GetManager()->SendNotify(GetTrigger()->GetParent(), _T("itemremove"), (WPARAM)GetTrigger(), 0, true);
			}

			CResourceDownloaderManager::GetInstance()->NotifyDownloaderDestroy(NULL, true);
		}
	}
}

void CDownloadItemHandler::SetDownloader( CResourceDownloader* pDownloader )
{
	m_pDownloader = pDownloader;

}

void CDownloadItemHandler::SetOriginalHandler( CItemHandler* pItemHandler )
{
	m_pOriginalItemHandler = pItemHandler;
}


CItemHandler* CDownloadItemHandler::GetOriginalHandler()
{
	return m_pOriginalItemHandler;
}

CResourceDownloader* CDownloadItemHandler::GetDownloader()
{
	return m_pDownloader;
}

void CDownloadItemHandler::AddHolder( CControlUI* pHolder )
{
	__super::AddHolder(pHolder);

	if (m_pOriginalItemHandler)
	{
		m_pOriginalItemHandler->AddHolder(pHolder);
	}
	
	if (GetDownloader())
	{
		GetDownloader()->AddListener(this);
	}
}

void CDownloadItemHandler::RemoveHolder( CControlUI* pHolder )
{
	__super::RemoveHolder(pHolder);

	if (m_pOriginalItemHandler)
	{
		m_pOriginalItemHandler->RemoveHolder(pHolder);
	}

	if (GetDownloader())
	{
		GetDownloader()->RemoveListener(this);
	}

}

void CDownloadItemHandler::OnDownloadBefore( THttpNotify* pHttpNotify )
{

}

void CDownloadItemHandler::OnDownloadProgress( THttpNotify* pHttpNotify )
{

}

void CDownloadItemHandler::OnDownloadInterpose( THttpNotify* pHttpNotify )
{
	if (pHttpNotify->dwErrorCode == eInterpose_Cancel)
	{
		//if success,we should remove downitem from original item handler
		//for bug 11397 from qc
		if (m_pOriginalItemHandler)
		{
			m_pOriginalItemHandler->RemoveHolder(GetHolderAt(0));
			m_pOriginalItemHandler = NULL;
		}
	}
}

void CDownloadItemHandler::OnDownloadCompleted( THttpNotify* pHttpNotify )
{
	if (pHttpNotify->dwErrorCode != 0)
	{
		ITransfer* pTransfer = dynamic_cast<ITransfer*>((ITransfer*)pHttpNotify->pDetailData);
		m_pRetryTransfer = pTransfer->Copy();
	}
	else
	{
		//if success,we should remove downitem from original item handler
		//for bug 11397 from qc
		if (m_pOriginalItemHandler)
		{
			m_pOriginalItemHandler->RemoveHolder(GetHolderAt(0));
			m_pOriginalItemHandler = NULL;
		}
	}

	

	SetDownloader(NULL);
}


void CDownloadItemHandler::InitHandlerId()
{
	throw std::exception("The method or operation is not implemented.");
}

tstring CDownloadItemHandler::GetHandlerId()
{
	throw std::exception("The method or operation is not implemented.");
}

void CDownloadItemHandler::SetHandlerId( LPCTSTR lptcsId )
{
	throw std::exception("The method or operation is not implemented.");
}

void CDownloadItemHandler::DoSetThumbnail( TNotifyUI* pNotify )
{
	throw std::exception("The method or operation is not implemented.");
}

void CDownloadItemHandler::DoDropDown( TNotifyUI* pNotify )
{
	throw std::exception("The method or operation is not implemented.");
}

void CDownloadItemHandler::ReadFrom( CStream* pStream )
{
	throw std::exception("The method or operation is not implemented.");
}

void CDownloadItemHandler::WriteTo( CStream* pStream )
{
	throw std::exception("The method or operation is not implemented.");
}

LPCTSTR CDownloadItemHandler::GetResource()
{
	throw std::exception("The method or operation is not implemented.");
}

LPCTSTR CDownloadItemHandler::GetTitle()
{
	throw std::exception("The method or operation is not implemented.");
}

LPCTSTR CDownloadItemHandler::GetGuid()
{
	throw std::exception("The method or operation is not implemented.");
}
