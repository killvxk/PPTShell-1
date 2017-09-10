#include "stdafx.h"
#include "DBankAssetsImageItem.h"
#include "NDCloud/NDCloudFile.h"
#include "NDCloud/NDCloudAPI.h"
#include "NDCloud/NDCloudUser.h"

CDBankAssetsImageItem::CDBankAssetsImageItem()
{

}

CDBankAssetsImageItem::~CDBankAssetsImageItem()
{

}

void CDBankAssetsImageItem::DoInit()
{
	m_strName= _T("ͼƬ");

	DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();
	SetJsonUrl(NDCloudComposeUrlPictureInfo(_T(""), "", 0, 500, dwUserId));

	__super::DoInit();
}

bool CDBankAssetsImageItem::OnDownloadDecodeList( void* pObj )
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

	m_pLayout->SetStream(m_pStream,DBankImage);
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

bool CDBankAssetsImageItem::OnChapterChanged( void* pObj )
{
	return true;
}

bool CDBankAssetsImageItem::OnEventLoginComplete( void* pObj )
{
	DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();
	SetJsonUrl(NDCloudComposeUrlPictureInfo(_T(""), "", 0, 500, dwUserId));

	return true;
}

