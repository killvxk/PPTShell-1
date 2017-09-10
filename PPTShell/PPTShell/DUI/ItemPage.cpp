#include "StdAfx.h"
#include "ItemPage.h"
#include "EventCenter/EventDefine.h"
#include "DUI/ItemExplorer.h"

namespace DuiLib
{
	const TCHAR* const kItemPageUIClassName = _T("ItemPageUI");
	const TCHAR* const kItemPageUIInterfaceName = _T("ItemPage");

	CItemPageUI::CItemPageUI()
	{
		m_nCurrentPageIndex = 0;
		m_nPerPageCount = 20;

	}

	CItemPageUI::~CItemPageUI()
	{

	}

	LPCTSTR CItemPageUI::GetClass() const
	{
		return kItemPageUIInterfaceName;
	}

	LPVOID CItemPageUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, kItemPageUIClassName) == 0 ) return static_cast<CItemPageUI*>(this);
		return CVerticalLayoutUI::GetInterface(pstrName);
	}

	void CItemPageUI::AddSrcData( ITEM_PAGE & itemPage)
	{
		m_vecItems.push_back(itemPage);
	}

	void CItemPageUI::Create(int nIndex)
	{
 		m_pMainLayout = CreateItemPage();
// 
 		m_pContentViewLayout = static_cast<CVerticalLayoutUI *>(m_pMainLayout->FindSubControl(_T("contentViewLayout")));
 		m_pContentPageLayout = static_cast<CVerticalLayoutUI *>(m_pMainLayout->FindSubControl(_T("contentPageLayout")));

		
		DWORD dwCount = ((nIndex+1) * m_nPerPageCount) > m_vecItems.size() ? m_vecItems.size() - (nIndex * m_nPerPageCount)  : m_nPerPageCount;
		vector<ITEM_PAGE>::iterator iter = m_vecItems.begin();

		for(int i = 0; i < dwCount; i++)
		{
			
			ITEM_PAGE ItemPage = *iter;
			if(m_OnCreateCallback)
			{
				TEventNotify Msg = {0};
				Msg.lParam = (LPARAM)&ItemPage;
				m_OnCreateCallback(&Msg);
				CVerticalLayoutUI* pItem = (CVerticalLayoutUI *)(Msg.wParam);
				
				m_pContentViewLayout->Add(pItem);
				if(i == 0 )
					pItem->SetPadding(CDuiRect(6, 0, 6, 0));
				else
					pItem->SetPadding(CDuiRect(6, 0, 6, 0));
			}
			iter += (nIndex * m_nPerPageCount + 1);
		}

		if(dwCount > 0)
		{
			CDialogBuilder dlgBuilder;
			CHorizontalLayoutUI* pPageLayout = static_cast<CHorizontalLayoutUI*>(dlgBuilder.Create(_T("RightBar\\Item\\CloudItemPage.xml"), (UINT)0, NULL, GetManager()));
			if(pPageLayout)
			{
				m_pContentPageLayout->Add(pPageLayout);

				TCHAR szBuff[128];
				InsertPageIndex( m_vecItems.size(), 0, 0, true );
				_stprintf_s(szBuff, _T("%d") , m_vecItems.size() );
				m_pContentPageLayout->SetUserData(szBuff);
			}
		}
	}

	void CItemPageUI::Clear()
	{
		m_vecItems.clear();
	}

	void CItemPageUI::InsertPageIndex( DWORD dwCount, int nStart, int nIndex, bool bInit )
	{
		CHorizontalLayoutUI* pPageIndex =  static_cast<CHorizontalLayoutUI*>(m_pContentPageLayout->FindSubControl(_T("pageIndex")));
		TCHAR szBuff[128];
		_stprintf_s(szBuff, _T("%d"), nStart);
		pPageIndex->SetUserData(szBuff);

		if(bInit)
		{
			CButtonUI* pButton = static_cast<CButtonUI*>(m_pContentPageLayout->FindSubControl(_T("firstBtn")));
			pButton->OnNotify += MakeDelegate(this, &CItemPageUI::OnBtnPageChange);
			pButton = static_cast<CButtonUI*>(m_pContentPageLayout->FindSubControl(_T("preBtn")));
			pButton->OnNotify += MakeDelegate(this, &CItemPageUI::OnBtnPageChange);
			pButton = static_cast<CButtonUI*>(m_pContentPageLayout->FindSubControl(_T("nextBtn")));
			pButton->OnNotify += MakeDelegate(this, &CItemPageUI::OnBtnPageChange);
			pButton = static_cast<CButtonUI*>(m_pContentPageLayout->FindSubControl(_T("lastBtn")));
			pButton->OnNotify += MakeDelegate(this, &CItemPageUI::OnBtnPageChange);
		}

		if(pPageIndex)
		{
			pPageIndex->RemoveAll();

			int nTotalCount ;
			if( dwCount % m_nPerPageCount == 0)
				nTotalCount = dwCount/m_nPerPageCount;
			else
				nTotalCount = dwCount/m_nPerPageCount + 1;
			//selected
			int nPageIndex = nStart/m_nPerPageCount + 1;

			COptionUI * pOption = new COptionUI;
			_stprintf_s(szBuff,_T("%d"), nPageIndex);
			pOption->SetText(szBuff);
			_stprintf_s(szBuff,_T("Group%d"), nPageIndex);
			pOption->SetGroup(szBuff);
			pOption->SetSelectedTextColor(0xFF11B0B6);
			pOption->SetTextColor(0xFF929292);
			pOption->SetFixedWidth(24);
			pOption->SetFixedHeight(24);
			pOption->SetBkImage(_T("file='RightBar\\Item\\btn_page_num.png' source='0,0,24,24'"));
			pOption->SetSelectedImage(_T("file='RightBar\\Item\\btn_page_num.png' source='24,0,48,24'"));
			pPageIndex->Add(pOption);
			pOption->OnNotify += MakeDelegate(this, &CItemPageUI::OnBtnPageChange);
			pOption->Selected(true);
		}
	}

	bool CItemPageUI::OnBtnPageChange( void* pNotify )
	{
		TNotifyUI* pNotifyUI = (TNotifyUI*)pNotify;

		if (pNotifyUI->sType == _T("click"))
		{
			int nPageIndex = -1;

			tstring strCount = m_pContentPageLayout->GetUserData(); //总数
			tstring strPageIndex;

			CHorizontalLayoutUI* pPageIndex =  static_cast<CHorizontalLayoutUI*>(m_pContentPageLayout->FindSubControl(_T("pageIndex")));
			tstring strStart = pPageIndex->GetUserData(); //当前数量
			int nStart = _ttoi(strStart.c_str());

			COptionUI * pOption = static_cast<COptionUI *>(pNotifyUI->pSender);
			int nTotalCount = _ttoi(strCount.c_str());

			tstring strName = pOption->GetName();
			if(strName == _T("firstBtn"))
			{
				nPageIndex = 1;
			}
			else if(strName == _T("preBtn"))
			{
				int nPage = (nStart / m_nPerPageCount) + 1;
				nPage--;
				nPageIndex = nPage;
				if(nPage <= 0)
					return true;
			}
			else if(strName == _T("nextBtn"))
			{
				int nPage = nStart / m_nPerPageCount + 1;
				nPage++;
				nPageIndex = nPage;

				int nCountPages = nTotalCount/m_nPerPageCount;
				if(nTotalCount % m_nPerPageCount != 0)
					nCountPages++;
				if(nPageIndex > nCountPages)
					return true;
			}
			else if(strName == _T("lastBtn"))
			{
				if(nTotalCount % m_nPerPageCount == 0)
					nPageIndex = nTotalCount/m_nPerPageCount;
				else
					nPageIndex = nTotalCount/m_nPerPageCount + 1;
			}
			else
			{
				strPageIndex = pOption->GetText();
				nPageIndex = _ttoi(strPageIndex.c_str());
			}

			if(nPageIndex == -1)
				return true;

			if(nStart == (nPageIndex - 1) * m_nPerPageCount)
				return true;

			nStart = (nPageIndex - 1) * m_nPerPageCount;
			int nEnd = nTotalCount - nStart;
			nEnd = nEnd > m_nPerPageCount ? m_nPerPageCount : nEnd;

			//m_pContentViewLayout->SetVisible(false);

			m_pContentViewLayout->SetTag((UINT_PTR)NULL);
			m_pContentViewLayout->RemoveAll();
			CItemExplorerUI::GetInstance()->ShowWindow(false);

			DWORD dwCount = (nPageIndex * m_nPerPageCount) > m_vecItems.size() ?  m_vecItems.size() - ((nPageIndex - 1) * m_nPerPageCount) : m_nPerPageCount;
			vector<ITEM_PAGE>::iterator iter = m_vecItems.begin();

			iter += ((nPageIndex - 1) * m_nPerPageCount);
			for(int i = 0; i < dwCount; i++)
			{
				ITEM_PAGE ItemPage = *iter;
				if(m_OnCreateCallback)
				{
					TEventNotify Msg = {0};
					Msg.lParam = (LPARAM)&ItemPage;
					m_OnCreateCallback(&Msg);
					CVerticalLayoutUI* pItem = (CVerticalLayoutUI *)(Msg.wParam);
					pItem->SetPadding(CDuiRect(10, 25, 10, 0));
					m_pContentViewLayout->Add(pItem);
				}
				iter++;
			}

			InsertPageIndex(m_vecItems.size() , nStart , 0 , false);

		}

		return true;
	}

	CVerticalLayoutUI* CItemPageUI::CreateItemPage()
	{
		CVerticalLayoutUI * pItem = NULL;

		if( !m_itemPageBuilder.GetMarkup()->IsValid() ) {
			pItem = static_cast<CVerticalLayoutUI*>(m_itemPageBuilder.Create(_T("RightBar\\Item\\ItemPage.xml"), (UINT)0, NULL, GetManager()));
		}
		else {
			pItem = static_cast<CVerticalLayoutUI*>(m_itemPageBuilder.Create(NULL, GetManager()));
		}
		if (pItem == NULL)
			return NULL;

		return pItem;
	}

	CVerticalLayoutUI* CItemPageUI::GetContentLayout()
	{
		return m_pContentViewLayout;
	}

}