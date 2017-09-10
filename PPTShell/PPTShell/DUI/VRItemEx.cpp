#include "StdAfx.h"


#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "Util/Util.h"
#include "Util/Stream.h"
#include "DUI/DragDialogUI.h"

#include "DUI/ResourceItemEx.h"

#include "DUI/ResourceStyleable.h"
#include "DUI/PhotoStyleable.h"
#include "DUI/VideoStyleable.h"
#include "DUI/FlashStyleable.h"
#include "DUI/VolumeStyleable.h"
#include "DUI/CourseStyleable.h"
#include "DUI/3DResourceStyleable.h"
#include "DUI/QuestionStyleable.h"
#include "DUI/BankCourseStyleable.h"
#include "DUI/PPTTemplateStyleable.h"
#include "DUI/NdpStyleable.h"
#include "DUI/NdpxStyleable.h"

#include "DUI/IComponent.h"
#include "DUI/ItemComponent.h"
#include "DUI/CloudComponent.h"
#include "DUI/MaskComponent.h"
#include "DUI/ToolbarComponent.h"

#include "Util/Tween.h"
#include "DUI/DragDialogUI.h"
#include "DUI/ItemExplorer.h"
#include "PPTControl/PPTController.h"
#include "NDCloud/NDCloudAPI.h"

//CloudPhotoHandler
#include "DUI/IItemHandler.h"
#include "DUI/INotifyHandler.h"
#include "NDCloud/NDCloudAPI.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IDownloadListener.h"
#include "Util/Stream.h"
#include "DUI/IStreamReader.h"
#include "DUI/IVisitor.h"
#include "DUI/ItemHandler.h"
#include "DUI/CloudResourceHandler.h"
#include "DUI/IThumbnailListener.h"
#include "DUI/CloudPhotoHandler.h"
#include "DUI/CloudFlashHandler.h"
#include "DUI/CloudVideoHandler.h"
#include "DUI/CloudCourseHandler.h"
#include "DUI/Cloud3DResourceHandler.h"
#include "DUI/CloudQuestionHandler.h"
#include "DUI/CloudInteractQuestionHandler.h"
#include "DUI/CloudNdpHandler.h"
#include "DUI/CloudNdpXHandler.h"
#include "DUI/CloudVRResourceHandler.h"

#include "DUI/Toast.h"
#include "NDCloud/NDCloudContentService.h"

#include "DUI/VRItemEx.h"

CVRItemExUI::CVRItemExUI( IComponent* pComponent )
	: CResourceItemExUI(pComponent)
{
	m_btnInsert		= NULL;
	m_btnResume		= NULL;
	m_btnPause		= NULL;
	m_layEmptyTip	= NULL;
}

CVRItemExUI::~CVRItemExUI()
{

}


void CVRItemExUI::DoInit()
{
	__super::DoInit();


	m_btnInsert		= static_cast<CControlUI*>( FindSubControl(_T("btn1")));
	m_btnResume		= static_cast<CControlUI*>( FindSubControl(_T("btn_start")));
	m_btnPause		= static_cast<CControlUI*>( FindSubControl(_T("btn_pause")));
	m_layEmptyTip	= static_cast<CContainerUI*>( FindSubControl(_T("lay_emptytip")));
	

	if (GetHandler())
	{
		tstring strResource = GetHandler()->GetResource();
		if (_tcsicmp(strResource.c_str(), _T("")) == 0)
		{
			EnableToolbar(false);
			ShowEmptyTip(true);
		}
		else if (!NDCloudFileManager::GetInstance()->IsFileDownloaded(strResource.c_str()))
		{
			m_btnInsert->SetVisible(false);
			m_btnResume->SetVisible(true);
			m_btnPause->SetVisible(false);
		}
	}

	EnableDrag(false);
	
}


bool CVRItemExUI::OnControlNotify( void* pObj )
{
	__super::OnControlNotify(pObj);
	TNotifyUI* pNotify = (TNotifyUI*)pObj;

	if (pNotify->pSender 
		&& pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		int			nBtnIndex	= 0;
		TNotifyUI	notify;
		if (_stscanf_s(pNotify->pSender->GetName(), _T("btn%d"), &nBtnIndex) != 1)
		{
			notify.wParam		= (WPARAM)pNotify->pSender->GetTag();
			notify.pSender		= this;
			if (GetHandler())
			{
				GetHandler()->SetTrigger(this);
				GetHandler()->DoButtonClick(&notify);
			}
		}
	}
	return true;
}


void CVRItemExUI::OnDownloadBefore( THttpNotify* pHttpNotify )
{
	__super::OnDownloadBefore(pHttpNotify);
}

void CVRItemExUI::OnDownloadProgress( THttpNotify* pHttpNotify )
{
	__super::OnDownloadProgress(pHttpNotify);


	m_btnInsert->SetVisible(false);
	m_btnResume->SetVisible(false);
	m_btnPause->SetVisible(true);
}

void CVRItemExUI::OnDownloadCompleted( THttpNotify* pHttpNotify )
{
	__super::OnDownloadCompleted(pHttpNotify);
	if (!pHttpNotify || pHttpNotify->dwErrorCode != 0)
	{
		this->ShowProgress(false);

		m_btnInsert->SetVisible(false);
		m_btnResume->SetVisible(true);
		m_btnPause->SetVisible(false);
		return;
	}

	m_btnInsert->SetVisible(true);
	m_btnResume->SetVisible(false);
	m_btnPause->SetVisible(false);
}

void CVRItemExUI::OnDownloadInterpose( THttpNotify* pHttpNotify )
{
	__super::OnDownloadInterpose(pHttpNotify);

	if (pHttpNotify->dwErrorCode == eInterpose_Pause)
	{
		m_btnInsert->SetVisible(false);
		m_btnResume->SetVisible(true);
		m_btnPause->SetVisible(false);

		this->ShowProgress(false);
		m_pEmptyCtrl->SetToolTip(_T(""));
	}
	else if (pHttpNotify->dwErrorCode == eInterpose_Resume)
	{
		m_btnInsert->SetVisible(false);
		m_btnResume->SetVisible(false);
		m_btnPause->SetVisible(true);
	}
	else if (pHttpNotify->dwErrorCode == eInterpose_Cancel)
	{
		m_btnInsert->SetVisible(false);
		m_btnResume->SetVisible(true);
		m_btnPause->SetVisible(false);
	}

}

void CVRItemExUI::ShowProgress( bool bVisible )
{
	m_proDownload->SetVisible(bVisible);
}

void CVRItemExUI::SetTitleColor( DWORD dwColor )
{
	if (GetTitleCtrl())
	{
		GetTitleCtrl()->SetTextColor(dwColor);
	}
}

void CVRItemExUI::ShowEmptyTip( bool bShow )
{
	if (m_layEmptyTip)
	{
		m_layEmptyTip->SetVisible(bShow);
	}
}	

