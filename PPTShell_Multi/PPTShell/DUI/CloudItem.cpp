#include "StdAfx.h"
#include "CloudItem.h"
#include "NDCloud/NDCloudAPI.h"
#include "Util/Tween.h"
#include "Util/Util.h"
#include "DragDialogUI.h"
#include "GUI/MainFrm.h"
#include "NDCloud/NDCloudUser.h"
#include "NDCloud/NDCloudContentService.h"
#include "Effect/Utility.h"


#define EASE_IN		100
#define EASE_OUT	 200
#define TIMER_FRAME	 10

CCloudItemUI::CCloudItemUI()
{
	m_proDownload			= NULL;
	m_dwResourceDownloadId	= 0;
	m_dwSlideId				= 0;
	m_dwPlaceHolderId		= 0;	
	m_eContentType = CONTENT_ASSETS;
}


CCloudItemUI::~CCloudItemUI()
{
	if (m_dwResourceDownloadId)
	{
		NDCloudDownloadCancel(m_dwResourceDownloadId);
	}
}

void CCloudItemUI::Init()
{
	__super::Init();
	//SetFixedWidth(170);
	m_proDownload= dynamic_cast<CProgressUI*>( FindSubControl(_T("progress")));
}

void CCloudItemUI::ReadStream(CStream* pStream)
{
	m_strGuid = pStream->ReadString();
	
	SetTitle(pStream->ReadString().c_str());
	SetResource(pStream->ReadString().c_str());
	SetGroup(_T("123"));

	// json information which used to modify title
	m_strJsonInfo = pStream->ReadString();

}

void CCloudItemUI::ShowProgress( bool bVisible )
{
	m_proDownload->SetVisible(bVisible);
	for (int i = 0; i < m_layToolBar->GetCount(); ++i)
	{
		CButtonUI* pBtn  = dynamic_cast<CButtonUI* >(m_layToolBar->GetItemAt(i));
		if (pBtn)
		{
			pBtn->SetEnabled(!bVisible);
		}
	}
	

}

void CCloudItemUI::SetProgress( int nPos )
{
	m_proDownload->SetValue(nPos);
}


void CCloudItemUI::OnDownloadResourceBefore()
{
	CMainFrame* pMainFrame	= (CMainFrame*)AfxGetMainWnd();
	m_dwLastOperationerId	= pMainFrame->GetOperationerId();


}

void CCloudItemUI::DownloadResource( int nButtonIndex, int nType, int nThumbnailSize)
{
	OnDownloadResourceBefore();

	m_nButtonIndex = nButtonIndex;
	this->ShowProgress(true);
	m_dwResourceDownloadId = NDCloudDownloadFile(GetResource(), _T(""), GetTitle(), nType, nThumbnailSize, MakeHttpDelegate(this, &CCloudItemUI::OnDownloadResourceCompleted), MakeHttpDelegate(this, &CCloudItemUI::OnDownloadResourceProgress));
	if (m_dwResourceDownloadId == 0)
	{
		this->ShowProgress(false);
		return;
	}

	
}

bool CCloudItemUI::OnDownloadResourceCompleted( void* pNotify )
{
	THttpNotify* pHttpNotify = (THttpNotify*)pNotify;
	this->SetProgress(m_proDownload->GetMaxValue());
	this->ShowProgress(false);
	this->SetProgress(0);

	CControlUI* pCtrl = FindSubControl(_T("empty"));
	pCtrl->SetToolTip(_T(""));

	//if operationer changed , cancel invoke and update operationer.
	if (m_nButtonIndex == 0)
	{
		CMainFrame* pMainFrame	= (CMainFrame*)AfxGetMainWnd();
		if (pMainFrame->IsOperationerChanged(m_dwLastOperationerId))
		{
			m_dwLastOperationerId = pMainFrame->GetOperationerId();
			return true;
		}
	}


	if(pHttpNotify->strFilePath != _T(""))
	{
		OnDownloadResourceCompleted(m_nButtonIndex, pHttpNotify->strFilePath.c_str());
	}
	else
	{
		CToast::Toast(_STR_PREVIEW_DIALOG_RESOURCE_NOT_FOUND);
	}
	return true;
}

bool CCloudItemUI::OnDownloadResourceProgress( void* pNotify )
{
	THttpNotify* pHttpNotify = (THttpNotify*)pNotify;
	int nPos = (int)(pHttpNotify->fPercent * m_proDownload->GetMaxValue());
	this->SetProgress(nPos);

	CControlUI* pCtrl	= FindSubControl(_T("empty"));
	TCHAR szTip[128]	= {0};
	TCHAR szSpeed[16]	= {0};
	TCHAR szRemain[32]	= {0};
	TCHAR szElapse[32]	= {0};
	TCHAR szSize[32]	= {0};
	int		nTemp		= 0;

	_stprintf_s(szSpeed, _T("%.1f %s"), 
		pHttpNotify->nSpeed < 1000 ? pHttpNotify->nSpeed : (pHttpNotify->nSpeed  * 1.0f / 1024),
		pHttpNotify->nSpeed < 1000 ? _T("KB/S") : _T("MB/S"));

	nTemp = pHttpNotify->nElapseTime;
	_stprintf_s(szElapse, _T("%02d:%02d:%02d"), 
		pHttpNotify->nElapseTime / 3600,
		(nTemp %= 3600, nTemp / 60),
		pHttpNotify->nElapseTime % 60);

	nTemp = pHttpNotify->nRemainTime;
	_stprintf_s(szRemain, _T("%02d:%02d:%02d"), 
		pHttpNotify->nRemainTime / 3600,
		(nTemp %= 3600, nTemp / 60),
		pHttpNotify->nRemainTime % 60);


	if (pHttpNotify->nTotalSize < 1000 )
	{
		_stprintf_s(szSize, _T("%.2f B"), 
			(pHttpNotify->nTotalSize  * 1.0f));
	}
	else if (pHttpNotify->nTotalSize < 1000 * 1000 )
	{
		_stprintf_s(szSize, _T("%.2f KB"), 
			(pHttpNotify->nTotalSize  * 1.0f / 1024));
	}
	else if (pHttpNotify->nTotalSize < 1000 * 1000 * 1000 )
	{
		_stprintf_s(szSize, _T("%.2f MB"), 
			(pHttpNotify->nTotalSize  * 1.0f / (1024 * 1024)));
	}
	

	_stprintf_s(szTip, _T("下载速度：%s<n>下载用时：%s<n>剩余时间：%s<n>文件大小：%s<n>"), 
		szSpeed,
		szElapse,
		szRemain,
		szSize);
	
	pCtrl->SetToolTip(szTip);


	return true;
}

void CCloudItemUI::OnDownloadResourceCompleted( int nButtonIndex, LPCTSTR lptcsPath )
{
	CControlUI* pCtrl	= FindSubControl(_T("empty"));
	if(pCtrl)
		pCtrl->SetToolTip("");
}

DWORD CCloudItemUI::GetSlideId()
{
	return m_dwSlideId;
}

DWORD CCloudItemUI::GetPlaceHolderId()
{
	return m_dwPlaceHolderId;
}

void CCloudItemUI::SetSlideId( DWORD dwId )
{
	m_dwSlideId = dwId;
}

void CCloudItemUI::SetPlaceHolderId( DWORD dwId )
{
	m_dwPlaceHolderId = dwId;
}

void CCloudItemUI::DownloadLocal()
{

}

void CCloudItemUI::Delete(int currentModeType)
{
	DWORD dwUserID = NDCloudUser::GetInstance()->GetUserId();
	if(dwUserID!=0)
	{
		CStream* pStream = new CStream(1024);
		pStream->WriteInt(currentModeType);
		pStream->WriteString(GetResource());
		pStream->WriteString(GetTitle());
		if(!NDCloudContentServiceManager::GetInstance()->DeleteCloudFile(dwUserID,m_strGuid,m_eContentType,GetResource(),MakeHttpDelegate(this, &CCloudItemUI::OnDeleteResourceCompelete),pStream))
		{
			BroadcastEvent(EVT_MENU_DELETE_COMPELETE,(WPARAM)1,(LPARAM)GetResource(),NULL);
		}
	}
	else
	{
		BroadcastEvent(EVT_MENU_DELETE_COMPELETE,(WPARAM)1,(LPARAM)GetResource(),NULL);
	}
}

bool CCloudItemUI::OnDeleteResourceCompelete( void* pNotify )
{
	THttpNotify* pHttpNotify = (THttpNotify*)pNotify;
	string strErrorCode="";
	string strMessage = "";
	string strTitle = "";
	string strResource = "";
	int currentModeType=0;
	if(pHttpNotify->pUserData)
	{
		CNDCloudContentService* pService = (CNDCloudContentService*)pHttpNotify->pUserData;
		if(pService)
		{
			strErrorCode = pService->GetErrorCode().c_str();
			strMessage = pService->GetErrorMessage().c_str();
			if(pService->GetUserData())
			{
				CStream* pStream = (CStream*)pService->GetUserData();
				if(pStream)
				{
					pStream->ResetCursor();
					currentModeType = pStream->ReadInt();
					strResource = pStream->ReadString();
					strTitle  = pStream->ReadString();
					delete pStream;
				}
			}
		}
	}
	CStream pStream(1024);
	pStream.WriteInt(currentModeType);
	pStream.WriteString(strResource);
	pStream.WriteString(strTitle);
	pStream.WriteString(strMessage);
	BroadcastEvent(EVT_MENU_DELETE_COMPELETE,(WPARAM)pHttpNotify->dwErrorCode,(LPARAM)&pStream,NULL);
	return true;
}

void CCloudItemUI::Rename(string strNewName)
{
	m_strNewName = strNewName;
	DWORD dwUserID = NDCloudUser::GetInstance()->GetUserId();
	if(dwUserID!=0)
	{
		if(!NDCloudContentServiceManager::GetInstance()->RenameCloudFile(m_strGuid,strNewName,m_strJsonInfo,m_eContentType,MakeHttpDelegate(this, &CCloudItemUI::OnRenameResourceCompelete)))
		{
			BroadcastEvent(EVT_MENU_RENAME_COMPELETE,(WPARAM)1,(LPARAM)GetResource(),NULL);
		}
	}
}

bool CCloudItemUI::OnRenameResourceCompelete( void* pNotify )
{
	THttpNotify* pHttpNotify = (THttpNotify*)pNotify;
	BroadcastEvent(EVT_MENU_RENAME_COMPELETE,(WPARAM)pHttpNotify->dwErrorCode,(LPARAM)GetResource(),NULL);
	if(pHttpNotify->dwErrorCode==0)
	{
		this->SetTitle(m_strNewName.c_str());
	}
	return true;
}