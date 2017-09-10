#include "StdAfx.h"
#include "DUI/ITransfer.h"
#include "DUI/IVisitor.h"
#include "DUI/BaseParamer.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IDownloadListener.h"
#include "DUI/ResourceDownloader.h"
#include "DUI/QuestionDownloader.h"
#include "DUI/InteractQuestionDownloader.h"
#include "NDCloud/NDCloudAPI.h"
#include "NDCloud/NDCloudCoursewareObjects.h"

CInteractQuestionDownloader::CInteractQuestionDownloader()
{

}


CInteractQuestionDownloader::~CInteractQuestionDownloader()
{

}

bool CInteractQuestionDownloader::Transfer()
{
	BOOL bRet = NDCloudCoursewareObjectsManager::GetInstance()->DownloadCoursewareObjects(m_pGuid,
		0,
		MakeHttpDelegate(this, &CInteractQuestionDownloader::OnDownloadResourceCompleted),
		MakeHttpDelegate(this, &CInteractQuestionDownloader::OnDownloadResourceProgress),
		__super::m_pParamer);

	if (bRet == FALSE)
	{
		THttpNotify notify;
		notify.dwErrorCode	= 10;
		notify.pUserData	= __super::m_pParamer;
		__super::OnDownloadResourceCompleted(&notify);
	}

	return bRet != 0;
}

bool CInteractQuestionDownloader::CancelTransfer()
{
	return NDCloudCoursewareObjectsManager::GetInstance()->CancelDownload(m_pGuid);
}

void CInteractQuestionDownloader::Pause()
{
	if (!m_pGuid)
	{
		return;
	}
	NDCloudCoursewareObjectsManager::GetInstance()->PauseDownload(m_pGuid, &MakeHttpDelegate(this, &CResourceDownloader::OnDownloadResourcePause));
}

void CInteractQuestionDownloader::Resume()
{
	if (!m_pGuid)
	{
		return;
	}
	NDCloudCoursewareObjectsManager::GetInstance()->ResumeDownload(m_pGuid);
	NotifyDownloadInterpose(eInterpose_Resume);
}
void CInteractQuestionDownloader::Cancel()
{
	if(!m_pGuid)
	{
		return;
	}
	NDCloudCoursewareObjectsManager::GetInstance()->CancelDownload(m_pGuid);
	NotifyDownloadInterpose(eInterpose_Cancel);
}

int CInteractQuestionDownloader::GetResourceType()
{
	return CloudFileCoursewareObjects;
}

ITransfer* CInteractQuestionDownloader::Copy()
{
	CInteractQuestionDownloader* pDownloader = new CInteractQuestionDownloader;
	pDownloader->m_pParamer			= __super::m_pCopyParamer->Copy();
	//	pDownloader->m_mapListener		= __super::m_mapListener;

	pDownloader->m_pGuid			= this->m_pGuid;
	pDownloader->m_pTitle			= this->m_pTitle;

	return pDownloader;
}
