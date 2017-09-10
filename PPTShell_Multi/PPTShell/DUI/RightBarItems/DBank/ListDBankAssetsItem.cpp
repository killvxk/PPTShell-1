#include "stdafx.h"
#include "ListDBankAssetsItem.h"
#include "NDCloud/NDCloudFile.h"
#include "NDCloud/NDCloudAPI.h"
#include "DUI/GroupExplorer.h"
#include "Util/FileTypeFilter.h"

CListDBankAssetsItem::CListDBankAssetsItem()
{
	m_dwImageDownloadThreadId = -1;
	m_dwFlashDownloadThreadId = -1;
	m_dwVideoDownloadThreadId = -1;
	m_dwVolumeDownloadThreadId = -1;
}

CListDBankAssetsItem::~CListDBankAssetsItem()
{
	if(m_dwImageDownloadThreadId != -1)
	{
		NDCloudDownloadCancel(m_dwImageDownloadThreadId);
	}
	if(m_dwFlashDownloadThreadId != -1)
	{
		NDCloudDownloadCancel(m_dwFlashDownloadThreadId);
	}
	if(m_dwVideoDownloadThreadId != -1)
	{
		NDCloudDownloadCancel(m_dwVideoDownloadThreadId);
	}
	if(m_dwVolumeDownloadThreadId != -1)
	{
		NDCloudDownloadCancel(m_dwVolumeDownloadThreadId);
	}
}

void CListDBankAssetsItem::DoInit()
{
	__super::DoInit();

	m_nType = DBankAssets;
	m_vecSupportType.push_back(DBankVideo);
	m_vecSupportType.push_back(DBankImage);
	m_vecSupportType.push_back(DBankFlash);
	m_vecSupportType.push_back(DBankVolume);
	m_vecSupportType.push_back(LocalFileVideo);
	m_vecSupportType.push_back(LocalFileImage);
	m_vecSupportType.push_back(LocalFileFlash);
	m_vecSupportType.push_back(LocalFileVolume);
	m_vecSupportType.push_back(FILE_FILTER_VIDEO);
	m_vecSupportType.push_back(FILE_FILTER_PIC);
	m_vecSupportType.push_back(FILE_FILTER_FLASH);
	m_vecSupportType.push_back(FILE_FILTER_VOLUME);
}

void CListDBankAssetsItem::DoClick( TNotifyUI* pNotify )
{
	CGroupExplorerUI * pGroupExplorer = CGroupExplorerUI::GetInstance();

	pGroupExplorer->ShowWindow(true);
	pGroupExplorer->ShowDBankAssetsUI();

	GetTotalCountInterface();

	if(m_OnDoClickCallBack)
		m_OnDoClickCallBack(pNotify);
}

bool CListDBankAssetsItem::OnEventLoginComplete( void* pObj )
{

	m_pTextTotalCount->SetVisible(false);
	// 
	m_pTotalLoadGif->SetVisible(true);
	m_pTotalLoadGif->PlayGif();

	GetTotalCountInterface();
	return true;
}

bool CListDBankAssetsItem::OnDownloadDecodeList( void* pObj )
{
	return true;
}

void CListDBankAssetsItem::GetTotalCountInterface()
{
	DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();
	tstring strImageUrl = NDCloudComposeUrlPictureInfo(_T(""), _T(""), 0, 500, dwUserId);
	tstring strVolumeUrl = NDCloudComposeUrlVolumeInfo(_T(""), _T(""), 0, 500, dwUserId);
	tstring strVideoUrl = NDCloudComposeUrlVideoInfo(_T(""), _T(""), 0, 500, dwUserId);
	tstring strFlashUrl = NDCloudComposeUrlFlashInfo(_T(""), _T(""), 0, 500, dwUserId);

	if(m_dwImageDownloadThreadId != -1)
	{
		NDCloudDownloadCancel(m_dwImageDownloadThreadId);
	}
	if(m_dwFlashDownloadThreadId != -1)
	{
		NDCloudDownloadCancel(m_dwFlashDownloadThreadId);
	}
	if(m_dwVideoDownloadThreadId != -1)
	{
		NDCloudDownloadCancel(m_dwVideoDownloadThreadId);
	}
	if(m_dwVolumeDownloadThreadId != -1)
	{
		NDCloudDownloadCancel(m_dwVolumeDownloadThreadId);
	}

	m_dwThreadFinishCount	= 0;
	m_dwRequestSuccess		= 0;
	m_nGetCurAssetsCount	= 0;
	m_dwImageDownloadThreadId = NDCloudDownload(strImageUrl.c_str(), MakeHttpDelegate(this, &CListDBankAssetsItem::OnGetAssetsCurCount));
	m_dwFlashDownloadThreadId = NDCloudDownload(strFlashUrl.c_str(), MakeHttpDelegate(this, &CListDBankAssetsItem::OnGetAssetsCurCount));
	m_dwVideoDownloadThreadId = NDCloudDownload(strVideoUrl.c_str(), MakeHttpDelegate(this, &CListDBankAssetsItem::OnGetAssetsCurCount));
	m_dwVolumeDownloadThreadId = NDCloudDownload(strVolumeUrl.c_str(), MakeHttpDelegate(this, &CListDBankAssetsItem::OnGetAssetsCurCount));
}

bool CListDBankAssetsItem::OnGetAssetsCurCount( void * pParam )
{
	m_dwThreadFinishCount++;

	THttpNotify * pNotify = (THttpNotify*)pParam;

	if(m_pTotalLoadGif && m_dwThreadFinishCount == 4)
	{
		m_pTotalLoadGif->StopGif();
		m_pTotalLoadGif->SetVisible(false);
	}

	if(pNotify->dwErrorCode != 0)
	{
		m_bTotalNetLess = true;
		return true;
	}
	m_dwRequestSuccess++;

	int nCount = NDCloudDecodeCount(pNotify->pData, pNotify->nDataSize);

	m_nGetCurAssetsCount += nCount;
	if(m_pTextTotalCount && m_dwThreadFinishCount == 4)
	{
		char szBuff[128] ={0};
		sprintf(szBuff,_T("%d"), m_nGetCurAssetsCount);
		CDuiRect rcCalc;
		rcCalc.right = 50;
		CalcText(GetManager()->GetPaintDC(), rcCalc, szBuff, m_pTextTotalCount->GetFont(), DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT|DT_NOPREFIX, UIFONT_GDI);
		int nCurLen = rcCalc.GetWidth();

		m_pTextTotalCount->SetFixedWidth(nCurLen);
		m_pTextTotalCount->SetVisible(true);
		m_pTextTotalCount->SetText(szBuff);

		if(m_dwRequestSuccess == 4)
			m_bTotalNetLess = false;
	}

	return true;
}
