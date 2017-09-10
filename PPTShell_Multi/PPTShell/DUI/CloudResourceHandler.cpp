#include "StdAfx.h"
#include "DUI/IItemHandler.h"
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
#include "DUI/IDownloadListener.h"
#include "DUI/IThumbnailListener.h"
#include "DUI/ITransfer.h"
#include "DUI/ResourceDownloader.h"
#include "DUI/AssetDownloader.h"
#include "DUI/CloudResourceHandler.h"
#include "DUI/BaseParamer.h"
#include "DUI/ItemMenu.h"
#include "DUI/IFunctionListener.h"
#include "NDCloud/NDCloudUser.h"
#include "Util/Util.h"
#include "NDCloud/NDCloudContentService.h"
#include "DUI/RenameDlgUI.h"
#include "DUI/ItemExplorer.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IVisitor.h"
#include "DUI/BaseParamer.h"
#include "DUI/InsertParamer.h"

#include "DUI/ITransfer.h"
#include "DUI/IDownloaderListener.h"
#include "DUI/ResourceDownloaderManager.h"

CCloudResourceHandler::CCloudResourceHandler()
{
	isDeleting = false;
	m_pThumbnailListener	= NULL;
	m_pCurrentDownloader	= NULL;
	m_pThumbnailDownloader	= NULL;

	m_bIsDBank				= false;
	m_nConentType			= 0;

	m_nGettingPlaceHolderReference	= 0;

}

CCloudResourceHandler::~CCloudResourceHandler()
{

}

void CCloudResourceHandler::DoClick(TNotifyUI*	pNotify)
{
	__super::DoClick(pNotify);
	CItemExplorerUI::GetInstance()->ShowWindow(false);
}

void CCloudResourceHandler::DoButtonClick(TNotifyUI*	pNotify)
{
	__super::DoButtonClick(pNotify);
}

void CCloudResourceHandler::DoRClick( TNotifyUI* pNotify )
{
	__super::DoRClick(pNotify);

	if (IsDbank()&&!isDeleting)
	{
		//
		CItemMenuUI* pMenu = new CItemMenuUI();
		pMenu->SetHolder(pNotify->pSender);
		pMenu->CreateMenu();
		pMenu->AddMenuItem(_T("重命名"), eMenu_Rename);
		pMenu->AddMenuSplitter();
		pMenu->AddMenuItem(_T("删除"), eMenu_Delete);
		pMenu->SetLeftWeight(0.4f);
		pMenu->ShowMenu();

	}
}

void CCloudResourceHandler::DoMenuClick(TNotifyUI*	pNotify)
{
	__super::DoMenuClick(pNotify);

	if (pNotify->wParam == eMenu_Rename)
	{
		Rename();
	}
	else if (pNotify->wParam == eMenu_Delete)
	{
		DWORD dwService =(DWORD)GetCycleCount();
		AddContentService(dwService);
		BOOL bRet = NDCloudContentServiceManager::GetInstance()->DeleteCloudFile(
			NDCloudUser::GetInstance()->GetUserId(),
			GetGuid(),
			GetContentType(),
			GetResource(),
			MakeHttpDelegate(this, &CCloudResourceHandler::OnDeleteComplete),
			(void*)dwService);
		if (!bRet)
		{
			RemoveContentService(dwService);
		}
		else
		{
			isDeleting = true;
		}
	}
}

void CCloudResourceHandler::DoKeyDown( TEventUI* pEvent )
{
	__super::DoKeyDown(pEvent);

	if (pEvent->chKey == VK_F2)
	{
		if (IsDbank())
		{
			Rename();
		}
	}
}


LPCTSTR CCloudResourceHandler::GetResource()
{
	return m_strUrl.c_str();
}

LPCTSTR CCloudResourceHandler::GetTitle()
{
	return m_strTitle.c_str();
}

LPCTSTR CCloudResourceHandler::GetGuid()
{
	return m_strGuid.c_str();
}

LPCTSTR CCloudResourceHandler::GetMD5()
{
	return m_strMD5.c_str();
}

void CCloudResourceHandler::ReadFrom( CStream* pStream )
{
	m_strGuid		= pStream->ReadString();
	m_strTitle		= pStream->ReadString();
	m_strUrl		= pStream->ReadString();
	m_strMD5		= pStream->ReadString();
	m_strJsonInfo	= pStream->ReadString();

	InitHandlerId();
}

void CCloudResourceHandler::WriteTo( CStream* pStream )
{
	pStream->WriteString(m_strGuid);
	pStream->WriteString(m_strTitle);
	pStream->WriteString(m_strUrl);

	//no used now
	//pStream->WriteString(m_strJsonInfo);
}

void CCloudResourceHandler::OnDownloadBefore( THttpNotify* pHttpNotify )
{

}

void CCloudResourceHandler::OnDownloadProgress( THttpNotify* pHttpNotify )
{

}

void CCloudResourceHandler::OnDownloadCompleted( THttpNotify* pHttpNotify )
{
	IBaseParamer*	pParamer= (IBaseParamer*)pHttpNotify->pUserData;
	ITransfer*		pTranfer= (ITransfer*)pHttpNotify->pDetailData;

	if (pHttpNotify->dwErrorCode != 0)
	{
		RemoveDownloader((CResourceDownloader*)pTranfer, true);
	}
	else
	{
		RemoveDownloader((CResourceDownloader*)pTranfer);
	}

	if (pParamer)
	{
		pParamer->InvokeHandler(pHttpNotify, pTranfer);
	}
	
	DestroyPossible();
}

void CCloudResourceHandler::OnDownloadInterpose( THttpNotify* pHttpNotify )
{
	if (pHttpNotify->dwErrorCode == eInterpose_Cancel)
	{
		ITransfer*		pTranfer= (ITransfer*)pHttpNotify->pDetailData;
		RemoveDownloader((CResourceDownloader*)pTranfer, true);

		CInsertParamer* pParamer = dynamic_cast<CInsertParamer*>((IBaseParamer*)pHttpNotify->pUserData);
		if (pParamer)
		{
			RemovePalceHolderByThread(pParamer->GetSlideId(), pParamer->GetPlaceHolderId());
		}

		DestroyPossible();
	}
}

void CCloudResourceHandler::DoInit(CControlUI* pHolder)
{
	CItemHandler* pHandler = CItemHandler::GetExistedHandler(GetHandlerId().c_str());
	if (pHandler)
	{
		IHandlerVisitor* pHandlerVisitor = dynamic_cast<IHandlerVisitor*>(pHolder);
		if (pHandlerVisitor->GetHandler() != pHandler)
		{
			pHandlerVisitor->SetHandler(pHandler);
			delete this;
		}
	}
	else
	{
		CItemHandler::AppendHandler(GetHandlerId().c_str(), this);
	}

}

void CCloudResourceHandler::DoDestroy(CControlUI* pHolder)
{
	RemoveHolder(pHolder);


	DestroyPossible();
}

void CCloudResourceHandler::SetThumbnailDownloader( CResourceDownloader* pDownloader )
{
	m_pThumbnailDownloader = pDownloader;
}

CResourceDownloader* CCloudResourceHandler::GetThumbnailDownloader()
{
	return m_pThumbnailDownloader;
}

CResourceDownloader* CCloudResourceHandler::GetDownloader()
{
	return m_pCurrentDownloader;
}

void CCloudResourceHandler::SetDownloader( CResourceDownloader* pDownloader )
{
	m_pCurrentDownloader = pDownloader;
	CResourceDownloaderManager::GetInstance()->NotifyDownloaderCreate(pDownloader, this);
}

void CCloudResourceHandler::RefreshDownloader( CResourceDownloader* pDownloader )
{
	m_pCurrentDownloader = pDownloader;

	m_pCurrentDownloader->AddListener(&GetDownloadListeners());
	m_pCurrentDownloader->AddListener(this);

	CResourceDownloaderManager::GetInstance()->NotifyDownloaderCreate(pDownloader, this, false);
}

void CCloudResourceHandler::RemoveDownloader( CResourceDownloader* pDownloader, bool bCancel /*= false*/ )
{
	if (m_pCurrentDownloader == pDownloader)
	{
		m_pCurrentDownloader = NULL;
		CResourceDownloaderManager::GetInstance()->NotifyDownloaderDestroy(pDownloader, bCancel);
	}

	if (m_pThumbnailDownloader == pDownloader)
	{
		m_pThumbnailDownloader = NULL;
	}
}

bool CCloudResourceHandler::HasDownloaders()
{
	if (m_pCurrentDownloader
		|| m_pThumbnailDownloader)
	{
		return true;
	}

	return false;
}

void CCloudResourceHandler::SetIsDbank( bool bIsDbank )
{
	m_bIsDBank = bIsDbank;
}

bool CCloudResourceHandler::IsDbank()
{
	return m_bIsDBank;
}

int CCloudResourceHandler::GetContentType()
{
	return m_nConentType;
}

void CCloudResourceHandler::SetContentType( int nType )
{
	m_nConentType = nType;
}

void CCloudResourceHandler::AddContentService( DWORD dwServiceId )
{
	m_mapContentService[dwServiceId] = dwServiceId;
}

void CCloudResourceHandler::RemoveContentService( DWORD dwServiceId )
{
	m_mapContentService.erase(dwServiceId);
}

bool CCloudResourceHandler::HasContentService( )
{
	return m_mapContentService.size() != 0;
}

bool CCloudResourceHandler::OnDeleteComplete( void* pObj )
{
	isDeleting = false;
	THttpNotify*		pHttpNotify	= (THttpNotify*)pObj;
	CNDCloudContentService* pService= (CNDCloudContentService*)pHttpNotify->pUserData;

	for (int i = 0; i < GetHolderCount(); ++i)
	{
		IDeleteListener*	pDeleteable	= dynamic_cast<IDeleteListener*>(GetHolderAt(i));
		if (!pDeleteable)
		{
			continue;
		}

		if (pService->IsSuccess())
		{
			pDeleteable->OnDeleteComplete(NULL);
		}
		else
		{
			pDeleteable->OnDeleteComplete(pService->GetErrorMessage().c_str());
		}
	}
	
	
	RemoveContentService((DWORD)pService->GetUserData());
	DestroyPossible();
	return true;
}

bool CCloudResourceHandler::OnRenameComplete( void* pObj )
{
	THttpNotify* pHttpNotify= (THttpNotify*)pObj;
	
	CNDCloudContentService* pService = (CNDCloudContentService*)pHttpNotify->pUserData;
	CStream*	pStream		= (CStream*)pService->GetUserData();
	pStream->ResetCursor();
	DWORD dwServiceContent	= pStream->ReadDWORD();
	tstring strNewName		= pStream->ReadString();
	delete pStream;

	for (int i = 0; i < GetHolderCount(); ++i)
	{
		IRenameListener* pRenameable= dynamic_cast<IRenameListener*>(GetHolderAt(i));

		if (!pRenameable)
		{
			continue;
		}

		if (pService->IsSuccess())
		{
			m_strTitle = strNewName;
			pRenameable->OnRenameComplete(NULL, strNewName.c_str());
		}
		else
		{
			pRenameable->OnRenameComplete(pService->GetErrorMessage().c_str(), NULL);
		}
	}

	RemoveContentService(dwServiceContent);
	DestroyPossible();
	return true;
}

void CCloudResourceHandler::DestroyPossible()
{
	if (GetHolderCount() > 0)
	{
		return;
	}

	if (HasDownloaders())
	{
		return;
	}

	if (HasContentService())
	{
		return;
	}

	if (m_nGettingPlaceHolderReference > 0)
	{
		return;
	}

	CItemHandler::RemoveHandler(GetHandlerId().c_str());

	//安全判断，临时解决崩溃bug 
	if( this != NULL )
		delete this;
}	

void CCloudResourceHandler::Rename()
{
	CRenameDlgUI renameDlg;

	renameDlg.Create(AfxGetMainWnd()->GetSafeHwnd(), _T("RenameWindow"), WS_POPUP , 0);
	renameDlg.SetOldName2Edit(GetTitle());
	renameDlg.AdjustWindowPos();

	if(renameDlg.ShowModal() != IDOK)
	{
		GetTrigger()->SetFocus();
		return;
	}
	GetTrigger()->SetFocus();

	CStream* pStream	= new CStream(512);
	DWORD dwService		=(DWORD)GetCycleCount();
	AddContentService(dwService);
	pStream->WriteDWORD(dwService);
	pStream->WriteString(renameDlg.GetNewName().c_str());

	BOOL bRet = NDCloudContentServiceManager::GetInstance()->RenameCloudFile(
																GetGuid(),
																renameDlg.GetNewName().c_str(),
																m_strJsonInfo.c_str(),
																GetContentType(),
																MakeHttpDelegate(this, &CCloudResourceHandler::OnRenameComplete),
																(void*)pStream);

	if (!bRet)
	{
		RemoveContentService(dwService);
		delete pStream;
	}
}

bool CCloudResourceHandler::GetPlaceHolderId( LPCTSTR szPath,
											 int nLeft /*= -1*/,
											 int nTop /*= -1*/,
											 int nWidth /*= -1*/,
											 int nHeight /*= -1*/,
											 int nSlideIdx )
{
	m_nGettingPlaceHolderReference ++;
	InsertPalceHolderByThread(szPath,
		nLeft,
		nTop,
		nWidth,
		nHeight,
		nSlideIdx,
		&MakeEventDelegate(this, &CCloudResourceHandler::OnGetPlaceHolderCompleted));

	return true;
}

bool CCloudResourceHandler::OnGetPlaceHolderCompleted( void* pObj )
{
	m_nGettingPlaceHolderReference --;

	return true;

}

vector<IDownloadListener*> CCloudResourceHandler::GetDownloadListeners()
{
	vector<IDownloadListener*> listeners;

	for (size_t i = 0; i< m_vctrHolder.size(); ++i)
	{

		IDownloadListener* pDownloadListener = dynamic_cast<IDownloadListener*>(m_vctrHolder.at(i));
		if (!pDownloadListener)
		{
			pDownloadListener = dynamic_cast<IDownloadListener*>((CWindowWnd*)m_vctrHolder.at(i));
		}
		if (pDownloadListener)
		{
			listeners.push_back(pDownloadListener);
		}
	}
	return listeners;
}

void CCloudResourceHandler::NotifyThumbnailCompleted( THttpNotify* pHttpNotify )
{
	if (!pHttpNotify)
	{
		return;
	}

	for (size_t i = 0; i< m_vctrHolder.size(); ++i)
	{
		IThumbnailListener* pThumbnailListener = dynamic_cast<IThumbnailListener*>(m_vctrHolder.at(i));
		if (pThumbnailListener)
		{
			pThumbnailListener->OnGetThumbnailCompleted(pHttpNotify->strFilePath.c_str());
		}
	}
}

void CCloudResourceHandler::AddHolder( CControlUI* pHolder )
{
	__super::AddHolder(pHolder);

	if (GetDownloader())
	{
		IDownloadListener* pDownloadListener = dynamic_cast<IDownloadListener*>(pHolder);
		if (!pDownloadListener)
		{
			pDownloadListener = dynamic_cast<IDownloadListener*>((CWindowWnd*)pHolder);
		}

		GetDownloader()->AddListener(pDownloadListener);
	}
}

void CCloudResourceHandler::RemoveHolder( CControlUI* pHolder )
{
	__super::RemoveHolder(pHolder);

	if (GetDownloader())
	{
		IDownloadListener* pDownloadListener = dynamic_cast<IDownloadListener*>(pHolder);
		if (!pDownloadListener)
		{
			pDownloadListener = dynamic_cast<IDownloadListener*>((CWindowWnd*)pHolder);
		}

		GetDownloader()->RemoveListener(pDownloadListener);
	}
}
