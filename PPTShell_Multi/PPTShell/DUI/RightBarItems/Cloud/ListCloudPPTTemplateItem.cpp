#include "stdafx.h"
#include "ListCloudPPTTemplateItem.h"
#include "NDCloud/NDCloudFile.h"
#include "NDCloud/NDCloudAPI.h"
#include "DUI/GroupExplorer.h"

CListCloudPPTTemplateItem::CListCloudPPTTemplateItem()
{

}

CListCloudPPTTemplateItem::~CListCloudPPTTemplateItem()
{

}

void CListCloudPPTTemplateItem::DoInit()
{
	__super::DoInit();

	m_nType = CloudFilePPTTemplate;
	SetTotalCountUrl(NDCloudComposeUrlPPTTemplateCount());
	GetTotalCountInterface();
}

bool CListCloudPPTTemplateItem::OnChapterChanged( void* pObj )
{
	TEventNotify* pEventNotify = (TEventNotify*)pObj;
	CStream* pStream = (CStream*)pEventNotify->wParam;
	pStream->ResetCursor();
	tstring strGuid = pStream->ReadString();

	if(m_dwCurCountDownId != -1)
	{
		NDCloudDownloadCancel(m_dwCurCountDownId);
	}

	CCategoryTree* pCategoryTree = NULL;
	NDCloudGetCategoryTree(pCategoryTree);
	if(pCategoryTree)
	{
		SetJsonUrl(NDCloudComposeUrlPPTTemplateInfo(pCategoryTree->GetSelectedSectionCode(), pCategoryTree->GetSelectedCourseCode(), _T(""), 0, 100));

		tstring strCourseCode = pCategoryTree->GetSelectedCourseCode();
		if(m_mapPPTTemplateStream.find(strCourseCode) == m_mapPPTTemplateStream.end())
		{
			SetCurCountUrl(NDCloudComposeUrlPPTTemplateInfo(pCategoryTree->GetSelectedSectionCode(), pCategoryTree->GetSelectedCourseCode() ,_T(""), 0, 1));
			__super::OnChapterChanged(pObj);
		}
		else
		{
			CStream* pStream = m_mapPPTTemplateStream[strCourseCode];
			pStream->ResetCursor();
			int nCount = pStream->ReadDWORD();

			char szBuff[128] ={0};
			sprintf(szBuff,_T("%d"), nCount);
			int nCurLen = _tcslen(szBuff);

			m_pTextCurCount->SetFixedWidth(nCurLen * 7);
			m_pTextCurCount->SetVisible(true);
			m_pTextCurCount->SetText(szBuff);

			m_bCurNetLess = false;

			m_dwCurCountDownId = -1;
		}
	}

	if(m_pStream)
	{
		delete m_pStream;
		m_pStream = NULL; 
	}

	if(IsSelected())
	{
		if(m_dwDownloadId != -1)
		{
			NDCloudDownloadCancel(m_dwDownloadId);
		}

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

	return true;
}

bool CListCloudPPTTemplateItem::OnDownloadDecodeList( void* pObj )
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

	if (!NDCloudDecodePPTTemplateList(pHttpNotify->pData, pHttpNotify->nDataSize, m_pStream))
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

