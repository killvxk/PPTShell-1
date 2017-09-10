#include "StdAfx.h"
#include "DUI/IItemHandler.h"
#include "DUI/INotifyHandler.h"
#include "NDCloud/NDCloudAPI.h"
#include "NDCloud/NDCloudUser.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IDownloadListener.h"
#include "Util/Stream.h"
#include "DUI/ItemHandler.h"
#include "DUI/CloudResourceHandler.h"
#include "DUI/IThumbnailListener.h"

#include "DUI/CloudVRResourceHandler.h"

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
#include "DUI/VRDownloader.h"
#include "DUI/Toast.h"



ImplementHandlerId(CCloudVRResourceHandler);
CCloudVRResourceHandler::CCloudVRResourceHandler()
{
	m_bHasPreviewUrl	= true;
	m_pIntroduceStream	= NULL;
	m_dwVRSize			= 0;
}


CCloudVRResourceHandler::~CCloudVRResourceHandler()
{
	if (m_pIntroduceStream)
	{
		delete m_pIntroduceStream;
		m_pIntroduceStream = NULL;
	}
}

void CCloudVRResourceHandler::ReadFrom( CStream* pStream )
{
	m_strGuid		= pStream->ReadString();
	m_strTitle		= pStream->ReadString();
	m_strDesc		= pStream->ReadString();
	m_strUrl		= pStream->ReadString();

	if (m_bHasPreviewUrl)
	{
		m_strPreviewUrl	= pStream->ReadString();
	}

	m_strApkPackageName		= pStream->ReadString(); 
	m_strApkActivityName	= pStream->ReadString(); 
	m_strApkVersion			= pStream->ReadString();
	m_dwVRSize				= pStream->ReadDWORD();

	
	if (m_pIntroduceStream == NULL)
	{
		m_pIntroduceStream = new CStream(1024);
	}

	int nIntroducesCount	= pStream->ReadInt();
	m_pIntroduceStream->WriteInt(nIntroducesCount);
	for (int i = 0; i < nIntroducesCount; ++i)
	{
		m_pIntroduceStream->WriteString(pStream->ReadString());
	}

	//m_strJsonInfo	= pStream->ReadString();
	InitHandlerId();
}

void CCloudVRResourceHandler::WriteTo( CStream* pStream )
{
	pStream->WriteString(m_strGuid);
	pStream->WriteString(m_strTitle);
	pStream->WriteString(m_strDesc);
	pStream->WriteString(m_strUrl);
	if (m_bHasPreviewUrl)
	{
		pStream->WriteString(m_strPreviewUrl);
	}

	pStream->WriteString(m_strApkPackageName);
	pStream->WriteString(m_strApkActivityName);
	//pStream->WriteString(m_strJsonInfo);
}


void CCloudVRResourceHandler::DoButtonClick(TNotifyUI*	pNotify)
{
	__super::DoButtonClick(pNotify);

	//check file whether if it was exists
	if (pNotify->wParam == eClickFor_Insert)
	{
		if (!NDCloudFileManager::GetInstance()->IsFileDownloaded(GetResource(), &m_strResource))
		{
			pNotify->wParam = eClickFor_Start;
		}
	}

	if (pNotify->wParam == eClickFor_Insert)
	{
		InsertVR(m_strResource.c_str(),m_strTitle.c_str());
	}
	else if (pNotify->wParam == eClickFor_Pause)
	{
		if (GetDownloader())
		{
			GetDownloader()->Pause();
		}
	}
	else if (pNotify->wParam == eClickFor_Start)
	{
		if (GetDownloader())
		{
			GetDownloader()->Resume();
		}
		else
		{
			CMainFrame*		pMainFrame	= (CMainFrame*)AfxGetMainWnd();
			CInsertParamer* pParamer	= new CInsertParamer();
			pParamer->SetTrigger(GetTrigger());
			pParamer->SetCompletedDelegate(MakeDelegate(this, &CCloudVRResourceHandler::OnHandleInsert));

			//don't delete
			CVRDownloader* pVRDownloader = new CVRDownloader();
			pVRDownloader->SetAssetGuid(GetGuid());
			pVRDownloader->SetAssetTitle(GetTitle());
			pVRDownloader->SetAssetType(CloudFileVRResource);
			pVRDownloader->SetAssetUrl(GetResource());
			pVRDownloader->SetAssetMD5(GetMD5());
			pVRDownloader->SetParamer(pParamer);
			pVRDownloader->AddListener(&GetDownloadListeners());
			pVRDownloader->AddListener(this);//self must be added  at rearmost

			SetDownloader(pVRDownloader);
			pVRDownloader->Transfer();
		}
	}
	else if (pNotify->wParam == eClickFor_Introduce)
	{
		//Introduce preview

		if (m_pIntroduceStream == NULL)
		{
			return;
		}
		m_pIntroduceStream->ResetCursor();
		int nCount = m_pIntroduceStream->ReadInt();
		if (nCount <= 0)
		{
			CToast::Toast(_T("暂无简介"));
			return;
		}

		BOOL bExists = NDCloudFileManager::GetInstance()->IsFileDownloaded(GetResource(), NULL);
		
		CStream stream(1024);
		//index
		stream.WriteDWORD(0);
		//handler
		stream.WriteDWORD((DWORD)this);
		//decription
		stream.WriteString(m_strDesc);
		stream.WriteDWORD(m_dwVRSize);
		stream.WriteBOOL(bExists);
		stream.WriteDWORD(nCount);

		for (int i = 0; i < nCount; ++i)
		{
			stream.WriteString(GetTitle());
			stream.WriteString(GetGuid());
			stream.WriteString(GetResource());
			stream.WriteString(m_pIntroduceStream->ReadString());
		}	
		stream.ResetCursor();

		
		CRect rect;
		HWND hwnd = AfxGetMainWnd()->GetSafeHwnd();
		::GetWindowRect(hwnd, &rect);
		CPreviewDlgUI * pPreviewDlg = new CPreviewDlgUI();
		pPreviewDlg->Create(hwnd, _T("PreviewWindow"), WS_POPUP , 0, 0, 0, 0, 0);
		pPreviewDlg->Init(rect);
		pPreviewDlg->ShowPreviewWindow(&stream, E_WND_VR);
		pPreviewDlg->ShowModal();

		if (GetHolderIndex(GetTrigger()) != -1)
		{
			GetTrigger()->SetFocus();
		}
		
// 		//preview
// 
// 		//delegate
// 		CInvokeParamer* pParamer = new CInvokeParamer();
// 		pParamer->SetTrigger(GetTrigger());
// 		pParamer->SetCompletedDelegate(MakeDelegate(this, &CCloudVRResourceHandler::OnHandlePreivew));
// 
// 		//don't delete
// 		CAssetDownloader* pAssetDownloader = new CAssetDownloader();
// 		SetDownloader(pAssetDownloader);
// 
// 		pAssetDownloader->SetAssetGuid(GetGuid());
// 		pAssetDownloader->SetAssetTitle(GetTitle());
// 		pAssetDownloader->SetAssetType(CloudFileVRResource);
// 		pAssetDownloader->SetAssetUrl(GetResource());
// 		pAssetDownloader->SetUserData(pParamer);
// 		pAssetDownloader->AddListener(&GetDownloadListeners());
// 		pAssetDownloader->AddListener(this);//self must be added  at rearmost
// 		pAssetDownloader->Transfer();


	}

}

void CCloudVRResourceHandler::DoSetThumbnail( TNotifyUI* pNotify )
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
	pParamer->SetCompletedDelegate(MakeDelegate(this, &CCloudVRResourceHandler::OnHandleThumbnail));

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

void CCloudVRResourceHandler::DoDropDown( TNotifyUI* pNotify )
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

bool CCloudVRResourceHandler::OnGetPlaceHolderCompleted( void* pObj )
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
	
	return true;
}

bool CCloudVRResourceHandler::OnHandleInsert( void* pObj )
{
	CInsertParamer* pParamer = (CInsertParamer*)pObj;
	//insert

	do 
	{
		if (!pParamer->GetHttpNotify()
			|| pParamer->GetHttpNotify()->dwErrorCode != 0)
		{
			//RemovePalceHolderByThread(pParamer->GetSlideId(), pParamer->GetPlaceHolderId());
			if (pParamer->GetHttpNotify()->dwErrorCode == Error_Access_Denied)
			{
				CToast::Toast(_STR_NO_PERMISSION);
			}
			else if (pParamer->GetHttpNotify()->dwErrorCode == Error_Not_LoggedIn)
			{
				CToast::Toast(_T("请先登录"));
			}
			else
			{
				CToast::Toast(_STR_FILE_DWONLOAD_FAILED);
			}
			break;
		}

		m_strResource = pParamer->GetHttpNotify()->strFilePath.c_str();
		//InsertVR(pParamer->GetHttpNotify()->strFilePath.c_str());

	} while (false);
	delete pParamer;
	return true;
}

bool CCloudVRResourceHandler::InsertVR( LPCTSTR lpVr,LPCTSTR nTitle)
{
	tstring strFolder = lpVr;
	int pos = strFolder.rfind('.');
	if( pos != -1 )
		strFolder = strFolder.substr(0, pos);

	PPTVRType nVRtype = VRTYPE_OTHER;
	tstring strAppPath			= strFolder;

	tstring strexeSearchPath	= strFolder;
	strexeSearchPath += _T("\\*.exe");

	tstring strMp4SearchPath	= strFolder;
	strMp4SearchPath += _T("\\*.mp4");

	tstring strApkSearchPath = strFolder;
	strApkSearchPath += _T("\\*.apk");

	WIN32_FIND_DATA Mp4FindFileData;
	WIN32_FIND_DATA ExeFindFileData;
	WIN32_FIND_DATA ApkFindFileData;

	HANDLE hFind = ::FindFirstFile(strMp4SearchPath.c_str(), &Mp4FindFileData);
	HANDLE hFindexe = ::FindFirstFile(strexeSearchPath.c_str(), &ExeFindFileData);
	HANDLE hFind2 = ::FindFirstFile(strApkSearchPath.c_str(), &ApkFindFileData);

	bool nFind = false;
	if (hFind == INVALID_HANDLE_VALUE || hFind2 == INVALID_HANDLE_VALUE||hFindexe==INVALID_HANDLE_VALUE)
	{
		if (hFind != INVALID_HANDLE_VALUE)
		{
			nFind = true;
			nVRtype = VRTYPE_VIDEO;
			FindClose(hFind);
		}
		if (hFind2 != INVALID_HANDLE_VALUE)
		{
			nFind = true;
			FindClose(hFind2);
		}
		if (hFindexe != INVALID_HANDLE_VALUE)
		{
			nFind = true;
			nVRtype = VRTYPE_EXE;
			FindClose(hFindexe);
		}
		if (!nFind){
			CUnZipper UnZipper;
			UnZipper.UnZip(Str2Ansi(lpVr).c_str(), Str2Ansi(strFolder).c_str());
		}
	}

	hFind = ::FindFirstFile(strMp4SearchPath.c_str(), &Mp4FindFileData);
	hFindexe = ::FindFirstFile(strexeSearchPath.c_str(), &ExeFindFileData);
	hFind2 = ::FindFirstFile(strApkSearchPath.c_str(), &ApkFindFileData);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		nVRtype = VRTYPE_VIDEO;
	}
	if (hFindexe != INVALID_HANDLE_VALUE)
	{
		nVRtype = VRTYPE_EXE;
	}

	if ( nVRtype != VRTYPE_OTHER)
	{
		strAppPath += _T("\\");
		
		tstring nVrTypestr =  _T("");
		tstring nParam = _T("");
		if (nVRtype==VRTYPE_VIDEO){
			strAppPath += Mp4FindFileData.cFileName;
			nParam = strAppPath;
			nVrTypestr =  _T("VRTYPE_VIDEO");
		}else{
			strAppPath += ExeFindFileData.cFileName;
			nParam = strAppPath;
			nVrTypestr =   _T("VRTYPE_EXE");
		}
		nParam += _T("|");
		nParam += m_strApkPackageName;
		nParam += _T("|");
		nParam += m_strApkActivityName;
		nParam += _T("|");
		nParam += m_strApkVersion;
		nParam += _T("|");
		nParam += nVrTypestr;
		nParam += _T("|");
		nParam += nTitle;

		Statistics::GetInstance()->Report(STAT_INSERT_VR);
		return InsertVRByThread(nVRtype,strAppPath.c_str(),nParam.c_str(), m_strThumbnailPath.c_str());
	}
	else
	{
		DeleteFile(lpVr);
	}
	if (hFind != INVALID_HANDLE_VALUE)
	{
		FindClose(hFind);
	}
	if (hFind2 != INVALID_HANDLE_VALUE)
	{
		FindClose(hFind2);
	}
	if (hFindexe != INVALID_HANDLE_VALUE)
	{
		FindClose(hFindexe);
	}
	return false;
}

bool CCloudVRResourceHandler::OnHandlePreivew( void* pObj )
{
	CInvokeParamer* pParamer = (CInvokeParamer*)pObj;

	do 
	{
		if (!pParamer->GetHttpNotify()
			|| pParamer->GetHttpNotify()->dwErrorCode != 0)
		{
			if (pParamer->GetHttpNotify()->dwErrorCode == Error_Access_Denied)
			{
				CToast::Toast(_STR_NO_PERMISSION);
			}
			else if (pParamer->GetHttpNotify()->dwErrorCode == Error_Not_LoggedIn)
			{
				CToast::Toast(_T("请先登录"));
			}
			else
			{
				CToast::Toast(_STR_FILE_DWONLOAD_FAILED);
			}
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

			ShellExecute(NULL, _T("open"), strAppPath.c_str(), _T(""), strFolder.c_str(), SW_SHOWNORMAL);

			Statistics::GetInstance()->Report(STAT_INSERT_3D);
		}

	} while (false);

	delete pParamer;
	return true;
}

bool CCloudVRResourceHandler::OnHandleThumbnail( void* pObj )
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

bool CCloudVRResourceHandler::OnHandlePermission(void* pObj)
{
	THttpNotify* pNotify = (THttpNotify*)pObj;
	TNotifyUI* pUINotify = (TNotifyUI*)pNotify->pUserData;

	// check permission
	pNotify->pData[pNotify->nDataSize] = '\0';
	if( 0 == strcmpi(pNotify->pData, "0") )
	{
		//insert
		DoDropDown(pUINotify);
	}
	else
	{
		UIMessageBox(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), _STR_NO_PERMISSION, _T("提示"), _T("确定"), CMessageBoxUI::enMessageBoxTypeInfo);
	}

	return true;
}

void CCloudVRResourceHandler::SetHasPreviewUrl( bool bHas )
{
	m_bHasPreviewUrl = bHas;
}

