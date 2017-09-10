#include "StdAfx.h"
#include "DUI/StyleItem.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IDownloadListener.h"
#include "DUI/DownloadItem.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IDownloadListener.h"

CDownloadItemUI::CDownloadItemUI( IComponent* pComponent )
: CStyleItemUI(pComponent)
{
	m_btnStart				= NULL;
	m_btnPause				= NULL;
	m_btnClose				= NULL;
	m_btnRetry				= NULL;
	m_lbName				= NULL;
	m_ctrlIcon				= NULL;
	m_pLayProgressNormal	= NULL;
	m_pLayProgressError		= NULL;

	m_bAsycRelease			= false;
	m_bPaused				= false;
	m_bDownloadError		= false;
}


CDownloadItemUI::~CDownloadItemUI()
{
	if (GetHandler())
	{
		GetHandler()->DoDestroy(this);
		SetHandler(NULL);
	}

	OutputDebugString(_T("~CDownloadItemUI.\n"));
}

void CDownloadItemUI::DoInit()
{
	__super::DoInit();

	m_btnStart				= static_cast<CButtonUI*>(this->FindSubControl(_T("dm_item_download_start")));
	m_btnPause				= static_cast<CButtonUI*>(this->FindSubControl(_T("dm_item_download_pause")));
	m_btnClose				= static_cast<CButtonUI*>(this->FindSubControl(_T("dm_item_download_close")));
	m_btnRetry				= static_cast<CButtonUI*>(this->FindSubControl(_T("dm_item_download_retry")));
	m_lbName				= static_cast<CLabelUI*>(this->FindSubControl(_T("name")));
	m_ctrlIcon				= static_cast<CControlUI*>(this->FindSubControl(_T("image")));
	m_pLayProgressNormal	= static_cast<CContainerUI*>(this->FindSubControl(_T("pro_normal_layout")));
	m_pLayProgressError		= static_cast<CContainerUI*>(this->FindSubControl(_T("pro_error_layout")));
}

bool CDownloadItemUI::OnControlNotify( void* pObj )
{
	TNotifyUI* pNotify = (TNotifyUI*)pObj;
	if (pNotify->sType == DUI_MSGTYPE_CLICK
		&& pNotify->pSender)
	{
		TNotifyUI	notify;
		notify.wParam		= (WPARAM)pNotify->pSender->GetTag();
		notify.pSender		= this;
		if (GetHandler())
		{
			GetHandler()->SetTrigger(this);
			GetHandler()->DoButtonClick(&notify);
		}
	}

	return true;
}

bool CDownloadItemUI::OnControlEvent( void* pObj )
{
	return true;
}

void CDownloadItemUI::SetTitle( LPCTSTR lptcsName )
{
	if (m_lbName && lptcsName)
	{
		m_lbName->SetText(lptcsName);
		m_lbName->SetToolTip(lptcsName);
	}
}

void CDownloadItemUI::SetIcon( LPCTSTR lptcsIcon )
{
	if (m_ctrlIcon && lptcsIcon)
	{
		m_ctrlIcon->SetBkImage(lptcsIcon);
	}
}

void CDownloadItemUI::OnDownloadBefore( THttpNotify* pHttpNotify )
{
	
}

void CDownloadItemUI::OnDownloadProgress( THttpNotify* pHttpNotify )
{
	m_bPaused			= false;
	m_bDownloadError	= false;
	//btn state
	m_btnPause->SetVisible(true);
	m_btnStart->SetVisible(false);
	m_btnRetry->SetVisible(false);

	int nPos = (int)(pHttpNotify->fPercent * 100);
	//progress
	ShowNormalProgressLayout(true);
	ShowErrorProgressLayout(false);

	//queue ing
	if (pHttpNotify->dwErrorCode == -1)
	{
		nPos = -1;
	}
	UpdateProgressLayout(nPos);

}

void CDownloadItemUI::OnDownloadCompleted( THttpNotify* pHttpNotify )
{
	if (pHttpNotify->dwErrorCode != 0)
	{
		ShowErrorProgressLayout(true);
		ShowNormalProgressLayout(false);

		m_btnPause->SetVisible(false);
		m_btnStart->SetVisible(false);
		m_btnRetry->SetVisible(true);

		m_bPaused			= true;
		m_bDownloadError	= true;
		return;
	}

	if (GetManager())
	{
		m_bAsycRelease = true;
		GetManager()->SendNotify(this->GetParent(), _T("itemremove"), (WPARAM)this, 0, true);
	}
}

void CDownloadItemUI::OnDownloadInterpose( THttpNotify* pHttpNotify )
{
	if (pHttpNotify->dwErrorCode == eInterpose_Pause)
	{
		m_btnPause->SetVisible(false);
		m_btnStart->SetVisible(true);
		m_btnRetry->SetVisible(false);
		m_bPaused = true;
	}
	else if (pHttpNotify->dwErrorCode == eInterpose_Resume)
	{
		m_btnPause->SetVisible(true);
		m_btnStart->SetVisible(false);
		m_btnRetry->SetVisible(false);
		m_bPaused = false;
	}
	else if (pHttpNotify->dwErrorCode == eInterpose_Cancel)
	{
		if (GetManager())
		{
			m_bAsycRelease = true;
			GetManager()->SendNotify(this->GetParent(), _T("itemremove"), (WPARAM)this, 0, true);
		}
	}
}

void CDownloadItemUI::ShowNormalProgressLayout( bool bShow )
{
	m_pLayProgressNormal->SetVisible(bShow);
}

void CDownloadItemUI::ShowErrorProgressLayout( bool bShow )
{
	m_pLayProgressError->SetVisible(bShow);
}

void CDownloadItemUI::UpdateProgressLayout( int nProgress )
{
	UpdateProgressLayout(m_pLayProgressError, nProgress);
	UpdateProgressLayout(m_pLayProgressNormal, nProgress);
}

void CDownloadItemUI::UpdateProgressLayout( CContainerUI* pLayout, int nProgress )
{
	CProgressUI*	pProgress	= (CProgressUI*)pLayout->FindSubControl(_T("progress"));
	CLabelUI*		pProressNum	= (CProgressUI*)pLayout->FindSubControl(_T("pro_num"));
	if (pProressNum)
	{
		if (nProgress >= 0)
		{
			TCHAR szNum[32] = {0};
			_stprintf_s(szNum, _T("%d%%"), nProgress);
			pProressNum->SetText(szNum);
		}
		else
		{
			pProressNum->SetText(_T("¶ÓÁÐÖÐ"));
		}
	}

	if (pProgress)
	{
		pProgress->SetValue(nProgress);
	}
}

void CDownloadItemUI::Cancel()
{
	if (GetManager())
	{
		GetManager()->SendNotify(m_btnClose, DUI_MSGTYPE_CLICK, 0, 0, true);
	}
}

void CDownloadItemUI::Pause()
{
	if (GetManager())
	{
		GetManager()->SendNotify(m_btnPause, DUI_MSGTYPE_CLICK);
	}
}

void CDownloadItemUI::Retry()
{
	if (GetManager())
	{
		GetManager()->SendNotify(m_btnRetry, DUI_MSGTYPE_CLICK);
	}

	m_bDownloadError = false;
}

void CDownloadItemUI::Resume()
{
	if (m_bDownloadError)
	{
		Retry();
	}
	else
	{
		if (GetManager())
		{
			GetManager()->SendNotify(m_btnStart, DUI_MSGTYPE_CLICK);
		}
	}
	
}

bool CDownloadItemUI::IsAsycRelease()
{
	return m_bAsycRelease;
}	

bool CDownloadItemUI::IsPaused()
{
	return m_bPaused;
}

bool CDownloadItemUI::IsErrorOccurred()
{
	return m_bDownloadError;
}

void CDownloadItemUI::SetGuid( LPCTSTR lptcsIcon )
{
	if (!lptcsIcon)
	{
		return;
	}
	SetUserData(lptcsIcon);
}

LPCTSTR CDownloadItemUI::GetGuid()
{
	return GetUserData();
}
