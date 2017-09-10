#include "stdafx.h"
#include "AssetsItem.h"
#include "NDCloud/NDCloudFile.h"
#include "NDCloud/NDCloudAPI.h"

CAssetsItem::CAssetsItem()
{
	m_dwDownloadId				= -1;

	m_pStream					= NULL;
	m_bSelected					= false;
	m_bCurNetLess				= false;
	::OnEvent(EVT_SET_CHAPTER_GUID, MakeEventDelegate(this, &CAssetsItem::OnChapterChanged));
	::OnEvent(EVT_REFRESH_GROUPEXPLORER, MakeEventDelegate(this, &CAssetsItem::OnRefreshGroupExplorer));
	::OnEvent(EVT_LOGIN, MakeEventDelegate(this, &CAssetsItem::OnEventLoginComplete));
}

CAssetsItem::~CAssetsItem()
{
	if(m_dwDownloadId != -1)
	{
		NDCloudDownloadCancel(m_dwDownloadId);
	}

	CancelEvent(EVT_SET_CHAPTER_GUID,MakeEventDelegate(this,&CAssetsItem::OnChapterChanged));
	CancelEvent(EVT_REFRESH_GROUPEXPLORER,MakeEventDelegate(this,&CAssetsItem::OnRefreshGroupExplorer));
	CancelEvent(EVT_LOGIN, MakeEventDelegate(this, &CAssetsItem::OnEventLoginComplete));
}

void CAssetsItem::DoInit()
{
	this->OnEvent			+= MakeDelegate(this, &CAssetsItem::OnControlNotify);

	

	CHorizontalLayoutUI * pLayout = new CHorizontalLayoutUI;
	pLayout->SetFixedHeight(20);
	
	CLabelUI * pLabel = new CLabelUI;
	m_pTextLabel = pLabel;
	pLabel->SetTextColor(0xFFA1A1A1);
	pLabel->SetFont(120000);
	pLabel->SetText(m_strName.c_str());

//	SIZE textSize = CRenderEngine::GetTextSize(GetManager()->GetPaintDC(), GetManager(), 
//		m_strName.c_str(), m_pTextLabel->GetFont(), DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT| DT_NOPREFIX);
//	pLabel->SetFixedWidth(textSize.cx);

	pLabel->SetAttribute(_T("align"), _T("center"));
	pLayout->Add(pLabel);

	this->Add(pLayout);

	pLabel->OnEvent			+= MakeDelegate(this, &CAssetsItem::OnControlNotify);
	pLayout->OnEvent		+= MakeDelegate(this, &CAssetsItem::OnControlNotify);

	pLayout = new CHorizontalLayoutUI;
	pLayout->SetFixedHeight(20);
	
	CGifAnimUI * pGif = new CGifAnimUI;
	m_pGif = pGif;
	pGif->SetBkImage(_T("RightBar\\loading.gif"));
	pGif->SetFixedWidth(12);
	pGif->SetFixedHeight(12);
	pGif->SetPadding(CDuiRect(30,3,0,0));
	pGif->SetVisible(false);
	pLayout->Add(pGif);
	pLabel = new CLabelUI;
	m_pCountLabel = pLabel;
	pLabel->SetTextColor(0xFFA1A1A1);
	pLabel->SetFont(120000);
	pLabel->SetVisible(false);
	pLabel->SetAttribute(_T("align"), _T("center"));
	pLayout->Add(pLabel);

	this->Add(pLayout);

	pGif->OnEvent			+= MakeDelegate(this, &CAssetsItem::OnControlNotify);
	pLayout->OnEvent		+= MakeDelegate(this, &CAssetsItem::OnControlNotify);
	pLabel->OnEvent			+= MakeDelegate(this, &CAssetsItem::OnControlNotify);
}


bool CAssetsItem::OnControlNotify( void* pEvent )
{
	TEventUI* Event = (TEventUI *)pEvent;

	if(Event->Type == UIEVENT_BUTTONDOWN )
	{
		DoClick(Event);
	}
	else if(Event->Type == UIEVENT_RBUTTONDOWN )
	{
		DoRClick(Event);
	}
	return true;
}

void CAssetsItem::DoClick( TEventUI* pEvent )
{
	if(m_bSelected == false)
	{
		Select(!m_bSelected);

		if(m_OnDoClickCallBack)
			m_OnDoClickCallBack(this);
	}
}

void CAssetsItem::DoRClick( TEventUI* pEvent )
{
	if(m_OnDoRClickCallBack)
		m_OnDoRClickCallBack(pEvent);
}

void CAssetsItem::SetJsonUrl( tstring strJsonUrl )
{
	m_strJsonUrl = strJsonUrl;
}

tstring CAssetsItem::GetJsonUrl()
{
	return m_strJsonUrl;
}

int CAssetsItem::GetItemType()
{
	return m_nType;
}

void CAssetsItem::SetDoClickCallBack( CDelegateBase& delegate )
{
	m_OnDoClickCallBack.clear();
	m_OnDoClickCallBack += delegate;
}

void CAssetsItem::SetDoRClickCallBack( CDelegateBase& delegate )
{
	m_OnDoRClickCallBack.clear();
	m_OnDoRClickCallBack += delegate;
}

void CAssetsItem::SetDoFinishCallBack( CDelegateBase& delegate )
{
	m_OnFinishCallBack.clear();
	m_OnFinishCallBack += delegate;
}


void CAssetsItem::RefreshData(bool bForce)
{
	if(m_dwDownloadId != -1 && bForce)
	{
		NDCloudDownloadCancel(m_dwDownloadId);
	}

	if(m_pStream == NULL || bForce)
	{
		m_pCountLabel->SetVisible(false);
		m_pGif->PlayGif();
		m_pGif->SetVisible(true);

		GetLayout()->GetContentLayout()->RemoveAll();
		GetLayout()->StartLoading();
		GetLayout()->ShowNetLess(false);
		m_dwDownloadId = NDCloudDownload(GetJsonUrl(), MakeHttpDelegate(this, &CAssetsItem::OnDownloadDecodeList));
	}
}

bool CAssetsItem::OnChapterChanged( void* pObj )
{
	m_bCurNetLess = false;
	if(m_pStream)
	{
		delete m_pStream;
		m_pStream = NULL;
	}
	return true;
}

bool CAssetsItem::OnRefreshGroupExplorer( void* pObj )
{
	if(IsVisible())
		RefreshData(true);
	return true;
}

void CAssetsItem::SetLayout( CAssetsLayout* pLayout )
{
	m_pLayout = pLayout;
}

CAssetsLayout* CAssetsItem::GetLayout()
{
	return m_pLayout;
}

void CAssetsItem::Select( bool bSelected )
{
	m_bSelected = bSelected;
	if(bSelected)//бЁжа
	{
		m_pTextLabel->SetTextColor(0xFFFFFFFF);
		m_pCountLabel->SetTextColor(0xFFFFFFFF);
		this->SetBkColor(0xFF10B0B6);
	}
	else
	{
		m_pTextLabel->SetTextColor(0xFFA1A1A1);
		m_pCountLabel->SetTextColor(0xFFA1A1A1);
		this->SetBkColor(0xFF515151);
	}
}

bool CAssetsItem::GetSelected()
{
	return m_bSelected;
}

void CAssetsItem::SetCountText( int nCount )
{
	TCHAR szText[32] = {0};
	_stprintf_s(szText, _T("%d"), nCount);

// 	SIZE textSize = CRenderEngine::GetTextSize(GetManager()->GetPaintDC(), GetManager(), 
// 		szText, m_pCountLabel->GetFont(), DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT| DT_NOPREFIX);
// 	m_pCountLabel->SetFixedWidth(textSize.cx);


	m_pCountLabel->SetText(szText);
	m_pCountLabel->SetVisible(true);
}

void CAssetsItem::CalcText( HDC hdc, RECT& rc, LPCTSTR lpszText, int nFontId, UINT format, UITYPE_FONT nFontType, int c /*= -1*/ )
{
	if (nFontType == UIFONT_GDI)
	{
		HFONT hFont = GetManager()->GetFont(nFontId);
		HFONT hOldFont = (HFONT)::SelectObject(hdc, hFont);

		if ((DT_SINGLELINE & format))
		{
			SIZE size = {0};
			::GetTextExtentExPoint(hdc, lpszText, c == -1 ? _tcslen(lpszText) : c, 0, NULL, NULL, &size);
			rc.right = rc.left + size.cx;
			rc.bottom = rc.top + size.cy;
		}
		else
		{
			format &= ~DT_END_ELLIPSIS;
			format &= ~DT_PATH_ELLIPSIS;
			if (!(DT_SINGLELINE & format)) format |= DT_WORDBREAK | DT_EDITCONTROL;
			::DrawText(hdc, lpszText, c, &rc, format | DT_CALCRECT);
		}
		::SelectObject(hdc, hOldFont);
	}

}

CStream* CAssetsItem::GetStream()
{
	return m_pStream;
}

bool CAssetsItem::GetNetStatus()
{
	return m_bCurNetLess;
}

bool CAssetsItem::OnEventLoginComplete( void* pObj )
{
	return true;
}
