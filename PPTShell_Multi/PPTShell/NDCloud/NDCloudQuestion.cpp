//-----------------------------------------------------------------------
// FileName:				NDCloudQuestion.cpp
//
// Desc:
//------------------------------------------------------------------------
#include "stdafx.h"
#include "NDCloudAPI.h"
#include "NDCloudQuestion.h"
#include "Util/Util.h"
#include "Util/Base64.h"
#include "ThirdParty/zip/ZipWrapper.h"
#include "ThirdParty/openssl/des.h"
#include "NDCloud/NDCloudUser.h"
#include "ThirdParty/json/json.h"
#include "NDCloud/NDCloudLocalQuestion.h"
#include "DUI/ExercisesEditFloatWindow.h"
#include <fstream>


#define EduPlatformHost							_T("esp-lifecycle.web.sdp.101.com")
#define EduPlatformAddQuestionHost				_T("mooc-lms.web.sdp.101.com")
#define EduPlatformAddQuestionUrl				_T("/v1.5/exercises")
#define EduPlatformBasicQuestionEditorHost		_T("esp-slides.edu.web.sdp.101.com")
#define EduPlatformInteractQuestionEditorHost	_T("esp-ceditor-front.edu.web.sdp.101.com")

 
CNDCloudQuestion::CNDCloudQuestion(tstring strGuid, DWORD dwUserId, CHttpDelegateBase& OnCompleteDelegate, CHttpDelegateBase& OnProgressDelegate,void* pUserData)
{
	m_strGuid	= strGuid;
	m_dwUserId	= dwUserId;
	m_bPolling	= FALSE;
	m_bFinished	= FALSE;
	m_bPacking	= FALSE;
	m_bPolled	= FALSE;
	m_pUserData = pUserData;

	m_nPendingCount = 0;
	m_dwDownloadId	= 0;

	if( OnCompleteDelegate.GetFn() != NULL )
		m_OnComplete += OnCompleteDelegate;

	if( OnProgressDelegate.GetFn() != NULL )
		m_OnProgress += OnProgressDelegate;
	
}

CNDCloudQuestion::~CNDCloudQuestion()
{
	
}

bool CNDCloudQuestion::Cancel()
{
	m_OnComplete.clear();
	m_OnProgress.clear();

	return true;
}

bool CNDCloudQuestion::Pause(CHttpDelegateBase* pDelegate)
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

bool CNDCloudQuestion::Resume()
{
	if( m_dwDownloadId != 0 )
		NDCloudDownloadResume(m_dwDownloadId);

	return true;
}

bool CNDCloudQuestion::PollPackState()
{
	if( m_bFinished )
		return false;	// done already 

	if( m_bPolled )
		return true;

	if( m_bPolling )
		return true;

	tstring strUrl = NDCloudComposeUrlPackQuestionState(m_strGuid, m_dwUserId);
	NDCloudDownloadEx(strUrl, _T(""), _T("GET"), _T(""),  MakeHttpDelegate(this, &CNDCloudQuestion::OnPackStateObtained));
	
	m_bPolling = TRUE;
	return true;
}

bool CNDCloudQuestion::OnPackStateObtained(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pData[pNotify->nDataSize] = '\0';

	m_bPolling = FALSE;
	string str = Utf8ToAnsi(pNotify->pData);
	Json::Reader reader;
	Json::Value root;

	bool result = true;
	bool res = reader.parse(str, root);
	if(res)
	{
		// check state
		if(!root["archive_state"].isNull() )
		{
			tstring strState = Ansi2Str(root["archive_state"].asCString());
			if( strState == _T("ready") )
			{
				m_bPolled = TRUE;

				// can download now
				tstring strUrl = Ansi2Str(root["access_url"].asCString());
				tstring strMD5 = Ansi2Str(root["md5"].asCString());
				m_dwDownloadId = NDCloudDownloadFile(strUrl, _T(""), _T(""), CloudFileQuestion, 0, MakeHttpDelegate(this, &CNDCloudQuestion::OnQuestionDownloaded), MakeHttpDelegate(this, &CNDCloudQuestion::OnQuestionDownloading), NULL, strMD5);	
			}
			else if( strState == _T("unpack") )
			{
				// pack this question
				if( !m_bPacking )
				{
					tstring strUrl = NDCloudComposeUrlPackQuestion(m_strGuid, m_dwUserId);
					tstring strHeader = NDCloudUser::GetInstance()->GetAuthorizationHeader(EduPlatformHost, strUrl, _T("POST"));

					NDCloudDownloadEx(strUrl, strHeader, _T("POST"), _T(""),  MakeHttpDelegate(this, &CNDCloudQuestion::OnPackStarted));
					m_bPacking = TRUE;
				}
			}
			else if( strState == _T("pending") )
			{
				// temp solution!!!
				m_nPendingCount ++;
				if( m_nPendingCount >= 3 )
				{
					// pack this question
					if( !m_bPacking )
					{
						tstring strUrl = NDCloudComposeUrlPackQuestion(m_strGuid, m_dwUserId);
						tstring strHeader = NDCloudUser::GetInstance()->GetAuthorizationHeader(EduPlatformHost, strUrl, _T("POST"));

						NDCloudDownloadEx(strUrl, strHeader, _T("POST"), _T(""),  MakeHttpDelegate(this, &CNDCloudQuestion::OnPackStarted));
						m_bPacking = TRUE;
					}
				}
			}
			else if( strState == _T("error") )
			{
				// error occur
				m_bFinished = true;
				result = false;
			}
		}
		else
		{
			// "LC/RESOURCE_NOT_FOUND"
			if( !root["code"].isNull() )
			{
				m_bFinished = true;
			}
			result = false;
		}
	}
	else
	{
		m_bFinished = true;
		result = false;
	}
	if(!result&&m_OnComplete)
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
		pNotify->strFilePath="";
		pNotify->pDetailData=NULL;
		pNotify->pUserData=this->m_pUserData;

		m_OnComplete(pNotify);
	}
	return result;
}

bool CNDCloudQuestion::OnPackStarted(void *param)
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

bool CNDCloudQuestion::OnQuestionDownloaded(void* param)
{
	
	THttpNotify* pNotify = (THttpNotify*)param;
	
	// unzip
	if( pNotify->strFilePath != _T("") )
	{
		// folder
		tstring strFolder = pNotify->strFilePath;

		// f52c9431-e1c4-43ca-8db0-6e92cf7a53c3_default.zip ==> f52c9431-e1c4-43ca-8db0-6e92cf7a53c3.pkg
		int pos = strFolder.rfind('\\');
		if( pos != -1 )
			strFolder = strFolder.substr(0, pos);

		strFolder +=  _T("\\");
		strFolder += m_strGuid;
		strFolder += _T(".pkg");
 
		CUnZipper UnZipper;
		UnZipper.UnZip(Str2Ansi(pNotify->strFilePath).c_str(), Str2Ansi(strFolder).c_str());

		pNotify->strFilePath = strFolder;
 
	}
	pNotify->pUserData = m_pUserData;
	m_OnComplete(param);

	m_bFinished = TRUE;
	return true;
}

bool CNDCloudQuestion::OnQuestionDownloading(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;

	if( m_OnProgress )
		m_OnProgress(param);

	return true;
}

//-----------------------------------------------------------
// CNDCloudQuestionManager
//
CNDCloudQuestionManager::CNDCloudQuestionManager()
{
	m_bLoadJson = FALSE;
	InitializeCriticalSection(&m_Lock);
}

CNDCloudQuestionManager::~CNDCloudQuestionManager()
{
	Destroy();
	DeleteCriticalSection(&m_Lock);
}

BOOL CNDCloudQuestionManager::Initialize()
{
	m_hEvent	= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hThread	= CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PollPackStateThread, this, 0, NULL);

	return TRUE;
}

BOOL CNDCloudQuestionManager::Destroy()
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

void CNDCloudQuestionManager::PollPackState()
{
	while(TRUE)
	{
		DWORD ret = WaitForSingleObject(m_hEvent, 1000);
		if( ret == WAIT_FAILED )
			break;

		EnterCriticalSection(&m_Lock);

		map<tstring, CNDCloudQuestion*>::iterator itr;
		for(itr = m_mapQuestions.begin(); itr != m_mapQuestions.end(); )
		{
			CNDCloudQuestion* pQuestion = itr->second;
			bool res = pQuestion->PollPackState();
			if( !res )
			{
				// already downloading file can remove this now
				delete pQuestion;
				pQuestion = NULL;
				
				m_mapQuestions.erase(itr++);
			}
			else
				itr++;
		}

		LeaveCriticalSection(&m_Lock);
	}
}

DWORD WINAPI CNDCloudQuestionManager::PollPackStateThread(LPARAM lParam)
{
	CNDCloudQuestionManager* pThis = (CNDCloudQuestionManager*)lParam;
	pThis->PollPackState();

	return 0;
}

BOOL CNDCloudQuestionManager::DownloadQuestion(tstring strQuestionGuid, 
											   DWORD dwUserId, 
											   CHttpDelegateBase& OnCompleteDelegate, 
											   CHttpDelegateBase& OnProgressDelegate,void* pUserData)
{
	CNDCloudQuestion* pQuestion = new CNDCloudQuestion(strQuestionGuid, dwUserId, OnCompleteDelegate, OnProgressDelegate,pUserData);
	if( pQuestion == NULL )
		return FALSE;
	
	EnterCriticalSection(&m_Lock);
	m_mapQuestions[strQuestionGuid] = pQuestion;
	LeaveCriticalSection(&m_Lock);

	SetEvent(m_hEvent);

	return TRUE;
}

//
// cancel download
//
BOOL CNDCloudQuestionManager::CancelDownload(tstring strQuestionGuid)
{
	EnterCriticalSection(&m_Lock);

	map<tstring, CNDCloudQuestion*>::iterator itr = m_mapQuestions.find(strQuestionGuid);
	if( itr != m_mapQuestions.end() )
	{
		CNDCloudQuestion* pQuestion = itr->second;
		pQuestion->Cancel();
	}

	LeaveCriticalSection(&m_Lock);

	return TRUE;
}

//
// pause download
//
BOOL CNDCloudQuestionManager::PauseDownload(tstring strQuestionGuid, CHttpDelegateBase* pDelegate)
{
	EnterCriticalSection(&m_Lock);

	map<tstring, CNDCloudQuestion*>::iterator itr = m_mapQuestions.find(strQuestionGuid);
	if( itr != m_mapQuestions.end() )
	{
		CNDCloudQuestion* pQuestion = itr->second;
		pQuestion->Pause(pDelegate);
	}

	LeaveCriticalSection(&m_Lock);

	
	return TRUE;
}

//
// resume download
//
BOOL CNDCloudQuestionManager::ResumeDownload(tstring strQuestionGuid)
{
	EnterCriticalSection(&m_Lock);

	map<tstring, CNDCloudQuestion*>::iterator itr = m_mapQuestions.find(strQuestionGuid);
	if( itr != m_mapQuestions.end() )
	{
		CNDCloudQuestion* pQuestion = itr->second;
		pQuestion->Resume();
	}

	LeaveCriticalSection(&m_Lock);

	return TRUE;
}

// 
// pack question
//
DWORD CNDCloudQuestionManager::PackQuestion(tstring strQuestionGuid, DWORD dwUserId, CHttpDelegateBase& OnCompleteDelegate)
{
	tstring strUrl = NDCloudComposeUrlPackQuestion(strQuestionGuid, dwUserId);
	tstring strHeader = NDCloudUser::GetInstance()->GetAuthorizationHeader(EduPlatformHost, strUrl, _T("POST"));

	DWORD dwTaskId = NDCloudDownloadEx(strUrl, strHeader, _T("POST"), _T(""),  OnCompleteDelegate);
	return dwTaskId;
}
BOOL  CNDCloudQuestionManager::LoadJsonQuestionResType(){
	if (!m_bLoadJson){
		m_bLoadJson = true;
		tstring strJsonPath = GetQuestionJsonPath();
		Json::Value root;
		Json::Reader reader; 
		std::ifstream is;  
		std::string curLocale = setlocale(LC_ALL,NULL);
		setlocale(LC_ALL,"chs");
		is.open(strJsonPath.c_str());
		setlocale(LC_ALL,curLocale.c_str());
		if(!is.is_open()){
			return FALSE;
		}
		try{
			if (reader.parse(is, root,FALSE)) {
				Json::Value item = root["basic_question"];
				if (item.isArray()){
					int iSize = item.size();  
					for ( int nIndex = 0;nIndex < iSize;++ nIndex )   
					{
						Json::Value subitem = item[nIndex];
						if (subitem.isObject()){
							TagQuestionJsonResType nType;
							nType.nQuestionType = BASIC_QUESTION;
							nType.szQuestionResCode = subitem["code"].asString();
							nType.szQuestionResType = subitem["type"].asString();
							nType.szQuestionResLabel =  Utf8ToAnsi(subitem["label"].asString().c_str());
							m_vQuestionsType.push_back(nType);
						}
					}
				}
				item = root["interact_question"];
				if (item.isArray()){
					int iSize = item.size();  
					for ( int nIndex = 0;nIndex < iSize;++ nIndex )   
					{
						Json::Value subitem = item[nIndex];
						if (subitem.isObject()){
							TagQuestionJsonResType nType;
							nType.nQuestionType = INTERACT_QUESTION;
							nType.szQuestionResCode = subitem["code"].asString();
							nType.szQuestionResType = subitem["type"].asString();
							nType.szQuestionResLabel = Utf8ToAnsi(subitem["label"].asString().c_str());
							m_vQuestionsType.push_back(nType);
						}
					}
				}
			}
		}catch(...){

		}
	}
	return true;
}
tstring CNDCloudQuestionManager::FindQuestionResType(tagQuestionOptType ntype,tstring nUrl){
	LoadJsonQuestionResType();

	//strUrl = "http://localhost:3001/prepare/edit.html?id=be9561e8-6d71-4e1d-8a8c-1ed4455242cd&question_type=textentry"
	//http://localhost:3001/questions/question.html#/3e375d57-7e04-4bbe-985e-9d222529ba25/edit/choice?noinsert=false&chapter_id=a10f58dc-e6ab-4d16-9699-c1fab3e154d0&sys=prepare&_t=1449737603006&_lang_=zh_CN&space=personal&token_info=U2FsdGVkX1966MNXWk6CAdlFs3R%2BiDqC3%2B9ZRmL7iIqRuhbFwqHTt1yQEn1xoI41vchB8p%2BavTbJJWF%2Fv7WjZY4aNH5DLg5%2BVsfkgBhOllTVE1e12%2B6xKESea8FAgxT%2BJCfQkW1vQ2uAXaReLcA%2FP6UEdRaCFAgD0AzD2ggnU2aZrplku23LF%2F2vVr0DmQmNECexvGX48Ru%2BWuZeL1OyOGq%2BS%2FoP9GPM1nJCuQS9DS1egCn6BSCphte2YXjs448ccSz%2FYxEJE4BNVd63EUnqK7bgeAcMteUzhtyKt8MMdeaj9HZayI64vod3sgBPg5sua2MScnK8Fjz3l8v84N3XaVOdb9jfivJ6oKPwO2hGe4kWE0%2FHzv27yc7lHkjCoJW5fBZqFbLph1hjmlSi7bcOI6uT5Au5J32Q5KE2FiJ0y5drslEGOQF5Z1nnxBPEi9ZCv4g8%2B%2FZZ7cQZNudEUNpAYSTCnwHAeUmxb%2F1jHWnmZNJs9JYOamH7FRyyZAD1DpEGrpwA74HAhDMkfL4Ebb14a2jrYsfego%2Bf8zT4%2BQ2oqUOZvXg59rbIvsCwVSe7QkM6JL7Wd%2B0HFdHC3p7s%2FbJ%2BMw%3D%3D&chapter_name=%E4%B8%80%20%E4%B8%A4%E3%80%81%E4%B8%89%E4%BD%8D%E6%95%B0%E4%B9%98%E4%B8%80%E4%BD%8D%E6%95%B0
	//http://localhost:3001/interaction/#/lego?rnd=1456381106515&area=home&token_info=&id=e44ee97a-05a2-40ac-9f23-019e51dfaf69&question_type=nd_lego&_lang_=zh_CN&space=personal&chapter_id=&coverage=&english_card_type=&grade=&oper=&sys=pptshell&is_modify=&question_base=
	tstring strAppDir = GetLocalPath();
	tstring strQuestionFolder = strAppDir;
	string nGuid = _T("");
	
	tstring nQuestionFlag = _T("question.html#/");
	tstring nInteractionFlag = _T("interaction/#/");
	tstring nIdFlag = _T("&id=");
	

	tstring strPath = ExercisesEditUI::GetInstance()->GetExercisePath();

	int nFind = nUrl.find(nQuestionFlag);
	if (nFind!=-1){
		int nStart = nFind+nQuestionFlag.length();
		int nCount = 36;
		if (nStart+nCount<nUrl.length()){
			nGuid = nUrl.substr(nStart,nCount);
			strQuestionFolder += _T("\\Package\\nodejs\\app\\userdatas\\edu\\esp\\questions");
		}
	}
	nFind = nUrl.find(nInteractionFlag);
	if (nFind!=-1){
		nFind = nUrl.find(nIdFlag);
		if (nFind!=-1){
			int nStart = nFind+nIdFlag.length();
			int nCount = 36;
			if (nStart+nCount<nUrl.length()){
				nGuid = nUrl.substr(nStart,nCount);
				strQuestionFolder += _T("\\Package\\nodejs\\app\\userdatas\\edu\\esp\\interaction");
			}
		}
	}

	TCHAR szMetaFilePath[2048];
	if (ntype==QUESTIONTYPE_NOEXIST){
		_stprintf_s(szMetaFilePath, _T("%s\\%s.pkg\\metadata.json"), strQuestionFolder.c_str(), nGuid.c_str());
	}else if (ntype==QUESTIONTYPE_EXIST){
		if (strPath.find(nGuid)==-1){
			strPath = _T("");
		}
		_stprintf_s(szMetaFilePath, _T("%s\\metadata.json"), strPath.c_str());	
	}

	string nQuestionType;
	string nQuestionCode;

	NDCloudLocalQuestionManager::GetInstance()->GetQuestionCodeInfo(szMetaFilePath, nGuid, nQuestionType,nQuestionCode);

	tstring nResult = _T("");
	for(int i=0;i<m_vQuestionsType.size();i++){
		if (nQuestionCode == m_vQuestionsType[i].szQuestionResCode){
			nResult = m_vQuestionsType[i].szQuestionResLabel.c_str();
			break;
		}
	}
	if (nResult.length()==0)
	{
		for(int i=0;i<m_vQuestionsType.size();i++){
			tstring nUrlRestype = _T("/")+m_vQuestionsType[i].szQuestionResType+("?");
			tstring nQustionRestype = _T("&question_type=")+m_vQuestionsType[i].szQuestionResType+("&");
			if (nUrl.find(nQustionRestype)!=-1){
				nResult = m_vQuestionsType[i].szQuestionResLabel.c_str();
				break;
			}
			if (nUrl.find(nUrlRestype)!=-1){
				nResult = m_vQuestionsType[i].szQuestionResLabel.c_str();
				break;
			}
		}
	}
	return nResult;
}
TCHAR* CNDCloudQuestionManager::GetQuestionResType( LPCTSTR szQuestionResDescription )
{
	LoadJsonQuestionResType();
	tstring nResDesc(szQuestionResDescription);
	for(int i=0;i<m_vQuestionsType.size();i++){
		if (m_vQuestionsType[i].szQuestionResLabel ==nResDesc){
			return (TCHAR*)(m_vQuestionsType[i].szQuestionResType.c_str());
		}
		if (m_vQuestionsType[i].szQuestionResCode ==nResDesc){
			return (TCHAR*)(m_vQuestionsType[i].szQuestionResType.c_str());
		}
	}
	/*
	// find by description
	for(int i = 0;i<sizeof(tagQuestionResTypeList)/sizeof(tagQuestionResTypeList[0]);i++)
	{
		if(_tcsicmp(tagQuestionResTypeList[i].szQuestionResDescription , szQuestionResDescription) == 0 )
		{
			return tagQuestionResTypeList[i].szQuestionResType;
		}
	}

	// find by code
	for(int i = 0;i<sizeof(tagQuestionResTypeListEx)/sizeof(tagQuestionResTypeListEx[0]);i++)
	{
		if(_tcsicmp(tagQuestionResTypeListEx[i].szQuestionResDescription , szQuestionResDescription) == 0 )
		{
			return tagQuestionResTypeListEx[i].szQuestionResType;
		}
	}
	*/
	return _T("");
}

TCHAR* CNDCloudQuestionManager::GetQuestionResDesc(LPCTSTR szQuestionResType)
{
	LoadJsonQuestionResType();
	tstring nResType(szQuestionResType);
	for(int i=0;i<m_vQuestionsType.size();i++){
		if (m_vQuestionsType[i].szQuestionResType ==nResType){
			return (TCHAR*)(m_vQuestionsType[i].szQuestionResLabel.c_str());
		}
	}
	/*
	for(int i = 0;i<sizeof(tagQuestionResTypeList)/sizeof(tagQuestionResTypeList[0]);i++)
	{
		if(_tcsicmp(tagQuestionResTypeList[i].szQuestionResType , szQuestionResType) == 0 )
		{
			return tagQuestionResTypeList[i].szQuestionResDescription;
		}
	}
	*/

	return _T("");

}

//
// add question
//
BOOL CNDCloudQuestionManager::AddQuestion(tstring strChapterGuid, tstring strQuestionType, 
										  tstring strMaterialGuid, tstring strCategories,
										  bool bBasicQuestion, CHttpDelegateBase& delegate)
{
	m_strChapterGuid = strChapterGuid;
	m_bBasicQuestion = bBasicQuestion;
	m_strQuestionType = strQuestionType;

	if( delegate.GetFn() != NULL )
		m_AddQuestionNotify += delegate;

	Json::Value root;
	Json::Value materialItem;
	Json::Value categoriesArray(Json::arrayValue);
	
	root["target"]			= "person";
	root["sub_type"]		= Str2Utf8(strQuestionType);
	root["chapter_id"]		= Str2Utf8(strChapterGuid);
	root["org_id"]			= "";

	categoriesArray.append(strCategories);

	materialItem["identifier"]	= Str2Utf8(strMaterialGuid);
	materialItem["categories"] = categoriesArray;

	root["material"]		= materialItem;

	if( bBasicQuestion )
		root["exercise_type"]	= "question";
	else
		root["exercise_type"]	= "coursewareobject";

	Json::FastWriter writter;
	string str = writter.write(root);

	// Authorization header
	string strAuthorization = NDCloudUser::GetInstance()->GetAuthorizationHeader(Str2Ansi(EduPlatformAddQuestionHost), EduPlatformAddQuestionUrl, "POST");

	tstring strHeader = _T("Content-Type: application/json");
	strHeader += _T("\r\n");
	strHeader += Ansi2Str(strAuthorization);

	HttpDownloadManager::GetInstance()->AddTask(EduPlatformAddQuestionHost, EduPlatformAddQuestionUrl,
												strHeader.c_str(), _T("POST"), str.c_str(), INTERNET_DEFAULT_HTTP_PORT,
												MakeHttpDelegate(this, &CNDCloudQuestionManager::OnQuestionAdded), MakeHttpDelegate(NULL), MakeHttpDelegate(NULL));


	return TRUE;
}


bool CNDCloudQuestionManager::OnQuestionAdded(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pData[pNotify->nDataSize] = '\0';

	string str = Utf8ToAnsi(pNotify->pData);

	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(str, root);
	if( !res )
		return false;

	if( root["identifier"].isNull() )
		return false;

	tstring strQuestionGuid = root["identifier"].asCString();

	// user id
	DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();
	TCHAR szUserId[MAX_PATH];
	_stprintf_s(szUserId, _T("%d"), dwUserId);

	m_strUserId = szUserId;

	// compose question edit url
	tstring strLoginResonse = NDCloudUser::GetInstance()->GetLoginResponse();
	tstring strUrl;
	
	if( m_bBasicQuestion )
		strUrl = ComposeBasicQuestionEditUrl(strLoginResonse, szUserId, strQuestionGuid, m_strChapterGuid);
	else
		strUrl = ComposeInteractQuestionEditUrl(strLoginResonse, szUserId, strQuestionGuid, m_strChapterGuid, m_strQuestionType);

	THttpNotify notify;
	notify.pData = (char*)strUrl.c_str();
	
	if( m_AddQuestionNotify )
		m_AddQuestionNotify(&notify);

	return true;
}

//
// compose basic question url
//
tstring CNDCloudQuestionManager::ComposeBasicQuestionEditUrl(tstring strTokenInfo, tstring strUserId, tstring strQuestionGuid, tstring strChapterGuid)
{
	unsigned char szOutBuffer[1024] = {0};
	char szEncodedTokenInfo[1024] = {0};
	int nOutDataLen = 0;

	bool res = TripleDESEncrypt((unsigned char*)strTokenInfo.c_str(), strTokenInfo.length(), szOutBuffer, &nOutDataLen, "13465c85caab4dfd8b1cb5093131c6d0");
	if( !res )
		return _T("");
 

	// base64
	base64_encode((char*)szOutBuffer, nOutDataLen, szEncodedTokenInfo, sizeof(szEncodedTokenInfo));

	tstring strEncodedTokenInfo = UrlEncode(Ansi2Str(szEncodedTokenInfo), false);

	TCHAR szUrl[2048];
	_stprintf_s(szUrl, _T("/questions/%s/edit?sys=prepare&creator_id=%s&chapter_id=%s&_lang=zh_CN&space=personal&token_info=%s"),
				strQuestionGuid.c_str(), strUserId.c_str(), strChapterGuid.c_str(), strEncodedTokenInfo.c_str());

	tstring strUrl = szUrl;
	strUrl = EduPlatformBasicQuestionEditorHost + strUrl;

	return strUrl;
	
}

//
// compose interaction question url
//
tstring CNDCloudQuestionManager::ComposeInteractQuestionEditUrl(tstring strTokenInfo, tstring strUserId, tstring strQuestionGuid, tstring strChapterGuid, tstring strQuestionType)
{
	unsigned char szOutBuffer[1024] = {0};
	char szEncodedTokenInfo[1024] = {0};
	int nOutDataLen = 0;

	bool res = TripleDESEncrypt((unsigned char*)strTokenInfo.c_str(), strTokenInfo.length(), szOutBuffer, &nOutDataLen, "13465c85caab4dfd8b1cb5093131c6d0");
	if( !res )
		return _T("");


	// base64
	base64_encode((char*)szOutBuffer, nOutDataLen, szEncodedTokenInfo, sizeof(szEncodedTokenInfo));

	tstring strEncodedTokenInfo = UrlEncode(Ansi2Str(szEncodedTokenInfo), false);

	TCHAR szUrl[2048];
	_stprintf_s(szUrl, _T("/#/home?question_type=%s&id=%s&chapter_id=%s&coverage=&_lang_=zh_CN&space=personal&token_info=%s"),
						strQuestionType.c_str(), strQuestionGuid.c_str(), strChapterGuid.c_str(), strEncodedTokenInfo.c_str());


	tstring strUrl = szUrl;
	strUrl = EduPlatformInteractQuestionEditorHost + strUrl;

	return strUrl;
}

//
// 3DES Encrypt using openssl
//
bool CNDCloudQuestionManager::TripleDESEncrypt( unsigned char* data,int datalen, unsigned char* outdata,int* outlen, const char* password)
{
	//
	int tmplen = 0;
	int outdatalen = 0;

	const EVP_CIPHER *cipher;
	const EVP_MD *dgst = NULL;

	unsigned char key[EVP_MAX_KEY_LENGTH], iv[EVP_MAX_IV_LENGTH];
	unsigned char salt[PKCS5_SALT_LEN] = {0};

	memcpy(salt, "\x12\x34\x56\x78\x12\x34\x56\x78", 8);
 
	OpenSSL_add_all_algorithms();

	cipher	= EVP_get_cipherbyname("des3");
	dgst	= EVP_md5();
	

	if( !EVP_BytesToKey(cipher, dgst, salt, (unsigned char*)password, strlen(password), 1, key, iv) )
		return false;

	// add Salt
	const char magic[] = "Salted__";
	memcpy(outdata, magic, 8);
	outdatalen += 8;

	memcpy(outdata+outdatalen, salt, 8);
	outdatalen += 8;


	// encrypt
	EVP_CIPHER_CTX ctx;
	EVP_CIPHER_CTX_init(&ctx);
	EVP_EncryptInit_ex(&ctx,EVP_des_ede3_cbc(),NULL,key,iv);

	if(!EVP_EncryptUpdate(&ctx, outdata + outdatalen, &tmplen, (unsigned char*)data, datalen))
	{
		EVP_CIPHER_CTX_cleanup(&ctx);
		return false;
	}

	outdatalen += tmplen;

	if( !EVP_EncryptFinal_ex(&ctx, outdata + outdatalen, &tmplen) )
	{
		EVP_CIPHER_CTX_cleanup(&ctx);	
		return false;
	}

	outdatalen += tmplen;
	EVP_CIPHER_CTX_cleanup(&ctx);


	*outlen = outdatalen;
	return true;
}

//
// convert question file to course file
//
BOOL CNDCloudQuestionManager::ConvertQuestionToCourseFile(tstring strGuid, tstring strQuestionType, tstring strQuestionName, tstring strPath )
{
	// DON'T NEED TO CONVERT IN NEW NODEJS VERSION!!!
	return TRUE;


	tstring strMainXmlPath = strPath;
	strMainXmlPath += _T("\\main.xml");

	tstring strItemXmlPath = strPath;
	strItemXmlPath += _T("\\item.xml");

	if(GetFileAttributes(strItemXmlPath.c_str()) == INVALID_FILE_ATTRIBUTES)
	{
		tstring strItemJsonPath = strPath;
		strItemJsonPath += _T("\\item.json");
		if(GetFileAttributes(strItemJsonPath.c_str()) != INVALID_FILE_ATTRIBUTES)
		{
			::MoveFileEx(strItemJsonPath.c_str(), strItemXmlPath.c_str(), MOVEFILE_COPY_ALLOWED|MOVEFILE_REPLACE_EXISTING);
		}
	}
	TiXmlDocument doc;
	BOOL bRet = FALSE;

	// assemble
	tstring strXmlFilePath = GetLocalPath();
	strXmlFilePath += _T("\\Template\\question\\main.xml");

	// load template file
	bool res = doc.LoadFile(strXmlFilePath.c_str());
	if( !res )
		return FALSE;

	TiXmlElement* pRootElement = doc.FirstChildElement();
	if( pRootElement == NULL )
		return FALSE;

	TiXmlElement* pPagesElement = GetElementsByTagName(pRootElement, "pages");
	if( pPagesElement == NULL )
		return FALSE;
	
	CTime tm = CTime::GetCurrentTime();
	TCHAR szHref[256];
	_stprintf(szHref, _T("pages/page.xml?v=%u"), tm.GetTime());

	TiXmlElement *pPage= new TiXmlElement("page");

	pPage->SetAttribute(_T("id"),strGuid.c_str());
	pPage->SetAttribute(_T("name"),strGuid.c_str());
	pPage->SetAttribute(_T("href"),szHref);
	pPage->SetAttribute(_T("preview"),_T(""));
	pPage->SetAttribute(_T("reportable"),_T("true"));
	pPagesElement->LinkEndChild(pPage);

	res = doc.SaveFile(strMainXmlPath.c_str());
	if(!res)
		return FALSE;

	strXmlFilePath = GetLocalPath();
	strXmlFilePath += _T("\\Template\\question\\pages\\page.xml");

	// save
	res = doc.LoadFile(strXmlFilePath.c_str());
	if( !res )
		return FALSE;

	pRootElement = doc.FirstChildElement();
	if( pRootElement == NULL )
		return FALSE;

	TiXmlElement* pModulesElement = GetElementsByTagName(pRootElement, "modules");
	if(pModulesElement == NULL )
		return FALSE;

	TiXmlElement*  pAddonModuleElement = pModulesElement->FirstChildElement();
	if(pAddonModuleElement == NULL )
		return FALSE;
	
	TiXmlElement* pPropertiesElement = GetElementsByTagName(pAddonModuleElement, "properties");
	if(pPropertiesElement == NULL )
		return FALSE;
	
	TiXmlElement* pPropertyElement = pPropertiesElement->FirstChildElement();
	while(pPropertyElement)
	{
		const char * pName = pPropertyElement->Attribute(_T("name"));
		if(stricmp(pName, _T("id")) == 0)
			pPropertyElement->SetAttribute(_T("value"), strGuid.c_str());
		
		else if(stricmp(pName, _T("type_code")) == 0)
			pPropertyElement->SetAttribute(_T("value"), Str2Utf8(strQuestionType).c_str());
		
		else if(stricmp(pName, _T("type_name")) == 0)
			pPropertyElement->SetAttribute(_T("value"), Str2Utf8(strQuestionName).c_str());
		
		else if(stricmp(pName, _T("url")) == 0)
		{
			TCHAR szValue[256];
			_stprintf(szValue, _T("${ref-path}/%s/esp/questions/%s.pkg/item.xml"), _T("edu"), strGuid.c_str());
			pPropertyElement->SetAttribute(_T("value"), szValue);
		}

		pPropertyElement = pPropertyElement->NextSiblingElement();
	}

	tstring strPageDicPath = strPath;
	strPageDicPath += _T("\\pages\\");
	createDirWithFullPath((char *)strPageDicPath.c_str());

	tstring strPageXmlPath = strPath;
	strPageXmlPath += _T("\\pages\\page.xml");

	res = doc.SaveFile(strPageXmlPath.c_str());
	if( !res )
		return FALSE;
	

	//move _ref\\edu\\esp\\questions
	tstring strItemDicPath = strPath;

	strItemDicPath += _T("\\_ref\\edu\\esp\\questions\\");
	strItemDicPath += strGuid.c_str();
	strItemDicPath += _T(".pkg\\");

	createDirWithFullPath((char *)strItemDicPath.c_str());

	tstring strNewItemXmlPath = strItemDicPath;
	strNewItemXmlPath += _T("\\item.xml");

	::MoveFileEx(strItemXmlPath.c_str(), strNewItemXmlPath.c_str(), MOVEFILE_COPY_ALLOWED|MOVEFILE_REPLACE_EXISTING);
	return TRUE;

}
