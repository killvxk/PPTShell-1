#include "stdafx.h"
#include "ListCloudItem.h"
#include "NDCloud/NDCloudFile.h"
#include "NDCloud/NDCloudAPI.h"

#include "DUI/GifAnimUI.h"

#include "Util/Util.h"
#include "DUI/GroupExplorer.h"

CListCloudItem::CListCloudItem()
{
	m_dwCurCountDownId			= -1;
	m_dwTotalCountDownId		= -1;
	m_pStream					= NULL;
}

CListCloudItem::~CListCloudItem()
{
	if(m_dwCurCountDownId != -1)
	{
		NDCloudDownloadCancel(m_dwCurCountDownId);
	}
	if(m_dwTotalCountDownId != -1)
	{
		NDCloudDownloadCancel(m_dwTotalCountDownId);
	}

	CancelEvent(EVT_SET_CHAPTER_GUID,MakeEventDelegate(this,&CListCloudItem::OnChapterChanged));
	CancelEvent(EVT_REFRESH_GROUPEXPLORER,MakeEventDelegate(this,&CListCloudItem::OnRefreshGroupExplorer));
}

void CListCloudItem::DoInit()
{
	__super::DoInit();

	::OnEvent(EVT_SET_CHAPTER_GUID, MakeEventDelegate(this, &CListCloudItem::OnChapterChanged));
	::OnEvent(EVT_REFRESH_GROUPEXPLORER, MakeEventDelegate(this, &CListCloudItem::OnRefreshGroupExplorer));

	m_pTextCurCount		= dynamic_cast<CLabelUI *>(FindSubControl(_T("CurCount")));
	m_pTextSeparator	= dynamic_cast<CLabelUI *>(FindSubControl(_T("Separator")));
	m_pTextTotalCount	= dynamic_cast<CLabelUI *>(FindSubControl(_T("TotalCount")));

	m_pContentBtn		= dynamic_cast<CButtonUI *>(FindSubControl(_T("ContentBtn")));

	m_pTextCurCount->SetText(_T("0"));

	m_pCountContainer	= dynamic_cast<CHorizontalLayoutUI*>(FindSubControl(_T("CountContainer")));

	m_pTotalLoadGif		= new CGifAnimUI;
	m_pTotalLoadGif->SetBkImage(_T("RightBar\\loading.gif"));
	m_pTotalLoadGif->SetFixedWidth(12);
	m_pTotalLoadGif->SetFixedHeight(12);
	m_pTotalLoadGif->SetPadding(CDuiRect(0,6,0,0));
	m_pTotalLoadGif->SetVisible(false);
	m_pCountContainer->AddAt(m_pTotalLoadGif,3);

	m_pCurLoadGif		= new CGifAnimUI;
	m_pCurLoadGif->SetBkImage(_T("RightBar\\loading.gif"));
	m_pCurLoadGif->SetFixedWidth(12);
	m_pCurLoadGif->SetFixedHeight(12);
	m_pCurLoadGif->SetPadding(CDuiRect(0,6,0,0));
	m_pCountContainer->AddAt(m_pCurLoadGif,1);
	m_pCurLoadGif->SetVisible(false);

	m_pContainerIcon	= static_cast<COptionUI*>(FindSubControl(_T("icon")));

	m_dwSelectedTextColor	= 0xFF10b0b6;
	m_dwSelectedCountColor	= 0xFFE1D719;
	m_dwNormalTextColor		= 0xFFD8D8D8;
	m_dwNormalCountColor	= 0xFFD8D8D8;
}

void CListCloudItem::DoClick( TNotifyUI* pNotify )
{
	CGroupExplorerUI * pGroupExplorer = CGroupExplorerUI::GetInstance();
	if(!IsWindowVisible(pGroupExplorer->GetHWND()))
		pGroupExplorer->ShowWindow(true);

	CItemExplorerUI * pItemExplorer = CItemExplorerUI::GetInstance();
	if(IsWindowVisible(pItemExplorer->GetHWND()))
		pItemExplorer->ShowWindow(false);

	pGroupExplorer->StartMask();
	
	pGroupExplorer->SetTitleText(m_strName.c_str());

	if(m_pStream == NULL)
	{
		if(m_dwDownloadId != -1)
		{
			NDCloudDownloadCancel(m_dwDownloadId);
		}
		m_dwDownloadId = NDCloudDownload(GetJsonUrl(), MakeHttpDelegate(this, &CListCloudItem::OnDownloadDecodeList));
	}
	else
	{
		pGroupExplorer->ShowResource(m_nType, m_pStream);
		pGroupExplorer->StopMask();
	}

	if(m_bCurNetLess == true)
	{
		GetCurCountInterface(NDCloudGetChapterGUID());//网络错误点击刷新
	}

	if(m_OnDoClickCallBack)
		m_OnDoClickCallBack(pNotify);
}

void CListCloudItem::DoRClick( TNotifyUI* pNotify )
{

}

bool CListCloudItem::OnChapterChanged( void* pObj )
{
	//刷新总量
	TEventNotify* pEventNotify = (TEventNotify*)pObj;
	CStream* pStream = (CStream*)pEventNotify->wParam;
	pStream->ResetCursor();

	tstring strGuid = pStream->ReadString();

	GetCurCountInterface(strGuid);
	return true;
}

bool CListCloudItem::OnRefreshGroupExplorer( void* pObj )
{
	if(m_bCurNetLess == true)
	{
		GetCurCountInterface(NDCloudGetChapterGUID());
		GetTotalCountInterface();
	}

	if(IsSelected())
	{
		if(m_pStream == NULL)
		{
// 			if(m_dwDownloadId != -1)
// 			{
// 				NDCloudDownloadCancel(m_dwDownloadId);
// 			}
			if(GetJsonUrl().length() > 0)
				m_dwDownloadId = NDCloudDownload(GetJsonUrl(), MakeHttpDelegate(this, &CListCloudItem::OnDownloadDecodeList));
		}
	}
	
	return true;
}

void CListCloudItem::GetTotalCountInterface()
{
	m_pTextTotalCount->SetVisible(false);

	m_pTotalLoadGif->SetVisible(true);
	m_pTotalLoadGif->PlayGif();


	//增加本地统计
	CTime tm = CTime::GetCurrentTime();
	DWORD dwTime = tm.GetTime();

	tstring strConfinFile = GetLocalPath();
	strConfinFile += _T("\\Setting\\Config.ini");

	TCHAR szTime[MAX_PATH * 2 + 1];
	GetPrivateProfileString(_T("Data"), _T("Date"), _T(""), szTime, MAX_PATH * 2, strConfinFile.c_str());
	if( _tcslen(szTime) > 0)
	{
		DWORD dwLocalTime = _tcstoul(szTime, NULL , 10);

		if(dwTime - dwLocalTime < 12 * 60 * 60)
		{
			TCHAR szCount[MAX_PATH * 2 + 1];
			GetPrivateProfileString(_T("Data"), GetUserData(), _T(""), szCount, MAX_PATH * 2, strConfinFile.c_str());
			if( _tcslen(szCount) > 0)
			{
				int nCount = _ttoi(szCount);
				if(nCount != 0)
				{
					m_pTotalLoadGif->StopGif();
					m_pTotalLoadGif->SetVisible(false);


					TCHAR szBuff[128] ={0};
					_stprintf(szBuff,_T("%d"), nCount);

					SIZE textSize = CRenderEngine::GetTextSize(GetManager()->GetPaintDC(), GetManager(), 
						szBuff, m_pTextTotalCount->GetFont(), DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT| DT_NOPREFIX);

					m_pTextTotalCount->SetFixedWidth(textSize.cx);
					m_pTextTotalCount->SetVisible(true);
					m_pTextTotalCount->SetText(szBuff);

					m_bTotalNetLess = false;

					return;
				}
				
			}
		}

	}

	//

	m_dwTotalCountDownId = NDCloudDownload(GetTotalCountUrl(), MakeHttpDelegate(this, &CListCloudItem::OnGetTotalCount));
}

void CListCloudItem::GetCurCountInterface( tstring strGuid )
{
	m_pTextCurCount->SetVisible(false);

	m_pCurLoadGif->SetVisible(true);
	m_pCurLoadGif->PlayGif();

	if(m_dwCurCountDownId != -1)
	{
		NDCloudDownloadCancel(m_dwCurCountDownId);
	}

	m_dwCurCountDownId = NDCloudDownload(GetCurCountUrl(), MakeHttpDelegate(this, &CListCloudItem::OnGetCurCount));
}

void CListCloudItem::SetCurCountUrl( tstring strUrl )
{
	m_strCurCountUrl = strUrl;
}

tstring CListCloudItem::GetCurCountUrl()
{
	return m_strCurCountUrl;
}

void CListCloudItem::SetTotalCountUrl( tstring strUrl )
{
	m_strTotalCountUrl = strUrl;
}

tstring CListCloudItem::GetTotalCountUrl()
{
	return m_strTotalCountUrl;
}

bool CListCloudItem::OnGetTotalCount( void * pParam )
{
	THttpNotify * pNotify = (THttpNotify*)pParam;

	m_pTotalLoadGif->StopGif();
	m_pTotalLoadGif->SetVisible(false);

	if(pNotify->dwErrorCode != 0)
	{
		m_bTotalNetLess = true;
		return true;
	}

	int nCount = NDCloudDecodeCount(pNotify->pData, pNotify->nDataSize);


	TCHAR szBuff[128] ={0};
	wsprintf(szBuff,_T("%d"), nCount);

	SIZE textSize = CRenderEngine::GetTextSize(GetManager()->GetPaintDC(), GetManager(), 
		szBuff, m_pTextTotalCount->GetFont(), DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT| DT_NOPREFIX);

	m_pTextTotalCount->SetFixedWidth(textSize.cx);
	m_pTextTotalCount->SetVisible(true);
	m_pTextTotalCount->SetText(szBuff);


	m_bTotalNetLess = false;

	//增加本地统计
	CTime tm = CTime::GetCurrentTime();
	DWORD dwTime = tm.GetTime();

	tstring strConfinFile = GetLocalPath();
	strConfinFile += _T("\\Setting\\Config.ini");

	TCHAR szTime[MAX_PATH * 2 + 1];
	_stprintf(szTime, _T("%d"), dwTime);
	WritePrivateProfileString(_T("Data"), _T("Date"), szTime, strConfinFile.c_str());

	TCHAR szCount[MAX_PATH * 2 + 1];
	_stprintf(szCount, _T("%d"), nCount);
	WritePrivateProfileString(_T("Data"), GetUserData(), szCount, strConfinFile.c_str());
	

	return true;
}

bool CListCloudItem::OnGetCurCount( void * pParam )
{
	THttpNotify * pNotify = (THttpNotify*)pParam;

	m_pCurLoadGif->StopGif();
	m_pCurLoadGif->SetVisible(false);


	if(pNotify->dwErrorCode != 0)
	{
		m_bCurNetLess = true;
		return true;
	}


	int nCount = NDCloudDecodeCount(pNotify->pData, pNotify->nDataSize);

	if(m_pTextCurCount)
	{
		TCHAR szBuff[128] ={0};
		wsprintf(szBuff,_T("%d"), nCount);
		
		SIZE textSize = CRenderEngine::GetTextSize(GetManager()->GetPaintDC(), GetManager(), 
			szBuff, m_pTextCurCount->GetFont(), DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT| DT_NOPREFIX);

		m_pTextCurCount->SetFixedWidth(textSize.cx);
		m_pTextCurCount->SetVisible(true);
		m_pTextCurCount->SetText(szBuff);

		m_bCurNetLess = false;
	}
	
	return true;
}

void  CListCloudItem::CalcText( HDC hdc, RECT& rc, LPCTSTR lpszText, int nFontId, UINT format, UITYPE_FONT nFontType, int c)
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

bool CListCloudItem::Select( bool bSelect /*= true*/ )
{
	__super::Select(bSelect);

	if ( m_pContainerIcon != NULL )
		m_pContainerIcon->Selected(bSelect);

	m_pContentBtn->SetTextColor(bSelect ? m_dwSelectedTextColor : m_dwNormalTextColor);
	m_pTextCurCount->SetTextColor(bSelect ? m_dwSelectedCountColor : m_dwNormalCountColor);

	return true;
}
