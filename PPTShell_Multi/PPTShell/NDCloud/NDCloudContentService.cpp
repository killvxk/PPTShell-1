//-----------------------------------------------------------------------
// FileName:				NDCloudContentService.h
//
// Desc:
//------------------------------------------------------------------------
#include "stdafx.h"
#include "Util/Util.h"
#include "json/json.h"
#include "NDCloudUser.h"
#include "NDCloudContentService.h"

#define EduServiceGuid								"cc0d47a0-54d9-442e-bf47-9fb0ca4e6bf8"
#define UserCenterHost								_T("aqapi.101.com")
#define BearerTokensUrl								_T("/v0.93/bearer_tokens")

#define EduPlatformHostCS							_T("cs.101.com")
#define EduPlatformHostLC							_T("esp-lifecycle.web.sdp.101.com")
#define EduPlatformSessionUrl						_T("/v0.1/sessions")
#define EduPlatformDeleteCSDentry					_T("/v0.1/static/%s?session=%s")
#define EduPlatformCreateCSDirectory				_T("/v0.1/dentries?session=%s&rename=false")
#define EduPlatformDeleteLCDentry					_T("/v0.6/%s/%s")
#define EduPlatformRenameLCDentry					_T("/v0.6/%s/%s")


//---------------------------------------------------
// CNDCloudContentService
//
CNDCloudContentService::CNDCloudContentService()
{
	m_bSuccess			= FALSE;
	m_dwServiceId		= 0;

}

CNDCloudContentService::~CNDCloudContentService()
{

}

void CNDCloudContentService::SetServiceId(DWORD dwServiceId)
{
	m_dwServiceId = dwServiceId;
}

DWORD CNDCloudContentService::GetServiceId()
{
	return m_dwServiceId;
}

BOOL CNDCloudContentService::IsSuccess()
{
	return m_bSuccess;
}

tstring CNDCloudContentService::GetErrorCode()
{
	return m_strErrorCode;
}

tstring CNDCloudContentService::GetErrorMessage()
{
	return m_strErrorMessage;
}

CHttpEventSource* CNDCloudContentService::GetUserDelegate()
{
	return &m_OnUserDelegate;
}

void* CNDCloudContentService::GetUserData()
{
	return m_pUserData;
}

//
// CNDCloudContentDeleteService
//
CNDCloudContentDeleteService::CNDCloudContentDeleteService(DWORD dwUserId, tstring strGuid, 
														   int nContentType, tstring strFileServerPath, 
														   CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnUserDelegate,void* pUserData)
{
	m_bSuccess				= FALSE;
	m_dwUserId				= dwUserId;
	m_strLCGuid				= strGuid;
	m_nContentType			= nContentType;
	m_strFileServerPath		= strFileServerPath;
	m_pUserData = pUserData;

	m_OnCompleteDelegate.clear();

	if( OnCompleteDelegate.GetFn() != NULL )
		m_OnCompleteDelegate += OnCompleteDelegate;

	m_OnUserDelegate.clear();

	if( OnUserDelegate.GetFn() != NULL )
		m_OnUserDelegate += OnUserDelegate;

}

CNDCloudContentDeleteService::~CNDCloudContentDeleteService()
{

}

//
// Delete cloud file
//
BOOL CNDCloudContentDeleteService::StartService()
{
	OnCSFileDeleted(NULL);

	//
	//Json::Value root;

	//root["path"]		= Str2Utf8(m_strFileServerPath);
	//root["service_id"]	= EduServiceGuid;
	//root["uid"]			= (int)m_dwUserId;
	//root["role"]		= "user";

	//Json::FastWriter writter;
	//string strPost = writter.write(root);


	//// obtain bearer token
	//tstring strToken = NDCloudUser::GetInstance()->GetBearerToken();
	//if( strToken == _T("") )
	//	return FALSE;

	//tstring strAuthorization = "Authorization: Bearer \"";
	//strAuthorization += strToken;
	//strAuthorization += "\"";

	//tstring strHeader = _T("Content-Type: application/json");
	//strHeader += _T("\r\n");
	//strHeader += Ansi2Str(strAuthorization);


	//DWORD dwTaskId = HttpDownloadManager::GetInstance()->AddTask(EduPlatformHostCS,
	//	EduPlatformSessionUrl, 
	//	strHeader.c_str(),
	//	_T("POST"),
	//	strPost.c_str(),
	//	INTERNET_DEFAULT_HTTP_PORT,
	//	MakeHttpDelegate(this, &CNDCloudContentDeleteService::OnSessionObtained),
	//	MakeHttpDelegate(NULL),
	//	MakeHttpDelegate(NULL) );

	//if( dwTaskId == 0 )
	//	return FALSE;
 
	return TRUE;
}



//
// Session obtained
//
bool CNDCloudContentDeleteService::OnSessionObtained(void *param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pData[pNotify->nDataSize] = '\0';

	string str = Utf8ToAnsi(pNotify->pData);

	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(str, root);
	if( !res )
	{
		m_bSuccess = FALSE;

		m_strErrorMessage = _T("ÍøÂç´íÎó");

		pNotify->pUserData = this;
		pNotify->dwErrorCode = 1;
		m_OnCompleteDelegate(param);
		return false;
	}

	if( root["session"].isNull() )
	{
		m_bSuccess = FALSE;

		if( !root["code"].isNull() )
			m_strErrorCode = root["code"].asCString();

		if( !root["message"].isNull() )
		{
			m_strErrorMessage = root["message"].asCString();

			int pos = m_strErrorMessage.find(_T("info="));
			if( pos != -1 )
			{
				int pos1 = m_strErrorMessage.find(_T(":"), pos);
				if( pos1 != -1 )
					m_strErrorMessage = m_strErrorMessage.substr(pos+6, pos1-pos-6);
				else
					m_strErrorMessage = _T("");
			}
			else
				m_strErrorMessage = _T("");
		}

		pNotify->pUserData = this;
		pNotify->dwErrorCode = 1;
		m_OnCompleteDelegate(param);
		return false;
	}

	tstring strSessionId = root["session"].asCString();

	//
	tstring strFilePath = m_strFileServerPath.substr(1);

	// delete this file
	TCHAR szUrl[1024];
	_stprintf_s(szUrl, EduPlatformDeleteCSDentry, strFilePath.c_str(), strSessionId.c_str());

	// obtain bearer token
	tstring strToken = NDCloudUser::GetInstance()->GetBearerToken();
	if( strToken == _T("") )
		return FALSE;

	tstring strAuthorization = "Authorization: Bearer \"";
	strAuthorization += strToken;
	strAuthorization += "\"";

	tstring strHeader = _T("Content-Type: application/json");
	strHeader += _T("\r\n");
	strHeader += Ansi2Str(strAuthorization);


	DWORD dwTaskId = HttpDownloadManager::GetInstance()->AddTask(EduPlatformHostCS,
		szUrl, 
		strHeader.c_str(),
		_T("DELETE "),
		_T(""),
		INTERNET_DEFAULT_HTTP_PORT,
		MakeHttpDelegate(this, &CNDCloudContentDeleteService::OnCSFileDeleted),
		MakeHttpDelegate(NULL),
		MakeHttpDelegate(NULL) );

	if( dwTaskId == 0 )
		return false;
	
	return true;
}
 
//
// Cloud file deleted
//
bool CNDCloudContentDeleteService::OnCSFileDeleted(void* param)
{
	//THttpNotify* pNotify = (THttpNotify*)param;
	//pNotify->pData[pNotify->nDataSize] = '\0';

	//string str = Utf8ToAnsi(pNotify->pData);
	//if( str != _T("") )
	//{
	//	Json::Value root;
	//	Json::Reader reader;
	//	
	//	bool res = reader.parse(str, root);
	//	if( !res )
	//	{
	//		m_bSuccess = FALSE;
	//		m_strErrorMessage = _T("ÍøÂç´íÎó");

	//		pNotify->pUserData = this;
	//		m_OnCompleteDelegate(param);
	//		return false;
	//	}

	//	// "CS/DENTRY_NOT_FOUND"
	//	if( !root["code"].isNull() )
	//	{
	//		string strCode = root["code"].asCString();
	//		if( strCode != "CS/DENTRY_NOT_FOUND")
	//		{
	//			m_bSuccess = FALSE;

	//			pNotify->pUserData = this;
	//			m_OnCompleteDelegate(param);
	//			return false;
	//		}
	//	}
	//	else
	//	{
	//		m_bSuccess = FALSE;

	//		pNotify->pUserData = this;
	//		m_OnCompleteDelegate(param);
	//		return false;
	//	}
	//}

	// delete LC database's data
	tstring strType;

	if( m_nContentType == CONTENT_ASSETS )
		strType = _T("assets");
	else if( m_nContentType == CONTENT_COURSEWARE )
		strType = _T("coursewares");
	else if( m_nContentType == CONTENT_QUESTIONS )
		strType = _T("questions");
	else if( m_nContentType == CONTENT_COUSEWAREOBJECTS )
		strType = _T("coursewareobjects");

	//
	TCHAR szUrl[1024];
	_stprintf_s(szUrl, EduPlatformDeleteLCDentry, strType.c_str(), m_strLCGuid.c_str());

	// Authorization header
	string strAuthorization = NDCloudUser::GetInstance()->GetAuthorizationHeader(Str2Ansi(EduPlatformHostLC), Str2Ansi(szUrl), "DELETE");

	tstring strHeader = _T("Content-Type: application/json");
	strHeader += _T("\r\n");
	strHeader += Ansi2Str(strAuthorization);

	DWORD dwTaskId = HttpDownloadManager::GetInstance()->AddTask(EduPlatformHostLC,
																szUrl, 
																strHeader.c_str(),
																_T("DELETE "),
																_T(""),
																INTERNET_DEFAULT_HTTP_PORT,
																MakeHttpDelegate(this, &CNDCloudContentDeleteService::OnLCFileDeleted),
																MakeHttpDelegate(NULL),
																MakeHttpDelegate(NULL) );

	if( dwTaskId == 0 )
		return false;


	return true;
}

bool CNDCloudContentDeleteService::OnLCFileDeleted(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pData[pNotify->nDataSize] = '\0';

	string str = Utf8ToAnsi(pNotify->pData);
	
	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(str, root);
	if( !res )
	{
		m_bSuccess = FALSE;
		m_strErrorMessage = _T("ÍøÂç´íÎó");

		pNotify->pUserData = this;
		pNotify->dwErrorCode = 1;
		m_OnCompleteDelegate(param);
		return false;
	}
	
	if( root["process_code"].isNull() )
	{
		m_bSuccess = FALSE;

		pNotify->pUserData = this;
		pNotify->dwErrorCode = 1;
		m_OnCompleteDelegate(param);
		return false;
	}

	string strCode = root["process_code"].asCString();
	if( strCode != "LC/DELETE_RESOURCE_SUCCESS" )
	{
		m_bSuccess = FALSE;

		pNotify->pUserData = this;
		pNotify->dwErrorCode = 1;
		m_OnCompleteDelegate(param);
		return false;
	}

	m_bSuccess = TRUE;
	pNotify->pUserData = this;
	m_OnCompleteDelegate(param);

	return true;
}


//--------------------------------------------------------------
// CNDCloudContentRenameService
//

CNDCloudContentRenameService::CNDCloudContentRenameService( tstring strGuid, tstring strTitle, tstring strJson, int nContentType, 
														   CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnUserDelegate, void* pUserData/*=NULL*/ )
{
	m_strLCGuid		= strGuid;
	m_strTitle		= strTitle;
	m_strJson		= strJson;
	m_nContentType	= nContentType;
	m_pUserData = pUserData;

	if( OnCompleteDelegate.GetFn() != NULL )
		m_OnCompleteDelegate += OnCompleteDelegate;

	if( OnUserDelegate.GetFn() != NULL )
		m_OnUserDelegate += OnUserDelegate;
}

CNDCloudContentRenameService::~CNDCloudContentRenameService()
{

}

BOOL CNDCloudContentRenameService::StartService()
{
	TCHAR szUrl[1024];

	tstring strType;
	if( m_nContentType == CONTENT_ASSETS )
		strType = _T("assets");
	else
		strType = _T("coursewares");

	_stprintf_s(szUrl, EduPlatformRenameLCDentry, strType.c_str(), m_strLCGuid.c_str());

	//
	string strAuthorization = NDCloudUser::GetInstance()->GetAuthorizationHeader(EduPlatformHostLC, Str2Ansi(szUrl), "PUT");
	
	tstring strHeader = _T("Content-Type: application/json");
	strHeader += _T("\r\n");
	strHeader += Ansi2Str(strAuthorization);


	//
	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(m_strJson, root);
	if( !res )
		return FALSE;

	root["title"] = m_strTitle;

	Json::FastWriter writter;
	string strPost = writter.write(root);
	

	DWORD dwTaskId = HttpDownloadManager::GetInstance()->AddTask(EduPlatformHostLC, 
												szUrl, 
												strHeader.c_str(),
												_T("PUT"),
												AnsiToUtf8(strPost).c_str(),
												INTERNET_DEFAULT_HTTP_PORT,
												MakeHttpDelegate(this, &CNDCloudContentRenameService::OnTitleRenamed),
												MakeHttpDelegate(NULL),
												MakeHttpDelegate(NULL));
	if( dwTaskId == 0 )
		return FALSE;
	
	return TRUE;
}

bool CNDCloudContentRenameService::OnTitleRenamed(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pData[pNotify->nDataSize] = '\0';

	string str = Utf8ToAnsi(pNotify->pData);

	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(str, root);
	if( !res )
	{
		m_bSuccess = FALSE;
		m_strErrorMessage = _T("ÍøÂç´íÎó");

		pNotify->pUserData = this;
		pNotify->dwErrorCode = 1;
		m_OnCompleteDelegate(param);

		return false;
	}

	// code
	if( !root["code"].isNull() )
	{
		m_bSuccess = FALSE;

		m_strErrorCode = root["code"].asCString();
		m_strErrorMessage = root["message"].asCString();

		pNotify->pUserData = this;
		pNotify->dwErrorCode = 1;
		m_OnCompleteDelegate(param);
		return false;
	}


	m_bSuccess = TRUE;
	pNotify->pUserData = this;
	m_OnCompleteDelegate(param);

	return true;
}

//--------------------------------------------------------------
// CNDCloudContentCreateFolderService
//
CNDCloudContentCreateDirectoryService::CNDCloudContentCreateDirectoryService(DWORD dwUserId, tstring strFileServerPath, 
																			 tstring strRelativeDirPath,
																			CHttpDelegateBase& OnCompleteDelegate, 
																			CHttpDelegateBase& OnUserDelegate)
{	
	m_dwUserId				= dwUserId;
	m_strFileServerPath		= strFileServerPath;
	m_strRelativeDirPath	= strRelativeDirPath;

	m_OnCompleteDelegate.clear();

	if( OnCompleteDelegate.GetFn() != NULL )
		m_OnCompleteDelegate += OnCompleteDelegate;

	m_OnUserDelegate.clear();

	if( OnUserDelegate.GetFn() != NULL )
		m_OnUserDelegate += OnUserDelegate;
}

CNDCloudContentCreateDirectoryService::~CNDCloudContentCreateDirectoryService()
{

}

BOOL CNDCloudContentCreateDirectoryService::StartService()
{
	Json::Value root;

	root["path"]		= Str2Utf8(m_strFileServerPath);
	root["service_id"]	= EduServiceGuid;
	root["uid"]			= (int)m_dwUserId;
	root["role"]		= "user";

	Json::FastWriter writter;
	string strPost = writter.write(root);


	CHttpDownloadManager* pHttpManager = HttpDownloadManager::GetInstance();
	if( pHttpManager == NULL )
		return false;

	// obtain bearer token
	tstring strToken = NDCloudUser::GetInstance()->GetBearerToken();
	if( strToken == _T("") )
		return FALSE;

	tstring strAuthorization = "Authorization: Bearer \"";
	strAuthorization += strToken;
	strAuthorization += "\"";

	tstring strHeader = _T("Content-Type: application/json");
	strHeader += _T("\r\n");
	strHeader += Ansi2Str(strAuthorization);


	DWORD dwTaskId = HttpDownloadManager::GetInstance()->AddTask(EduPlatformHostCS,
		EduPlatformSessionUrl, 
		strHeader.c_str(),
		_T("POST"),
		strPost.c_str(),
		INTERNET_DEFAULT_HTTP_PORT,
		MakeHttpDelegate(this, &CNDCloudContentCreateDirectoryService::OnSessionObtained),
		MakeHttpDelegate(NULL),
		MakeHttpDelegate(NULL) );

	if( dwTaskId == 0 )
		return FALSE;

	return TRUE;
}

bool CNDCloudContentCreateDirectoryService::OnSessionObtained(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pData[pNotify->nDataSize] = '\0';
	string str = Utf8ToAnsi(pNotify->pData);

	bool result = false;
	Json::Reader reader;
	Json::Value root;
	bool res = reader.parse(str, root);
	if(res)
	{
		if(! root["session"].isNull() )
		{
			tstring strSessionId = root["session"].asCString();
			// create directory
			TCHAR szUrl[1024];
			_stprintf_s(szUrl, EduPlatformCreateCSDirectory, strSessionId.c_str());
			// obtain bearer token
			tstring strToken = NDCloudUser::GetInstance()->GetBearerToken();
			if( strToken != _T("") )
			{
				tstring strAuthorization = "Authorization: Bearer \"";
				strAuthorization += strToken;
				strAuthorization += "\"";
				tstring strHeader = _T("Content-Type: application/json");
				strHeader += _T("\r\n");
				strHeader += Ansi2Str(strAuthorization);
				// split path
				root.clear();
				tstring strPath = m_strFileServerPath;
				tstring strName;
				int pos = m_strRelativeDirPath.rfind('/');
				if( pos != -1 )
				{
					strPath += _T("/") + m_strRelativeDirPath.substr(0, pos);
					strName = m_strRelativeDirPath.substr(pos+1);
				}
				else
				{
					strName = m_strRelativeDirPath;
				}
				root["path"] = Str2Utf8(strPath);
				root["name"] = Str2Utf8(strName);
				Json::FastWriter writter;
				string strPost = writter.write(root);
				DWORD dwTaskId = HttpDownloadManager::GetInstance()->AddTask(EduPlatformHostCS,
					szUrl, 
					strHeader.c_str(),
					_T("POST"),
					Ansi2Str(strPost).c_str(),
					INTERNET_DEFAULT_HTTP_PORT,
					MakeHttpDelegate(this, &CNDCloudContentCreateDirectoryService::OnDirectoryCreated),
					MakeHttpDelegate(NULL),
					MakeHttpDelegate(NULL) );
				if( dwTaskId != 0 )
				{
					result = true;
				}
			}
		}
		else
		{
			if( !root["code"].isNull() )
				m_strErrorCode = root["code"].asCString();
			if( !root["message"].isNull() )
				m_strErrorMessage = root["message"].asCString();
		}
	}
	if(!result)
	{
		pNotify->pUserData = this;
		pNotify->dwErrorCode = 1;
		m_OnCompleteDelegate(param);
		return false;
	}
	return true;
}

bool CNDCloudContentCreateDirectoryService::OnDirectoryCreated(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pData[pNotify->nDataSize] = '\0';
	string str = Utf8ToAnsi(pNotify->pData);
	Json::Reader reader;
	Json::Value root;
	bool res = reader.parse(str, root);
	if( !res )
	{
		m_bSuccess = FALSE;
		pNotify->pUserData = this;
		pNotify->dwErrorCode = 1;
		m_OnCompleteDelegate(param);
		return false;
	}
	else
	{
		m_bSuccess = TRUE;
		pNotify->pUserData = this;
		m_OnCompleteDelegate(param);
		return true;
	}
}

//--------------------------------------------------------------
// CNDCloudContentServiceManager
//
CNDCloudContentServiceManager::CNDCloudContentServiceManager()
{
	InitializeCriticalSection(&m_Lock);

}

CNDCloudContentServiceManager::~CNDCloudContentServiceManager()
{
	DeleteCriticalSection(&m_Lock);
}

BOOL CNDCloudContentServiceManager::DeleteCloudFile(DWORD dwUserId, tstring strGuid, int nContentType, tstring strFileServerPath, 
													CHttpDelegateBase& delegate,void* pUserData)
{
	int pos = strFileServerPath.find(_T("${ref-path}"));
	if( pos != -1 )
	{
		int len = _tcslen(_T("${ref-path}"));
		strFileServerPath = strFileServerPath.substr(pos+len);
	}


	// add service
	CNDCloudContentService* pService = new CNDCloudContentDeleteService(dwUserId, strGuid, nContentType, strFileServerPath, 
									MakeHttpDelegate(this, &CNDCloudContentServiceManager::OnComplete), delegate,pUserData);
	if( pService == NULL )
		return FALSE;

	DWORD dwServiceId = GetCycleCount();

	pService->SetServiceId(dwServiceId);

	EnterCriticalSection(&m_Lock);
	m_mapContentServices[dwServiceId] = pService;
	LeaveCriticalSection(&m_Lock);

	BOOL res = pService->StartService();
	if( !res )
		return FALSE;

	return TRUE;
}

BOOL CNDCloudContentServiceManager::RenameCloudFile( tstring strGuid, tstring strTitle, tstring strJson, int nContentType, CHttpDelegateBase& delegate,void* pUserData/*=NULL*/ )
{
	CNDCloudContentService* pService = new CNDCloudContentRenameService(strGuid, strTitle, strJson, nContentType,   
															MakeHttpDelegate(this, &CNDCloudContentServiceManager::OnComplete), delegate, pUserData);
	if( pService == NULL )
		return FALSE;

	DWORD dwServiceId = GetCycleCount();

	pService->SetServiceId(dwServiceId);

	EnterCriticalSection(&m_Lock);
	m_mapContentServices[dwServiceId] = pService;
	LeaveCriticalSection(&m_Lock);

	BOOL res = pService->StartService();
	if( !res )
		return FALSE;


	return TRUE;
}



BOOL CNDCloudContentServiceManager::CreateCloudDirectory(DWORD dwUserId, tstring strFileServerPath, tstring strRelativeDirPath, CHttpDelegateBase& delegate)
{
	CNDCloudContentService* pService = new CNDCloudContentCreateDirectoryService(dwUserId, strFileServerPath, strRelativeDirPath,
										MakeHttpDelegate(this, &CNDCloudContentServiceManager::OnComplete), delegate);
	if( pService == NULL )
		return FALSE;

	DWORD dwServiceId = GetCycleCount();

	pService->SetServiceId(dwServiceId);

	EnterCriticalSection(&m_Lock);
	m_mapContentServices[dwServiceId] = pService;
	LeaveCriticalSection(&m_Lock);

	BOOL res = pService->StartService();
	if( !res )
		return FALSE;

	return TRUE;
}

bool CNDCloudContentServiceManager::OnComplete(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;

	CNDCloudContentService* pService = (CNDCloudContentService*)pNotify->pUserData;
	//BOOL bIsSuccess = FALSE;
	//tstring strErrorMessage;
	//tstring strErrorCode;
	//if( pService != NULL )
	//{
	//	bIsSuccess = pService->IsSuccess();
	//	strErrorCode = pService->GetErrorCode();
	//	strErrorMessage = pService->GetErrorMessage();
	//}
	//// error
	//CStream ErrorInfoStream(512);
	//ErrorInfoStream.WriteString(strErrorCode);
	//ErrorInfoStream.WriteString(strErrorMessage);
	//ErrorInfoStream.ResetCursor();

	//THttpNotify notify;
	//notify.dwErrorCode = bIsSuccess ? 0 : 1;
	//notify.pUserData = &ErrorInfoStream;

	CHttpEventSource* pHttpEventSource = pService->GetUserDelegate();
	(*pHttpEventSource)(pNotify);

	// 
	if( pService != NULL )
	{
		EnterCriticalSection(&m_Lock);

		DWORD dwServiceId = pService->GetServiceId();
		m_mapContentServices.erase(dwServiceId);

		LeaveCriticalSection(&m_Lock);

		delete pService;
		pService = NULL;
	}


	return true;
}
