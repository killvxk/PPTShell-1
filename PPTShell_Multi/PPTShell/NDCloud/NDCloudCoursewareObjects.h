//-----------------------------------------------------------------------
// FileName:				NDCloudQuestion.h
//
// Desc:
//------------------------------------------------------------------------
#ifndef _NDCLOUD_COURSEWAREOBJECTS_H_
#define _NDCLOUD_COURSEWAREOBJECTS_H_

#include "Util/Singleton.h"


static TCHAR* tagCoursewareObjectsResTypeList[] = 
{
	_T("$RE0401"),//连连看
	_T("$RE0407"),//记忆卡牌
	_T("$RE0418"),//排序题 
	_T("$RE0215"),//分类表格题 
	_T("$RE0409"),//比大小 
	_T("$RE0408"),//竖式计算 
	_T("$RE0416"),//分式加减
};

class CNDCloudCoursewareObjects
{
public:
	CNDCloudCoursewareObjects(tstring strGuid, DWORD dwUserId, CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressDelegate,void* pUserData=NULL);
	~CNDCloudCoursewareObjects();

	bool PollPackState();
	bool Cancel();
	bool Pause(CHttpDelegateBase* pDelegate);
	bool Resume();

	bool OnPackStateObtained(void* param);
	bool OnPackStarted(void* param);

	bool OnCoursewareObjectsDownloaded(void* param);
	bool OnCoursewareObjectsDownloading(void* param);


protected:
	tstring					m_strGuid;
	DWORD					m_dwUserId;
	BOOL					m_bPolling;
	BOOL					m_bPolled;
	BOOL					m_bFinished;		
	BOOL					m_bPacking;
	DWORD					m_dwDownloadId;

	CHttpEventSource		m_OnProgress;
	CHttpEventSource		m_OnComplete;
	void*					m_pUserData;
	
	
};

class CNDCloudCoursewareObjectsManager
{
private:
	CNDCloudCoursewareObjectsManager();
	~CNDCloudCoursewareObjectsManager();

public:
	BOOL	Initialize();
	BOOL	Destroy();
	BOOL	DownloadCoursewareObjects(tstring strGuid, DWORD dwUserId, CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressDelegate,void* pUserData=NULL);
	BOOL	CancelDownload(tstring strGuid);
	BOOL	PauseDownload(tstring strQuestionGuid, CHttpDelegateBase* pDelegate = NULL);
	BOOL	ResumeDownload(tstring strQuestionGuid);

	DWORD	PackCoursewareObjects(tstring strQuestionGuid, DWORD dwUserId, CHttpDelegateBase& OnCompleteDelegate);

	BOOL GetCoursewareObjectsResType(LPCTSTR szCoursewareObjectsResDescription);

	DECLARE_SINGLETON_CLASS(CNDCloudCoursewareObjectsManager);


protected:
	void PollPackState();
	static DWORD WINAPI PollPackStateThread(LPARAM lParam);


protected:
	HANDLE									m_hThread;
	HANDLE									m_hEvent;
	CRITICAL_SECTION						m_Lock;

	map<tstring, CNDCloudCoursewareObjects*> m_mapCoursewareObjects;
	
};

typedef Singleton<CNDCloudCoursewareObjectsManager>		NDCloudCoursewareObjectsManager;

#endif