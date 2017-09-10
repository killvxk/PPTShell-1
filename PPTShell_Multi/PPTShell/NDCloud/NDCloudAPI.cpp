//-----------------------------------------------------------------------
// FileName:				NDCloudAPI.cpp
//
// Desc:
//------------------------------------------------------------------------
#include "stdafx.h"
#include "NDCloudAPI.h"
#include "NDCloudPreLoader.h"
#include "NDCloudQuestion.h"
#include "NDCloudCoursewareObjects.h"
#include "Http/HttpDownload.h"
#include "Http/HttpDownloadManager.h"
#include "Util/Util.h"
#include "ThirdParty/json/json.h"
#include "ThirdParty/Tinyxml/tinyxml.h"
#include "EventCenter/EventDefine.h"


#define EduPlatformHost								_T("esp-lifecycle.web.sdp.101.com")

#define EduPlatformUrlCategories					_T("/v0.6/categories/relations?patternPath=K12")
#define EduPlatformUrlBookInfo						_T("/v0.6/teachingmaterials/actions/query?&include=TI,EDU,CG&category=K12%s&relation&coverage&prop&words&limit=(%d,%d)")
#define EduPlatformUrlChapterInfo					_T("/v0.6/teachingmaterials/%s/chapters/none/subitems")



#define EduPlatformUrlCourseInfo					_T("/v0.6/coursewares/actions/query?words%s&limit=(%d,%d)&include=%s&category&relation%s&coverage=%s")
#define EduPlatformUrlPictureInfo					_T("/v0.6/assets/actions/query?words%s&limit=(%d,%d)&include=%s&category=$RA0101&relation%s&coverage=%s")
#define EduPlatformUrlVolumeInfo					_T("/v0.6/assets/actions/query?words%s&limit=(%d,%d)&include=%s&category=$RA0102&relation%s&coverage=%s")
#define EduPlatformUrlVideoInfo						_T("/v0.6/assets/actions/query?words%s&limit=(%d,%d)&include=%s&category=$RA0103&relation%s&coverage=%s")
#define EduPlatformUrlFlashInfo						_T("/v0.6/assets/actions/query?words%s&limit=(%d,%d)&include=%s&category=$RA0104&relation%s&coverage=%s")
#define EduPlatformUrlPPTTemplateInfo				_T("/v0.6/assets/actions/query?words%s&limit=(%d,%d)&include=%s&category=$RA0501&category%s&coverage=%s")
#define EduPlatformUrl3DResourceInfo				_T("/v0.6/assets/actions/query?words%s&limit=(%d,%d)&include=%s&category=$RT0209 and $F070004&relation%s&coverage=%s")
#define EduPlatformUrlVRResourceInfo				_T("/v0.6/assets/actions/query?words%s&limit=(%d,%d)&include=%s&category=$RT0400 and $F070004%s&relation%s&coverage=%s")

#define EduPlatformUrlQuestionInfo					_T("/v0.6/questions/actions/query?words%s&limit=(%d,%d)&include=TI,CG,LC&category=$RE0200&relation%s&coverage=%s")
#define EduPlatformUrlCoursewareObjectsInfo			_T("/v0.6/coursewareobjects/actions/query?words%s&limit=(%d,%d)&include=TI,CG,LC&relation%s&coverage=%s")
#define EduPlatformUrlToolsInfo						_T("/v0.6/tools/actions/query?words%s&limit=(%d,%d)&include=TI,CG,LC&relation%s&coverage=%s")

#define EduPlatformUrlPackQuestion					_T("/v0.6/questions/%s/archive?uid=%d")
#define EduPlatformUrlPackQuestionState				_T("/v0.6/questions/%s/archiveinfo?uid=%d")

#define EduPlatformUrlPackCoursewareObjects			_T("/v0.6/coursewareobjects/%s/archive?uid=%d")
#define EduPlatformUrlPackCoursewareObjectsState	_T("/v0.6/coursewareobjects/%s/archiveinfo?uid=%d")

#define EduPlatformUrlPPTThumbnail					_T("/v0.6/coursewares/%s/previews")

#define EduPlatformUrlCategoryInfo					_T("/v0.6/categories/$S/datas?words=%s&limit=(%d,%d)")
//--------------------------------------------------------------------------------
// Global variables
//
CCategoryTree*		  g_pCategoryTree		 = NULL;
CChapterTree*		  g_pChapterTree		 = NULL;
tstring				  g_strChapterGUID		 = _T("");
tstring				  g_strBookGUID			 = _T("");

//
// Initialize 
//
BOOL NDCloudInitialize(CHttpDownloadManager* pHttpManager)
{
	NDCloudFileManager::GetInstance()->Initialize(pHttpManager);
	NDCloudPreLoader::GetInstance()->Initialize();
	NDCloudQuestionManager::GetInstance()->Initialize();
	NDCloudCoursewareObjectsManager::GetInstance()->Initialize();

	if( g_pCategoryTree == NULL )
		g_pCategoryTree = new CCategoryTree;
 
	return TRUE;
}

//
// Destroy
//
BOOL NDCloudDestroy()
{
	NDCloudPreLoader::GetInstance()->Destory();
	NDCloudFileManager::GetInstance()->Destroy();
	HttpDownloadManager::GetInstance()->Destroy();

	return TRUE;
}

//
// Download with complete notify function
//
DWORD NDCloudDownload(tstring strUrl, CHttpDelegateBase& OnCompleteCB, void* pUserData /*= NULL*/)
{
	return HttpDownloadManager::GetInstance()->AddTask(EduPlatformHost, 
														strUrl.c_str(), 
														_T(""), 
														_T("GET"),
														_T(""),
														INTERNET_DEFAULT_HTTP_PORT, 
														OnCompleteCB, 
														MakeHttpDelegate(NULL), 
														MakeHttpDelegate(NULL),
														TRUE,
														FALSE,
														0,
														pUserData);
}

//
// Download with complete notify function and progress notify function
//
DWORD NDCloudDownload(tstring strUrl, CHttpDelegateBase& OnCompleteCB, CHttpDelegateBase& OnProgressCB, void* pUserData /*= NULL*/)
{
	return HttpDownloadManager::GetInstance()->AddTask(EduPlatformHost, 
														strUrl.c_str(), 
														_T(""), 
														_T("GET"), 
														_T(""), 
														INTERNET_DEFAULT_HTTP_PORT, 
														OnCompleteCB, 
														OnProgressCB,
														MakeHttpDelegate(NULL),
														TRUE,
														FALSE,
														0,
														pUserData);
}

//
// Download with complete notify function 
//
DWORD NDCloudDownloadEx(tstring strUrl, tstring strHeader, tstring strMethod, tstring strPost, CHttpDelegateBase& OnCompleteCB, void* pUserData /*= NULL*/)
{
	return HttpDownloadManager::GetInstance()->AddTask(EduPlatformHost, 
														strUrl.c_str(), 
														strHeader.c_str(), 
														strMethod.c_str(), 
														strPost.c_str(), 
														80, 
														OnCompleteCB, 
														MakeHttpDelegate(NULL), 
														MakeHttpDelegate(NULL),
														TRUE,
														FALSE,
														0,
														pUserData);
}

//
// Download file
//
DWORD NDCloudDownloadFile(tstring strUrl, tstring strGuid, tstring strName, int nCloudFileType, int nThumbSize, 
						  CHttpDelegateBase& OnCompleteCB, void* pUserData, tstring strMD5 /*= _T("")*/)
{

	DWORD dwTaskId = 0;
	switch( nCloudFileType )
	{
	case CloudFileImage:
		dwTaskId = NDCloudFileManager::GetInstance()->DownloadFileImage(strUrl, strGuid, strName, nThumbSize, OnCompleteCB, MakeHttpDelegate(NULL), pUserData, strMD5);
		break;

	case CloudFileVideo:
		dwTaskId = NDCloudFileManager::GetInstance()->DownloadFileVideo(strUrl, strGuid, strName, OnCompleteCB, MakeHttpDelegate(NULL), pUserData, strMD5);
		break;

	case CloudFileVolume:
		dwTaskId = NDCloudFileManager::GetInstance()->DownloadFileVolume(strUrl, strGuid, strName, OnCompleteCB, MakeHttpDelegate(NULL), pUserData, strMD5);
		break;

	case CloudFileFlash:
		dwTaskId = NDCloudFileManager::GetInstance()->DownloadFileFlash(strUrl, strGuid, strName, OnCompleteCB, MakeHttpDelegate(NULL), pUserData, strMD5);
		break;

	case CloudFileQuestion:
		dwTaskId = NDCloudFileManager::GetInstance()->DownloadFileQuestion(strUrl, strGuid, strName, OnCompleteCB, MakeHttpDelegate(NULL), pUserData, strMD5);
		break;

	case CloudFileCoursewareObjects:
		dwTaskId = NDCloudFileManager::GetInstance()->DownloadFileQuestion(strUrl, strGuid, strName, OnCompleteCB, MakeHttpDelegate(NULL), pUserData, strMD5);
		break;

	case CloudFileNdpCourse:
		dwTaskId = NDCloudFileManager::GetInstance()->DownloadFileNdpCourse(strUrl, strGuid, strName, OnCompleteCB, MakeHttpDelegate(NULL), pUserData, strMD5);
		break;

	case CloudFile3DResource:
		dwTaskId = NDCloudFileManager::GetInstance()->DownloadFile3DResource(strUrl, strGuid, strName, OnCompleteCB, MakeHttpDelegate(NULL), pUserData, strMD5);
		break;

	case CloudFileVRResource:
		dwTaskId = NDCloudFileManager::GetInstance()->DownloadFileVRResource(strUrl, strGuid, strName, OnCompleteCB, MakeHttpDelegate(NULL), pUserData, strMD5);
		break;
	}

	return dwTaskId;
}

//
// Download file
//
DWORD NDCloudDownloadFile(tstring strUrl, tstring strGuid, tstring strName, int nCloudFileType, int nThumbSize, 
						  CHttpDelegateBase& OnCompleteCB, CHttpDelegateBase& OnProgressCB, void* pUserData, tstring strMD5 /*= _T("")*/)
{
		 
	DWORD dwTaskId = 0;
	switch( nCloudFileType )
	{
	case CloudFileImage:
		dwTaskId = NDCloudFileManager::GetInstance()->DownloadFileImage(strUrl, strGuid, strName, nThumbSize, OnCompleteCB, OnProgressCB, pUserData, strMD5);
		break;

	case CloudFileImageEx:
		dwTaskId = NDCloudFileManager::GetInstance()->DownloadFileImageEx(strUrl, strGuid, strName, nThumbSize, OnCompleteCB, OnProgressCB, pUserData, strMD5);
		break;

	case CloudFileVideo:
		dwTaskId = NDCloudFileManager::GetInstance()->DownloadFileVideo(strUrl, strGuid, strName, OnCompleteCB, OnProgressCB, pUserData, strMD5);
		break;

	case CloudFileVolume:
		dwTaskId = NDCloudFileManager::GetInstance()->DownloadFileVolume(strUrl, strGuid, strName, OnCompleteCB, OnProgressCB, pUserData, strMD5);
		break;

	case CloudFileFlash:
		dwTaskId = NDCloudFileManager::GetInstance()->DownloadFileFlash(strUrl, strGuid, strName, OnCompleteCB, OnProgressCB, pUserData, strMD5);
		break;

	case CloudFileQuestion:
		dwTaskId = NDCloudFileManager::GetInstance()->DownloadFileQuestion(strUrl, strGuid, strName, OnCompleteCB, OnProgressCB, pUserData, strMD5);
		break;

	case CloudFileCoursewareObjects:
		dwTaskId = NDCloudFileManager::GetInstance()->DownloadFileQuestion(strUrl, strGuid, strName, OnCompleteCB, OnProgressCB, pUserData, strMD5);
		break;

	case CloudFileNdpCourse:
		dwTaskId = NDCloudFileManager::GetInstance()->DownloadFileNdpCourse(strUrl, strGuid, strName, OnCompleteCB, OnProgressCB, pUserData, strMD5);
		break;

	case CloudFile3DResource:
		dwTaskId = NDCloudFileManager::GetInstance()->DownloadFile3DResource(strUrl, strGuid, strName, OnCompleteCB, OnProgressCB, pUserData, strMD5);
		break;

	case CloudFileVRResource:
		dwTaskId = NDCloudFileManager::GetInstance()->DownloadFileVRResource(strUrl, strGuid, strName, OnCompleteCB, OnProgressCB, pUserData, strMD5);
		break;
	}

	return dwTaskId;
}

//
// Download course file
//
DWORD NDCloudDownloadCourseFile(tstring strUrl, tstring strGuid, tstring strPPTName, int nCourseFileType, CHttpDelegateBase& OnCompleteCB,void* pUserData, tstring strMD5)
{
	return NDCloudFileManager::GetInstance()->DownloadFileCourse(strUrl, strGuid, strPPTName, nCourseFileType, OnCompleteCB, MakeHttpDelegate(NULL),pUserData, strMD5); 
}

//
// Download course file
//
DWORD NDCloudDownloadCourseFile(tstring strUrl, tstring strGuid, tstring strPPTName, int nCourseFileType, CHttpDelegateBase& OnCompleteCB, CHttpDelegateBase& OnProgressCB,void* pUserData, tstring strMD5)
{
	return NDCloudFileManager::GetInstance()->DownloadFileCourse(strUrl, strGuid, strPPTName, nCourseFileType, OnCompleteCB, OnProgressCB,pUserData, strMD5);

}

//
// Download priority
//
BOOL NDCloudDownloadPriority(DWORD dwTaskId, int nPriority)
{
	return HttpDownloadManager::GetInstance()->SetTaskPriority(dwTaskId, nPriority);

}

//
// Download cancel
//
BOOL NDCloudDownloadCancel(DWORD dwTaskId, CHttpDelegateBase* pOnCompleteDelegate /*= NULL*/, CHttpDelegateBase* pOnProgressDelegate /*= NULL*/)
{
	if( pOnCompleteDelegate != NULL || pOnProgressDelegate != NULL )
	{
		NDCloudFileManager::GetInstance()->CancelDownload(dwTaskId, pOnCompleteDelegate, pOnProgressDelegate);
	}
	else
	{
		HttpDownloadManager::GetInstance()->CancelTask(dwTaskId);
		NDCloudFileManager::GetInstance()->CancelDownload(dwTaskId);
	}

	return TRUE;
}

//
// Download pause
//
BOOL NDCloudDownloadPause(DWORD dwTaskId, CHttpDelegateBase* pOnPauseDelegate /*= NULL*/)
{
	NDCloudFileManager::GetInstance()->PauseDownload(dwTaskId, pOnPauseDelegate);
	return TRUE;
}

BOOL NDCloudDownloadResume(DWORD dwTaskId)
{
	NDCloudFileManager::GetInstance()->ResumeDownload(dwTaskId);
	return TRUE;
}


//----------------------------------------------------------------------------
// compose url
//
tstring NDCloudComposeUrlCategory(tstring strGradeCode /*= ""*/, tstring strCourseCode /*= ""*/)
{
	// default
	if( strGradeCode == _T("") && strCourseCode == _T("") )
		return EduPlatformUrlCategories;

	// "/v0.3/categories/relations?patternPath=K12/$ON030100/$SB01300"
	TCHAR szUrl[1024];

	tstring strFormat = _T("%s");
	if( strGradeCode != _T("") )
	{
		strFormat += _T("/%s");

		if( strCourseCode != _T("") )
			strFormat += _T("/%s");
	}
	 
	_stprintf_s(szUrl, strFormat.c_str(), EduPlatformUrlCategories, strGradeCode.c_str(), strCourseCode.c_str());
	return szUrl;
}

tstring NDCloudComposeUrlBookInfo(tstring strSectionCode, tstring strGradeCode, tstring strCourseCode, tstring strEditionCode, tstring strSubEditionCode, int nLimitStart, int nLimitEnd)
{
	if( strGradeCode == _T("") || strCourseCode == _T("") || strEditionCode == _T("") || strSubEditionCode == _T("") )
		return _T("");

	TCHAR szParam[1024];

	if( strSectionCode == strGradeCode )
		_stprintf_s(szParam, _T("/%s//%s/%s/%s"), strGradeCode.c_str(), strCourseCode.c_str(), strEditionCode.c_str(), strSubEditionCode.c_str());
	else
		_stprintf_s(szParam, _T("/%s/%s/%s/%s/%s"), strSectionCode.c_str(), strGradeCode.c_str(), strCourseCode.c_str(), strEditionCode.c_str(), strSubEditionCode.c_str());

	TCHAR szUrl[1024];
	_stprintf_s(szUrl, EduPlatformUrlBookInfo, szParam, nLimitStart, nLimitEnd);
	return szUrl;
}

tstring NDCloudComposeUrlChapterInfo(tstring strBookGUID)
{
	if( strBookGUID == _T("") )
		return _T("");
	
	TCHAR szUrl[1024];
	_stprintf_s(szUrl, EduPlatformUrlChapterInfo, strBookGUID.c_str());
	return szUrl;
}

tstring NDCloudComposeUrlCourseCount(tstring strChapterGUID /*= ""*/, tstring strSearchKeyWord /*=""*/)
{
	return NDCloudComposeUrlCourseInfo(strChapterGUID, strSearchKeyWord, 1, 1);
}

tstring NDCloudComposeUrlCourseInfo(tstring strChapterGUID /*= ""*/, tstring strSearchKeyWord /*=""*/, int nLimitStart /*=0*/, int nLimitEnd /*=0*/,DWORD dwUserId /*= 0*/)
{
	if( strSearchKeyWord != _T("") )
		strSearchKeyWord = _T("=") + UrlEncode(Str2Utf8(strSearchKeyWord));

	if( strChapterGUID != _T("") )
	{
		strChapterGUID = _T("=chapters/") + strChapterGUID;
		strChapterGUID = strChapterGUID + _T("/ASSOCIATE");
	}

	tstring strInclude = _T("TI");
	tstring strCoverage = _T("Org/nd/");
	if( dwUserId != 0 )
	{
		TCHAR szCoverage[MAX_PATH];
		_stprintf_s(szCoverage, _T("User/%d/"), dwUserId);
		strCoverage = szCoverage;
		strInclude += ",CG,LC";
	}

	TCHAR szUrl[1024];
	_stprintf_s(szUrl, EduPlatformUrlCourseInfo, strSearchKeyWord.c_str(), nLimitStart, nLimitEnd, strInclude.c_str(), strChapterGUID.c_str(), strCoverage.c_str());
	return szUrl;
}

tstring NDCloudComposeUrlPictureCount(tstring strChapterGUID, tstring strSearchKeyWord /*= ""*/)
{
	return NDCloudComposeUrlPictureInfo(strChapterGUID, strSearchKeyWord, 1, 1);
}

tstring NDCloudComposeUrlPictureInfo(tstring strChapterGUID, tstring strSearchKeyWord, int nLimitStart, int nLimitEnd, DWORD dwUserId /*= 0*/)
{
	if( strSearchKeyWord != _T("") )
		strSearchKeyWord = _T("=") + UrlEncode(Str2Utf8(strSearchKeyWord));

	if( strChapterGUID != _T("") )
	{
		strChapterGUID = _T("=chapters/") + strChapterGUID;
		strChapterGUID = strChapterGUID + _T("/ASSOCIATE");
	}

	tstring strInclude = _T("TI");
	tstring strCoverage = _T("Org/nd/");
	if( dwUserId != 0 )
	{
		TCHAR szCoverage[MAX_PATH];
		_stprintf_s(szCoverage, _T("User/%d/"), dwUserId);
		strCoverage = szCoverage;
		strInclude += ",CG,LC";
	}

	TCHAR szUrl[1024];
	_stprintf_s(szUrl, EduPlatformUrlPictureInfo, strSearchKeyWord.c_str(), nLimitStart, nLimitEnd, strInclude.c_str(), strChapterGUID.c_str(), strCoverage.c_str());
	return szUrl;
}

tstring NDCloudComposeUrlVolumeCount(tstring strChapterGUID /*= ""*/, tstring strSearchKeyWord/* = ""*/)
{
	return NDCloudComposeUrlVolumeInfo(strChapterGUID, strSearchKeyWord, 1, 1);
}

tstring NDCloudComposeUrlVolumeInfo(tstring strChapterGUID, tstring strSearchKeyWord, int nLimitStart, int nLimitEnd, DWORD dwUserId /*= 0*/)
{
	if( strSearchKeyWord != _T("") )
		strSearchKeyWord = _T("=") + UrlEncode(Str2Utf8(strSearchKeyWord));

	if( strChapterGUID != _T("") )
	{
		strChapterGUID = _T("=chapters/") + strChapterGUID;
		strChapterGUID = strChapterGUID + _T("/ASSOCIATE");
	}

	tstring strInclude = _T("TI");
	tstring strCoverage = _T("Org/nd/");
	if( dwUserId != 0 )
	{
		TCHAR szCoverage[MAX_PATH];
		_stprintf_s(szCoverage, _T("User/%d/"), dwUserId);
		strCoverage = szCoverage;
		strInclude += ",CG,LC";
	}

	TCHAR szUrl[1024];
	_stprintf_s(szUrl, EduPlatformUrlVolumeInfo, strSearchKeyWord.c_str(), nLimitStart, nLimitEnd, strInclude.c_str(), strChapterGUID.c_str(), strCoverage.c_str());
	return szUrl;
}

tstring NDCloudComposeUrlVideoCount(tstring strChapterGUID /*= ""*/, tstring strSearchKeyWord/* = ""*/)
{
	return NDCloudComposeUrlVideoInfo(strChapterGUID, strSearchKeyWord, 1, 1);
}

tstring NDCloudComposeUrlVideoInfo(tstring strChapterGUID, tstring strSearchKeyWord, int nLimitStart, int nLimitEnd, DWORD dwUserId /*= 0*/)
{
	if( strSearchKeyWord != _T("") )
		strSearchKeyWord = _T("=") + UrlEncode(Str2Utf8(strSearchKeyWord));

	if( strChapterGUID != _T("") )
	{
		strChapterGUID = _T("=chapters/") + strChapterGUID;
		strChapterGUID = strChapterGUID + _T("/ASSOCIATE");
	}

	tstring strInclude = _T("TI");
	tstring strCoverage = _T("Org/nd/");
	if( dwUserId != 0 )
	{
		TCHAR szCoverage[MAX_PATH];
		_stprintf_s(szCoverage, _T("User/%d/"), dwUserId);
		strCoverage = szCoverage;
		strInclude += ",CG,LC";
	}

	TCHAR szUrl[1024];
	_stprintf_s(szUrl, EduPlatformUrlVideoInfo, strSearchKeyWord.c_str(), nLimitStart, nLimitEnd, strInclude.c_str(), strChapterGUID.c_str(), strCoverage.c_str());
	return szUrl;
}

tstring NDCloudComposeUrlFlashCount(tstring strChapterGUID /* =  */, tstring strSearchKeyWord /* = */ )
{
	return NDCloudComposeUrlFlashInfo(strChapterGUID, strSearchKeyWord, 1, 1);
}

tstring NDCloudComposeUrlFlashInfo(tstring strChapterGUID, tstring strSearchKeyWord, int nLimitStart, int nLimitEnd, DWORD dwUserId /* = 0 */)
{
	if( strSearchKeyWord != _T("") )
		strSearchKeyWord = _T("=") + UrlEncode(Str2Utf8(strSearchKeyWord));

	if( strChapterGUID != _T("") )
	{
		strChapterGUID = _T("=chapters/") + strChapterGUID;
		strChapterGUID = strChapterGUID + _T("/ASSOCIATE");
	}

	tstring strInclude = _T("TI");
	tstring strCoverage = _T("Org/nd/");
	if( dwUserId != 0 )
	{
		TCHAR szCoverage[MAX_PATH];
		_stprintf_s(szCoverage, _T("User/%d/"), dwUserId);
		strCoverage = szCoverage;
		strInclude += ",CG,LC";
	}

	TCHAR szUrl[1024];
	_stprintf_s(szUrl, EduPlatformUrlFlashInfo, strSearchKeyWord.c_str(), nLimitStart, nLimitEnd, strInclude.c_str(), strChapterGUID.c_str(), strCoverage.c_str());
	return szUrl;
}

tstring NDCloudComposeUrlPPTTemplateCount(tstring strSectionCode /* =  */,tstring strCourseCode /* =  */, tstring strSearchKeyWord /* = */ )
{
	return NDCloudComposeUrlPPTTemplateInfo(strSectionCode, strCourseCode, strSearchKeyWord, 1, 1);
}

tstring NDCloudComposeUrlPPTTemplateInfo(tstring strSectionCode, tstring strCourseCode, tstring strSearchKeyWord, int nLimitStart, int nLimitEnd, DWORD dwUserId /* = 0 */)
{
	if( strSearchKeyWord != _T("") )
		strSearchKeyWord = _T("=") + UrlEncode(Str2Utf8(strSearchKeyWord));

	tstring strK12Code;
	if( strSectionCode != _T("") && strCourseCode != _T("") )
	{
		strK12Code = _T("=K12/") + strSectionCode;
		strK12Code += _T("//");
		strK12Code += strCourseCode;
		strK12Code += _T("//");
	}

	tstring strInclude = _T("TI");
	tstring strCoverage = _T("Org/nd/");
	if( dwUserId != 0 )
	{
		TCHAR szCoverage[MAX_PATH];
		_stprintf_s(szCoverage, _T("User/%d/"), dwUserId);
		strCoverage = szCoverage;
		strInclude += ",CG,LC";
	}

	TCHAR szUrl[1024];
	_stprintf_s(szUrl, EduPlatformUrlPPTTemplateInfo, strSearchKeyWord.c_str(), nLimitStart, nLimitEnd, strInclude.c_str(), strK12Code.c_str(), strCoverage.c_str());
	return szUrl;
}

tstring NDCloudComposeUrl3DResourceCount(tstring strChapterGUID /*= _T("")*/, tstring strSearchKeyWord /*= _T("")*/)
{
	return NDCloudComposeUrl3DResourceInfo(strChapterGUID, strSearchKeyWord, 1, 1);
}	

tstring NDCloudComposeUrl3DResourceInfo(tstring strChapterGUID, tstring strSearchKeyWord, int nLimitStart, int nLimitEnd, DWORD dwUserId /* = 0 */)
{
	if( strSearchKeyWord != _T("") )
		strSearchKeyWord = _T("=") + UrlEncode(Str2Utf8(strSearchKeyWord));

	if( strChapterGUID != _T("") )
	{
		strChapterGUID = _T("=chapters/") + strChapterGUID;
		strChapterGUID = strChapterGUID + _T("/ASSOCIATE");
	}

	tstring strInclude = _T("TI");
	tstring strCoverage = _T("Org/nd/");
	if( dwUserId != 0 )
	{
		TCHAR szCoverage[MAX_PATH];
		_stprintf_s(szCoverage, _T("User/%d/"), dwUserId);
		strCoverage = szCoverage;
		strInclude += ",CG,LC";
	}

	TCHAR szUrl[1024];
	_stprintf_s(szUrl, EduPlatformUrl3DResourceInfo, strSearchKeyWord.c_str(), nLimitStart, nLimitEnd, strInclude.c_str(), strChapterGUID.c_str(), strCoverage.c_str());
	return szUrl;
}

tstring NDCloudComposeUrlVRResourceCount(tstring strChapterGUID /*= _T("")*/, tstring strSearchKeyWord /*= _T("")*/, tstring strCategory /*= _T("")*/)
{
	return NDCloudComposeUrlVRResourceInfo(strChapterGUID, strSearchKeyWord, strCategory, 1, 1);
}	

tstring NDCloudComposeUrlVRResourceInfo(tstring strChapterGUID, tstring strSearchKeyWord, tstring strCategory, int nLimitStart, int nLimitEnd, DWORD dwUserId /* = 0 */)
{
	if( strSearchKeyWord != _T("") )
		strSearchKeyWord = _T("=") + UrlEncode(Str2Utf8(strSearchKeyWord));

	if( strChapterGUID != _T("") )
	{
		strChapterGUID = _T("=chapters/") + strChapterGUID;
		strChapterGUID = strChapterGUID + _T("/ASSOCIATE");
	}

	if( strCategory != _T(""))
	{
		strCategory = _T(" and ") + strCategory;
	}
	tstring strInclude = _T("TI");
	tstring strCoverage = _T("Org/nd/");
	if( dwUserId != 0 )
	{
		TCHAR szCoverage[MAX_PATH];
		_stprintf_s(szCoverage, _T("User/%d/"), dwUserId);
		strCoverage = szCoverage;
		strInclude += ",CG,LC";
	}

	TCHAR szUrl[1024];
	_stprintf_s(szUrl, EduPlatformUrlVRResourceInfo, strSearchKeyWord.c_str(), nLimitStart, nLimitEnd, strInclude.c_str(), strCategory.c_str(), strChapterGUID.c_str(), strCoverage.c_str());
	return szUrl;
}

tstring NDCloudComposeUrlQuestionCount(tstring strChapterGUID /* =  */, tstring strSearchKeyWord /* = */ )
{
	return NDCloudComposeUrlQuestionInfo(strChapterGUID, strSearchKeyWord, 1, 1);
}

tstring NDCloudComposeUrlCoursewareObjectsCount( tstring strChapterGUID /*= ""*/, tstring strSearchKeyWord /*= ""*/ )
{
	return NDCloudComposeUrlCoursewareObjectsInfo(strChapterGUID, strSearchKeyWord, 1, 1);
}

tstring NDCloudComposeUrlQuestionInfo(tstring strChapterGUID, tstring strSearchKeyWord, int nLimitStart, int nLimitEnd, DWORD dwUserId /*= 0*/)
{
	if( strSearchKeyWord != _T("") )
		strSearchKeyWord = _T("=") + UrlEncode(Str2Utf8(strSearchKeyWord));

	if( strChapterGUID != _T("") )
	{
		strChapterGUID = _T("=chapters/") + strChapterGUID;
		strChapterGUID = strChapterGUID + _T("/ASSOCIATE");
	}

	tstring strCoverage = _T("Org/nd/");
	if( dwUserId != 0 )
	{
		TCHAR szCoverage[MAX_PATH];
		_stprintf_s(szCoverage, _T("User/%d/"), dwUserId);
		strCoverage = szCoverage;
	}

	TCHAR szUrl[1024];
	_stprintf_s(szUrl, EduPlatformUrlQuestionInfo, strSearchKeyWord.c_str(), nLimitStart, nLimitEnd, strChapterGUID.c_str(), strCoverage.c_str());
	return szUrl;

}

tstring NDCloudComposeUrlCoursewareObjectsInfo( tstring strChapterGUID, tstring strSearchKeyWord, int nLimitStart, int nLimitEnd, DWORD dwUserId /*= 0*/ )
{
	if( strSearchKeyWord != _T("") )
		strSearchKeyWord = _T("=") + UrlEncode(AnsiToUtf8(strSearchKeyWord));

	if( strChapterGUID != _T("") )
	{
		strChapterGUID = _T("=chapters/") + strChapterGUID;
		strChapterGUID = strChapterGUID + _T("/ASSOCIATE");
	}

	tstring strCoverage = _T("Org/nd/");
	if( dwUserId != 0 )
	{
		TCHAR szCoverage[MAX_PATH];
		_stprintf_s(szCoverage, _T("User/%d/"), dwUserId);
		strCoverage = szCoverage;
	}

	TCHAR szUrl[1024];
	_stprintf_s(szUrl, EduPlatformUrlCoursewareObjectsInfo, strSearchKeyWord.c_str(), nLimitStart, nLimitEnd, strChapterGUID.c_str(), strCoverage.c_str());
	return szUrl;
}

tstring NDCloudComposeUrlPackQuestion(tstring strQuestionGUID, DWORD dwUserId)
{
	TCHAR szUrl[1024];
	_stprintf_s(szUrl, EduPlatformUrlPackQuestion, strQuestionGUID.c_str(), dwUserId);
	return szUrl;
}

tstring NDCloudComposeUrlPackCoursewareObjects( tstring strQuestionGUID, DWORD dwUserId )
{
	TCHAR szUrl[1024];
	_stprintf_s(szUrl, EduPlatformUrlPackCoursewareObjects, strQuestionGUID.c_str(), dwUserId);
	return szUrl;
}

//
// Query pack state
//
tstring NDCloudComposeUrlPackQuestionState(tstring strQuestionGUID, DWORD dwUserId)
{
	TCHAR szUrl[1024];
	_stprintf_s(szUrl, EduPlatformUrlPackQuestionState, strQuestionGUID.c_str(), dwUserId);
	return szUrl;
}

tstring NDCloudComposeUrlPackCoursewareObjectsState( tstring strGUID, DWORD dwUserId )
{
	TCHAR szUrl[1024];
	_stprintf_s(szUrl, EduPlatformUrlPackCoursewareObjectsState, strGUID.c_str(), dwUserId);
	return szUrl;
}


//
// tools
//
tstring NDCloudComposeUrlToolsCount(tstring strChapterGUID /* =  */, tstring strSearchKeyWord /* = */ )
{
	return NDCloudComposeUrlQuestionInfo(strChapterGUID, strSearchKeyWord, 1, 1);
}

tstring NDCloudComposeUrlToolsInfo(tstring strChapterGUID, tstring strSearchKeyWord, int nLimitStart, int nLimitEnd)
{
	if( strSearchKeyWord != _T("") )
		strSearchKeyWord = _T("=") + UrlEncode(AnsiToUtf8(strSearchKeyWord));

	if( strChapterGUID != _T("") )
	{
		strChapterGUID = _T("=chapters/") + strChapterGUID;
		strChapterGUID = strChapterGUID + _T("/ASSOCIATE");
	}

	tstring strCoverage = _T("Org/nd/");

	TCHAR szUrl[1024];
	_stprintf_s(szUrl, EduPlatformUrlToolsInfo, strSearchKeyWord.c_str(), nLimitStart, nLimitEnd, strChapterGUID.c_str(), strCoverage.c_str());
	return szUrl;

}

tstring NDCloudComposeUrlCagegoryInfo(tstring strSearchKeyWord, int nLimitStart, int nLimitEnd)
{
	TCHAR szUrl[1024];
	_stprintf_s(szUrl, EduPlatformUrlCategoryInfo, strSearchKeyWord.c_str(), nLimitStart, nLimitEnd);
	return szUrl;
}

//----------------------------------------------------------------------------
// json decode
//
int NDCloudDecodeCount(char* pData, int nDataSize)
{
	// utf8 to ansi
	pData[nDataSize] = '\0';
	string str = Utf8ToAnsi(pData);

	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(str, root);
	if( !res )
		return 0;

	if( root["total"].isNull() )
		return 0;

	int nCount = root["total"].asInt();

	return nCount;
}


int NDCloudDecodeExercisesCount(char* pData, int nDataSize)
{
	// utf8 to ansi
	pData[nDataSize] = '\0';
	string str = Utf8ToAnsi(pData);

	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(str, root);
	if( !res )
		return 0;

	if( root["total"].isNull() )
		return 0;

	int nCount = root["total"].asInt();

	Json::Value& items = root["items"];

	for(int i = 0; i < (int)items.size(); i++)
	{
		Json::Value& item = items[i];

		tstring strTitle;
		tstring strGuid;
		tstring strDesc;
		tstring strQuestionType;
		tstring strPreviewUrl;
		tstring strXmlUrl;
		tstring strStatus;


		if( !item["title"].isNull() )
			strTitle = Ansi2Str(item["title"].asCString());

		if( !item["identifier"].isNull() )
			strGuid = Ansi2Str(item["identifier"].asCString());

		if( !item["description"].isNull() )
			strDesc = Ansi2Str(item["description"].asCString());

		// question type
		if( !item["categories"].isNull() )
		{
			if(!item["categories"]["res_type"].isNull())
			{
				Json::Value& itemResType = item["categories"]["res_type"];
				for(int i = 0; i < (int)itemResType.size(); i++)
				{
					tstring strTaxoncode = Ansi2Str(item["categories"]["res_type"][i]["taxoncode"].asCString());
					if(strTaxoncode != _T("$RE0200"))
					{
						strQuestionType = Ansi2Str(item["categories"]["res_type"][1]["taxonname"].asCString());
						break;
					}
				}
			}
		}

		// preview
		if( !item["preview"].isNull() && !item["preview"]["question_small"].isNull() )
			strPreviewUrl = item["preview"]["question_small"].asCString();

		// item.xml href 
		if( !item["tech_info"].isNull() && !item["tech_info"]["href"].isNull() && !item["tech_info"]["href"]["location"].isNull() )
			strXmlUrl = item["tech_info"]["href"]["location"].asCString();

		if( !item["life_cycle"].isNull() && !item["life_cycle"]["status"].isNull() )
		{
			strStatus = item["life_cycle"]["status"].asCString();

			_tcslwr((char*)strStatus.c_str());

			if( strStatus == _T("creating") )
			{
				nCount --;
				continue;
			}
		}


		// item json string
		//Json::FastWriter writter;
		//string strItemJson = writter.write(item);
	}


	return nCount;
}

BOOL NDCloudDecodeCategory(IN char* pData, IN int nDataSize, OUT CCategoryTree*& pCourseTree)
{
	// utf8 to ansi
	pData[nDataSize] = '\0';
	string str = Utf8ToAnsi(pData);

	if( g_pCategoryTree == NULL )
		g_pCategoryTree = new CCategoryTree;

	BOOL bRet = g_pCategoryTree->CreateTree(str);
	pCourseTree = g_pCategoryTree;

	return bRet;

}

BOOL NDCloudDecodeCategory(IN char* pData, IN int nDataSize,IN tstring strSectionCode, IN tstring strGradeCode, IN tstring strCourseCode, IN tstring strEditionCode)
{
	// utf8 to ansi
	pData[nDataSize] = '\0';
	string str = Utf8ToAnsi(pData);

	if( g_pCategoryTree == NULL )
		g_pCategoryTree = new CCategoryTree;

	BOOL bRet = g_pCategoryTree->AddTreeNode(str, strSectionCode, strGradeCode, strCourseCode, strEditionCode);
	return bRet;
}

BOOL NDCloudDecodeBookGUID(IN char* pData, IN int nDataSize,tstring& strBookGUID)
{
	pData[nDataSize] = '\0';
	string str = Utf8ToAnsi(pData);

	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(str, root);
	if( !res )
		return FALSE;

	if( root["total"].isNull() )
		return FALSE;

	// 
	int count = root["total"].asInt();
	Json::Value& items = root["items"];

	for(int i = 0; i < count; i++)
	{	
		strBookGUID = Ansi2Str(items[i]["identifier"].asCString());
		g_strBookGUID = strBookGUID;
	}

	// save in config.ini
	tstring strPath = GetLocalPath();
	strPath += _T("\\Setting\\Config.ini");

	WritePrivateProfileString(_T("Config"),_T("BookGuid"),strBookGUID.c_str(), strPath.c_str());

	return TRUE;
}

BOOL NDCloudDecodeChapterInfo(IN char* pData, IN int nDataSize, IN CChapterTree* pChapterTree)
{
	// utf8 to ansi
	pData[nDataSize] = '\0';
	string str = Utf8ToAnsi(pData);

// 	if( g_pChapterTree == NULL )
// 		g_pChapterTree = new CChapterTree;
// 	else
// 		g_pChapterTree->DestroyTree();
// 
 //	pChapterTree = g_pChapterTree;
	return pChapterTree->CreateTree(str);

}

BOOL NDCloudDecodeImageList(IN char* pData, IN int nDataSize, OUT CStream* pImageStream)
{
	// utf8 to ansi
	pData[nDataSize] = '\0';
	string str = Utf8ToAnsi(pData);


	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(str, root);
	if( !res )
		return FALSE;

	if( root["total"].isNull() )
		return FALSE;

	// 
	Json::Value& items = root["items"];
	int count = items.size();
	
	// count
	pImageStream->WriteDWORD(count);

	for(int i = 0; i < (int)items.size(); i++)
	{
		Json::Value& item = items[i];

		tstring strGuid;
		tstring strTitle = Ansi2Str(item["title"].asCString());
		tstring strUrl;
		tstring strMD5;

		if( !item["identifier"].isNull() )
			strGuid = Ansi2Str(item["identifier"].asCString());

		if( !item["tech_info"].isNull() && !item["tech_info"]["href"].isNull() && !item["tech_info"]["href"]["location"].isNull() )
			strUrl = Ansi2Str(item["tech_info"]["href"]["location"].asCString());	 

		if( strUrl == _T("") )
		{
			if( !item["tech_info"].isNull() && !item["tech_info"]["source"].isNull() && !item["tech_info"]["source"]["location"].isNull() )
				strUrl = Ansi2Str(item["tech_info"]["source"]["location"].asCString());	
		}

		// source md5
		if( !item["tech_info"].isNull() && !item["tech_info"]["source"].isNull() && !item["tech_info"]["source"]["md5"].isNull() )
			strMD5 = Ansi2Str(item["tech_info"]["source"]["md5"].asCString());	

		// item json string
		Json::FastWriter writter;
		string strItemJson = writter.write(item);

		
		pImageStream->WriteString(strGuid);
		pImageStream->WriteString(strTitle);
		pImageStream->WriteString(strUrl);
		pImageStream->WriteString(strMD5);
		pImageStream->WriteString(strItemJson);

	}

	// total
	int total = root["total"].asInt();

	pImageStream->WriteDWORD(total);

	tstring strLimit = Ansi2Str(root["limit"].asCString());
	int nStart, nEnd;
	if(sscanf_s(strLimit.c_str(), _T("(%d,%d)"), &nStart, &nEnd ) == 2)
	{
		pImageStream->WriteDWORD(nStart);
		pImageStream->WriteDWORD(nEnd);
	}

	pImageStream->ResetCursor();

	return TRUE;
}


BOOL NDCloudDecodeVolumeList(IN char* pData, IN int nDataSize, OUT CStream* pVolumeStream)
{
	// utf8 to ansi
	pData[nDataSize] = '\0';
	string str = Utf8ToAnsi(pData);


	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(str, root);
	if( !res )
		return FALSE;

	if( root["total"].isNull() )
		return FALSE;

	// 
	
	Json::Value& items = root["items"];
	int count = items.size();

	// count
	pVolumeStream->WriteDWORD(count);

	for(int i = 0; i < (int)items.size(); i++)
	{
		Json::Value& item = items[i];

		tstring strGuid;
		tstring strTitle = Ansi2Str(item["title"].asCString());
		tstring strUrl;
		tstring strMD5;

		if( !item["identifier"].isNull() )
			strGuid = Ansi2Str(item["identifier"].asCString());


		if( !item["tech_info"].isNull() && !item["tech_info"]["href"].isNull() && !item["tech_info"]["href"]["location"].isNull() )
			strUrl = Ansi2Str(item["tech_info"]["href"]["location"].asCString());

		if( strUrl == _T("") )
		{
			if( !item["tech_info"].isNull() && !item["tech_info"]["source"].isNull() && !item["tech_info"]["source"]["location"].isNull() )
				strUrl = Ansi2Str(item["tech_info"]["source"]["location"].asCString());	
		}

		// source md5
		if( !item["tech_info"].isNull() && !item["tech_info"]["source"].isNull() && !item["tech_info"]["source"]["md5"].isNull() )
			strMD5 = Ansi2Str(item["tech_info"]["source"]["md5"].asCString());	


		// item json string
		Json::FastWriter writter;
		string strItemJson = writter.write(item);

		pVolumeStream->WriteString(strGuid);
		pVolumeStream->WriteString(strTitle);
		pVolumeStream->WriteString(strUrl);
		pVolumeStream->WriteString(strMD5);
		pVolumeStream->WriteString(strItemJson);
	}

	// total
	int total = root["total"].asInt();
	pVolumeStream->WriteDWORD(total);

	tstring strLimit = Ansi2Str(root["limit"].asCString());
	int nStart, nEnd;
	if(sscanf_s(strLimit.c_str(), _T("(%d,%d)"), &nStart, &nEnd ) == 2)
	{
		pVolumeStream->WriteDWORD(nStart);
		pVolumeStream->WriteDWORD(nEnd);
	}

	pVolumeStream->ResetCursor();

	return TRUE;
}


BOOL NDCloudDecodeVideoList(IN char* pData, IN int nDataSize, OUT CStream* pVideoStream)
{
	// utf8 to ansi
	pData[nDataSize] = '\0';
	string str = Utf8ToAnsi(pData);


	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(str, root);
	if( !res )
		return FALSE;

	if( root["total"].isNull() )
		return FALSE;

	// 
	Json::Value& items = root["items"];
	int count = items.size();

	// count
	pVideoStream->WriteDWORD(count);

	for(int i = 0; i < (int)items.size(); i++)
	{
		Json::Value& item = items[i];

		tstring strGuid;
		tstring strTitle = Ansi2Str(item["title"].asCString());
		tstring strUrl;
		tstring strPreviewUrl;
		tstring strMD5;
		tstring strFormat;
		
		if( !item["identifier"].isNull() )
			strGuid = Ansi2Str(item["identifier"].asCString());

		if( !item["tech_info"].isNull() && !item["tech_info"]["source"].isNull() && !item["tech_info"]["source"]["location"].isNull() )
			strUrl = Ansi2Str(item["tech_info"]["source"]["location"].asCString());	

		// video format
		if( !item["tech_info"].isNull() && !item["tech_info"]["source"].isNull() && !item["tech_info"]["source"]["format"].isNull() )
			strFormat = Ansi2Str(item["tech_info"]["source"]["format"].asCString());	

		// support mp4
		if( strUrl == _T("") || strFormat.find(_T("mp4")) == -1 )
		{
			if( !item["tech_info"].isNull() && !item["tech_info"]["href"].isNull() && !item["tech_info"]["href"]["location"].isNull() )
				strUrl = Ansi2Str(item["tech_info"]["href"]["location"].asCString());
		}

		// preview
		if( !item["preview"].isNull() && !item["preview"]["cover"].isNull() )
			strPreviewUrl = item["preview"]["cover"].asCString();

		// source md5
		if( !item["tech_info"].isNull() && !item["tech_info"]["source"].isNull() && !item["tech_info"]["source"]["md5"].isNull() )
			strMD5 = Ansi2Str(item["tech_info"]["source"]["md5"].asCString());	


		// item json string
		Json::FastWriter writter;
		string strItemJson = writter.write(item);

		pVideoStream->WriteString(strGuid);
		pVideoStream->WriteString(strTitle);
		pVideoStream->WriteString(strUrl);
		pVideoStream->WriteString(strPreviewUrl);
		pVideoStream->WriteString(strMD5);
		pVideoStream->WriteString(strItemJson);
	}

	// total
	int total = root["total"].asInt();
	pVideoStream->WriteDWORD(total);

	tstring strLimit = Ansi2Str(root["limit"].asCString());
	int nStart, nEnd;
	if(sscanf_s(strLimit.c_str(), _T("(%d,%d)"), &nStart, &nEnd ) == 2)
	{
		pVideoStream->WriteDWORD(nStart);
		pVideoStream->WriteDWORD(nEnd);
	}

	pVideoStream->ResetCursor();

	return TRUE;
}

BOOL NDCloudDecodeFlashList(IN char* pData, IN int nDataSize, OUT CStream* pFlashStream)
{
	// utf8 to ansi
	pData[nDataSize] = '\0';
	string str = Utf8ToAnsi(pData);


	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(str, root);
	if( !res )
		return FALSE;

	if( root["total"].isNull() )
		return FALSE;

	// 
	Json::Value& items = root["items"];
	int count = items.size();

	// count
	pFlashStream->WriteDWORD(count);

	for(int i = 0; i < (int)items.size(); i++)
	{
		Json::Value& item = items[i];

		tstring strGuid;
		tstring strTitle = Ansi2Str(item["title"].asCString());
		tstring strUrl;
		tstring strMD5;

		if( !item["identifier"].isNull() )
			strGuid = Ansi2Str(item["identifier"].asCString());


		if( !item["tech_info"].isNull() && !item["tech_info"]["href"].isNull() && !item["tech_info"]["href"]["location"].isNull() )
			strUrl = Ansi2Str(item["tech_info"]["href"]["location"].asCString());

		if( strUrl == _T("") )
		{
			if( !item["tech_info"].isNull() && !item["tech_info"]["source"].isNull() && !item["tech_info"]["source"]["location"].isNull() )
				strUrl = Ansi2Str(item["tech_info"]["source"]["location"].asCString());	
		}

		// source md5
		if( !item["tech_info"].isNull() && !item["tech_info"]["source"].isNull() && !item["tech_info"]["source"]["md5"].isNull() )
			strMD5 = Ansi2Str(item["tech_info"]["source"]["md5"].asCString());	


		// item json string
		Json::FastWriter writter;
		string strItemJson = writter.write(item);

		pFlashStream->WriteString(strGuid);
		pFlashStream->WriteString(strTitle);
		pFlashStream->WriteString(strUrl);
		pFlashStream->WriteString(strMD5);
		pFlashStream->WriteString(strItemJson);
	}

	// total
	int total = root["total"].asInt();
	pFlashStream->WriteDWORD(total);

	tstring strLimit = Ansi2Str(root["limit"].asCString());
	int nStart, nEnd;
	if(sscanf_s(strLimit.c_str(), _T("(%d,%d)"), &nStart, &nEnd ) == 2)
	{
		pFlashStream->WriteDWORD(nStart);
		pFlashStream->WriteDWORD(nEnd);
	}

	pFlashStream->ResetCursor();

	return TRUE;
}


BOOL NDCloudDecodePPTTemplateList(IN char* pData, IN int nDataSize, OUT CStream* pPPTTemplateStream)
{
	// utf8 to ansi
	pData[nDataSize] = '\0';
	string str = Utf8ToAnsi(pData);


	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(str, root);
	if( !res )
		return FALSE;

	if( root["total"].isNull() )
		return FALSE;

	// 
	Json::Value& items = root["items"];
	int count = items.size();

	// count
	pPPTTemplateStream->WriteDWORD(count);

	for(int i = 0; i < (int)items.size(); i++)
	{
		Json::Value& item = items[i];

		tstring strTitle = Ansi2Str(item["title"].asCString());
		tstring strGuid = Ansi2Str(item["identifier"].asCString());
		tstring strPPTUrl;
		tstring strMD5;

		bool bPPTUrlMissing = false;

		if( !item["tech_info"].isNull() )
		{
			// ppt file
			if( !item["tech_info"]["source"].isNull() )
			{
				if( !item["tech_info"]["source"]["location"].isNull() )
					strPPTUrl = Ansi2Str(item["tech_info"]["source"]["location"].asCString());
				else
					bPPTUrlMissing = true;

				if( !item["tech_info"]["source"]["md5"].isNull() )
					strMD5 = Ansi2Str(item["tech_info"]["source"]["md5"].asCString());
			}
			else
				bPPTUrlMissing = true;
		}
		else
			bPPTUrlMissing = true;


		if( bPPTUrlMissing )
		{
			tstring strPost;

			if( bPPTUrlMissing )
				strPost = _T("missing_resource=[PPT File Url Missing]: ");


			strPost += strTitle;
			strPost += _T(" | ");
			strPost += strGuid;

			CHttpDownloadManager* pHttpManager = HttpDownloadManager::GetInstance();
			if( pHttpManager )
			{
				pHttpManager->AddTask(_T("p.101.com"), _T("/101ppt/resourceMissing.php"), _T(""), _T("POST"), strPost.c_str(), 80, 
					MakeHttpDelegate(NULL), 
					MakeHttpDelegate(NULL), 
					MakeHttpDelegate(NULL), 
					FALSE);
			}

		}

		// NdpFile | title | guid | pptUrl
		pPPTTemplateStream->WriteDWORD(false);
		pPPTTemplateStream->WriteString(strTitle);
		pPPTTemplateStream->WriteString(strGuid);
		pPPTTemplateStream->WriteString(strPPTUrl);
		pPPTTemplateStream->WriteString(strMD5);

		// thumbnail list 
		Json::Value& previewItems = item["preview"];
		if( previewItems.isNull() )
		{
			pPPTTemplateStream->WriteDWORD(0);
			continue;
		}

		// calculate count of slide thumbnail
		int nThumbCount = previewItems.size();
		int nRealThumbCount = 0;

		for(int j = 0; j < nThumbCount; j++)
		{
			TCHAR szKey[MAX_PATH];
			_stprintf_s(szKey, _T("Slide%d"), j+1);

			Json::Value& previewItem = previewItems[szKey];
			if( !previewItem.isNull() )
				nRealThumbCount ++;	
		}

		// write 
		pPPTTemplateStream->WriteDWORD(nRealThumbCount);

		for(int j = 0; j < nThumbCount; j++)
		{
			TCHAR szKey[MAX_PATH];
			_stprintf_s(szKey, _T("Slide%d"), j+1);

			Json::Value& previewItem = previewItems[szKey];
			if( !previewItem.isNull() )
			{
				tstring strThumbUrl = previewItem.asCString();
				pPPTTemplateStream->WriteString(strThumbUrl);
			}		
		}

		// item json string
		Json::FastWriter writter;
		string strItemJson = writter.write(item);

		pPPTTemplateStream->WriteString(strItemJson);
	}

	// total
	int total = root["total"].asInt();

	pPPTTemplateStream->WriteDWORD(total);

	tstring strLimit = Ansi2Str(root["limit"].asCString());
	int nStart, nEnd;
	if(sscanf_s(strLimit.c_str(), _T("(%d,%d)"), &nStart, &nEnd ) == 2)
	{
		pPPTTemplateStream->WriteDWORD(nStart);
		pPPTTemplateStream->WriteDWORD(nEnd);
	}

	pPPTTemplateStream->ResetCursor();

	return TRUE;
}


BOOL NDCloudDecodeCourseList(IN char* pData, IN int nDataSize, OUT CStream* pCourseStream)
{
	// utf8 to ansi
	pData[nDataSize] = '\0';
	string str = Utf8ToAnsi(pData);


	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(str, root);
	if( !res )
		return FALSE;

	if( root["total"].isNull() ) 
		return FALSE;

	// 
	Json::Value& items = root["items"];
	int count = items.size();

	// count
	pCourseStream->WriteDWORD(count);

	for(int i = 0; i < (int)items.size(); i++)
	{
		Json::Value& item = items[i];

		tstring strTitle = Ansi2Str(item["title"].asCString());
		tstring strGuid = Ansi2Str(item["identifier"].asCString());
		tstring strPPTUrl;
		tstring strMD5;

		bool bPPTUrlMissing = false;
		int  nNdpType		= 0;
	
		Json::Value jTechInfo = item.get("tech_info",Json::ValueType::nullValue);
		if( !jTechInfo.isNull() )
		{	
			Json::Value jSource = jTechInfo.get("source",Json::ValueType::nullValue);
			Json::Value jHref = jTechInfo.get("href",Json::ValueType::nullValue);
			if( !jHref.isNull() || !jSource.isNull())
			{
				tstring strFormat;
				if( !jHref["format"].isNull() )
				{
					strFormat= Ansi2Str(jHref["format"].asCString());
				}
				else if( !jSource.isNull()&&!jSource.get("format",Json::ValueType::nullValue).isNull() )
				{
					strFormat= Ansi2Str(jTechInfo["source"]["format"].asCString());
				}

				// source md5
				if( !jSource.isNull() && !jSource["md5"].isNull() )
				{
					strMD5 = Ansi2Str(jSource["md5"].asCString());
				}
				
				
				if(strFormat == _T("ndp")) // ndp file
				{
					nNdpType = 1;
					if( !jTechInfo["offline_webp"].isNull() )
					{
						if( !jTechInfo["offline_webp"]["location"].isNull() )
						{
							strPPTUrl = Ansi2Str(jTechInfo["offline_webp"]["location"].asCString());
						}
						else
							bPPTUrlMissing = true;
					}
					else if( !jTechInfo["offline"].isNull() )
					{
						if( !jTechInfo["offline"]["location"].isNull() )
						{
							strPPTUrl = Ansi2Str(jTechInfo["offline"]["location"].asCString());
						}
						else
							bPPTUrlMissing = true;

					}	
				}
				else if( strFormat == _T("ndf/cw-x") ) // ndpx file
				{
					nNdpType = 2;

					if( !jTechInfo["source"].isNull() )
					{
						if( !jTechInfo["source"]["location"].isNull() )
							strPPTUrl = Ansi2Str(jTechInfo["source"]["location"].asCString());
						else
							bPPTUrlMissing = true;
					}
					else if( !jHref.isNull() )
					{
						if( !jHref["location"].isNull() )
							strPPTUrl = Ansi2Str(jHref["location"].asCString());
						else
							bPPTUrlMissing = true;
					}

				}
				else // ppt file
				{		
					nNdpType = 0;

					if( !jTechInfo["source"].isNull() )
					{
						if( !jTechInfo["source"]["location"].isNull() )
							strPPTUrl = Ansi2Str(jTechInfo["source"]["location"].asCString());
						else
							bPPTUrlMissing = true;
					}
					else if( !jHref.isNull() )
					{
						if( !jHref["location"].isNull() )
							strPPTUrl = Ansi2Str(jHref["location"].asCString());
						else
							bPPTUrlMissing = true;
					}
				}
				
			}
			else
				bPPTUrlMissing = true;
		}
		else
			bPPTUrlMissing = true;


		if( bPPTUrlMissing )
		{
			tstring strPost;
			
			if( bPPTUrlMissing )
				strPost = _T("missing_resource=[PPT File Url Missing]: ");


			strPost += strTitle;
			strPost += _T(" | ");
			strPost += strGuid;
			
			CHttpDownloadManager* pHttpManager = HttpDownloadManager::GetInstance();
			if( pHttpManager )
			{
				pHttpManager->AddTask(_T("p.101.com"), _T("/101ppt/resourceMissing.php"), _T(""), _T("POST"), strPost.c_str(), 80, 
					MakeHttpDelegate(NULL), 
					MakeHttpDelegate(NULL), 
					MakeHttpDelegate(NULL), 
					FALSE);
			}

		}

		// title | guid | pptUrl
		pCourseStream->WriteInt(nNdpType);
		pCourseStream->WriteString(strTitle);
		pCourseStream->WriteString(strGuid);
		pCourseStream->WriteString(strPPTUrl);
		pCourseStream->WriteString(strMD5);

		// thumbnail list 
		Json::Value& previewItems = item["preview"];
		if( previewItems.isNull() )
		{
			pCourseStream->WriteDWORD(0);
			continue;
		}

		// calculate count of slide thumbnail
		int nThumbCount = previewItems.size();
		int nRealThumbCount = 0;

		for(int j = 0; j < nThumbCount; j++)
		{
			TCHAR szKey[MAX_PATH];
			_stprintf_s(szKey, _T("Slide%d"), j+1);

			Json::Value& previewItem = previewItems[szKey];
			if( !previewItem.isNull() )
				nRealThumbCount ++;	
		}
	
		// write 
		pCourseStream->WriteDWORD(nRealThumbCount);

		for(int j = 0; j < nThumbCount; j++)
		{
			TCHAR szKey[MAX_PATH];
			_stprintf_s(szKey, _T("Slide%d"), j+1);

			Json::Value& previewItem = previewItems[szKey];
			if( !previewItem.isNull() )
			{
				tstring strThumbUrl = previewItem.asCString();
				pCourseStream->WriteString(strThumbUrl);
			}		
		}

		// item json string
		Json::FastWriter writter;
		string strItemJson = writter.write(item);
		pCourseStream->WriteString(strItemJson);
	}
	
	// total
	int total = root["total"].asInt();

	pCourseStream->WriteDWORD(total);

	tstring strLimit = Ansi2Str(root["limit"].asCString());
	int nStart, nEnd;
	if(sscanf_s(strLimit.c_str(), _T("(%d,%d)"), &nStart, &nEnd ) == 2)
	{
		pCourseStream->WriteDWORD(nStart);
		pCourseStream->WriteDWORD(nEnd);
	}

	pCourseStream->ResetCursor();

	return TRUE;
}


BOOL NDCloudDecodeQuestionList(IN char* pData, IN int nDataSize, OUT CStream* pQuestionStream)
{
	// utf8 to ansi
	pData[nDataSize] = '\0';
	string str = Utf8ToAnsi(pData);


	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(str, root);
	if( !res )
		return FALSE;

	if( root["total"].isNull() )
		return FALSE;

	// 
	Json::Value& items = root["items"];
	int count = items.size();

	// count
	pQuestionStream->WriteDWORD(count);

	for(int i = 0; i < (int)items.size(); i++)
	{
		Json::Value& item = items[i];

		tstring strTitle;
		tstring strGuid;
		tstring strDesc;
		tstring strQuestionType;
		tstring strPreviewUrl;
		tstring strXmlUrl;
		tstring strStatus;
		

		if( !item["title"].isNull() )
			strTitle = Ansi2Str(item["title"].asCString());

		if( !item["identifier"].isNull() )
			strGuid = Ansi2Str(item["identifier"].asCString());

		if( !item["description"].isNull() )
			strDesc = Ansi2Str(item["description"].asCString());

		// question type
		if( !item["categories"].isNull() )
		{
			if(!item["categories"]["res_type"].isNull())
			{
				Json::Value& itemResType = item["categories"]["res_type"];
				for(int i = 0; i < (int)itemResType.size(); i++)
				{
					tstring strTaxoncode = Ansi2Str(item["categories"]["res_type"][i]["taxoncode"].asCString());
					if(strTaxoncode != _T("$RE0200"))
					{
						strQuestionType = Ansi2Str(item["categories"]["res_type"][1]["taxonname"].asCString());
						break;
					}
				}
			}
		}

		// preview
		if( !item["preview"].isNull() && !item["preview"]["question_small"].isNull() )
			strPreviewUrl = item["preview"]["question_small"].asCString();

		// item.xml href 
		if( !item["tech_info"].isNull() && !item["tech_info"]["href"].isNull() && !item["tech_info"]["href"]["location"].isNull() )
			strXmlUrl = item["tech_info"]["href"]["location"].asCString();

		if( !item["life_cycle"].isNull() && !item["life_cycle"]["status"].isNull() )
		{
			strStatus = item["life_cycle"]["status"].asCString();

			_tcslwr((char*)strStatus.c_str());

			if( strStatus == _T("creating") )
			{
				count --;
				continue;
			}
		}


		// item json string
		Json::FastWriter writter;
		string strItemJson = writter.write(item);
		

		// title | guid | description | xmlUrl 
		pQuestionStream->WriteString(strTitle);
		pQuestionStream->WriteString(strGuid);
		pQuestionStream->WriteString(strDesc);
		pQuestionStream->WriteString(strQuestionType);
		pQuestionStream->WriteString(strPreviewUrl);
		pQuestionStream->WriteString(strXmlUrl);
		pQuestionStream->WriteString(strItemJson);
	}

	pQuestionStream->Write(0, (char*)&count, 4);

	pQuestionStream->ResetCursor();
	return TRUE;
}

BOOL NDCloudDecodeCoursewareObjectsList( IN char* pData, IN int nDataSize, OUT CStream* pCoursewareObjectsStream )
{
	// utf8 to ansi
	pData[nDataSize] = '\0';
	string str = Utf8ToAnsi(pData);


	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(str, root);
	if( !res )
		return FALSE;

	if( root["total"].isNull() )
		return FALSE;

	// 
	Json::Value& items = root["items"];
	int count = items.size();

	// count
	pCoursewareObjectsStream->WriteDWORD(count);

	for(int i = 0; i < (int)items.size(); i++)
	{
		Json::Value& item = items[i];

		tstring strTitle;
		tstring strGuid;
		tstring strDesc;
		tstring strCoursewareObjectsName;
		tstring strStatus;
		tstring strPreviewUrl;
		tstring strXmlUrl;

		if( !item["title"].isNull() )
			strTitle = item["title"].asCString();

		if( !item["identifier"].isNull() )
			strGuid = item["identifier"].asCString();

		if( !item["description"].isNull() )
			strDesc = item["description"].asCString();

		if( !item["categories"].isNull() )
		{
			if(!item["categories"]["res_type"].isNull())
			{
				Json::Value& itemResType = item["categories"]["res_type"];
				for(int i = 0; i < (int)itemResType.size(); i++)
				{
					tstring strTaxoncode = item["categories"]["res_type"][i]["taxoncode"].asCString();
					if(strTaxoncode != _T("$RE0400"))
					{
						strCoursewareObjectsName = item["categories"]["res_type"][1]["taxonname"].asCString();
						break;
					}
				}
			}
		}

		// preview
		if( !item["preview"].isNull() && !item["preview"]["question_small"].isNull() )
			strPreviewUrl = item["preview"]["question_small"].asCString();

		// main.xml url
		if( !item["tech_info"].isNull() && !item["tech_info"]["href"].isNull() && !item["tech_info"]["href"]["location"].isNull() )
			strXmlUrl = item["tech_info"]["href"]["location"].asCString();


		if( !item["life_cycle"].isNull() && !item["life_cycle"]["status"].isNull() )
		{
			strStatus = item["life_cycle"]["status"].asCString();

			_tcslwr((char*)strStatus.c_str());

			if( strStatus == _T("creating") )
			{
				count --;
				continue;
			}
		}

		// item json string
		Json::FastWriter writter;
		string strItemJson = writter.write(item);


		// title | guid | description | xmlUrl | PreviewUrl
		pCoursewareObjectsStream->WriteString(strTitle);
		pCoursewareObjectsStream->WriteString(strGuid);
		pCoursewareObjectsStream->WriteString(strDesc);
		pCoursewareObjectsStream->WriteString(strCoursewareObjectsName);
		pCoursewareObjectsStream->WriteString(strPreviewUrl);
		pCoursewareObjectsStream->WriteString(strXmlUrl);
		pCoursewareObjectsStream->WriteString(strItemJson);
	}

	pCoursewareObjectsStream->Write(0, (char*)&count, 4);

	pCoursewareObjectsStream->ResetCursor();
	return TRUE;
}

BOOL NDCloudDecodePPTThumbnailList(IN char* pData, IN int nDataSize, OUT CStream* pThumbStream)
{
	// utf8 to ansi
	pData[nDataSize] = '\0';
	string str = Utf8ToAnsi(pData);

	pThumbStream->ResetCursor();

	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(str, root);
	if( !res )
		return FALSE;


	if( root["previewUrls"].isNull() )
		return FALSE;

	Json::Value& previewUrls = root["previewUrls"];
	pThumbStream->WriteDWORD(previewUrls.size());

	int count = 0;
	for(int i = 0; i < (int)previewUrls.size(); i++)
	{
		if( previewUrls[i].isNull() )
			break;

		tstring strUrl = Ansi2Str(previewUrls[i].asCString());

		pThumbStream->WriteString("");
		pThumbStream->WriteString(strUrl);

		count ++;
	}
	
	pThumbStream->Write(0, (char*)&count, 4);
	pThumbStream->ResetCursor();

	return TRUE;
}

BOOL NDCloudDecode3DResourceList(IN char* pData, IN int nDataSize, OUT CStream* p3DResourceStream)
{
	pData[nDataSize] = '\0';
	string str = Utf8ToAnsi(pData);

	p3DResourceStream->ResetCursor();

	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(str, root);
	if( !res )
		return FALSE;

	if( root["total"].isNull() )
		return FALSE;

	// 
	Json::Value& items = root["items"];
	int count = items.size();

	// count
	p3DResourceStream->WriteDWORD(count);

	for(int i = 0; i < (int)items.size(); i++)
	{
		Json::Value& item = items[i];

		tstring strTitle;
		tstring strGuid;
		tstring strDesc;
		tstring strPreviewUrl;
		tstring strFileUrl;

		if( !item["title"].isNull() )
			strTitle = item["title"].asCString();

		if( !item["identifier"].isNull() )
			strGuid = item["identifier"].asCString();

		if( !item["description"].isNull() )
			strDesc = item["description"].asCString();

		// preview
		if( !item["preview"].isNull() && !item["preview"]["cover"].isNull() )
			strPreviewUrl = item["preview"]["cover"].asCString();
		else if( !item["preview"].isNull() && !item["preview"]["png"].isNull())
		{
			strPreviewUrl = item["preview"]["png"].asCString();
		}
		
		// file url
		if( !item["tech_info"].isNull() && !item["tech_info"]["href"].isNull() && !item["tech_info"]["href"]["location"].isNull() )
			strFileUrl = item["tech_info"]["href"]["location"].asCString();


		// guid | title | description | fileUrl |  PreviewUrl
		p3DResourceStream->WriteString(strGuid);
		p3DResourceStream->WriteString(strTitle);		
	//	p3DResourceStream->WriteString(strDesc);
		p3DResourceStream->WriteString(strFileUrl);
		p3DResourceStream->WriteString(strPreviewUrl);
		p3DResourceStream->WriteString(_T(""));
	}

	// total
	int total = root["total"].asInt();
	p3DResourceStream->WriteDWORD(total);

	tstring strLimit = Ansi2Str(root["limit"].asCString());
	int nStart, nEnd;
	if(sscanf_s(strLimit.c_str(), _T("(%d,%d)"), &nStart, &nEnd ) == 2)
	{
		p3DResourceStream->WriteDWORD(nStart);
		p3DResourceStream->WriteDWORD(nEnd);
	}

	p3DResourceStream->ResetCursor();
	return TRUE;
}


BOOL NDCloudDecodeVRResourceList(IN char* pData, IN int nDataSize, OUT CStream* pVRResourceStream)
{
	pData[nDataSize] = '\0';
	string str = Utf8ToAnsi(pData);

	pVRResourceStream->ResetCursor();

	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(str, root);
	if( !res )
		return FALSE;

	if( root["total"].isNull() )
		return FALSE;

	// 
	Json::Value& items = root["items"];
	int count = items.size();

	// count
	pVRResourceStream->WriteDWORD(count);

	for(int i = 0; i < (int)items.size(); i++)
	{
		Json::Value& item = items[i];

		tstring strTitle;
		tstring strGuid;
		tstring strDesc;
		tstring strPreviewUrl;
		tstring strFileUrl;
		tstring strApkPackageName;
		tstring strApkActivityName;
		tstring strApkVersion;
		DWORD   dwSize = 0;

		if( !item["title"].isNull() )
			strTitle = Ansi2Str(item["title"].asCString());

		if( !item["identifier"].isNull() )
			strGuid = Ansi2Str(item["identifier"].asCString());

		if( !item["description"].isNull() )
			strDesc = Ansi2Str(item["description"].asCString());

		// preview
		if( !item["preview"].isNull() && !item["preview"]["cover"].isNull() )
			strPreviewUrl = Ansi2Str(item["preview"]["cover"].asCString());

		else if( !item["preview"].isNull() && !item["preview"]["png"].isNull())
		{
			strPreviewUrl = Ansi2Str(item["preview"]["png"].asCString());
		}

		// file url
		if( !item["tech_info"].isNull() && !item["tech_info"]["href"].isNull() )
		{
			tstring strFormat;
			if( !item["tech_info"]["href"]["format"].isNull() )
				strFormat = Ansi2Str(item["tech_info"]["href"]["format"].asCString());

			if( strFormat != _T("application/zip") )
			{
				count --;
				continue;
			}
				
			if( !item["tech_info"]["href"]["location"].isNull() )
				strFileUrl = Ansi2Str(item["tech_info"]["href"]["location"].asCString());

			if( !item["tech_info"]["href"]["size"].isNull() )
				dwSize = item["tech_info"]["href"]["size"].asUInt();
		}

		// apkPackageName | apkActivityName
		if( !item["custom_properties"].isNull() )
		{
			if( !item["custom_properties"]["apk_package_name"].isNull() )
				strApkPackageName = Ansi2Str(item["custom_properties"]["apk_package_name"].asCString());

			if( !item["custom_properties"]["apk_activity_name"].isNull() )
				strApkActivityName = Ansi2Str(item["custom_properties"]["apk_activity_name"].asCString());	

			if( !item["custom_properties"]["app_version"].isNull() )
				strApkVersion = Ansi2Str(item["custom_properties"]["app_version"].asCString());	
		}


		// guid | title | description | fileUrl |  PreviewUrl | apkPackageName | apkActivityName | size
		pVRResourceStream->WriteString(strGuid);
		pVRResourceStream->WriteString(strTitle);		
		pVRResourceStream->WriteString(strDesc);
		pVRResourceStream->WriteString(strFileUrl);
		pVRResourceStream->WriteString(strPreviewUrl);
		pVRResourceStream->WriteString(strApkPackageName);
		pVRResourceStream->WriteString(strApkActivityName);
		pVRResourceStream->WriteString(strApkVersion);
		pVRResourceStream->WriteDWORD(dwSize);
		

		// summary 
		if( !item["preview"].isNull() )
		{
			int nSummaryCount = 0;
			for(int j = 0; ; j++)
			{
				TCHAR szSummaryKey[MAX_PATH];
				_stprintf_s(szSummaryKey, _T("summary%d"), j+1);

				if( item["preview"][szSummaryKey].isNull() )
					break;

				nSummaryCount ++;
			}

			// summary count
			pVRResourceStream->WriteInt(nSummaryCount);

			for(int j = 0; ; j++)
			{
				TCHAR szSummaryKey[MAX_PATH];
				_stprintf_s(szSummaryKey, _T("summary%d"), j+1);

				if( item["preview"][szSummaryKey].isNull() )
					break;

				tstring strSummaryUrl = Ansi2Str(item["preview"][szSummaryKey].asCString());
				pVRResourceStream->WriteString(strSummaryUrl);
			}
		}


	}

	pVRResourceStream->Write(0, (char*)&count, 4);

	// total
	int total = root["total"].asInt();
	pVRResourceStream->WriteDWORD(total);

	tstring strLimit = Ansi2Str(root["limit"].asCString());
	int nStart, nEnd;
	if(sscanf_s(strLimit.c_str(), _T("(%d,%d)"), &nStart, &nEnd ) == 2)
	{
		pVRResourceStream->WriteDWORD(nStart);
		pVRResourceStream->WriteDWORD(nEnd);
	}

	pVRResourceStream->ResetCursor();
	return TRUE;
}

BOOL NDCloudDecodeCategoryList( IN char* pData, IN int nDataSize, OUT CStream* pStream )
{
	pData[nDataSize] = '\0';
	string str = Utf8ToAnsi(pData);

	pStream->ResetCursor();

	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(str, root);
	if( !res )
		return FALSE;

	if( root["total"].isNull() )
		return FALSE;

	// 
	Json::Value& items = root["items"];
	int count = items.size();

	// count
	pStream->WriteDWORD(count);

	for(int i = 0; i < (int)items.size(); i++)
	{
		Json::Value& item = items[i];

		tstring strTitle;
		tstring strGuid;
		tstring strNdCode;

		if( !item["title"].isNull() )
			strTitle = Ansi2Str(item["title"].asCString());

		if( !item["identifier"].isNull() )
			strGuid = Ansi2Str(item["identifier"].asCString());

		if( !item["nd_code"].isNull() )
			strNdCode = Ansi2Str(item["nd_code"].asCString());

		pStream->WriteString(strGuid);
		pStream->WriteString(strTitle);		
		pStream->WriteString(strNdCode);
	}

	// total
	int total = root["total"].asInt();
	pStream->WriteDWORD(total);

	tstring strLimit = Ansi2Str(root["limit"].asCString());
	int nStart, nEnd;
	if(sscanf_s(strLimit.c_str(), _T("(%d,%d)"), &nStart, &nEnd ) == 2)
	{
		pStream->WriteDWORD(nStart);
		pStream->WriteDWORD(nEnd);
	}


	pStream->ResetCursor();

	return TRUE;
}
//BOOL NDCloudDecodePPTThumbnailList(IN char* pData, IN int nDataSize, tstring strXmlFilePath, OUT CStream* pThumbStream)
//{
//	
//	TiXmlDocument doc;
//	if( pData == NULL || nDataSize == 0 )
//	{
//		bool res = doc.LoadFile(strXmlFilePath.c_str());
//		if( !res )
//			return FALSE;
//	}
//	else
//	{
//		// utf8 to ansi
//		pData[nDataSize] = '\0';
//		string str = Utf8ToAnsi(pData);
//		
//		doc.Parse(str.c_str());
//	}
//	
//	TiXmlElement* pRootElement = doc.FirstChildElement();
//	if( pRootElement == NULL )
//		return FALSE;
//	
//	TiXmlElement* pPagesElement = GetElementsByTagName(pRootElement, "pages");
//	if( pPagesElement == NULL )
//		return FALSE;
//
//	const char* value = pPagesElement->Value();
//	if( value == NULL || strcmpi(value, "pages") )
//		return FALSE;
//	
//	//
//	vector<tstring> vecStrings;
//	vecStrings.clear();
//
//	TiXmlElement* pPageElement = pPagesElement->FirstChildElement();
//	while( pPageElement != NULL )
//	{
//		const char* pValue= pPageElement->Value();
//		if( pValue == NULL || strcmpi(pValue, "page") )
//			break;
//
//		tstring strName = pPageElement->Attribute("name");
//		tstring strUrl = pPageElement->Attribute("preview");
// 
//		vecStrings.push_back(strName);
//		vecStrings.push_back(strUrl);
//
//		pPageElement = pPageElement->NextSiblingElement();
//	}
//
//	int count = vecStrings.size() / 2;
//	pThumbStream->WriteDWORD(count);
//	
//	for(int i = 0; i < count; i++)
//	{
//		pThumbStream->WriteString((char*)vecStrings[2*i].c_str());
//		pThumbStream->WriteString((char*)vecStrings[2*i+1].c_str());
//	}
//
//	pThumbStream->ResetCursor();
//	return TRUE;
//}

//---------------------------------------------------------------------
// Obtain global variables
//
tstring NDCloudGetBookGUID()
{
	return g_strBookGUID;
}

tstring NDCloudGetChapterGUID()
{
	return g_strChapterGUID;
}

BOOL NDCloudSetChapterGUID(tstring strGUID, tstring strChapterName, bool bInit)
{
	g_strChapterGUID = strGUID;

	tstring strPath = GetLocalPath();
	strPath += _T("\\Setting\\Config.ini");

	WritePrivateProfileString(_T("Config"),_T("ChapterGuid"),strGUID.c_str(), strPath.c_str());
	WritePrivateProfileString(_T("Config"),_T("ChapterName"),strChapterName.c_str(), strPath.c_str());

	//
	if( g_pCategoryTree == NULL || g_pChapterTree == NULL )
		return FALSE;

	ChapterNode* pChapterNode = g_pChapterTree->FindNode(strGUID);
	if( pChapterNode == NULL )
		return FALSE;

	// 人教版-三年级-语文-2 荷叶与母亲
	CategoryNode* pEditionNode = g_pCategoryTree->GetSelectedNode();
	if( pEditionNode == NULL )
	{
		TCHAR szSection[MAX_PATH]		= _T("");
		TCHAR szGrade[MAX_PATH]			= _T("$ON020100");
		TCHAR szCourse[MAX_PATH]		= _T("");
		TCHAR szEdition[MAX_PATH]		= _T("");
		TCHAR szSubEdition[MAX_PATH]	= _T("");


		TCHAR szCourseRecord[1024] = {0};

		GetPrivateProfileString(_T("Config"),_T("CourseRecord"),_T(""),szCourseRecord, sizeof(szCourseRecord)-1, strPath.c_str());

		if(sscanf_s(szCourseRecord,_T("%[^/]/%[^/]/%[^/]/%[^/]/%[^/]"), 
			szSection, sizeof(szSection) - 1,
			szGrade, sizeof(szGrade) - 1,
			szCourse, sizeof(szCourse) - 1,
			szEdition, sizeof(szEdition) - 1,
			szSubEdition,sizeof(szSubEdition) - 1) == 5)
		{
			g_pCategoryTree->SelectSectionCode(szSection);
			g_pCategoryTree->SelectGradeCode(szGrade);
			g_pCategoryTree->SelectCourseCode(szCourse);
			g_pCategoryTree->SelectEditionCode(szEdition);
			g_pCategoryTree->SelectSubEditionCode(szSubEdition);
		}

		pEditionNode = g_pCategoryTree->GetSelectedNode();

	}

	// prefix
	tstring strPrefix;
	if( pEditionNode != NULL )
	{
		CategoryNode* pCourseNode = pEditionNode->pParent;
		CategoryNode* pGradeNode = pCourseNode == NULL ? NULL : pCourseNode->pParent;

		//////////////////////////////////////////////////////////////////////////
		//只显示年级和课程名称，科目和出版社不显示
		//strPrefix = pEditionNode->strTitle;
		tstring strGrade	= pGradeNode == NULL ? _T("") : pGradeNode->strTitle;
		tstring strCourse   = pCourseNode == NULL ? _T("") : pCourseNode->strTitle;

		strGrade = strGrade == _T("十年级") ? _T("高中") : strGrade;
		strPrefix = strGrade;
		//strPrefix += "-";
		//strPrefix += strGrade;
		//strPrefix += "-";
		//strPrefix += strCourse;
		//////////////////////////////////////////////////////////////////////////
	}

	if( strPrefix != _T("") )
	{
		strPrefix += _T("-");
		strChapterName = strPrefix + strChapterName;
	}

	CStream stream(1024);
	stream.WriteString((char*)strGUID.c_str());
	stream.WriteString((char*)strChapterName.c_str());
	stream.WriteDWORD(bInit);

	BroadcastEvent(EVT_SET_CHAPTER_GUID, &stream);

	// preload resource 
	//NDCloudPreLoader::GetInstance()->PreLoad(strGUID);

	return TRUE;
}

BOOL NDCloudGetCategoryTree(CCategoryTree*& pCategory)
{
	pCategory = g_pCategoryTree;
	return TRUE;
}

BOOL NDCloudGetChapterTree(CChapterTree*& pChapter)
{
	pChapter = g_pChapterTree;
	return TRUE;
}

tstring NDCloudPrevChapter(BOOL& bFirst)
{
	bFirst = FALSE;

	if( g_pChapterTree == NULL )
		return _T("");

	ChapterNode* pNode = g_pChapterTree->FindNode(g_strChapterGUID);
	if( pNode == NULL )
		return _T("");
/*
	if( pNode->pPrevSlibing == NULL )
	{
		bFirst = TRUE;
		NDCloudSetChapterGUID(pNode->strGuid, pNode->strTitle);
		return pNode->strTitle;
	}

	ChapterNode* pPrevSlibing = pNode->pPrevSlibing;
	if( pPrevSlibing->pPrevSlibing == NULL )
		bFirst = TRUE;
*/
	ChapterNode* pPrevSlibing = TraversalChapterTree(pNode, TRUE);

	//
	NDCloudSetChapterGUID(pPrevSlibing->strGuid, pPrevSlibing->strTitle);
	return pPrevSlibing->strTitle;
}

tstring NDCloudCurrentChapter()
{
	tstring strChapter="";
	if( g_pChapterTree != NULL )
	{
		ChapterNode* pNode = g_pChapterTree->FindNode(g_strChapterGUID);
		if( pNode != NULL )
		{
			strChapter = pNode->strTitle;
		}
	}
	return strChapter;
}

tstring	NDCloudNextChapter(BOOL& bLast)
{
	bLast = FALSE;

	if( g_pChapterTree == NULL )
		return _T("");

	ChapterNode* pNode = g_pChapterTree->FindNode(g_strChapterGUID);
	if( pNode == NULL )
		return _T("");
/*
	if( pNode->pNextSlibing == NULL )
	{
		bLast = TRUE;
		NDCloudSetChapterGUID(pNode->strGuid, pNode->strTitle);
		return pNode->strTitle;
	}

	ChapterNode* pNextSlibing = pNode->pNextSlibing;
	if( pNextSlibing->pNextSlibing == NULL )
		bLast = TRUE;
*/
	ChapterNode* pNextSlibing = TraversalChapterTree(pNode, FALSE);

	NDCloudSetChapterGUID(pNextSlibing->strGuid, pNextSlibing->strTitle);
	return pNextSlibing->strTitle;
}

void NDCloudSaveCategoryTrees(CCategoryTree* pCategory)
{
	if(pCategory)
	{
		if(g_pCategoryTree)
			g_pCategoryTree->DestroyTree();
		else
			g_pCategoryTree = new CCategoryTree;
		g_pCategoryTree = pCategory;
	}
}

void NDCloudSaveChapterTrees(CChapterTree* pChapter)
{
	if(pChapter)
	{
		if(g_pChapterTree)
			g_pChapterTree->DestroyTree();
		else
			g_pChapterTree = new CChapterTree;
		g_pChapterTree = pChapter;
	}
}

BOOL NDCloudCurrentPos(BOOL& bFirst, BOOL& bLast)
{
	bFirst	= TRUE;
	bLast	= TRUE;

	if( g_pChapterTree == NULL )
		return FALSE;

	ChapterNode* pNode = g_pChapterTree->FindNode(g_strChapterGUID);

	if( pNode == NULL )
		return FALSE;

	bFirst = isEndChapterTreeNode(pNode,TRUE);
	bLast = isEndChapterTreeNode(pNode,FALSE);
/*
	if( pNode == NULL )
		return FALSE;

	if( pNode->pNextSlibing != NULL )
		bLast = FALSE;
	
	if( pNode->pPrevSlibing != NULL )
		bFirst = FALSE;
*/
	return TRUE;
}

BOOL NDCloudLoadTrees()
{
	BOOL bRet = TRUE;
	if( g_pCategoryTree != NULL )
		bRet = g_pCategoryTree->LoadTree();

	if(!bRet)
		return bRet;

	if( g_pChapterTree == NULL )
		g_pChapterTree = new CChapterTree;

	if( g_pChapterTree != NULL )
		bRet = g_pChapterTree->LoadTree();

	tstring strPath = GetLocalPath();
	strPath += _T("\\Setting\\Config.ini");

	TCHAR szGuid[128] = {0};
	GetPrivateProfileString(_T("Config"),_T("ChapterGuid"),_T(""), szGuid, sizeof(szGuid) - 1, strPath.c_str());

	TCHAR szChapterName[128] = {0};
	GetPrivateProfileString(_T("Config"),_T("ChapterName"),_T(""), szChapterName, sizeof(szChapterName) - 1, strPath.c_str());

	if(_tcslen(szGuid) == 0)
		bRet = FALSE;
	else
	{
		NDCloudSetChapterGUID(szGuid, szChapterName);
	}

	return bRet;
}

BOOL NDCloudSaveTrees()
{
	if( g_pCategoryTree != NULL )
		g_pCategoryTree->SaveTree();

	if( g_pChapterTree != NULL )
		g_pChapterTree->SaveTree();

	return TRUE;
}

BOOL NDCloudClearTrees()
{
	tstring strLocalPath = GetLocalPath();

	TCHAR szPath[MAX_PATH];
	_stprintf_s(szPath, _T("%s\\Setting\\Category.dat"), strLocalPath.c_str());

	DeleteFile(szPath);

	return TRUE;
}

