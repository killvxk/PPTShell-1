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

#include "DUI/CloudNdpXHandler.h"

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
#include "DUI/NdpDownloader.h"
#include "DUI/IButtonTag.h"
#include "DUI/ItemExplorer.h"

#include "ThirdParty/zip/ZipWrapper.h"
#include "CoursePlayUI.h"
#include "DUI/CoursePlayUI.h"
#include "NDCloud/NDCloudContentService.h"
#include "GroupExplorer.h"



CCloudNdpXHandler::CCloudNdpXHandler()
{

}


CCloudNdpXHandler::~CCloudNdpXHandler()
{

}

void CCloudNdpXHandler::ReadFrom( CStream* pStream )
{
	m_strTitle			= pStream->ReadString();
	m_strGuid			= pStream->ReadString();
	m_strUrl			= pStream->ReadString();
	m_strMD5			= pStream->ReadString();

	int nPreviewsCount	= pStream->ReadInt();
	for (int i = 0; i < nPreviewsCount ; ++i)
	{
		pStream->ReadString();
	}

	m_strJsonInfo = pStream->ReadString();


	InitHandlerId();
}

void CCloudNdpXHandler::WriteTo( CStream* pStream )
{

// 	pStream->WriteString(m_strTitle);
// 	pStream->WriteString(m_strGuid);
// 	pStream->WriteString(m_strDesc);
// 	pStream->WriteString(m_strQuestionName);
// 	pStream->WriteString(m_strPreviewUrl);
// 	pStream->WriteString(m_strXmlUrl);
// 	pStream->WriteString(m_strJsonInfo);

}	

void CCloudNdpXHandler::DoButtonClick(TNotifyUI*	pNotify)
{
	__super::DoButtonClick(pNotify);
	
	if (pNotify->wParam == eClickFor_Open)
	{
		if (GetDownloader())
		{
			//already had downloader
			return;
		}

		//delegate
		CInvokeParamer* pParamer = new CInvokeParamer();
		pParamer->SetTrigger(GetTrigger());
		pParamer->SetCompletedDelegate(MakeDelegate(this, &CCloudNdpXHandler::OnHandleOpen));

		//don't delete
		CAssetDownloader* pAssetDownloader = new CAssetDownloader();
		pAssetDownloader->SetAssetGuid(GetGuid());
		pAssetDownloader->SetAssetTitle(GetTitle());
		pAssetDownloader->SetAssetType(CloudFileNdpCourse);
		pAssetDownloader->SetAssetUrl(GetResource());
		pAssetDownloader->SetAssetMD5(GetMD5());
		pAssetDownloader->SetParamer(pParamer);
		pAssetDownloader->AddListener(&GetDownloadListeners());
		pAssetDownloader->AddListener(this);//self must be added  at rearmost

		SetDownloader(pAssetDownloader);
		pAssetDownloader->Transfer();
	}
}

void CCloudNdpXHandler::DoSetThumbnail( TNotifyUI* pNotify )
{
	
}


void CCloudNdpXHandler::DoDropDown( TNotifyUI* pNotify )
{

}

bool CCloudNdpXHandler::OnHandleOpen( void* pObj )
{
	CInvokeParamer* pParamer = (CInvokeParamer*)pObj;
	//insert

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

		// open 
		if( pParamer->GetHttpNotify()->strFilePath != _T("") )
		{
			COPYDATASTRUCT copyData = {0};
			copyData.dwData = MSG_OPEN_FILE;
			copyData.cbData = pParamer->GetHttpNotify()->strFilePath.length() + 1;
			copyData.lpData = (LPVOID)pParamer->GetHttpNotify()->strFilePath.c_str();

			SendMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), WM_COPYDATA, MSG_OPEN_FILE, (LPARAM)&copyData);
		}

	} while (false);

	//clean user data
	if (pParamer->GetHttpNotify())
	{
		pParamer->GetHttpNotify()->pUserData = NULL;
	}
	delete pParamer;


	return true;
}

bool CCloudNdpXHandler::OnDeleteComplete( void* pObj )
{
	THttpNotify*		pHttpNotify	= (THttpNotify*)pObj;
	CNDCloudContentService* pService = (CNDCloudContentService*)pHttpNotify->pUserData;
	if (pService->IsSuccess())
	{
		CGroupExplorerUI::GetInstance()->AddDBankItemCount(DBankCourse);
		if(CGroupExplorerUI::GetInstance()->GetCurrentType()==DBankCourse)
		{
			int count = CGroupExplorerUI::GetInstance()->GetTypeCount();
			CGroupExplorerUI::GetInstance()->SetTypeCount(--count);
			if(count<=0)
			{
				CGroupExplorerUI::GetInstance()->ShowReslessUI(true);
			}
		}
	}
	return __super::OnDeleteComplete(pObj);
}
