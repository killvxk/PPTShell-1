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

#include "DUI/Cloud3DResourceHandler.h"

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

#include "DUI/Toast.h"


CCloud3DResourceHandler::CCloud3DResourceHandler()
{
	m_bHasPreviewUrl = true;
}


CCloud3DResourceHandler::~CCloud3DResourceHandler()
{

}

void CCloud3DResourceHandler::ReadFrom( CStream* pStream )
{
	m_strGuid		= pStream->ReadString();
	m_strTitle		= pStream->ReadString();
	m_strUrl		= pStream->ReadString();
	if (m_bHasPreviewUrl)
	{
		m_strPreviewUrl	= pStream->ReadString();
	}
	m_strJsonInfo	= pStream->ReadString();
	InitHandlerId();
}

void CCloud3DResourceHandler::WriteTo( CStream* pStream )
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


void CCloud3DResourceHandler::DoButtonClick(TNotifyUI*	pNotify)
{
	__super::DoButtonClick(pNotify);
	
	if (pNotify->wParam == eClickFor_Insert)
	{
		//insert
		DoDropDown(pNotify);
	}
	else if (pNotify->wParam == eClickFor_Play)
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
		pParamer->SetCompletedDelegate(MakeDelegate(this, &CCloud3DResourceHandler::OnHandlePreivew));
		
		//don't delete
		CAssetDownloader* pAssetDownloader = new CAssetDownloader();
		pAssetDownloader->SetAssetGuid(GetGuid());
		pAssetDownloader->SetAssetTitle(GetTitle());
		pAssetDownloader->SetAssetType(CloudFile3DResource);
		pAssetDownloader->SetAssetUrl(GetResource());
		pAssetDownloader->SetParamer(pParamer);
		pAssetDownloader->AddListener(&GetDownloadListeners());
		pAssetDownloader->AddListener(this);//self must be added  at rearmost

		SetDownloader(pAssetDownloader);
		pAssetDownloader->Transfer();

		
	}

}

void CCloud3DResourceHandler::DoSetThumbnail( TNotifyUI* pNotify )
{
	if (!m_bHasPreviewUrl)
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
	pParamer->SetCompletedDelegate(MakeDelegate(this, &CCloud3DResourceHandler::OnHandleThumbnail));

	//don't delete
	CAssetDownloader* pAssetDownloader = new CAssetDownloader();
	SetThumbnailDownloader(pAssetDownloader);

	pAssetDownloader->SetThumbnailSize(240);
	pAssetDownloader->SetAssetGuid(GetGuid());
	pAssetDownloader->SetAssetTitle(GetTitle());
	pAssetDownloader->SetAssetType(CloudFileImage);
	pAssetDownloader->SetAssetUrl(m_strPreviewUrl.c_str());
	pAssetDownloader->SetParamer(pParamer);
	pAssetDownloader->AddListener(this);//self must be added  at rearmost
	pAssetDownloader->Transfer();
}

void CCloud3DResourceHandler::DoDropDown( TNotifyUI* pNotify )
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

bool CCloud3DResourceHandler::OnGetPlaceHolderCompleted( void* pObj )
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
	pParamer->SetCompletedDelegate(MakeDelegate(this, &CCloud3DResourceHandler::OnHandleInsert));
	

	//don't delete
	CAssetDownloader* pAssetDownloader = new CAssetDownloader();
	pAssetDownloader->SetAssetGuid(GetGuid());
	pAssetDownloader->SetAssetTitle(GetTitle());
	pAssetDownloader->SetAssetType(CloudFile3DResource);
	pAssetDownloader->SetAssetUrl(GetResource());
	pAssetDownloader->SetParamer(pParamer);
	pAssetDownloader->AddListener(&GetDownloadListeners());
	pAssetDownloader->AddListener(this);//self must be added  at rearmost

	SetDownloader(pAssetDownloader);
	pAssetDownloader->Transfer();

	return true;
}

bool CCloud3DResourceHandler::OnHandleInsert( void* pObj )
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

		tstring strFolder = pParamer->GetHttpNotify()->strFilePath;
		int pos = strFolder.rfind('.');
		if( pos != -1 )
			strFolder = strFolder.substr(0, pos);

		tstring strAppPath		= strFolder;
		tstring strSearchPath	= strFolder;
		strSearchPath += _T("\\*.exe");
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = ::FindFirstFile(strSearchPath.c_str(), &FindFileData);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			CUnZipper UnZipper;
			UnZipper.UnZip(Str2Ansi(pParamer->GetHttpNotify()->strFilePath).c_str(), Str2Ansi(strFolder).c_str());

			hFind = ::FindFirstFile(strSearchPath.c_str(), &FindFileData);
		}

		if ( hFind != INVALID_HANDLE_VALUE )
		{
			strAppPath += _T("\\");
			strAppPath += FindFileData.cFileName;
			Insert3DByThread(strAppPath.c_str(), m_strThumbnailPath.c_str(),
				pParamer->GetSlideId(),
				pParamer->GetPlaceHolderId());

			Statistics::GetInstance()->Report(STAT_INSERT_3D);
		}
		

	} while (false);
	delete pParamer;
	return true;
}

bool CCloud3DResourceHandler::OnHandlePreivew( void* pObj )
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

		tstring strFolder = pParamer->GetHttpNotify()->strFilePath;
		int pos = strFolder.rfind('.');
		if( pos != -1 )
			strFolder = strFolder.substr(0, pos);

		tstring strAppPath		= strFolder;
		tstring strSearchPath	= strFolder;
		strSearchPath += _T("\\*.exe");
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = ::FindFirstFile(strSearchPath.c_str(), &FindFileData);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			CUnZipper UnZipper;
			UnZipper.UnZip(Str2Ansi(pParamer->GetHttpNotify()->strFilePath).c_str(), Str2Ansi(strFolder).c_str());

			hFind = ::FindFirstFile(strSearchPath.c_str(), &FindFileData);
		}

		if ( hFind != INVALID_HANDLE_VALUE )
		{
			strAppPath += _T("\\");
			strAppPath += FindFileData.cFileName;

			ShellExecute(NULL, "open", strAppPath.c_str(), _T(""), strFolder.c_str(), SW_SHOWNORMAL);

			Statistics::GetInstance()->Report(STAT_INSERT_3D);
		}

	} while (false);

	delete pParamer;
	return true;
}

bool CCloud3DResourceHandler::OnHandleThumbnail( void* pObj )
{
	CInvokeParamer* pParamer = (CInvokeParamer*)pObj;

	if (pParamer->GetHttpNotify())
	{
		m_strThumbnailPath = pParamer->GetHttpNotify()->strFilePath;
	}
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

void CCloud3DResourceHandler::SetHasPreviewUrl( bool bHas )
{
	m_bHasPreviewUrl = bHas;
}



