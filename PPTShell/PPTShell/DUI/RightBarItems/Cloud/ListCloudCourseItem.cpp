#include "stdafx.h"
#include "ListCloudCourseItem.h"
#include "NDCloud/NDCloudFile.h"
#include "NDCloud/NDCloudAPI.h"
#include "DUI/GroupExplorer.h"

CListCloudCourseItem::CListCloudCourseItem()
{

}

CListCloudCourseItem::~CListCloudCourseItem()
{

}

void CListCloudCourseItem::DoInit()
{
	__super::DoInit();

	m_nType = CloudFileCourse;
	SetTotalCountUrl(NDCloudComposeUrlCourseCount());
	GetTotalCountInterface();
}

bool CListCloudCourseItem::OnChapterChanged( void* pObj )
{
	TEventNotify* pEventNotify = (TEventNotify*)pObj;
	CStream* pStream = (CStream*)pEventNotify->wParam;
	pStream->ResetCursor();
	tstring strGuid = pStream->ReadString();

	SetCurCountUrl(NDCloudComposeUrlCourseInfo(strGuid ,_T(""), 0, 1));
	SetJsonUrl(NDCloudComposeUrlCourseInfo(strGuid.c_str(), _T(""), 0, 500));

	if(m_pStream)
	{
		delete m_pStream;
		m_pStream = NULL; 
	}

//	if(IsSelected())
	if(false)
	{
// 		if(m_dwDownloadId != -1)
// 		{
// 			NDCloudDownloadCancel(m_dwDownloadId);
// 		}

		CGroupExplorerUI * pGroupExplorer = CGroupExplorerUI::GetInstance();
		pGroupExplorer->ShowWindow(true);

		pGroupExplorer->StartMask();
		m_dwDownloadId = NDCloudDownload(GetJsonUrl(), MakeHttpDelegate(this, &CListCloudItem::OnDownloadDecodeList));
	}
// 	else
// 	{
// 		if(m_pStream == NULL)
// 			m_dwDownloadId = NDCloudDownload(GetJsonUrl(), MakeHttpDelegate(this, &ClistCloudItem::OnDownloadDecodeList));
// 	}

	if(m_dwCurCountDownId != -1)
	{
		NDCloudDownloadCancel(m_dwCurCountDownId);
	}

	

	__super::OnChapterChanged(pObj);

	return true;
}

bool CListCloudCourseItem::OnDownloadDecodeList( void* pObj )
{
// 	if(m_pStream)
// 	{
// 		return false;
// 	}
	CGroupExplorerUI * pGroupExplorer = CGroupExplorerUI::GetInstance();
	
	if(IsSelected())
	{
		pGroupExplorer->SetTitleText(m_strName.c_str());
	}

	THttpNotify* pHttpNotify = (THttpNotify*)pObj;

	if (pHttpNotify->dwErrorCode > 0)
	{
		pGroupExplorer->StopMask();
		pGroupExplorer->ShowNetlessUI(true);
		return true;
	}
	else
	{
		pGroupExplorer->ShowNetlessUI(false);
	}

	if(m_pStream)
		delete m_pStream;
	m_pStream = new CStream(1024);

	if (!NDCloudDecodeCourseList(pHttpNotify->pData, pHttpNotify->nDataSize, m_pStream))
	{
		pGroupExplorer->ShowNetlessUI(true);
		pGroupExplorer->StopMask();
		delete m_pStream;
		m_pStream = NULL;
		return false;
	}

	if(IsSelected())
	{
		pGroupExplorer->ShowResource(m_nType, m_pStream);
		pGroupExplorer->StopMask();
	}
	

	return true;
	
}
