#include "stdafx.h"
#include "ListDBankCourseItem.h"
#include "NDCloud/NDCloudFile.h"
#include "NDCloud/NDCloudAPI.h"
#include "DUI/GroupExplorer.h"
#include "NDCloud/NDCloudUser.h"
#include "Util/FileTypeFilter.h"

CListDBankCourseItem::CListDBankCourseItem()
{

}

CListDBankCourseItem::~CListDBankCourseItem()
{

}

void CListDBankCourseItem::DoInit()
{
	__super::DoInit();

	m_nType = DBankCourse;

	m_vecSupportType.push_back(LocalFileCourse);
	m_vecSupportType.push_back(FILE_FILTER_PPT);

}

bool CListDBankCourseItem::OnDownloadDecodeList( void* pObj )
{
	CGroupExplorerUI * pGroupExplorer = CGroupExplorerUI::GetInstance();
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
		return false;
	}

	if(IsSelected())
	{
		pGroupExplorer->ShowResource(m_nType, m_pStream);
		pGroupExplorer->StopMask();
	}
	

	return true;
}

bool CListDBankCourseItem::OnEventLoginComplete( void* pObj )
{
	DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();
	SetJsonUrl(NDCloudComposeUrlCourseInfo(_T(""), "", 0, 500, dwUserId));
	SetTotalCountUrl(NDCloudComposeUrlCourseInfo(_T(""), "", 0, 500, dwUserId));

	m_pTextTotalCount->SetVisible(false);
	// 
	m_pTotalLoadGif->SetVisible(true);
	m_pTotalLoadGif->PlayGif();
	GetTotalCountInterface();
	return true;
}
