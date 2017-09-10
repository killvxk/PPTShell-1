//-----------------------------------------------------------------------
// FileName:				NDCloudPreLoader.cpp
//
// Desc:
//------------------------------------------------------------------------
#include "stdafx.h"
#include "NDCloudPreLoader.h"

CNDCloudPreLoader::CNDCloudPreLoader()
{
	memset(m_nPreLoadCounts, 0, sizeof(m_nPreLoadCounts));
	m_nPreLoadCounts[CloudFileCourse] = 5;

	m_pCourseStream = new CStream(1024);
}

CNDCloudPreLoader::~CNDCloudPreLoader()
{
	delete m_pCourseStream;
	m_pCourseStream = NULL;
}	

BOOL CNDCloudPreLoader::Initialize()
{
	return TRUE;
}

BOOL CNDCloudPreLoader::Destory()
{
	return TRUE;
}

//
// set preload count
//
void CNDCloudPreLoader::SetPreLoadCount(int nFileType, int nCount)
{
	m_nPreLoadCounts[nFileType] = nCount;
}

//
// preload all type of files
//
void CNDCloudPreLoader::PreLoad(tstring strChapterGUID)
{
	m_strChapterGUID = strChapterGUID;

	for(int i = 0; i < CloudFileTotal; i++)
	{
		if( m_nPreLoadCounts[i] == 0 )
			continue;

		switch( i )
		{
		case CloudFileCourse:
			PreLoadCourseFile();	
			break;

		case CloudFileVideo:
			//PreLoadVideoFile();
			break;

		case CloudFileImage:
			//PreLoadImageFile();
			break;

		case CloudFileFlash:
			//PreLoadFlashFile();
			break;

		case CloudFileVolume:
			//PreLoadVolumeFile();
			break;
		}
	}
}

//---------------------------------------------------------------------
// preload course file
//
void CNDCloudPreLoader::PreLoadCourseFile()
{
	tstring strUrl = NDCloudComposeUrlCourseInfo(m_strChapterGUID.c_str(), _T(""), 0, 200);
	NDCloudDownload(strUrl, MakeHttpDelegate(this, &CNDCloudPreLoader::OnDownloadCourseList));

}

bool CNDCloudPreLoader::OnDownloadCourseList(void* param)
{
	THttpNotify* pHttpNotify = (THttpNotify*)param;

	CStream stream(1024);
	if (!NDCloudDecodeCourseList(pHttpNotify->pData, pHttpNotify->nDataSize, &stream))
		return true;

	m_pCourseStream->ResetCursor();
	int nCourseCount = stream.ReadInt();

	// preload first n course's thumbnails and ppt files
	m_nCourseCount = nCourseCount < m_nPreLoadCounts[CloudFileCourse] ? nCourseCount : m_nPreLoadCounts[CloudFileCourse];

	for(int i = 0; i < m_nCourseCount; i++)
	{
		tstring strTitle	= stream.ReadString();
		tstring strGuid		= stream.ReadString();
		tstring strXmlUrl	= stream.ReadString();
		tstring strPPTUrl	= stream.ReadString();

		m_pCourseStream->WriteString(strTitle);
		m_pCourseStream->WriteString(strGuid);
		m_pCourseStream->WriteString(strXmlUrl);
		m_pCourseStream->WriteString(strPPTUrl);
			
	}

	m_pCourseStream->ResetCursor();
	
	// download first course
	tstring strTitle	= m_pCourseStream->ReadString();
	tstring strGuid		= m_pCourseStream->ReadString();
	tstring strThumbUrl	= m_pCourseStream->ReadString();
	tstring strPPTUrl	= m_pCourseStream->ReadString();

	m_strLastPPTUrl		= strPPTUrl;
	m_strLastPPTGuid	= strGuid;
	m_strLastPPTTitle	= strTitle;

	DWORD dwId = NDCloudDownload(strThumbUrl, MakeHttpDelegate(this, &CNDCloudPreLoader::OnDownloadCourseFileThumbnail));
	if( dwId != 0 )
	{

	}

	return true;
}

bool CNDCloudPreLoader::OnDownloadCourseFileThumbnail(void *param)
{
	// preload thumbnails
	THttpNotify* pHttpNotify = (THttpNotify*)param;

	CStream stream(1024);
	BOOL res = NDCloudDecodePPTThumbnailList(pHttpNotify->pData, pHttpNotify->nDataSize, &stream);
	if( !res )
		return true;
 
	// 
	int nCount = stream.ReadInt();
	m_nLastPPTThumbCount = nCount;

	for(int i = 0; i < nCount; i++)
	{
		tstring strUrl = stream.ReadString();
		NDCloudDownloadCourseFile(strUrl, m_strLastPPTGuid, m_strLastPPTTitle,  CourseFileThumb, MakeHttpDelegate(this, &CNDCloudPreLoader::OnDownloadCourseFileThumb));
	}

	return true;
}

bool CNDCloudPreLoader::OnDownloadCourseFileThumb(void *param)
{
	THttpNotify* pHttpNotify = (THttpNotify*)param;

	m_nLastPPTThumbCount --;

	// all thumbnail are downloaded then download ppt file
	if( m_nLastPPTThumbCount == 0 )
	{
		DWORD dwTaskId = NDCloudDownloadCourseFile(m_strLastPPTUrl, m_strLastPPTGuid, m_strLastPPTTitle,  CourseFilePPT, MakeHttpDelegate(this, &CNDCloudPreLoader::OnDownloadCourseFilePPT));
		NDCloudDownloadPriority(dwTaskId, LOWEST_DOWNLOAD_PRIORITY);
	}
	

	return true;
}

bool CNDCloudPreLoader::OnDownloadCourseFilePPT(void *param)
{
	// ppt file
	THttpNotify* pHttpNotify = (THttpNotify*)param;

	m_nCourseCount --;
	if( m_nCourseCount <= 0 )
		return true;

	// download first course
	tstring strTitle	= m_pCourseStream->ReadString();
	tstring strGuid		= m_pCourseStream->ReadString();
	tstring strThumbUrl	= m_pCourseStream->ReadString();
	tstring strPPTUrl	= m_pCourseStream->ReadString();


	m_strLastPPTUrl		= strPPTUrl;
	m_strLastPPTGuid	= strGuid;
	m_strLastPPTTitle	= strTitle;

	DWORD dwTaskId = NDCloudDownload(strThumbUrl, MakeHttpDelegate(this, &CNDCloudPreLoader::OnDownloadCourseFileThumbnail));
	if( dwTaskId != 0 )
		NDCloudDownloadPriority(dwTaskId, LOWEST_DOWNLOAD_PRIORITY);

	return true;
}



