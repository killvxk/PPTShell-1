#include "stdafx.h"
#include "ListLocalItem.h"
#include "NDCloud/NDCloudFile.h"
#include "NDCloud/NDCloudAPI.h"

#include "DUI/GifAnimUI.h"

#include "Util/Util.h"
#include "DUI/GroupExplorer.h"

CListLocalItem::CListLocalItem()
{

}

CListLocalItem::~CListLocalItem()
{
	CancelEvent(EVT_SET_CHAPTER_GUID, MakeEventDelegate(this, &CListLocalItem::OnChapterChanged));
}

void CListLocalItem::DoInit()
{
	__super::DoInit();

	::OnEvent(EVT_SET_CHAPTER_GUID, MakeEventDelegate(this, &CListLocalItem::OnChapterChanged));

	m_pTextTotalCount	= dynamic_cast<CLabelUI *>(FindSubControl(_T("TotalCount")));

	m_pContentBtn		= dynamic_cast<CButtonUI *>(FindSubControl(_T("ContentBtn")));

	m_pCountContainer	= dynamic_cast<CHorizontalLayoutUI*>(FindSubControl(_T("CountContainer")));

	m_pContainerIcon	= static_cast<COptionUI*>(FindSubControl(_T("icon")));

	m_strFolderName		= GetUserData();

	m_dwSelectedTextColor	= 0xFF10b0b6;
	m_dwSelectedCountColor	= 0xFFE1D719;
	m_dwNormalTextColor		= 0xFFD8D8D8;
	m_dwNormalCountColor	= 0xFFD8D8D8;
}

void CListLocalItem::DoClick( TNotifyUI* pNotify )
{
	tstring strName = GetFolderName();
	int	nCount	= GetFileCount();
	CStream Steam(1024);
	Steam.WriteDWORD(nCount);
	Steam.WriteString((char*)strName.c_str());
	Steam.ResetCursor();

	CStream streamParams(1024);
	streamParams.WriteString(m_strChapterGUID);
	streamParams.WriteString(m_strChapter);
	streamParams.ResetCursor();

	CGroupExplorerUI * pGroupExplorer = CGroupExplorerUI::GetInstance();
	pGroupExplorer->ShowResource(m_nType, &Steam, &streamParams);

	if(m_OnDoClickCallBack)
		m_OnDoClickCallBack(pNotify);
}

void CListLocalItem::DoRClick( TNotifyUI* pNotify )
{
	if(m_OnDoRClickCallBack)
		m_OnDoRClickCallBack(pNotify);
}

void CListLocalItem::DoSelect( TNotifyUI* pNotify )
{

}

bool CListLocalItem::OnChapterChanged( void* pObj )
{
	TEventNotify* pEventNotify = (TEventNotify*)pObj;
	CStream* pStream = (CStream*)pEventNotify->wParam;
	pStream->ResetCursor();

	m_strChapterGUID	= pStream->ReadString();
	m_strChapter		= pStream->ReadString();

	return true;
}

bool CListLocalItem::OnRefreshGroupExplorer( void* pObj )
{
	return true;
}

void  CListLocalItem::CalcText( HDC hdc, RECT& rc, LPCTSTR lpszText, int nFontId, UINT format, UITYPE_FONT nFontType, int c)
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

void CListLocalItem::SetFileCount( int nCount )
{
	m_nCount = nCount;
	TCHAR szBuff[128] ={0};
	_stprintf(szBuff,_T("%d"), nCount);
	CDuiRect rcCalc;
	rcCalc.right = 50;
	CalcText(GetManager()->GetPaintDC(), rcCalc, szBuff, m_pTextTotalCount->GetFont(), DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT|DT_NOPREFIX, UIFONT_GDI);
	int nCurLen = rcCalc.GetWidth();

	m_pTextTotalCount->SetFixedWidth(nCurLen);
	m_pTextTotalCount->SetVisible(true);
	m_pTextTotalCount->SetText(szBuff);

	m_bTotalNetLess = false;
}

tstring CListLocalItem::GetFolderName()
{
	return m_strFolderName;
}

int CListLocalItem::GetFileCount()
{
	return m_nCount;
}

bool CListLocalItem::Select( bool bSelect /*= true*/ )
{
	__super::Select(bSelect);

	if ( m_pContainerIcon != NULL )
		m_pContainerIcon->Selected(bSelect);

	m_pContentBtn->SetTextColor(bSelect ? m_dwSelectedTextColor : m_dwNormalTextColor);
	m_pTextTotalCount->SetTextColor(bSelect ? m_dwSelectedCountColor : m_dwNormalCountColor);

	return true;
}