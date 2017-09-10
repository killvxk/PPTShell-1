#include "StdAfx.h"
#include "DUI/IItemHandler.h"
#include "DUI/INotifyHandler.h"
#include "NDCloud/NDCloudAPI.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IDownloadListener.h"
#include "Util/Stream.h"
#include "DUI/IStreamReader.h"
#include "DUI/IVisitor.h"
#include "DUI/ItemHandler.h"
#include "DUI/CloudResourceHandler.h"
#include "DUI/IThumbnailListener.h"

#include "DUI/CloudPhotoHandler.h"

#include "DUI/BaseParamer.h"
#include "DUI/InsertParamer.h"


#include "GUI/MainFrm.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "Util/Util.h"
#include <Windef.h>
#include "DUI/PreviewDialogUI.h"
#include "Statistics/Statistics.h"

#include "DUI/ITransfer.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IDownloadListener.h"
#include "DUI/ResourceDownloader.h"
#include "DUI/AssetDownloader.h"
#include "DUI/IButtonTag.h"
#include "GroupExplorer.h"
#include "NDCloud/NDCloudContentService.h"


CCloudPhotoHandler::CCloudPhotoHandler()
{

}


CCloudPhotoHandler::~CCloudPhotoHandler()
{

}

void CCloudPhotoHandler::DoButtonClick(TNotifyUI*	pNotify)
{
	__super::DoButtonClick(pNotify);
	
	if (pNotify->wParam == eClickFor_Insert)
	{
		//insert
		DoDropDown(pNotify);
	}
	else if (pNotify->wParam == eClickFor_Preview)
	{
		//preview
 		CContainerUI*	pParaent	= (CContainerUI*)GetTrigger()->GetParent();
		int				nCount		= pParaent->GetCount();
 		CStream stream(1024);
 		stream.WriteDWORD(pParaent->GetItemIndex(GetTrigger()));
		stream.WriteDWORD(nCount);

		int nCountTmp = 0;
		for (int i = 0; i < nCount; ++i)
		{
			IHandlerVisitor* pHandlerVisitor	= dynamic_cast<IHandlerVisitor*>(pParaent->GetItemAt(i));
			if (pHandlerVisitor)
			{
				CCloudPhotoHandler* pHandler	= dynamic_cast<CCloudPhotoHandler*>(pHandlerVisitor->GetHandler());
				if(pHandler)
				{
					nCountTmp++;
					pHandler->WriteTo(&stream);
				}
			}
		}	
 		stream.ResetCursor();
 
		//
		stream.ReadDWORD();
		stream.WriteDWORD(nCountTmp);
		stream.ResetCursor();
		//

 		CRect rect;
 		HWND hwnd = AfxGetMainWnd()->GetSafeHwnd();
 		::GetWindowRect(hwnd, &rect);
 		CPreviewDlgUI * pPreviewDlg = new CPreviewDlgUI();
 		pPreviewDlg->Create(hwnd, _T("PreviewWindow"), WS_POPUP , 0, 0, 0, 0, 0);
 		pPreviewDlg->Init(rect);
 		pPreviewDlg->ShowPreviewWindow(&stream, E_WND_PICTURE);
 		pPreviewDlg->ShowModal();
 		GetTrigger()->SetFocus();
 
 		Statistics::GetInstance()->Report(STAT_PREIVIEW_PICUTURE);
	}

}

void CCloudPhotoHandler::DoSetThumbnail( TNotifyUI* pNotify )
{
	IThumbnailListener* pListener = dynamic_cast<IThumbnailListener*>(pNotify->pSender);
	if (pListener)
	{
		pListener->OnGetThumbnailBefore();
	}

	//delegate
	CInvokeParamer* pParamer = new CInvokeParamer();
	pParamer->SetCompletedDelegate(MakeDelegate(this, &CCloudPhotoHandler::OnHandleThumbnail));

	//don't delete
	CAssetDownloader* pAssetDownloader = new CAssetDownloader();
	SetThumbnailDownloader(pAssetDownloader);

	pAssetDownloader->SetThumbnailSize(240);
	pAssetDownloader->SetAssetGuid(GetGuid());
	pAssetDownloader->SetAssetTitle(GetTitle());
	pAssetDownloader->SetAssetType(CloudFileImage);
	pAssetDownloader->SetAssetUrl(GetResource());
	pAssetDownloader->SetAssetMD5(GetMD5());
	pAssetDownloader->SetParamer(pParamer);
	pAssetDownloader->AddListener(this);//self must be added  at rearmost
	pAssetDownloader->Transfer();

}

bool CCloudPhotoHandler::OnHandleThumbnail( void* pObj )
{
	CInvokeParamer* pParamer = (CInvokeParamer*)pObj;
	//insert
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

void CCloudPhotoHandler::DoDropDown( TNotifyUI* pNotify )
{
	if (!pNotify)
	{
		return;
	}

	GetPlaceHolderId(pNotify->sType.GetData(),
		pNotify->ptMouse.x,
		pNotify->ptMouse.y,
		(int)LOWORD(pNotify->lParam),
		(int)HIWORD(pNotify->lParam),
		0);
}

bool CCloudPhotoHandler::OnGetPlaceHolderCompleted( void* pObj )
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
	pParamer->SetCompletedDelegate(MakeDelegate(this, &CCloudPhotoHandler::OnHandleInsert));
	

	//don't delete
	CAssetDownloader* pAssetDownloader = new CAssetDownloader();
	pAssetDownloader->SetAssetGuid(GetGuid());
	pAssetDownloader->SetAssetTitle(GetTitle());
	pAssetDownloader->SetAssetType(CloudFileImage);
	pAssetDownloader->SetAssetUrl(GetResource());
	pAssetDownloader->SetParamer(pParamer);
	pAssetDownloader->AddListener(&GetDownloadListeners());
	pAssetDownloader->AddListener(this);//self must be added  at rearmost

	SetDownloader(pAssetDownloader);
	pAssetDownloader->Transfer();

	return true;
}

bool CCloudPhotoHandler::OnHandleInsert( void* pObj )
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

		SIZE size = CRenderEngine::GetImageSize(pParamer->GetHttpNotify()->strFilePath.c_str());
		if (size.cx == 0
			|| size.cy == 0)
		{
			size.cx = -1;
			size.cy = -1;
		}

		InsertPictureByThread(
			pParamer->GetHttpNotify()->strFilePath.c_str(),
			pParamer->GetInsertPos().x,
			pParamer->GetInsertPos().y,
			size.cx,
			size.cy, 0,
			pParamer->GetSlideId(),
			pParamer->GetPlaceHolderId());

		Statistics::GetInstance()->Report(STAT_INSERT_PICTURE);

	} while (false);

	//clean user data
	if (pParamer->GetHttpNotify())
	{
		pParamer->GetHttpNotify()->pUserData = NULL;
	}
	delete pParamer;


	return true;
}

bool CCloudPhotoHandler::OnDeleteComplete( void* pObj )
{
	THttpNotify*		pHttpNotify	= (THttpNotify*)pObj;
	CNDCloudContentService* pService = (CNDCloudContentService*)pHttpNotify->pUserData;
	if (pService->IsSuccess())
	{
		CGroupExplorerUI::GetInstance()->AddDBankItemCount(DBankImage);
	}
	return __super::OnDeleteComplete(pObj);
}



