//-----------------------------------------------------------------------
// FileName:				NDCloudQuestion.cpp
//
// Desc:
//------------------------------------------------------------------------
#include "stdafx.h"
#include "NDCloudAPI.h"
#include "NDCloudCoursewareObjects.h"
#include "Util/Util.h"
#include "ThirdParty/zip/ZipWrapper.h"
#include "NDCloud/NDCloudUser.h"

#define EduPlatformHost							_T("esp-lifecycle.web.sdp.101.com")

CNDCloudCoursewareObjects::CNDCloudCoursewareObjects(tstring strGuid, DWORD dwUserId, CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressDelegate,void* pUserData)
{
	m_strGuid			= strGuid;
	m_dwUserId			= dwUserId;
	m_bPolling			= FALSE;
	m_bFinished			= FALSE;
	m_bPacking			= FALSE;
	m_bPolled			= FALSE;
	m_pUserData			= pUserData;
	m_dwDownloadId		= 0;

	if( OnCompleteDelegate.GetFn() != NULL )
		m_OnComplete += OnCompleteDelegate;

	if( OnProgressDelegate.GetFn() != NULL )
		m_OnProgress += OnProgressDelegate;
	
}

CNDCloudCoursewareObjects::~CNDCloudCoursewareObjects()
{
	
}

bool CNDCloudCoursewareObjects::Cancel()
{
	m_OnComplete.clear();
	m_OnProgress.clear();

	return true;
}

bool CNDCloudCoursewareObjects::Pause(CHttpDelegateBase* pDelegate)
{
	if( m_dwDownloadId != 0 )
		NDCloudDownloadPause(m_dwDownloadId, pDelegate);
	else
	{
		// it is still polling then stop polling and remove it
		m_bFinished = TRUE;
	}

	return true;
}

bool CNDCloudCoursewareObjects::Resume()
{
	if( m_dwDownloadId != 0 )
		NDCloudDownloadResume(m_dwDownloadId);

	return true;
}

bool CNDCloudCoursewareObjects::PollPackState()
{
	if( m_bFinished )
		return false;	// done already 

	if( m_bPolled )
		return true;

	if( m_bPolling )
		return true;

	tstring strUrl = NDCloudComposeUrlPackCoursewareObjectsState(m_strGuid, m_dwUserId);
	NDCloudDownloadEx(strUrl, _T(""), _T("GET"), "",  MakeHttpDelegate(this, &CNDCloudCoursewareObjects::OnPackStateObtained));
	
	m_bPolling = TRUE;
	return true;
}

bool CNDCloudCoursewareObjects::OnPackStateObtained(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pData[pNotify->nDataSize] = '\0';

	m_bPolling = FALSE;

	string str = Utf8ToAnsi(pNotify->pData);


	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(str, root);
	if( !res )
		return false;

	// check state
	if( root["archive_state"].isNull() )
	{
		// "LC/RESOURCE_NOT_FOUND"
		if( !root["code"].isNull() )
		{
			// error occur
			m_bFinished = true;
		}

		return false;
	}

	tstring strState = Ansi2Str(root["archive_state"].asCString());
	if( strState == _T("ready") )
	{
		m_bPolled = TRUE;

		// can download now
		tstring strUrl = Ansi2Str(root["access_url"].asCString());
		tstring strMD5 = Ansi2Str(root["md5"].asCString());
		m_dwDownloadId = NDCloudDownloadFile(strUrl, _T(""), _T(""), CloudFileCoursewareObjects, 0, MakeHttpDelegate(this, &CNDCloudCoursewareObjects::OnCoursewareObjectsDownloaded), MakeHttpDelegate(this, &CNDCloudCoursewareObjects::OnCoursewareObjectsDownloading), NULL, strMD5);	
	}
	else if( strState == _T("unpack") || strState == _T("pending") )
	{
		// pack this question
		if( m_bPacking )
			return true;

		tstring strUrl = NDCloudComposeUrlPackCoursewareObjects(m_strGuid, m_dwUserId);
		NDCloudDownloadEx(strUrl, _T(""), _T("POST"), "",  MakeHttpDelegate(this, &CNDCloudCoursewareObjects::OnPackStarted));
		m_bPacking = TRUE;
	}
	else
	{
		m_bFinished = TRUE;

		THttpNotify Notify;
		Notify.strFilePath = _T("");
		Notify.dwErrorCode = 1;
		Notify.pUserData = m_pUserData;
		m_OnComplete(&Notify);
	}
	
	return true;
}

bool CNDCloudCoursewareObjects::OnPackStarted(void *param)
{
	// pack started
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pData[pNotify->nDataSize] = '\0';


	string str = Utf8ToAnsi(pNotify->pData);


	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(str, root);
	if( !res )
		return false;

	return true;
}

bool CNDCloudCoursewareObjects::OnCoursewareObjectsDownloaded(void* param)
{
	
	THttpNotify* pNotify = (THttpNotify*)param;
	
	// unzip
	if( pNotify->strFilePath != _T("") )
	{
		// folder
		tstring strFolder = pNotify->strFilePath;
		int pos = strFolder.rfind('\\');
		if( pos != -1 )
			strFolder = strFolder.substr(0, pos);

		strFolder +=  _T("\\");
		strFolder += m_strGuid;
		strFolder += _T(".pkg");
 
		CUnZipper UnZipper;
		UnZipper.UnZip(Str2Ansi(pNotify->strFilePath).c_str(), Str2Ansi(strFolder).c_str());

		pNotify->strFilePath = strFolder;
//		pNotify->strFilePath += _T("\\item.xml");
	}
	pNotify->pUserData=m_pUserData;
	m_OnComplete(param);

	m_bFinished = TRUE;
	return true;
}

bool CNDCloudCoursewareObjects::OnCoursewareObjectsDownloading(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;

	if( m_OnProgress )
		m_OnProgress(param);

	return true;
}

//-----------------------------------------------------------
// CNDCloudQuestionManager
//
CNDCloudCoursewareObjectsManager::CNDCloudCoursewareObjectsManager()
{
	InitializeCriticalSection(&m_Lock);
}

CNDCloudCoursewareObjectsManager::~CNDCloudCoursewareObjectsManager()
{
	Destroy();
	DeleteCriticalSection(&m_Lock);
}

BOOL CNDCloudCoursewareObjectsManager::Initialize()
{
	m_hEvent	= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hThread	= CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PollPackStateThread, this, 0, NULL);

	return TRUE;
}

BOOL CNDCloudCoursewareObjectsManager::Destroy()
{
	if(m_hThread!=NULL)
	{
		TerminateThread(m_hThread, 0);
		m_hThread=NULL;
	}
	if(m_hEvent=NULL)
	{
		CloseHandle(m_hEvent);
		m_hEvent=NULL;
	}
	
	return TRUE;
}

void CNDCloudCoursewareObjectsManager::PollPackState()
{
	while(TRUE)
	{
		DWORD ret = WaitForSingleObject(m_hEvent, 2000);
		if( ret == WAIT_FAILED )
			break;

		EnterCriticalSection(&m_Lock);

		map<tstring, CNDCloudCoursewareObjects*>::iterator itr;
		for(itr = m_mapCoursewareObjects.begin(); itr != m_mapCoursewareObjects.end(); )
		{
			CNDCloudCoursewareObjects* pCoursewareObject = itr->second;
			bool res = pCoursewareObject->PollPackState();
			if( !res )
			{
				// already downloading file can remove this now
				delete pCoursewareObject;
				pCoursewareObject = NULL;
				
				m_mapCoursewareObjects.erase(itr++);
			}
			else
				itr++;
		}

		LeaveCriticalSection(&m_Lock);
	}
}

DWORD WINAPI CNDCloudCoursewareObjectsManager::PollPackStateThread(LPARAM lParam)
{
	CNDCloudCoursewareObjectsManager* pThis = (CNDCloudCoursewareObjectsManager*)lParam;
	pThis->PollPackState();

	return 0;
}

BOOL CNDCloudCoursewareObjectsManager::DownloadCoursewareObjects(tstring strGuid, DWORD dwUserId, CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressDelegate,void* pUserData)
{
	CNDCloudCoursewareObjects* pQCoursewareObject = new CNDCloudCoursewareObjects(strGuid, dwUserId, OnCompleteDelegate, OnProgressDelegate,pUserData);
	if( pQCoursewareObject == NULL )
		return FALSE;
	
	EnterCriticalSection(&m_Lock);
	m_mapCoursewareObjects[strGuid] = pQCoursewareObject;
	LeaveCriticalSection(&m_Lock);

	SetEvent(m_hEvent);

	return TRUE;
}

BOOL CNDCloudCoursewareObjectsManager::CancelDownload(tstring strGuid)
{
	EnterCriticalSection(&m_Lock);

	map<tstring, CNDCloudCoursewareObjects*>::iterator itr = m_mapCoursewareObjects.find(strGuid);
	if( itr != m_mapCoursewareObjects.end() )
	{
		CNDCloudCoursewareObjects* pCoursewareObject = itr->second;
		pCoursewareObject->Cancel();
	}

	LeaveCriticalSection(&m_Lock);

	return TRUE;
}

BOOL CNDCloudCoursewareObjectsManager::PauseDownload(tstring strQuestionGuid, CHttpDelegateBase *pDelegate /*= NULL*/)
{
	EnterCriticalSection(&m_Lock);

	map<tstring, CNDCloudCoursewareObjects*>::iterator itr = m_mapCoursewareObjects.find(strQuestionGuid);
	if( itr != m_mapCoursewareObjects.end() )
	{
		CNDCloudCoursewareObjects* pCoursewareObject = itr->second;
		pCoursewareObject->Pause(pDelegate);
	}

	LeaveCriticalSection(&m_Lock);

	return TRUE;
}

BOOL CNDCloudCoursewareObjectsManager::ResumeDownload(tstring strQuestionGuid)
{
	EnterCriticalSection(&m_Lock);

	map<tstring, CNDCloudCoursewareObjects*>::iterator itr = m_mapCoursewareObjects.find(strQuestionGuid);
	if( itr != m_mapCoursewareObjects.end() )
	{
		CNDCloudCoursewareObjects* pCoursewareObject = itr->second;
		pCoursewareObject->Resume();
	}

	LeaveCriticalSection(&m_Lock);

	return TRUE;
}


BOOL CNDCloudCoursewareObjectsManager::GetCoursewareObjectsResType( LPCTSTR szCoursewareObjectsResDescription )
{
 	for(int i = 0;i<sizeof(tagCoursewareObjectsResTypeList)/sizeof(tagCoursewareObjectsResTypeList[0]);i++)
 	{
 		if(_tcsicmp(tagCoursewareObjectsResTypeList[i] , szCoursewareObjectsResDescription) == 0 )
 		{
 			return TRUE;
 		}
 	}
	return FALSE;
}

DWORD CNDCloudCoursewareObjectsManager::PackCoursewareObjects( tstring strQuestionGuid, DWORD dwUserId, CHttpDelegateBase& OnCompleteDelegate )
{
	tstring strUrl = NDCloudComposeUrlPackCoursewareObjects(strQuestionGuid, dwUserId);
	string strHeader = NDCloudUser::GetInstance()->GetAuthorizationHeader(Str2Ansi(EduPlatformHost), Str2Ansi(strUrl), "POST");

	DWORD dwTaskId = NDCloudDownloadEx(strUrl, Ansi2Str(strHeader), _T("POST"), "",  OnCompleteDelegate);
	return dwTaskId;
}
