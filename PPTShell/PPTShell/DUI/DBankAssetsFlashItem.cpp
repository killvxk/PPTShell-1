#include "stdafx.h"
#include "DBankAssetsFlashItem.h"
#include "NDCloud/NDCloudFile.h"
#include "NDCloud/NDCloudAPI.h"
#include "NDCloud/NDCloudUser.h"

CDBankAssetsFlashItem::CDBankAssetsFlashItem()
{

}

CDBankAssetsFlashItem::~CDBankAssetsFlashItem()
{

}

void CDBankAssetsFlashItem::DoInit()
{
	m_strName= _T("¶¯»­");

	DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();
	SetJsonUrl(NDCloudComposeUrlFlashInfo(_T(""), _T(""), 0, 500, dwUserId));

	__super::DoInit();
}

bool CDBankAssetsFlashItem::OnDownloadDecodeList( void* pObj )
{
	int nCount = 0;
	THttpNotify* pHttpNotify = (THttpNotify*)pObj;

	if (pHttpNotify->dwErrorCode > 0)
	{
		m_bCurNetLess = true;
		goto _Out;
	}

	if(m_pStream)
		delete m_pStream;
	m_pStream = new CStream(1024);

	if (!NDCloudDecodeFlashList(pHttpNotify->pData, pHttpNotify->nDataSize, m_pStream))
	{
		m_bCurNetLess = true;
		delete m_pStream;
		m_pStream = NULL;
		goto _Out;
	}

	m_bCurNetLess = false;
	m_pStream->ResetCursor();
	nCount = m_pStream->ReadInt();

	m_pLayout->SetStream(m_pStream,DBankFlash);

_Out:
	SetCountText(nCount);

	m_pGif->StopGif();
	m_pGif->SetVisible(false);
	GetLayout()->StopLoading();

	if(m_bCurNetLess)
		GetLayout()->ShowNetLess();

	if(m_OnFinishCallBack)
		m_OnFinishCallBack(NULL);
	return true;
}

bool CDBankAssetsFlashItem::OnChapterChanged( void* pObj )
{
	return true;
}

bool CDBankAssetsFlashItem::OnEventLoginComplete( void* pObj )
{
	DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();
	SetJsonUrl(NDCloudComposeUrlFlashInfo(_T(""), _T(""), 0, 500, dwUserId));

	return true;
}

