//-----------------------------------------------------------------------
// FileName:				NDCloudFile.cpp
//
// Desc:
//------------------------------------------------------------------------
#include "stdafx.h"
#include "NDCloudAPI.h"
#include "NDCloudFile.h"
#include "Util/Util.h"
#include <openssl/md5.h>

#define HISTORY_DATA_FORMAT_VERSION		0x100

#define EduPlatformHostRes				_T("cs.101.com")
#define EduPlatformUrlRes				_T("/v0.1/download?path=")
#define NDCloudFileDirectory			_T("NDCloud")
#define DOWNLOAD_DENTRY_NOT_FOUND		"CS/DOWNLOAD_DENTRY_NOT_FOUND"

CNDCloudFileManager* g_pCloudFileManager = NULL;
tstring				 g_strNDCloudDrive = _T("");


tstring GetNDCloudPath();
void SetNDCloudPath(tstring strPath);

CNDCloudFile::CNDCloudFile(tstring strUrl, tstring strFilePath, int nLastDownloadSize,
						   CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressDelegate, PVOID pUserData /*= NULL*/, tstring strMD5 /*= _T("")*/)
{
	m_strRelativeFilePath	= strFilePath;
	m_strUrl				= strUrl;
	m_strMD5				= strMD5;
	m_bDirectWriteFile		= FALSE;
	m_dwTotalElapseTime		= 0;
	m_bPaused				= FALSE;
	
	m_hFile					= NULL;
	m_hFileMapping			= NULL;
	m_pFileData				= NULL;
	m_nCurFileSize			= 0;
	m_dwStartTime			= 0;
	m_hNdsFile				= NULL;
	m_nLastDownloadSize		= nLastDownloadSize;

	if( OnCompleteDelegate.GetFn() != NULL )
		m_OnComplete += OnCompleteDelegate;

	if( OnProgressDelegate.GetFn() != NULL )
		m_OnProgress += OnProgressDelegate;

	// user data
	DWORD dwKey = (DWORD)OnCompleteDelegate.GetFn() + (DWORD)OnCompleteDelegate.GetObject();
	m_mapOnCompleteUserDatas[dwKey] = pUserData;

	dwKey = (DWORD)OnProgressDelegate.GetFn() + (DWORD)OnProgressDelegate.GetObject();
	m_mapOnProgressUserDatas[dwKey] = pUserData;

}

CNDCloudFile::~CNDCloudFile()
{

}

bool CNDCloudFile::OnStart(void* param)
{
	// check file size
	THttpNotify* pNotify = (THttpNotify*)param;

	// all threads are busy
	if( pNotify->dwErrorCode == -1 && pNotify->nTotalSize == 1 )
	{
		m_OnProgress(param);
		return false;
	}

	tstring strLocalPath = GetNDCloudPath();
	tstring str = strLocalPath + _T("\\");
	tstring strFilePath = str + m_strRelativeFilePath;

	// temp data file (*.ndf) (*.nds)
	tstring strNdfFile = strFilePath + _T(".ndf");
	tstring strNdsFile = strFilePath + _T(".nds");

	//
	DWORD dwFileSize = 0;
	HANDLE hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if( hFile != INVALID_HANDLE_VALUE )
		dwFileSize = GetFileSize(hFile, NULL);

	if( pNotify->nTotalSize != 0 && dwFileSize == pNotify->nTotalSize + m_nLastDownloadSize )
	{
		// don't need to download so OnProgress won't be invoked
		m_nCurFileSize = pNotify->nTotalSize + m_nLastDownloadSize;
		CloseHandle(hFile);
		return false;	
	}

	CloseHandle(hFile);

	// report remote missing error
	if( pNotify->dwErrorCode != 0 || pNotify->nTotalSize == 0 )
	{
		ReportResourceMissingError();
		return false;
	}
		

	//
	m_hFile = CreateFile(strNdfFile.c_str(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if( m_hFile == NULL )
		return false;

	m_hFileMapping = CreateFileMapping(m_hFile, NULL, PAGE_READWRITE, 0, pNotify->nTotalSize + m_nLastDownloadSize, NULL);
	if( m_hFileMapping == NULL )
	{
		CloseHandle(m_hFile);
		m_hFile = NULL;
		return false;
	}

	m_pFileData = (BYTE*)MapViewOfFile(m_hFileMapping, FILE_MAP_WRITE, 0, 0, pNotify->nTotalSize + m_nLastDownloadSize);
	if( m_pFileData == NULL )
	{
		// memory not enough then change to direct write mode
		DWORD dwErrorCode = GetLastError();
		if( dwErrorCode == ERROR_NOT_ENOUGH_MEMORY )
		{
			m_bDirectWriteFile = TRUE;
			CloseHandle(m_hFileMapping);
			m_hFileMapping = NULL;
		}
		else
		{
			CloseHandle(m_hFileMapping);
			CloseHandle(m_hFile);

			m_hFileMapping = NULL;
			m_hFile = NULL;

			return false;
		}
	}

	// set start pos to last downloaded size
	m_nCurFileSize = m_nLastDownloadSize;
	m_hNdsFile = CreateFile(strNdsFile.c_str(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if( m_hNdsFile == INVALID_HANDLE_VALUE )
		m_hNdsFile = NULL;

	m_dwStartTime = GetTickCount();

	return true;
}

bool CNDCloudFile::OnProgress(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;

	// write to file
	if( !m_bDirectWriteFile && m_pFileData == NULL )
		return false;

	// check whether this file not exist {"code":"CS/DOWNLOAD_DENTRY_NOT_FOUND"
	if( pNotify->pData[0] == '{' )
	{
		char szTemp[512] = {0};
		int size = pNotify->nDataSize >= sizeof(szTemp) ? sizeof(szTemp)-1 : pNotify->nDataSize;

		memcpy(szTemp, pNotify->pData, size);

		if( strstr(szTemp, DOWNLOAD_DENTRY_NOT_FOUND) != NULL )
		{
			// report this to server
			ReportResourceMissingError();
			return false;
		}
	}

	// something wrong
	assert( m_nCurFileSize < (pNotify->nTotalSize + m_nLastDownloadSize) );
		
	if( m_bDirectWriteFile )
	{
		DWORD dwBytesWritten = 0;
		SetFilePointer(m_hFile, m_nCurFileSize, 0, FILE_BEGIN);
		WriteFile(m_hFile, pNotify->pData, pNotify->nDataSize, &dwBytesWritten, NULL);
	}
	else
		memcpy(m_pFileData+m_nCurFileSize, pNotify->pData, pNotify->nDataSize);

	m_nCurFileSize += pNotify->nDataSize;

	// record current size to (*.nds)
	if( m_hNdsFile != NULL )
	{	
		DWORD dwBytesWritten = 0;
		SetFilePointer(m_hNdsFile, 0, 0, FILE_BEGIN);
		WriteFile(m_hNdsFile, &m_nCurFileSize, 4, &dwBytesWritten, NULL);
	}

	// percent
	pNotify->fPercent = (float)m_nCurFileSize / (float)(pNotify->nTotalSize + m_nLastDownloadSize);

	// change total size to update user
	pNotify->nTotalSize = pNotify->nTotalSize + m_nLastDownloadSize;
	
	// calculate speed
	DWORD dwElapseTime = GetTickCount() - m_dwStartTime + m_dwTotalElapseTime;
	float fSpeed = ((float)(m_nCurFileSize - m_nLastDownloadSize) / 1024.0f) / ((float)dwElapseTime / 1000.0f);
	
	pNotify->nSpeed =(int)fSpeed;
	if( pNotify->nSpeed < 0 )
		pNotify->nSpeed = 0;

	pNotify->nElapseTime = dwElapseTime / 1000;
 
	// calculate remain time
	int nRemainSizeInK =  (pNotify->nTotalSize - m_nCurFileSize) / 1024;

	if( pNotify->nSpeed )
		pNotify->nRemainTime = nRemainSizeInK / pNotify->nSpeed;
	else
		pNotify->nRemainTime = nRemainSizeInK;

	// traverse with different user data
	CHttpPtrArray* pDelegates = m_OnProgress.GetDelegates();
	for(int i = 0; i < pDelegates->GetSize(); i++)
	{
		CHttpDelegateBase* pDelegate = (CHttpDelegateBase*)pDelegates->GetAt(i);
		if( pDelegate == NULL )
			continue;

		CHttpEventSource EventSource;
		EventSource += *pDelegate;

		PVOID pUserData = NULL;

		DWORD dwKey = (DWORD)pDelegate->GetFn() + (DWORD)pDelegate->GetObject();
		map<DWORD, PVOID>::iterator itr = m_mapOnProgressUserDatas.find(dwKey);
		if( itr != m_mapOnProgressUserDatas.end() )
			pUserData = itr->second;

		pNotify->pUserData = pUserData;
		EventSource(pNotify);
	}

	return true;
}

bool CNDCloudFile::OnComplete(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->strFilePath = _T("");


	tstring strLocalPath = GetNDCloudPath();
	tstring str = strLocalPath + _T("\\");
	tstring strFilePath = str + m_strRelativeFilePath;

	if( m_hNdsFile != NULL )
	{
		CloseHandle(m_hNdsFile);
		m_hNdsFile = NULL;
	}

	if( m_pFileData != NULL )
	{
		UnmapViewOfFile(m_pFileData);
		m_pFileData = NULL;
	}

	if( m_hFileMapping != NULL )
	{
		CloseHandle(m_hFileMapping);
		m_hFileMapping = NULL;
	}

	if( m_hFile != NULL )
	{
		CloseHandle(m_hFile);
		m_hFile = NULL;
	}

	// check file size
	if( m_nCurFileSize != 0 && m_nCurFileSize == pNotify->nTotalSize + m_nLastDownloadSize )
	{
		// rename *.ndf
		tstring strNdcfFilePath = strFilePath + _T(".ndf");
		MoveFileEx(strNdcfFilePath.c_str(), strFilePath.c_str(), MOVEFILE_COPY_ALLOWED|MOVEFILE_REPLACE_EXISTING);
		pNotify->strFilePath = strFilePath;

		// remove *.nds
		tstring strNdsFilePath = strFilePath + _T(".nds");
		DeleteFile(strNdsFilePath.c_str());
	}

	// traverse with different user data
	CHttpPtrArray* pDelegates = m_OnComplete.GetDelegates();
	//assert(pDelegates->GetSize() == m_mapOnCompleteUserDatas.size());

	for(int i = 0; i < pDelegates->GetSize(); i++)
	{
		CHttpDelegateBase* pDelegate = (CHttpDelegateBase*)pDelegates->GetAt(i);
		if( pDelegate == NULL )
			continue;

		CHttpEventSource EventSource;
		EventSource += *pDelegate;

		PVOID pUserData = NULL;

		DWORD dwKey = (DWORD)pDelegate->GetFn() + (DWORD)pDelegate->GetObject();
		map<DWORD, PVOID>::iterator itr = m_mapOnCompleteUserDatas.find(dwKey);
		if( itr != m_mapOnCompleteUserDatas.end() )
			pUserData = itr->second;

		pNotify->pUserData = pUserData;
		EventSource(pNotify);
	}


	g_pCloudFileManager->OnComplete(this);

	return true;
}

bool CNDCloudFile::OnPause(void *param)
{
	if( m_pFileData != NULL )
	{
		UnmapViewOfFile(m_pFileData);
		m_pFileData = NULL;
	}

	if( m_hFileMapping != NULL )
	{
		CloseHandle(m_hFileMapping);
		m_hFileMapping = NULL;
	}

	if( m_hFile != NULL )
	{
		CloseHandle(m_hFile);
		m_hFile = NULL;
	}

	if( m_hNdsFile != NULL )
	{
		CloseHandle(m_hNdsFile);
		m_hNdsFile = NULL;
	}

	m_dwTotalElapseTime += GetTickCount() - m_dwStartTime;

	// pause
	m_OnPause(param);

	m_bPaused = TRUE;
	return true;
}

void CNDCloudFile::PauseDownload(CHttpDelegateBase* pOnPauseDelegate)
{
	if( pOnPauseDelegate != NULL && pOnPauseDelegate->GetFn() != NULL )
		m_OnPause += *pOnPauseDelegate;

}

void CNDCloudFile::ResumeDownload()
{
	m_bPaused = FALSE;
}

void CNDCloudFile::CancelDownload()
{
	if( m_pFileData != NULL )
	{
		UnmapViewOfFile(m_pFileData);
		m_pFileData = NULL;
	}

	if( m_hFileMapping != NULL )
	{
		CloseHandle(m_hFileMapping);
		m_hFileMapping = NULL;
	}

	if( m_hFile != NULL )
	{
		CloseHandle(m_hFile);
		m_hFile = NULL;
	}

	if( m_hNdsFile != NULL )
	{
		CloseHandle(m_hNdsFile);
		m_hNdsFile = NULL;
	}

	m_OnComplete.clear();
	m_OnProgress.clear();
	m_OnPause.clear();

}

BOOL CNDCloudFile::AddTaskCallBack(CHttpDelegateBase &OnCompleteDelegate, CHttpDelegateBase &OnProgressDelegate, PVOID pUserData /*= NULL*/)
{
	if( OnCompleteDelegate.GetFn() != NULL )
		m_OnComplete += OnCompleteDelegate;

	if( OnProgressDelegate.GetFn() != NULL )
		m_OnProgress += OnProgressDelegate;

	// user data
	DWORD dwKey = (DWORD)OnCompleteDelegate.GetFn() + (DWORD)OnCompleteDelegate.GetObject();
	m_mapOnCompleteUserDatas[dwKey] = pUserData;

	dwKey = (DWORD)OnProgressDelegate.GetFn() + (DWORD)OnProgressDelegate.GetObject();
	m_mapOnProgressUserDatas[dwKey] = pUserData;


	return TRUE;
}

BOOL CNDCloudFile::DelTaskCallBack(CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressDelegate)
{
	if( OnCompleteDelegate.GetFn() != NULL )
		m_OnComplete -= OnCompleteDelegate;

	if( OnProgressDelegate.GetFn() != NULL )
		m_OnProgress -= OnProgressDelegate;

	// user data
	DWORD dwKey = (DWORD)OnCompleteDelegate.GetFn() + (DWORD)OnCompleteDelegate.GetObject();
	m_mapOnCompleteUserDatas.erase(dwKey);

	dwKey = (DWORD)OnProgressDelegate.GetFn() + (DWORD)OnProgressDelegate.GetObject();
	m_mapOnProgressUserDatas.erase(dwKey);

	return TRUE;
}

DWORD CNDCloudFile::GetTaskId()
{
	return m_dwTaskId;
}

void CNDCloudFile::SetTaskId(DWORD dwTaskId)
{
	m_dwTaskId = dwTaskId;
}

void CNDCloudFile::SetLastDownloadSize(DWORD dwSize)
{
	m_nLastDownloadSize = dwSize;
}

tstring CNDCloudFile::GetUrl()
{
	return m_strUrl;
}

tstring CNDCloudFile::GetPath()
{
	return m_strRelativeFilePath;
}

tstring CNDCloudFile::GetMD5()
{
	return m_strMD5;
}

void CNDCloudFile::SetPath(tstring strPath)
{
	m_strRelativeFilePath = strPath;
}

BOOL CNDCloudFile::IsPaused()
{
	return m_bPaused;
}

void CNDCloudFile::ReportResourceMissingError()
{
	CHttpDownloadManager* pHttpManager = HttpDownloadManager::GetInstance();
	if( pHttpManager == NULL )
		return;

	tstring strErrorMessage = _T("missing_resource=[File Missing]: ");
	strErrorMessage += m_strUrl;
	
	pHttpManager->AddTask(_T("p.101.com"), _T("/101ppt/resourceMissing.php"), _T(""), _T("POST"), Str2Ansi(strErrorMessage).c_str(), 80, 
							MakeHttpDelegate(this, &CNDCloudFile::OnResourceMissingReported), 
							MakeHttpDelegate(NULL), 
							MakeHttpDelegate(NULL), 
							FALSE);

}

bool CNDCloudFile::OnResourceMissingReported(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pData[pNotify->nDataSize] = '\0';

	return true;
}

//-----------------------------------------------------------------
// Cloud file manager
//
CNDCloudFileManager::CNDCloudFileManager()
{
	g_pCloudFileManager = this;
	m_hOwnerWnd			= NULL;
}

CNDCloudFileManager::~CNDCloudFileManager()
{
	DeleteCriticalSection(&m_Lock);
	TerminateThread(m_hNotifyThread, 0);
}

BOOL CNDCloudFileManager::Initialize(CHttpDownloadManager* pHttpDownloadManager)
{
	m_pHttpDownloadManager = pHttpDownloadManager;

	InitializeCriticalSection(&m_Lock);

	// create directory
	tstring strFilePath = GetNDCloudPath();
	strFilePath += _T("\\");
	strFilePath += NDCloudFileDirectory;

	// just use local path if create directory denied
	BOOL res = CreateDirectory(strFilePath.c_str(), NULL);
	if( !res )
	{
		DWORD dwErrorCode = GetLastError();
		if( dwErrorCode != ERROR_ALREADY_EXISTS )	// like access denied
		{
			SetNDCloudPath(GetLocalPath());
			strFilePath = GetNDCloudPath();
			strFilePath += _T("\\");
			strFilePath += NDCloudFileDirectory;

			CreateDirectory(strFilePath.c_str(), NULL);
		}
	}
	
	
	// image
	tstring strImagePath		= strFilePath + _T("\\Image");
	tstring strVolumePath		= strFilePath + _T("\\Volume");
	tstring strVideoPath		= strFilePath + _T("\\Video");
	tstring	strFlashPath		= strFilePath + _T("\\Flash");
	tstring strCoursePath		= strFilePath + _T("\\Course");
	tstring strQuestionPath		= strFilePath + _T("\\Question");
	tstring strNdpCoursePath	= strFilePath + _T("\\NdpCourse");
	tstring strPPTTempPath		= strFilePath + _T("\\PPTTemp");
	tstring str3DResourcePath	= strFilePath + _T("\\3DResource");
	tstring strVRResourcePath	= strFilePath + _T("\\VRResource");


	CreateDirectory(strImagePath.c_str(), NULL);
	CreateDirectory(strVolumePath.c_str(), NULL);
	CreateDirectory(strVideoPath.c_str(), NULL);
	CreateDirectory(strFlashPath.c_str(), NULL);
	CreateDirectory(strCoursePath.c_str(), NULL);
	CreateDirectory(strQuestionPath.c_str(), NULL);
	CreateDirectory(strNdpCoursePath.c_str(), NULL);
	CreateDirectory(strPPTTempPath.c_str(), NULL);
	CreateDirectory(str3DResourcePath.c_str(), NULL);
	CreateDirectory(strVRResourcePath.c_str(), NULL);

	
	// exist file notify thread
	m_hNotifyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hNotifyThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ExistFileNotifyThread, this, 0, NULL);

	// load history
	LoadHistory();
	
	
	return TRUE;
}
		
BOOL CNDCloudFileManager::Destroy()
{
	EnterCriticalSection(&m_Lock);
	map<DWORD, CNDCloudFile*>::iterator itr;
	for(itr = m_mapFiles.begin(); itr != m_mapFiles.end(); itr++)
	{
		CNDCloudFile* pFile = itr->second;
		DWORD dwTaskId = pFile->GetTaskId();

		// cancel http download task
		HttpDownloadManager::GetInstance()->CancelTask(dwTaskId);
		pFile->CancelDownload();

		delete pFile;
		pFile = NULL;
	}

	m_mapFiles.clear();
	LeaveCriticalSection(&m_Lock);

	// save history
	SaveHistory();
	return TRUE;
}

BOOL CNDCloudFileManager::SetOwner(HWND hWnd)
{
	m_hOwnerWnd = hWnd;
	return TRUE;
}

//
// Download image file
//
DWORD CNDCloudFileManager::DownloadFileImage(tstring strUrl, tstring strGuid, tstring strName, int nThumbSize, 
											 CHttpDelegateBase &OnCompleteDelegate, CHttpDelegateBase &OnProgressCB,void* pUserData, tstring strMD5)
{
	// check thumb size
	if( !CheckThumbSize(nThumbSize) )
		return 0;

	// 
	tstring strFileName, strFileExt;
	SplitFileNameExt(strUrl, strFileName, strFileExt);

	//if( strName != _T("") )
	//{
	//	TCHAR szName[MAX_PATH];
	//	_stprintf_s(szName, _T("%s_%08lX"), strName.c_str(), GetCycleCount());
	//	strFileName = szName;
	//}

	char szText[20];
	itoa(nThumbSize,szText,10);

	tstring strSize = Ansi2Str(szText);
	tstring strPath = NDCloudFileDirectory;
	strPath += _T("\\Image\\") + strFileName + _T("_") + strGuid + _T("_") + strSize + _T(".") + strFileExt;

	// compose url
	tstring strFileUrl = ComposeUrl(strUrl, nThumbSize);
	if( strFileUrl == _T("") )
		return 0;

	// cannot continue if thumbnail size is not 0
	DWORD dwSize = 0;
	if( nThumbSize == 0 )
		dwSize = GetLastDownloadFileSize(strPath);

	return DownloadCloudFile(strFileUrl, strPath, dwSize, OnCompleteDelegate, OnProgressCB, pUserData, strMD5);
}

DWORD CNDCloudFileManager::DownloadFileImageEx(tstring strUrl, tstring strGuid, tstring strName, int nThumbSize, 
											 CHttpDelegateBase &OnCompleteDelegate, CHttpDelegateBase &OnProgressCB,void* pUserData, tstring strMD5)
{
	// check thumb size
	if( !CheckThumbSize(nThumbSize) )
		return 0; 


	tstring strFileName, strFileExt;
	SplitFileNameExt(strUrl, strFileName, strFileExt);

	TCHAR szPath[MAX_PATH]; 
	_stprintf_s(szPath, _T("%s\\Image\\%s_%d.png"), NDCloudFileDirectory, strFileName.c_str(), nThumbSize);

	// cannot continue if thumbnail size is not 0
	DWORD dwSize = 0;
	if( nThumbSize == 0 )
		dwSize = GetLastDownloadFileSize(szPath);

	return DownloadCloudFile(strUrl, szPath, dwSize, OnCompleteDelegate, OnProgressCB, pUserData, strMD5);
}

//
// Download video file
//
DWORD CNDCloudFileManager::DownloadFileVideo(tstring strUrl, tstring strGuid, tstring strName, 
											 CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressCB,void* pUserData, tstring strMD5)
{
	tstring strFileName, strFileExt;
	SplitFileNameExt(strUrl, strFileName, strFileExt);

	//if( strName != _T("") )
	//{
	//	TCHAR szName[MAX_PATH];
	//	_stprintf_s(szName, _T("%s_%08lX"), strName.c_str(),  GetCycleCount());
	//	strFileName = szName;
	//}

	TCHAR szPath[MAX_PATH];
	_stprintf_s(szPath, _T("%s\\Video\\%s.%s"), NDCloudFileDirectory, strFileName.c_str(), strFileExt.c_str());

	// compose url
	tstring strFileUrl = ComposeUrl(strUrl, 0);
	if( strFileUrl == _T("") )
		return 0;

	DWORD dwSize = GetLastDownloadFileSize(szPath);
	return DownloadCloudFile(strFileUrl, szPath, dwSize, OnCompleteDelegate, OnProgressCB, pUserData, strMD5);
}

//
// Download volume file
//
DWORD CNDCloudFileManager::DownloadFileVolume(tstring strUrl, tstring strGuid, tstring strName, 
											  CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressCB,void* pUserData, tstring strMD5)
{
	tstring strFileName, strFileExt;
	SplitFileNameExt(strUrl, strFileName, strFileExt);

	//if( strName != _T("") )
	//{
	//	TCHAR szName[MAX_PATH];
	//	_stprintf_s(szName, _T("%s_%08lX"), strName.c_str(), GetCycleCount());
	//	strFileName = szName;
	//}

	TCHAR szPath[MAX_PATH];
	_stprintf_s(szPath, _T("%s\\Volume\\%s.%s"), NDCloudFileDirectory, strFileName.c_str(), strFileExt.c_str());

	// compose url
	tstring strFileUrl = ComposeUrl(strUrl, 0);
	if( strFileUrl == _T("") )
		return 0;

	DWORD dwSize = GetLastDownloadFileSize(szPath);
	return DownloadCloudFile(strFileUrl, szPath, dwSize, OnCompleteDelegate, OnProgressCB, pUserData, strMD5);
}

//
// Download flash file
//
DWORD CNDCloudFileManager::DownloadFileFlash(tstring strUrl, tstring strGuid, tstring strName, 
											 CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressCB,void* pUserData, tstring strMD5)
{
	tstring strFileName, strFileExt;
	SplitFileNameExt(strUrl, strFileName, strFileExt);

	//if( strName != _T("") )
	//{
	//	TCHAR szName[MAX_PATH];
	//	_stprintf_s(szName, _T("%s_%08lX"), strName.c_str(), GetCycleCount());
	//	strFileName = szName;
	//}

	TCHAR szPath[MAX_PATH];
	_stprintf_s(szPath, _T("%s\\Flash\\%s.%s"), NDCloudFileDirectory, strFileName.c_str(), strFileExt.c_str());

	// compose url
	tstring strFileUrl = ComposeUrl(strUrl, 0);
	if( strFileUrl == _T("") )
		return 0;

	DWORD dwSize = GetLastDownloadFileSize(szPath);
	return DownloadCloudFile(strFileUrl, szPath, dwSize, OnCompleteDelegate, OnProgressCB, pUserData, strMD5);
}


//
// Download question file
//
DWORD CNDCloudFileManager::DownloadFileQuestion(tstring strUrl, tstring strGuid, tstring strName, 
												CHttpDelegateBase &OnCompleteDelegate, CHttpDelegateBase &OnProgressCB,void* pUserData, tstring strMD5)
{
	tstring strFileName, strFileExt;
	SplitFileNameExt(strUrl, strFileName, strFileExt);

	TCHAR szDir[MAX_PATH];
	_stprintf_s(szDir, _T("%s\\Question"), NDCloudFileDirectory);

	tstring strFullDir = GetNDCloudPath();
	strFullDir += _T("\\");
	strFullDir += szDir;

	TCHAR szPath[MAX_PATH];
	_stprintf_s(szPath, _T("%s\\%s.%s"), szDir, strFileName.c_str(), strFileExt.c_str());

	// full url
	DWORD dwSize = GetLastDownloadFileSize(szPath);

	// strip cs.101.com
	int pos = strUrl.find(EduPlatformHostRes);
	if( pos != -1 )
	{
		tstring strHost = EduPlatformHostRes;
		strUrl = strUrl.substr(pos+strHost.length());
	}
	

	return DownloadCloudFile(strUrl, szPath, dwSize, OnCompleteDelegate, OnProgressCB, pUserData, strMD5);
}


DWORD CNDCloudFileManager::DownloadFileNdpCourse( tstring strUrl, tstring strGuid, tstring strName, 
												 CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressCB ,void* pUserData, tstring strMD5)
{
	tstring strFileName, strFileExt;
	SplitFileNameExt(strUrl, strFileName, strFileExt);

	TCHAR szDir[MAX_PATH];
	_stprintf_s(szDir, _T("%s\\NdpCourse"), NDCloudFileDirectory);

	tstring strFullDir = GetNDCloudPath();
	strFullDir += _T("\\");
	strFullDir += szDir;

	TCHAR szPath[MAX_PATH];
	_stprintf_s(szPath, _T("%s\\%s.%s"), szDir, strFileName.c_str(), strFileExt.c_str());

	// full url
	DWORD dwSize = GetLastDownloadFileSize(szPath);

	// strip cs.101.com
	tstring strFileUrl;
	int pos = strUrl.find(EduPlatformHostRes);
	if( pos != -1 )
	{
		tstring strHost = EduPlatformHostRes;
		strFileUrl = strUrl.substr(pos+strHost.length());
	}
	else
	{
		// compose url
		strFileUrl = ComposeUrl(strUrl, 0);
		if( strFileUrl == _T("") )
			return 0;
	}
	

	return DownloadCloudFile(strFileUrl, szPath, dwSize, OnCompleteDelegate, OnProgressCB, pUserData, strMD5);
}

DWORD CNDCloudFileManager::DownloadFile3DResource( tstring strUrl, tstring strGuid, tstring strName, 
												  CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressCB,void* pUserData, tstring strMD5 )
{
	tstring strFileName, strFileExt;
	SplitFileNameExt(strUrl, strFileName, strFileExt);

	TCHAR szDir[MAX_PATH];
	_stprintf_s(szDir, _T("%s\\3DResource"), NDCloudFileDirectory);

	tstring strFullDir = GetNDCloudPath();
	strFullDir += _T("\\");
	strFullDir += szDir;

	TCHAR szPath[MAX_PATH];
	_stprintf_s(szPath, _T("%s\\%s.%s"), szDir, strFileName.c_str(), strFileExt.c_str());

	// compose url
	tstring strFileUrl = ComposeUrl(strUrl, 0);
	if( strFileUrl == _T("") )
		return 0;

	DWORD dwSize = GetLastDownloadFileSize(szPath);
	return DownloadCloudFile(strFileUrl, szPath, dwSize, OnCompleteDelegate, OnProgressCB, pUserData, strMD5);
}

DWORD CNDCloudFileManager::DownloadFileVRResource( tstring strUrl, tstring strGuid, tstring strName, CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressCB,void* pUserData/*=NULL*/, tstring strMD5 /*= _T("")*/ )
{
	tstring strDownloadPath;
	tstring strDownloadSize;
	tstring strDownloadSession;

	SplitDownloadParameters(strUrl, strDownloadPath, strDownloadSize, strDownloadSession);


	// split out file name and extension
	tstring strFileName, strFileExt;
	SplitFileNameExt(strDownloadPath, strFileName, strFileExt);

	TCHAR szDir[MAX_PATH];
	_stprintf_s(szDir, _T("%s\\VRResource"), NDCloudFileDirectory);

	tstring strFullDir = GetNDCloudPath();
	strFullDir += _T("\\");
	strFullDir += szDir;

	TCHAR szPath[MAX_PATH];
	_stprintf_s(szPath, _T("%s\\%s.%s"), szDir, strFileName.c_str(), strFileExt.c_str());

	// compose url
	tstring strFileUrl = ComposeUrl(strDownloadPath, 0);
	if( strFileUrl == _T("") )
		return 0;

	DWORD dwSize = GetLastDownloadFileSize(szPath);
	return DownloadCloudFile(strFileUrl, szPath, dwSize, OnCompleteDelegate, OnProgressCB, pUserData, strMD5, strDownloadSession);
}

DWORD CNDCloudFileManager::DownloadFileSearchPlatform( tstring strUrl, tstring strGuid, tstring strName, int nFileType, CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressCB,void* pUserData/*=NULL*/, tstring strMD5 /*= _T("")*/ )
{
	
	TCHAR szHost[MAX_PATH];
	TCHAR szUrlPath[1024];
	TCHAR szExt[1024];
	if(_stscanf_s(strUrl.c_str(), _T("http://%[^/]%s"), szHost, _countof(szHost) - 1, szUrlPath , _countof(szUrlPath) - 1) != 2)
	{
		return 0;
	}

	TCHAR * pos = _tcsrchr((TCHAR *)strUrl.c_str(), _T('.'));
	if(!pos)
		return 0;
	_tcscpy_s(szExt, _countof(szExt) - 1 ,pos);

	tstring strFullDir;
	TCHAR szDir[MAX_PATH];
	if(nFileType == CourseFilePPT)
	{
		_stprintf_s(szDir, _T("%s\\Course\\"), NDCloudFileDirectory);
		strFullDir = szDir;
		strFullDir += strGuid;
		strFullDir += _T(".ppt");
	}
	else if(nFileType == CloudFileImage)
	{
		_stprintf_s(szDir, _T("%s\\Image\\"), NDCloudFileDirectory);
		strFullDir = szDir;
		strFullDir += strGuid;
		strFullDir += szExt;

	}

	 
	DWORD dwSize = GetLastDownloadFileSize(strFullDir.c_str());
	return DownloadCloudFile(szHost, szUrlPath, strFullDir, dwSize, OnCompleteDelegate, OnProgressCB, pUserData, strMD5);
}

//
// Download course file
//
DWORD CNDCloudFileManager::DownloadFileCourse(tstring strUrl, tstring strGuid, tstring strName, int nCourseFileType, 
											  CHttpDelegateBase &OnCompleteDelegate, CHttpDelegateBase &OnProgressCB,void* pUserData, tstring strMD5)
{
	// create directory
	tstring strLocalPath = GetNDCloudPath();
	tstring strPPT = strLocalPath + _T("\\");
	strPPT += NDCloudFileDirectory;
	strPPT += _T("\\Course\\");
	strPPT += strGuid;

	tstring strThumb = strPPT + _T("\\Thumbnails");

	CreateDirectory(strPPT.c_str(), NULL);
	CreateDirectory(strThumb.c_str(), NULL);

	// split
	tstring strFileName, strFileExt;
	SplitFileNameExt(strUrl, strFileName, strFileExt);

	// compose url
	tstring strFileUrl = ComposeUrl(strUrl, 0);
	if( strFileUrl == _T("") )
		return 0;


	// compose file path
	tstring strRelativePath;

	TCHAR szPath[MAX_PATH];
	_stprintf_s(szPath, _T("%s\\Course\\%s\\"), NDCloudFileDirectory, strGuid.c_str());
	
	if( nCourseFileType == CourseFilePPT )
	{
		strRelativePath = tstring(szPath) + strFileName;
		strRelativePath += _T(".");
		strRelativePath += strFileExt;
	}
	else if( nCourseFileType == CourseFileThumb )
	{
		strRelativePath = tstring(szPath) + _T("Thumbnails\\");
		strRelativePath += strFileName;
		strRelativePath += _T(".");
		strRelativePath += strFileExt;
	}

	DWORD dwSize = GetLastDownloadFileSize(strRelativePath);
	return DownloadCloudFile(strFileUrl, strRelativePath, dwSize, OnCompleteDelegate, OnProgressCB, pUserData, strMD5);
}

//-----------------------------------------------------------------------------------
// Download cloud file
//
DWORD CNDCloudFileManager::DownloadCloudFile(tstring strFileUrl, 
											 tstring strRelativeFilePath, 
											 DWORD dwLastFileSize,
											 CHttpDelegateBase &OnCompleteDelegate, 
											 CHttpDelegateBase& OnProgressDelegate,
											 void* pUserData, 
											 tstring strMD5,
											 tstring strSessionId /*= _T("")*/)
{
	if( m_pHttpDownloadManager == NULL )
		return 0;

	strFileUrl = UrlEncode(Str2Utf8(strFileUrl));

	//

	tstring strLocalPath = GetNDCloudPath();

	// check whether we have downloaded before
	DWORD dwCrc = CalcCRC((char*)Str2Utf8(strFileUrl).c_str(), strFileUrl.length());

	map<DWORD, tstring>::iterator itr = m_mapFilePaths.find(dwCrc);
	if( itr != m_mapFilePaths.end() )
	{
		tstring strFilePath = strLocalPath + _T("\\");
		strFilePath += itr->second;

	 
		// check file exist
		DWORD dwRet = GetFileAttributes(strFilePath.c_str());
		if( dwRet != INVALID_FILE_ATTRIBUTES )
		{
			BOOL bNeedDownload = FALSE;

			// check file md5 if supplied
			if( strMD5 != _T("") )
			{
				map<DWORD, tstring>::iterator itr_md5 = m_mapFileMD5s.find(dwCrc);
				if( itr_md5 != m_mapFileMD5s.end() )
				{
					tstring strLastMD5 = itr_md5->second;
					if( strLastMD5 != strMD5 )
						bNeedDownload = TRUE;
				}
			}

			if( !bNeedDownload )
			{
				DWORD dwTaskId = GetCycleCount() ^ dwCrc;

				THttpNotify* pCompleteNotify = new THttpNotify;

				pCompleteNotify->dwErrorCode	= 0;
				pCompleteNotify->fPercent		= 1.0;
				pCompleteNotify->pData			= NULL;
				pCompleteNotify->nDataSize		= 0;
				pCompleteNotify->pUserData		= pUserData;
				pCompleteNotify->strFilePath	= strFilePath;
				pCompleteNotify->dwTaskId		= dwTaskId;

				EnterCriticalSection(&m_Lock);
				m_mapExistFileNotify[OnCompleteDelegate.Copy()] = pCompleteNotify;
				LeaveCriticalSection(&m_Lock);

				SetEvent(m_hNotifyEvent);
				//OnCompleteDelegate(&CompleteNotify);
				return dwTaskId;
			}
		
		}
	}


	// check whether we are downloading this file
	BOOL bDownloading = FALSE;
	DWORD dwDownloadingTaskId = 0;

	EnterCriticalSection(&m_Lock);
	map<DWORD, CNDCloudFile*>::iterator itr_url = m_mapUrlFiles.find(dwCrc);
	if( itr_url != m_mapUrlFiles.end() )
	{
		// add to file's callback
		CNDCloudFile* pFile = itr_url->second;
		pFile->AddTaskCallBack(OnCompleteDelegate, OnProgressDelegate, pUserData);

		dwDownloadingTaskId = pFile->GetTaskId();
		bDownloading = TRUE;
	}
	LeaveCriticalSection(&m_Lock);

	if( bDownloading )
		return dwDownloadingTaskId;


	// session id
	if( strSessionId != _T("") )
	{
		strFileUrl += _T("&session=");
		strFileUrl += strSessionId;

		 dwCrc = CalcCRC((char*)Str2Utf8(strFileUrl).c_str(), strFileUrl.length());
	}

	// callback
	CNDCloudFile* pFile = new CNDCloudFile(strFileUrl, strRelativeFilePath, dwLastFileSize, OnCompleteDelegate, OnProgressDelegate, pUserData, strMD5);
	if( pFile == NULL )
		return 0;

	CHttpDelegateBase& httpCompleteDelegate = MakeHttpDelegate(pFile, &CNDCloudFile::OnComplete);
	CHttpDelegateBase& httpProgressDelegate = MakeHttpDelegate(pFile, &CNDCloudFile::OnProgress);
	CHttpDelegateBase& httpStartDelegate	= MakeHttpDelegate(pFile, &CNDCloudFile::OnStart);

	DWORD dwTaskId = m_pHttpDownloadManager->AddTask(EduPlatformHostRes, 
		strFileUrl.c_str(), 
		_T(""), 
		_T("GET"), 
		"", 
		INTERNET_DEFAULT_HTTP_PORT, 
		httpCompleteDelegate, 
		httpProgressDelegate,
		httpStartDelegate,
		TRUE, 
		TRUE,
		dwLastFileSize,
		NULL);

	pFile->SetTaskId(dwTaskId);

	EnterCriticalSection(&m_Lock);
	m_mapFiles[dwTaskId] = pFile;
	m_mapUrlFiles[dwCrc] = pFile;
	LeaveCriticalSection(&m_Lock);

	return dwTaskId;

}

DWORD CNDCloudFileManager::DownloadCloudFile( tstring strHost, 
											 tstring strFileUrl, 
											 tstring strRelativeFilePath, 
											 DWORD dwLastFileSize, 
											 CHttpDelegateBase &OnCompleteDelegate, 
											 CHttpDelegateBase& OnProgressDelegate,
											 void* pUserData/*=NULL*/, 
											 tstring strMD5 /*= _T("")*/, 
											 tstring strSessionId /*= _T("")*/ 
											 )
{
	if( m_pHttpDownloadManager == NULL )
		return 0;

	strFileUrl = UrlEncode(Str2Utf8(strFileUrl));

	//

	tstring strLocalPath = GetNDCloudPath();

	// check whether we have downloaded before
	DWORD dwCrc = CalcCRC((char*)Str2Utf8(strFileUrl).c_str(), strFileUrl.length());

	map<DWORD, tstring>::iterator itr = m_mapFilePaths.find(dwCrc);
	if( itr != m_mapFilePaths.end() )
	{
		tstring strFilePath = strLocalPath + _T("\\");
		strFilePath += itr->second;


		// check file exist
		DWORD dwRet = GetFileAttributes(strFilePath.c_str());
		if( dwRet != INVALID_FILE_ATTRIBUTES )
		{
			BOOL bNeedDownload = FALSE;

			// check file md5 if supplied
			if( strMD5 != _T("") )
			{
				map<DWORD, tstring>::iterator itr_md5 = m_mapFileMD5s.find(dwCrc);
				if( itr_md5 != m_mapFileMD5s.end() )
				{
					tstring strLastMD5 = itr_md5->second;
					if( strLastMD5 != strMD5 )
						bNeedDownload = TRUE;
				}
			}

			if( !bNeedDownload )
			{
				DWORD dwTaskId = GetCycleCount() ^ dwCrc;

				THttpNotify* pCompleteNotify = new THttpNotify;

				pCompleteNotify->dwErrorCode	= 0;
				pCompleteNotify->fPercent		= 1.0;
				pCompleteNotify->pData			= NULL;
				pCompleteNotify->nDataSize		= 0;
				pCompleteNotify->pUserData		= pUserData;
				pCompleteNotify->strFilePath	= strFilePath;
				pCompleteNotify->dwTaskId		= dwTaskId;

				EnterCriticalSection(&m_Lock);
				m_mapExistFileNotify[OnCompleteDelegate.Copy()] = pCompleteNotify;
				LeaveCriticalSection(&m_Lock);

				SetEvent(m_hNotifyEvent);
				//OnCompleteDelegate(&CompleteNotify);
				return dwTaskId;
			}

		}
	}


	// check whether we are downloading this file
	BOOL bDownloading = FALSE;
	DWORD dwDownloadingTaskId = 0;

	EnterCriticalSection(&m_Lock);
	map<DWORD, CNDCloudFile*>::iterator itr_url = m_mapUrlFiles.find(dwCrc);
	if( itr_url != m_mapUrlFiles.end() )
	{
		// add to file's callback
		CNDCloudFile* pFile = itr_url->second;
		pFile->AddTaskCallBack(OnCompleteDelegate, OnProgressDelegate, pUserData);

		dwDownloadingTaskId = pFile->GetTaskId();
		bDownloading = TRUE;
	}
	LeaveCriticalSection(&m_Lock);

	if( bDownloading )
		return dwDownloadingTaskId;


	// session id
	if( strSessionId != _T("") )
	{
		strFileUrl += _T("&session=");
		strFileUrl += strSessionId;

		dwCrc = CalcCRC((char*)Str2Utf8(strFileUrl).c_str(), strFileUrl.length());
	}

	// callback
	CNDCloudFile* pFile = new CNDCloudFile(strFileUrl, strRelativeFilePath, dwLastFileSize, OnCompleteDelegate, OnProgressDelegate, pUserData, strMD5);
	if( pFile == NULL )
		return 0;

	CHttpDelegateBase& httpCompleteDelegate = MakeHttpDelegate(pFile, &CNDCloudFile::OnComplete);
	CHttpDelegateBase& httpProgressDelegate = MakeHttpDelegate(pFile, &CNDCloudFile::OnProgress);
	CHttpDelegateBase& httpStartDelegate	= MakeHttpDelegate(pFile, &CNDCloudFile::OnStart);

	DWORD dwTaskId = m_pHttpDownloadManager->AddTask(strHost.c_str(), 
		strFileUrl.c_str(), 
		_T(""), 
		_T("GET"), 
		"", 
		INTERNET_DEFAULT_HTTP_PORT, 
		httpCompleteDelegate, 
		httpProgressDelegate,
		httpStartDelegate,
		TRUE, 
		TRUE,
		dwLastFileSize,
		NULL);

	pFile->SetTaskId(dwTaskId);

	EnterCriticalSection(&m_Lock);
	m_mapFiles[dwTaskId] = pFile;
	m_mapUrlFiles[dwCrc] = pFile;
	LeaveCriticalSection(&m_Lock);

	return dwTaskId;
}

//
// remove all delegates and remove this download task
// 
BOOL CNDCloudFileManager::CancelDownload(DWORD dwTaskId)
{
	EnterCriticalSection(&m_Lock);

	map<DWORD, CNDCloudFile*>::iterator itr = m_mapFiles.find(dwTaskId);
	if( itr != m_mapFiles.end() )
	{
		CNDCloudFile* pFile = itr->second;

		// url crc
		tstring strUrl = pFile->GetUrl();
		DWORD dwCrc = CalcCRC((char*)strUrl.c_str(), strUrl.length());

		map<DWORD, CNDCloudFile*>::iterator itr_url = m_mapUrlFiles.find(dwCrc);
		if( itr_url != m_mapUrlFiles.end() )
			m_mapUrlFiles.erase(itr_url);
		
		// cancel http download task
		HttpDownloadManager::GetInstance()->CancelTask(dwTaskId);

		// cancel download
		pFile->CancelDownload();

		delete pFile;
		pFile = NULL;

		m_mapFiles.erase(itr);
	}

	LeaveCriticalSection(&m_Lock);
	return TRUE;
}

//
// only remove one delegate and file is still downloading
// 
BOOL CNDCloudFileManager::CancelDownload(DWORD dwTaskId, CHttpDelegateBase* pOnCompleteDelegate, CHttpDelegateBase* pOnProgressDelegate)
{
	EnterCriticalSection(&m_Lock);

	map<DWORD, CNDCloudFile*>::iterator itr = m_mapFiles.find(dwTaskId);
	if( itr != m_mapFiles.end() )
	{
		CNDCloudFile* pFile = itr->second;
		if (!pOnCompleteDelegate)
		{
			pOnCompleteDelegate = &MakeHttpDelegate(NULL);
		}

		if (!pOnProgressDelegate)
		{
			pOnProgressDelegate = &MakeHttpDelegate(NULL);
		}
		pFile->DelTaskCallBack(*pOnCompleteDelegate, *pOnProgressDelegate);
	}

	LeaveCriticalSection(&m_Lock);
	return TRUE;
}

//
// Pause download
//
BOOL CNDCloudFileManager::PauseDownload(DWORD dwTaskId, CHttpDelegateBase* pOnPauseDelegate/* = NULL*/)
{
	EnterCriticalSection(&m_Lock);

	map<DWORD, CNDCloudFile*>::iterator itr = m_mapFiles.find(dwTaskId);
	if( itr != m_mapFiles.end() )
	{
		CNDCloudFile* pFile = itr->second;
		pFile->PauseDownload(pOnPauseDelegate);

		m_pHttpDownloadManager->PauseTask(dwTaskId, &MakeHttpDelegate(pFile, &CNDCloudFile::OnPause));	
	}

	LeaveCriticalSection(&m_Lock);
	return TRUE;
}

//
// Resume download
//
BOOL CNDCloudFileManager::ResumeDownload(DWORD dwTaskId)
{
	BOOL bPaused = FALSE;
	DWORD dwLastDownloadSize = 0;
	EnterCriticalSection(&m_Lock);

	map<DWORD, CNDCloudFile*>::iterator itr = m_mapFiles.find(dwTaskId);
	if( itr != m_mapFiles.end() )
	{
		CNDCloudFile* pFile = itr->second;

		if( pFile->IsPaused() )
		{
			tstring strRelativePath = pFile->GetPath();

			// calculate last downloaded file size
			dwLastDownloadSize = GetLastDownloadFileSize(strRelativePath);

			pFile->SetLastDownloadSize(dwLastDownloadSize);
			pFile->ResumeDownload();

			bPaused = TRUE;
		}
	}

	LeaveCriticalSection(&m_Lock);

	if( bPaused )
		m_pHttpDownloadManager->ResumeTask(dwTaskId, TRUE, dwLastDownloadSize);

	return TRUE;
}

//
// check file exist
//
BOOL CNDCloudFileManager::IsFileDownloaded(tstring strUrl, tstring* pOutFilePath /*= NULL*/)
{
	// check whether we have downloaded before
	tstring strFileUrl = ComposeUrl(strUrl, 0);

	DWORD dwCrc = CalcCRC((char*)Str2Utf8(strFileUrl).c_str(), strFileUrl.length());

	map<DWORD, tstring>::iterator itr = m_mapFilePaths.find(dwCrc);
	if( itr != m_mapFilePaths.end() )
	{
		tstring str = GetNDCloudPath();
		str += _T("\\");
		str += itr->second;

		DWORD dwFileAttr = GetFileAttributes(str.c_str());
		if (dwFileAttr == INVALID_FILE_ATTRIBUTES)
		{
			return FALSE;
		}

		if( pOutFilePath != NULL )
		{
			*pOutFilePath = str;
		}
		return TRUE;
	}

	return FALSE;
}

BOOL CNDCloudFileManager::OnComplete(CNDCloudFile *pFile)
{
	EnterCriticalSection(&m_Lock);

	tstring strUrl	= pFile->GetUrl();
	tstring strPath = pFile->GetPath();
	tstring strMD5	= pFile->GetMD5();


	if( strUrl.find(_T("&session")) != -1 )
	{
		vector<tstring> vecStrings = SplitString(strUrl, strUrl.length(), _T('&'), true);
		if( vecStrings.size() >= 2 )
			strUrl = vecStrings[0];
	}

	DWORD dwCrc = CalcCRC((char*)Str2Utf8(strUrl).c_str(), strUrl.length());

	map<DWORD, CNDCloudFile*>::iterator itr_url = m_mapUrlFiles.find(dwCrc);
	if( itr_url != m_mapUrlFiles.end() )
		m_mapUrlFiles.erase(itr_url);


	map<DWORD, CNDCloudFile*>::iterator itr = m_mapFiles.find(pFile->GetTaskId());
	if( itr != m_mapFiles.end() )
	{
		CNDCloudFile* pFile = itr->second;
		delete pFile;
		pFile = NULL;

		m_mapFiles.erase(itr);
	}
	
	m_mapFilePaths[dwCrc] = strPath;
	m_mapFileMD5s[dwCrc] = strMD5;

	LeaveCriticalSection(&m_Lock);
	return TRUE;
}


BOOL CNDCloudFileManager::CheckThumbSize(int nThumbSize)
{
	if( nThumbSize != 0 && nThumbSize != 80 && nThumbSize != 120 &&
		nThumbSize != 160 && nThumbSize != 240 && nThumbSize != 320 && 
		nThumbSize != 480 && nThumbSize != 640 && nThumbSize != 960 )
		return FALSE;

	return TRUE;
}

tstring CNDCloudFileManager::ComposeUrl(tstring strUrl, int nThumbSize)
{
	//
	// "${ref-path}/edu_product/esp/assets/b02bafe4-903d-4faf-97bc-d98811c78411.pkg/1440488260000.png"
	// "/v0.1/download?path=/edu_product/esp/assets/b02bafe4-903d-4faf-97bc-d98811c78411.pkg/1440488260000.png&size=240"
	//
	int pos = strUrl.find(_T("${ref-path}"));
	if( pos == -1 )
		return _T("");

	strUrl =  strUrl.substr(_tcslen(_T("${ref-path}")));
	pos = strUrl.rfind(_T("?size="));

	if( pos != -1 )
		strUrl = strUrl.substr(0, pos);

	strUrl = EduPlatformUrlRes + strUrl;

	tstring strResult = strUrl;
	if( nThumbSize != 0 )
	{
		strResult += _T("&size=");

		TCHAR cNum[20];
		_stprintf(cNum, _T("%d"), nThumbSize);
		strResult += cNum;
	}
	return strResult;
}

void CNDCloudFileManager::SplitDownloadParameters(tstring strUrl, tstring& strPath, tstring& strSize, tstring& strSession)
{
	//
	// ${ref-path}/edu_product/esp/assets/732f79db-6e7f-41d1-978c-3a4296f6e979.pkg/fabf5d0d-2f62-450a-ba60-032ad6d7505a.zip&session=xx&size=xx
	// split out dentryId | path | size | session | attachment | name | ext
	//
	vector<tstring> vecString = SplitString(strUrl, strUrl.length(), _T('&'), false);
	map<tstring, tstring> mapPairs;

	for(int i = 0; i < vecString.size(); i++)
	{
		vector<tstring> vecPair;

		tstring str = vecString[i];
		vecPair = SplitString(str, str.length(), _T('='), false);

		if( str.find(_T("{ref-path}")) != -1 )
		{
			strPath = str;
			continue;
		}

		if( vecPair.size() < 2 )
			continue;

		tstring strKey = vecPair[0];
		tstring strVal = vecPair[1];

		mapPairs[strKey] = strVal;
	}

	strSize = mapPairs[_T("size")];
	strSession = mapPairs[_T("session")];

}

void CNDCloudFileManager::SplitFileNameExt(tstring strUrl, tstring& strFileName, tstring& strExtension)
{
	// "${ref-path}/edu_product/esp/assets/b02bafe4-903d-4faf-97bc-d98811c78411.pkg/1440488260000.png"
	int pos = strUrl.rfind('/');
	if( pos == -1 )
		return;

	strUrl = strUrl.substr(pos+1);

	// extension
	pos = strUrl.find('.');
	if( pos == -1 )
		return;

	strFileName		= strUrl.substr(0, pos);

	//we can't  combine local win32 path because of  filename is too long 
	if (strFileName.length() > 50)
	{
		TCHAR szBuffer[120] = {0};
		_tcsncpy_s(szBuffer, strFileName.c_str(), 50);
		strFileName = szBuffer;
	}

	strExtension	= strUrl.substr(pos+1);
}

DWORD CNDCloudFileManager::GetLastDownloadFileSize(tstring strRelativeFilePath)
{
	// last download file size
	DWORD dwLastFileSize = 0;
	tstring strNDCloudPath = GetNDCloudPath();

	tstring strNdsFilePath = strNDCloudPath + _T("\\");
	strNdsFilePath += strRelativeFilePath;
	strNdsFilePath += _T(".nds");

	FILE* fp = NULL;
	_tfopen_s(&fp, strNdsFilePath.c_str(), _T("rb"));

	if( fp != NULL )
	{
		fread(&dwLastFileSize, 4, 1, fp);
		fclose(fp);
	}

	return dwLastFileSize;
}

BOOL CNDCloudFileManager::LoadHistory()
{
	tstring strLocalPath = GetNDCloudPath();
	tstring strFilePath = strLocalPath + _T("\\");
	strFilePath += NDCloudFileDirectory;
	strFilePath += _T("\\history.dat");

	FILE* fp = NULL;
	_tfopen_s(&fp, strFilePath.c_str(), _T("rb"));

	if( fp == NULL )
		return FALSE;

	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if( size == 0 )
		return FALSE;


	char* pBuffer = new char[size];
	if( pBuffer == NULL )
		return FALSE;

	fread(pBuffer, size, 1, fp);
	fclose(fp);

	//
	CStream stream(pBuffer, size);

	// signature = 'NDCH'
	DWORD dwSignature = stream.ReadDWORD();
	if( dwSignature != 'HCDN')
		return FALSE;	// unknown data version

	// version
	int nVersion = stream.ReadInt();
	if( nVersion != HISTORY_DATA_FORMAT_VERSION )		// don't load this file cos it use a different data format
		return FALSE;

	// data 
	int count = stream.ReadInt();
	for(int i = 0; i < count; i++)
	{
		DWORD  dwUrlCrc = stream.ReadDWORD();
		tstring strPath = stream.ReadString();
		tstring strMD5 =  stream.ReadString();

		m_mapFilePaths[dwUrlCrc] = strPath;
		m_mapFileMD5s[dwUrlCrc] = strMD5;
	}
	
	delete pBuffer;
	pBuffer = NULL;
	return TRUE;
}

BOOL CNDCloudFileManager::SaveHistory()
{
	tstring strLocalPath = GetNDCloudPath();
	tstring strFilePath = strLocalPath + _T("\\");
	strFilePath += NDCloudFileDirectory;
	strFilePath += _T("\\history.dat");

	//
	int count = m_mapFilePaths.size();
	CStream stream(0x1000);

	// signature and version  (fixed in any version)
	stream.WriteDWORD('HCDN');
	stream.WriteInt(HISTORY_DATA_FORMAT_VERSION);

	// below would be changed in each version
	stream.WriteDWORD(count);

	map<DWORD, tstring>::iterator itr;
	map<DWORD, tstring>::iterator itr_md5 = m_mapFileMD5s.begin();

	for(itr = m_mapFilePaths.begin(); itr != m_mapFilePaths.end(); itr ++)
	{
		DWORD dwCrc = itr->first;
		tstring strPath = itr->second;

		stream.WriteDWORD(dwCrc);
		stream.WriteString(strPath);

		// server md5
		if( itr_md5 != m_mapFileMD5s.end() )
		{	
			tstring strMD5 = itr_md5->second;
			stream.WriteString(strMD5);
			itr_md5 ++;
		}
	}

	FILE* fp = NULL;
	_tfopen_s(&fp, strFilePath.c_str(), _T("wb"));

	if( fp == NULL )
		return FALSE;

	char* pBuffer = stream.GetBuffer();
	int nDataSize = stream.GetDataSize();

	fwrite(pBuffer, nDataSize, 1, fp);
	fclose(fp);

	return TRUE;
}

//
// get downloaded file path
//
tstring CNDCloudFileManager::GetDownloadedFilePath(tstring strUrl)
{
	// compose url
	tstring strFileUrl = ComposeUrl(strUrl, 0);
	if( strFileUrl == _T("") )
		return 0;

	strFileUrl = UrlEncode(Str2Utf8(strFileUrl));


	// check whether we have downloaded before
	DWORD dwCrc = CalcCRC((char*)Str2Utf8(strFileUrl).c_str(), strFileUrl.length());

	map<DWORD, tstring>::iterator itr = m_mapFilePaths.find(dwCrc);
	if( itr == m_mapFilePaths.end() )
		return _T("");
	
	return itr->second;
	
}	

tstring CNDCloudFileManager::GetNDCloudDirectory()
{
	tstring strDir = GetNDCloudPath();
	strDir += _T("\\NDCloud");

	return strDir;
}

BOOL CNDCloudFileManager::ModifyDownloadedFilePath(tstring strUrl, tstring strFileRelativePath)
{
	// compose url
	tstring strFileUrl = ComposeUrl(strUrl, 0);
	if( strFileUrl == _T("") )
		return FALSE;

	strFileUrl = UrlEncode(Str2Utf8(strFileUrl));

	// check whether we have downloaded before
	DWORD dwCrc = CalcCRC((char*)Str2Utf8(strFileUrl).c_str(), strFileUrl.length());

	EnterCriticalSection(&m_Lock);
	map<DWORD, CNDCloudFile*>::iterator itr_url = m_mapUrlFiles.find(dwCrc);
	if( itr_url != m_mapUrlFiles.end() )
	{
		CNDCloudFile* pFile = itr_url->second;
		pFile->SetPath(strFileRelativePath);
	}
	LeaveCriticalSection(&m_Lock);

	//
	map<DWORD, tstring>::iterator itr = m_mapFilePaths.find(dwCrc);
	if( itr != m_mapFilePaths.end() )
		m_mapFilePaths[dwCrc] = strFileRelativePath;

	return TRUE;
}

//
// exist file notify thread
//
void CNDCloudFileManager::ExistFileNotify()
{
	while( TRUE )
	{
		DWORD ret = WaitForSingleObject(m_hNotifyEvent, INFINITE);
		if( ret == WAIT_FAILED )
			break;

		EnterCriticalSection(&m_Lock);

		map<CHttpDelegateBase*, THttpNotify*> mapExistFileNotify = m_mapExistFileNotify;
		m_mapExistFileNotify.clear();

		LeaveCriticalSection(&m_Lock);

		map<CHttpDelegateBase*, THttpNotify*>::iterator itr;
		for(itr = mapExistFileNotify.begin(); itr != mapExistFileNotify.end(); itr++)
		{
			CHttpDelegateBase* pDelegate = itr->first;
			THttpNotify* pNotify = itr->second;

			CHttpEventSource OnTempEventSource;
			OnTempEventSource += *pDelegate;

			SendMessage(m_hOwnerWnd, WM_USER_HTTP_DOWNLOAD, (WPARAM)&OnTempEventSource, (LPARAM)pNotify);

			delete pDelegate;
			delete pNotify;
		}
	}
}

DWORD CNDCloudFileManager::ExistFileNotifyThread(LPARAM lParam)
{
	CNDCloudFileManager* pFileManager = (CNDCloudFileManager*)lParam;
	pFileManager->ExistFileNotify();

	return 0;
}

//
// most free size drive
//
#define MAX_HARD_DISK_COUNT		24
static char g_HardDiskLetters[MAX_HARD_DISK_COUNT][4] = 
{
	"c:\\",	"d:\\",	"e:\\",	"f:\\",	"g:\\",	"h:\\",
	"i:\\",	"j:\\",	"k:\\",	"l:\\",	"m:\\",	"n:\\",
	"o:\\",	"p:\\",	"q:\\",	"r:\\",	"s:\\",	"t:\\",
	"u:\\",	"v:\\",	"w:\\",	"x:\\",	"y:\\",	"z:\\"
};

tstring GetNDCloudPath()
{
	if( g_strNDCloudDrive != _T("") )
		return g_strNDCloudDrive;

	// get path from config
	tstring strLocalPath = GetLocalPath();
	strLocalPath += _T("\\Setting\\Config.ini");
	
	TCHAR szNDCloudPath[MAX_PATH];
	GetPrivateProfileString(_T("Config"), _T("NDCloudPath"), _T(""), szNDCloudPath, MAX_PATH, strLocalPath.c_str());
	
	if( szNDCloudPath[0] != _T('\0') )
	{
		//// check drive validity
		//if( GetDriveType(szNDCloudPath) == DRIVE_FIXED )
		//{
		//	g_strNDCloudDrive = szNDCloudPath;
		//	return g_strNDCloudDrive;
		//}

		//ÅÐ¶ÏÊÇ·ñ´æÔÚ
		if(PathFileExists(szNDCloudPath))
		{
			//ÅÐ¶ÏÊÇ·ñÎªÄ¿Â¼
			DWORD attributes = ::GetFileAttributes(szNDCloudPath);  
			attributes &= FILE_ATTRIBUTE_DIRECTORY;
			if (attributes == FILE_ATTRIBUTE_DIRECTORY)
			{			
				g_strNDCloudDrive = szNDCloudPath;
				return g_strNDCloudDrive;
			}
		}
	}


	// os drive
	char szSystemPath[MAX_PATH];
	GetSystemDirectoryA(szSystemPath, MAX_PATH);
	_strlwr_s(szSystemPath, MAX_PATH);

	//
	ULARGE_INTEGER MaxFreeSize;
	MaxFreeSize.QuadPart = 0;

	
	ULARGE_INTEGER AvailableToCaller, Disk, Free;
	char szTemp[MAX_PATH] = {0};

	for( int i = 0; i < MAX_HARD_DISK_COUNT; i++ )
	{
		if ( GetDriveTypeA(g_HardDiskLetters[i]) == DRIVE_FIXED )
		{
			if (GetDiskFreeSpaceExA(g_HardDiskLetters[i], &AvailableToCaller, &Disk, &Free))
			{
				if( Free.QuadPart > MaxFreeSize.QuadPart )
				{
					if( strstr(szSystemPath, g_HardDiskLetters[i]) == NULL )
					{
						MaxFreeSize = Free;
						g_strNDCloudDrive = Ansi2Str(g_HardDiskLetters[i]);
						g_strNDCloudDrive = g_strNDCloudDrive.substr(0, 2);
						g_strNDCloudDrive[0] = toupper(g_strNDCloudDrive[0]);
					}
				}
			}

		}
	}

	//
	if( g_strNDCloudDrive == _T("") )
		g_strNDCloudDrive = GetLocalPath();

	WritePrivateProfileString(_T("Config"), _T("NDCloudPath"), g_strNDCloudDrive.c_str(), strLocalPath.c_str());
	return g_strNDCloudDrive;
}

void SetNDCloudPath(tstring strPath)
{
	// get path from config
	tstring strLocalPath = GetLocalPath();
	strLocalPath += _T("\\Setting\\Config.ini");

	g_strNDCloudDrive = strPath;
	WritePrivateProfileString(_T("Config"), _T("NDCloudPath"), g_strNDCloudDrive.c_str(), strLocalPath.c_str());
}
