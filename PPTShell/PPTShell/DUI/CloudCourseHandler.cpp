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

#include "DUI/CloudCourseHandler.h"

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
#include "DUI/CourseDownloader.h"
#include "DUI/IButtonTag.h"
#include "DUI/ItemExplorer.h"
#include "NDCloud/NDCloudContentService.h"
#include "GroupExplorer.h"
#include "NDCloud/NDCloudFile.h"

ImplementHandlerId(CCloudCourseHandler);
CCloudCourseHandler::CCloudCourseHandler()
{
	m_pPreviewsStream = NULL;
}


CCloudCourseHandler::~CCloudCourseHandler()
{
	if (m_pPreviewsStream)
	{
		delete m_pPreviewsStream;
		m_pPreviewsStream = NULL;
	}
}

void CCloudCourseHandler::ReadFrom( CStream* pStream )
{
	m_strTitle		= pStream->ReadString();
	m_strGuid		= pStream->ReadString();
	m_strUrl		= pStream->ReadString();
	m_strMD5		= pStream->ReadString();

	if (m_pPreviewsStream)
	{
		delete m_pPreviewsStream;
		m_pPreviewsStream = NULL;
	}

	m_pPreviewsStream	= new CStream(1024);
	int nPreviewsCount	= pStream->ReadInt();

	m_pPreviewsStream->WriteDWORD(nPreviewsCount);
	if(nPreviewsCount)
	{
		m_strPreviewUrl		= pStream->ReadString();
		m_pPreviewsStream->WriteString(m_strPreviewUrl);

		for (int i = 0; i < nPreviewsCount - 1; ++i)
		{
			m_pPreviewsStream->WriteString(pStream->ReadString());
		}

	}

	m_strJsonInfo = pStream->ReadString();
	InitHandlerId();
}

void CCloudCourseHandler::WriteTo( CStream* pStream )
{

	pStream->WriteString(m_strTitle);
	pStream->WriteString(m_strGuid);
	pStream->WriteString(m_strUrl);

	if (m_pPreviewsStream)
	{
		
		int nPreviewsCount	= m_pPreviewsStream->ReadInt();

		pStream->WriteDWORD(nPreviewsCount);
		for (int i = 0; i < nPreviewsCount; ++i)
		{
			pStream->WriteString(m_pPreviewsStream->ReadString());
		}
	}

	pStream->WriteString(m_strJsonInfo);
}

void CCloudCourseHandler::DoClick( TNotifyUI* pNotify )
{
	__super::DoClick(pNotify);
	if (!m_pPreviewsStream)
	{
		return;
	}

	m_pPreviewsStream->ResetCursor();
	int nCount = m_pPreviewsStream->ReadInt();
	if (nCount <= 0)
	{
		return;
	}

	CItemExplorerUI::GetInstance()->ShowWindow(true);
	CItemExplorerUI::GetInstance()->ResetUI();


	CStream ItemsStream(1024);
	ItemsStream.WriteDWORD(nCount);
	for (int i = 0; i < nCount; ++i)
	{
		ItemsStream.WriteInt(i + 1);
		ItemsStream.WriteString(m_strTitle);
		ItemsStream.WriteString(m_strGuid);
		ItemsStream.WriteString(m_strUrl);
		ItemsStream.WriteString(m_pPreviewsStream->ReadString());
	}

	CItemExplorerUI::GetInstance()->ShowResource(eItemExplorer_Cloud, &ItemsStream, NULL);
	CItemExplorerUI::GetInstance()->StopMask();

}

void CCloudCourseHandler::DoButtonClick(TNotifyUI*	pNotify)
{
	__super::DoButtonClick(pNotify);
	
	if (pNotify->wParam == eClickFor_Apply)
	{
		//insert
		if(IsDbank())
		{
			if (GetDownloader())
			{
				return;
			}

			CInvokeParamer* pParamer = new CInvokeParamer();
			pParamer->SetTrigger(GetTrigger());
			pParamer->SetCompletedDelegate(MakeDelegate(this, &CCloudCourseHandler::OnHandleApply));

			//don't delete
			CCourseDownloader* pAssetDownloader = new CCourseDownloader();
			pAssetDownloader->SetAssetGuid(GetGuid());
			pAssetDownloader->SetAssetTitle(GetTitle());
			pAssetDownloader->SetAssetType(CourseFilePPT);
			pAssetDownloader->SetAssetUrl(GetResource());
			pAssetDownloader->SetAssetMD5(GetMD5());
			pAssetDownloader->SetParamer(pParamer);
			pAssetDownloader->AddListener(&GetDownloadListeners());
			pAssetDownloader->AddListener(this);//self must be added  at rearmost

			SetDownloader(pAssetDownloader);
			pAssetDownloader->Transfer();
		}
		else
			DoDropDown(pNotify);
	}
}

void CCloudCourseHandler::DoSetThumbnail( TNotifyUI* pNotify )
{
	IThumbnailListener* pListener = dynamic_cast<IThumbnailListener*>(pNotify->pSender);
	if (pListener)
	{
		pListener->OnGetThumbnailBefore();
	}

	//delegate
	CInvokeParamer* pParamer = new CInvokeParamer();
	pParamer->SetCompletedDelegate(MakeDelegate(this, &CCloudCourseHandler::OnHandleThumbnail));

	//don't delete
	CCourseDownloader* pCourseDownloader = new CCourseDownloader();
	SetThumbnailDownloader(pCourseDownloader);

	pCourseDownloader->SetAssetGuid(GetGuid());
	pCourseDownloader->SetAssetTitle(GetTitle());
	pCourseDownloader->SetAssetType(CourseFilePPT);
	pCourseDownloader->SetAssetUrl(m_strPreviewUrl.c_str());
	pCourseDownloader->SetAssetMD5(GetMD5());
	pCourseDownloader->SetParamer(pParamer);
	pCourseDownloader->AddListener(this);//self must be added  at rearmost
	pCourseDownloader->Transfer();

}

bool CCloudCourseHandler::OnHandleThumbnail( void* pObj )
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

void CCloudCourseHandler::DoDropDown( TNotifyUI* pNotify )
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

bool CCloudCourseHandler::OnGetPlaceHolderCompleted( void* pObj )
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
	pParamer->SetCompletedDelegate(MakeDelegate(this, &CCloudCourseHandler::OnHandleApply));
	

	//don't delete
	CCourseDownloader* pCourseDownloader = new CCourseDownloader();
	pCourseDownloader->SetThumbnailSize(240);
	pCourseDownloader->SetAssetGuid(GetGuid());
	pCourseDownloader->SetAssetTitle(GetTitle());
	pCourseDownloader->SetAssetType(CourseFilePPT);
	pCourseDownloader->SetAssetUrl(GetResource());
	pCourseDownloader->SetAssetMD5(GetMD5());
	pCourseDownloader->SetParamer(pParamer);
	pCourseDownloader->AddListener(&GetDownloadListeners());
	pCourseDownloader->AddListener(this);//self must be added  at rearmost

	SetDownloader(pCourseDownloader);
	pCourseDownloader->Transfer();


	return true;
}

bool CCloudCourseHandler::OnHandleApply( void* pObj )
{
	CInsertParamer* pParamer = (CInsertParamer*)pObj;
	//insert

	if(IsDbank())
	{
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

				TCHAR szDrive[8]			= {0};
				TCHAR szDir[MAX_PATH*2]		= {0};
				TCHAR szFileName[MAX_PATH*2]	= {0};
				TCHAR szExt[128]				= {0};
				if (!_tsplitpath_s(pParamer->GetHttpNotify()->strFilePath.c_str(), szDrive, szDir, szFileName, szExt))
				{
					tstring strNewFile	= szDrive;
					strNewFile			+= szDir;
					strNewFile			+= GetTitle();
					strNewFile			+= szExt;
					if(MoveFileEx(pParamer->GetHttpNotify()->strFilePath.c_str(), strNewFile.c_str(), MOVEFILE_COPY_ALLOWED|MOVEFILE_REPLACE_EXISTING))
					{
						pParamer->GetHttpNotify()->strFilePath = strNewFile;
						
						if (!_tsplitpath_s(NDCloudFileManager::GetInstance()->GetDownloadedFilePath(GetResource()).c_str(), 
																					szDrive, szDir, szFileName, szExt))
						{
							tstring strDownloadedFilePath	= szDir;
							strDownloadedFilePath			+= GetTitle();
							strDownloadedFilePath			+= szExt;
 							NDCloudFileManager::GetInstance()->ModifyDownloadedFilePath(GetResource(), strDownloadedFilePath);
						}
						
					}
				}

				COPYDATASTRUCT copyData = {0};
				copyData.dwData = MSG_OPEN_FILE;
				copyData.cbData = pParamer->GetHttpNotify()->strFilePath.length() + 1;
				copyData.lpData = (LPVOID)pParamer->GetHttpNotify()->strFilePath.c_str();

				SendMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), WM_COPYDATA, MSG_OPEN_FILE, (LPARAM)&copyData);
			}
		} while (false);
	}
	else
	{
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

			InsertPPT(pObj);

		} while (false);
	}
	

	//clean user data
	if (pParamer->GetHttpNotify())
	{
		pParamer->GetHttpNotify()->pUserData = NULL;
	}
	delete pParamer;


	return true;
}

void CCloudCourseHandler::InsertPPT( void* pObj )
{
	CInsertParamer* pParamer = (CInsertParamer*)pObj;
	InsertPPTByThread(
		pParamer->GetHttpNotify()->strFilePath.c_str(),
		1,
		-1,
		0,
		pParamer->GetSlideId(),
		pParamer->GetPlaceHolderId());

	Statistics::GetInstance()->Report(STAT_INSERT_PPT);
}

bool CCloudCourseHandler::OnDeleteComplete( void* pObj )
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
