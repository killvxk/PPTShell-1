//-----------------------------------------------------------------------
// FileName:				NDCloudAPI.h
//
// Desc:
//------------------------------------------------------------------------
#ifndef _ND_CLOUD_API_H_
#define _ND_CLOUD_API_H_

#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "Http/HttpUpload.h"
#include "Util/Stream.h"
#include "CategoryTree.h"
#include "ChapterTree.h"
#include "NDCloudFile.h"


enum ResFileType
{
	CloudFileSearch = 0,
	CloudFileCourse,
	CloudFileVideo,
	CloudFileImage,
	CloudFileFlash,
	CloudFileVolume,
	CloudFileQuestion,
	CloudFileCoursewareObjects,
	CloudFilePPTTemplate,
	CloudFileNdpCourse,
	CloudFile3DResource,
	CloudFileNdpxCourse,
	CloudFileVRResource,
	CloudFileAssets,
	CloudFileTotal,
	CloudFileImageEx,

	LocalFileSearch = 50,
	LocalFileImport,
	LocalFileCourse,
	LocalFileVideo,
	LocalFileImage,
	LocalFileFlash,
	LocalFileVolume,
	LocalFileBasicExercises,
	LocalFileInteractiveExercises,
	LocalFileAnother,
	LocalFileTotal,

	DBankSearch = 100,
	DBankUpload,
	DBankCourse,
	DBankVideo,
	DBankImage,
	DBankFlash,
	DBankVolume,
	DBankQuestion,
	DBankCoursewareObjects,
	DBankPPTTemplate,
	DBankNdpCourse,
	DBank3DResource,
	DBankNdpxCourse,
	DBankAssets,
	DBankTotal,

	ResFileTotal,


	DBankLogin,
	DBankRegister,
	SearchALLRes,	//搜索全部资源
};


enum CourseFileType
{
	CourseFileXml,
	CourseFileThumb,
	CourseFilePPT,
};

//---------------------------------------------------------------------
// 云数据下载相关功能
//
//
BOOL		NDCloudInitialize(CHttpDownloadManager* pHttpManager);
BOOL		NDCloudDestroy();

DWORD		NDCloudDownload(tstring strUrl, CHttpDelegateBase& OnCompleteCB, void* pUserData = NULL);
DWORD		NDCloudDownload(tstring strUrl, CHttpDelegateBase& OnCompleteCB, CHttpDelegateBase& OnProgressCB, void* pUserData = NULL);
DWORD		NDCloudDownloadEx(tstring strUrl, tstring strHeader, tstring strMethod, string strPost, CHttpDelegateBase& OnCompleteCB, void* pUserData = NULL);

DWORD		NDCloudDownloadFile(tstring strUrl, tstring strGuid, tstring strName, int nCloudFileType, int nThumbSize, CHttpDelegateBase& OnCompleteCB, void* pUserData = NULL, tstring strMD5 = _T(""));
DWORD		NDCloudDownloadFile(tstring strUrl, tstring strGuid, tstring strName, int nCloudFileType, int nThumbSize, CHttpDelegateBase& OnCompleteCB, CHttpDelegateBase& OnProgressCB, void* pUserData = NULL, tstring strMD5 = _T(""));
DWORD		NDCloudDownloadCourseFile(tstring strUrl, tstring strGuid, tstring strPPTName, int nCourseFileType, CHttpDelegateBase& OnCompleteCB, void* pUserData = NULL, tstring strMD5 = _T(""));
DWORD		NDCloudDownloadCourseFile(tstring strUrl, tstring strGuid, tstring strPPTName, int nCourseFileType, CHttpDelegateBase& OnCompleteCB, CHttpDelegateBase& OnProgressCB, void* pUserData = NULL, tstring strMD5 = _T(""));

BOOL		NDCloudDownloadPriority(DWORD dwTaskId, int nPriority);
BOOL		NDCloudDownloadCancel(DWORD dwTaskId, CHttpDelegateBase* pOnCompleteDelegate = NULL, CHttpDelegateBase* pOnProgressDelegate = NULL);
BOOL		NDCloudDownloadPause(DWORD dwTaskId, CHttpDelegateBase* pOnPauseDelegate = NULL);
BOOL		NDCloudDownloadResume(DWORD dwTaskId);

//---------------------------------------------------------------------
// 目录树 章节树等全局变量操作接口
//
tstring		NDCloudGetBookGUID();
tstring		NDCloudGetChapterGUID();
BOOL		NDCloudSetChapterGUID(tstring strGUID, tstring strChapterName, bool bInit = false);
BOOL		NDCloudGetCategoryTree(CCategoryTree*& pCategory);
BOOL		NDCloudGetChapterTree(CChapterTree*& pChapter);
tstring		NDCloudPrevChapter(BOOL& bFirst);
tstring		NDCloudCurrentChapter();
tstring		NDCloudNextChapter(BOOL& bLast);
void		NDCloudSaveChapterTrees(CChapterTree* pChapter);
void		NDCloudSaveCategoryTrees(CCategoryTree* pCategory);
BOOL		NDCloudLoadTrees();
BOOL		NDCloudClearTrees();
BOOL		NDCloudSaveTrees();
BOOL		NDCloudCurrentPos(BOOL& bFirst, BOOL& bLast);


//---------------------------------------------------------------------
// URL地址组合相关功能
//

//
// 功能: 组合查询学段，年级，课程，出版社，上下册等信息的地址
//
// 参数:
// strGradeCode:	年级编码 (比如小学一年纪的编码)
// strCourseCode:	课程编码 (比如语文课的编码)
//
// 备注:			如果两个参数都为空的话，则表示一次性获取小学一年级的数据，
//					另外不能只提供课程编码而不提供年纪编码
//
tstring		NDCloudComposeUrlCategory(tstring strGradeCode = _T(""), tstring strCourseCode = _T(""));


//
// 功能:			获取书本信息 (书名和GUID编号)
//
// 参数:			
// strSectionCode:	年级段 (小学初中高中)
// strGradeCode:	年纪 (一年级 二年级)	
// strCourseCode:	学科 (语文 数学)
// strVersionCode:	教材版本 
// strSubVersionCode: 教材子版本
//
tstring		NDCloudComposeUrlBookInfo(tstring strSectionCode, tstring strGradeCode, tstring strCourseCode, tstring strEditionCode, tstring strSubEditionCode, int nLimitStart, int nLimitEnd);


//
// 功能:			获取书本章节信息  
//
// 参数:			
// strBookGUID:		书本的GUID编号
//
tstring		NDCloudComposeUrlChapterInfo(tstring strBookGUID);


//
// 功能:			获取课件总数
//
// 参数:
// strSearchKeyWord: 搜索关键字，如果为空表示总课件数，否则为搜索到的课件数量
//
tstring		NDCloudComposeUrlCourseCount(tstring strChapterGUID = _T(""), tstring strSearchKeyWord = _T(""));

//
// 功能:			获取课件信息
//
// 参数:			
// strSearchKeyWord:搜索关键字			
// nLimitStart:		搜索课件信息起始索引
// nLimitEnd:		搜索课件信息结束索引
//
tstring		NDCloudComposeUrlCourseInfo(tstring strChapterGUID = _T(""), tstring strSearchKeyWord = _T(""), int nLimitStart = 0, int nLimitEnd = 0, DWORD dwUserId = 0);


tstring		NDCloudComposeUrlPictureCount(tstring strChapterGUID = _T(""), tstring strSearchKeyWord = _T(""));
tstring		NDCloudComposeUrlPictureInfo(tstring strChapterGUID, tstring strSearchKeyWord, int nLimitStart, int nLimitEnd, DWORD dwUserId = 0);

tstring		NDCloudComposeUrlVolumeCount(tstring strChapterGUID = _T(""), tstring strSearchKeyWord = _T(""));
tstring		NDCloudComposeUrlVolumeInfo(tstring strChapterGUID, tstring strSearchKeyWord, int nLimitStart, int nLimitEnd, DWORD dwUserId = 0);

tstring		NDCloudComposeUrlVideoCount(tstring strChapterGUID = _T(""), tstring strSearchKeyWord = _T(""));
tstring		NDCloudComposeUrlVideoInfo(tstring strChapterGUID, tstring strSearchKeyWord, int nLimitStart, int nLimitEnd, DWORD dwUserId = 0);

tstring		NDCloudComposeUrlFlashCount(tstring strChapterGUID = _T(""), tstring strSearchKeyWord = _T(""));
tstring		NDCloudComposeUrlFlashInfo(tstring strChapterGUID, tstring strSearchKeyWord, int nLimitStart, int nLimitEnd, DWORD dwUserId = 0);

tstring		NDCloudComposeUrlPPTTemplateCount(tstring strSectionCode = _T(""), tstring strCourseCode = _T(""), tstring strSearchKeyWord = _T(""));
tstring		NDCloudComposeUrlPPTTemplateInfo(tstring strSectionCode, tstring strCourseCode, tstring strSearchKeyWord, int nLimitStart, int nLimitEnd, DWORD dwUserId = 0);

tstring		NDCloudComposeUrl3DResourceCount(tstring strChapterGUID = _T(""), tstring strSearchKeyWord = _T(""));
tstring		NDCloudComposeUrl3DResourceInfo(tstring strChapterGUID, tstring strSearchKeyWord, int nLimitStart, int nLimitEnd, DWORD dwUserId = 0);

tstring		NDCloudComposeUrlVRResourceCount(tstring strChapterGUID = _T(""), tstring strSearchKeyWord = _T(""), tstring strCategory = _T(""));
tstring		NDCloudComposeUrlVRResourceInfo(tstring strChapterGUID, tstring strSearchKeyWord, tstring strCategory, int nLimitStart, int nLimitEnd, DWORD dwUserId = 0);

tstring		NDCloudComposeUrlQuestionCount(tstring strChapterGUID = _T(""), tstring strSearchKeyWord = _T(""));
tstring		NDCloudComposeUrlQuestionInfo(tstring strChapterGUID, tstring strSearchKeyWord, int nLimitStart, int nLimitEnd, DWORD dwUserId = 0);
tstring		NDCloudComposeUrlPackQuestion(tstring strQuestionGUID, DWORD dwUserId);
tstring		NDCloudComposeUrlPackQuestionState(tstring strQuestionGUID, DWORD dwUserId);

tstring		NDCloudComposeUrlCoursewareObjectsCount(tstring strChapterGUID = _T(""), tstring strSearchKeyWord = _T(""));
tstring		NDCloudComposeUrlCoursewareObjectsInfo(tstring strChapterGUID, tstring strSearchKeyWord, int nLimitStart, int nLimitEnd, DWORD dwUserId = 0);
tstring		NDCloudComposeUrlPackCoursewareObjects(tstring strGUID, DWORD dwUserId);
tstring		NDCloudComposeUrlPackCoursewareObjectsState(tstring strGUID, DWORD dwUserId);

tstring		NDCloudComposeUrlToolsCount(tstring strChapterGUID = _T(""), tstring strSearchKeyWord = _T("") );
tstring		NDCloudComposeUrlToolsInfo(tstring strChapterGUID, tstring strSearchKeyWord, int nLimitStart, int nLimitEnd);

tstring		NDCloudComposeUrlCagegoryInfo(tstring strSearchKeyWord, int nLimitStart, int nLimitEnd);
//---------------------------------------------------------------------
// Json字符串结果解析相关
//
int			NDCloudDecodeCount(IN char* pData, IN int nDataSize);

int			NDCloudDecodeExercisesCount(char* pData, int nDataSize);

//
// 功能:			获取目录信息
//
// 参数:
// CourseTree:		返回目录树，后续直接操作该树 
//
// 备注:			使用完必须调用delete函数释放pOutputStream 
//
BOOL		NDCloudDecodeCategory(IN char* pData, IN int nDataSize, OUT CCategoryTree*& pCourseTree);

BOOL		NDCloudDecodeCategory(IN char* pData, IN int nDataSize,IN tstring strSectionCode, IN tstring strGradeCode, IN tstring strCourseCode, IN tstring strEditionCode);

//
// 功能:			获取书本GUID
//
// 参数:
//
//
BOOL		NDCloudDecodeBookGUID(IN char* pData, IN int nDataSize,tstring& strBookGUID);


//
// 功能:			获取章节信息
//
// 参数:
//
//
BOOL		NDCloudDecodeChapterInfo(IN char* pData, IN int nDataSize, IN CChapterTree* pChapterTree);


//
// 功能:			获取图片列表信息
//
// 参数:		
// ImageStream:		count|guid1|title1|url1|guid2|title2|url2...
//
BOOL		NDCloudDecodeImageList(IN char* pData, IN int nDataSize, OUT CStream* pImageStream);

//
// 功能:			获取音频列表信息
//
// 参数:		
// VolumeStream:	count|title1|url1|title2|url2...
//
BOOL		NDCloudDecodeVolumeList(IN char* pData, IN int nDataSize, OUT CStream* pVolumeStream);


//
// 功能:			获取视频列表信息
//
// 参数:		
// VideoStream:		count|title1|url1|title2|url2...
//
BOOL		NDCloudDecodeVideoList(IN char* pData, IN int nDataSize, OUT CStream* pVideoStream);


//
// 功能:			获取动画列表信息
//
// 参数:		
// VideoStream:		count|title1|url1|title2|url2.
//
BOOL		NDCloudDecodeFlashList(IN char* pData, IN int nDataSize, OUT CStream* pFlashStream);

//
// 功能:			获取PPT模板列表信息
//
// 参数:		
// VideoStream:		count|title1|guid1|pptUrl1|previews_count1|previews1|title2|guid2|pptUrl2|previews_count2|previews2|
//
BOOL		NDCloudDecodePPTTemplateList(IN char* pData, IN int nDataSize, OUT CStream* pPPTTemplateStream);


//
// 功能:			获取课件列表信息
//
// 参数:		
// VideoStream:		count|title1|guid1|pptUrl1|previews_count1|previews1|title2|guid2|pptUrl2|previews_count2|previews2|
//
BOOL		NDCloudDecodeCourseList(IN char* pData, IN int nDataSize, OUT CStream* pCourseStream);

//
// 功能:			获取习题列表信息
//
// 参数:		
// VideoStream:		count|title1|guid1|dercription1|questiontype1|preview1|title2|guid2|dercription2|questiontype2|preview2
//
BOOL		NDCloudDecodeQuestionList(IN char* pData, IN int nDataSize, OUT CStream* pQuestionStream);

//
// 功能:			获取互动题型列表信息
//
// 参数:		
// 
//
BOOL		NDCloudDecodeCoursewareObjectsList(IN char* pData, IN int nDataSize, OUT CStream* pCoursewareObjectsStream);

//
// 功能:			获取PPT缩略图列表信息  
//
// 参数:		
// VideoStream:		count|name1|ur1|name2|ur2|....
//
BOOL		NDCloudDecodePPTThumbnailList(IN char* pData, IN int nDataSize, OUT CStream* pThumbStream);

//
// 功能:			获取3D资源列表信息
//
// 参数:		
// VideoStream:		count|title1|guid1|description1|PreviewUrl1|fileUrl1 
//
BOOL		NDCloudDecode3DResourceList(IN char* pData, IN int nDataSize, OUT CStream* p3DResourceStream);

//
// 功能:			获取VR资源列表信息
//
// 参数:		
// VideoStream:		count| guid | title | description | fileUrl |  PreviewUrl | apkPackageName | apkActivityName
//
BOOL		NDCloudDecodeVRResourceList(IN char* pData, IN int nDataSize, OUT CStream* pVRResourceStream, BOOL bIncludeEmptyVR = TRUE);

//
// 功能:			获取学科信息
//
// 参数:		
// 
//
BOOL		NDCloudDecodeCategoryList(IN char* pData, IN int nDataSize, OUT CStream* pStream);

#endif