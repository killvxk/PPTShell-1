//-----------------------------------------------------------------------
// FileName:				NDCloudPreLoader.h
//
// Desc:
//------------------------------------------------------------------------
#ifndef _ND_CLOUD_PRELOADER_H_
#define _ND_CLOUD_PRELOADER_H_

#include "NDCloudAPI.h"
#include "Util/Singleton.h"

class CNDCloudPreLoader
{
private:
	CNDCloudPreLoader();
	~CNDCloudPreLoader();

public:
	BOOL Initialize();
	BOOL Destory();
	void PreLoad(tstring strChapterGUID);
	void SetPreLoadCount(int nFileType, int nCount);

	DECLARE_SINGLETON_CLASS(CNDCloudPreLoader);

protected:
	void PreLoadCourseFile();
	void PreLoadVideoFile();
	void PreLoadImageFile();
	void PreLoadFlashFile();
	void PreLoadVolumeFile();

protected:
	bool OnDownloadCourseList(void* param);
	bool OnDownloadCourseFileThumbnail(void* param);
	bool OnDownloadCourseFileThumb(void* param);
	bool OnDownloadCourseFilePPT(void* param);

protected:
	tstring					m_strChapterGUID;
	int						m_nPreLoadCounts[CloudFileTotal];

	// course
	int						m_nCourseCount;
	int						m_nLastPPTThumbCount;
	tstring					m_strLastPPTGuid;
	tstring					m_strLastPPTTitle;
	tstring					m_strLastPPTUrl;
	CStream*				m_pCourseStream;
						
};

typedef Singleton<CNDCloudPreLoader>	NDCloudPreLoader;

#endif