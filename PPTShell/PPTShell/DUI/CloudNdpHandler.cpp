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

#include "DUI/CloudNdpHandler.h"

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

ImplementHandlerId(CCloudNdpHandler);
CCloudNdpHandler::CCloudNdpHandler()
{

}


CCloudNdpHandler::~CCloudNdpHandler()
{

}

void CCloudNdpHandler::ReadFrom( CStream* pStream )
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

void CCloudNdpHandler::WriteTo( CStream* pStream )
{

// 	pStream->WriteString(m_strTitle);
// 	pStream->WriteString(m_strGuid);
// 	pStream->WriteString(m_strDesc);
// 	pStream->WriteString(m_strQuestionName);
// 	pStream->WriteString(m_strPreviewUrl);
// 	pStream->WriteString(m_strXmlUrl);
// 	pStream->WriteString(m_strJsonInfo);

}	

void CCloudNdpHandler::DoButtonClick(TNotifyUI*	pNotify)
{
	__super::DoButtonClick(pNotify);
	
	if (pNotify->wParam == eClickFor_Play)
	{
		if (GetDownloader())
		{
			//already had downloader
			return;
		}

		CInvokeParamer* pParamer	= new CInvokeParamer();
		pParamer->SetTrigger(GetTrigger());
		pParamer->SetCompletedDelegate(MakeDelegate(this, &CCloudNdpHandler::OnHandlePlay));

		//don't delete
		CNdpDownloader* pNdpDownloader = new CNdpDownloader();
		pNdpDownloader->SetNdpGuid(GetGuid());
		pNdpDownloader->SetNdpTitle(GetTitle());
		pNdpDownloader->SetParamer(pParamer);
		pNdpDownloader->AddListener(&GetDownloadListeners());
		pNdpDownloader->AddListener(this);//self must be added  at rearmost

		SetDownloader(pNdpDownloader);
		pNdpDownloader->Transfer();
	}
}

void CCloudNdpHandler::DoSetThumbnail( TNotifyUI* pNotify )
{
	
}


void CCloudNdpHandler::DoDropDown( TNotifyUI* pNotify )
{

}

bool CCloudNdpHandler::OnHandlePlay( void* pObj )
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

		tstring strFolder = pParamer->GetHttpNotify()->strFilePath;
		int pos = strFolder.rfind('.');
		if( pos != -1 )
			strFolder = strFolder.substr(0, pos);

		CUnZipper UnZipper;
		UnZipper.UnZip(Str2Ansi(pParamer->GetHttpNotify()->strFilePath).c_str(), Str2Ansi(strFolder).c_str());

		//用播放器打开
		tstring strXml = strFolder;
		strXml += _T("\\main.xml");
		if(GetFileAttributes(strXml.c_str()) != INVALID_FILE_ATTRIBUTES)
		{
			tstring strPlayerPath	=GetHtmlPlayerPath();
			tstring strExePath		=GetCoursePlayerExePath();
			tstring strWorkDirectory= GetCoursePlayerPath();


			PROCESS_INFORMATION pi;
			STARTUPINFO si;    
			ZeroMemory( &pi, sizeof(pi) );
			ZeroMemory( &si, sizeof(si) );
			si.cb = sizeof(si);

			TCHAR szParam[MAX_PATH*2]={0};
			strXml = UrlEncode(Str2Utf8(strXml));
			strPlayerPath = UrlEncode(Str2Utf8(strPlayerPath));

			for(int i = 0; i < (int)strXml.length(); i++)
			{
				if( strXml.at(i) == _T('\\') )
					strXml.replace(i, 1, _T("/"));
			}

			_stprintf_s(szParam, _T("file:///%s?main=/%s&hidePage=toolbar"), strPlayerPath.c_str(),  strXml.c_str());

			CCoursePlayUI * pCoursePlayUI = CoursePlayUI::GetInstance();
			pCoursePlayUI->Init((WCHAR *)Str2Unicode(szParam).c_str(), COURSEPLAY_PLAYER);

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

bool CCloudNdpHandler::OnDeleteComplete( void* pObj )
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
