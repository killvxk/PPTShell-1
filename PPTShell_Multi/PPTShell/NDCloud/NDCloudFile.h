//-----------------------------------------------------------------------
// FileName:				NDCloudFile.h
//
// Desc:
//------------------------------------------------------------------------
#ifndef _ND_CLOUD_FILE_H_
#define _ND_CLOUD_FILE_H_

#include "Util/Singleton.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "Http/HttpDownloadManager.h"
#include "Http/HttpUploadManager.h"

class CNDCloudFile
{
public:
	CNDCloudFile(tstring strUrl, tstring strFilePath, int nLastDownloadSize,
				 CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressDelegate, PVOID pUserData = NULL, tstring strMD5 = _T(""));

	~CNDCloudFile();

	bool		OnStart(void* param);
	bool		OnProgress(void* param);
	bool		OnComplete(void* param);
	bool		OnPause(void* param);

	DWORD		GetTaskId();
	void		SetTaskId(DWORD dwTaskId);
	void		SetLastDownloadSize(DWORD dwSize);

	tstring		GetUrl();
	tstring		GetPath();
	tstring		GetMD5();

	void		PauseDownload(CHttpDelegateBase* pOnPauseDelegate);
	void		ResumeDownload();
	void		CancelDownload();
	BOOL		AddTaskCallBack(CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressDelegate, PVOID pUserData = NULL);
	BOOL		DelTaskCallBack(CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressDelegate);

protected:
	void		ReportResourceMissingError();
	bool		OnResourceMissingReported(void* param);

protected:
	CHttpEventSource			m_OnProgress;
	CHttpEventSource			m_OnComplete;
	CHttpEventSource			m_OnPause;

	tstring						m_strUrl;
	tstring						m_strRelativeFilePath;
	tstring						m_strMD5;

	DWORD						m_dwTaskId;
	DWORD						m_dwStartTime;
	DWORD						m_dwTotalElapseTime;
			

	// file
	HANDLE						m_hFile;
	HANDLE						m_hFileMapping;
	BYTE*						m_pFileData;
	int							m_nCurFileSize;
	int							m_nLastDownloadSize;

	HANDLE						m_hNdsFile;
	BOOL						m_bDirectWriteFile;

	// user data
	map<DWORD, PVOID>			m_mapOnStartUserDatas;		// (Object|Function)  <--> pUserData
	map<DWORD, PVOID>			m_mapOnProgressUserDatas;	// (Object|Function)  <--> pUserData
	map<DWORD, PVOID>			m_mapOnCompleteUserDatas;	// (Object|Function)  <--> pUserData
};

class CNDCloudFileManager
{
private:
	CNDCloudFileManager();
	~CNDCloudFileManager();

public:
	BOOL	Initialize(CHttpDownloadManager* pHttpDownloadManager);
	BOOL	Destroy();
	BOOL	SetOwner(HWND hWnd);

	DWORD	DownloadFileVideo(tstring strUrl,    tstring strGuid, tstring strName, CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressCB,void* pUserData=NULL, tstring strMD5 = _T(""));
	DWORD	DownloadFileVolume(tstring strUrl,   tstring strGuid, tstring strName, CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressCB,void* pUserData=NULL, tstring strMD5 = _T(""));
	DWORD	DownloadFileFlash(tstring strUrl,    tstring strGuid, tstring strName, CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressCB,void* pUserData=NULL, tstring strMD5 = _T(""));
	DWORD   DownloadFileImage(tstring strUrl,    tstring strGuid, tstring strName, int nThumbSize, CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressCB,void* pUserData=NULL, tstring strMD5 = _T(""));
	DWORD   DownloadFileImageEx(tstring strUrl,    tstring strGuid, tstring strName, int nThumbSize, CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressCB,void* pUserData=NULL, tstring strMD5 = _T(""));
	DWORD	DownloadFileCourse(tstring strUrl,   tstring strGuid, tstring strName, int nCourseFileType, CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressCB,void* pUserData=NULL, tstring strMD5 = _T(""));
	DWORD	DownloadFileQuestion(tstring strUrl, tstring strGuid, tstring strName, CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressCB,void* pUserData=NULL, tstring strMD5 = _T(""));
	DWORD	DownloadFileNdpCourse(tstring strUrl, tstring strGuid, tstring strName, CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressCB,void* pUserData=NULL, tstring strMD5 = _T(""));
	DWORD	DownloadFile3DResource(tstring strUrl, tstring strGuid, tstring strName, CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressCB,void* pUserData=NULL, tstring strMD5 = _T(""));
	DWORD	DownloadFileVRResource(tstring strUrl, tstring strGuid, tstring strName, CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressCB,void* pUserData=NULL, tstring strMD5 = _T(""));

	DWORD	DownloadFileSearchPlatform(tstring strUrl, tstring strGuid, tstring strName, int nFileType, CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressCB,void* pUserData=NULL, tstring strMD5 = _T(""));

	BOOL	CancelDownload(DWORD dwTaskId);
	BOOL	CancelDownload(DWORD dwTaskId, CHttpDelegateBase* pOnCompleteDelegate, CHttpDelegateBase* pOnProgressDelegate);
	BOOL	PauseDownload(DWORD dwTaskId, CHttpDelegateBase* pOnPauseDelegate = NULL);
	BOOL	ResumeDownload(DWORD dwTaskId);
	BOOL	OnComplete(CNDCloudFile* pFile);

	tstring	GetDownloadedFilePath(tstring strUrl);
	tstring GetNDCloudDirectory();
	BOOL	IsFileDownloaded(tstring strUrl, tstring* pOutFilePath = NULL);

	DECLARE_SINGLETON_CLASS(CNDCloudFileManager);

protected:
	DWORD   DownloadCloudFile(tstring strFileUrl, tstring strRelativeFilePath, DWORD dwLastFileSize, 
		CHttpDelegateBase &OnCompleteDelegate, CHttpDelegateBase& OnProgressDelegate,void* pUserData=NULL, tstring strMD5 = _T(""), tstring strSessionId = _T(""));

	DWORD   DownloadCloudFile(tstring strHost, tstring strFileUrl, tstring strRelativeFilePath, DWORD dwLastFileSize, 
		CHttpDelegateBase &OnCompleteDelegate, CHttpDelegateBase& OnProgressDelegate,void* pUserData=NULL, tstring strMD5 = _T(""), tstring strSessionId = _T(""));

	tstring	ComposeUrl(tstring strUrl, int nThumbSize);

	void	SplitDownloadParameters(tstring strUrl, tstring& strPath, tstring& strSize, tstring& strSession);
	void    SplitFileNameExt(tstring strUrl, tstring& strFileName, tstring& strExtension);
	DWORD	GetLastDownloadFileSize(tstring strRelativeFilePath);
	BOOL	CheckThumbSize(int nThumbSize);
	BOOL	LoadHistory();
	BOOL	SaveHistory();
	tstring	GetSavePath();

protected:
	void   ExistFileNotify();
	static DWORD WINAPI ExistFileNotifyThread(LPARAM lParam);

protected:
	map<DWORD, tstring>								m_mapFilePaths;				// url crc <--> full path
	map<DWORD, tstring>								m_mapFileMD5s;				// url crc <--> last file server md5

	map<DWORD, CNDCloudFile*>						m_mapFiles;					// taskId  <--> CNDCloudFile*
	map<DWORD, CNDCloudFile*>						m_mapUrlFiles;				// url crc <--> CNDCloudFile*

	CHttpDownloadManager*							m_pHttpDownloadManager;
	CRITICAL_SECTION								m_Lock;

	HWND											m_hOwnerWnd;
	HANDLE											m_hNotifyEvent;
	HANDLE											m_hNotifyThread;
	map<CHttpDelegateBase*, THttpNotify*>			m_mapExistFileNotify;
};

typedef Singleton<CNDCloudFileManager>			NDCloudFileManager;

#endif