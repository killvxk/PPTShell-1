//-----------------------------------------------------------------------
// FileName:				NDCloudQuestion.cpp
//
// Desc:
//------------------------------------------------------------------------
#include "stdafx.h"
#include "NDCloudAPI.h"
#include "NdCloudUpload.h"
#include "Http/HttpUploadManager.h"
#include "Util/Util.h"
#include "NDCloudContentService.h"
#include "NDCloudUser.h"

#define EduServiceGuid								"cc0d47a0-54d9-442e-bf47-9fb0ca4e6bf8"
#define EduPlatformHostCS							_T("cs.101.com")
#define EduPlatformSessionUrl						_T("/v0.1/sessions")

#define EduPlatformHostLC							_T("esp-lifecycle.web.sdp.101.com")
#define EduPlatformLCUploadAssets					_T("/v0.6/assets/%s/uploadurl?uid=%u&renew=false")
#define EduPlatformLCUploadCoursewares				_T("/v0.6/coursewares/%s/uploadurl?uid=%u&renew=false")
#define EduPlatformLCUploadQuestions				_T("/v0.6/questions/%s/uploadurl?uid=%u&renew=true")
#define EduPlatformLCUploadCoursewareobjects		_T("/v0.6/coursewareobjects/%s/uploadurl?uid=%u&renew=true")


//-----------------------------------------------------------
// CNDCloudUpload
//
CNDCloudUploadTask::CNDCloudUploadTask()
{
	m_pUserData=NULL;
}

CNDCloudUploadTask::~CNDCloudUploadTask()
{

}

BOOL CNDCloudUploadTask::IsSuccess()
{
	return m_bSuccess;
}

tstring CNDCloudUploadTask::GetErrorCode()
{
	return m_strErrorCode;
}

tstring CNDCloudUploadTask::GetErrorMessage()
{
	return m_strErrorMessage;
}

tstring CNDCloudUploadTask::GetGuid()
{
	return m_strGuid;
}

tstring CNDCloudUploadTask::GetServerPath()
{
	return m_strServerFilePath;
}

tstring CNDCloudUploadTask::GetServerFolderPath()
{
	return m_strServerFolderPath;
}

DWORD CNDCloudUploadTask::GetUploadId()
{
	return m_dwUploadId;
}

void CNDCloudUploadTask::SetUploadId(DWORD dwId)
{
	m_dwUploadId = dwId;
}

tstring CNDCloudUploadTask::ComposeObtainServerPathUrl(DWORD dwUserId, tstring strGuid)
{
	if( strGuid == _T("") )
		strGuid = _T("none");

	TCHAR szUrl[1024];
	switch( m_nUploadType )
	{
	case UPLOAD_ASSETS:
		_stprintf_s(szUrl, EduPlatformLCUploadAssets, strGuid.c_str(), dwUserId);
		break;

	case UPLOAD_COURSEWARE:
		_stprintf_s(szUrl, EduPlatformLCUploadCoursewares, strGuid.c_str(), dwUserId);
		break;

	case UPLOAD_QUESTIONS:
		_stprintf_s(szUrl, EduPlatformLCUploadQuestions, strGuid.c_str(), dwUserId);
		break;

	case UPLOAD_COURSEWAREOBJECTS:
		_stprintf_s(szUrl, EduPlatformLCUploadCoursewareobjects, strGuid.c_str(), dwUserId);
		break;

	default:
		return _T("");
	}
	
	return szUrl;
}

bool CNDCloudUploadTask::OnUploadCompleted(void *param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pUserData = this;

	if( pNotify->dwErrorCode != 0 )
		m_bSuccess = FALSE;
	else
		m_bSuccess = TRUE;

	if( m_OnComplete )
		m_OnComplete(param);

	return true;
}

bool CNDCloudUploadTask::OnUploadProgress(void *param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pUserData = this;


	m_bSuccess = TRUE;

	if( m_OnProgress )
		m_OnProgress(param);

	return true;
}

CHttpEventSource* CNDCloudUploadTask::GetUserCompleteDelegate()
{
	return &m_OnUserComplete;
}

CHttpEventSource* CNDCloudUploadTask::GetUserProgressDelegate()
{
	return &m_OnUserProgress;
}

void* CNDCloudUploadTask::GetUserData()
{
	return m_pUserData;
}

//-----------------------------------------------------------
// CNDCloudUploadFile
//
CNDCloudUploadFileTask::CNDCloudUploadFileTask(int nUploadType,
											   DWORD dwUserId, 
											   tstring strFilePath, 
											   CHttpDelegateBase& OnCompleteDelegate, 
											   CHttpDelegateBase& OnProgressDelegate,
											   CHttpDelegateBase& OnUserCompleteDelegate, 
											   CHttpDelegateBase& OnUserProgressDelegate,
											   tstring strGuid /*= _T("")*/,
											   tstring strServerFilePath /*= _T("")*/)
											  
{
	m_bSuccess				= FALSE;
	m_nUploadType			= nUploadType;
	m_dwUserId				= dwUserId;
	m_strGuid				= strGuid;
	m_strFilePath			= strFilePath;
	m_strServerFilePath		= strServerFilePath;
	
	if( OnCompleteDelegate.GetFn() != NULL )
		m_OnComplete += OnCompleteDelegate;

	if( OnProgressDelegate.GetFn() != NULL )
		m_OnProgress += OnProgressDelegate;

	// user delegate
	if( OnUserCompleteDelegate.GetFn() != NULL )
		m_OnUserComplete += OnUserCompleteDelegate;

	if( OnUserProgressDelegate.GetFn() != NULL )
		m_OnUserProgress += OnUserProgressDelegate;

	m_bReplaceFile = m_strServerFilePath != _T("") ? TRUE : FALSE;
	
}

CNDCloudUploadFileTask::~CNDCloudUploadFileTask()
{

}

void CNDCloudUploadFileTask::StartUpload()
{
	CHttpDownloadManager* pHttpManager = HttpDownloadManager::GetInstance();
	if( pHttpManager == NULL )
		return;

	// check whether we only want to replace file
	tstring strUrl = ComposeObtainServerPathUrl(m_dwUserId, m_strGuid);

	pHttpManager->AddTask(EduPlatformHostLC, strUrl.c_str(), _T(""), _T("GET"), "", 80, 
								MakeHttpDelegate(this, &CNDCloudUploadFileTask::OnServerPathObtained), 
								MakeHttpDelegate(NULL), 
								MakeHttpDelegate(NULL), 
								FALSE);
	

}

bool CNDCloudUploadFileTask::OnServerPathObtained( void * pParam )
{
	THttpNotify* pNotify = (THttpNotify*)pParam;

	pNotify->pData[pNotify->nDataSize] = '\0';
	pNotify->pUserData = this;

	string str = Utf8ToAnsi(pNotify->pData);

	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(str, root);
	if( !res )
	{
		m_bSuccess = FALSE;
		m_OnComplete(pParam);
		return false;
	}
	
	tstring strUrl;
	tstring strUploadGuid;
	tstring strSessionId;
	tstring strServerPath;

	if( !root["access_url"].isNull() )
		strUrl = Ansi2Str(root["access_url"].asCString());

	if( !root["uuid"].isNull() )
		strUploadGuid = Ansi2Str(root["uuid"].asCString());

	if( !root["session_id"].isNull() )
		strSessionId = Ansi2Str(root["session_id"].asCString());

	if( !root["dist_path"].isNull() )
		strServerPath = Ansi2Str(root["dist_path"].asCString());

	if( m_strGuid == _T("") )
		m_strGuid = strUploadGuid;

	if( m_strServerFilePath == _T("") )
		m_strServerFilePath = strServerPath;

	m_strServerFolderPath = strServerPath;

	//
	// upload this file
	//
	TCHAR szHost[128];
	TCHAR szUrlPath[1024];
	if(_stscanf_s(strUrl.c_str(), _T("http://%[^/]%s"), szHost, _countof(szHost) - 1, szUrlPath , _countof(szUrlPath) - 1) != 2)
	{
		m_bSuccess = FALSE;
		m_OnComplete(pParam);
		return false;
	}
	
	tstring strHost = szHost;
	tstring strUrlPath = szUrlPath;
	strUrlPath += _T("?session=");
	strUrlPath += strSessionId;

	DWORD dwTaskId = HttpUploadManager::GetInstance()->AddUploadTask(strHost.c_str(), 
													strUrlPath.c_str(), 
													m_strFilePath.c_str(), 
													m_strServerFilePath.c_str(), 
													_T("POST"), 
													INTERNET_DEFAULT_HTTP_PORT, 
													MakeHttpDelegate(this, &CNDCloudUploadFileTask::OnUploadCompleted), 
													MakeHttpDelegate(this, &CNDCloudUploadFileTask::OnUploadProgress),
													TRUE,
													m_bReplaceFile);

	if( dwTaskId == 0 )
		return false;

	return true;
}

 
//-----------------------------------------------------------
// CNDCloudUploadFolder
//
CNDCloudUploadFolderTask::CNDCloudUploadFolderTask(int nUploadType, DWORD dwUserId, tstring strResType,tstring strTitle, tstring strGuid, tstring strFolderPath, 
										   CHttpDelegateBase &OnCompleteDelegate, CHttpDelegateBase& OnProgressDelegate,void* pUserData)
{
	m_nUploadType		= nUploadType;
	m_dwUserId			= dwUserId;
	m_strResType		= strResType;
	m_strGuid			= strGuid;
	m_nFolderCreated	= 0;
	m_nFileUploaded		= 0;
	m_strFolderPath		= strFolderPath;
	m_strTitle			= strTitle;
	m_pUserData = pUserData;

	if( OnCompleteDelegate.GetFn() != NULL )
		m_OnComplete += OnCompleteDelegate;

	if( OnProgressDelegate.GetFn() != NULL )
		m_OnProgress += OnProgressDelegate;

}

CNDCloudUploadFolderTask::~CNDCloudUploadFolderTask()
{

}

void CNDCloudUploadFolderTask::StartUpload()
{ 
	// get all file paths
	GetAllFilePaths(m_strFolderPath.c_str(), m_mapFilePaths);

	// find all deepest folder
	vector<tstring> vecFolderPaths;
	vecFolderPaths.clear();

	GetDeepestFolders(m_strFolderPath.c_str(), vecFolderPaths);

	// convert to relative path
	m_vecFolderRelativePaths.clear();

	for(int i = 0; i < vecFolderPaths.size(); i++)
	{
		tstring strDir = vecFolderPaths[i];
		int pos = strDir.find(m_strFolderPath);
		if( pos == -1 )
			continue;

		tstring strRelativePath = strDir.substr(m_strFolderPath.length()+1);
		
		// translate slash
		for(int j = 0; j < strRelativePath.length(); j++)
		{
			if( strRelativePath.at(j) == _T('\\') )
				strRelativePath.replace(j, 1, _T("/"));
		}

		m_vecFolderRelativePaths.push_back(strRelativePath);
	}

	// obtain server path to save this folder
	tstring strUrl = ComposeObtainServerPathUrl(m_dwUserId, m_strGuid);
	DWORD dwTaskId = HttpDownloadManager::GetInstance()->AddTask(EduPlatformHostLC, strUrl.c_str(), _T(""), _T("GET"), "", 80, 
											MakeHttpDelegate(this, &CNDCloudUploadFolderTask::OnServerPathObtained), 
											MakeHttpDelegate(NULL), 
											MakeHttpDelegate(NULL), 
											FALSE,FALSE,0,m_pUserData);
}

bool CNDCloudUploadFolderTask::OnServerPathObtained(void *pParam)
{
	THttpNotify* pNotify = (THttpNotify*)pParam;
	pNotify->pData[pNotify->nDataSize] = '\0';

	string str = Utf8ToAnsi(pNotify->pData);

	Json::Reader reader;
	Json::Value root;

	bool result = true;
	bool res = reader.parse(str, root);
	if( res )
	{
		if( root["access_url"].isNull() || root["session_id"].isNull() || root["dist_path"].isNull() )
		{
			result =  false;
		}
		else
		{
			m_strAccessUrl = Ansi2Str(root["access_url"].asCString());
			m_strSessionId = Ansi2Str(root["session_id"].asCString());
			m_strServerFilePath = Ansi2Str(root["dist_path"].asCString());
			// create these folders in server 
			if( m_vecFolderRelativePaths.size() == 0 )
			{
				UploadAllFiles();
			}
			else
			{
				for(int i = 0; i < m_vecFolderRelativePaths.size(); i++)
				{
					tstring strRelativePath = m_vecFolderRelativePaths[i];
					result = NDCloudContentServiceManager::GetInstance()->CreateCloudDirectory(m_dwUserId, m_strServerFilePath, strRelativePath, MakeHttpDelegate(this, &CNDCloudUploadFolderTask::OnFolderCreated));
				}
			}
		}
	}
	else
	{
		result =  false;
	}
	if(!result)
	{
		THttpNotify* pNotify = new THttpNotify;
		pNotify->dwErrorCode=1;
		pNotify->dwTaskId = 0;
		pNotify->pData=NULL;
		pNotify->nDataSize=0;
		pNotify->nTotalSize=0;
		pNotify->nSpeed=0;
		pNotify->nElapseTime=0;
		pNotify->nRemainTime=0;
		pNotify->fPercent=0;
		pNotify->strFilePath= _T("");
		pNotify->pDetailData=NULL;
		pNotify->pUserData=this->m_pUserData;

		m_OnComplete(pNotify);
	}
	return true;
}

bool CNDCloudUploadFolderTask::OnFolderCreated(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	if( pNotify->dwErrorCode == 0 )
	{
		m_nFolderCreated ++;
		// now can upload all files
		if( m_nFolderCreated == m_vecFolderRelativePaths.size() )
			UploadAllFiles();
	}
	else
	{
		THttpNotify notify;
		notify.dwErrorCode = 1;
		m_OnComplete(&notify);
	}
	return true;
}


BOOL CNDCloudUploadFolderTask::UploadAllFiles()
{
	TCHAR szHost[128];
	TCHAR szUrlPath[1024];
	bool result = false;
	if(_stscanf_s(m_strAccessUrl.c_str(), _T("http://%[^/]%s"), szHost, _countof(szHost) - 1, szUrlPath , _countof(szUrlPath) - 1) == 2)
	{
		// there is no any file need to be uploaded
		if( m_mapFilePaths.size() >0 )
		{
			tstring strHost = szHost;
			tstring strUrlPath = szUrlPath;
			strUrlPath += _T("?session=");
			strUrlPath += m_strSessionId;
			strUrlPath += _T("&rename=false");
			map<tstring, tstring>::iterator itr;
			for(itr = m_mapFilePaths.begin(); itr != m_mapFilePaths.end(); itr++)
			{
				tstring strFilePath = itr->first;
				tstring strServerFilePath = itr->second;
				strServerFilePath = m_strServerFilePath + strServerFilePath;
				TCHAR szName[1024];
				TCHAR * p = _tcsrchr((TCHAR *)strFilePath.c_str(), _T('\\'));
				if(p)
				{
					_tcscpy(szName, p + 1);
				}
				strServerFilePath += _T("/");
				strServerFilePath += szName;
				HttpUploadManager::GetInstance()->AddUploadTask(strHost.c_str(), 
					strUrlPath.c_str(), 
					strFilePath.c_str(), 
					strServerFilePath.c_str(), 
					_T("POST"), 
					INTERNET_DEFAULT_HTTP_PORT, 
					MakeHttpDelegate(this, &CNDCloudUploadFolderTask::OnFileUploadeded), 
					MakeHttpDelegate(NULL), TRUE, TRUE,m_pUserData);
			}
			result = true;
		}
		else
		{
			CStream stream(1024);
			stream.WriteString(_T("无文件需要上传"));
			THttpNotify notify;
			notify.dwErrorCode = 1;
			notify.pUserData = &stream;
			m_OnComplete(&notify);
		}
	}
	else
	{
		THttpNotify notify;
		notify.dwErrorCode = 1;
		m_OnComplete(&notify);
	}
	return result;
}

bool CNDCloudUploadFolderTask::OnFileUploadeded(void *param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pData[pNotify->nDataSize] = '\0';

	if(pNotify->dwErrorCode==0)
	{
		m_nFileUploaded ++;
		pNotify->pUserData = NULL;
		if( m_nFileUploaded == m_mapFilePaths.size() )
		{
			pNotify->dwErrorCode = 0;
			CStream stream(1024);
			stream.WriteInt(m_nUploadType);
			stream.WriteString(m_strResType);
			stream.WriteString(m_strGuid);
			stream.WriteString(m_strFolderPath);
			stream.WriteString(m_strServerFilePath);
			stream.WriteString(m_strTitle);
			if(m_pUserData)
			{
				CStream* pStream= (CStream*)m_pUserData;
				if(pStream)
				{
					pStream->ResetCursor();
					stream.WriteInt(pStream->ReadInt());
					stream.WriteInt(pStream->ReadInt());
					delete pStream;
				}
			}
			pNotify->pUserData = &stream;
			m_OnComplete(param);
		}
	}
	else
	{
		m_OnComplete(param);
	}	
	return true;
}

// 
// recuse to find all deepest folders
//
bool CNDCloudUploadFolderTask::GetDeepestFolders(LPCTSTR szFolderPath, vector<tstring>& vecFolderPaths)
{  
	bool bSubDirExist = false;

	TCHAR szFileFilter[512];
	TCHAR szFilePath[512];

	HANDLE		hFind  = NULL;
	WIN32_FIND_DATA fileinfo;
	memset(&fileinfo, 0, sizeof(WIN32_FIND_DATA));

	_tcscpy(szFilePath, szFolderPath);
	_tcscat(szFilePath, _T("\\"));

	_tcscpy(szFileFilter, szFilePath);
	_tcscat(szFileFilter, _T("*.*"));


	hFind = FindFirstFile(szFileFilter, &fileinfo);
	if( hFind == INVALID_HANDLE_VALUE )
		return false;

	do
	{
		if( _tcsicmp(fileinfo.cFileName, _T(".")) == 0 ||
			_tcsicmp(fileinfo.cFileName, _T("..")) == 0 )
			continue;

		if( (fileinfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY )
			continue;

		// 
		tstring strDir = szFolderPath;
		strDir += _T("\\");
		strDir += fileinfo.cFileName;

		// 
		bool res = GetDeepestFolders(strDir.c_str(), vecFolderPaths);
		if( !res )
			vecFolderPaths.push_back(strDir);

		bSubDirExist = true;
		
	}while( FindNextFile(hFind,&fileinfo) );

	FindClose(hFind);

	return bSubDirExist;
}  

bool CNDCloudUploadFolderTask::GetAllFilePaths(LPCTSTR szFolderPath, map<tstring, tstring>& mapFilePaths)
{
	TCHAR szFileFilter[512];
	TCHAR szFilePath[512];

	HANDLE		hFind  = NULL;
	WIN32_FIND_DATA fileinfo;
	memset(&fileinfo, 0, sizeof(WIN32_FIND_DATA));

	_tcscpy(szFilePath, szFolderPath);
	_tcscat(szFilePath, _T("\\"));

	_tcscpy(szFileFilter, szFilePath);
	_tcscat(szFileFilter, _T("*.*"));


	hFind = FindFirstFile(szFileFilter, &fileinfo);
	if( hFind == INVALID_HANDLE_VALUE )
		return false;

	do
	{
		if( _tcsicmp(fileinfo.cFileName, _T(".")) == 0 ||
			_tcsicmp(fileinfo.cFileName, _T("..")) == 0 )
			continue;

		tstring strDir = szFolderPath;
		strDir += _T("\\");
		strDir += fileinfo.cFileName;

		if( (fileinfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY )
			GetAllFilePaths(strDir.c_str(), mapFilePaths);
		
		else
		{
			tstring strRelativePath = szFolderPath;
			int pos = strRelativePath.find(m_strFolderPath);
			if( pos != -1 )
			{
				strRelativePath = strRelativePath.substr(m_strFolderPath.length());

				// translate slash
				for(int j = 0; j < strRelativePath.length(); j++)
				{
					if( strRelativePath.at(j) == _T('\\') )
						strRelativePath.replace(j, 1, _T("/"));
				}
			}
	
			m_mapFilePaths[strDir] = strRelativePath;
		}
		
	}while( FindNextFile(hFind,&fileinfo) );

	FindClose(hFind);

	return true;
}


//-----------------------------------------------------------
// CNDCloudUploadManager
//
CNDCloudUploadTaskManager::CNDCloudUploadTaskManager()
{
	InitializeCriticalSection(&m_Lock);
}

CNDCloudUploadTaskManager::~CNDCloudUploadTaskManager()
{
	DeleteCriticalSection(&m_Lock);
}

//
// upload file
//
BOOL CNDCloudUploadTaskManager::UploadFile(int nUploadType, 
										   DWORD dwUserId,
										   tstring strFilePath,
										   CHttpDelegateBase& OnCompleteDelegate, 
										   CHttpDelegateBase& OnProgressDelegate, 
										   tstring strGuid /* = _T */,
										   tstring strServerFilePath /*= _T("")*/)
									  
{
	// check parameters
	if( nUploadType == UPLOAD_NONE || dwUserId == 0 || strFilePath == _T("") )
		return FALSE;

	int pos = strServerFilePath.find(_T("{ref-path}"));
	if( pos != -1 )
		strServerFilePath = strServerFilePath.substr(pos + _tcslen(_T("{ref-path}")));


	CNDCloudUploadTask* pUploadTask = new CNDCloudUploadFileTask(nUploadType, 
																dwUserId, 																
																strFilePath, 
																MakeHttpDelegate(this, &CNDCloudUploadTaskManager::OnComplete),
																MakeHttpDelegate(this, &CNDCloudUploadTaskManager::OnProgress),
																OnCompleteDelegate, 
																OnProgressDelegate,
																strGuid, 
																strServerFilePath);
																
		
	if( pUploadTask == NULL )
		return FALSE;

	DWORD dwTaskId = GetCycleCount();

	EnterCriticalSection(&m_Lock);
	m_mapUploadTasks[dwTaskId] = pUploadTask;
	LeaveCriticalSection(&m_Lock);

	pUploadTask->StartUpload();
	return TRUE;
}

//
// upload folder
//
BOOL CNDCloudUploadTaskManager::UploadFolder(int nUploadType, 
										 DWORD dwUserId, 
										 tstring strResType,
										 tstring strTitle,  
										 tstring strGuid,
										 tstring strFolderPath, 
										 CHttpDelegateBase &OnCompleteDelegate,
										 CHttpDelegateBase& OnProgressDelegate,
										 void* pUserData)
{

	CNDCloudUploadFolderTask* pUploadFolder = new CNDCloudUploadFolderTask(nUploadType,
																			dwUserId, 
																			strResType,
																			strTitle, 
																			strGuid, 
																			strFolderPath, 
																			OnCompleteDelegate, 
																			OnProgressDelegate,pUserData);

	if( pUploadFolder == NULL )
		return FALSE;

	pUploadFolder->StartUpload();
	
	return TRUE;
}

bool CNDCloudUploadTaskManager::OnProgress(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	if( pNotify->pUserData == NULL )
		return false;

	CNDCloudUploadTask* pTask = (CNDCloudUploadTask*)pNotify->pUserData;
	CHttpEventSource* pOnUserProgress = pTask->GetUserProgressDelegate();

	if( pOnUserProgress )
		(*pOnUserProgress)(param);

	return true;
}

bool CNDCloudUploadTaskManager::OnComplete(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	if( pNotify->pUserData == NULL )
		return false;

	CNDCloudUploadTask* pTask = (CNDCloudUploadTask*)pNotify->pUserData;
	CHttpEventSource* pOnUserComplete = pTask->GetUserCompleteDelegate();


	//
	THttpNotify UserNotify;
	CStream* pStream = new CStream(1024);

	BOOL bSuccess = pTask->IsSuccess();
	if( bSuccess )
	{
		memcpy(&UserNotify, pNotify, sizeof(THttpNotify));
		
		tstring strGuid = pTask->GetGuid();
		tstring strServerPath = pTask->GetServerFolderPath();

		pStream->WriteString(strGuid);
		pStream->WriteString(strServerPath);
		pStream->ResetCursor();

		UserNotify.pDetailData = pStream;
	}
	else
		UserNotify.dwErrorCode = 1;


	if( pOnUserComplete )
		(*pOnUserComplete)(&UserNotify);

	delete pStream;

	// 
	if( pTask )
	{
		EnterCriticalSection(&m_Lock);

		DWORD dwId = pTask->GetUploadId();
		m_mapUploadTasks.erase(dwId);

		LeaveCriticalSection(&m_Lock);

		delete pTask;
		pTask = NULL;
	}


	return true;
}