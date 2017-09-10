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

#include "DUI/CloudVideoHandler.h"

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

ImplementHandlerId(CCloudVideoHandler);
CCloudVideoHandler::CCloudVideoHandler()
{
	m_bHasPreviewUrl = true;
}


CCloudVideoHandler::~CCloudVideoHandler()
{

}

void CCloudVideoHandler::ReadFrom( CStream* pStream )
{
	m_strGuid		= pStream->ReadString();
	m_strTitle		= pStream->ReadString();
	m_strUrl		= pStream->ReadString();
	if (m_bHasPreviewUrl)
	{
		m_strPreviewUrl	= pStream->ReadString();
	}

	m_strMD5		= pStream->ReadString();
	m_strJsonInfo	= pStream->ReadString();

	InitHandlerId();
}

void CCloudVideoHandler::WriteTo( CStream* pStream )
{
	pStream->WriteString(m_strGuid);
	pStream->WriteString(m_strTitle);
	pStream->WriteString(m_strUrl);
	if (m_bHasPreviewUrl)
	{
		pStream->WriteString(m_strPreviewUrl);
	}
	
	pStream->WriteString(m_strJsonInfo);
}


void CCloudVideoHandler::DoButtonClick(TNotifyUI*	pNotify)
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
		if( GetDownloader() )
			return;

		//delegate
		CInvokeParamer* pParamer = new CInvokeParamer();
		pParamer->SetTrigger(GetTrigger());
		pParamer->SetCompletedDelegate(MakeDelegate(this, &CCloudVideoHandler::OnHandlePreivew));
		
		//don't delete
		CAssetDownloader* pAssetDownloader = new CAssetDownloader();
		pAssetDownloader->SetAssetGuid(GetGuid());
		pAssetDownloader->SetAssetTitle(GetTitle());
		//目前有缩略图的是视频文件，为了少写一个类，orz
		if(m_bHasPreviewUrl)
		{
			pAssetDownloader->SetAssetType(CloudFileVideo);
		}
		else
		{
			pAssetDownloader->SetAssetType(CloudFileVolume);
		}
		pAssetDownloader->SetAssetUrl(GetResource());
		pAssetDownloader->SetParamer(pParamer);
		pAssetDownloader->SetAssetMD5(GetMD5());
		pAssetDownloader->AddListener(&GetDownloadListeners());
		pAssetDownloader->AddListener(this);//self must be added  at rearmost

		SetDownloader(pAssetDownloader);
		pAssetDownloader->Transfer();
		
	}

}

void CCloudVideoHandler::DoSetThumbnail( TNotifyUI* pNotify )
{
	if (!m_bHasPreviewUrl || m_strPreviewUrl.empty())
	{
		return;
	}

	IThumbnailListener* pListener = dynamic_cast<IThumbnailListener*>(pNotify->pSender);
	if (pListener)
	{
		pListener->OnGetThumbnailBefore();
	}

	//delegate
	CInvokeParamer* pParamer = new CInvokeParamer();
	pParamer->SetCompletedDelegate(MakeDelegate(this, &CCloudVideoHandler::OnHandleThumbnail));

	//don't delete
	CAssetDownloader* pAssetDownloader = new CAssetDownloader();
	SetThumbnailDownloader(pAssetDownloader);

	pAssetDownloader->SetThumbnailSize(240);
	pAssetDownloader->SetAssetGuid(GetGuid());
	pAssetDownloader->SetAssetTitle(GetTitle());
	pAssetDownloader->SetAssetType(CloudFileImage);
	pAssetDownloader->SetAssetUrl(m_strPreviewUrl.c_str());
	pAssetDownloader->SetAssetMD5(GetMD5());
	pAssetDownloader->SetParamer(pParamer);
	pAssetDownloader->AddListener(this);//self must be added  at rearmost
	pAssetDownloader->Transfer();
}

void CCloudVideoHandler::DoDropDown( TNotifyUI* pNotify )
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

bool CCloudVideoHandler::OnGetPlaceHolderCompleted( void* pObj )
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
		RemovePalceHolderByThread(dwSlideId, PlaceHolderId);
		//already had downloader
		return true;
	}

	CMainFrame*		pMainFrame	= (CMainFrame*)AfxGetMainWnd();
	CInsertParamer* pParamer	= new CInsertParamer();
	pParamer->SetOperationerId(pMainFrame->GetOperationerId());
	pParamer->SetSlideId(dwSlideId);
	pParamer->SetPlaceHolderId(PlaceHolderId);
	pParamer->SetInsertPos(nX, nY);
	pParamer->SetCompletedDelegate(MakeDelegate(this, &CCloudVideoHandler::OnHandleInsert));
	

	//don't delete
	CAssetDownloader* pAssetDownloader = new CAssetDownloader();
	pAssetDownloader->SetAssetGuid(GetGuid());
	pAssetDownloader->SetAssetTitle(GetTitle());
	//目前有缩略图的是视频文件，为了少写一个类，orz
	if(m_bHasPreviewUrl)
	{
		pAssetDownloader->SetAssetType(CloudFileVideo);
	}
	else
	{
		pAssetDownloader->SetAssetType(CloudFileVolume);
	}
	pAssetDownloader->SetAssetUrl(GetResource());
	pAssetDownloader->SetAssetMD5(GetMD5());
	pAssetDownloader->SetParamer(pParamer);
	pAssetDownloader->AddListener(&GetDownloadListeners());
	pAssetDownloader->AddListener(this);//self must be added  at rearmost

	SetDownloader(pAssetDownloader);
	pAssetDownloader->Transfer();

	return true;
}

bool CCloudVideoHandler::OnHandleInsert( void* pObj )
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

		InsertVideoByThread(
			pParamer->GetHttpNotify()->strFilePath.c_str(),
			pParamer->GetInsertPos().x,
			pParamer->GetInsertPos().y,
			-1,
			-1, 0,
			pParamer->GetSlideId(),
			pParamer->GetPlaceHolderId());

		if(m_bHasPreviewUrl)
		{
			Statistics::GetInstance()->Report(STAT_INSERT_VIDEO);
		}
		else
		{
			Statistics::GetInstance()->Report(STAT_INSERT_VOLUME);
		}
		

	} while (false);
	delete pParamer;
	return true;
}

bool CCloudVideoHandler::OnHandlePreivew( void* pObj )
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

bool CCloudVideoHandler::OnHandleThumbnail( void* pObj )
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

void CCloudVideoHandler::SetHasPreviewUrl( bool bHas )
{
	m_bHasPreviewUrl = bHas;
}

bool CCloudVideoHandler::OnDeleteComplete( void* pObj )
{
	THttpNotify*		pHttpNotify	= (THttpNotify*)pObj;
	CNDCloudContentService* pService = (CNDCloudContentService*)pHttpNotify->pUserData;
	if (pService->IsSuccess())
	{
		CGroupExplorerUI::GetInstance()->AddDBankItemCount(DBankVideo);
	}
	return __super::OnDeleteComplete(pObj);
}




