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

#include "DUI/CloudQuestionHandler.h"

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
#include "DUI/QuestionDownloader.h"
#include "DUI/IButtonTag.h"
#include "DUI/ItemExplorer.h"

#include "DUI/CoursePlayUI.h"
#include "NDCloud/NDCloudQuestion.h"

#include "DUI/ItemMenu.h"
#include "QuestionPreviewDialogUI.h"
#include "NDCloud/NDCloudContentService.h"
#include "GroupExplorer.h"

ImplementHandlerId(CCloudQuestionHandler);
CCloudQuestionHandler::CCloudQuestionHandler()
{
	isDeleting = false;
}


CCloudQuestionHandler::~CCloudQuestionHandler()
{

}

tstring CCloudQuestionHandler::GetQuestionName()
{
	return m_strQuestionName;
}

void CCloudQuestionHandler::ReadFrom( CStream* pStream )
{
	m_strTitle			= pStream->ReadString();
	m_strGuid			= pStream->ReadString();
	m_strDesc			= pStream->ReadString();
	m_strQuestionName	= pStream->ReadString();
	m_strPreviewUrl		= pStream->ReadString();
	m_strXmlUrl			= pStream->ReadString();
	m_strJsonInfo		= pStream->ReadString();
	InitHandlerId();
}

void CCloudQuestionHandler::WriteTo( CStream* pStream )
{
	pStream->WriteString(m_strTitle);
	pStream->WriteString(m_strGuid);
	pStream->WriteString(m_strDesc);
	pStream->WriteString(m_strQuestionName);
	pStream->WriteString(m_strPreviewUrl);
	pStream->WriteString(m_strXmlUrl);
	pStream->WriteString(m_strJsonInfo);

}	

void CCloudQuestionHandler::DoRClick( TNotifyUI* pNotify )
{
	CItemHandler::DoRClick(pNotify);

	if (IsDbank()&&!isDeleting)
	{
		CItemMenuUI* pMenu = new CItemMenuUI();
		pMenu->SetHolder(pNotify->pSender);
		pMenu->CreateMenu();
		pMenu->AddMenuItem(_T("删除"), eMenu_Delete);
		pMenu->SetLeftWeight(0.4f);
		pMenu->ShowMenu();

	}
}

void CCloudQuestionHandler::DoButtonClick(TNotifyUI*	pNotify)
{
	__super::DoButtonClick(pNotify);
	
	if (pNotify->wParam == eClickFor_Insert)
	{
		//insert
		DoDropDown(pNotify);
	}
	else if (pNotify->wParam == eClickFor_Preview)
	{
		if (GetDownloader())
		{
			//already had downloader
			return;
		}

		CInvokeParamer* pParamer	= new CInvokeParamer();
		pParamer->SetTrigger(GetTrigger());
		pParamer->SetCompletedDelegate(MakeDelegate(this, &CCloudQuestionHandler::OnHandlePreivew));

		//don't delete
		CQuestionDownloader* pQuestionDownloader = new CQuestionDownloader();
		pQuestionDownloader->SetQuestionGuid(GetGuid());
		pQuestionDownloader->SetQuestionTitle(GetTitle());
		pQuestionDownloader->SetParamer(pParamer);
		pQuestionDownloader->AddListener(&GetDownloadListeners());
		pQuestionDownloader->AddListener(this);//self must be added  at rearmost

		SetDownloader(pQuestionDownloader);
		pQuestionDownloader->Transfer();
	}
}

void CCloudQuestionHandler::DoSetThumbnail( TNotifyUI* pNotify )
{
	if (m_strPreviewUrl.empty())
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
	pParamer->SetCompletedDelegate(MakeDelegate(this, &CCloudQuestionHandler::OnHandleThumbnail));

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

bool CCloudQuestionHandler::OnHandleThumbnail( void* pObj )
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

void CCloudQuestionHandler::DoDropDown( TNotifyUI* pNotify )
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

bool CCloudQuestionHandler::OnGetPlaceHolderCompleted( void* pObj )
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
	pParamer->SetCompletedDelegate(MakeDelegate(this, &CCloudQuestionHandler::OnHandleInsert));
	

	//don't delete
	CQuestionDownloader* pQuestionDownloader = new CQuestionDownloader();
	pQuestionDownloader->SetQuestionGuid(GetGuid());
	pQuestionDownloader->SetQuestionTitle(GetTitle());
	pQuestionDownloader->SetParamer(pParamer);
	pQuestionDownloader->AddListener(&GetDownloadListeners());
	pQuestionDownloader->AddListener(this);//self must be added  at rearmost

	SetDownloader(pQuestionDownloader);
	pQuestionDownloader->Transfer();


	return true;
}

bool CCloudQuestionHandler::OnHandleInsert( void* pObj )
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

		tstring strMainXmlPath = pParamer->GetHttpNotify()->strFilePath;
		strMainXmlPath += _T("\\main.xml");

		// add reportable = true attribute to main.xml
		TiXmlDocument doc;
		bool res = doc.LoadFile(Str2Ansi(strMainXmlPath).c_str());
		if( !res )
			break;

		TiXmlElement* pRootElement = doc.FirstChildElement();
		TiXmlElement* pPagesElement = GetElementsByTagName(pRootElement, "pages");
		if( pPagesElement == NULL )
			break;

		TiXmlElement* pPageElement = pPagesElement->FirstChildElement();
		while( pPageElement != NULL )
		{
			TCHAR szHref[MAX_PATH];
			_stprintf_s(szHref, _T("pages/%s.xml"), GetGuid());

			pPageElement->SetAttribute("id", Str2Ansi(GetGuid()).c_str());
			pPageElement->SetAttribute("name", Str2Ansi(GetGuid()).c_str());
			pPageElement->SetAttribute("reportable", "true");
			pPageElement->SetAttribute("href", Str2Ansi(szHref).c_str());

			pPageElement = pPageElement->NextSiblingElement();
			
		}

		doc.SaveFile(Str2Ansi(strMainXmlPath).c_str()); 

		// rename file
		tstring strPageFilePath = strMainXmlPath;
		int pos = strPageFilePath.rfind('\\');
		if( pos != -1 )
			strPageFilePath = strPageFilePath.substr(0, pos);

		tstring strSearchName = strPageFilePath + _T("\\pages\\*.xml");


		tstring strOldName;
		tstring strNewName;

		WIN32_FIND_DATA FindData;
		HANDLE hFind = FindFirstFile(strSearchName.c_str(), &FindData);
		if( hFind != NULL )
		{
			strOldName = strPageFilePath + _T("\\pages\\");
			strOldName += FindData.cFileName;

			strNewName = strPageFilePath + _T("\\pages\\");
			strNewName += GetGuid();
			strNewName += _T(".xml");


			MoveFile(strOldName.c_str(), strNewName.c_str());
		}
	
		// set addonModule.id = BasicQuestionViewer if addonModule.addonId = BasicQuestionViewer
		res = doc.LoadFile(Str2Ansi(strNewName).c_str());
		if( res )
		{
			TiXmlElement* pRootElement = doc.FirstChildElement();
			TiXmlElement* pAddonModuleElement = GetElementsByTagName(pRootElement, "addonModule");
			if( pAddonModuleElement == NULL )
				break;
		
			const char* pAddonId = pAddonModuleElement->Attribute("addonId");
			if( pAddonId != NULL && !strcmpi(pAddonId, "BasicQuestionViewer") )
				pAddonModuleElement->SetAttribute("id", "BasicQuestionViewer");

			doc.SaveFile(Str2Ansi(strNewName).c_str());
		}


		// get question type
		tstring strQuestionType = NDCloudQuestionManager::GetInstance()->GetQuestionResType(m_strQuestionName.c_str());

		res = NDCloudQuestionManager::GetInstance()->ConvertQuestionToCourseFile(GetGuid(), strQuestionType, m_strQuestionName, pParamer->GetHttpNotify()->strFilePath);
		if( !res )
		{
			break;
		}


		InsertQuestionByThread(strMainXmlPath.c_str(),
			GetTitle(),
			GetGuid(),
			pParamer->GetSlideId(),
			pParamer->GetPlaceHolderId());

		Statistics::GetInstance()->Report(STAT_INSERT_QUESTION);

	} while (false);

	//clean user data
	if (pParamer->GetHttpNotify())
	{
		pParamer->GetHttpNotify()->pUserData = NULL;
	}
	delete pParamer;


	return true;
}


bool CCloudQuestionHandler::OnHandlePreivew( void* pObj )
{
	CInvokeParamer* pParamer = (CInvokeParamer*)pObj;
	//insert
	if (pParamer->GetHttpNotify()&&pParamer->GetHttpNotify()->dwErrorCode == 0)
	{
		if (GetTrigger()&&GetTrigger() == pParamer->GetTrigger()&&GetTrigger()->IsVisible())
		{
			tstring strQuestionPath = pParamer->GetHttpNotify()->strFilePath;
			strQuestionPath += _T("\\main.xml");
			tstring strQuestionType = NDCloudQuestionManager::GetInstance()->GetQuestionResType(m_strQuestionName.c_str());
			BOOL res = NDCloudQuestionManager::GetInstance()->ConvertQuestionToCourseFile(GetGuid(), strQuestionType, m_strQuestionName, pParamer->GetHttpNotify()->strFilePath);
			if( res )
			{
				Statistics::GetInstance()->Report(STAT_PREVIEW_QUESTION);
				// translate slash					
				for(int i = 0; i < (int)strQuestionPath.length(); i++)
				{
					if( strQuestionPath.at(i) == _T('\\') )
						strQuestionPath.replace(i, 1, _T("/"));
				}
				// js player path
				tstring strLocalPath = GetLocalPath();
				// TCHAR szPlayerPath[MAX_PATH*2] = {0};
				tstring strPlayerPath=GetHtmlPlayerPath();
				TCHAR szParam[MAX_PATH*3]={0};
				tstring strAppDir = GetLocalPath();
				tstring ntitle = _T("预览");
				_stprintf_s(szParam, _T("%s?main=/%s&sys=pptshell&hidePage=footer %s"), 
					UrlEncode(Str2Utf8(strPlayerPath)).c_str(), 
					UrlEncode(Str2Utf8(strQuestionPath)).c_str(),
					ntitle.c_str());
				/*CCoursePlayUI * pCoursePlayUI = CoursePlayUI::GetInstance();
				pCoursePlayUI->SetQuestionInfo(strQuestionPath, m_strTitle, m_strGuid);
				pCoursePlayUI->Init((WCHAR *)AnsiToUnicode(szParam).c_str(),COURSEPLAY_PREVIEW);*/


				//获取其他item信息
				CStream stream(1024);
				tstring strUrl			= szParam;
				CContainerUI* pParaent	= (CContainerUI*)GetTrigger()->GetParent();
				int nCount				= pParaent->GetCount();
				int nIndex				= pParaent->GetItemIndex(GetTrigger());

				stream.WriteString(strUrl);
				stream.WriteDWORD(nIndex);
				stream.WriteDWORD(nCount);
				int nRealCount	= 0;
				int nRealIndex	= 0;
				for (int i = 0; i < nCount; ++i)
				{
					IHandlerVisitor* pHandlerVisitor = dynamic_cast<IHandlerVisitor*>(pParaent->GetItemAt(i));
					if (pHandlerVisitor)
					{
						CCloudQuestionHandler* pHandler = dynamic_cast<CCloudQuestionHandler*>(pHandlerVisitor->GetHandler());
						if (pHandler)
						{
							if (nIndex == i)
							{
								nRealIndex = nRealCount;
							}

							nRealCount++;
							pHandler->WriteTo(&stream);
						}
						
					}
				}	
				//update index and count
				stream.ResetCursor();		
				stream.ReadString();
				stream.WriteDWORD(nRealIndex);
				stream.WriteDWORD(nRealCount);
				stream.ResetCursor();

				//
				CQuestionPreviewDialogUI* pPreviewUI = QuestionPreviewDialogUI::GetInstance();
				pPreviewUI->SetQuestionType(QUESTION_CLOUD);
				pPreviewUI->InitPosition();
				pPreviewUI->InitData(&stream);
				pPreviewUI->ShowWindow();
			}
		}	
	}
	else
	{
		CToast::Toast(_STR_FILE_DWONLOAD_FAILED);
	}

	//clean user data
	if (pParamer->GetHttpNotify())
	{
		pParamer->GetHttpNotify()->pUserData = NULL;
	}
	delete pParamer;


	return true;
}

bool CCloudQuestionHandler::OnDeleteComplete( void* pObj )
{
	THttpNotify*		pHttpNotify	= (THttpNotify*)pObj;
	CNDCloudContentService* pService = (CNDCloudContentService*)pHttpNotify->pUserData;
	if (pService->IsSuccess())
	{
		CGroupExplorerUI::GetInstance()->AddDBankItemCount(DBankQuestion);
		if(CGroupExplorerUI::GetInstance()->GetCurrentType()==DBankQuestion)
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
