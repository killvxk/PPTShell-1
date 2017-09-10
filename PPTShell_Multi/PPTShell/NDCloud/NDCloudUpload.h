//-----------------------------------------------------------------------
// FileName:				NDCloudUpload.h
//
// Desc:
//------------------------------------------------------------------------
#ifndef _NDCLOUD_UPLOAD_H_
#define _NDCLOUD_UPLOAD_H_

#include "Util/Singleton.h"


enum
{
	UPLOAD_NONE,
	UPLOAD_ASSETS,
	UPLOAD_COURSEWARE,
	UPLOAD_QUESTIONS,
	UPLOAD_COURSEWAREOBJECTS,
};

//
// CNDCloudUploadTask
//
class CNDCloudUploadTask
{
public:
	CNDCloudUploadTask();
	~CNDCloudUploadTask();

	virtual void		StartUpload() = 0;

	BOOL				IsSuccess();
	tstring				GetErrorCode();
	tstring				GetErrorMessage();

	DWORD				GetUploadId();
	void				SetUploadId(DWORD dwTaskId);

	tstring				GetGuid();
	tstring				GetServerPath();
	tstring				GetServerFolderPath();
	tstring				ComposeObtainServerPathUrl(DWORD dwUserId, tstring strGuid = _T(""));

	CHttpEventSource*	GetUserCompleteDelegate();
	CHttpEventSource*	GetUserProgressDelegate();

	bool				OnUploadProgress(void* param);
	bool				OnUploadCompleted(void* param);

	void* GetUserData();

protected:
	BOOL					m_bSuccess;	
	tstring					m_strErrorCode;
	tstring					m_strErrorMessage;
	
	tstring					m_strGuid;		
	tstring					m_strServerFilePath;
	tstring					m_strServerFolderPath;

	DWORD					m_dwUserId;
	DWORD					m_dwUploadId;
	int						m_nUploadType;
	void* m_pUserData;

	CHttpEventSource		m_OnComplete;
	CHttpEventSource		m_OnProgress;
	CHttpEventSource		m_OnUserComplete;
	CHttpEventSource		m_OnUserProgress;

};

//
// CNDCloudUploadFile
//
class CNDCloudUploadFileTask : public CNDCloudUploadTask
{
public:
	CNDCloudUploadFileTask(int nUploadType,
						DWORD dwUserId, 						
						tstring strFilePath,
						CHttpDelegateBase& OnCompleteDelegate,
						CHttpDelegateBase& OnProgressDelegate, 
						CHttpDelegateBase& OnUserCompleteDelegate,
						CHttpDelegateBase& OnUserProgressDelegate,
						tstring strGuid = _T(""),						// if want to replace file then supply these two parameters
						tstring strServerFilePath = _T(""));

	~CNDCloudUploadFileTask();

 	virtual void StartUpload();

protected:
 	bool OnServerPathObtained(void* param);
	bool OnSessionObtained(void* param);
 

protected:
 	tstring					m_strFilePath;
	BOOL					m_bReplaceFile;
 
};


//
// CNDCloudUploadFolder
//
class CNDCloudUploadFolderTask : public CNDCloudUploadTask
{
public:
	CNDCloudUploadFolderTask(int nUploadType, DWORD dwUserId, tstring strResType,tstring strTitle, tstring strGuid, tstring strFolderPath,
								CHttpDelegateBase &OnCompleteDelegate, CHttpDelegateBase& OnProgressDelegate,void* pUserData=NULL);
	~CNDCloudUploadFolderTask();

	virtual void StartUpload();

protected:
	bool		OnServerPathObtained(void* pParam);
	bool		OnFolderCreated(void* param);
	bool		OnFileUploadeded(void* param);

	bool		GetDeepestFolders(LPCTSTR szFolderPath, vector<tstring>& vecFolderPaths);
	bool		GetAllFilePaths(LPCTSTR szFolderPath, map<tstring, tstring>& mapFilePaths);
	BOOL		UploadAllFiles();

protected:
	DWORD					m_dwUserId;
	tstring					m_strResType;
	tstring					m_strTitle;
	tstring					m_strFolderPath;
	
	tstring					m_strAccessUrl;
	tstring					m_strSessionId;
	
 
	CHttpEventSource		m_OnComplete;
	CHttpEventSource		m_OnProgress;

	vector<tstring>			m_vecFolderRelativePaths;
	map<tstring, tstring>	m_mapFilePaths;
	
	int						m_nFolderCreated;
	int						m_nFileUploaded;

};


//
// CNDCloudUploadManager
//
class CNDCloudUploadTaskManager
{
private:
	CNDCloudUploadTaskManager();
	~CNDCloudUploadTaskManager();

public:
	BOOL UploadFile(int nUploadType,
					DWORD dwUserId, 
					tstring strFilePath, 
					CHttpDelegateBase& OnCompleteDelegate, 
					CHttpDelegateBase& OnProgressDelegate,
					tstring strGuid = _T(""),					// if want to replace file then supply these two parameters 
					tstring strServerFilePath = _T(""));		// file full path not just folder path

	BOOL UploadFolder(int nUploadType,
					DWORD dwUserId, 
					tstring strResType,
					tstring strTitle, 
					tstring strGuid, 
					tstring strFolderPath, 
					CHttpDelegateBase& OnCompleteDelegate, 
					CHttpDelegateBase& OnProgressDelegate,
					void* pUserData=NULL);

	DECLARE_SINGLETON_CLASS(CNDCloudUploadTaskManager);

protected:
	bool OnProgress(void* param);
	bool OnComplete(void* param);

protected:
	CRITICAL_SECTION							m_Lock;
	map<DWORD, CNDCloudUploadTask*>				m_mapUploadTasks;

};

typedef Singleton<CNDCloudUploadTaskManager>		NDCloudUploadManager;

#endif