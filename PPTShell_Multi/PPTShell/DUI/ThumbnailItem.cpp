#include "StdAfx.h"
#include "DUI/ThumbnailItem.h"
#include "DUI/GifAnimUI.h"


CThumbnailItemUI::CThumbnailItemUI( IComponent* pComponent )
: CStyleItemUI(pComponent)
{
	m_pOption			= NULL;
	m_lbImage			= NULL;
	m_bHasSetThumbnail	= false;
	m_layMask			= NULL;

}

CThumbnailItemUI::~CThumbnailItemUI()
{

}

void CThumbnailItemUI::DoInit()
{
	__super::DoInit();

	m_lbImage	= (CControlUI*)this->FindSubControl(_T("image"));
	m_pOption	= (CItemOptionUI*)this->FindSubControl(_T("item"));
	m_layMask	= (CContainerUI*)this->FindSubControl(_T("mask"));

	if (GetComponent())
	{
		m_pOption->SetGroup(GetComponent()->GetDescription().c_str());
	}
}

void CThumbnailItemUI::StartMask()
{
	m_layMask->SetVisible(true);
	CGifAnimUI* pLoading = (CGifAnimUI*)m_layMask->FindSubControl(_T("loading"));
	if (pLoading)
	{
		pLoading->PlayGif();
	}
}

void CThumbnailItemUI::StopMask()
{
	m_layMask->SetVisible(false);
	CGifAnimUI* pLoading = (CGifAnimUI*)m_layMask->FindSubControl(_T("loading"));
	if (pLoading)
	{
		pLoading->StopGif();
	}
}

CItemOptionUI* CThumbnailItemUI::GetOption()
{	
	return m_pOption;
}

void CThumbnailItemUI::DoPaint( HDC hDC, const RECT& rcPaint )
{
	__super::DoPaint(hDC, rcPaint);

	RECT rcTemp = { 0 };
	if( !::IntersectRect(&rcTemp, &rcPaint, &m_rcItem) ) return;

 	if (!m_bHasSetThumbnail)
 	{
		m_bHasSetThumbnail = true;
		SetThumbnail();
	}

}


void CThumbnailItemUI::SetThumbnail()
{
	if (GetHandler())
	{
		TNotifyUI	notify;
		notify.pSender		= this;

		GetHandler()->SetTrigger(this);
		GetHandler()->DoSetThumbnail(&notify);
	}
}


bool CThumbnailItemUI::OnControlNotify( void* pObj )
{
	TNotifyUI* pNotify = (TNotifyUI*)pObj;
	if (pNotify->pSender 
		&& pNotify->pSender->GetName() == _T("item")
		&& pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		if (GetHandler())
		{
			TNotifyUI notify;
			notify.pSender  = this;
			GetHandler()->SetTrigger(this);
			GetHandler()->DoClick(&notify);
		}
	}

	return true;
}

bool CThumbnailItemUI::OnControlEvent( void* pObj )
{
	return true;
}


void CThumbnailItemUI::SetImage( LPCTSTR lptcsPath )
{
	if (m_lbImage)
	{
		m_lbImage->SetBkImage(lptcsPath);
	}
}

CControlUI* CThumbnailItemUI::GetImageCtrl()
{
	return m_lbImage;
}

void CThumbnailItemUI::OnGetThumbnailBefore()
{
	this->StartMask();
}

void CThumbnailItemUI::OnGetThumbnailCompleted( LPCTSTR lptcsPath )
{
	this->StopMask();

	if (IsResourceExist(lptcsPath))
	{
		SetImage(lptcsPath);
	}
}

void CThumbnailItemUI::ReadStream( CStream* pStream )
{
	if (GetHandler())
	{
		GetHandler()->ReadFrom(pStream);
	}
}

bool CThumbnailItemUI::IsResourceExist( LPCTSTR lptcsPath )
{
	DWORD ret = GetFileAttributes(lptcsPath);
	if( ret == INVALID_FILE_ATTRIBUTES )
		return false;	
	return true;
}
