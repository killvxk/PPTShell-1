#include "StdAfx.h"
#include "ListItem.h"
#include "EventCenter/EventDefine.h"
#include "Util/Util.h"

namespace DuiLib
{

	//// MenuUI
	const TCHAR* const kListItemUIClassName = _T("ListIconItemUI");
	const TCHAR* const kListItemUIInterfaceName = _T("ListIconItem");

	CListItemUI::CListItemUI()
	{
		m_pContainerIcon		= NULL;
		m_pTextCurCount			= NULL;
		m_pTextTotalCount		= NULL;
		m_pCountContainer		= NULL;
		m_pBtn					= NULL;
		m_pCurLoadGif			= NULL;
		m_pCurDBLoadGif			= NULL;

		m_bTotalNetLess			= false;
		m_bCurNetLess			= false;
		m_bModifyName			= true;
		m_nType					= 0;
		m_nCurCount				= 0;
		m_nTotalCount			= 0;

		m_dwGetCurCountId		= -1;

		m_strNormalIcon			= _T("");
		m_strHotIcon			= _T("");

		m_dwGetCurImageThreadId	= -1;
		m_dwGetCurVideoThreadId = -1;
		m_dwGetCurVolumeThreadId = -1;
		m_dwGetCurFlashThreadId = -1;
	

	}

	CListItemUI::~CListItemUI()
	{}

	LPCTSTR CListItemUI::GetClass() const
	{
		return kListItemUIClassName;
	}

	LPVOID CListItemUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, kListItemUIInterfaceName) == 0 ) return static_cast<CListItemUI*>(this);
		return CListContainerElementUI::GetInterface(pstrName);
	}

	void CListItemUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcsicmp(pstrName, _T("icon")) == 0 ) 
			m_strLogo = pstrValue;
		else if( _tcsicmp(pstrName, _T("content")) == 0 ) 
			m_strName = pstrValue;
		else if (_tcsicmp(pstrName, _T("textcolor")) == 0 )
		{
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			m_dwNormalColor  = clrColor;
		}
		else if (_tcsicmp(pstrName, _T("hottextcolor")) == 0 )
		{
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			m_dwHotColor  = clrColor;
		}
		else if (_tcsicmp(pstrName, _T("textselectedcolor")) == 0 )
		{
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			m_dwSelectedColor  = clrColor;
		}
		else if (_tcsicmp(pstrName, _T("selcounttextcolor")) == 0 )
		{
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			m_dwSelCountColor  = clrColor;
		}
		else if (_tcsicmp(pstrName, _T("type")) == 0 )
		{
			m_nType = _ttoi(pstrValue);
		}
		else if (_tcsicmp(pstrName, _T("modifyname")) == 0 )
		{
			if ( _tcsicmp(pstrValue, _T("true")) == 0 )
				m_bModifyName = true;
			else
				m_bModifyName = false;
		}


		CListContainerElementUI::SetAttribute(pstrName, pstrValue);
	}

	void CListItemUI::InitItem()
	{
		m_pContainerIcon	= static_cast<COptionUI*>(FindSubControl(_T("icon")));
		m_pBtn				= static_cast<CButtonUI*>(FindSubControl(_T("contentBtn")));

		m_pTextCurCount		= static_cast<CLabelUI*>(FindSubControl(_T("curCount")));
		m_pTextSeparator	= static_cast<CLabelUI*>(FindSubControl(_T("Separator")));
		m_pTextTotalCount	= static_cast<CLabelUI*>(FindSubControl(_T("totalCount")));

		if ( m_pContainerIcon != NULL )
		{
			m_strNormalIcon		= m_pContainerIcon->GetNormalImage();
			m_strHotIcon		= m_pContainerIcon->GetHotImage();
		}
	}

	void CListItemUI::SetFolderName( LPCTSTR strName )
	{
		if ( m_pBtn != NULL )
			m_pBtn->SetText(strName);
	}

	DuiLib::CDuiString CListItemUI::GetFolderName()
	{
		if ( m_pBtn != NULL )
			return m_pBtn->GetText();

		return _T("");
	}

	void CListItemUI::DoInit()
	{
		InitItem();
		if(m_nType > CloudFileSearch && m_nType < CloudFileTotal )
		{
			::OnEvent(EVT_SET_CHAPTER_GUID, MakeEventDelegate(this, &CListItemUI::OnChapterChanged));
			::OnEvent(EVT_REFRESH_GROUPEXPLORER, MakeEventDelegate(this, &CListItemUI::OnRefreshGroupExplorer));
			//m_pTextCurCount		= static_cast<CLabelUI*>(FindSubControl(_T("curCount")));
			if(m_pTextCurCount)
				m_pTextCurCount->SetText(_T("0"));

			m_pTextTotalCount	= static_cast<CLabelUI*>(FindSubControl(_T("totalCount")));
			if(m_pTextTotalCount)
				m_pTextTotalCount->SetVisible(false);

			m_pCountContainer	= static_cast<CHorizontalLayoutUI*>(FindSubControl(_T("CountContainer")));
			if(m_pCountContainer)
			{
				m_pTotalLoadGif		= new CGifAnimUI;
				m_pTotalLoadGif->SetBkImage(_T("RightBar\\loading.gif"));
				m_pTotalLoadGif->SetFixedWidth(12);
				m_pTotalLoadGif->SetFixedHeight(12);
				m_pTotalLoadGif->SetPadding(CDuiRect(0,6,0,0));
				m_pCountContainer->AddAt(m_pTotalLoadGif,3);

				m_pCurLoadGif		= new CGifAnimUI;
				m_pCurLoadGif->SetBkImage(_T("RightBar\\loading.gif"));
				//				m_pCurLoadGif->SetVisible(false);
				m_pCurLoadGif->SetFixedWidth(12);
				m_pCurLoadGif->SetFixedHeight(12);
				m_pCurLoadGif->SetPadding(CDuiRect(0,6,0,0));
				m_pCountContainer->AddAt(m_pCurLoadGif,1);
				m_pCurLoadGif->SetVisible(false);
				//m_pCurLoadGif->SetPos(CDuiRect(30,3,42,15));
				//m_pCurLoadGif->PlayGif();
			}

			GetTotalCountInterface();
		}
		else if( m_nType > DBankSearch && m_nType < DBankTotal )  //zcs
		{
			if(m_pTextCurCount)
				m_pTextCurCount->SetVisible(false);

			m_pCountContainer	= static_cast<CHorizontalLayoutUI*>(FindSubControl(_T("CountContainer")));
			if(m_pCountContainer)
			{
				m_pCurDBLoadGif		= new CGifAnimUI;
				m_pCurDBLoadGif->SetBkImage(_T("RightBar\\loading.gif"));
				m_pCurDBLoadGif->SetFixedWidth(12);
				m_pCurDBLoadGif->SetFixedHeight(12);
				m_pCurDBLoadGif->SetPadding(CDuiRect(0,6,0,0));
				m_pCountContainer->AddAt(m_pCurDBLoadGif,1);
				m_pCurDBLoadGif->SetVisible(false);
			}

			//GetTotalCountInterface();
		}
		

#ifndef DEVELOP_VERSION
		if(CloudFile3DResource == m_nType)
		{
			this->SetVisible(false);
		}
#endif

	}

	bool CListItemUI::Select( bool bSelect /* = true */ )
	{
		__super::Select(bSelect);
		if ( m_pContainerIcon != NULL )
			m_pContainerIcon->Selected(bSelect);
		if ( m_pBtn != NULL )
		{
			if ( m_bModifyName )
				m_pBtn->SetMouseEnabled(bSelect);

			m_pBtn->SetTextColor(bSelect ? m_dwSelectedColor : m_dwNormalColor);
		}

		if ( m_pTextCurCount != NULL )
			m_pTextCurCount->SetTextColor(bSelect ? m_dwSelCountColor : m_dwNormalColor);

		if ( m_pTextSeparator != NULL )
			m_pTextSeparator->SetTextColor(bSelect ? m_dwSelectedColor : m_dwNormalColor);

		if ( m_pTextTotalCount != NULL )
			m_pTextTotalCount->SetTextColor(bSelect ? m_dwSelectedColor : m_dwNormalColor);

		Invalidate();

		return true;
	}

	bool CListItemUI::OnGetTotalCount(void * pParam)
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

		if(m_pTextTotalCount)
		{
			TCHAR szBuff[128] = {0};
			_stprintf(szBuff,_T("%d"), nCount);
			CDuiRect rcCalc;
			rcCalc.right = 50;
			CalcText(GetManager()->GetPaintDC(), rcCalc, szBuff, m_pTextTotalCount->GetFont(), DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT|DT_NOPREFIX, UIFONT_GDI);
			int nLen = rcCalc.GetWidth();

			//m_pTextTotalCount->SetVisible(true);
			m_pTextTotalCount->SetText(szBuff);
			m_pTextTotalCount->SetFixedWidth(nLen);

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
			WritePrivateProfileString(_T("Data"), ListItemName[m_nType], szCount, strConfinFile.c_str());
		}
		return true;
	}

	void CListItemUI::SetType(int nType)
	{
		m_nType = nType;
	}

	int CListItemUI::GetType()
	{
		return m_nType;
	}


	bool CListItemUI::OnChapterChanged( void* pObj )
	{
		TEventNotify* pEventNotify = (TEventNotify*)pObj;
		CStream* pStream = (CStream*)pEventNotify->wParam;
		pStream->ResetCursor();

		tstring strGuid = pStream->ReadString();

		GetCurCountInterface(strGuid);
		return true;
	}

	bool CListItemUI::OnGetCurCount(void * pParam)
	{
		THttpNotify * pNotify = (THttpNotify*)pParam;

		if (m_dwGetCurCountId != pNotify->dwTaskId)
		{
			return true;
		}


		m_pCurLoadGif->StopGif();
		m_pCurLoadGif->SetVisible(false);


		if(pNotify->dwErrorCode != 0)
		{
			m_bCurNetLess = true;
			return true;
		}
		//m_pTotalLoadGif->SetVisible(false);

		int nCount = NDCloudDecodeCount(pNotify->pData, pNotify->nDataSize);

		if(m_pTextCurCount)
		{
			TCHAR szBuff[128] ={0};
			_stprintf_s(szBuff,_T("%d"), nCount);
			CDuiRect rcCalc;
			rcCalc.right = 50;
			CalcText(GetManager()->GetPaintDC(), rcCalc, szBuff, m_pTextCurCount->GetFont(), DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT|DT_NOPREFIX, UIFONT_GDI);
			int nCurLen = rcCalc.GetWidth();

			m_pTextCurCount->SetFixedWidth(nCurLen);
			m_pTextCurCount->SetVisible(true);
			m_pTextCurCount->SetText(szBuff);

			m_bCurNetLess = false;
		}

		m_dwGetCurCountId = -1;
		return true;
	};

	bool CListItemUI::OnRefreshGroupExplorer( void* pObj )
	{
		if(m_bCurNetLess == true)
		{
			GetCurCountInterface(NDCloudGetChapterGUID());
		}
		if(m_bTotalNetLess == true)
		{
			GetTotalCountInterface();
		}
		return true;
	}
	
	void CListItemUI::AddButtonClick()
	{
		//m_pBtn->OnNotify	+= MakeDelegate(this, &CListItemUI::OnBtnNotify);
	}

	void CListItemUI::DoEvent( TEventUI& event )
	{
		if( event.Type == UIEVENT_BUTTONDOWN )
		{
			if( IsEnabled() )
			{
				if(m_bCurNetLess == true)
				{
					GetCurCountInterface(NDCloudGetChapterGUID());
				}
				if(m_bTotalNetLess == true)
				{
					GetTotalCountInterface();
				}
				Select(true);
				Invalidate();
				m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMCLICK);
			}
			return;
		}
		else if ( event.Type == UIEVENT_RBUTTONDOWN )
		{
			
		}
		else if(event.Type == UIEVENT_RBUTTONUP)
		{
			if( IsEnabled() ){
				m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMRCLICK);
				Invalidate();
			}
		}

		// modify different status icon 、text color
		if (event.Type == UIEVENT_MOUSEENTER )
		{
			if ( m_pContainerIcon != NULL && !m_strHotIcon.IsEmpty())
			{
				m_pContainerIcon->SetNormalImage(m_strHotIcon);
			}

			if ( m_pBtn != NULL && !IsSelected() )
				m_pBtn->SetTextColor(m_dwHotColor);

			if ( m_pTextCurCount != NULL && !IsSelected() )
				m_pTextCurCount->SetTextColor(m_dwHotColor);

			if ( m_pTextSeparator != NULL && !IsSelected() )
				m_pTextSeparator->SetTextColor(m_dwHotColor);

			if ( m_pTextTotalCount != NULL && !IsSelected() )
				m_pTextTotalCount->SetTextColor(m_dwHotColor);
		}
		else if ( event.Type == UIEVENT_MOUSELEAVE && !m_strNormalIcon.IsEmpty() )
		{
			if ( m_pContainerIcon != NULL)
			{
				m_pContainerIcon->SetNormalImage(m_strNormalIcon);
			}

			if ( m_pBtn != NULL )
			{
				if ( IsSelected() )
					m_pBtn->SetTextColor(m_dwSelectedColor);
				else
					m_pBtn->SetTextColor(m_dwNormalColor);
			}

			if ( m_pTextCurCount  != NULL )
			{
				if ( IsSelected() )
					m_pTextCurCount->SetTextColor(m_dwSelCountColor);
				else
					m_pTextCurCount->SetTextColor(m_dwNormalColor);
			}

			if ( m_pTextTotalCount  != NULL )
			{
				if ( IsSelected() )
					m_pTextTotalCount->SetTextColor(m_dwSelectedColor);
				else
					m_pTextTotalCount->SetTextColor(m_dwNormalColor);
			}

			if ( m_pTextSeparator  != NULL )
			{
				if ( IsSelected() )
					m_pTextSeparator->SetTextColor(m_dwSelectedColor);
				else
					m_pTextSeparator->SetTextColor(m_dwNormalColor);
			}
		}
		__super::DoEvent(event);
	}

	void CListItemUI::GetTotalCountInterface()
	{
		if(m_pTotalLoadGif)
		{
			if(m_pTextTotalCount)
				m_pTextTotalCount->SetVisible(false);

			m_pTotalLoadGif->SetVisible(true);
			m_pTotalLoadGif->PlayGif();
		}


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
				GetPrivateProfileString(_T("Data"), ListItemName[m_nType], _T(""), szCount, MAX_PATH * 2, strConfinFile.c_str());
				if( _tcslen(szCount) > 0)
				{
					int nCount = _ttoi(szCount);

					m_pTotalLoadGif->StopGif();
					m_pTotalLoadGif->SetVisible(false);

					if(m_pTextTotalCount)
					{
						TCHAR szBuff[128] ={0};
						_stprintf_s(szBuff,_T("%d"), nCount);
						CDuiRect rcCalc;
						rcCalc.right = 50;
						CalcText(GetManager()->GetPaintDC(), rcCalc, szBuff, m_pTextTotalCount->GetFont(), DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT|DT_NOPREFIX, UIFONT_GDI);
						int nLen = rcCalc.GetWidth();

						//m_pTextTotalCount->SetVisible(true);
						m_pTextTotalCount->SetText(szBuff);
						m_pTextTotalCount->SetFixedWidth(nLen);

						m_bTotalNetLess = false;
					}

					return;
				}
			}

		}
		
		
		//
		tstring strUrl;
		DWORD dwTaskId = 0;
		switch(m_nType)
		{
		case CloudFileCourse:
			{
				strUrl = NDCloudComposeUrlCourseCount();
				dwTaskId =NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetTotalCount));
			}
			break;
		case CloudFileVideo:
			{
				strUrl = NDCloudComposeUrlVideoCount();
				dwTaskId =NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetTotalCount));
			}
			break;
		case CloudFileImage:
			{
				strUrl = NDCloudComposeUrlPictureCount();
				dwTaskId =NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetTotalCount));
			}
			break;
		case CloudFileFlash:
			{
				strUrl = NDCloudComposeUrlFlashCount();
				dwTaskId =NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetTotalCount));
			}
			break;
		case CloudFileVolume:
			{
				strUrl = NDCloudComposeUrlVolumeCount();
				dwTaskId =NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetTotalCount));
			}
			break;
		case CloudFileQuestion:
			{
				strUrl = NDCloudComposeUrlQuestionCount();
				dwTaskId =NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetTotalCount));
			}
			break;
		case CloudFileCoursewareObjects:
			{
				strUrl = NDCloudComposeUrlCoursewareObjectsCount();
				dwTaskId =NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetTotalCount));
			}
			break;
		case CloudFilePPTTemplate:
			{
				strUrl = NDCloudComposeUrlPPTTemplateCount();
				dwTaskId =NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetTotalCount));
			}
			break;
		case CloudFile3DResource:
			{
				strUrl = NDCloudComposeUrl3DResourceCount();
				dwTaskId =NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetTotalCount));
				NDCloudDownloadPriority(dwTaskId, HIGHEST_DOWNLOAD_PRIORITY);
			}
			break;
		case CloudFileVRResource:
			{
				strUrl = NDCloudComposeUrlVRResourceCount();
				dwTaskId =NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetTotalCount));
				NDCloudDownloadPriority(dwTaskId, HIGHEST_DOWNLOAD_PRIORITY);
			}
			break;
		case CloudFileAssets:
			{
				m_nGetTotalAssetsCount = 0;
				m_nGetTotalAssetsThreadCount = 0;
				GetCloudAssetsTotalCountInterface();
			}
		}

		//NDCloudDownloadPriority(dwTaskId, LOWEST_DOWNLOAD_PRIORITY);
	}

	void CListItemUI::GetCurCountInterface(tstring strGuid)
	{
		if(m_pCurLoadGif)
		{
			if(m_pTextCurCount)
				m_pTextCurCount->SetVisible(false);

// 			if(m_nType != CloudFilePPTTemplate)
// 			{
// 				m_pCurLoadGif->SetVisible(true);
// 				m_pCurLoadGif->PlayGif();
// 			}
			m_pCurLoadGif->SetVisible(true);
			m_pCurLoadGif->PlayGif();
		}

		if(m_dwGetCurCountId != -1)
		{
			NDCloudDownloadCancel(m_dwGetCurCountId);
		}

		tstring strUrl;
		switch(m_nType)
		{
		case CloudFileCourse:
			{
				strUrl = NDCloudComposeUrlCourseInfo(strGuid ,_T(""), 0, 1);
				m_dwGetCurCountId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetCurCount));
			}
			break;
		case CloudFileVideo:
			{
				strUrl = NDCloudComposeUrlVideoInfo(strGuid, _T(""), 0, 1);
				m_dwGetCurCountId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetCurCount));
			}
			break;
		case CloudFileImage:
			{
				strUrl = NDCloudComposeUrlPictureInfo(strGuid ,_T(""), 0, 1);
				m_dwGetCurCountId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetCurCount));
			}
			break;
		case CloudFileFlash:
			{
				strUrl = NDCloudComposeUrlFlashInfo(strGuid ,_T(""), 0, 1);
				m_dwGetCurCountId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetCurCount));
			}
			break;
		case CloudFileVolume:
			{
				strUrl = NDCloudComposeUrlVolumeInfo(strGuid ,_T(""), 0, 1);
				m_dwGetCurCountId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetCurCount));
			}
			break;
		case CloudFileQuestion:
			{
				strUrl = NDCloudComposeUrlQuestionInfo(strGuid ,_T(""), 0, 1);
				m_dwGetCurCountId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetCurCount));
			}
			break;
		case CloudFileCoursewareObjects:
			{
				strUrl = NDCloudComposeUrlCoursewareObjectsInfo(strGuid ,_T(""), 0, 1);
				m_dwGetCurCountId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetCurCount));
			}
			break;
		case CloudFilePPTTemplate:
			{
// 				strUrl = NDCloudComposeUrlPPTTemplateInfo(strGuid ,_T(""), 0, 1);
// 				m_dwGetCurCountId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetCurCount));
				CCategoryTree* pCategoryTree = NULL;
				NDCloudGetCategoryTree(pCategoryTree);
				if(pCategoryTree)
				{
					tstring strCourseCode = pCategoryTree->GetSelectedCourseCode();
					if(m_mapPPTTemplateStream.find(strCourseCode) == m_mapPPTTemplateStream.end())
					{
						strUrl = NDCloudComposeUrlPPTTemplateInfo(pCategoryTree->GetSelectedSectionCode(), pCategoryTree->GetSelectedCourseCode() ,_T(""), 0, 1);
						m_dwGetCurCountId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetCurCount));
						return ;
					}
					else
					{
						CStream* pStream = m_mapPPTTemplateStream[strCourseCode];
						pStream->ResetCursor();
						int nCount = pStream->ReadDWORD();
						if(m_pTextCurCount)
						{
							char szBuff[128] ={0};
							_stprintf(szBuff,_T("%d"), nCount);
							int nCurLen = _tcslen(szBuff);

							m_pTextCurCount->SetFixedWidth(nCurLen * 7);
							m_pTextCurCount->SetVisible(true);
							m_pTextCurCount->SetText(szBuff);

							m_bCurNetLess = false;
						}

						m_dwGetCurCountId = -1;
					}
				}
				
				m_pCurLoadGif->StopGif();
				m_pCurLoadGif->SetVisible(false);
			}
			break;
		case CloudFile3DResource:
			{
				strUrl = NDCloudComposeUrl3DResourceInfo(_T("") ,_T(""), 0, 1);
				m_dwGetCurCountId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetCurCount));
				NDCloudDownloadPriority(m_dwGetCurCountId, HIGHEST_DOWNLOAD_PRIORITY);
			}
			break;
		case CloudFileVRResource:
			{
				strUrl = NDCloudComposeUrlVRResourceInfo(_T("") ,_T(""), _T(""), 0, 1);
				m_dwGetCurCountId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetCurCount));
				NDCloudDownloadPriority(m_dwGetCurCountId, HIGHEST_DOWNLOAD_PRIORITY);
			}
			break;
		case CloudFileAssets:
			{
				if(m_dwGetCurImageThreadId != -1)
				{
					NDCloudDownloadCancel(m_dwGetCurImageThreadId);
				}
				if(m_dwGetCurVideoThreadId != -1)
				{
					NDCloudDownloadCancel(m_dwGetCurVideoThreadId);
				}
				if(m_dwGetCurVolumeThreadId != -1)
				{
					NDCloudDownloadCancel(m_dwGetCurVolumeThreadId);
				}
				if(m_dwGetCurFlashThreadId != -1)
				{
					NDCloudDownloadCancel(m_dwGetCurFlashThreadId);
				}

				m_nGetCurAssetsCount = 0;
				m_nGetCurAssetsThreadCount = 0;
				GetCloudAssetsCurCountInterface(strGuid);
			}
		}
	}

	void CListItemUI::SetPPTTemplateStream( CStream *pPPTTemplateStream )
	{
		CCategoryTree* pCategoryTree = NULL;
		NDCloudGetCategoryTree(pCategoryTree);
		if(pCategoryTree)
		{
			tstring strSectionCode = pCategoryTree->GetSelectedSectionCode();
			tstring strCourseCode = pCategoryTree->GetSelectedCourseCode();
			tstring strCode = strSectionCode+strCourseCode;
			if(m_mapPPTTemplateStream.find(strCode) == m_mapPPTTemplateStream.end())
			{
				m_mapPPTTemplateStream[strCode] = pPPTTemplateStream;
			}
		}
		
	}

	CStream* CListItemUI::GetPPTTemplateStream()
	{
		CCategoryTree* pCategoryTree = NULL;
		NDCloudGetCategoryTree(pCategoryTree);
		if(pCategoryTree)
		{
			tstring strSectionCode = pCategoryTree->GetSelectedSectionCode();
			tstring strCourseCode = pCategoryTree->GetSelectedCourseCode();
			tstring strCode = strSectionCode+strCourseCode;
			if(m_mapPPTTemplateStream.find(strCode) == m_mapPPTTemplateStream.end())
				return NULL;
			return m_mapPPTTemplateStream[strCode];
		}
		return NULL;
	}

	void  CListItemUI::CalcText( HDC hdc, RECT& rc, LPCTSTR lpszText, int nFontId, UINT format, UITYPE_FONT nFontType, int c)
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

	void CListItemUI::SetModuleIcon( tstring strNormalPath, tstring strHotPath, tstring strSelectPath )
	{
		if ( m_pContainerIcon != NULL )
		{
			m_strNormalIcon		= strNormalPath.c_str();
			m_strHotIcon		= strHotPath.c_str();

			m_pContainerIcon->SetHotImage(strHotPath.c_str());
			m_pContainerIcon->SetNormalImage(strNormalPath.c_str());
			m_pContainerIcon->SetSelectedImage(strSelectPath.c_str());
		}
	}

	void CListItemUI::SetFileCount( int nCount )
	{
		m_nCurCount = nCount;

		if ( m_pTextCurCount != NULL )
		{
			m_pTextCurCount->SetVisible(true);
			TCHAR szCount[24] = {0};
			wsprintf(szCount, _T("%d"), nCount);
			m_pTextCurCount->SetText(szCount);
		}
	}

	bool CListItemUI::OnGetCurDBCount( void* pParam )
	{
		THttpNotify * pNotify = (THttpNotify*)pParam;

		if (m_dwGetCurCountId != pNotify->dwTaskId)
		{
			return true;
		}
		
		if(m_pCurDBLoadGif)
		{
			m_pCurDBLoadGif->StopGif();
			m_pCurDBLoadGif->SetVisible(false);
		}

		int nCount = NDCloudDecodeCount(pNotify->pData, pNotify->nDataSize);

		SetFileCount(nCount);
		
		return true;
	}

	bool CListItemUI::OnGetCurDBExercisesCount( void* pParam )
	{
		THttpNotify * pNotify = (THttpNotify*)pParam;

		if (m_dwGetCurCountId != pNotify->dwTaskId)
		{
			return true;
		}

		if(m_pCurDBLoadGif)
		{
			m_pCurDBLoadGif->StopGif();
			m_pCurDBLoadGif->SetVisible(false);
		}

		int nCount = NDCloudDecodeExercisesCount(pNotify->pData, pNotify->nDataSize);

		SetFileCount(nCount);

		return true;
	}

	void CListItemUI::GetCurDBCountInterface( DWORD dwUserId )
	{
		if(m_pCurDBLoadGif)
		{
			if(m_pTextCurCount)
				m_pTextCurCount->SetVisible(false);

			m_pCurDBLoadGif->SetVisible(true);
			m_pCurDBLoadGif->PlayGif();
		}

		if(m_dwGetCurCountId != -1)
		{
			NDCloudDownloadCancel(m_dwGetCurCountId);
		}

		tstring strUrl;
		switch(m_nType)
		{
		case DBankCourse:
			{
				strUrl = NDCloudComposeUrlCourseInfo(_T(""), _T(""), 0, 500, dwUserId);
				m_dwGetCurCountId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetCurDBCount));
			}
			break;
		case DBankVideo:
			{
				strUrl = NDCloudComposeUrlVideoInfo(_T(""), _T(""), 0, 500, dwUserId);
				m_dwGetCurCountId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetCurDBCount));
			}
			break;
		case DBankImage:
			{
				strUrl = NDCloudComposeUrlPictureInfo(_T(""), _T(""), 0, 500, dwUserId);
				m_dwGetCurCountId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetCurDBCount));
			}
			break;
		case DBankFlash:
			{
				strUrl = NDCloudComposeUrlFlashInfo(_T(""), _T(""), 0, 500, dwUserId);
				m_dwGetCurCountId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetCurDBCount));
			}
			break;
		case DBankVolume:
			{
				strUrl = NDCloudComposeUrlVolumeInfo(_T(""), _T(""), 0, 500, dwUserId);
				m_dwGetCurCountId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetCurDBCount));
			}
			break;
		case DBankQuestion:
			{
				strUrl = NDCloudComposeUrlQuestionInfo(_T(""), _T(""), 0, 500, dwUserId);
				m_dwGetCurCountId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetCurDBExercisesCount));
			}
			break;
		case DBankCoursewareObjects:
			{
				strUrl = NDCloudComposeUrlCoursewareObjectsInfo(_T(""), _T(""), 0, 500, dwUserId);
				m_dwGetCurCountId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetCurDBExercisesCount));
			}
			break;
		case DBankAssets:
			{
				if(m_dwGetCurImageThreadId != -1)
				{
					NDCloudDownloadCancel(m_dwGetCurImageThreadId);
				}
				if(m_dwGetCurVideoThreadId != -1)
				{
					NDCloudDownloadCancel(m_dwGetCurVideoThreadId);
				}
				if(m_dwGetCurVolumeThreadId != -1)
				{
					NDCloudDownloadCancel(m_dwGetCurVolumeThreadId);
				}
				if(m_dwGetCurFlashThreadId != -1)
				{
					NDCloudDownloadCancel(m_dwGetCurFlashThreadId);
				}

				m_nGetCurAssetsCount = 0;
				m_nGetCurAssetsThreadCount = 0;
				GetDBankAssetsCurCountInterface(dwUserId);
			}
			break;
		}

	}

	void CListItemUI::GetCloudAssetsCurCountInterface(tstring strGuid)
	{
		tstring strUrl = NDCloudComposeUrlVideoInfo(strGuid, _T(""), 0, 1);
		m_dwGetCurVideoThreadId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetCloudAssetsCurCount));

		strUrl = NDCloudComposeUrlPictureInfo(strGuid, _T(""), 0, 1);
		m_dwGetCurImageThreadId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetCloudAssetsCurCount));

		strUrl = NDCloudComposeUrlVolumeInfo(strGuid, _T(""), 0, 1);
		m_dwGetCurVolumeThreadId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetCloudAssetsCurCount));

		strUrl = NDCloudComposeUrlFlashInfo(strGuid, _T(""), 0, 1);
		m_dwGetCurFlashThreadId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetCloudAssetsCurCount));
	}

	void CListItemUI::GetCloudAssetsTotalCountInterface()
	{
		tstring strUrl = NDCloudComposeUrlVideoCount();
		DWORD dwThreadId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetCloudAssetsTotalCount));

		strUrl = NDCloudComposeUrlPictureCount();
		dwThreadId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetCloudAssetsTotalCount));

		strUrl = NDCloudComposeUrlVolumeCount();
		dwThreadId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetCloudAssetsTotalCount));

		strUrl = NDCloudComposeUrlFlashCount();
		dwThreadId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetCloudAssetsTotalCount));
	}

	bool CListItemUI::OnGetCloudAssetsCurCount( void * pParam )
	{
		m_nGetCurAssetsThreadCount++;

		THttpNotify * pNotify = (THttpNotify*)pParam;

		if(m_nGetCurAssetsThreadCount == 4)
		{
			m_pCurLoadGif->StopGif();
			m_pCurLoadGif->SetVisible(false);
		}

		if(pNotify->dwErrorCode != 0)
		{
			m_bCurNetLess = true;
			return true;
		}

		int nCount = NDCloudDecodeCount(pNotify->pData, pNotify->nDataSize);

		m_nGetCurAssetsCount += nCount;
		if(m_pTextCurCount && m_nGetCurAssetsThreadCount == 4)
		{
			

			TCHAR szBuff[128] ={0};
			_stprintf(szBuff,_T("%d"), m_nGetCurAssetsCount);
			CDuiRect rcCalc;
			rcCalc.right = 50;
			CalcText(GetManager()->GetPaintDC(), rcCalc, szBuff, m_pTextCurCount->GetFont(), DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT|DT_NOPREFIX, UIFONT_GDI);
			int nCurLen = rcCalc.GetWidth();

			m_pTextCurCount->SetFixedWidth(nCurLen);
			m_pTextCurCount->SetVisible(true);
			m_pTextCurCount->SetText(szBuff);

			m_bCurNetLess = false;
		}

		return true;
	}

	bool CListItemUI::OnGetDBankAssetsCurCount( void * pParam )
	{
		m_nGetCurAssetsThreadCount++;

		THttpNotify * pNotify = (THttpNotify*)pParam;

		if(m_nGetCurAssetsThreadCount == 4)
		{
			m_pCurDBLoadGif->StopGif();
			m_pCurDBLoadGif->SetVisible(false);
		}

		if(pNotify->dwErrorCode != 0)
		{
			m_bCurNetLess = true;
			return true;
		}

		int nCount = NDCloudDecodeCount(pNotify->pData, pNotify->nDataSize);

		m_nGetCurAssetsCount += nCount;
		if(m_pTextCurCount && m_nGetCurAssetsThreadCount == 4)
		{


			TCHAR szBuff[128] ={0};
			_stprintf(szBuff,_T("%d"), m_nGetCurAssetsCount);
			CDuiRect rcCalc;
			rcCalc.right = 50;
			CalcText(GetManager()->GetPaintDC(), rcCalc, szBuff, m_pTextCurCount->GetFont(), DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT|DT_NOPREFIX, UIFONT_GDI);
			int nCurLen = rcCalc.GetWidth();

			m_pTextCurCount->SetFixedWidth(nCurLen);
			m_pTextCurCount->SetVisible(true);
			m_pTextCurCount->SetText(szBuff);

			m_bCurNetLess = false;
		}

		return true;
	}

	bool CListItemUI::OnGetCloudAssetsTotalCount( void * pParam )
	{
		m_nGetTotalAssetsThreadCount++;

		THttpNotify * pNotify = (THttpNotify*)pParam;


		if(m_nGetTotalAssetsThreadCount == 4)
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
		m_nGetTotalAssetsCount += nCount;
		if(m_pTextTotalCount && m_nGetTotalAssetsThreadCount == 4)
		{
			TCHAR szBuff[128] ={0};
			_stprintf(szBuff,_T("%d"), m_nGetTotalAssetsCount);
			CDuiRect rcCalc;
			rcCalc.right = 50;
			CalcText(GetManager()->GetPaintDC(), rcCalc, szBuff, m_pTextTotalCount->GetFont(), DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT|DT_NOPREFIX, UIFONT_GDI);
			int nLen = rcCalc.GetWidth();

			//m_pTextTotalCount->SetVisible(true);
			m_pTextTotalCount->SetText(szBuff);
			m_pTextTotalCount->SetFixedWidth(nLen);

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
			_stprintf(szCount, _T("%d"), m_nGetTotalAssetsCount);
			WritePrivateProfileString(_T("Data"), ListItemName[m_nType], szCount, strConfinFile.c_str());
		}
		return true;
	}

	void CListItemUI::GetDBankAssetsCurCountInterface( DWORD dwUserId )
	{
		tstring strUrl = NDCloudComposeUrlVideoInfo(_T(""), _T(""), 0, 500, dwUserId);
		m_dwGetCurVideoThreadId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetDBankAssetsCurCount));

		strUrl = NDCloudComposeUrlPictureInfo(_T(""), _T(""), 0, 500, dwUserId);
		m_dwGetCurImageThreadId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetDBankAssetsCurCount));
	
		strUrl = NDCloudComposeUrlFlashInfo(_T(""), _T(""), 0, 500, dwUserId);
		m_dwGetCurFlashThreadId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetDBankAssetsCurCount));

		strUrl = NDCloudComposeUrlVolumeInfo(_T(""), _T(""), 0, 500, dwUserId);
		m_dwGetCurVolumeThreadId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CListItemUI::OnGetDBankAssetsCurCount));
	}
}


