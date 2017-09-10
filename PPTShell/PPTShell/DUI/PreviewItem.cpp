#include "StdAfx.h"
#include "PreviewItem.h"
#include "NDCloud/NDCloudAPI.h"
#include "PreviewDialogUI.h"

CPreviewItemUI::CPreviewItemUI()
{
	m_dwDownloadId		= 0;
	m_bLocal			= false;
}

CPreviewItemUI::~CPreviewItemUI()
{
	if( m_dwDownloadId != 0 )
		NDCloudDownloadCancel(m_dwDownloadId);
}


void CPreviewItemUI::Init()
{
	__super::Init();
	CControlUI* pCtrl = FindSubControl(_T("btn2"));
	if (pCtrl)
	{
		pCtrl->SetVisible(false);
	}

	pCtrl = FindSubControl(_T("toolbar"));
	if (pCtrl)
	{
		pCtrl->SetVisible(false);
	}

	m_lbName->SetVisible(false);
	EnableToolbar(false);

	SetFixedWidth(128);
	SetContentHeight(72);
}

void CPreviewItemUI::OnButtonClick( int nButtonIndex, TNotifyUI& msg )
{

}

void CPreviewItemUI::OnItemClick( TNotifyUI& msg )
{
	m_bItemClick = true;

	if( m_bLocal )
	{
		THttpNotify notify;
		notify.strFilePath = GetUrl();

		OnDownloadThumbnailCompleted(&notify);
	}
	else
	{
		m_dwDownloadId = NDCloudDownloadCourseFile(GetResource(), m_strPPTGuid,  _T(""), CourseFileThumb, MakeHttpDelegate(this, &CPreviewItemUI::OnDownloadThumbnailCompleted));
		if (m_dwDownloadId == 0)
		{
			this->StopMask();
			return;
		}
	}
	
}

bool CPreviewItemUI::OnDownloadThumbnailCompleted( void* pNotify )
{
	THttpNotify* pHttpNotify = (THttpNotify*)pNotify;
	SetImage(pHttpNotify->strFilePath.c_str());

	this->StopMask();
	m_layToolBar->SetVisible(false);

	CPreviewDlgUI* pDlg = (CPreviewDlgUI*)m_pPreviewDlg;
	if( pDlg == NULL )
		return false;

	if( m_bItemClick )
	{
		pDlg->SetCurItemIndex(m_nIndex);
		pDlg->OnPictureDownloaded(pHttpNotify);

		m_bItemClick = false;
	}
	

	return true;

}

void CPreviewItemUI::SetResourceUrl( LPCTSTR lpctsTitle, LPCTSTR lptcsResourceUrl, tstring strGUID)
{
	m_bItemClick = false;
	m_strPPTGuid = strGUID;
	SetResource(lptcsResourceUrl);
	
}

void CPreviewItemUI::SetPreviewDlgPtr(void* pDlg)
{
	m_pPreviewDlg = pDlg;
}

void CPreviewItemUI::SetIndex(int nIndex)
{
	m_nIndex = nIndex;
}

tstring CPreviewItemUI::GetUrl()
{
	return GetResource();
}

void CPreviewItemUI::SetLocal(bool bLocal)
{
	m_bLocal = bLocal;
}

bool CPreviewItemUI::IsLocal()
{
	return m_bLocal;
}

void CPreviewItemUI::DownloadThumbnail()
{
	if( m_bLocal )
	{
		THttpNotify notify;
		notify.strFilePath = GetUrl();

		OnDownloadThumbnailCompleted(&notify);
	}
	else
	{
		this->StartMask();
		m_dwDownloadId = NDCloudDownloadCourseFile(GetResource(), m_strPPTGuid, _T(""), CourseFileThumb, MakeHttpDelegate(this, &CPreviewItemUI::OnDownloadThumbnailCompleted));
		if (m_dwDownloadId == 0)
		{
			this->StopMask();
			return;
		}

	}
	
}

void CPreviewItemUI::ReadStream( CStream* pStream )
{
	if (m_bLocal)
	{
		__super::ReadStream(pStream);
	}
	else
	{
		SetResource(pStream->ReadString().c_str());
		SetGroup(_T("PreviewItem"));
	}
	
}

bool CPreviewItemUI::OnItemEvent( void* param )
{
	return true;
}
