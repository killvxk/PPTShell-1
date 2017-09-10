//-----------------------------------------------------------------------
// FileName:				NDCloudLocalQuestion.h
//
// Desc:
//------------------------------------------------------------------------
#ifndef _NDCLOUD_LOCAL_QUESTION_H_
#define _NDCLOUD_LOCAL_QUESTION_H_

#include "Util/Singleton.h"

//
// CNDCloudLocalQuestionManager
//
class CNDCloudLocalQuestionManager
{
private:
	CNDCloudLocalQuestionManager();
	~CNDCloudLocalQuestionManager();

public:
	BOOL Initialize();
	BOOL Destroy();

	BOOL CreateQuestion();
	BOOL InsertQuestion(tstring strGuid, bool bBasicQuestion, bool bInsertQuestion);
	BOOL UpdateQuestion(tstring strGuid, bool bBasicQuestion);
	BOOL EditQuestion(tstring strPath);
	BOOL EditQuestion(tstring strGuid, tstring strQuestionType = _T(""), tstring strQuestionPath = _T(""));
	BOOL UploadQuestion(tstring strTitle,tstring strGuid,bool bBasicQuestion,CHttpDelegateBase& OnCompleteDelegate,void* pUserData=NULL);
	BOOL GetQuestionInfo(tstring strJosnPath, tstring& strGuid, tstring& strQuestionType);
	BOOL GetQuestionCodeInfo( tstring strJosnPath, tstring& strGuid, tstring& strQuestionType,tstring& strQuestionCode );


	DECLARE_SINGLETON_CLASS(CNDCloudLocalQuestionManager);

protected:
	bool OnChapterChange(void* pNotify);
	bool OnQuestionFolderUploaded(void* param);
	bool OnQuestionModelCommited(void* param);
};

typedef Singleton<CNDCloudLocalQuestionManager> NDCloudLocalQuestionManager;

#endif