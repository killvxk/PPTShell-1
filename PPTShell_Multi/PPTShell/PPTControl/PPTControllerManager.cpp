//===========================================================================
// FileName:				PPTControllerManager.h
// 
// Desc:					One thread one PPTController
//============================================================================
#include "stdafx.h"
#include "PPTControllerManager.h"
#include "Util/FileTypeFilter.h"
#include "DUI/Toast.h"
#include "Util/Util.h"
#include "NDCloud/NDCloudFile.h"
#include "Statistics/Statistics.h"
#include "GUI/MainFrm.h"

CPPTControllerManager* CPPTControllerManager::g_pPPTControllerManager = NULL;

CPPTControllerManager::CPPTControllerManager()
{
	g_pPPTControllerManager	= this;
	m_hOwnerWnd				= NULL;
	//m_lstPalceHolders.clear();
	m_mapPalceHolders.clear();
}

CPPTControllerManager::~CPPTControllerManager()
{
	if ( g_pPPTControllerManager != NULL )
	 g_pPPTControllerManager->Destroy();
}

BOOL CopyAndRegCef()
{
	BOOL nPPTx64 = CPPTController::IsPPT64Bit();
	tstring strAndCefPath =  GetCoursePlayerPath();
	tstring strAndCefName = _T("\\andcef.ocx");
	tstring strRegName = _T("regsvr32.exe ");
	if (nPPTx64){
		strAndCefPath = GetAndCefX64Path();

		tstring strSrc = GetCoursePlayerPath();
		tstring strDst = GetAndCefX64Path();
		tstring str[]={_T("\\cef.pak"),_T("\\cef_100_percent.pak"),_T("\\cef_200_percent.pak"),_T("\\icudtl.dat")};  
		std::vector<tstring> strArray(str, str+4); 
		tstring strlocales =  _T("\\locales");
		tstring strSrclocales = strSrc + strlocales;
		tstring strDstlocales = strDst + strlocales;

		CopyDir(strSrclocales.c_str(),strDstlocales.c_str(),true);

		for (size_t i =0;i<strArray.size();i++){
			tstring strSrcFile = strSrc + strArray[i];
			tstring strDstFile = strDst + strArray[i];
			CopyFile(strSrcFile.c_str(), strDstFile.c_str(), true);
		}
	}

	strAndCefPath += strAndCefName;
	strRegName = strRegName + _T("\"") +strAndCefPath + _T("\" /s");

	PROCESS_INFORMATION pi;
	STARTUPINFO si;      //隐藏进程窗口
	si.cb = sizeof(STARTUPINFO);
	si.lpReserved = NULL;
	si.lpDesktop = NULL;
	si.lpTitle = NULL;
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	si.cbReserved2 = NULL;
	si.lpReserved2 = NULL;
	BOOL ret = CreateProcess(NULL,(LPSTR)strRegName.c_str(),NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
	if(ret)
	{
		
	}
	return TRUE;
}

BOOL CPPTControllerManager::Initialize()
{
	InitializeCriticalSection(&m_Lock);
	InitializeCriticalSection(&m_ControlLock);

	m_lstPptOperation.clear();
	m_lstPptControl.clear();

	m_bRuning		= false;
	m_nCurrentPos	= 0;
	m_bCancelInsert	= false;

	for ( int i = 0; i < PPT_OPERATION_THREAD_MAX_COUNT; i++)
	{
		m_hEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
//		m_ThreadInsertCount[i] = 0;
		CStream* pStream = new CStream(1024);
		pStream->WriteWORD(i);
		pStream->WriteDWORD((DWORD)this);
		pStream->ResetCursor();
		m_hThread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PPTInsertThread, pStream, 0, NULL);
		
	}

	m_hControlEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	//m_ControlThreadInserCount = 0;
	m_hControlThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PPTControlThread, this, 0, NULL);

	string strPPTPath = GetPPTController()->GetPPTPath();
	CPPTController::SetPPT64Bit(CheckBinIs64(strPPTPath));

	::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CopyAndRegCef, this, 0, NULL);

	return TRUE;
}

BOOL CPPTControllerManager::Destroy()
{
	m_bRuning = false;
	for (int i = 0; i < PPT_OPERATION_THREAD_MAX_COUNT; i++)
	{
		
		if ( m_hThread[i] != NULL )
		{
			TerminateThread(m_hThread[i],0);
			CloseHandle(m_hThread[i]);
		}
		CloseHandle(m_hEvents[i]);
		//m_ThreadInsertCount[i] = 0;
	}
	
	if ( m_hControlThread != NULL )
	{
		TerminateThread(m_hControlThread,0);
		CloseHandle(m_hControlThread);
	}
	CloseHandle(m_hControlEvent);
	//m_ControlThreadInserCount = 0;


	list<CStream*>::iterator iter = m_lstPptOperation.begin();

	EnterCriticalSection(&m_Lock);
	for ( iter; iter != m_lstPptOperation.end(); iter++ )
	{
		delete &iter;
	}
	LeaveCriticalSection(&m_Lock);

	m_lstPptOperation.clear();

	iter = m_lstPptControl.begin();

	EnterCriticalSection(&m_ControlLock);
	for ( iter; iter != m_lstPptControl.end(); iter++ )
	{
		delete &iter;
	}
	LeaveCriticalSection(&m_ControlLock);

	m_lstPptControl.clear();

	PPTControllerDestory();
	
	DeleteCriticalSection(&m_Lock);
	DeleteCriticalSection(&m_ControlLock);

	g_pPPTControllerManager		= NULL;
	return TRUE;
}

//
// set owner
//
BOOL CPPTControllerManager::SetOwner(HWND hWnd)
{
	m_hOwnerWnd = hWnd;
	return TRUE;
}

CPPTController* CPPTControllerManager::GetPPTController()
{
	CPPTController* pController = NULL;
	EnterCriticalSection(&m_Lock);

	DWORD dwThreadId = GetCurrentThreadId();
	map<DWORD, CPPTController*>::iterator itr = m_mapControllers.find(dwThreadId);
	if( itr != m_mapControllers.end() )
	{
		pController = itr->second;
		if( pController->GetApplication() == NULL)
		{
			pController->initApp();
		}

		//if ( pController->IsNeedUpdatePresentation() )
		//	pController->Update();
	}
	else
	{
		pController = new CPPTController;
		m_mapControllers[dwThreadId] = pController;
	}

	LeaveCriticalSection(&m_Lock);

	return pController;
}


BOOL CPPTControllerManager::PPTControllerUpdate()
{

	EnterCriticalSection(&g_pPPTControllerManager->m_Lock);

	map<DWORD, CPPTController*>::iterator iter = g_pPPTControllerManager->m_mapControllers.begin();

	for ( iter; iter != g_pPPTControllerManager->m_mapControllers.end(); iter++)
	{
		iter->second->SetApplication(NULL);
		//iter->second->Update();
		//iter->second->SetUpdatePresentationStatus(TRUE);
	}

	LeaveCriticalSection(&g_pPPTControllerManager->m_Lock);

	return TRUE;
}

BOOL CPPTControllerManager::PPTControllerDestory()
{

	EnterCriticalSection(&g_pPPTControllerManager->m_Lock);

	map<DWORD, CPPTController*>::iterator iter = g_pPPTControllerManager->m_mapControllers.begin();

	for ( iter; iter != g_pPPTControllerManager->m_mapControllers.end(); iter++)
	{
		//test
		//delete iter->second;
		iter->second = NULL;
	}

	g_pPPTControllerManager->m_mapControllers.clear();

	LeaveCriticalSection(&g_pPPTControllerManager->m_Lock);

	return TRUE;
}

//
// insert PPT inside a new created thread
//
void CPPTControllerManager::AddPPTInsertOperation(CStream* pStream)
{
	EnterCriticalSection(&m_Lock);
	m_lstPptOperation.push_back(pStream);
	
	m_nCurrentPos = m_nCurrentPos % PPT_OPERATION_THREAD_MAX_COUNT;
	SetEvent(m_hEvents[m_nCurrentPos]);
//	m_ThreadInsertCount[m_nCurrentPos]++;
	m_nCurrentPos++;
	LeaveCriticalSection(&m_Lock);
}


DWORD WINAPI CPPTControllerManager::PPTInsertThread(LPARAM lParam)
{

	CStream* pMainStream = (CStream*)lParam;
	int nIndex = pMainStream->ReadWORD();
	CPPTControllerManager* pThis = (CPPTControllerManager*) pMainStream->ReadDWORD();

	delete pMainStream;

	pThis->m_bRuning = true;

	pThis->GetPPTController();

	while ( pThis->m_bRuning )
	{
		Sleep(1);
		WaitForSingleObject(pThis->m_hEvents[nIndex], INFINITE);

		CStream* pStream = NULL;

		EnterCriticalSection(&pThis->m_Lock);

		if ( pThis->m_bCancelInsert )
		{
			std::list<CStream*>::iterator iter = pThis->m_lstPptOperation.begin();
			for ( iter; iter != pThis->m_lstPptOperation.end(); iter)
			{
				pStream = *iter;
				if ( pStream != NULL)
					delete pStream;
				iter = pThis->m_lstPptOperation.erase(iter);
			}
			pThis->m_lstPptOperation.clear();

			pThis->RemoveAllPPTPalceHolder(pThis->GetPPTController());

			pStream = NULL;
		}

		if ( !pThis->m_lstPptOperation.empty() )
		{
			pStream	= pThis->m_lstPptOperation.front();
			pThis->m_lstPptOperation.pop_front();
		}

		if ( !pThis->m_lstPptOperation.empty() && !pThis->m_bCancelInsert )
			SetEvent(pThis->m_hEvents[nIndex]);

		LeaveCriticalSection(&pThis->m_Lock);

		if ( pStream == NULL)
			continue;

		CPPTController* pController = pThis->GetPPTController();
		if ( pController != NULL )
		{

			if ( pController->IsInitFailed() )
				CToast::Toast(_STR_PPTCTRLMGR_TIP_PPT_INIT_FAILED, true, 2000);
			else  if (!pController->IsPPTActive() || (!CMainFrame::IsMainFrmActive() && (pController->GetPresentationCount() == 0)) )			
				CToast::Toast(_STR_PPTCTRLMGR_TIP_MODIFY_PPT_EDITING, true, 1000); 
			else if ( !pController->IsInit() || (pController->GetPresentationCount() == 0) )
				CToast::Toast(_STR_PPTCTRLMGR_TIP_PPT_NOT_OPEN, true, 1000);
			else if( pController->IsFinal() )
				CToast::Toast(_STR_PPTCTRLMGR_TIP_FINAL_PPT, true, 100);
			else
			{
				BOOL bRet = pThis->PPTInsertFunc(pController, pStream);
				
				if ( bRet == PPTEC_SLIDE_NOT_SELECT )
					CToast::Toast(_STR_PPTCTRLMGR_TIP_SLIDE_NO_SELECT, false, 1000);
				else if ( !bRet )
				{
					tstring strErorInfo = pController->GetLastComErrorInfo();
					tstring strFail= _STR_PPTCTRLMGR_TIP_INSERT_FAILED;
					CToast::Toast(strFail, false, 1000);
					WRITE_LOG_LOCAL(_T("%s"), strErorInfo.c_str());
				}
			}

			delete pStream;
			pController->GetNewApplication();
		}
		Sleep(100);
	}
	return 1;	
}

//
// control PPT inside a new created thread
//
void CPPTControllerManager::AddPPTControlOperation(CStream* pStream, BOOL bFront)
{
	EnterCriticalSection(&m_ControlLock);

	if ( bFront )
		m_lstPptControl.push_front(pStream);
	else
		m_lstPptControl.push_back(pStream);

	SetEvent(m_hControlEvent);

	LeaveCriticalSection(&m_ControlLock);
}

DWORD WINAPI CPPTControllerManager::PPTControlThread(LPARAM lParam)
{
	CPPTControllerManager* pThis = (CPPTControllerManager*) lParam;

	pThis->GetPPTController();
	pThis->m_bRuning = true;

	while ( pThis->m_bRuning )
	{
		Sleep(1);
		WaitForSingleObject(pThis->m_hControlEvent, INFINITE);

		CStream* pStream = NULL;

		EnterCriticalSection(&pThis->m_ControlLock);
		if ( !pThis->m_lstPptControl.empty() )
		{
			pStream	= pThis->m_lstPptControl.front();
			pThis->m_lstPptControl.pop_front();
		}

		if ( !pThis->m_lstPptControl.empty() )
			SetEvent(pThis->m_hControlEvent);
		LeaveCriticalSection(&pThis->m_ControlLock);

		if ( pStream == NULL)
			continue;

		CPPTController* pController = pThis->GetPPTController();
		if ( pController != NULL )
		{
			if ( pController->IsInitFailed() )
				CToast::Toast(_STR_PPTCTRLMGR_TIP_PPT_INIT_FAILED, true, 2000);
			else if ( !pController->IsInit() )
				CToast::Toast(_STR_PPTCTRLMGR_TIP_PPT_NOT_OPEN, true, 1000);
			else	
				pThis->PPTControlFunc(pController, pStream);
	
			delete pStream;
		}
		Sleep(1);
	}
	return 1;	
}

CPPTControllerManager* CPPTControllerManager::GetInstance()
{
	if( g_pPPTControllerManager == NULL )
	{
		g_pPPTControllerManager = new CPPTControllerManager;
		g_pPPTControllerManager->Initialize();
	}

	if( g_pPPTControllerManager == NULL )
		return FALSE;

	return g_pPPTControllerManager;
}

BOOL CPPTControllerManager::UnInstance()
{
	if ( g_pPPTControllerManager != NULL )
	{
		g_pPPTControllerManager->Destroy();
		g_pPPTControllerManager = NULL;
		return TRUE;
	}

	return FALSE;

}

BOOL CPPTControllerManager::PPTInsertFunc(CPPTController* pController, CStream* pStream )
{
	pStream->ResetCursor();

	CStream* pRetStream = new CStream(256);
	BOOL bRet = FALSE;

	if( pController != NULL )
	{
		int	nType = pStream->ReadDWORD();
		pRetStream->WriteDWORD(nType);

		int nSlideId		 = 0;
		int nPalceHolderId	 = 0;

		switch (nType)
		{
		case STAT_INSERT_SLIDE:
			{
				bRet =	pController->AddSlide();
				pRetStream->WriteBOOL(bRet);
			}
			break;
		case STAT_REMOVE_PALCE_HOLDER:
			{
				nSlideId			 = pStream->ReadInt();
				nPalceHolderId		 = pStream->ReadInt();
				RemovePPTPalceHolder(pController, nSlideId, nPalceHolderId);
				pRetStream->WriteBOOL(bRet);
				bRet = TRUE;
			}
			break;
		case STAT_INSET_PALCE_HOLDER:
			{
				tstring strPath		 = pStream->ReadString();
				int nLeft			 = pStream->ReadInt();
				int nTop			 = pStream->ReadInt();
				int nWidth			 = pStream->ReadInt();
				int nHeight			 = pStream->ReadInt();
				int nSlideIdx		 = pStream->ReadInt();

				NOTIFY_EVENT* pNotify= (NOTIFY_EVENT*)pStream->ReadDWORD();

				int nOutSlideId			= 0;
				int nOutPalceHolderId	= 0;

				bRet = pController->InsertPicture(strPath.c_str(), nOutSlideId, nOutPalceHolderId, nLeft, nTop, nWidth, nHeight, nSlideIdx);

				if ( bRet )
				{
					PALCE_HOLDER sPalceHolder = {nOutSlideId, nOutPalceHolderId};
					m_mapPalceHolders.insert(pair<DWORD, PALCE_HOLDER>(nOutSlideId, sPalceHolder));
				}

				if ( pNotify != NULL && pNotify->m_OnComplete != NULL )
				{
					CStream cStream = CStream(256);
					cStream.WriteBOOL(bRet);
					cStream.WriteDWORD(nOutSlideId);
					cStream.WriteInt(nOutPalceHolderId);

					cStream.WriteInt(nLeft);
					cStream.WriteInt(nTop);

					cStream.ResetCursor();
					// notify caller
					TEventNotify CompleteNotify;
					memset(&CompleteNotify, 0, sizeof(TEventNotify));
					CompleteNotify.wParam = (WPARAM)&cStream;

					if( m_hOwnerWnd != NULL )
						SendMessage(m_hOwnerWnd, WM_USER_PPT_CONTROL, (WPARAM)&pNotify->m_OnComplete, (LPARAM)&CompleteNotify);
					else
						pNotify->m_OnComplete(&CompleteNotify);

					delete pNotify;
				}

				pRetStream->WriteBOOL(bRet);
			}
			break;
		case STAT_INSERT_PICTURE:
			{
				tstring strPath		 = pStream->ReadString();
				int nLeft			 = pStream->ReadInt();
				int nTop			 = pStream->ReadInt();
				int nWidth			 = pStream->ReadInt();
				int nHeight			 = pStream->ReadInt();
				int nSlideIdx		 = pStream->ReadInt();
				nSlideId			 = pStream->ReadInt();
				nPalceHolderId		 = pStream->ReadInt();

				if ( nSlideId == 0 )
					bRet = pController->InsertPicture(strPath.c_str(), nSlideId, nPalceHolderId, nLeft, nTop, nWidth, nHeight, nSlideIdx);
				else
					bRet = pController->InsertPictureBySlideId(strPath.c_str(), nSlideId, nPalceHolderId, nLeft, nTop, nWidth, nHeight);

				pRetStream->WriteBOOL(bRet);
			}
			break;
		case STAT_INSERT_HYPERLINKPICTURE:
			{
				tstring strPath		 = pStream->ReadString();
				int nLeft			 = pStream->ReadInt();
				int nTop			 = pStream->ReadInt();
				int nWidth			 = pStream->ReadInt();
				int nHeight			 = pStream->ReadInt();
				int nSlideIdx		 = pStream->ReadInt();
				nSlideId			 = pStream->ReadInt();
				nPalceHolderId		 = pStream->ReadInt();
				tstring strHyperLink = pStream->ReadString();

				if ( nSlideId == 0 )
					bRet = pController->InsertHyperLinkPicture(strPath.c_str(), nSlideId, nPalceHolderId, strHyperLink.c_str(), nLeft, nTop, nWidth, nHeight, nSlideIdx);
				else
					bRet = pController->InsertHyperLinkPictureBySlideId(strPath.c_str(), nSlideId, nPalceHolderId, strHyperLink.c_str(), nLeft, nTop, nWidth, nHeight);

				pRetStream->WriteBOOL(bRet);
			}
			break;
		case STAT_INSERT_PPT:
			{
				tstring strPath		 = pStream->ReadString();
				int nStartSlideIdx	 = pStream->ReadInt();
				int nEndSlideIdx	 = pStream->ReadInt();
				int nSlideIdx		 = pStream->ReadInt();
				nSlideId			 = pStream->ReadInt();
				nPalceHolderId		 = pStream->ReadInt();
				NOTIFY_EVENT* pNotify= (NOTIFY_EVENT*)pStream->ReadDWORD();

				tstring strErr;
				if ( nSlideId == 0 )
					bRet = InsertPPT(strPath.c_str(), nStartSlideIdx, nEndSlideIdx, nSlideIdx, strErr, pNotify);
				else
					bRet = InsertPPTBySlideId(strPath.c_str(), nStartSlideIdx, nEndSlideIdx, nSlideId, nPalceHolderId, strErr, pNotify);

				pRetStream->WriteBOOL(bRet);

				if ( pNotify != NULL && pNotify->m_OnComplete != NULL )
				{
					CStream cStream = CStream(256);
					cStream.WriteBOOL(bRet);
					cStream.WriteString(strErr);

					cStream.ResetCursor();
					// notify caller
					TEventNotify CompleteNotify;
					memset(&CompleteNotify, 0, sizeof(TEventNotify));
					CompleteNotify.wParam = (WPARAM)&cStream;

					if( m_hOwnerWnd != NULL )
						SendMessage(m_hOwnerWnd, WM_USER_PPT_CONTROL, (WPARAM)&pNotify->m_OnComplete, (LPARAM)&CompleteNotify);
					else
						pNotify->m_OnComplete(&CompleteNotify);

					delete pNotify;
				}	
					
			}
			break;
		case STAT_INSERT_VIDEO:
			{
				tstring strPath		 = pStream->ReadString();
				int nLeft			 = pStream->ReadInt();
				int nTop			 = pStream->ReadInt();
				int nWidth			 = pStream->ReadInt();
				int nHeight			 = pStream->ReadInt();
				int nSlideIdx		 = pStream->ReadInt();
				nSlideId			 = pStream->ReadInt();
				nPalceHolderId		 = pStream->ReadInt();
				
				if ( nSlideId == 0 )
					bRet = pController->InsertVideo(strPath.c_str(), nLeft, nTop, nWidth, nHeight, nSlideIdx);
				else
					bRet = pController->InsertVideoBySlideId(strPath.c_str(), nSlideId, nPalceHolderId, nLeft, nTop, nWidth, nHeight);

				pRetStream->WriteBOOL(bRet);
			}
			break;
		case STAT_INSERT_FLASH:
			{
				tstring strPath		 = pStream->ReadString();
				tstring strPicPath	 = pStream->ReadString();
				tstring strTitle	 = pStream->ReadString();
				tstring strSign		 = pStream->ReadString();
				int nSlideIdx		 = pStream->ReadInt();
				nSlideId			 = pStream->ReadInt();
				nPalceHolderId		 = pStream->ReadInt();

				if ( nSlideId == 0 )
					bRet = pController->InsertSwf(strPath.c_str(), strPicPath.c_str(), strTitle.c_str(), strSign.c_str(), nSlideIdx);
				else
					bRet = pController->InsertSwfBySlideId(strPath.c_str(), strPicPath.c_str(), strTitle.c_str(), nSlideId, nPalceHolderId, strSign.c_str());
				
				pRetStream->WriteBOOL(bRet);
			}
			break;
		case STAT_INSERT_UPDATE_QUESTION:
			{
				CToast::Toast(_STR_QUESTION_INSERTING, true, 3000);

				tstring strtitle			= pStream->ReadString();
				bool bBasicQuestion			= pStream->ReadBOOL();
				tstring strPath				= pStream->ReadString();
				tstring strQuestionType		= pStream->ReadString();
				tstring strGuid				= pStream->ReadString();
				nSlideId					= pStream->ReadInt();
				nPalceHolderId				= pStream->ReadInt();
				NOTIFY_EVENT* pNotify= (NOTIFY_EVENT*)pStream->ReadDWORD();

				tstring strUrl = GetLocalPath();
				strUrl += _T("\\Package\\nodejs\\app\\player\\index.html?main=/");

				// translate slash
				tstring strQuestionPath = strPath;
				for(int i = 0; i < strQuestionPath.length(); i++)
				{
					if( strQuestionPath.at(i) == _T('\\') )
						strQuestionPath.replace(i, 1, _T("/"));
				}

				strUrl += strQuestionPath;
				strUrl += _T("&sys=pptshell&hidePage=footer");

				//tstring strThumbPath = GenerateQuestionThumbnail(strPath);
				//LPCTSTR szThumbPath = strThumbPath.empty() ? NULL : strThumbPath.c_str();

				if ( nSlideId == 0 )
					bRet = pController->InsertCef(strUrl.c_str(),  strPath.c_str());
				else
					bRet = pController->InsertCefBySlideId(strUrl.c_str(), strPath.c_str(), nSlideId, nPalceHolderId);

				CMainFrame::UploadQuestion(strtitle,strGuid,bBasicQuestion);

				if ( pNotify != NULL && pNotify->m_OnComplete != NULL )
				{
					CStream cStream = CStream(256);
					cStream.WriteBOOL(bRet);
					// notify caller
					TEventNotify CompleteNotify;
					memset(&CompleteNotify, 0, sizeof(TEventNotify));
					CompleteNotify.wParam = (WPARAM)&cStream;
					if( m_hOwnerWnd != NULL )
						SendMessage(m_hOwnerWnd, WM_USER_PPT_CONTROL, (WPARAM)&pNotify->m_OnComplete, (LPARAM)&CompleteNotify);
					else
						pNotify->m_OnComplete(&CompleteNotify);

					delete pNotify;
				}

				pRetStream->WriteBOOL(bRet);
				pRetStream->WriteString(strPath);
			}
			break;
		case STAT_INSERT_QUESTION:
			{
				CToast::Toast(_STR_QUESTION_INSERTING, true, 3000);
				
				tstring strPath				= pStream->ReadString();
				tstring strQuestionType		= pStream->ReadString();
				tstring strGuid				= pStream->ReadString();
				nSlideId					= pStream->ReadInt();
				nPalceHolderId				= pStream->ReadInt();
				NOTIFY_EVENT* pNotify= (NOTIFY_EVENT*)pStream->ReadDWORD();

				tstring strUrl = GetLocalPath();
				strUrl += _T("\\Package\\nodejs\\app\\player\\index.html?main=/");

				// translate slash
				tstring strQuestionPath = strPath;
				for(int i = 0; i < strQuestionPath.length(); i++)
				{
					if( strQuestionPath.at(i) == _T('\\') )
						strQuestionPath.replace(i, 1, _T("/"));
				}

				strUrl += strQuestionPath;
				strUrl += _T("&sys=pptshell&hidePage=footer");

				//tstring strThumbPath = GenerateQuestionThumbnail(strPath);
				//LPCTSTR szThumbPath = strThumbPath.empty() ? NULL : strThumbPath.c_str();
			
				if ( nSlideId == 0 )
					bRet = pController->InsertCef(strUrl.c_str(),  strPath.c_str());
				else
					bRet = pController->InsertCefBySlideId(strUrl.c_str(), strPath.c_str(), nSlideId, nPalceHolderId);

				if ( pNotify != NULL && pNotify->m_OnComplete != NULL )
				{
					CStream cStream = CStream(256);
					cStream.WriteBOOL(bRet);
					// notify caller
					TEventNotify CompleteNotify;
					memset(&CompleteNotify, 0, sizeof(TEventNotify));
					CompleteNotify.wParam = (WPARAM)&cStream;
					if( m_hOwnerWnd != NULL )
						SendMessage(m_hOwnerWnd, WM_USER_PPT_CONTROL, (WPARAM)&pNotify->m_OnComplete, (LPARAM)&CompleteNotify);
					else
						pNotify->m_OnComplete(&CompleteNotify);

					delete pNotify;
				}

				pRetStream->WriteBOOL(bRet);
				pRetStream->WriteString(strPath);
			}
			break;
		case STAT_UPDATE_QUESTION:
			{
				tstring strPath		= pStream->ReadString();
				tstring strGuid		= pStream->ReadString();
				pController->UpdateCef(strPath.c_str(), strGuid.c_str());
				bRet = TRUE;

				pRetStream->WriteBOOL(bRet);

			}
			break;
		case STAT_INSERT_3D:
			{
				tstring strPath		 = pStream->ReadString();
				tstring	strThumbnail = pStream->ReadString();
				nSlideId			 = pStream->ReadInt();
				nPalceHolderId		 = pStream->ReadInt();

				if ( nSlideId == 0 )
					bRet = pController->Insert3D(strPath.c_str(), strThumbnail.c_str());
				else
					bRet = pController->Insert3DBySlideId(strPath.c_str(), strThumbnail.c_str(), nSlideId, nPalceHolderId);

				pRetStream->WriteBOOL(bRet);
			}
			break;
		case STAT_INSERT_VR:
			{
				PPTVRType nVRType	 = (PPTVRType)pStream->ReadDWORD();
				tstring strPath		 = pStream->ReadString();
				tstring	strParam	 = pStream->ReadString();
				tstring	strThumbnail = pStream->ReadString();
				nSlideId			 = pStream->ReadInt();
				nPalceHolderId		 = pStream->ReadInt();

				if (nVRType==VRTYPE_EXE){
					if ( nSlideId == 0 )
						bRet = pController->InsertVRExe(strPath.c_str(), strParam.c_str(),strThumbnail.c_str());
					else
						bRet = pController->InsertVRExeBySlideId(strPath.c_str(), strParam.c_str(),strThumbnail.c_str(), nSlideId, nPalceHolderId);
				}else if (nVRType == VRTYPE_VIDEO){
					if ( nSlideId == 0 )
						bRet = pController->InsertVRVideo(strPath.c_str(), strParam.c_str(),strThumbnail.c_str());
					else
						bRet = pController->InsertVRVideoBySlideId(strPath.c_str(), strParam.c_str(),strThumbnail.c_str(), nSlideId, nPalceHolderId);
				}
				pRetStream->WriteBOOL(bRet);
			}
			break;
		case STAT_INSERT_OTHER:
			{
				tstring strPath		 = pStream->ReadString();
				int nLeft			 = pStream->ReadInt();
				int nTop			 = pStream->ReadInt();
				int nWidth			 = pStream->ReadInt();
				int nHeight			 = pStream->ReadInt();
				int nSlideIdx		 = pStream->ReadInt();
				nSlideId			 = pStream->ReadInt();
				nPalceHolderId		 = pStream->ReadInt();

				if ( nSlideId == 0 )
					bRet = pController->InsertOLEObject(strPath.c_str(), nLeft, nTop, nWidth, nHeight, nSlideIdx);
				else
					bRet = pController->InsertOLEObjectBySlideId(strPath.c_str(), nLeft, nTop, nWidth, nHeight, nSlideIdx);

				pRetStream->WriteBOOL(bRet);
			}
			break;
		}

		if ( nSlideId != 0 && nPalceHolderId != 0 )
			RemovePalceHolder(nSlideId, nPalceHolderId);

		pRetStream->ResetCursor();
		
		BroadcastEvent(EVT_THREAD_INSERT_PPT, (WPARAM)pRetStream, 0, NULL);
	}
	return bRet;
}

BOOL CPPTControllerManager::PPTControlFunc( CPPTController* pController, CStream* pStream )
{
	pStream->ResetCursor();

	CStream* pTmpStream	= new CStream(256);

	BOOL bRet = FALSE;

	if( pController != NULL )
	{
		int	nType = pStream->ReadDWORD();
		pTmpStream->WriteDWORD(nType);

		switch (nType)
		{
		case PPTC_PLAY:
			{
				bRet = pController->Play();
				pTmpStream->WriteDWORD(bRet);
				
			}
			break;
		case PPTC_STOP:
			{
				bRet = pController->Stop();
				pTmpStream->WriteDWORD(bRet);

			}
			break;
		case PPTC_NEXT_SLIDE:
			{
				pController->TopShowSlideView();
				bRet = pController->Next();
				int nState = pController->GetSlideShowViewState();
				pTmpStream->WriteDWORD(bRet);
				pTmpStream->WriteInt(nState);

				OutputDebugString("PPTControlFunc - PPTC_NEXT_SLIDE");
				// 通知移动端PPT已切换到黑幕,切换到黑幕无法响应SlideShowNextSlide
				int nCurrentPage = pController->GetCurrentShowPosition();
				int nViewCount = pController->GetShowViewCount();
				if(nCurrentPage > nViewCount)
				{
					TCHAR szLog[100];
					_stprintf(szLog, _T("Recv PPTCTL_NEXT Page-BlackScreen, Send Black Screen Index, Page=%d, PageCount=%d\n"), nCurrentPage, nViewCount);
					OutputDebugString(szLog);
					BroadcastPostedEvent(EVT_PPT_INFO_SYNC, PPT_PLAY_SLIDE, nCurrentPage);
				}
			}
			break;
		case PPTC_GOTO_SLIDE:
			{
				int nIdx	= pStream->ReadInt();
				pController->TopShowSlideView();
				bRet		= pController->Goto(nIdx);
				pTmpStream->WriteDWORD(bRet);
			}
			break;
		case PPTC_PREVIOUS_SLIDE:
			{
				pController->TopShowSlideView();
				bRet = pController->Prev();
				pTmpStream->WriteDWORD(bRet);
			}
			break;
		case PPTC_SAVE:
			{
				
				tstring strPath = pStream->ReadString();
				BOOL bBefore	= pStream->ReadDWORD();
				BOOL bClosePre	= pStream->ReadDWORD();
				BOOL bICRPlay	= pStream->ReadDWORD();
				int nCommand	= pStream->ReadDWORD();	
				int nCount		= pController->GetPresentationCount();
				
				//bRet = pController->SaveAndClose(strPath.c_str(), bClosePre, bBefore);
				bRet = pController->SaveAs(strPath.c_str());

				CToast::SetShow(TRUE);
				m_bCancelInsert = FALSE;

				pTmpStream->WriteDWORD(bRet);
				pTmpStream->WriteDWORD(nCommand);
				pTmpStream->WriteDWORD(bBefore);
				pTmpStream->WriteDWORD(bClosePre);
				pTmpStream->WriteDWORD(bICRPlay);
				pTmpStream->WriteDWORD(nCount);
				pTmpStream->WriteString(strPath);
			}
			break;
		case PPTC_NEW:
			{
				tstring strTitle = _T("");
				bRet = pController->New(strTitle);
				pTmpStream->WriteDWORD(bRet);
				pTmpStream->WriteString(strTitle);
			}
			break;
		case PPTC_OPEN:
			{
				BOOL bReadOnly		= pStream->ReadDWORD();
				tstring strFilePath = pStream->ReadString();

				//if ( pController->GetPresentationCount() >= 1)
				//	pController->Close(FALSE); //close before doc

				bRet = pController->Open(strFilePath.c_str(), bReadOnly);
			
				if ( !bRet )
					CToast::Toast(_STR_PPTCTRLMGR_TIP_OPEN_PPT_FAILED, true);

				pTmpStream->WriteDWORD(bRet);
				pTmpStream->WriteDWORD(bReadOnly);
				pTmpStream->WriteString(strFilePath);
			}
			break;
		case PPTC_UNINSERT:
			{
				bRet = pController->DeleteSlide();
				pTmpStream->WriteDWORD(bRet);
			}
			break;
		case PPTC_IS_CHANGED_READONLY:
			{
				int nCommand	= pStream->ReadDWORD();
				BOOL bBefore	= pStream->ReadDWORD();
				BOOL bChanged	= pController->IsChanged(bBefore);
				BOOL bReadOnly	= pController->IsReadOnly(bBefore);
				int nCount		= pController->GetPresentationCount();
				tstring strFilePath = pController->GetFilePath(bBefore);
				tstring strFileName = pController->GetFileName(bBefore);

				m_bCancelInsert	= FALSE;

				pTmpStream->WriteDWORD(nCommand);
				pTmpStream->WriteDWORD(bBefore);
				pTmpStream->WriteDWORD(bChanged);
				pTmpStream->WriteDWORD(bReadOnly);
				pTmpStream->WriteDWORD(nCount);
				pTmpStream->WriteString(strFilePath);
				pTmpStream->WriteString(strFileName);
			}
			break;
		case PPTC_CLOSE:
			{
				BOOL bBefore	= pStream->ReadDWORD();
				BOOL bRet		= pController->Close(bBefore);

				pTmpStream->WriteDWORD(bRet);
				pTmpStream->WriteDWORD(bBefore);
			}
			break;
		case PPTC_CLOSE_APP:
			{
				BOOL bRet =	pController->ClosePPTWnd();
				pTmpStream->WriteDWORD(bRet);
			}
			break;
		case PPTC_SET_PEN_COLOR:
			{
				DWORD dwColor = pStream->ReadDWORD();
				BOOL bRet	  = pController->SetPenColor(dwColor);

				pTmpStream->WriteDWORD(bRet);
			}
			break;
		case PPTC_SET_POINTER_TYPE:
			{
				int nType = pStream->ReadDWORD();
				BOOL bRet = pController->SetPointerType((PpSlideShowPointerType)nType);

				pTmpStream->WriteDWORD(bRet);
			}
			break;
		case PPTC_CLEAR_INK:
			{
				BOOL bSliderClear	=	pController->ClearInk(TRUE);
				BOOL bShowViewClear =	pController->ClearShowViewInk(FALSE);

				pTmpStream->WriteDWORD( (bSliderClear && bShowViewClear) );
			}
			break;
		case PPTC_UNDO:
			{
				int nCount = pStream->ReadDWORD();
				BOOL bRet =	pController->UnDo(nCount);
				pTmpStream->WriteDWORD(bRet);
			}
			break;
		case PPTC_GET_SLIDER_INDEX:
			{
				int nIndex = pController->GetShowViewIndex();
				pTmpStream->WriteDWORD(nIndex);
				pTmpStream->WriteDWORD(FALSE);
			}
			break;

		case PPTC_GET_SLIDER_COUNT:
			{
				int nCount = pController->GetSlideCount();
				pTmpStream->WriteDWORD(nCount);
			}
			break;
		
		case PPTC_RESTORE_FLASH_FRAME_NUM:
			{
				int nIndex = pController->GetShowViewIndex();
				pController->RestoreFlashFrameNum(nIndex);
			}
			break;

		case PPTC_GET_VIDEO_LIST:
			{
				vector<float> vecVideoPositions;
				int nSlideIndex = 0;

				pController->ObtainVideoList(vecVideoPositions, nSlideIndex);

				pTmpStream->WriteDWORD(nSlideIndex);
				pTmpStream->WriteDWORD(vecVideoPositions.size());

				for(int i = 0; i < (int)vecVideoPositions.size(); i++)
					pTmpStream->WriteFloat(vecVideoPositions[i]);

			}
			break;

		case PPTC_GET_QUESTION_LIST:
			{
				//	
				float fLeft		= 0.0f;
				float fTop		= 0.0f;
				float fRight	= 100.0f;
				float fBottom	= 100.0f;

				pController->SlidePosToScreenPos(fLeft, fTop);
				pController->SlidePosToScreenPos(fRight, fBottom);

				pTmpStream->WriteFloat(fLeft);
				pTmpStream->WriteFloat(fTop);
				pTmpStream->WriteFloat(fRight);
				pTmpStream->WriteFloat(fBottom);

				//
				vector<tstring> vecQuestionList;
				pController->ObtainQuestionList(vecQuestionList);

				pTmpStream->WriteDWORD(vecQuestionList.size());
				
				for(int i = 0; i < (int)vecQuestionList.size(); i++)
					pTmpStream->WriteString(vecQuestionList[i]);
			}	
			break;
		case PPTC_HAS_ACTIVEX_AT_PADGE:
			{
				int nIndex = pStream->ReadInt();

				CStream result(256);
				pController->GetSlideAllShapesInfo(nIndex, &result);

				*pTmpStream += result;

				// set pointer type
				//if( hasQuestion )
					//pController->SetPointerType(ppSlideShowPointerArrow);
			}	
			break;

		case PPTC_SEND_EVENT_TO_CEF:
			{
				tstring strEventName = pStream->ReadString();
				tstring strEventData = pStream->ReadString();

				pController->SendEventToCef(strEventName.c_str(), strEventData.c_str());
			}
			break;

		case PPTC_SET_FOCUS_TO_CEF:
			pController->SetFocusToCef(false);
			break;
		case PPTC_PLAY_VIDEO:
			pController->PlayVRVideo();
			break;
		case PPTC_STOP_VIDEO:
			pController->StopVRVideo();
			break;
		}

		BroadcastPostedEvent(EVT_THREAD_CONTROL_PPT, (WPARAM)pTmpStream, 0, NULL);
	}

	return bRet;
}

BOOL CPPTControllerManager::ExportPPTBackGround(LPCTSTR szPath, int nStartSlideIdx, int nEndSlideIdx, std::set<int>& vecHasBackGround)
{
	vecHasBackGround.clear();

	CResGuard::CGuard gd(CPPTController::m_Lock);

	try
	{
		CPPTController* pController = GetPPTController();

		_ApplicationPtr pApplication = pController->GetApplication();
		if( pApplication == NULL )
			return FALSE;

		//ppt是否可操作判断
		if ( !pController->IsPPTActive() || pController->isPlayMode() )	
		{	
			return FALSE;
		}

		//if ( !pController->ActivatePane(1) )
		//{
		//	return FALSE;
		//}

		PresentationsPtr pSourcePres = pApplication->GetPresentations();
		if ( pSourcePres == NULL )
			return FALSE;

		CPPTController::m_nOperationPpt		= OPE_USER_INSERT;
		Sleep(10);
		_PresentationPtr pSourePre = NULL;
		pSourePre = pSourcePres->Open2007(szPath, Office::msoTrue, Office::msoFalse, Office::msoFalse, Office::msoCTrue);
		Sleep(10);

		CPPTController::m_nOperationPpt		= OPE_POWERPOINT;

		VARIANT_BOOL bFinal = pSourePre->GetFinal();

		if ( bFinal )
			pSourePre->PutFinal(VARIANT_FALSE);

		if ( pSourePre == NULL )
			return FALSE;

		SlidesPtr pSourceSlides = pSourePre->GetSlides();
		if ( pSourceSlides == NULL )
		{
			pSourePre->Close();
			return FALSE;
		}

		if ( nEndSlideIdx == -1 )
			nEndSlideIdx = pSourceSlides->GetCount();

		TCHAR szBkPicPath[MAX_PATH] = {0};
		TCHAR szBkFolder[MAX_PATH] = {0};

		wsprintf(szBkFolder, _T("%s\\Cache\\Background"), GetLocalPath().c_str());
		// create directory if not exist
		CreateDirectory(szBkFolder, NULL);


		int nCount = 0;
		int i = 0;

		for ( i= nStartSlideIdx; i <= nEndSlideIdx; i++)
		{
			wsprintf(szBkPicPath, _T("%s\\bg_%d.jpg"), szBkFolder, i);
			if ( m_bCancelInsert )
			{
				pSourePre->Close();
				return -1;
			}

			_SlidePtr pSourceSlide = pSourceSlides->Item(_variant_t(long(i)));
			if ( pSourceSlide == NULL)
				continue;

			if (pController->ExportBackgroundToImages_check(szBkPicPath, _T("JPG"), pSourceSlide))
				vecHasBackGround.insert(i);
		}

		//pSourePre->Tags->Add(_T("close"),_T("thread"));
		pSourePre->Close();
	}
	catch(_com_error& e)
	{
		if( (LPCTSTR)e.Description() )
		{
			OutputDebugString((LPCTSTR)e.Description());
		}
		return FALSE;
	}

	return TRUE;
}

//
// Insert PPT
//

BOOL CPPTControllerManager::InsertPPT( LPCTSTR szPath, int nStartSlideIdx, int nEndSlideIdx, int nSlideIdx, tstring& strErroInfo, NOTIFY_EVENT* pNotify)
{
	CResGuard::CGuard gd(CPPTController::m_Lock);

	std::set<int> vecHasBG;
	//if ( !ExportPPTBackGround(szPath, nStartSlideIdx, nEndSlideIdx, vecHasBG) )
	//	return FALSE;

	try
	{

		CPPTController* pController = GetPPTController();

		_ApplicationPtr pApplication = pController->GetApplication();
		if( pApplication == NULL )
			return FALSE;

		//ppt是否可操作判断
		if ( !pController->IsPPTActive() || pController->isPlayMode() )	
		{	
			return FALSE;
		}

		//if ( !pController->ActivatePane(1) )
		//{
		//	return FALSE;
		//}

		SlidesPtr pSlides = pController->GetSlides();
		if( pSlides == NULL )
			return FALSE;

		if ( nSlideIdx == 0 )
			nSlideIdx = pController->GetActiveSlideIndex();

		if ( nSlideIdx == 0 )
			return PPTEC_SLIDE_NOT_SELECT;

		nSlideIdx++;

		int nTempSlideIdx = nSlideIdx;

		PresentationsPtr pSourcePres = pApplication->GetPresentations();
		if ( pSourcePres == NULL )
			return FALSE;

		if( pNotify != NULL && pNotify->m_OnProgress )
		{
			// notify start
			TEventNotify ProgressNotify;
			memset(&ProgressNotify, 0, sizeof(TEventNotify));

			CStream cStream(1024);
			cStream.WriteInt(TRUE);
			cStream.WriteInt(0);
			cStream.WriteInt(0);
			cStream.ResetCursor();

			ProgressNotify.wParam = (WPARAM)&cStream;

			if( m_hOwnerWnd != NULL )
				SendMessage(m_hOwnerWnd, WM_USER_PPT_CONTROL, (WPARAM)&pNotify->m_OnProgress, (LPARAM)&ProgressNotify);
			else
				pNotify->m_OnProgress(&ProgressNotify);
		}

		CPPTController::m_nOperationPpt		= OPE_USER_INSERT;
		Sleep(10);
		_PresentationPtr pSourePre = NULL;

		//pSourePre = pSourcePres->Open2007(szPath, Office::msoTrue, Office::msoFalse, Office::msoFalse, Office::msoCTrue);
		pSourePre = pSourcePres->Open(szPath, Office::msoTrue, Office::msoFalse, Office::msoFalse);

		Sleep(10);
		CPPTController::m_nOperationPpt		= OPE_POWERPOINT;

		VARIANT_BOOL bFinal = pSourePre->GetFinal();

		if ( bFinal )  //fix insert final ppt result in error
			pSourePre->PutFinal(VARIANT_FALSE);

		if ( pSourePre == NULL )
			return FALSE;


		SlidesPtr pSourceSlides = pSourePre->GetSlides();

		if ( pSourceSlides == NULL )
		{
			pSourePre->Close();
			return FALSE;
		}


		int i = nStartSlideIdx;

		if ( nEndSlideIdx == -1 )
			nEndSlideIdx = pSourceSlides->GetCount();

		tstring strPPTFileName = pController->GetFileName();

		TCHAR szBkPicPath[MAX_PATH] = {0};
		TCHAR szBkFolder[MAX_PATH] = {0};

		wsprintf(szBkFolder, _T("%s\\Cache\\Background"), GetLocalPath().c_str());

		// create directory if not exist
		CreateDirectory(szBkFolder, NULL);

		int nCount = 0;

		BOOL bInsertBk = TRUE;

		const int MAX_INSERT_PPT_NUM = 512;			// 最多一次插入的页数
		SlideRangePtr pSlideRange[MAX_INSERT_PPT_NUM];

		//insert all slide and export background
		for ( i = nStartSlideIdx; i<=nEndSlideIdx && i<MAX_INSERT_PPT_NUM; i++)
		{
			if ( m_bCancelInsert )
			{
				pSourePre->Close();
				return PPTEC_USER_CANCEL;
			}

			_SlidePtr pSourceSlide = pSourceSlides->Item(_variant_t(long(i)));
			if ( pSourceSlide == NULL)
				continue;

			pSourceSlide->Copy();
			Sleep(1);

			int nSlideCount = pSlides->GetCount();
			if( nTempSlideIdx > nSlideCount+1 )
				nTempSlideIdx = nSlideCount;

			if( nTempSlideIdx == 0 )
				nTempSlideIdx = 1;

			pSlideRange[i] = pSlides->Paste(nTempSlideIdx);

			if( pNotify != NULL && pNotify->m_OnProgress )
			{
				// notify caller
				TEventNotify ProgressNotify;
				memset(&ProgressNotify, 0, sizeof(TEventNotify));

				CStream cStream(1024);
				cStream.WriteInt(FALSE);
				cStream.WriteInt(i - nStartSlideIdx);//current
				cStream.WriteInt(nEndSlideIdx - nStartSlideIdx + 1);//total
				cStream.ResetCursor();

				ProgressNotify.wParam = (WPARAM)&cStream;

				if( m_hOwnerWnd != NULL )
					SendMessage(m_hOwnerWnd, WM_USER_PPT_CONTROL, (WPARAM)&pNotify->m_OnProgress, (LPARAM)&ProgressNotify);
				else
					pNotify->m_OnProgress(&ProgressNotify);
			}

			if (pSlideRange[i] == NULL)
				continue;

			nTempSlideIdx++;

			pSlideRange[i]->PutFollowMasterBackground(Office::msoFalse);

			wsprintf(szBkPicPath, _T("%s\\bg_%d.jpg"), szBkFolder, i);

			if (pController->ExportBackgroundToImages_check(szBkPicPath, _T("JPG"), pSourceSlide))
				vecHasBG.insert(i);

		}

		//insert all background
		for (i = nStartSlideIdx; i<=nEndSlideIdx && i<MAX_INSERT_PPT_NUM; i++)
		{
			if ( m_bCancelInsert )
			{
				pSourePre->Close();
				return -1;
			}

			wsprintf(szBkPicPath, _T("%s\\bg_%d.jpg"), szBkFolder, i);

			if (vecHasBG.find(i) != vecHasBG.end())
			{
				pController->InsertBackground(szBkPicPath, pSlideRange[i], false);
				remove(szBkPicPath);
			}
		}

		//pSourePre->Tags->Add(_T("close"),_T("thread"));
		pSourePre->Close();

		int nSlideCount = pSlides->GetCount();
		if( nSlideCount == 0 )
			return nCount;

		if( nSlideIdx > nSlideCount )
			nSlideIdx = nSlideCount;

		if( nSlideIdx == 0 )
			nSlideIdx = 1;

		_SlidePtr pSlide = pSlides->Item(_variant_t(long(nSlideIdx)));
		if ( pSlide != NULL )
			pSlide->Select();
	}
	catch(_com_error& e)
	{
		TCHAR szErrorInfo[MAX_PATH] = {0};
		TCHAR szDescription[MAX_PATH] = {0};

		if( (LPCTSTR)e.ErrorMessage() )
			_stprintf_s(szErrorInfo, _T("%s"),  (LPCTSTR)e.ErrorMessage());

		if( (LPCTSTR)e.Description() )
		{
			_stprintf_s(szDescription, _T("%s"), (LPCTSTR)e.Description());
			strErroInfo = szErrorInfo;

			if( szDescription[0] != _T('\0') )
			{
				strErroInfo += _T(":");
				strErroInfo += szDescription;
				OutputDebugString(strErroInfo.c_str());
			}
		}

		return FALSE;
	}

	return TRUE;
}

BOOL CPPTControllerManager::InsertPPTBySlideId( LPCTSTR szPath, int nStartSlideIdx, int nEndSlideIdx, int nSlideId, int nPlaceHolderId, tstring& strErroInfo, NOTIFY_EVENT* pNotify )
{
	try
	{
		CPPTController* pController = GetPPTController();

		_ApplicationPtr pApplication = pController->GetApplication();
		if( pApplication == NULL )
			return FALSE;

		_SlidePtr pSlide = pController->GetSlideById(nSlideId);

		if ( pSlide == NULL )
			return -2;

		pController->DelShape(pSlide, nPlaceHolderId);

		return InsertPPT(szPath, nStartSlideIdx, nEndSlideIdx, pSlide->GetSlideIndex(), strErroInfo, pNotify);
	}
	catch(_com_error& e)
	{
		TCHAR szErrorInfo[MAX_PATH] = {0};
		TCHAR szDescription[MAX_PATH] = {0};

		if( (LPCTSTR)e.ErrorMessage() )
			_stprintf_s(szErrorInfo, _T("%s"),  (LPCTSTR)e.ErrorMessage());

		if( (LPCTSTR)e.Description() )
		{
			_stprintf_s(szDescription, _T("%s"), (LPCTSTR)e.Description());
			strErroInfo = szErrorInfo;

			if( szDescription[0] != _T('\0') )
			{
				strErroInfo += _T(":");
				strErroInfo += szDescription;
				OutputDebugString(strErroInfo.c_str());
			}
		}

		return FALSE;
	}

}

BOOL CPPTControllerManager::RemovePalceHolder( DWORD dwSlideId, DWORD dwPalceHolderId )
{
	pair<ITER_PH, ITER_PH> range = m_mapPalceHolders.equal_range(dwSlideId);
	for(ITER_PH itr = range.first; itr != range.second; itr++)
	{
		PALCE_HOLDER sPalceHolder = itr->second;
		
		if ( sPalceHolder.dwSlideId == dwSlideId && sPalceHolder.dwShapeId == dwPalceHolderId )
		{
			m_mapPalceHolders.erase(itr);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CPPTControllerManager::RemovePPTPalceHolder(CPPTController* pPPTController, DWORD dwSlideId, DWORD dwPalceHolderId )
{
	pair<ITER_PH, ITER_PH> range = m_mapPalceHolders.equal_range(dwSlideId);
	for(ITER_PH itr = range.first; itr != range.second; itr++)
	{
		PALCE_HOLDER sPalceHolder = itr->second;

		if ( sPalceHolder.dwSlideId == dwSlideId && sPalceHolder.dwShapeId == dwPalceHolderId )
		{
			pPPTController->DelPalceHolder(sPalceHolder.dwSlideId, sPalceHolder.dwShapeId);
			m_mapPalceHolders.erase(itr);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CPPTControllerManager::RemoveAllPPTPalceHolder(CPPTController* pPPTController)
{
	for(ITER_PH itr = m_mapPalceHolders.begin(); itr != m_mapPalceHolders.end(); itr)
	{
		PALCE_HOLDER sPalceHolder = itr->second;
		pPPTController->DelPalceHolder(sPalceHolder.dwSlideId, sPalceHolder.dwShapeId);
		itr = m_mapPalceHolders.erase(itr);
	}
	
	return TRUE;
}

CPPTController* GetPPTController()
{
	if ( CPPTControllerManager::GetInstance() == NULL )
		return NULL;

	return CPPTControllerManager::GetInstance()->GetPPTController();
}


BOOL PPTControllerMgrUpdate()
{
	if ( CPPTControllerManager::GetInstance() == NULL )
		return FALSE;

	return CPPTControllerManager::GetInstance()->PPTControllerUpdate();
}

BOOL InsertPPTByThread(LPCTSTR szPath, int nStartSlideIdx, int nEndSlideIdx, int nSlideIdx /*= 0*/, int nSlideId/* = 0*/, int nPlaceHolderId/* = 0*/, CEventDelegateBase* pOnProcessDelegate /*= NULL*/, CEventDelegateBase* pOnCompleteDelegate /*= NULL*/)
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;

	CStream* pStream = new CStream(1024);
	pStream->WriteDWORD((DWORD)STAT_INSERT_PPT);
	pStream->WriteString((char*)szPath);
	pStream->WriteDWORD(nStartSlideIdx);
	pStream->WriteDWORD(nEndSlideIdx);
	pStream->WriteDWORD(nSlideIdx);
	pStream->WriteDWORD(nSlideId);
	pStream->WriteDWORD(nPlaceHolderId);


	if ( pOnProcessDelegate != NULL || pOnCompleteDelegate != NULL )
	{
		NOTIFY_EVENT* pNotify = new NOTIFY_EVENT;

		memset(pNotify, 0, sizeof(NOTIFY_EVENT));

		// delegates
		if( pOnCompleteDelegate != NULL && pOnCompleteDelegate->GetFn() != NULL )
			pNotify->m_OnComplete += *pOnCompleteDelegate;

		if( pOnProcessDelegate != NULL && pOnProcessDelegate->GetFn() != NULL )
			pNotify->m_OnProgress += *pOnProcessDelegate;

		pStream->WriteDWORD((DWORD)pNotify);
	}
	else
	{
		pStream->WriteDWORD(NULL);
	}


	CPPTControllerManager::GetInstance()->AddPPTInsertOperation(pStream);

	return TRUE;
}

BOOL InsertPalceHolderByThread( LPCTSTR szPath, int nLeft /*= -1*/, int nTop /*= -1*/, int nWidth /*= -1*/, int nHeight /*= -1*/, int nSlideIdx /*= 0*/, CEventDelegateBase* pOnCompleteDelegate /*= NULL*/ )
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;

	CStream* pStream = new CStream(1024);
	pStream->WriteDWORD((DWORD)STAT_INSET_PALCE_HOLDER);
	pStream->WriteString((char*)szPath);
	pStream->WriteDWORD(nLeft);
	pStream->WriteDWORD(nTop);
	pStream->WriteDWORD(nWidth);
	pStream->WriteDWORD(nHeight);
	pStream->WriteDWORD(nSlideIdx);

	if ( pOnCompleteDelegate != NULL )
	{
		NOTIFY_EVENT* pNotify = new NOTIFY_EVENT;

		memset(pNotify, 0, sizeof(NOTIFY_EVENT));

		// delegates
		if( pOnCompleteDelegate != NULL && pOnCompleteDelegate->GetFn() != NULL )
			pNotify->m_OnComplete += *pOnCompleteDelegate;

		pStream->WriteDWORD((DWORD)pNotify);
	}
	else
	{
		pStream->WriteDWORD(NULL);
	}

	CPPTControllerManager::GetInstance()->AddPPTInsertOperation(pStream);

	return TRUE;
}

BOOL RemovePalceHolderByThread( int nSlideId, int nPalceHolderId )
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;

	CStream* pStream = new CStream(256);
	pStream->WriteDWORD((DWORD)STAT_REMOVE_PALCE_HOLDER);
	pStream->WriteDWORD(nSlideId);
	pStream->WriteDWORD(nPalceHolderId);

	CPPTControllerManager::GetInstance()->AddPPTInsertOperation(pStream);

	return TRUE;
}

BOOL InsertPictureByThread( LPCTSTR szPath, int nLeft /*= -1*/, int nTop /*= -1*/, int nWidth /*= -1*/, int nHeight /*= -1*/, int nSlideIdx /*= 0*/, int nSlideId/* = 0*/, int nPlaceHolderId/* = 0*/)
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;

	CStream* pStream = new CStream(1024);
	pStream->WriteDWORD((DWORD)STAT_INSERT_PICTURE);
	pStream->WriteString((char*)szPath);
	pStream->WriteDWORD(nLeft);
	pStream->WriteDWORD(nTop);
	pStream->WriteDWORD(nWidth);
	pStream->WriteDWORD(nHeight);
	pStream->WriteDWORD(nSlideIdx);
	pStream->WriteDWORD(nSlideId);
	pStream->WriteDWORD(nPlaceHolderId);

	CPPTControllerManager::GetInstance()->AddPPTInsertOperation(pStream);

	return TRUE;
}

BOOL InsertHyperLinkPictureByThread( LPCTSTR szPath, LPCTSTR szHyperlink, int nLeft /*= -1*/, int nTop /*= -1*/, int nWidth /*= -1*/, int nHeight /*= -1*/, int nSlideIdx /*= 0*/, int nSlideId /*= 0*/, int nPlaceHolderId /*= 0*/ )
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;

	CStream* pStream = new CStream(1024);
	pStream->WriteDWORD((DWORD)STAT_INSERT_HYPERLINKPICTURE);
	pStream->WriteString((char*)szPath);
	pStream->WriteDWORD(nLeft);
	pStream->WriteDWORD(nTop);
	pStream->WriteDWORD(nWidth);
	pStream->WriteDWORD(nHeight);
	pStream->WriteDWORD(nSlideIdx);
	pStream->WriteDWORD(nSlideId);
	pStream->WriteDWORD(nPlaceHolderId);
	pStream->WriteString((char*)szHyperlink);

	CPPTControllerManager::GetInstance()->AddPPTInsertOperation(pStream);

	return TRUE;
}

BOOL InsertVideoByThread( LPCTSTR szPath, int nLeft /*= -1*/, int nTop /*= -1*/, int nWidth /*= -1*/, int nHeight /*= -1*/, int nSlideIdx /*= 0*/, int nSlideId/* = 0*/, int nPlaceHolderId/* = 0*/ )
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;


	CStream* pStream = new CStream(1024);
	pStream->WriteDWORD((DWORD)STAT_INSERT_VIDEO);
	pStream->WriteString((char*)szPath);
	pStream->WriteDWORD(nLeft);
	pStream->WriteDWORD(nTop);
	pStream->WriteDWORD(nWidth);
	pStream->WriteDWORD(nHeight);
	pStream->WriteDWORD(nSlideIdx);
	pStream->WriteDWORD(nSlideId);
	pStream->WriteDWORD(nPlaceHolderId);

	CPPTControllerManager::GetInstance()->AddPPTInsertOperation(pStream);

	return TRUE;
}

BOOL InsertOLEObjectByThread( LPCTSTR szPath, int nLeft /*= -1*/, int nTop /*= -1*/, int nWidth /*= -1*/, int nHeight /*= -1*/, int nSlideIdx /*= 0*/, int nSlideId/* = 0*/, int nPlaceHolderId/* = 0*/ )
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;

	CStream* pStream = new CStream(1024);
	pStream->WriteDWORD((DWORD)STAT_INSERT_OTHER);
	pStream->WriteString((char*)szPath);
	pStream->WriteDWORD(nLeft);
	pStream->WriteDWORD(nTop);
	pStream->WriteDWORD(nWidth);
	pStream->WriteDWORD(nHeight);
	pStream->WriteDWORD(nSlideIdx);
	pStream->WriteDWORD(nSlideId);
	pStream->WriteDWORD(nPlaceHolderId);

	CPPTControllerManager::GetInstance()->AddPPTInsertOperation(pStream);

	return TRUE;
}

BOOL InsertSwfByThread( LPCTSTR szPath, LPCTSTR szPicPath, LPCTSTR szTitle, LPCTSTR szSign, int nSlideIdx /*= 0*/, int nSlideId/* = 0*/, int nPlaceHolderId/* = 0*/ )
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;

	CStream* pStream = new CStream(1024);
	pStream->WriteDWORD((DWORD)STAT_INSERT_FLASH);
	pStream->WriteString((char*)szPath);
	pStream->WriteString((char*)szPicPath);
	pStream->WriteString((char*)szTitle);
	pStream->WriteString((char*)szSign);
	pStream->WriteDWORD(nSlideIdx);
	pStream->WriteDWORD(nSlideId);
	pStream->WriteDWORD(nPlaceHolderId);

	CPPTControllerManager::GetInstance()->AddPPTInsertOperation(pStream);

	return TRUE;
}

BOOL InsertUpdateQuestionByThread( LPCTSTR szTitle,bool bBasicQuestion,LPCTSTR szPath, LPCTSTR szQuestionType, LPCTSTR szGuid, int nSlideId/* = 0*/, int nPlaceHolderId/* = 0*/, CEventDelegateBase* pOnCompleteDelegate /*= NULL*/ )
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;

	CStream* pStream = new CStream(1024);
	pStream->WriteDWORD((DWORD)STAT_INSERT_UPDATE_QUESTION);
	pStream->WriteString((char*)szTitle);
	pStream->WriteBOOL(bBasicQuestion);
	pStream->WriteString((char*)szPath);
	pStream->WriteString((char*)szQuestionType);
	pStream->WriteString((char*)szGuid);
	pStream->WriteDWORD(nSlideId);
	pStream->WriteDWORD(nPlaceHolderId);

	if ( pOnCompleteDelegate != NULL )
	{
		NOTIFY_EVENT* pNotify = new NOTIFY_EVENT;

		memset(pNotify, 0, sizeof(NOTIFY_EVENT));

		// delegates
		if( pOnCompleteDelegate != NULL && pOnCompleteDelegate->GetFn() != NULL )
			pNotify->m_OnComplete += *pOnCompleteDelegate;

		pStream->WriteDWORD((DWORD)pNotify);
	}
	else
	{
		pStream->WriteDWORD(NULL);
	}

	CPPTControllerManager::GetInstance()->AddPPTInsertOperation(pStream);

	return TRUE;
}
BOOL InsertQuestionByThread(LPCTSTR szPath, LPCTSTR szQuestionType, LPCTSTR szGuid, int nSlideId/* = 0*/, int nPlaceHolderId/* = 0*/, CEventDelegateBase* pOnCompleteDelegate /*= NULL*/ )
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;

	CStream* pStream = new CStream(1024);
	pStream->WriteDWORD((DWORD)STAT_INSERT_QUESTION);
	pStream->WriteString((char*)szPath);
	pStream->WriteString((char*)szQuestionType);
	pStream->WriteString((char*)szGuid);
	pStream->WriteDWORD(nSlideId);
	pStream->WriteDWORD(nPlaceHolderId);

	if ( pOnCompleteDelegate != NULL )
	{
		NOTIFY_EVENT* pNotify = new NOTIFY_EVENT;

		memset(pNotify, 0, sizeof(NOTIFY_EVENT));

		// delegates
		if( pOnCompleteDelegate != NULL && pOnCompleteDelegate->GetFn() != NULL )
			pNotify->m_OnComplete += *pOnCompleteDelegate;

		pStream->WriteDWORD((DWORD)pNotify);
	}
	else
	{
		pStream->WriteDWORD(NULL);
	}

	CPPTControllerManager::GetInstance()->AddPPTInsertOperation(pStream);

	return TRUE;
}

BOOL UpdateQuestionByThread(LPCTSTR szPath, LPCTSTR szGuid)
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;

	CStream* pStream = new CStream(1024);
	pStream->WriteDWORD((DWORD)STAT_UPDATE_QUESTION);
	pStream->WriteString((char*)szPath);
	pStream->WriteString((char*)szGuid);

	CPPTControllerManager::GetInstance()->AddPPTInsertOperation(pStream);

	return TRUE;
}

BOOL Insert3DByThread( LPCTSTR szPath, LPCTSTR szThumbnailPath, int nSlideId /*=0*/, int nPlaceHolderId /*= 0*/ )
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;

	CStream* pStream = new CStream(1024);
	pStream->WriteDWORD((DWORD)STAT_INSERT_3D);
	pStream->WriteString((char*)szPath);
	pStream->WriteString((char*)szThumbnailPath);
	pStream->WriteDWORD(nSlideId);
	pStream->WriteDWORD(nPlaceHolderId);

	CPPTControllerManager::GetInstance()->AddPPTInsertOperation(pStream);

	return TRUE;
}

BOOL InsertVRByThread(PPTVRType nVrType, LPCTSTR szPath, LPCTSTR szParam,LPCTSTR szThumbnailPath, int nSlideId /*=0*/, int nPlaceHolderId /*= 0*/ )
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;

	CStream* pStream = new CStream(1024);
	pStream->WriteDWORD((DWORD)STAT_INSERT_VR);
	pStream->WriteDWORD((DWORD)nVrType);
	pStream->WriteString((char*)szPath);
	pStream->WriteString((char*)szParam);
	pStream->WriteString((char*)szThumbnailPath);
	pStream->WriteDWORD(nSlideId);
	pStream->WriteDWORD(nPlaceHolderId);

	CPPTControllerManager::GetInstance()->AddPPTInsertOperation(pStream);

	return TRUE;
}


BOOL AddSlideByThread()
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;
	CStream* pStream = new CStream(256);
	pStream->WriteDWORD(STAT_INSERT_SLIDE);
	CPPTControllerManager::GetInstance()->AddPPTInsertOperation(pStream);
	return TRUE;
}


//控制相关
BOOL PlayPPTByThread()
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;
	CStream* pStream = new CStream(256);
	pStream->WriteDWORD(PPTC_PLAY);
	CPPTControllerManager::GetInstance()->AddPPTControlOperation(pStream, TRUE);

	return TRUE;
}

BOOL StopPPTByThread()
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;

	CStream* pStream = new CStream(256);
	pStream->WriteDWORD(PPTC_STOP);

	CPPTControllerManager::GetInstance()->AddPPTControlOperation(pStream, TRUE);

	return TRUE;
}

BOOL NextSlideByThread()
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;

	CStream* pStream = new CStream(256);
	pStream->WriteDWORD(PPTC_NEXT_SLIDE);

	CPPTControllerManager::GetInstance()->AddPPTControlOperation(pStream);

	return TRUE;
}

BOOL GotoSlideByThread(int nSlideIdx)
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;

	CStream* pStream = new CStream(256);
	pStream->WriteDWORD(PPTC_GOTO_SLIDE);
	pStream->WriteInt(nSlideIdx);

	CPPTControllerManager::GetInstance()->AddPPTControlOperation(pStream);

	return TRUE;
}

BOOL PreviousSlideByThread()
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;

	CStream* pStream = new CStream(256);
	pStream->WriteDWORD(PPTC_PREVIOUS_SLIDE);

	CPPTControllerManager::GetInstance()->AddPPTControlOperation(pStream);

	return TRUE;
}

BOOL SavePPTByThread( LPCTSTR szPath, BOOL bBefore, BOOL bClosePre, BOOL bICRPlay, int nCommand )
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;

	CPPTControllerManager::GetInstance()->m_bCancelInsert = TRUE;
	CToast::SetShow(FALSE);

	CStream* pStream = new CStream(256);
	pStream->WriteDWORD(PPTC_SAVE);
	pStream->WriteString((char*)szPath);
	pStream->WriteDWORD(bBefore);
	pStream->WriteDWORD(bClosePre);
	pStream->WriteDWORD(bICRPlay);
	pStream->WriteDWORD(nCommand);

	CPPTControllerManager::GetInstance()->AddPPTControlOperation(pStream);

	return TRUE;
}

BOOL NewPPTByThread()
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;
	CStream* pStream = new CStream(256);
	pStream->WriteDWORD(PPTC_NEW);
	CPPTControllerManager::GetInstance()->AddPPTControlOperation(pStream);

	return TRUE;
}

BOOL OpenPPTByThread( LPCTSTR szPath, BOOL bReadOnly )
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;

	CStream* pStream = new CStream(256);
	pStream->WriteDWORD(PPTC_OPEN);
	pStream->WriteDWORD(bReadOnly);
	pStream->WriteString((char*)szPath);

	CPPTControllerManager::GetInstance()->AddPPTControlOperation(pStream);

	return TRUE;
}

BOOL OpenDeskTopPPTByThread( LPCTSTR szPath, BOOL bReadOnly )
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;

	CStream* pStream = new CStream(256);
	pStream->WriteDWORD(PPTC_OPEN_DESKTOPPPT);
	pStream->WriteDWORD(bReadOnly);
	pStream->WriteString((char*)szPath);

	CPPTControllerManager::GetInstance()->AddPPTControlOperation(pStream);

	return TRUE;
}


BOOL IsPPTChangedReadOnlyByThread(BOOL bBefore, int nCommand)
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;
	
	CPPTControllerManager::GetInstance()->m_bCancelInsert = TRUE;
	CStream* pStream = new CStream(256);
	pStream->WriteDWORD(PPTC_IS_CHANGED_READONLY);
	pStream->WriteDWORD(nCommand);
	pStream->WriteDWORD(bBefore);
	
	CPPTControllerManager::GetInstance()->AddPPTControlOperation(pStream, TRUE);

	return TRUE;
}

BOOL ClosePPTByThread(BOOL bBefore)
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;
	CStream* pStream = new CStream(256);
	pStream->WriteDWORD(PPTC_CLOSE);
	pStream->WriteDWORD(bBefore);
	CPPTControllerManager::GetInstance()->AddPPTControlOperation(pStream, TRUE);

	return TRUE;
}

BOOL CloseAPPByThread()
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;
	CStream* pStream = new CStream(256);
	pStream->WriteDWORD(PPTC_CLOSE_APP);
	CPPTControllerManager::GetInstance()->AddPPTControlOperation(pStream);

	return TRUE;
}


BOOL SetPenColorByThread( DWORD dwColor )
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;
	CStream* pStream = new CStream(256);
	pStream->WriteDWORD(PPTC_SET_PEN_COLOR);
	pStream->WriteDWORD(dwColor);
	CPPTControllerManager::GetInstance()->AddPPTControlOperation(pStream);

	return TRUE;
}

BOOL SetPointerTypeByThread( int nType )
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;
	CStream* pStream = new CStream(256);
	pStream->WriteDWORD(PPTC_SET_POINTER_TYPE);
	pStream->WriteDWORD(nType);
	CPPTControllerManager::GetInstance()->AddPPTControlOperation(pStream);

	return TRUE;
}

BOOL ClearInkByThread()
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;
	CStream* pStream = new CStream(256);
	pStream->WriteDWORD(PPTC_CLEAR_INK);
	CPPTControllerManager::GetInstance()->AddPPTControlOperation(pStream);

	return TRUE;
}

BOOL UnDoByThread(int nCount)
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;
	CStream* pStream = new CStream(256);
	pStream->WriteDWORD(PPTC_UNDO);
	pStream->WriteDWORD(nCount);
	CPPTControllerManager::GetInstance()->AddPPTControlOperation(pStream);

	return TRUE;
}

BOOL UnDoInsertPPTByThread()
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;

	CStream* pStream = new CStream(12);
	pStream->WriteDWORD((DWORD)PPTC_UNINSERT);
	CPPTControllerManager::GetInstance()->AddPPTControlOperation(pStream);

	return TRUE;
}

BOOL GetCurSlideIndexByThread()
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;
	CStream* pStream = new CStream(256);
	pStream->WriteDWORD(PPTC_GET_SLIDER_INDEX);
	CPPTControllerManager::GetInstance()->AddPPTControlOperation(pStream);

	return TRUE;
}

BOOL GetCurSlideCountByThread()
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;
	CStream* pStream = new CStream(256);
	pStream->WriteDWORD(PPTC_GET_SLIDER_COUNT);
	CPPTControllerManager::GetInstance()->AddPPTControlOperation(pStream);

	return TRUE;
}

BOOL RestoreFlashFrameNumByThread()
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;
	CStream* pStream = new CStream(256);
	pStream->WriteDWORD(PPTC_RESTORE_FLASH_FRAME_NUM);
	CPPTControllerManager::GetInstance()->AddPPTControlOperation(pStream);

	return TRUE;
}

BOOL GetVideoListByThread()
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;
	CStream* pStream = new CStream(256);
	pStream->WriteDWORD(PPTC_GET_VIDEO_LIST);
	CPPTControllerManager::GetInstance()->AddPPTControlOperation(pStream);
	return TRUE;
}

BOOL GetQuestionListByThread()
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;
	CStream* pStream = new CStream(256);
	pStream->WriteDWORD(PPTC_GET_QUESTION_LIST);
	CPPTControllerManager::GetInstance()->AddPPTControlOperation(pStream);
	return TRUE;
}

BOOL SendEventToCefByThread(LPCTSTR szEventName, LPCTSTR szEventData)
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;
	CStream* pStream = new CStream(256);
	pStream->WriteDWORD(PPTC_SEND_EVENT_TO_CEF);
	pStream->WriteString(szEventName);
	pStream->WriteString(szEventData);
	CPPTControllerManager::GetInstance()->AddPPTControlOperation(pStream);
	return TRUE;
}

BOOL SetFocusToCefByThread()
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;
	CStream* pStream = new CStream(256);
	pStream->WriteDWORD(PPTC_SET_FOCUS_TO_CEF);

	CPPTControllerManager::GetInstance()->AddPPTControlOperation(pStream);
	return TRUE;
}

BOOL VRPlayVideo()
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;
	CStream* pStream = new CStream(256);
	pStream->WriteDWORD(PPTC_PLAY_VIDEO);

	CPPTControllerManager::GetInstance()->AddPPTControlOperation(pStream);
	return TRUE;
}
BOOL VRStopVideo()
{
	if ( CPPTControllerManager::GetInstance() == NULL ) return FALSE;
	CStream* pStream = new CStream(256);
	pStream->WriteDWORD(PPTC_STOP_VIDEO);

	CPPTControllerManager::GetInstance()->AddPPTControlOperation(pStream);
	return TRUE;
}
