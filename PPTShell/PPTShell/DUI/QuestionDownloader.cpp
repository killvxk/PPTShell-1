#include "StdAfx.h"
#include "DUI/ITransfer.h"
#include "DUI/IVisitor.h"
#include "DUI/BaseParamer.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IDownloadListener.h"
#include "DUI/ResourceDownloader.h"
#include "DUI/QuestionDownloader.h"

#include "NDCloud/NDCloudAPI.h"
#include "NDCloud/NDCloudQuestion.h"

CQuestionDownloader::CQuestionDownloader()
{
	m_pGuid				=  NULL;
	m_pTitle			=  NULL;
}


CQuestionDownloader::~CQuestionDownloader()
{

}

bool CQuestionDownloader::Transfer()
{
	BOOL bRet = NDCloudQuestionManager::GetInstance()->DownloadQuestion(m_pGuid,
		0,
		MakeHttpDelegate(this, &CQuestionDownloader::OnDownloadResourceCompleted),
		MakeHttpDelegate(this, &CQuestionDownloader::OnDownloadResourceProgress),
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

void CQuestionDownloader::SetQuestionGuid( LPCTSTR lptcstr )
{
	m_pGuid = lptcstr;
}

void CQuestionDownloader::SetQuestionTitle( LPCTSTR lptcstr )
{
	m_pTitle = lptcstr;
}	

void CQuestionDownloader::Cancel()
{
	if (!m_pGuid)
	{
		return;
	}

	NDCloudQuestionManager::GetInstance()->CancelDownload(m_pGuid);
	NotifyDownloadInterpose(eInterpose_Cancel);
}


void CQuestionDownloader::Pause()
{
	if (!m_pGuid)
	{
		return;
	}
	NDCloudQuestionManager::GetInstance()->PauseDownload(m_pGuid, &MakeHttpDelegate(this, &CResourceDownloader::OnDownloadResourcePause));
}

void CQuestionDownloader::Resume()
{
	if (!m_pGuid)
	{
		return;
	}
	NDCloudQuestionManager::GetInstance()->ResumeDownload(m_pGuid);
	NotifyDownloadInterpose(eInterpose_Resume);
}

int CQuestionDownloader::GetResourceType()
{
	return CloudFileQuestion;
}

bool CQuestionDownloader::CancelTransfer()
{
	return NDCloudQuestionManager::GetInstance()->CancelDownload(m_pGuid);
}

LPCTSTR CQuestionDownloader::GetResourceTitle()
{
	return m_pTitle;
}

ITransfer* CQuestionDownloader::Copy()
{
	CQuestionDownloader* pDownloader= new CQuestionDownloader;
	pDownloader->m_pParamer			= __super::m_pCopyParamer->Copy();
	//	pDownloader->m_mapListener		= __super::m_mapListener;

	pDownloader->m_pGuid			= this->m_pGuid;
	pDownloader->m_pTitle			= this->m_pTitle;

	return pDownloader;
}

LPCTSTR CQuestionDownloader::GetResourceGuid()
{
	return m_pGuid;
}

