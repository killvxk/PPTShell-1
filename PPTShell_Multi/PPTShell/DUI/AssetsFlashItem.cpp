#include "stdafx.h"
#include "AssetsFlashItem.h"
#include "NDCloud/NDCloudFile.h"
#include "NDCloud/NDCloudAPI.h"

CAssetsFlashItem::CAssetsFlashItem()
{

}

CAssetsFlashItem::~CAssetsFlashItem()
{

}

void CAssetsFlashItem::DoInit()
{
	m_strName= _T("¶¯»­");

	tstring strGuid = NDCloudGetChapterGUID();
	SetJsonUrl(NDCloudComposeUrlFlashInfo(strGuid.c_str(), "", 0, 500));

	__super::DoInit();
}

bool CAssetsFlashItem::OnDownloadDecodeList( void* pObj )
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
	
	m_pLayout->SetStream(m_pStream,CloudFileFlash);

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

bool CAssetsFlashItem::OnChapterChanged( void* pObj )
{
	TEventNotify* pEventNotify = (TEventNotify*)pObj;
	CStream* pStream = (CStream*)pEventNotify->wParam;
	pStream->ResetCursor();
	tstring strGuid = pStream->ReadString();

	SetJsonUrl(NDCloudComposeUrlFlashInfo(strGuid.c_str(), "", 0, 500));

	__super::OnChapterChanged(pObj);

	return true;
}

