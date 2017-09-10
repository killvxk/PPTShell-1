#include "stdafx.h"
#include "AssetsImageItem.h"
#include "NDCloud/NDCloudFile.h"
#include "NDCloud/NDCloudAPI.h"

CAssetsImageItem::CAssetsImageItem()
{

}

CAssetsImageItem::~CAssetsImageItem()
{

}

void CAssetsImageItem::DoInit()
{
	m_strName= _T("ͼƬ");

	tstring strGuid = NDCloudGetChapterGUID();
	SetJsonUrl(NDCloudComposeUrlPictureInfo(strGuid.c_str(), _T(""), 0, 500));

	__super::DoInit();
}

bool CAssetsImageItem::OnDownloadDecodeList( void* pObj )
{
	if(m_pStream)
		return false;
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

	if (!NDCloudDecodeImageList(pHttpNotify->pData, pHttpNotify->nDataSize, m_pStream))
	{
		m_bCurNetLess = true;
		delete m_pStream;
		m_pStream = NULL;
		goto _Out;
	}

	m_bCurNetLess = false;
	m_pStream->ResetCursor();
	nCount = m_pStream->ReadInt();

	m_pLayout->SetStream(m_pStream,CloudFileImage);
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

bool CAssetsImageItem::OnChapterChanged( void* pObj )
{
	TEventNotify* pEventNotify = (TEventNotify*)pObj;
	CStream* pStream = (CStream*)pEventNotify->wParam;
	pStream->ResetCursor();
	tstring strGuid = pStream->ReadString();

	SetJsonUrl(NDCloudComposeUrlPictureInfo(strGuid.c_str(), _T(""), 0, 500));

	__super::OnChapterChanged(pObj);

	return true;
}

