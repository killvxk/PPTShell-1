#include "stdafx.h"
#include "ListDBankItem.h"
#include "NDCloud/NDCloudFile.h"
#include "NDCloud/NDCloudAPI.h"
#include "DUI/GroupExplorer.h"

CListDBankItem::CListDBankItem()
{
	m_dwTotalCountDownId		= -1;
}

CListDBankItem::~CListDBankItem()
{
	if(m_dwTotalCountDownId != -1)
	{
		NDCloudDownloadCancel(m_dwTotalCountDownId);
	}

	CancelEvent(EVT_REFRESH_GROUPEXPLORER,MakeEventDelegate(this,&CListDBankItem::OnRefreshGroupExplorer));
	CancelEvent(EVT_LOGIN, MakeEventDelegate(this, &CListDBankItem::OnEventLoginComplete));
}

void CListDBankItem::DoInit()
{
	__super::DoInit();

	::OnEvent(EVT_LOGIN, MakeEventDelegate(this, &CListDBankItem::OnEventLoginComplete));
	::OnEvent(EVT_REFRESH_GROUPEXPLORER, MakeEventDelegate(this, &CListDBankItem::OnRefreshGroupExplorer));

	m_pTextTotalCount	= dynamic_cast<CLabelUI *>(FindSubControl(_T("TotalCount")));

	m_pContentBtn		= dynamic_cast<CButtonUI *>(FindSubControl(_T("ContentBtn")));

	m_pCountContainer	= dynamic_cast<CHorizontalLayoutUI*>(FindSubControl(_T("CountContainer")));
	m_pContainerIcon	= static_cast<COptionUI*>(FindSubControl(_T("icon")));

	m_pTotalLoadGif		= new CGifAnimUI;
	m_pTotalLoadGif->SetBkImage("RightBar\\loading.gif");
	m_pTotalLoadGif->SetFixedWidth(12);
	m_pTotalLoadGif->SetFixedHeight(12);
	m_pTotalLoadGif->SetPadding(CDuiRect(0,6,0,0));
	m_pTotalLoadGif->SetVisible(false);
	m_pCountContainer->AddAt(m_pTotalLoadGif,1);

	m_dwSelectedTextColor	= 0xFF10b0b6;
	m_dwSelectedCountColor	= 0xFFE1D719;
	m_dwNormalTextColor		= 0xFFD8D8D8;
	m_dwNormalCountColor	= 0xFFD8D8D8;
}

void CListDBankItem::DoClick( TNotifyUI* pNotify )
{
	CGroupExplorerUI * pGroupExplorer = CGroupExplorerUI::GetInstance();
	pGroupExplorer->ShowWindow(true);

	pGroupExplorer->StartMask();

	pGroupExplorer->SetTitleText(m_strName.c_str());

	if(m_dwDownloadId != -1)
	{
		NDCloudDownloadCancel(m_dwDownloadId);
	}
	m_dwDownloadId = NDCloudDownload(GetJsonUrl(), MakeHttpDelegate(this, &CListDBankItem::OnDownloadDecodeList));

	GetTotalCountInterface();

	if(m_OnDoClickCallBack)
		m_OnDoClickCallBack(pNotify);
}

void CListDBankItem::DoRClick( TNotifyUI* pNotify )
{
	if(m_OnDoRClickCallBack)
		m_OnDoRClickCallBack(pNotify);
}

bool CListDBankItem::OnRefreshGroupExplorer( void* pObj )
{
// 	if(m_bTotalNetLess == true)
// 	{
// 		
// 	}
	GetTotalCountInterface();

	if(IsSelected())
	{
		if(m_dwDownloadId != -1)
		{
			NDCloudDownloadCancel(m_dwDownloadId);
		}
		if(GetJsonUrl().length() > 0)
			m_dwDownloadId = NDCloudDownload(GetJsonUrl(), MakeHttpDelegate(this, &CListDBankItem::OnDownloadDecodeList));
	}
	
	return true;
}

void CListDBankItem::GetTotalCountInterface()
{
	if(m_dwTotalCountDownId != -1)
	{
		NDCloudDownloadCancel(m_dwTotalCountDownId);
	}

	m_dwTotalCountDownId = NDCloudDownload(GetTotalCountUrl(), MakeHttpDelegate(this, &CListDBankItem::OnGetTotalCount));
}

void CListDBankItem::SetTotalCountUrl( tstring strUrl )
{
	m_strTotalCountUrl = strUrl;
}

tstring CListDBankItem::GetTotalCountUrl()
{
	return m_strTotalCountUrl;
}

bool CListDBankItem::OnGetTotalCount( void * pParam )
{
	THttpNotify * pNotify = (THttpNotify*)pParam;

	if(m_pTotalLoadGif)
	{
		m_pTotalLoadGif->StopGif();
		m_pTotalLoadGif->SetVisible(false);
	}

	if(pNotify->dwErrorCode != 0)
	{
		m_bTotalNetLess = true;
		return true;
	}

	int nCount = NDCloudDecodeCount(pNotify->pData, pNotify->nDataSize);

	char szBuff[128] ={0};
	sprintf(szBuff,_T("%d"), nCount);
	CDuiRect rcCalc;
	rcCalc.right = 50;
	CalcText(GetManager()->GetPaintDC(), rcCalc, szBuff, m_pTextTotalCount->GetFont(), DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT|DT_NOPREFIX, UIFONT_GDI);
	int nLen = rcCalc.GetWidth();

	m_pTextTotalCount->SetVisible(true);
	m_pTextTotalCount->SetText(szBuff);
	m_pTextTotalCount->SetFixedWidth(nLen);

	m_bTotalNetLess = false;

	return true;
}

bool CListDBankItem::OnChapterChanged( void* pObj )
{	
	return true;
}

void CListDBankItem::CalcText( HDC hdc, RECT& rc, LPCTSTR lpszText, int nFontId, UINT format, UITYPE_FONT nFontType, int c /*= -1*/ )
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

bool CListDBankItem::Select( bool bSelect /*= true*/ )
{
	__super::Select(bSelect);

	if ( m_pContainerIcon != NULL )
		m_pContainerIcon->Selected(bSelect);

	m_pContentBtn->SetTextColor(bSelect ? m_dwSelectedTextColor : m_dwNormalTextColor);
	m_pTextTotalCount->SetTextColor(bSelect ? m_dwSelectedCountColor : m_dwNormalCountColor);

	return true;
}


vector<int>* CListDBankItem::GetSupportType()
{
	return &m_vecSupportType;
}
