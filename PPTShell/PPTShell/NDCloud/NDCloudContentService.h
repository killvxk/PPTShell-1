//-----------------------------------------------------------------------
// FileName:				NDCloudContentService.h
//
// Desc:
//------------------------------------------------------------------------
#ifndef _ND_CLOUD_CONTENT_SERVICE_H_
#define _ND_CLOUD_CONTENT_SERVICE_H_

#include "Util/Singleton.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "Http/HttpDownloadManager.h"

typedef enum
{
	CONTENT_NONE,
	CONTENT_ASSETS,
	CONTENT_COURSEWARE,
	CONTENT_QUESTIONS,
	CONTENT_COUSEWAREOBJECTS,

}CONTENT_TYPE;


//
// CNDCloudContentService
//
class CNDCloudContentService
{
public:
	CNDCloudContentService();
	~CNDCloudContentService();

	virtual BOOL StartService() = 0;

	BOOL					IsSuccess();
	tstring					GetErrorCode();
	tstring					GetErrorMessage();
	CHttpEventSource*		GetUserDelegate();


	DWORD					GetServiceId();
	void					SetServiceId(DWORD dwServiceId);
	void* GetUserData();
protected:
	tstring						m_strErrorCode;
	tstring						m_strErrorMessage;
	BOOL						m_bSuccess;	
	DWORD						m_dwServiceId;
	CHttpEventSource			m_OnUserDelegate;
	CHttpEventSource			m_OnCompleteDelegate;
	void* m_pUserData;
};

//
// delete service
//
class CNDCloudContentDeleteService : public CNDCloudContentService
{
public:
	CNDCloudContentDeleteService(DWORD dwUserId, tstring strGuid, 
								int nContentType, tstring strFileServerPath, 
								CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnUserDelegate,void* pUserData=NULL);

	~CNDCloudContentDeleteService();
	
	virtual BOOL StartService();

protected:
	bool	OnSessionObtained(void* param);
	bool	OnCSFileDeleted(void* param);
	bool	OnLCFileDeleted(void* param);

 
protected:	
	DWORD							m_dwUserId;
	tstring							m_strFileServerPath;
	tstring							m_strLCGuid;
	int								m_nContentType;
	
};

//
// rename service
//
class CNDCloudContentRenameService : public CNDCloudContentService
{
public:
	CNDCloudContentRenameService(tstring strGuid, tstring strTitle, tstring strJson, int nContentType, 
								CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnUserDelegate, void* pUserData=NULL);
	~CNDCloudContentRenameService();

	virtual BOOL StartService();

protected:
	bool	OnTitleRenamed(void* param);

protected:
	tstring							m_strLCGuid;
	tstring							m_strTitle;
	tstring							m_strJson;
	int								m_nContentType;


};

//
// create directory
//
class CNDCloudContentCreateDirectoryService : public CNDCloudContentService
{
public:
	CNDCloudContentCreateDirectoryService(DWORD dwUserId, tstring strFileServerPath, tstring strRelativeDirPath,
											CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnUserDelegate);

	~CNDCloudContentCreateDirectoryService();

	virtual BOOL StartService();

protected:
	bool	OnSessionObtained(void* param);
	bool	OnDirectoryCreated(void* param);

protected:
	DWORD							m_dwUserId;
	tstring							m_strFileServerPath;
	tstring							m_strRelativeDirPath;
};

//
// CNDCloudContentServiceManager
// 
class CNDCloudContentServiceManager
{
private:
	CNDCloudContentServiceManager();
	~CNDCloudContentServiceManager();

public:
	BOOL DeleteCloudFile(DWORD dwUserId, tstring strGuid, int nContentType, tstring strFileServerPath, CHttpDelegateBase& delegate,void* pUserData=NULL);
	BOOL RenameCloudFile(tstring strGuid, tstring strTitle, tstring strJson, int nContentType, CHttpDelegateBase& delegate,void* pUserData=NULL);
	BOOL CreateCloudDirectory(DWORD dwUserId, tstring strFileServerPath, tstring strRelativeDirPath, CHttpDelegateBase& delegate);
 
	DECLARE_SINGLETON_CLASS(CNDCloudContentServiceManager);

protected:
	bool OnComplete(void* param);

protected:
	CRITICAL_SECTION						m_Lock;
	map<DWORD, CNDCloudContentService*>		m_mapContentServices;

};

typedef Singleton<CNDCloudContentServiceManager>	NDCloudContentServiceManager;

#endif