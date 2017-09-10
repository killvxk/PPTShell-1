#include "StdAfx.h"
#include "DUI/IItemHandler.h"
#include "DUI/INotifyHandler.h"
#include "NDCloud/NDCloudAPI.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IDownloadListener.h"
#include "Util/Stream.h"
#include "DUI/ItemHandler.h"
#include "DUI/CloudResourceHandler.h"
#include "DUI/IThumbnailListener.h"

#include "DUI/CloudFlashHandler.h"

#include "DUI/BaseParamer.h"
#include "DUI/InsertParamer.h"


#include "GUI/MainFrm.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "Util/Util.h"
#include "DUI/PreviewDialogUI.h"
#include "Statistics/Statistics.h"
#include "DUI/IButtonTag.h"

#include "DUI/ITransfer.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IDownloadListener.h"
#include "DUI/ResourceDownloader.h"
#include "DUI/AssetDownloader.h"
#include "NDCloud/NDCloudContentService.h"
#include "GroupExplorer.h"

ImplementHandlerId(CCloudFlashHandler);
CCloudFlashHandler::CCloudFlashHandler()
{

}


CCloudFlashHandler::~CCloudFlashHandler()
{

}

void CCloudFlashHandler::DoButtonClick(TNotifyUI*	pNotify)
{
	__super::DoButtonClick(pNotify);
	
	if (pNotify->wParam == eClickFor_Insert)
	{
		//insert
		DoDropDown(pNotify);
	}
	else if (pNotify->wParam == eClickFor_Preview
			|| pNotify->wParam == eClickFor_Audition)
	{
		//preview

		if (GetDownloader())
		{
			//already had downloader
			return;
		}

		//delegate
		CInvokeParamer* pParamer = new CInvokeParamer();
		pParamer->SetTrigger(GetTrigger());
		pParamer->SetCompletedDelegate(MakeDelegate(this, &CCloudFlashHandler::OnHandlePreivew));
		
		//don't delete
		CAssetDownloader* pAssetDownloader = new CAssetDownloader();
		pAssetDownloader->SetAssetGuid(GetGuid());
		pAssetDownloader->SetAssetTitle(GetTitle());
		pAssetDownloader->SetAssetType(CloudFileFlash);
		pAssetDownloader->SetAssetUrl(GetResource());
		pAssetDownloader->SetAssetMD5(GetMD5());
		pAssetDownloader->SetParamer(pParamer);
		pAssetDownloader->AddListener(&GetDownloadListeners());
		pAssetDownloader->AddListener(this);//self must be added  at rearmost

		SetDownloader(pAssetDownloader);
		pAssetDownloader->Transfer();

		
	}

}

void CCloudFlashHandler::DoSetThumbnail( TNotifyUI* pNotify )
{

}

void CCloudFlashHandler::DoDropDown( TNotifyUI* pNotify )
{
	if (!pNotify)
	{
		return;
	}

	GetPlaceHolderId(pNotify->sType.GetData(),
		pNotify->ptMouse.x,
		pNotify->ptMouse.y,
		-1,
		-1,
		0);
}

bool CCloudFlashHandler::OnGetPlaceHolderCompleted( void* pObj )
{
	__super::OnGetPlaceHolderCompleted(pObj);
	TEventNotify* pNotify	= (TEventNotify*)pObj;
	CStream* pStream		= (CStream*)pNotify->wParam;

	pStream->ResetCursor();

	BOOL	bResult			= pStream->ReadBOOL();
	DWORD	dwSlideId		= pStream->ReadDWORD();
	DWORD	PlaceHolderId	= pStream->ReadDWORD();
	int		nX				= pStream->ReadInt();
	int		nY				= pStream->ReadInt();
	if (!bResult)
	{
		//get placeholder id fail
		return true;
	}

	if (GetDownloader())
	{
		//already had downloader
		return true;
	}

	CMainFrame*		pMainFrame	= (CMainFrame*)AfxGetMainWnd();
	CInsertParamer* pParamer	= new CInsertParamer();
	pParamer->SetOperationerId(pMainFrame->GetOperationerId());
	pParamer->SetSlideId(dwSlideId);
	pParamer->SetPlaceHolderId(PlaceHolderId);
	pParamer->SetInsertPos(nX, nY);
	pParamer->SetCompletedDelegate(MakeDelegate(this, &CCloudFlashHandler::OnHandleInsert));
	

	//don't delete
	CAssetDownloader* pAssetDownloader = new CAssetDownloader();
	pAssetDownloader->SetAssetGuid(GetGuid());
	pAssetDownloader->SetAssetTitle(GetTitle());
	pAssetDownloader->SetAssetType(CloudFileFlash);
	pAssetDownloader->SetAssetUrl(GetResource());
	pAssetDownloader->SetAssetMD5(GetMD5());
	pAssetDownloader->SetParamer(pParamer);
	pAssetDownloader->AddListener(&GetDownloadListeners());
	pAssetDownloader->AddListener(this);//self must be added  at rearmost

	SetDownloader(pAssetDownloader);
	pAssetDownloader->Transfer();

	return true;
}

bool CCloudFlashHandler::OnHandleInsert( void* pObj )
{
	CInsertParamer* pParamer = (CInsertParamer*)pObj;
	//insert

	do 
	{
		if (!pParamer->GetHttpNotify()
			|| pParamer->GetHttpNotify()->dwErrorCode != 0)
		{
			RemovePalceHolderByThread(pParamer->GetSlideId(), pParamer->GetPlaceHolderId());
			CToast::Toast(_STR_FILE_DWONLOAD_FAILED);
			break;
		}

		CMainFrame*		pMainFrame	= (CMainFrame*)AfxGetMainWnd();
		if (pMainFrame->IsOperationerChanged(pParamer->GetOperationerId()))
		{
			break;
		}

		InsertSwfByThread(
			pParamer->GetHttpNotify()->strFilePath.c_str(),
			_T(""),
			GetTitle(),
			PLUGIN_SIGN,
			0,
			pParamer->GetSlideId(),
			pParamer->GetPlaceHolderId());

		Statistics::GetInstance()->Report(STAT_INSERT_FLASH);

	} while (false);
	delete pParamer;
	return true;
}

bool CCloudFlashHandler::OnHandlePreivew( void* pObj )
{
	CInvokeParamer* pParamer = (CInvokeParamer*)pObj;
	
	do 
	{
		if (!pParamer->GetHttpNotify()
			|| pParamer->GetHttpNotify()->dwErrorCode != 0)
		{
			CToast::Toast(_STR_FILE_DWONLOAD_FAILED);
			break;
		}

		if (!GetTrigger()
			|| GetTrigger() != pParamer->GetTrigger()
			|| !GetTrigger()->IsVisible())
		{
			break;
		}

		OpenAsDefaultExec(pParamer->GetHttpNotify()->strFilePath.c_str());

	} while (false);

	delete pParamer;
	return true;
}

bool CCloudFlashHandler::OnHandleThumbnail( void* pObj )
{
	CInvokeParamer* pParamer = (CInvokeParamer*)pObj;
	//notify
	NotifyThumbnailCompleted(pParamer->GetHttpNotify());

	//clean user data
	if (pParamer->GetHttpNotify())
	{
		pParamer->GetHttpNotify()->pUserData = NULL;
	}
	delete pParamer;
	return true;
}

bool CCloudFlashHandler::OnDeleteComplete( void* pObj )
{
	THttpNotify*		pHttpNotify	= (THttpNotify*)pObj;
	CNDCloudContentService* pService = (CNDCloudContentService*)pHttpNotify->pUserData;
	if (pService->IsSuccess())
	{
		CGroupExplorerUI::GetInstance()->AddDBankItemCount(DBankFlash);
	}
	return __super::OnDeleteComplete(pObj);
}
