#include "stdafx.h"
#include "SearchWindowUI.h"
#include "Util/Util.h"
#include "DUI/ItemExplorer.h"
#include "DUI/GroupExplorer.h"
#include "NDCloud/NDCloudAPI.h"
#include "Http/HttpDownload.h"
#include "NDCloud/NDCloudUser.h"
#include "Util/Util.h"
#include "WebCrawler/WebSearch.h"

CSearchWindowUI::CSearchWindowUI()
{
	m_pSearchEdit		= NULL;
	m_dwCheckBoxFlag	= 0;

	m_SearchTipListDlg.Create(CGroupExplorerUI::GetInstance()->GetHWND(), _T("SearchTipListDlg"), WS_POPUP, WS_EX_TOOLWINDOW , 0, 0, 0, 0);
	m_SearchTipListDlg.SetKillFocusCallBack(MakeDelegate(this, &CSearchWindowUI::HideTipListDlg));
	m_SearchTipListDlg.SetSearchCallBack(MakeDelegate(this, &CSearchWindowUI::SelectSearchType));
	m_bShowTipListDlg	= true;
	m_dwCheckBoxFlag	= CheckBox_ALL;
	m_bRequestingMore	= false;
	m_dwSearchType		= Search_Type_101PPT;
	m_hThread			= NULL;
}

CSearchWindowUI::~CSearchWindowUI()
{
	
}

CControlUI* CSearchWindowUI::CreateControl( LPCTSTR pstrClass )
{
	return NULL;
}

void CSearchWindowUI::Init()
{
	__super::Init();
	m_pSearchEdit = dynamic_cast<CEditClearUI*>(FindSubControl(_T("searchEdit")));
	m_pSearchEdit->SetHandleSpecialKeydown(true);

	m_pTipLayout		= dynamic_cast<CHorizontalLayoutUI*>(FindSubControl(_T("tipLayout")));
	m_pTipLabel			= dynamic_cast<CLabelUI*>(FindSubControl(_T("tipLabel")));
	m_pTipTriangle		= dynamic_cast<CLabelUI*>(FindSubControl(_T("triangleLabel")));
	m_pKeyWordLabel		= dynamic_cast<CLabelUI*>(FindSubControl(_T("labelKeyWord")));
	m_pSearchCountLabel = dynamic_cast<CLabelUI*>(FindSubControl(_T("labelSearchCount")));

	m_pTipLayout->OnEvent += MakeDelegate(this, &CSearchWindowUI::OnBtnTipEvent);
	m_pTipLabel->OnEvent += MakeDelegate(this, &CSearchWindowUI::OnBtnTipEvent);
	m_pTipTriangle->OnEvent += MakeDelegate(this, &CSearchWindowUI::OnBtnTipEvent);

	m_pSearchBtn = dynamic_cast<CButtonUI*>(FindSubControl(_T("searchBtn")));
	m_pSearchBtn->OnNotify += MakeDelegate(this, &CSearchWindowUI::OnBtnSearchNotify);

	m_pSearchHistoryLayout	= dynamic_cast<CVerticalLayoutUI*>(FindSubControl("searchhistory"));
	m_pSearchLoadingLayout	= dynamic_cast<CVerticalLayoutUI*>(FindSubControl("searchLoading"));
	m_pSearchContentLayout	= dynamic_cast<CVerticalLayoutUI*>(FindSubControl("searchContent"));
	m_pNetlessLayout		= dynamic_cast<CVerticalLayoutUI*>(FindSubControl("netlessLayout"));
	m_pSearchContentList	= dynamic_cast<CTileWithButtonLayoutUI*>(FindSubControl("searchContentList"));
	m_pSearchContentList->SetDelayedDestroy(false);
	m_pLoadingGif			= dynamic_cast<CGifAnimUI*>(FindSubControl(_T("searchGifLoading")));
	//searchhistory
	m_strKeyWordPath = GetLocalPath();
	m_strKeyWordPath += _T("\\Setting\\SearchKeyWord.dat");

	this->OnNotify	+= MakeDelegate(this, &CSearchWindowUI::OnBtnWindowNotify);

	CButtonUI* pButton		= dynamic_cast<CButtonUI*>(FindSubControl("clearHistory"));
	pButton->OnNotify		+= MakeDelegate(this, &CSearchWindowUI::OnBtnClearHistoryNotify);
	
	//checkboxs
	CCheckBoxUI* pCheckBox = dynamic_cast<CCheckBoxUI*>(FindSubControl(_T("boxall")));
	pCheckBox->OnNotify += MakeDelegate(this, &CSearchWindowUI::OnCheckBoxALLNotify);
	pCheckBox->Selected(true);
	m_vecCheckBoxs.push_back(pCheckBox);
	pCheckBox = dynamic_cast<CCheckBoxUI*>(FindSubControl(_T("boxcourse")));
	pCheckBox->OnNotify += MakeDelegate(this, &CSearchWindowUI::OnCheckBoxNotify);
	pCheckBox->Selected(true);
	m_vecCheckBoxs.push_back(pCheckBox);
	pCheckBox = dynamic_cast<CCheckBoxUI*>(FindSubControl(_T("boxvideo")));
	pCheckBox->OnNotify += MakeDelegate(this, &CSearchWindowUI::OnCheckBoxNotify);
	pCheckBox->Selected(true);
	m_vecCheckBoxs.push_back(pCheckBox);
	pCheckBox = dynamic_cast<CCheckBoxUI*>(FindSubControl(_T("boximage")));
	pCheckBox->OnNotify += MakeDelegate(this, &CSearchWindowUI::OnCheckBoxNotify);
	pCheckBox->Selected(true);
	m_vecCheckBoxs.push_back(pCheckBox);
	pCheckBox = dynamic_cast<CCheckBoxUI*>(FindSubControl(_T("boxflash")));
	pCheckBox->OnNotify += MakeDelegate(this, &CSearchWindowUI::OnCheckBoxNotify);
	pCheckBox->Selected(true);
	m_vecCheckBoxs.push_back(pCheckBox);
	pCheckBox = dynamic_cast<CCheckBoxUI*>(FindSubControl(_T("boxvolume")));
	pCheckBox->OnNotify += MakeDelegate(this, &CSearchWindowUI::OnCheckBoxNotify);
	pCheckBox->Selected(true);
	m_vecCheckBoxs.push_back(pCheckBox);
	pCheckBox = dynamic_cast<CCheckBoxUI*>(FindSubControl(_T("boxquestion")));
	pCheckBox->OnNotify += MakeDelegate(this, &CSearchWindowUI::OnCheckBoxNotify);
	pCheckBox->Selected(true);
	m_vecCheckBoxs.push_back(pCheckBox);
	pCheckBox = dynamic_cast<CCheckBoxUI*>(FindSubControl(_T("boxcoursewareobjests")));
	pCheckBox->OnNotify += MakeDelegate(this, &CSearchWindowUI::OnCheckBoxNotify);
	pCheckBox->Selected(true);
	m_vecCheckBoxs.push_back(pCheckBox);
	pCheckBox = dynamic_cast<CCheckBoxUI*>(FindSubControl(_T("boxppttemplate")));
	pCheckBox->OnNotify += MakeDelegate(this, &CSearchWindowUI::OnCheckBoxNotify);
	pCheckBox->Selected(true);
	m_vecCheckBoxs.push_back(pCheckBox);
	pCheckBox = dynamic_cast<CCheckBoxUI*>(FindSubControl(_T("boxother")));
	pCheckBox->OnNotify += MakeDelegate(this, &CSearchWindowUI::OnCheckBoxNotify);
	pCheckBox->Selected(true);
	m_vecCheckBoxs.push_back(pCheckBox);
	//

	//检测是否有网络
	if(CheckNetStatus())
		ShowSearchHistory();
	else
	{
		m_pNetlessLayout->SetVisible(true);
		m_pSearchHistoryLayout->SetVisible(false);
	}

	::SetFocus(CGroupExplorerUI::GetInstance()->GetHWND());
}

bool CSearchWindowUI::OnBtnTipEvent( void * pObj )
{
	TEventUI* pEvent = (TEventUI* )pObj;
	if (pEvent->Type == UIEVENT_BUTTONDOWN)
	{
		if(m_bShowTipListDlg)
		{
			CRect rect, rect1;
			GetWindowRect(CGroupExplorerUI::GetInstance()->GetHWND(), &rect);
			rect1 = m_pTipLayout->GetPos();
			::SetWindowPos(m_SearchTipListDlg.GetHWND(), NULL, rect.left + rect1.left , rect.top + rect1.top + rect1.Height(), 271, 69, SWP_NOACTIVATE | SWP_NOZORDER);
			m_SearchTipListDlg.ShowWindow(true);
		}
		else
			m_bShowTipListDlg = true;
	}


	return true;
}

bool CSearchWindowUI::OnBtnSearchNotify( void * pObj )
{
	TNotifyUI* pNotify = (TNotifyUI* )pObj;
	if (pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		if(CheckNetStatus() || m_pSearchContentLayout->IsVisible())
		{
			if(m_pNetlessLayout->IsVisible())
				m_pNetlessLayout->SetVisible(false);

			string strKeyWord = m_pSearchEdit->GetText();
			if(strKeyWord == _T("") || strKeyWord == _T("搜索课件、视频、图片、音频"))
			{
				CToast::Toast(_T("请输入搜索关键字"));
				if(!m_pSearchHistoryLayout->IsVisible() && !m_pSearchContentLayout->IsVisible())
					ShowSearchHistory();
				return false;
			}
			if(m_dwCheckBoxFlag == CheckBox_NULL)
			{
				CToast::Toast(_T("请选择搜索项目"));
				if(!m_pSearchHistoryLayout->IsVisible() && !m_pSearchContentLayout->IsVisible())
					ShowSearchHistory();
				return false;
			}

			ClearDownloadThreads();

			m_strKeyword = strKeyWord;

			CItemExplorerUI::GetInstance()->ShowWindow(false);

			if(m_dwSearchType & Search_Type_101PPT)
			{
				OnSearchGetCountWith101PPT();
			}
			else if(m_dwSearchType & Search_Type_Baidu)
			{
				OnSearchWithBaidu(true);
			}
		}
		else
		{
			if(m_pSearchHistoryLayout->IsVisible())
				m_pSearchHistoryLayout->SetVisible(false);
			m_pNetlessLayout->SetVisible(true);
		}
	}

	return true;
}


bool CSearchWindowUI::OnBtnWindowNotify( void * pObj )
{
	TNotifyUI* pNotify = (TNotifyUI* )pObj;
	if (pNotify->sType == Refresh_TileLayout)
	{
		m_hThread = NULL;

		m_pSearchContentList->HideMoreButton();

		map<int ,CStream *>::iterator iter;
		for(iter = m_mapStream.begin() ; iter != m_mapStream.end() ; iter++)
		{
			CStream * pStream = iter->second;
			int nType = iter->first;
			int m_nCurrentType = nType;

			pStream->ResetCursor();
			int nCount = pStream->ReadInt();
			m_dwRequestedResCount += nCount;
			m_dwResourceTotal += nCount;

			for (int i = 0; i < nCount; i++)
			{
				if (nType == CloudFileCourse || nType == CloudFilePPTTemplate || nType == DBankCourse)
				{
					int nNdpType = pStream->ReadInt();
					if(nNdpType == 0 )
						m_nCurrentType = CloudFileCourse;

					else if( nNdpType == 1 )
						m_nCurrentType = CloudFileNdpCourse;

					else
						m_nCurrentType = CloudFileNdpxCourse;
				}

				CResourceItemExUI* pItem = CResourceItemExUI::AllocResourceItem(m_nCurrentType);

				if (pItem)
				{
					pItem->SetPadding(CDuiRect(6, 0, 6, 0));
					pItem->ReadStream(pStream);
					m_pSearchContentList->Add(pItem);

				}
			}
		}

		StopMask();
		ShowSearchResult();
	}

	return true;
}

bool CSearchWindowUI::OnBtnClearHistoryNotify( void * pObj )
{
	TNotifyUI* pNotify = (TNotifyUI* )pObj;
	if (pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		DeleteFile(m_strKeyWordPath.c_str());
		ShowSearchHistory();
	}

	return true;
}

bool CSearchWindowUI::OnBtnMoreCallBack( void* pObj )
{
	if(!m_bRequestingMore)
	{
		m_bRequestingMore = true;
		map<int,CStream*>::iterator iter1;
		for(iter1 = m_mapStream.begin() ; iter1 != m_mapStream.end() ; iter1++)
		{
			delete iter1->second;
		}
		m_mapStream.clear();
		m_pSearchContentList->StartMoreBtnLoading();
		OnDoRequestCountFinish();
	}
	return true;
}

void CSearchWindowUI::ShowSearchHistory()
{
	CListUI * pList = static_cast<CListUI*>(FindSubControl(_T("searchList")));
	FILE* fp = fopen(m_strKeyWordPath.c_str(), "rb");
	m_jsonKeyWordRoot.clear();
	if( fp )
	{
		fseek(fp, 0, SEEK_END);
		int size = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		char* pBuff = new char[size + 1];
		pBuff[size] = 0;
		fread(pBuff, size, 1, fp);

		fclose(fp);

		bool bRet = m_jsonKeyWord.parse(pBuff,m_jsonKeyWordRoot);

		delete pBuff;
	}
	pList->RemoveAll();

	Json::Value value;

	CDialogBuilder		m_builder;

	CLabelUI * pLabel = NULL;

	int nCount = m_jsonKeyWordRoot.size();
	if(nCount > 0)
	{
		for(int i = 0; i< nCount ; i++)
		{
			value = m_jsonKeyWordRoot[i];
			tstring strKeyWord = value["keyword"].asString();
			tstring strdate = value["date"].asString();

			CContainerUI * pItem = NULL;

			if( !m_Builder.GetMarkup()->IsValid() ) {
				pItem = dynamic_cast<CContainerUI*>(m_Builder.Create(_T("RightBar\\SearchKeyWord.xml"), (UINT)0, this, this->GetManager()));
			}
			else {
				pItem = dynamic_cast<CContainerUI*>(m_Builder.Create(this, this->GetManager()));
			}

			if (pItem == NULL)
				return ;

			pLabel = dynamic_cast<CLabelUI*>(pItem->FindSubControl(_T("keyword")));
			pLabel->SetText(strKeyWord.c_str());
			pLabel->SetToolTip(strKeyWord.c_str());
			pLabel = dynamic_cast<CLabelUI*>(pItem->FindSubControl(_T("date")));
			pLabel->SetText(strdate.c_str());
			pList->Add(pItem);
		}

		
	}
	m_pSearchHistoryLayout->SetVisible(true);
}


void CSearchWindowUI::InsertKeyWordJson( tstring & strKeyWord )
{
	DWORD dwCount = m_jsonKeyWordRoot.size();
	Json::Value valueNew;
	Json::Value valueKW;

	CTime tm=CTime::GetCurrentTime();
	CString strTime=tm.Format("%Y/%m/%d");

	valueKW["keyword"] = strKeyWord;
	valueKW["date"] = strTime.GetString();
	valueNew.append(valueKW);

	//判断重复

	int nExsit = -1;
	for(int i = 0; i < (int)dwCount ; i++)
	{
		valueKW = m_jsonKeyWordRoot[i];
		if( strKeyWord == valueKW["keyword"].asString())
		{
			nExsit = i;

			break;;
		}
	}
	//

	if(dwCount == Search_History_MaxCount)
	{	
		if(nExsit == -1)
		{
			m_jsonKeyWordRoot[Search_History_MaxCount - 1] = Json::Value::null;
			dwCount--;
		}
		else
		{
			m_jsonKeyWordRoot[nExsit] = Json::Value::null;
		}
	}
	else
	{
		m_jsonKeyWordRoot[nExsit] = Json::Value::null;
	}

	for(int i = 0; i < (int)dwCount ; i++)
	{
		if(nExsit == i)
		{
			continue;
		}
		valueKW = m_jsonKeyWordRoot[i];

		valueNew.append(valueKW);
	}

	tstring str = m_jsonKeyWordWriter.write(valueNew);

	m_jsonKeyWordRoot = valueNew;

	FILE* fp = fopen(m_strKeyWordPath.c_str(), "wb+");
	if( fp == NULL )
	{
		return ;
	}

	fwrite(str.c_str(), str.length(), 1, fp);
	fclose(fp);
}

void CSearchWindowUI::StartMask()
{
	m_pSearchLoadingLayout->SetVisible(true);
	m_pLoadingGif->PlayGif();
}

void CSearchWindowUI::StopMask()
{
	m_pSearchLoadingLayout->SetVisible(false);
	m_pLoadingGif->StopGif();
}

void CSearchWindowUI::OnSearchGetCountWith101PPT()
{
	tstring strKeyWord = m_strKeyword;
	//先获取总量
	m_dwRequestAllCount		= 0; //清空
	m_dwRequestSuccess		= 0;
	m_dwRequestedResCount	= 0;
	m_dwResourceTotal		= 0 ;
	m_pSearchContentList->RemoveAll();

	map<int,Record_Requested*>::iterator iter;
	for(iter = m_mapResCount.begin() ; iter != m_mapResCount.end() ; iter++)
	{
		memset(iter->second, 0x00, sizeof(Record_Requested));
	}

	map<int,CStream*>::iterator iter1;
	for(iter1 = m_mapStream.begin() ; iter1 != m_mapStream.end() ; iter1++)
	{
		delete iter1->second;
	}
	m_mapStream.clear();

	//
	DWORD dwDownloadThreadId;
	if( m_dwCheckBoxFlag & CheckBox_Course)
	{
		int* nType = new int;
		*nType = CloudFileCourse;
		m_strCloudRequestUrl = NDCloudComposeUrlCourseInfo(_T(""), strKeyWord, 0 ,RequestTotal_Count);
		dwDownloadThreadId = NDCloudDownload(m_strCloudRequestUrl,MakeHttpDelegate(this,&CSearchWindowUI::OnRequestResponse), nType);
		m_vecDownloadThreads.push_back(dwDownloadThreadId);
		m_dwRequestAllCount++;
	}

	if( m_dwCheckBoxFlag & CheckBox_Flash)
	{
		int* nType = new int;
		*nType = CloudFileFlash;
		m_strCloudRequestUrl = NDCloudComposeUrlFlashInfo(_T(""), strKeyWord, 0 ,RequestTotal_Count);
		dwDownloadThreadId = NDCloudDownload(m_strCloudRequestUrl,MakeHttpDelegate(this,&CSearchWindowUI::OnRequestResponse), nType);
		m_vecDownloadThreads.push_back(dwDownloadThreadId);
		m_dwRequestAllCount++;
	}

	if( m_dwCheckBoxFlag & CheckBox_Image)
	{
		int* nType = new int;
		*nType = CloudFileImage;
		m_strCloudRequestUrl = NDCloudComposeUrlPictureInfo(_T(""), strKeyWord, 0 ,RequestTotal_Count);
		dwDownloadThreadId = NDCloudDownload(m_strCloudRequestUrl,MakeHttpDelegate(this,&CSearchWindowUI::OnRequestResponse), nType);
		m_vecDownloadThreads.push_back(dwDownloadThreadId);
		m_dwRequestAllCount++;
	}

	if( m_dwCheckBoxFlag & CheckBox_Video)
	{
		int* nType = new int;
		*nType = CloudFileVideo;
		m_strCloudRequestUrl = NDCloudComposeUrlVideoInfo(_T(""), strKeyWord, 0 ,RequestTotal_Count);
		dwDownloadThreadId = NDCloudDownload(m_strCloudRequestUrl,MakeHttpDelegate(this,&CSearchWindowUI::OnRequestResponse), nType);
		m_vecDownloadThreads.push_back(dwDownloadThreadId);
		m_dwRequestAllCount++;
	}

	if( m_dwCheckBoxFlag & CheckBox_Volume)
	{
		int* nType = new int;
		*nType = CloudFileVolume;
		m_strCloudRequestUrl = NDCloudComposeUrlVolumeInfo(_T(""), strKeyWord, 0 ,RequestTotal_Count);
		dwDownloadThreadId = NDCloudDownload(m_strCloudRequestUrl,MakeHttpDelegate(this,&CSearchWindowUI::OnRequestResponse), nType);
		m_vecDownloadThreads.push_back(dwDownloadThreadId);
		m_dwRequestAllCount++;
	} 

	if( m_dwCheckBoxFlag & CheckBox_Questions)
	{
		int* nType = new int;
		*nType = CloudFileQuestion;
		m_strCloudRequestUrl = NDCloudComposeUrlQuestionInfo(_T(""), strKeyWord, 0 ,RequestTotal_Count);
		dwDownloadThreadId = NDCloudDownload(m_strCloudRequestUrl,MakeHttpDelegate(this,&CSearchWindowUI::OnRequestResponse), nType);
		m_vecDownloadThreads.push_back(dwDownloadThreadId);
		m_dwRequestAllCount++;
	} 

	if( m_dwCheckBoxFlag & CheckBox_CoursewareObjects)
	{
		int* nType = new int;
		*nType = CloudFileCoursewareObjects;
		m_strCloudRequestUrl = NDCloudComposeUrlCoursewareObjectsInfo(_T(""), strKeyWord, 0 ,RequestTotal_Count);
		dwDownloadThreadId = NDCloudDownload(m_strCloudRequestUrl,MakeHttpDelegate(this,&CSearchWindowUI::OnRequestResponse), nType);
		m_vecDownloadThreads.push_back(dwDownloadThreadId);
		m_dwRequestAllCount++;
	} 

	if( m_dwCheckBoxFlag & CheckBox_PPTTemplate)
	{
		int* nType = new int;
		*nType = CloudFilePPTTemplate;
// 
// 		CCategoryTree * pCategoryTree = NULL;
// 		NDCloudGetCategoryTree(pCategoryTree);

		m_strCloudRequestUrl = NDCloudComposeUrlPPTTemplateInfo(_T(""), _T(""), strKeyWord, 0 ,RequestTotal_Count);
		dwDownloadThreadId = NDCloudDownload(m_strCloudRequestUrl,MakeHttpDelegate(this,&CSearchWindowUI::OnRequestResponse), nType);
		m_vecDownloadThreads.push_back(dwDownloadThreadId);
		m_dwRequestAllCount++;
	} 

	if( m_dwCheckBoxFlag & CheckBox_Other)
	{
		int* nType = new int;
		*nType = CloudFile3DResource;
		m_strCloudRequestUrl = NDCloudComposeUrl3DResourceInfo(_T(""), strKeyWord, 0 ,RequestTotal_Count);
		dwDownloadThreadId = NDCloudDownload(m_strCloudRequestUrl,MakeHttpDelegate(this,&CSearchWindowUI::OnRequestResponse), nType);
		m_vecDownloadThreads.push_back(dwDownloadThreadId);
		m_dwRequestAllCount++;
	}

	if( m_dwCheckBoxFlag & CheckBox_Other)
	{
		int* nType = new int;
		*nType = CloudFileVRResource;
		m_strCloudRequestUrl = NDCloudComposeUrlVRResourceInfo(_T(""), strKeyWord, _T(""), 0 ,RequestTotal_Count);
		dwDownloadThreadId = NDCloudDownload(m_strCloudRequestUrl,MakeHttpDelegate(this,&CSearchWindowUI::OnRequestResponse), nType);
		m_vecDownloadThreads.push_back(dwDownloadThreadId);
		m_dwRequestAllCount++;
	} 

	DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();
	if(dwUserId > 0)
	{
		if( m_dwCheckBoxFlag & CheckBox_Course)
		{
			int* nType = new int;
			*nType = DBankCourse;
			m_strDBankRequestUrl = NDCloudComposeUrlCourseInfo(_T(""), strKeyWord, 0, RequestTotal_Count, dwUserId);
			dwDownloadThreadId = NDCloudDownload(m_strDBankRequestUrl,MakeHttpDelegate(this,&CSearchWindowUI::OnRequestResponse), nType);
			m_vecDownloadThreads.push_back(dwDownloadThreadId);
			m_dwRequestAllCount++;
		} 

		if( m_dwCheckBoxFlag & CheckBox_Image)
		{
			int* nType = new int;
			*nType = DBankImage;
			m_strDBankRequestUrl = NDCloudComposeUrlPictureInfo(_T(""), strKeyWord, 0, RequestTotal_Count, dwUserId);
			dwDownloadThreadId = NDCloudDownload(m_strDBankRequestUrl,MakeHttpDelegate(this,&CSearchWindowUI::OnRequestResponse), nType);
			m_vecDownloadThreads.push_back(dwDownloadThreadId);
			m_dwRequestAllCount++;
		} 

		if( m_dwCheckBoxFlag & CheckBox_Volume)
		{
			int* nType = new int;
			*nType = DBankVolume;
			m_strDBankRequestUrl = NDCloudComposeUrlVolumeInfo(_T(""), strKeyWord, 0, RequestTotal_Count, dwUserId);
			dwDownloadThreadId = NDCloudDownload(m_strDBankRequestUrl,MakeHttpDelegate(this,&CSearchWindowUI::OnRequestResponse), nType);
			m_vecDownloadThreads.push_back(dwDownloadThreadId);
			m_dwRequestAllCount++;
		}

		if( m_dwCheckBoxFlag & CheckBox_Video)
		{
			int* nType = new int;
			*nType = DBankVideo;
			m_strDBankRequestUrl = NDCloudComposeUrlVideoInfo(_T(""), strKeyWord, 0, RequestTotal_Count, dwUserId);
			dwDownloadThreadId = NDCloudDownload(m_strDBankRequestUrl,MakeHttpDelegate(this,&CSearchWindowUI::OnRequestResponse), nType);
			m_vecDownloadThreads.push_back(dwDownloadThreadId);
			m_dwRequestAllCount++;
		}

		if( m_dwCheckBoxFlag & CheckBox_Flash)
		{
			int* nType = new int;
			*nType = DBankFlash;
			m_strDBankRequestUrl = NDCloudComposeUrlFlashInfo(_T(""), strKeyWord, 0, RequestTotal_Count, dwUserId);
			dwDownloadThreadId = NDCloudDownload(m_strDBankRequestUrl,MakeHttpDelegate(this,&CSearchWindowUI::OnRequestResponse), nType);
			m_vecDownloadThreads.push_back(dwDownloadThreadId);
			m_dwRequestAllCount++;
		}

		if( m_dwCheckBoxFlag & CheckBox_Questions)
		{
			int* nType = new int;
			*nType = DBankQuestion;
			m_strDBankRequestUrl = NDCloudComposeUrlQuestionInfo(_T(""), strKeyWord, 0, RequestTotal_Count, dwUserId);
			dwDownloadThreadId = NDCloudDownload(m_strDBankRequestUrl,MakeHttpDelegate(this,&CSearchWindowUI::OnRequestResponse), nType);
			m_vecDownloadThreads.push_back(dwDownloadThreadId);
			m_dwRequestAllCount++;
		} 

		if( m_dwCheckBoxFlag & CheckBox_CoursewareObjects)
		{
			int* nType = new int;
			*nType = DBankCoursewareObjects;
			m_strDBankRequestUrl = NDCloudComposeUrlCoursewareObjectsInfo(_T(""), strKeyWord, 0, RequestTotal_Count, dwUserId);
			dwDownloadThreadId = NDCloudDownload(m_strDBankRequestUrl,MakeHttpDelegate(this,&CSearchWindowUI::OnRequestResponse), nType);
			m_vecDownloadThreads.push_back(dwDownloadThreadId);
			m_dwRequestAllCount++;
		} 
	}

	StartMask();

	InsertKeyWordJson(m_strKeyword);

}


void CSearchWindowUI::OnSearchWithBaidu(bool bInit)
{
	if(bInit)
	{
		//清空
		m_dwRequestSuccess		= 0;
		m_dwRequestedResCount	= 0;
		m_dwResourceTotal		= 0 ;
		m_pSearchContentList->RemoveAll();

		StartMask();
		InsertKeyWordJson(m_strKeyword);

		map<int,Record_Requested*>::iterator iter;
		for(iter = m_mapResCount.begin() ; iter != m_mapResCount.end() ; iter++)
		{
			memset(iter->second, 0x00, sizeof(Record_Requested));
		}
	}
	m_dwRequestAllCount		= 0; 
	
	if(m_hThread)
	{
		TerminateThread(m_hThread, 0);  
		CloseHandle(m_hThread);  
	}

	map<int,CStream*>::iterator iter1;
	for(iter1 = m_mapStream.begin() ; iter1 != m_mapStream.end() ; iter1++)
	{
		delete iter1->second;
	}
	m_mapStream.clear();

	m_dwRequestAllCount++;
	m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)OnSearchWithBaiduThread, this, 0, NULL);

	

	
}


bool CSearchWindowUI::OnRequestResponse(void* pObj)
{
	THttpNotify* pHttpNotify = (THttpNotify*)pObj;

	int nType = *(int *)pHttpNotify->pUserData;
	delete pHttpNotify->pUserData;
 	m_dwRequestSuccess++;
	
	if (pHttpNotify->dwErrorCode > 0)
	{
		goto _Out;
	}

	int nCount = NDCloudDecodeCount(pHttpNotify->pData, pHttpNotify->nDataSize);

	if(m_mapResCount.find(nType) == m_mapResCount.end())
	{
		Record_Requested* pRecord = new Record_Requested;
		m_mapResCount[nType] = pRecord;
	}
	m_mapResCount[nType]->nStart = 0;
	m_mapResCount[nType]->nCount = nCount;
	
	m_dwResourceTotal += nCount;
_Out:
	if(m_dwRequestSuccess == m_dwRequestAllCount)
	{
		ClearDownloadThreads();
		OnDoRequestCountFinish();
	}
	return true;
}

void CSearchWindowUI::OnDoRequestCountFinish()
{
	if(m_dwSearchType & Search_Type_101PPT)
	{
		map<int,Record_Requested*>::iterator iter;
		m_dwRequestSuccess = 0;
		m_dwRequestAllCount = 0;

		int nTempCount = 0;
		int nNeedRequestResCount	= Request_Count;
		int nRequestedResCount		= m_dwRequestedResCount;
		for(iter = m_mapResCount.begin() ; iter != m_mapResCount.end() ; iter++)
		{
			nTempCount += iter->second->nCount;
			if(nRequestedResCount + nNeedRequestResCount <= nTempCount )
			{
				OnRequesResource(iter->first, nRequestedResCount, nNeedRequestResCount);
				break;
			}
			else if(nRequestedResCount + nNeedRequestResCount > nTempCount && nTempCount > nRequestedResCount)
			{
				OnRequesResource(iter->first, nRequestedResCount, nTempCount - nRequestedResCount);
				nNeedRequestResCount -= (nTempCount - nRequestedResCount);
				nRequestedResCount += nTempCount - nRequestedResCount;

			}
		}
	}
	else if(m_dwSearchType & Search_Type_Baidu)
	{
		OnSearchWithBaidu(false);
	}

	if(m_dwRequestAllCount == 0)
	{
		StopMask();
		ShowSearchResult();
		m_bRequestingMore = false;
	}
}

void CSearchWindowUI::OnRequesResource(int nType, int nStart, int nCount )
{
	DWORD dwUserId = 0;
	
	
	if(nType > DBankSearch)
	{
		dwUserId = NDCloudUser::GetInstance()->GetUserId();
		if(dwUserId < 0)
			dwUserId = 0;
	}

	DWORD dwDownloadThreadId;
	switch(nType)
	{
	case CloudFileCourse:
	case DBankCourse:
		{
			int* pType = new int;
			*pType = nType;
			
			m_strCloudRequestUrl = NDCloudComposeUrlCourseInfo(_T(""), m_strKeyword, m_mapResCount[nType]->nStart, nCount, dwUserId);
			dwDownloadThreadId = NDCloudDownload(m_strCloudRequestUrl,MakeHttpDelegate(this,&CSearchWindowUI::OnDoRequesResourceFinish), pType);
			m_vecDownloadThreads.push_back(dwDownloadThreadId);
			m_dwRequestAllCount++;
		}
		break;
	case CloudFileFlash:
	case DBankFlash:
		{
			int* pType = new int;
			*pType = nType;
			m_strCloudRequestUrl = NDCloudComposeUrlFlashInfo(_T(""), m_strKeyword, m_mapResCount[nType]->nStart, nCount, dwUserId);
			dwDownloadThreadId = NDCloudDownload(m_strCloudRequestUrl,MakeHttpDelegate(this,&CSearchWindowUI::OnDoRequesResourceFinish), pType);
			m_vecDownloadThreads.push_back(dwDownloadThreadId);
			m_dwRequestAllCount++;
		}
		break;
	case CloudFileImage:
	case DBankImage:
		{
			int* pType = new int;
			*pType = nType;
			m_strCloudRequestUrl = NDCloudComposeUrlPictureInfo(_T(""), m_strKeyword, m_mapResCount[nType]->nStart, nCount, dwUserId);
			dwDownloadThreadId = NDCloudDownload(m_strCloudRequestUrl,MakeHttpDelegate(this,&CSearchWindowUI::OnDoRequesResourceFinish), pType);
			m_vecDownloadThreads.push_back(dwDownloadThreadId);
			m_dwRequestAllCount++;
		}
		break;
	case CloudFileVideo:
	case DBankVideo:
		{
			int* pType = new int;
			*pType = nType;
			m_strCloudRequestUrl = NDCloudComposeUrlVideoInfo(_T(""), m_strKeyword, m_mapResCount[nType]->nStart, nCount, dwUserId);
			dwDownloadThreadId = NDCloudDownload(m_strCloudRequestUrl,MakeHttpDelegate(this,&CSearchWindowUI::OnDoRequesResourceFinish), pType);
			m_vecDownloadThreads.push_back(dwDownloadThreadId);
			m_dwRequestAllCount++;
		}
		break;
	case CloudFileVolume:
	case DBankVolume:
		{
			int* pType = new int;
			*pType = nType;
			m_strCloudRequestUrl = NDCloudComposeUrlVolumeInfo(_T(""), m_strKeyword, m_mapResCount[nType]->nStart, nCount, dwUserId);
			dwDownloadThreadId = NDCloudDownload(m_strCloudRequestUrl,MakeHttpDelegate(this,&CSearchWindowUI::OnDoRequesResourceFinish), pType);
			m_vecDownloadThreads.push_back(dwDownloadThreadId);
			m_dwRequestAllCount++;
		}
		break;
	case CloudFileQuestion:
	case DBankQuestion:
		{
			int* pType = new int;
			*pType = nType;
			m_strCloudRequestUrl = NDCloudComposeUrlQuestionInfo(_T(""), m_strKeyword, m_mapResCount[nType]->nStart, nCount, dwUserId);
			dwDownloadThreadId = NDCloudDownload(m_strCloudRequestUrl,MakeHttpDelegate(this,&CSearchWindowUI::OnDoRequesResourceFinish), pType);
			m_vecDownloadThreads.push_back(dwDownloadThreadId);
			m_dwRequestAllCount++;
		}
		break;
	case CloudFileCoursewareObjects:
	case DBankCoursewareObjects:
		{
			int* pType = new int;
			*pType = nType;
			m_strCloudRequestUrl = NDCloudComposeUrlCoursewareObjectsInfo(_T(""), m_strKeyword, m_mapResCount[nType]->nStart, nCount, dwUserId);
			dwDownloadThreadId = NDCloudDownload(m_strCloudRequestUrl,MakeHttpDelegate(this,&CSearchWindowUI::OnDoRequesResourceFinish), pType);
			m_vecDownloadThreads.push_back(dwDownloadThreadId);
			m_dwRequestAllCount++;
		}
		break;
	case CloudFilePPTTemplate:
		{
			int* pType = new int;
			*pType = nType;

// 			CCategoryTree * pCategoryTree = NULL;
// 			NDCloudGetCategoryTree(pCategoryTree);

			m_strCloudRequestUrl = NDCloudComposeUrlPPTTemplateInfo(_T(""), _T(""), m_strKeyword, m_mapResCount[nType]->nStart, nCount);
			dwDownloadThreadId = NDCloudDownload(m_strCloudRequestUrl,MakeHttpDelegate(this,&CSearchWindowUI::OnDoRequesResourceFinish), pType);
			m_vecDownloadThreads.push_back(dwDownloadThreadId);
			m_dwRequestAllCount++;
		}
		break;
	case CloudFile3DResource:
		{
			int* pType = new int;
			*pType = nType;
			m_strCloudRequestUrl = NDCloudComposeUrl3DResourceInfo(_T(""), m_strKeyword, m_mapResCount[nType]->nStart, nCount);
			dwDownloadThreadId = NDCloudDownload(m_strCloudRequestUrl,MakeHttpDelegate(this,&CSearchWindowUI::OnDoRequesResourceFinish), pType);
			m_vecDownloadThreads.push_back(dwDownloadThreadId);
			m_dwRequestAllCount++;
		}
		break;
	case CloudFileVRResource:
		{
			int* pType = new int;
			*pType = nType;
			m_strCloudRequestUrl = NDCloudComposeUrlVRResourceInfo(_T(""), m_strKeyword, _T(""), m_mapResCount[nType]->nStart, nCount);
			dwDownloadThreadId = NDCloudDownload(m_strCloudRequestUrl,MakeHttpDelegate(this,&CSearchWindowUI::OnDoRequesResourceFinish), pType);
			m_vecDownloadThreads.push_back(dwDownloadThreadId);
			m_dwRequestAllCount++;
		}
		break;
	}
}

bool CSearchWindowUI::OnDoRequesResourceFinish(void* pObj)
{
	THttpNotify* pHttpNotify = (THttpNotify*)pObj;

	int nType = *(int *)pHttpNotify->pUserData;
	delete pHttpNotify->pUserData;
 	m_dwRequestSuccess++;
 	if(m_dwRequestSuccess == m_dwRequestAllCount)
 	{
 		StopMask();
 	}
	
	if (pHttpNotify->dwErrorCode > 0)
	{
		goto _Out;
	}

	//
	if(m_mapStream.find(nType) == m_mapStream.end())
	{
		CStream* pStream = new CStream(1024);
		m_mapStream[nType] = pStream;
	}
	else
	{
		CStream* pStream = m_mapStream[nType] ;
		delete pStream;
		pStream = new CStream(1024);
		m_mapStream[nType] = pStream;
	}

	CStream* pStream = NULL;
	switch(nType)
	{
	case CloudFileCourse:
	case DBankCourse:
		{
			pStream = m_mapStream[nType] ;
			if (NDCloudDecodeCourseList(pHttpNotify->pData, pHttpNotify->nDataSize, pStream))
			{
				
			}
		}
		break;
	case CloudFileImage:
	case DBankImage:
		{
			pStream = m_mapStream[nType] ;
			if (NDCloudDecodeImageList(pHttpNotify->pData, pHttpNotify->nDataSize, pStream))
			{

			}
		}
		break;
	case CloudFileVideo:
	case DBankVideo:
		{
			pStream = m_mapStream[nType] ;
			if (NDCloudDecodeVideoList(pHttpNotify->pData, pHttpNotify->nDataSize, pStream))
			{

			}
		}
		break;
	case CloudFileVolume:
	case DBankVolume:
		{
			pStream = m_mapStream[nType] ;
			if (NDCloudDecodeVolumeList(pHttpNotify->pData, pHttpNotify->nDataSize, pStream))
			{

			}
		}
		break;
	case CloudFileFlash:
	case DBankFlash:
		{
			pStream = m_mapStream[nType] ;
			if (NDCloudDecodeFlashList(pHttpNotify->pData, pHttpNotify->nDataSize, pStream))
			{

			}
		}
		break;
	case CloudFilePPTTemplate:
		{
			pStream = m_mapStream[nType] ;
			if (NDCloudDecodePPTTemplateList(pHttpNotify->pData, pHttpNotify->nDataSize, pStream))
			{

			}
		}
		break;
	case CloudFile3DResource:
		{
			pStream = m_mapStream[nType] ;
			if (NDCloudDecode3DResourceList(pHttpNotify->pData, pHttpNotify->nDataSize, pStream))
			{

			}
		}
		break;
	case CloudFileVRResource:
		{
			pStream = m_mapStream[nType] ;
			if (NDCloudDecodeVRResourceList(pHttpNotify->pData, pHttpNotify->nDataSize, pStream))
			{

			}
		}
		break;
	case CloudFileQuestion:
	case DBankQuestion:
		{
			pStream = m_mapStream[nType] ;
			if (NDCloudDecodeQuestionList(pHttpNotify->pData, pHttpNotify->nDataSize, pStream))
			{

			}
		}
		break;
	case CloudFileCoursewareObjects:
	case DBankCoursewareObjects:
		{
			pStream = m_mapStream[nType] ;
			if (NDCloudDecodeCoursewareObjectsList(pHttpNotify->pData, pHttpNotify->nDataSize, pStream))
			{

			}
		}
		break;
	}

	pStream->ResetCursor();
	int nCount = pStream->ReadInt();

	//
	m_mapResCount[nType]->nStart += nCount;
	m_dwRequestedResCount += nCount;
	//

	
_Out:
	if(m_dwRequestSuccess == m_dwRequestAllCount)
	{
		ClearDownloadThreads();

		m_pSearchContentList->HideMoreButton();

		map<int ,CStream *>::iterator iter;
		for(iter = m_mapStream.begin() ; iter != m_mapStream.end() ; iter++)
		{
			CStream * pStream = iter->second;
			int nType = iter->first;
			int m_nCurrentType = nType;

			pStream->ResetCursor();
			int nCount = pStream->ReadInt();

			for (int i = 0; i < nCount; i++)
			{
				if (nType == CloudFileCourse || nType == CloudFilePPTTemplate || nType == DBankCourse)
				{
					int nNdpType = pStream->ReadInt();
					if(nNdpType == 0 )
						m_nCurrentType = CloudFileCourse;

					else if( nNdpType == 1 )
						m_nCurrentType = CloudFileNdpCourse;

					else
						m_nCurrentType = CloudFileNdpxCourse;
				}

				CResourceItemExUI* pItem = CResourceItemExUI::AllocResourceItem(m_nCurrentType);

				if (pItem)
				{
					pItem->SetPadding(CDuiRect(6, 0, 6, 0));
					pItem->ReadStream(pStream);
					m_pSearchContentList->Add(pItem);

				}
			}
		}

		ShowSearchResult();
		
	}
	

	return true;
}

DWORD		WINAPI CSearchWindowUI::OnSearchWithBaiduThread( LPARAM lParam )
{
	CSearchWindowUI * pSearchWindowUI = (CSearchWindowUI*)lParam;
	WebSearch webSearch;

	int nCount = 0;
	tstring strJsonResult;
	if( pSearchWindowUI->m_dwCheckBoxFlag & CheckBox_Course)
	{
		int nType = CloudFileCourse;

		if(pSearchWindowUI->m_mapResCount.find(nType) == pSearchWindowUI->m_mapResCount.end())
		{
			Record_Requested* pRecord = new Record_Requested;
			memset(pRecord, 0x00, sizeof(Record_Requested));
			pSearchWindowUI->m_mapResCount[nType] = pRecord;
		}

		strJsonResult = webSearch.JsonResInfoByType(pSearchWindowUI->m_strKeyword,WebSearch::enResTypeCourseware, pSearchWindowUI->m_mapResCount[nType]->nStart, pSearchWindowUI->m_mapResCount[nType]->nStart + Request_Count);

		Json::Reader		m_jsonReader;
		Json::Value			m_jsonRoot;
		bool bRet = m_jsonReader.parse(strJsonResult.c_str(),m_jsonRoot);
		if(bRet)
		{
			//
			CStream* pCourseStream  = NULL;
			if(pSearchWindowUI->m_mapStream.find(nType) == pSearchWindowUI->m_mapStream.end())
			{
				pCourseStream = new CStream(1024);
				pSearchWindowUI->m_mapStream[nType] = pCourseStream;
			}
			else
			{
				pCourseStream = pSearchWindowUI->m_mapStream[nType] ;
				delete pCourseStream;
				pCourseStream = new CStream(1024);
				pSearchWindowUI->m_mapStream[nType] = pCourseStream;
			}

			Json::Value& items = m_jsonRoot[_T("ItemsInfo")];
			nCount = items.size();
			pSearchWindowUI->m_mapResCount[nType]->nStart += nCount;
			for(int i = 0 ; i < nCount ; i++)
			{
				// count
				pCourseStream->WriteDWORD(nCount);

				for(int i = 0; i < (int)items.size(); i++)
				{
					Json::Value& item = items[i];

					if(item["CoursewareNa"].isNull() || item["CoursewareObjAdd"].isNull())
						continue;
					tstring strTitle = Ansi2Str(item["CoursewareNa"].asCString());
					tstring strGuid = CalcMD5(Ansi2Str(item["CoursewareObjAdd"].asCString()));
					tstring strPPTUrl = Ansi2Str(item["CoursewareObjAdd"].asCString());
					tstring strMD5;
					int  nNdpType		= 0;

					// title | guid | pptUrl
					pCourseStream->WriteInt(nNdpType);
					pCourseStream->WriteString(strTitle);
					pCourseStream->WriteString(strGuid);
					pCourseStream->WriteString(strPPTUrl);
					pCourseStream->WriteString(strMD5);

					pCourseStream->WriteInt(0);

					// item json string
					pCourseStream->WriteString(_T(""));
				}
			}
		}
		//goto _OUT;
	}
// 
// 	if( pSearchWindowUI->m_dwCheckBoxFlag & CheckBox_Flash)
// 	{
// 		strJsonResult = webSearch.JsonResInfoByType(pSearchWindowUI->m_strKeyword,WebSearch::enResTypeCartoon, pSearchWindowUI->m_dwRequestedResCount, pSearchWindowUI->m_dwRequestedResCount + Request_Count);
// 
// 		Json::Reader		m_jsonReader;
// 		Json::Value			m_jsonRoot;
// 		bool bRet = m_jsonReader.parse(strJsonResult.c_str(),m_jsonRoot);
// 		if(bRet)
// 		{
// 
// 		}
// 		goto _OUT;
// 	}

	if( pSearchWindowUI->m_dwCheckBoxFlag & CheckBox_Image)
	{
		int nType = CloudFileImage;

		if(pSearchWindowUI->m_mapResCount.find(nType) == pSearchWindowUI->m_mapResCount.end())
		{
			Record_Requested* pRecord = new Record_Requested;
			memset(pRecord, 0x00, sizeof(Record_Requested));
			pSearchWindowUI->m_mapResCount[nType] = pRecord;
		}

		strJsonResult = webSearch.JsonResInfoByType(pSearchWindowUI->m_strKeyword,WebSearch::enResTypePicture, pSearchWindowUI->m_mapResCount[nType]->nStart, pSearchWindowUI->m_mapResCount[nType]->nStart + (Request_Count - nCount));

		Json::Reader		m_jsonReader;
		Json::Value			m_jsonRoot;
		bool bRet = m_jsonReader.parse(strJsonResult.c_str(),m_jsonRoot);
		if(bRet)
		{
			//
			CStream* pImageStream  = NULL;
			if(pSearchWindowUI->m_mapStream.find(nType) == pSearchWindowUI->m_mapStream.end())
			{
				pImageStream = new CStream(1024);
				pSearchWindowUI->m_mapStream[nType] = pImageStream;
			}
			else
			{
				pImageStream = pSearchWindowUI->m_mapStream[nType] ;
				delete pImageStream;
				pImageStream = new CStream(1024);
				pSearchWindowUI->m_mapStream[nType] = pImageStream;
			}

			Json::Value& items = m_jsonRoot["ItemsInfo"];
			nCount = items.size();
			pSearchWindowUI->m_mapResCount[nType]->nStart += nCount;
			for(int i = 0 ; i < nCount ; i++)
			{
				// count
				pImageStream->WriteDWORD(nCount);

				for(int i = 0; i < (int)items.size(); i++)
				{
					Json::Value& item = items[i];

					if(item["PictureNa"].isNull() || item["PictureObjAdd"].isNull())
						continue;
					tstring strTitle = Ansi2Str(item["PictureNa"].asCString());
					tstring strGuid = CalcMD5(Ansi2Str(item["PictureObjAdd"].asCString()));
					tstring strImageUrl = Ansi2Str(item["PictureObjAdd"].asCString());
					tstring strMD5;
					int  nNdpType		= 0;

					// title | guid | pptUrl
					pImageStream->WriteString(strGuid);
					pImageStream->WriteString(strTitle);
					pImageStream->WriteString(strImageUrl);
					pImageStream->WriteString(strMD5);
					pImageStream->WriteString(_T(""));
				}
			}
			
		}
		//goto _OUT;
	}
// 
// 	if( pSearchWindowUI->m_dwCheckBoxFlag & CheckBox_Video)
// 	{
// 		strJsonResult = webSearch.JsonResInfoByType(pSearchWindowUI->m_strKeyword,WebSearch::enResTypeVedio, pSearchWindowUI->m_dwRequestedResCount, pSearchWindowUI->m_dwRequestedResCount + Request_Count);
// 
// 		Json::Reader		m_jsonReader;
// 		Json::Value			m_jsonRoot;
// 		bool bRet = m_jsonReader.parse(strJsonResult.c_str(),m_jsonRoot);
// 		if(bRet)
// 		{
// 
// 		}
// 		goto _OUT;
// 	}
// 
// 	if( pSearchWindowUI->m_dwCheckBoxFlag & CheckBox_Volume)
// 	{
// 		strJsonResult = webSearch.JsonResInfoByType(pSearchWindowUI->m_strKeyword,WebSearch::enResTypeAudio, pSearchWindowUI->m_dwRequestedResCount, pSearchWindowUI->m_dwRequestedResCount + Request_Count);
// 
// 		Json::Reader		m_jsonReader;
// 		Json::Value			m_jsonRoot;
// 		bool bRet = m_jsonReader.parse(strJsonResult.c_str(),m_jsonRoot);
// 		if(bRet)
// 		{
// 
// 		}
// 		goto _OUT;
// 	} 
// 
// 	if( pSearchWindowUI->m_dwCheckBoxFlag & CheckBox_Questions)
// 	{
// 		strJsonResult = webSearch.JsonResInfoByType(pSearchWindowUI->m_strKeyword,WebSearch::enResTypeBasedproblem, pSearchWindowUI->m_dwRequestedResCount, pSearchWindowUI->m_dwRequestedResCount + Request_Count);
// 
// 		Json::Reader		m_jsonReader;
// 		Json::Value			m_jsonRoot;
// 		bool bRet = m_jsonReader.parse(strJsonResult.c_str(),m_jsonRoot);
// 		if(bRet)
// 		{
// 
// 		}
// 		goto _OUT;
// 	} 
// 
// 	if( pSearchWindowUI->m_dwCheckBoxFlag & CheckBox_CoursewareObjects)
// 	{
// 		strJsonResult = webSearch.JsonResInfoByType(pSearchWindowUI->m_strKeyword,WebSearch::enResTypeBasedproblem, pSearchWindowUI->m_dwRequestedResCount, pSearchWindowUI->m_dwRequestedResCount + Request_Count);
// 
// 		Json::Reader		m_jsonReader;
// 		Json::Value			m_jsonRoot;
// 		bool bRet = m_jsonReader.parse(strJsonResult.c_str(),m_jsonRoot);
// 		if(bRet)
// 		{
// 
// 		}
// 		goto _OUT;
// 	} 
// 
// 	if( pSearchWindowUI->m_dwCheckBoxFlag & CheckBox_PPTTemplate)
// 	{
// 		strJsonResult = webSearch.JsonResInfoByType(pSearchWindowUI->m_strKeyword,WebSearch::enResTypeInteractivetopic, pSearchWindowUI->m_dwRequestedResCount, pSearchWindowUI->m_dwRequestedResCount + Request_Count);
// 
// 		Json::Reader		m_jsonReader;
// 		Json::Value			m_jsonRoot;
// 		bool bRet = m_jsonReader.parse(strJsonResult.c_str(),m_jsonRoot);
// 		if(bRet)
// 		{
// 
// 		}
// 		goto _OUT;
// 	} 
// 
// 	if( pSearchWindowUI->m_dwCheckBoxFlag & CheckBox_Other)
// 	{
// 		strJsonResult = webSearch.JsonResInfoByType(pSearchWindowUI->m_strKeyword,WebSearch::enResTypeOthers, pSearchWindowUI->m_dwRequestedResCount, pSearchWindowUI->m_dwRequestedResCount + Request_Count);
// 
// 		Json::Reader		m_jsonReader;
// 		Json::Value			m_jsonRoot;
// 		bool bRet = m_jsonReader.parse(strJsonResult.c_str(),m_jsonRoot);
// 		if(bRet)
// 		{
// 
// 		}
// 		goto _OUT;
// 	}

_OUT:
	pSearchWindowUI->GetManager()->SendNotify(pSearchWindowUI, Refresh_TileLayout, 0 ,0 , true);

	return 0 ;
}

void CSearchWindowUI::ShowSearchResult()
{
	m_bRequestingMore = false;
	m_pSearchHistoryLayout->SetVisible(false);
	m_pSearchContentLayout->SetVisible(true);

	if(m_dwRequestedResCount < m_dwResourceTotal || (m_dwSearchType & Search_Type_Baidu && m_dwResourceTotal != 0 ))
	{
		m_pSearchContentList->ShowMoreButton();//显示加载更多的按钮，坑一个
		m_pSearchContentList->SetBtnMoreCallBack(MakeDelegate(this, &CSearchWindowUI::OnBtnMoreCallBack));
	}

	TCHAR szBuff[256];
	_stprintf(szBuff, _T("%d"), m_dwResourceTotal);

	CDuiRect rcCalc;
	rcCalc.right = 400;

	CalcText(GetManager()->GetPaintDC(), rcCalc, m_strKeyword.c_str(), m_pKeyWordLabel->GetFont(), DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT|DT_NOPREFIX, UIFONT_GDI);
	m_pKeyWordLabel->SetFixedWidth(rcCalc.GetWidth());
	m_pKeyWordLabel->SetText(m_strKeyword.c_str());
	m_pKeyWordLabel->SetToolTip(m_strKeyword.c_str());

	rcCalc.right = 400;
	CalcText(GetManager()->GetPaintDC(), rcCalc, szBuff, m_pSearchCountLabel->GetFont(), DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT|DT_NOPREFIX, UIFONT_GDI);
	m_pSearchCountLabel->SetFixedWidth(rcCalc.GetWidth());
	m_pSearchCountLabel->SetText(szBuff);
}


void CSearchWindowUI::CheckBoxSelectAll( bool bSelected )
{
	for(int i = 0 ;i < m_vecCheckBoxs.size(); i++)
	{
		if(m_vecCheckBoxs[i]->GetName() != _T("boxall"))
		{
			m_vecCheckBoxs[i]->Selected(bSelected);
		}
	}
}

void CSearchWindowUI::SetAllCheckBox( bool bSelected )
{
	for(int i = 0 ;i < m_vecCheckBoxs.size(); i++)
	{
		if(m_vecCheckBoxs[i]->GetName() == _T("boxall"))
		{
			m_vecCheckBoxs[i]->Selected(bSelected);
			break;
		}
		
	}
}

bool CSearchWindowUI::OnCheckBoxALLNotify( void * pObj )
{
	TNotifyUI* pNotify = (TNotifyUI *)pObj;
	if(pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		CCheckBoxUI* pCheckBox = dynamic_cast<CCheckBoxUI*>(pNotify->pSender);
		m_bSelectAllCheckBox = true;
		CheckBoxSelectAll(!pCheckBox->IsSelected());
	}
	return true;
}

bool CSearchWindowUI::OnCheckBoxNotify( void * pObj )
{
	TNotifyUI* pNotify = (TNotifyUI *)pObj;
	if(pNotify->sType == DUI_MSGTYPE_CLICK)
	{
		m_bSelectAllCheckBox = false;
	}
	if(pNotify->sType == DUI_MSGTYPE_SELECTCHANGED)
	{
		CCheckBoxUI* pCheckBox = dynamic_cast<CCheckBoxUI*>(pNotify->pSender);
		if(pCheckBox->GetName() == _T("boxcourse"))
		{
			if(pCheckBox->IsSelected())
				m_dwCheckBoxFlag |= CheckBox_Course;
			else
				m_dwCheckBoxFlag &= ~CheckBox_Course;
		}
		else if(pCheckBox->GetName() == _T("boximage"))
		{
			if(pCheckBox->IsSelected())
				m_dwCheckBoxFlag |= CheckBox_Image;
			else
				m_dwCheckBoxFlag &= ~CheckBox_Image;
		}
		else if(pCheckBox->GetName() == _T("boxvideo"))
		{
			if(pCheckBox->IsSelected())
				m_dwCheckBoxFlag |= CheckBox_Video;
			else
				m_dwCheckBoxFlag &= ~CheckBox_Video;
		}
		else if(pCheckBox->GetName() == _T("boxflash"))
		{
			if(pCheckBox->IsSelected())
				m_dwCheckBoxFlag |= CheckBox_Flash;
			else
				m_dwCheckBoxFlag &= ~CheckBox_Flash;
		}
		else if(pCheckBox->GetName() == _T("boxvolume"))
		{
			if(pCheckBox->IsSelected())
				m_dwCheckBoxFlag |= CheckBox_Volume;
			else
				m_dwCheckBoxFlag &= ~CheckBox_Volume;
		}
		else if(pCheckBox->GetName() == _T("boxquestion"))
		{
			if(pCheckBox->IsSelected())
				m_dwCheckBoxFlag |= CheckBox_Questions;
			else
				m_dwCheckBoxFlag &= ~CheckBox_Questions;
		}
		else if(pCheckBox->GetName() == _T("boxcoursewareobjests"))
		{
			if(pCheckBox->IsSelected())
				m_dwCheckBoxFlag |= CheckBox_CoursewareObjects;
			else
				m_dwCheckBoxFlag &= ~CheckBox_CoursewareObjects;
		}
		else if(pCheckBox->GetName() == _T("boxppttemplate"))
		{
			if(pCheckBox->IsSelected())
				m_dwCheckBoxFlag |= CheckBox_PPTTemplate;
			else
				m_dwCheckBoxFlag &= ~CheckBox_PPTTemplate;
		}
		else if(pCheckBox->GetName() == _T("boxother"))
		{
			if(pCheckBox->IsSelected())
				m_dwCheckBoxFlag |= CheckBox_Other;
			else
				m_dwCheckBoxFlag &= ~CheckBox_Other;
		}
		if(!m_bSelectAllCheckBox)
		{
			if(m_dwCheckBoxFlag == CheckBox_ALL)
				SetAllCheckBox(true);
			else
				SetAllCheckBox(false);
		}
	}

	return true;
}

bool CSearchWindowUI::HideTipListDlg( void* pObj )
{
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(CGroupExplorerUI::GetInstance()->GetHWND(), &point);
	m_SearchTipListDlg.ShowWindow(false);
	
	if(!PtInRect(&m_pTipLayout->GetPos(), point))
		m_bShowTipListDlg = true;
	else
		m_bShowTipListDlg = false;

	return true;
}

bool CSearchWindowUI::SelectSearchType( void* pObj )
{
	TNotifyUI* pNotify = (TNotifyUI*)pObj;
	CButtonUI* pButton = dynamic_cast<CButtonUI*>(pNotify->pSender);
	if(pButton->GetName() == _T("tipListPPT"))
	{
		m_dwSearchType = Search_Type_101PPT;
		m_pTipLabel->SetBkImage(_T("file='RightBar\\icon_logo_PPT.png' dest='8,8,27,27'"));
	}
	else if(pButton->GetName() == _T("tipListBaidu"))
	{
		m_dwSearchType = Search_Type_Baidu;
		m_pTipLabel->SetBkImage(_T("file='RightBar\\icon_Baidu.png' dest='8,8,27,27'"));
	}
	m_SearchTipListDlg.ShowWindow(false);

	return true;
}

void CSearchWindowUI::SetClickCallBack( CDelegateBase& delegate )
{
	m_OnClickCallBack.clear();
	m_OnClickCallBack += delegate;
}

void CSearchWindowUI::CalcText( HDC hdc, RECT& rc, LPCTSTR lpszText, int nFontId, UINT format, UITYPE_FONT nFontType, int c /*= -1*/ )
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

void CSearchWindowUI::ClearDownloadThreads()
{
	for(int i = 0 ; i< m_vecDownloadThreads.size(); i++)
	{
		NDCloudDownloadCancel(m_vecDownloadThreads[i]);
	}
	m_vecDownloadThreads.clear();
}

BOOL CSearchWindowUI::CheckNetStatus()
{
	DWORD flags;//上网方式
	BOOL bOnline=TRUE;//是否在线 
	bOnline=InternetGetConnectedState(&flags,0); 

	return bOnline;
}
