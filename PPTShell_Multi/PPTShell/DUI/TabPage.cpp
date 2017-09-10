#include "StdAfx.h"
#include "TabPage.h"
#include "DUI/GroupExplorer.h"
#include "DUI/ItemExplorer.h"
#include "NDCloud/NDCloudUser.h"
#include "ResourceItemEx.h"

namespace DuiLib
{
	const TCHAR* const kTabLayoutPageUIClassName = _T("TabLayoutPageUI");
	const TCHAR* const kTabLayoutPageUIInterfaceName = _T("TabLayoutPage");

	CTabLayoutPageUI::CTabLayoutPageUI()
	{
		m_dwDownloadId = -1;
	}

	CTabLayoutPageUI::~CTabLayoutPageUI()
	{
		
	}

	LPCTSTR CTabLayoutPageUI::GetClass() const
	{
		return kTabLayoutPageUIClassName;
	}

	LPVOID CTabLayoutPageUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, kTabLayoutPageUIInterfaceName) == 0 ) return static_cast<CTabLayoutPageUI*>(this);
		return CVerticalLayoutUI::GetInterface(pstrName);
	}

	bool CTabLayoutPageUI::SetContent(int nIndex,CStream & stream, DWORD &dwRetCount,bool bInit, bool bDBank)
	{
		SetType(nIndex + 1);

		bool bRet = false;

		if(bInit)
		{
			SetSimpleContent(nIndex, stream, bDBank);
		}


		CHorizontalLayoutUI * pHorizontalLayout = NULL;
		CVerticalLayoutUI * pVerticalLayout = NULL;
		//增加到完整列表
		DWORD dwCount = stream.ReadDWORD();
		if(dwCount > 0 && bInit)
		{
			CDialogBuilder dlgBuilder;
			m_vecContentPageLayout[nIndex]->SetVisible(true);
			m_vecContentPageLayout[nIndex]->RemoveAll();
			CHorizontalLayoutUI* pPageLayout = static_cast<CHorizontalLayoutUI*>(dlgBuilder.Create(_T("RightBar\\Item\\CloudItemPage.xml"), (UINT)0, NULL, GetManager()));
			if(pPageLayout)
			{
				m_vecContentPageLayout[nIndex]->Add(pPageLayout);
			}
		}
		m_vecContentLayout[nIndex]->SetDelayedDestroy(false);
		m_vecContentLayout[nIndex]->RemoveAll();
		CScrollBarUI* pScrollBar = m_vecContentLayout[nIndex]->GetVerticalScrollBar();
		if(pScrollBar)
			pScrollBar->SetScrollPos(0);

		int nType = m_nType;
		for(int i= 0 ; i< (int)dwCount; i++)
		{
			if(i % 2 == 0)
			{
				pVerticalLayout = new CVerticalLayoutUI;
				pVerticalLayout->SetDelayedDestroy(false);
				pHorizontalLayout = new CHorizontalLayoutUI;
				pHorizontalLayout->SetDelayedDestroy(false);
				pHorizontalLayout->SetChildPadding(20);
				pVerticalLayout->SetPadding(CDuiRect(0,10,0,0));
				pVerticalLayout->Add(pHorizontalLayout);
				pVerticalLayout->SetFixedHeight(130);
				m_vecContentLayout[nIndex]->Add(pVerticalLayout);
			}
			
			if(m_nType == CloudFileCourse)
			{
				int nCourseType = stream.ReadDWORD();
				if(nCourseType == 1)
				{
					nType = CloudFileNdpCourse;
				}
				else if(nCourseType == 2)
				{
					nType = CloudFileNdpxCourse;
				}
			}
			
			//20160127
			CContainerUI* pContainer = new CContainerUI;
// 			if(m_nType == CloudFileNdpCourse)
// 			{
// 				CCloudItemUI * pItem = static_cast<CCloudItemUI *>(CreateCloudItem());
// 				pItem->ReadStream(&stream);
// 				pItem->NeedTypeIcon(true);
// 
// 				pContainer->Add(pItem);
// 
// 				pHorizontalLayout->Add(pContainer);
// 
// 				if(nIndex != TabLayout_Type_Course)
// 				{
// 					CContainerUI*pEmpty= dynamic_cast<CContainerUI*>(pItem->FindSubControl(_T("empty")));
// 					pEmpty->OnNotify	+= MakeDelegate(this, &CTabLayoutPageUI::OnItemSelect);
// 				}
// 
// 				pContainer->SetFixedWidth(150);
// 				pItem->SetContentHeight(110);
// 			}
// 			else
			{
				if(bDBank)
				{
					switch(nType)
					{
					case CloudFileCourse:
						nType = DBankCourse;
						break;
					case CloudFileVideo:
						nType = DBankVideo;
						break;
					case CloudFileImage:
						nType = DBankImage;
						break;
					case CloudFileFlash:
						nType = DBankFlash;
						break;
					case CloudFileVolume:
						nType = DBankVolume;
						break;
					case CloudFileNdpCourse:
						nType = DBankNdpCourse;
						break;
					case CloudFileNdpxCourse:
						nType = DBankNdpxCourse;
						break;
					}
				}
				
				CResourceItemExUI* pItem = CResourceItemExUI::AllocResourceItem(nType);
				pItem->ReadStream(&stream);
// 				tstring strGuid = pItem->GetHandler()->GetHandlerId();
// 				strGuid += _T("_s");
// 				pItem->GetHandler()->SetHandlerId(strGuid.c_str());
				//pItem->NeedTypeIcon(true);
				pContainer->Add(pItem);

				pHorizontalLayout->Add(pContainer);

				pContainer->SetFixedWidth(150);
			}

		}

		//
		TCHAR szBuff[128];
	//	dwCount = stream.ReadDWORD();
		dwRetCount = stream.ReadDWORD();

		DWORD dwStart = stream.ReadDWORD();
		if(dwCount > 0)
		{
			InsertPageIndex(dwRetCount , dwStart , nIndex , bInit);
			_stprintf_s(szBuff, _T("%d") , dwRetCount );
			m_vecContentPageLayout[nIndex]->SetUserData(szBuff);
		}
		else
		{
			m_vecContentPageLayout[nIndex]->RemoveAll();
		}
		_stprintf_s(szBuff, _T("{c #FFF700}%d{/c} 个资源") , dwRetCount );
		m_vecSimpleCountTextLabel[nIndex]->SetText(szBuff);

		return bRet;
	}

	void CTabLayoutPageUI::SetSimpleContent(int nIndex,CStream & stream, bool bDBank)
	{
		DWORD dwCount = stream.ReadDWORD();

		//简易列表
		CVerticalLayoutUI * pSearchSimpleView= m_vecSimpleContentViewLayout[nIndex];

		if(dwCount > 0)
		{
			pSearchSimpleView->SetVisible(true);
		}
		else
		{
			pSearchSimpleView->SetVisible(false);
		}

		dwCount = dwCount > Search_QuickShow_Count ? Search_QuickShow_Count : dwCount;

		CVerticalLayoutUI * pSearchSimpleContent = m_vecSimpleContentLayout[nIndex];
		pSearchSimpleContent->SetDelayedDestroy(false);
		pSearchSimpleContent->RemoveAll();
		CScrollBarUI* pScrollBar = m_vecContentViewLayout[m_vecContentViewLayout.size() - 1]->GetVerticalScrollBar();
		if(pScrollBar)
		{
			pScrollBar->SetScrollPos(0);
		}
		CHorizontalLayoutUI * pHorizontalLayout = NULL;
		CVerticalLayoutUI * pVerticalLayout = NULL;

		int n = 0;
		int nType = m_nType;
		for(int i= 0 ; i< (int)dwCount; i++)
		{
			if(i % 2 == 0)
			{
				n++;
				pSearchSimpleView->SetFixedHeight(21 + n*150);

				pVerticalLayout = new CVerticalLayoutUI;
				pVerticalLayout->SetDelayedDestroy(false);
				pHorizontalLayout = new CHorizontalLayoutUI;
				pHorizontalLayout->SetDelayedDestroy(false);
				pHorizontalLayout->SetChildPadding(20);
				pVerticalLayout->SetPadding(CDuiRect(0,10,0,0));
				pVerticalLayout->Add(pHorizontalLayout);
				pVerticalLayout->SetFixedHeight(130);
				pSearchSimpleContent->Add(pVerticalLayout);
			}

			if(m_nType == CloudFileCourse)
			{
				int nCourseType = stream.ReadDWORD();
				if(nCourseType == 1)
				{
					nType = CloudFileNdpCourse;
				}
				else if(nCourseType == 2)
				{
					nType = CloudFileNdpxCourse;
				}
			}

			//20160127
			CContainerUI* pContainer = new CContainerUI;
// 			if(m_nType == CloudFileNdpCourse)
// 			{
// 				CCloudItemUI * pItem = static_cast<CCloudItemUI *>(CreateCloudItem());
// 				pItem->ReadStream(&stream);
// 				pItem->NeedTypeIcon(true);
// 
// 				pContainer->Add(pItem);
// 
// 				pHorizontalLayout->Add(pContainer);
// 
// 				if(nIndex != TabLayout_Type_Course)
// 				{
// 					CContainerUI*pEmpty= dynamic_cast<CContainerUI*>(pItem->FindSubControl(_T("empty")));
// 					pEmpty->OnNotify	+= MakeDelegate(this, &CTabLayoutPageUI::OnItemSelect);
// 				}
// 
// 				pContainer->SetFixedWidth(150);
// 				pItem->SetContentHeight(110);
// 			}
// 			else
			{
				if(bDBank)
				{
					switch(nType)
					{
					case CloudFileCourse:
						nType = DBankCourse;
						break;
					case CloudFileVideo:
						nType = DBankVideo;
						break;
					case CloudFileImage:
						nType = DBankImage;
						break;
					case CloudFileFlash:
						nType = DBankFlash;
						break;
					case CloudFileVolume:
						nType = DBankVolume;
						break;
					case CloudFileNdpCourse:
						nType = DBankNdpCourse;
						break;
					case CloudFileNdpxCourse:
						nType = DBankNdpxCourse;
						break;
					}
				}

				CResourceItemExUI* pItem = CResourceItemExUI::AllocResourceItem(nType);
				pItem->ReadStream(&stream);
// 				tstring strGuid = pItem->GetHandler()->GetHandlerId();
// 				strGuid += _T("_ss");
// 				pItem->GetHandler()->SetHandlerId(strGuid.c_str());

				//pItem->NeedTypeIcon(true);
				pContainer->Add(pItem);

				pHorizontalLayout->Add(pContainer);

				pContainer->SetFixedWidth(150);
			}
			
			
		}

		stream.ResetCursor();
	}

	void	CTabLayoutPageUI::InsertPageIndex(DWORD dwCount, int nStart, int nIndex, bool bInit)//dwCount 总量 nStart 起始
	{
		CHorizontalLayoutUI* pPageIndex =  static_cast<CHorizontalLayoutUI*>(m_vecContentPageLayout[nIndex]->FindSubControl(_T("pageIndex")));
		TCHAR szBuff[128];
		_stprintf_s(szBuff, _T("%d"), nStart);
		pPageIndex->SetUserData(szBuff);

		if(bInit)
		{
			CButtonUI* pButton = static_cast<CButtonUI*>(m_vecContentPageLayout[nIndex]->FindSubControl(_T("firstBtn")));
			pButton->OnNotify += MakeDelegate(this, &CTabLayoutPageUI::OnBtnPageChange);
			pButton = static_cast<CButtonUI*>(m_vecContentPageLayout[nIndex]->FindSubControl(_T("preBtn")));
			pButton->OnNotify += MakeDelegate(this, &CTabLayoutPageUI::OnBtnPageChange);
			pButton = static_cast<CButtonUI*>(m_vecContentPageLayout[nIndex]->FindSubControl(_T("nextBtn")));
			pButton->OnNotify += MakeDelegate(this, &CTabLayoutPageUI::OnBtnPageChange);
			pButton = static_cast<CButtonUI*>(m_vecContentPageLayout[nIndex]->FindSubControl(_T("lastBtn")));
			pButton->OnNotify += MakeDelegate(this, &CTabLayoutPageUI::OnBtnPageChange);
		}

		if(pPageIndex)
		{
			if(bInit)
			{
				pPageIndex->RemoveAll();
				int nPer = dwCount/Search_Page_Count + 1;
				nPer = nPer > Search_MaxPage_Count ? Search_MaxPage_Count : nPer;

				pPageIndex->SetFixedWidth((nPer-1) * 5 + nPer * 24);
				TCHAR szBuff[128];
				for(int i =0 ;i < nPer ; i++)
				{
					COptionUI * pOption = new COptionUI;
					_stprintf_s(szBuff,_T("%d"), i+1);
					pOption->SetText(szBuff);
					_stprintf_s(szBuff,_T("Group%d"), nIndex);
					pOption->SetGroup(szBuff);
					pOption->SetSelectedTextColor(0xFF11B0B6);
					pOption->SetTextColor(0xFF929292);
					pOption->SetFixedWidth(24);
					pOption->SetFixedHeight(24);
					pOption->SetBkImage(_T("file='RightBar\\Item\\btn_page_num.png' source='0,0,24,24'"));
					pOption->SetSelectedImage(_T("file='RightBar\\Item\\btn_page_num.png' source='24,0,48,24'"));
					pPageIndex->Add(pOption);
					pOption->OnNotify += MakeDelegate(this, &CTabLayoutPageUI::OnBtnPageChange);
					if(i == 0)
						pOption->Selected(true);
				}
			}
			else
			{
				int nTotalCount ;
				if( dwCount % Search_Page_Count == 0)
					nTotalCount = dwCount/Search_Page_Count;
				else
					nTotalCount = dwCount/Search_Page_Count + 1;
				//selected
				int nPageIndex = nStart/Search_Page_Count + 1;

				//
				if(nTotalCount <= Search_MaxPage_Count)
				{
					int nPageIndexTmp;
					tstring strPageIndexTmp;

					int nOptCount = pPageIndex->GetCount();
					COptionUI *pOption;
					for(int i=0;i<nOptCount;i++)
					{
						pOption = static_cast<COptionUI*>(pPageIndex->GetItemAt(i));
						strPageIndexTmp = pOption->GetText();
						nPageIndexTmp = _ttoi(strPageIndexTmp.c_str());
						if(nPageIndexTmp == nPageIndex)
						{
							pOption->Selected(true);
							break;
						}
					}
					return ;
				}

				pPageIndex->RemoveAll();


				int n = nPageIndex;
				n = (n - 4) >=0 ? n - 2 : 1; 
				if(n >= 2)
				{
					n = (nTotalCount - n) >= 4 ? n : nTotalCount - 4  ; 
				}
				for ( int i = n; i < n + 5 ; i++ )
				{
					pPageIndex->SetFixedWidth((5-1) * 5 + 5 * 24);
					TCHAR szBuff[128];
	
					COptionUI * pOption = new COptionUI;
					_stprintf_s(szBuff,_T("%d"), i);
					pOption->SetText(szBuff);
					_stprintf_s(szBuff,_T("Group%d"), nIndex);
					pOption->SetGroup(szBuff);
					pOption->SetSelectedTextColor(0xFF11B0B6);
					pOption->SetTextColor(0xFF929292);
					pOption->SetFixedWidth(24);
					pOption->SetFixedHeight(24);
					pOption->SetBkImage(_T("file='RightBar\\Item\\btn_page_num.png' source='0,0,24,24'"));
					pOption->SetSelectedImage(_T("file='RightBar\\Item\\btn_page_num.png' source='24,0,48,24'"));
					pPageIndex->Add(pOption);
					pOption->OnNotify += MakeDelegate(this, &CTabLayoutPageUI::OnBtnPageChange);
					if(i == nPageIndex)
						pOption->Selected(true);
				}
			}
		}
	}

	CResourceItemUI* CTabLayoutPageUI::CreateCloudItem()
	{
		CResourceItemUI * pItem = NULL;

		if( !m_cloudItemBuilder.GetMarkup()->IsValid() ) {
			pItem = dynamic_cast<CResourceItemUI*>(m_cloudItemBuilder.Create(_T("RightBar\\Item\\ResourceItem.xml"), (UINT)0, &m_callback, GetManager()));
		}
		else {
			pItem = dynamic_cast<CResourceItemUI*>(m_cloudItemBuilder.Create(&m_callback, GetManager()));
		}
		if (pItem == NULL)
			return NULL;
		return pItem;
	}

	void CTabLayoutPageUI::SelectOption(int nIndex)
	{
		m_vecTabLayoutOpt[nIndex]->Selected(true);
	}

	void CTabLayoutPageUI::SelectOption(tstring strName)
	{
		for(int i = 0 ; i< (int)m_vecTabLayoutOpt.size() ; i++)
		{
			if(_tcsicmp(m_vecTabLayoutOpt[i]->GetText() , strName.c_str() ) == 0)
			{
				m_vecTabLayoutOpt[i]->Selected(true);
				break;
			}
		}
	}

	void CTabLayoutPageUI::SelectContentLayout(int nIndex)
	{
		m_pSearchTabLayout->SelectItem(nIndex);
	}

	void CTabLayoutPageUI::SelectContentLayout(tstring strName)
	{
		for(int i = 0 ; i< (int)m_vecContentViewLayout.size() ; i++)
		{
			if(_tcsicmp(m_vecContentViewLayout[i]->GetName() , strName.c_str()) == 0 )
			{
				m_pSearchTabLayout->SelectItem(i);
				break;
			}
		}
	}

	void CTabLayoutPageUI::Init()
	{
		CDialogBuilder builder;
		CContainerUI* pGuideGrade = static_cast<CContainerUI*>(builder.Create(_T("RightBar\\SearchTab.xml"), (UINT)0, NULL));
		if( pGuideGrade ) 
		{
			this->Add(pGuideGrade);

			CVerticalLayoutUI* pSearchTabLayoutBtn =  static_cast<CVerticalLayoutUI*>(this->FindSubControl(_T("searchTabLayoutBtn")));

			//////////////////////////////////////////////////////////////////////////
			COptionUI * pOption = NULL;
			pOption =  static_cast<COptionUI*>(pSearchTabLayoutBtn->FindSubControl(_T("searchBtnCourse")));
			pOption->OnNotify += MakeDelegate(this, &CTabLayoutPageUI::OnOptSearch);
			m_vecTabLayoutOpt.push_back(pOption);
			pOption =  static_cast<COptionUI*>(pSearchTabLayoutBtn->FindSubControl(_T("searchBtnVideo")));
			pOption->OnNotify += MakeDelegate(this, &CTabLayoutPageUI::OnOptSearch);
			m_vecTabLayoutOpt.push_back(pOption);
			pOption =  static_cast<COptionUI*>(pSearchTabLayoutBtn->FindSubControl(_T("searchBtnPic")));
			pOption->OnNotify += MakeDelegate(this, &CTabLayoutPageUI::OnOptSearch);
			m_vecTabLayoutOpt.push_back(pOption);
			pOption =  static_cast<COptionUI*>(pSearchTabLayoutBtn->FindSubControl(_T("searchBtnFlash")));
			pOption->OnNotify += MakeDelegate(this, &CTabLayoutPageUI::OnOptSearch);
			m_vecTabLayoutOpt.push_back(pOption);
			pOption =  static_cast<COptionUI*>(pSearchTabLayoutBtn->FindSubControl(_T("searchBtnVolume")));
			pOption->OnNotify += MakeDelegate(this, &CTabLayoutPageUI::OnOptSearch);
			m_vecTabLayoutOpt.push_back(pOption);
			pOption =  static_cast<COptionUI*>(pSearchTabLayoutBtn->FindSubControl(_T("searchBtnAll")));
			pOption->OnNotify += MakeDelegate(this, &CTabLayoutPageUI::OnOptSearch);
			m_vecTabLayoutOpt.push_back(pOption);

			//////////////////////////////////////////////////////////////////////////
			m_pSearchTabLayout = static_cast<CTabLayoutUI*>(this->FindSubControl(_T("searchTabView")));

			//////////////////////////////////////////////////////////////////////////
			CVerticalLayoutUI * pLayout = NULL;

			pLayout = static_cast<CVerticalLayoutUI*>(m_pSearchTabLayout->FindSubControl(_T("searchTabCourse")));
			m_vecContentViewLayout.push_back(pLayout);
			pLayout = static_cast<CVerticalLayoutUI*>(m_pSearchTabLayout->FindSubControl(_T("searchTabVideo")));
			m_vecContentViewLayout.push_back(pLayout);
			pLayout = static_cast<CVerticalLayoutUI*>(m_pSearchTabLayout->FindSubControl(_T("searchTabPic")));
			m_vecContentViewLayout.push_back(pLayout);
			pLayout = static_cast<CVerticalLayoutUI*>(m_pSearchTabLayout->FindSubControl(_T("searchTabFlash")));
			m_vecContentViewLayout.push_back(pLayout);
			pLayout = static_cast<CVerticalLayoutUI*>(m_pSearchTabLayout->FindSubControl(_T("searchTabVolume")));
			m_vecContentViewLayout.push_back(pLayout);

			//////////////////////////////////////////////////////////////////////////
			CButtonUI * pButton = NULL;
			pLayout = static_cast<CVerticalLayoutUI*>(m_pSearchTabLayout->FindSubControl(_T("searchTabAll")));
			m_vecContentViewLayout.push_back(pLayout);
			CVerticalLayoutUI * pSubLayout = static_cast<CVerticalLayoutUI*>(pLayout->FindSubControl(_T("searchSimpleCourseView")));
			m_vecSimpleContentViewLayout.push_back(pSubLayout);
			pButton =  static_cast<CButtonUI*>(pSubLayout->FindSubControl(_T("showAllBtn")));
			pButton->OnNotify += MakeDelegate(this, &CTabLayoutPageUI::OnBtnShowAll);
			pSubLayout = static_cast<CVerticalLayoutUI*>(pSubLayout->FindSubControl(_T("searchSimpleCourseContent")));
			m_vecSimpleContentLayout.push_back(pSubLayout);

			pSubLayout = static_cast<CVerticalLayoutUI*>(pLayout->FindSubControl(_T("searchSimpleVideoView")));
			m_vecSimpleContentViewLayout.push_back(pSubLayout);
			pButton =  static_cast<CButtonUI*>(pSubLayout->FindSubControl(_T("showAllBtn")));
			pButton->OnNotify += MakeDelegate(this, &CTabLayoutPageUI::OnBtnShowAll);
			pSubLayout = static_cast<CVerticalLayoutUI*>(pSubLayout->FindSubControl(_T("searchSimpleVideoContent")));
			m_vecSimpleContentLayout.push_back(pSubLayout);

			pSubLayout = static_cast<CVerticalLayoutUI*>(pLayout->FindSubControl(_T("searchSimplePicView")));
			m_vecSimpleContentViewLayout.push_back(pSubLayout);
			pButton =  static_cast<CButtonUI*>(pSubLayout->FindSubControl(_T("showAllBtn")));
			pButton->OnNotify += MakeDelegate(this, &CTabLayoutPageUI::OnBtnShowAll);
			pSubLayout = static_cast<CVerticalLayoutUI*>(pSubLayout->FindSubControl(_T("searchSimplePicContent")));
			m_vecSimpleContentLayout.push_back(pSubLayout);

			pSubLayout = static_cast<CVerticalLayoutUI*>(pLayout->FindSubControl(_T("searchSimpleFlashView")));
			m_vecSimpleContentViewLayout.push_back(pSubLayout);
			pButton =  static_cast<CButtonUI*>(pSubLayout->FindSubControl(_T("showAllBtn")));
			pButton->OnNotify += MakeDelegate(this, &CTabLayoutPageUI::OnBtnShowAll);
			pSubLayout = static_cast<CVerticalLayoutUI*>(pSubLayout->FindSubControl(_T("searchSimpleFlashContent")));
			m_vecSimpleContentLayout.push_back(pSubLayout);

			pSubLayout = static_cast<CVerticalLayoutUI*>(pLayout->FindSubControl(_T("searchSimpleVolumeView")));
			m_vecSimpleContentViewLayout.push_back(pSubLayout);
			pButton =  static_cast<CButtonUI*>(pSubLayout->FindSubControl(_T("showAllBtn")));
			pButton->OnNotify += MakeDelegate(this, &CTabLayoutPageUI::OnBtnShowAll);
			pSubLayout = static_cast<CVerticalLayoutUI*>(pSubLayout->FindSubControl(_T("searchSimpleVolumeContent")));
			m_vecSimpleContentLayout.push_back(pSubLayout);

			//////////////////////////////////////////////////////////////////////////

			pLayout = static_cast<CVerticalLayoutUI*>(this->FindSubControl(_T("searchCourseContent")));
			m_vecContentLayout.push_back(pLayout);
			pLayout = static_cast<CVerticalLayoutUI*>(this->FindSubControl(_T("searchVideoContent")));
			m_vecContentLayout.push_back(pLayout);
			pLayout = static_cast<CVerticalLayoutUI*>(this->FindSubControl(_T("searchPicContent")));
			m_vecContentLayout.push_back(pLayout);
			pLayout = static_cast<CVerticalLayoutUI*>(this->FindSubControl(_T("searchFlashContent")));
			m_vecContentLayout.push_back(pLayout);
			pLayout = static_cast<CVerticalLayoutUI*>(this->FindSubControl(_T("searchVolumeContent")));
			m_vecContentLayout.push_back(pLayout);

			//////////////////////////////////////////////////////////////////////////
			pLayout = static_cast<CVerticalLayoutUI*>(this->FindSubControl(_T("searchCoursePage")));
			m_vecContentPageLayout.push_back(pLayout);
			pLayout = static_cast<CVerticalLayoutUI*>(this->FindSubControl(_T("searchVideoPage")));
			m_vecContentPageLayout.push_back(pLayout);
			pLayout = static_cast<CVerticalLayoutUI*>(this->FindSubControl(_T("searchPicPage")));
			m_vecContentPageLayout.push_back(pLayout);
			pLayout = static_cast<CVerticalLayoutUI*>(this->FindSubControl(_T("searchFlashPage")));
			m_vecContentPageLayout.push_back(pLayout);
			pLayout = static_cast<CVerticalLayoutUI*>(this->FindSubControl(_T("searchVolumePage")));
			m_vecContentPageLayout.push_back(pLayout);

			//////////////////////////////////////////////////////////////////////////
			CLabelUI * pLabel = static_cast<CLabelUI*>(this->FindSubControl(_T("simpleCourseCountText")));
			m_vecSimpleCountTextLabel.push_back(pLabel);
			pLabel = static_cast<CLabelUI*>(this->FindSubControl(_T("simpleVideoCountText")));
			m_vecSimpleCountTextLabel.push_back(pLabel);
			pLabel = static_cast<CLabelUI*>(this->FindSubControl(_T("simplePicCountText")));
			m_vecSimpleCountTextLabel.push_back(pLabel);
			pLabel = static_cast<CLabelUI*>(this->FindSubControl(_T("simpleFlashCountText")));
			m_vecSimpleCountTextLabel.push_back(pLabel);
			pLabel = static_cast<CLabelUI*>(this->FindSubControl(_T("simpleVolumeCountText")));
			m_vecSimpleCountTextLabel.push_back(pLabel);
		}
		else 
		{
			this->RemoveAll();
			return;
		}
	}

	void CTabLayoutPageUI::SetKeyword(tstring &strKeyword)
	{
		m_strKeyword = strKeyword;
	}

	bool CTabLayoutPageUI::OnOptSearch(void* pNotify)
	{
		TNotifyUI* pNotifyUI = (TNotifyUI*)pNotify;

		if (pNotifyUI->sType == _T("click"))
		{
			COptionUI * pOption = static_cast<COptionUI *>(pNotifyUI->pSender);
			if(!pOption->IsSelected())
			{
				tstring strName = pOption->GetName();
				if(strName == _T("searchBtnAll"))
					m_pSearchTabLayout->SelectItem(0);
				else if(strName == _T("searchBtnCourse"))
					m_pSearchTabLayout->SelectItem(1);
				else if(strName == _T("searchBtnVideo"))
					m_pSearchTabLayout->SelectItem(2);
				else if(strName == _T("searchBtnPic"))
					m_pSearchTabLayout->SelectItem(3);
				else if(strName == _T("searchBtnFlash"))
					m_pSearchTabLayout->SelectItem(4);
				else if(strName == _T("searchBtnVolume"))
					m_pSearchTabLayout->SelectItem(5);

				if(strName != _T("searchBtnCourse"))
				{
					CItemExplorerUI::GetInstance()->ShowWindow(false);
				}
			}
		}
		
		return true;
	}

	bool CTabLayoutPageUI::OnBtnShowAll(void* pNotify)
	{
		TNotifyUI* pNotifyUI = (TNotifyUI*)pNotify;

		if (pNotifyUI->sType == _T("click"))
		{
			tstring strFlag = pNotifyUI->pSender->GetUserData();

			COptionUI* pOption = NULL;

			if( strFlag == _T("course") )
			{
				pOption = m_vecTabLayoutOpt[TabLayout_Type_Course];
				pOption->Selected(true);
				m_pSearchTabLayout->SelectItem(1);
			}
			else if( strFlag == _T("video") )
			{
				pOption = m_vecTabLayoutOpt[TabLayout_Type_Video];
				pOption->Selected(true);
				m_pSearchTabLayout->SelectItem(2);
			}
			else if( strFlag == _T("pic") )
			{
				pOption = m_vecTabLayoutOpt[TabLayout_Type_Pic];
				pOption->Selected(true);
				m_pSearchTabLayout->SelectItem(3);
			}
			else if( strFlag == _T("flash") )
			{
				pOption = m_vecTabLayoutOpt[TabLayout_Type_Flash];
				pOption->Selected(true);
				m_pSearchTabLayout->SelectItem(4);
			}
			else if( strFlag == _T("volume") )
			{
				pOption = m_vecTabLayoutOpt[TabLayout_Type_Volume];
				pOption->Selected(true);
				m_pSearchTabLayout->SelectItem(5);
			}

			CItemExplorerUI::GetInstance()->ShowWindow(false);
		}

		return true;
	}

	bool CTabLayoutPageUI::OnItemSelect(void* pNotify)
	{
		TNotifyUI* pNotifyUI = (TNotifyUI*)pNotify;

		if (pNotifyUI->sType == _T("setfocus"))
		{
			CItemExplorerUI::GetInstance()->ShowWindow(false);
		}
	
		return true;
	}

	bool CTabLayoutPageUI::OnBtnPageChange(void* pNotify)
	{
		TNotifyUI* pNotifyUI = (TNotifyUI*)pNotify;

		if (pNotifyUI->sType == _T("click"))
		{
			int nCurSelected = m_pSearchTabLayout->GetCurSel();
			int nPageIndex = -1;

			tstring strCount = m_vecContentPageLayout[nCurSelected - 1]->GetUserData(); //总数
			tstring strPageIndex;

			CHorizontalLayoutUI* pPageIndex =  static_cast<CHorizontalLayoutUI*>(m_vecContentPageLayout[nCurSelected - 1]->FindSubControl(_T("pageIndex")));
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
				int nPage = (nStart / Search_Page_Count) + 1;
				nPage--;
				nPageIndex = nPage;
				if(nPage <= 0)
					return true;
			}
			else if(strName == _T("nextBtn"))
			{
				int nPage = nStart / Search_Page_Count + 1;
				nPage++;
				nPageIndex = nPage;
				if(nPageIndex > nTotalCount/Search_Page_Count + 1)
					return true;
			}
			else if(strName == _T("lastBtn"))
			{
				if(nTotalCount % Search_Page_Count == 0)
					nPageIndex = nTotalCount/Search_Page_Count;
				else
					nPageIndex = nTotalCount/Search_Page_Count + 1;
			}
			else
			{
				strPageIndex = pOption->GetText();
				nPageIndex = _ttoi(strPageIndex.c_str());
			}

			if(nPageIndex == -1)
				return true;

			if(nStart == (nPageIndex - 1) * Search_Page_Count)
				return true;

			nStart = (nPageIndex - 1) * Search_Page_Count;
			int nEnd = nTotalCount - nStart;
			nEnd = nEnd > Search_Page_Count ? Search_Page_Count : nEnd;

			DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();
			tstring strUrl;
			switch(nCurSelected - 1)
			{
			case TabLayout_Type_Course:
				{
					if(m_nResType == CloudFileSearch)
						strUrl = NDCloudComposeUrlCourseInfo(_T(""), m_strKeyword, nStart, nEnd);
					else if(m_nResType == DBankSearch)
						strUrl = NDCloudComposeUrlCourseInfo(_T(""), m_strKeyword, nStart, nEnd, dwUserId);
				}
				break;
			case TabLayout_Type_Video:
				{
					if(m_nResType == CloudFileSearch)
						strUrl = NDCloudComposeUrlVideoInfo(_T(""), m_strKeyword, nStart, nEnd);
					else if(m_nResType == DBankSearch)
						strUrl = NDCloudComposeUrlVideoInfo(_T(""), m_strKeyword, nStart, nEnd, dwUserId);
				}
				break;
			case TabLayout_Type_Pic:
				{
					if(m_nResType == CloudFileSearch)
						strUrl = NDCloudComposeUrlPictureInfo(_T(""), m_strKeyword, nStart, nEnd);
					else if(m_nResType == DBankSearch)
						strUrl = NDCloudComposeUrlPictureInfo(_T(""), m_strKeyword, nStart, nEnd, dwUserId);
				}	
				break;
			case TabLayout_Type_Flash:
				{
					if(m_nResType == CloudFileSearch)
						strUrl = NDCloudComposeUrlFlashInfo(_T(""), m_strKeyword, nStart, nEnd);
					else if(m_nResType == DBankSearch)
						strUrl = NDCloudComposeUrlFlashInfo(_T(""), m_strKeyword, nStart, nEnd, dwUserId);
				}
				break;
			case TabLayout_Type_Volume:
				{
					if(m_nResType == CloudFileSearch)
						strUrl = NDCloudComposeUrlVolumeInfo(_T(""), m_strKeyword, nStart, nEnd);
					else if(m_nResType == CloudFileSearch)
						strUrl = NDCloudComposeUrlVolumeInfo(_T(""), m_strKeyword, nStart, nEnd, dwUserId);
				}
				break;
			}
			m_dwDownloadId = NDCloudDownload(strUrl,MakeHttpDelegate(this,&CTabLayoutPageUI::OnGetCloudRes));
		}

		return true;
	}

	bool CTabLayoutPageUI::OnGetCloudRes(void * pParam)
	{
		THttpNotify* pHttpNotify = (THttpNotify*)pParam;

		if (pHttpNotify->dwErrorCode > 0)
		{
			return true;
		}
		if (m_dwDownloadId != pHttpNotify->dwTaskId)
		{
			return true;
		}

		int nCurSelected = m_pSearchTabLayout->GetCurSel();

		THttpNotify * pNotify = static_cast<THttpNotify *>(pParam);

		CStream stream(1024);

		BOOL bRet;
		switch(nCurSelected - 1)
		{
		case TabLayout_Type_Course:
			bRet = NDCloudDecodeCourseList(pNotify->pData, pNotify->nDataSize ,&stream);
			break;
		case TabLayout_Type_Video:
			bRet = NDCloudDecodeVideoList(pNotify->pData, pNotify->nDataSize ,&stream);
			break;
		case TabLayout_Type_Pic:
			bRet = NDCloudDecodeImageList(pNotify->pData, pNotify->nDataSize ,&stream);
			break;
		case TabLayout_Type_Flash:
			bRet = NDCloudDecodeFlashList(pNotify->pData, pNotify->nDataSize ,&stream);
			break;
		case TabLayout_Type_Volume:
			bRet = NDCloudDecodeVolumeList(pNotify->pData, pNotify->nDataSize ,&stream);
			break;
		}


		DWORD dwCount = 0;
		if(bRet)
		{
			SetContent(nCurSelected - 1, stream, dwCount, false);
		}

		return true;	
	}

	void CTabLayoutPageUI::SetType(int nType)
	{
		m_nType = nType;
		m_callback.SetType(nType);
	}

	void CTabLayoutPageUI::SelectItem( int nIndex )
	{
		m_pSearchTabLayout->SelectItem(nIndex);

		int nType = 0;
		switch(nIndex)
		{
		case 0:
			nType = TabLayout_Type_All - 1;
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			nType = nIndex + 1;
			break;
		}
		COptionUI* pOption = m_vecTabLayoutOpt[nType];
		pOption->Selected(true);
	}

	void CTabLayoutPageUI::HideSearchTab()
	{
		m_pSearchTabLayout->SetVisible(false);
	}

	void CTabLayoutPageUI::ShowSearchTab()
	{
		m_pSearchTabLayout->SetVisible(true);
	}

	void CTabLayoutPageUI::SetResType( int nResType )
	{
		m_nResType = nResType;
	}

	int CTabLayoutPageUI::GetResType( )
	{
		return m_nResType;
	}
	//////////////////////////////////////////////////////////////////////////
	const TCHAR* const kTabLayoutLocalPageUIClassName = _T("TabLayoutLocalPageUI");
	const TCHAR* const kTabLayoutLocalPageUIInterfaceName = _T("TabLayoutLocalPage");

	CTabLayoutLocalPageUI::CTabLayoutLocalPageUI()
	{
		
	}

	CTabLayoutLocalPageUI::~CTabLayoutLocalPageUI()
	{

	}

	LPCTSTR CTabLayoutLocalPageUI::GetClass() const
	{
		return kTabLayoutLocalPageUIClassName;
	}

	LPVOID CTabLayoutLocalPageUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, kTabLayoutLocalPageUIInterfaceName) == 0 ) return static_cast<CTabLayoutLocalPageUI*>(this);
		return CVerticalLayoutUI::GetInterface(pstrName);
	}

	bool CTabLayoutLocalPageUI::OnBtnPageChange(void* pNotify)
	{
		TNotifyUI* pNotifyUI = (TNotifyUI*)pNotify;

		if (pNotifyUI->sType == _T("click"))
		{
			int nCurSelected = m_pSearchTabLayout->GetCurSel();
			int nPageIndex = -1;

			tstring strCount = m_vecContentPageLayout[nCurSelected - 1]->GetUserData(); //总数
			tstring strPageIndex;

			CHorizontalLayoutUI* pPageIndex =  static_cast<CHorizontalLayoutUI*>(m_vecContentPageLayout[nCurSelected - 1]->FindSubControl(_T("pageIndex")));
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
				int nPage ;
				if( nTotalCount % Search_Page_Count == 0)
					nPage = nTotalCount/Search_Page_Count;
				else
					nPage = nTotalCount/Search_Page_Count + 1;

				nPage--;
				nPageIndex = nPage;
				if(nPage <= 0)
					return true;
			}
			else if(strName == _T("nextBtn"))
			{
				int nPage = nStart / Search_Page_Count + 1;
				nPage++;
				nPageIndex = nPage;
				if(nPageIndex > nTotalCount/Search_Page_Count + 1)
					return true;
			}
			else if(strName == _T("lastBtn"))
			{
				if(nTotalCount % Search_Page_Count == 0)
					nPageIndex = nTotalCount/Search_Page_Count;
				else
					nPageIndex = nTotalCount/Search_Page_Count + 1;
			}
			else
			{
				strPageIndex = pOption->GetText();
				nPageIndex = _ttoi(strPageIndex.c_str());
			}

			if(nPageIndex == -1)
				return true;

			if(nStart == (nPageIndex - 1) * Search_Page_Count)
				return true;

			nStart = (nPageIndex - 1) * Search_Page_Count;
			int nEnd = nTotalCount - nStart;
			nEnd = nEnd > Search_Page_Count ? Search_Page_Count : nEnd;

			CStream stream;
			switch(nCurSelected - 1)
			{
			case TabLayout_Type_Course:
				stream = m_streamCourse;
				break;
			case TabLayout_Type_Video:
				stream = m_streamVideo;
				break;
			case TabLayout_Type_Pic:
				stream = m_streamPic;
				break;
			case TabLayout_Type_Flash:
				stream = m_streamFlash;
				break;
			case TabLayout_Type_Volume:
				stream = m_streamVolume;
				break;
			}
			SetType(nCurSelected - 1);

			DWORD dwCount = stream.ReadDWORD();

			m_vecContentLayout[nCurSelected - 1]->RemoveAll();

			CHorizontalLayoutUI * pHorizontalLayout = NULL;
			CVerticalLayoutUI * pVerticalLayout = NULL;
			for(int i= 0 ; i< (int)dwCount; i++)
			{
				if( i >= nStart && i < i+nEnd)
				{
					if(i % 2 == 0)
					{
						pVerticalLayout = new CVerticalLayoutUI;
						pHorizontalLayout = new CHorizontalLayoutUI;
						pHorizontalLayout->SetName(_T("tabpageHorizontalLayout"));
						pHorizontalLayout->SetChildPadding(20);
						pVerticalLayout->SetPadding(CDuiRect(0,10,0,0));
						pVerticalLayout->Add(pHorizontalLayout);
						pVerticalLayout->SetFixedHeight(130);
						m_vecContentLayout[nCurSelected - 1]->Add(pVerticalLayout);
					}

					CResourceItemUI * pItem = static_cast<CResourceItemUI *>(CreateLocalItem());
// 					int nType;
// 					switch(nCurSelected - 1)
// 					{
// 					case TabLayout_Type_Course:
// 						nType = RES_TYPE_COURSE;
// 						break;
// 					case TabLayout_Type_Video:
// 						nType = RES_TYPE_VIDEO;
// 						break;
// 					case TabLayout_Type_Pic:
// 						nType = RES_TYPE_PIC;
// 						break;
// 					case TabLayout_Type_Flash:
// 						nType = RES_TYPE_FLASH;
// 						break;
// 					case TabLayout_Type_Volume:
// 						nType = RES_TYPE_VOLUME;
// 						break;
// 					}
					pItem->SetResource(stream.ReadString().c_str());
					pItem->SetTitle(stream.ReadString().c_str());

					pItem->SetGroup(_T("Locals"));

					pHorizontalLayout->Add(pItem);
					pItem->SetFixedWidth(150);
					pItem->SetContentHeight(110);
				}
				else
				{
					stream.ReadString();
					stream.ReadString();
				}
			}

			InsertPageIndex(dwCount , nStart , nCurSelected - 1 , false);

			stream.ResetCursor();
		}

		return true;
	}

	bool CTabLayoutLocalPageUI::SetContent(int nIndex, CStream & stream, DWORD &dwRetCount, bool bInit)
	{
		SetType(nIndex);

		switch(nIndex)
		{
		case TabLayout_Type_Course:
			m_streamCourse = stream;
			break;
		case TabLayout_Type_Video:
			m_streamVideo = stream;
			break;
		case TabLayout_Type_Pic:
			m_streamPic = stream;
			break;
		case TabLayout_Type_Flash:
			m_streamFlash = stream;
			break;
		case TabLayout_Type_Volume:
			m_streamVolume = stream;
			break;
		}

		bool bRet = false;

		if(bInit)
		{
			SetSimpleContent(nIndex, stream);
		}


		CHorizontalLayoutUI * pHorizontalLayout = NULL;
		CVerticalLayoutUI * pVerticalLayout = NULL;
		//增加到完整列表
		DWORD dwCount = stream.ReadDWORD();//总量
		if(dwCount > 0 && bInit)
		{
			CDialogBuilder dlgBuilder;
			m_vecContentPageLayout[nIndex]->SetVisible(true);
			m_vecContentPageLayout[nIndex]->RemoveAll();
			CHorizontalLayoutUI* pPageLayout = static_cast<CHorizontalLayoutUI*>(dlgBuilder.Create(_T("RightBar\\Item\\CloudItemPage.xml"), (UINT)0, NULL, GetManager()));
			if(pPageLayout)
			{
				m_vecContentPageLayout[nIndex]->Add(pPageLayout);
			}
		}

		DWORD dwStart = 0;//总量
		DWORD dwCount1 = dwCount - dwStart > Search_Page_Count ? Search_Page_Count : dwCount - dwStart;
		m_vecContentLayout[nIndex]->RemoveAll();
		for(int i= 0 ; i< (int)dwCount; i++)
		{
			if( i >= (int)dwStart && i < (int)(i+dwCount1))
			{
				if(i % 2 == 0)
				{
					pVerticalLayout = new CVerticalLayoutUI;
					pHorizontalLayout = new CHorizontalLayoutUI;
					pHorizontalLayout->SetChildPadding(20);
					pVerticalLayout->SetPadding(CDuiRect(0,10,0,0));
					pVerticalLayout->Add(pHorizontalLayout);
					pVerticalLayout->SetFixedHeight(145);
					m_vecContentLayout[nIndex]->Add(pVerticalLayout);
				}

				CLocalItemUI * pItem = static_cast<CLocalItemUI *>(CreateLocalItem());

				CContainerUI* pContainer = new CContainerUI;
				pContainer->Add(pItem);
				
				pItem->SetResource(stream.ReadString().c_str());
				pItem->SetTitle(stream.ReadString().c_str());
				pItem->SetChapter(stream.ReadString().c_str());
				pItem->SetGroup(_T("Locals"));

				pHorizontalLayout->Add(pContainer);
				pContainer->SetFixedWidth(150);
				pItem->SetContentHeight(110);
			}
			else
			{
				stream.ReadString();
				stream.ReadString();
				stream.ReadString();
			}
		}

		//
		TCHAR szBuff[128];

		if(dwCount > 0)
		{
			InsertPageIndex(dwCount , dwStart , nIndex , bInit);
			_stprintf_s(szBuff, _T("%d") , dwCount );
			m_vecContentPageLayout[nIndex]->SetUserData(szBuff);
		}
		else
		{
			m_vecContentPageLayout[nIndex]->RemoveAll();
		}
		_stprintf_s(szBuff, _T("{c #FFF700}%d{/c} 个资源") , dwCount );
		m_vecSimpleCountTextLabel[nIndex]->SetText(szBuff);

		return bRet;
	}

	void CTabLayoutLocalPageUI::SetSimpleContent(int nIndex,CStream & stream)
	{
		SetType(nIndex);

		DWORD dwCount = stream.ReadDWORD();
		//简易列表
		CVerticalLayoutUI * pSearchSimpleView= m_vecSimpleContentViewLayout[nIndex];

		if(dwCount > 0)
		{
			pSearchSimpleView->SetVisible(true);
		}
		else
		{
			pSearchSimpleView->SetVisible(false);
		}

		dwCount = dwCount > Search_QuickShow_Count ? Search_QuickShow_Count : dwCount;

		CVerticalLayoutUI * pSearchSimpleContent = m_vecSimpleContentLayout[nIndex];
		pSearchSimpleContent->RemoveAll();
		CHorizontalLayoutUI * pHorizontalLayout = NULL;
		CVerticalLayoutUI * pVerticalLayout = NULL;

		int n = 0;
		for(int i= 0 ; i< (int)dwCount; i++)
		{
			if(i % 2 == 0)
			{
				n++;
				pSearchSimpleView->SetFixedHeight(36 + n*165);

				pVerticalLayout = new CVerticalLayoutUI;
				pHorizontalLayout = new CHorizontalLayoutUI;
				pHorizontalLayout->SetChildPadding(20);
				pVerticalLayout->SetPadding(CDuiRect(0,10,0,0));
				pVerticalLayout->Add(pHorizontalLayout);
				pVerticalLayout->SetFixedHeight(145);
				pSearchSimpleContent->Add(pVerticalLayout);
			}

			CLocalItemUI * pItem = dynamic_cast<CLocalItemUI *>(CreateLocalItem());

			CContainerUI* pContainer = new CContainerUI;
			pContainer->Add(pItem);

			pItem->SetResource(stream.ReadString().c_str());
			pItem->SetTitle(stream.ReadString().c_str());
			pItem->SetChapter(stream.ReadString().c_str());
			pItem->SetGroup(_T("Locals"));

			pHorizontalLayout->Add(pContainer);
			pContainer->SetFixedWidth(150);
			pItem->SetContentHeight(110);
		}

		stream.ResetCursor();
	}

	CLocalItemUI* CTabLayoutLocalPageUI::CreateLocalItem()
	{
		CLocalItemUI * pItem = NULL;

		if( !m_cloudItemBuilder.GetMarkup()->IsValid() ) {
			pItem = dynamic_cast<CLocalItemUI*>(m_cloudItemBuilder.Create(_T("RightBar\\Item\\LocalItem.xml"), (UINT)0, &m_callback, GetManager()));
		}
		else {
			pItem = dynamic_cast<CLocalItemUI*>(m_cloudItemBuilder.Create(&m_callback, GetManager()));
		}
		if (pItem == NULL)
			return NULL;

		return pItem;
	}
}

