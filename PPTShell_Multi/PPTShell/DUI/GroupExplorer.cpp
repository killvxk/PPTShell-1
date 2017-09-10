#include "stdafx.h"
#include "GroupExplorer.h"
#include "CourseItem.h"
#include "VideoItem.h"
#include "PhotoItem.h"
#include "FlashItem.h"
#include "MusicItem.h"
#include "QuestionItem.h"
#include "CoursewareObjectsItem.h"
#include "NdpCourseItem.h"
#include "NdpxCourseItem.h"
#include "3DResourceItem.h"

#include "LocalCourseItem.h"
#include "LocalVideoItem.h"
#include "LocalPhotoItem.h"
#include "LocalFlashItem.h"
#include "LocalMusicItem.h"
#include "LocalQuestionItem.h"
#include "LocalInteractiveQuestionItem.h"

#include "NDCloud/NDCloudAPI.h"
#include "NDCloud/NDCloudUser.h"


#include "DUI/LabelMulti.h"
#include "DUI/TileWithButtonLayout.h"

#include "EventCenter/EventDefine.h"
#include "ListBarDlg.h"
#include "LocalItem.h"
#include "Util/FileTypeFilter.h"
#include <algorithm>
#include "Effect/ViewAnimation.h"
#include "Effect/ActionManager.h"
#include "DBankUploadUI.h"
#include "DUI/IDownloaderListener.h"
#include "GUI/MainFrm.h"
#include <regex>
#include "Util/Stream.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "MenuUI.h"
#include "RenameDlgUI.h"
#include "Statistics/Statistics.h"


#include "DUI/IThumbnailListener.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IDownloadListener.h"
#include "Util/Util.h"
#include "Util/Stream.h"
#include "DUI/IStreamReader.h"
#include "DUI/IStyleable.h"
#include "DUI/IVisitor.h"
#include "DUI/StyleItem.h"
#include "DUI/IComponent.h"
#include "DUI/IVisitor.h"
#include "DUI/INotifyHandler.h"
#include "DUI/IItemHandler.h"

#include "DUI/ItemHandler.h"

#include "ResourceItemEx.h"

#include "DUI/CloudResourceHandler.h"
#include "DUI/CloudPhotoHandler.h"
#include "DUI/CloudVideoHandler.h"

#include "DUI/SeeMoreVRDialog.h"

CGroupExplorerUI* CGroupExplorerUI::m_pInstance = NULL;
CGroupExplorerUI* CGroupExplorerUI::GetInstance()
{
	if (!m_pInstance)
	{
		m_pInstance = new CGroupExplorerUI;
	}
	return m_pInstance;
}

CGroupExplorerUI::CGroupExplorerUI()
{	
	m_pItemExplorer = CItemExplorerUI::GetInstance();
	m_nCurrentType	= -1;
	m_pGif			= NULL;
	m_pListBar		= NULL;
	m_pDlgImport	= NULL;
	m_pEmptyLayout	= NULL;
	m_pImportLayout = NULL;
	m_layResless	= NULL;
	m_pItemPageUI	= NULL;
	m_pMenuUI = NULL;

	m_bImporting	= false;
	m_bLayoutChanged= false;

	m_pLoginWindowUI = NULL;
	m_pRegisterWindowUI = NULL;
	m_pChangePasswordWindowUI = NULL;
	m_pGetBackPwdWindowUI=NULL;
	m_pEmailTipLayout=NULL;
	m_layResless=NULL;
	m_pGif=NULL;
	m_pDlgImport=NULL;
	m_pListBar=NULL;
	m_pEmptyLayout;
	m_pImportLayout=NULL;
	m_pImportTipLabelOne=NULL;
	m_pImportTipLabelTwo=NULL;
	m_pTipLabel=NULL;
	m_pCountLabel=NULL;
	m_pImportProgress=NULL;
	m_pLoginList=NULL;
	m_pChangePasswordList=NULL;
	m_pGetBackPwdList=NULL;
	m_pLabelCloudCount=NULL;
	m_pTabLayoutCloudPage=NULL;
	m_pTabLayoutLocalPage=NULL;
	m_pSearchMainwindow=NULL;
	m_pSearchFailwindow=NULL;
	m_pSearchWindowUI=NULL;
	m_pCloudSearchContainer=NULL;
	m_pLocalSearchContainer=NULL;
	m_pSearchNetless=NULL;
	m_pSearchFailContainer=NULL;
//	m_pSearchWindow=NULL;
	m_pItemPageUI=NULL;
	m_pMenuUI=NULL;

	m_nLocalItemType	= FILE_FILTER;

	m_strChapterGuid	= _T("");
	m_strChapter		= _T("");

	OnEvent(EVT_LOCAL_SEARCH_SWITCH_STATUS, MakeEventDelegate(this, &CGroupExplorerUI::OnLocalSearchSwitchStatus));
	OnEvent(EVT_MENU_DOWNLOADLOCAL, MakeEventDelegate(this, &CGroupExplorerUI::DownLoadToLocal));
	OnEvent(EVT_MENU_DOWNLOADLOCAL_COMPELETE, MakeEventDelegate(this, &CGroupExplorerUI::DownLoadToLocalCompelete));
	OnEvent(EVT_MENU_UPLOADNETDISC, MakeEventDelegate(this, &CGroupExplorerUI::UploadNetdisc));
	OnEvent(EVT_MENU_UPLOADNETDISC_COMPELETE, MakeEventDelegate(this, &CGroupExplorerUI::UploadNetdiscCompelete));
	OnEvent(EVT_MENU_DELETE, MakeEventDelegate(this, &CGroupExplorerUI::DeleteResource));
	OnEvent(EVT_MENU_DELETE_COMPELETE, MakeEventDelegate(this, &CGroupExplorerUI::DeleteResourceCompelete));
	OnEvent(EVT_MENU_RENAME,MakeEventDelegate(this,&CGroupExplorerUI::RenameResource));
	OnEvent(EVT_MENU_RENAME_COMPELETE,MakeEventDelegate(this,&CGroupExplorerUI::RenameResourceCompelete));
	OnEvent(EVT_MENU_EDITEXERCISES,MakeEventDelegate(this,&CGroupExplorerUI::EditExercises));
}

CGroupExplorerUI::~CGroupExplorerUI()
{
	CancelEvent(EVT_LOCAL_SEARCH_SWITCH_STATUS, MakeEventDelegate(this, &CGroupExplorerUI::OnLocalSearchSwitchStatus));
	CancelEvent(EVT_MENU_DOWNLOADLOCAL, MakeEventDelegate(this, &CGroupExplorerUI::DownLoadToLocal));
	CancelEvent(EVT_MENU_DOWNLOADLOCAL_COMPELETE, MakeEventDelegate(this, &CGroupExplorerUI::DownLoadToLocalCompelete));
	CancelEvent(EVT_MENU_UPLOADNETDISC, MakeEventDelegate(this, &CGroupExplorerUI::UploadNetdisc));
	CancelEvent(EVT_MENU_UPLOADNETDISC_COMPELETE, MakeEventDelegate(this, &CGroupExplorerUI::UploadNetdiscCompelete));
	CancelEvent(EVT_MENU_DELETE, MakeEventDelegate(this, &CGroupExplorerUI::DeleteResource));
	CancelEvent(EVT_MENU_DELETE_COMPELETE, MakeEventDelegate(this, &CGroupExplorerUI::DeleteResourceCompelete));
	CancelEvent(EVT_MENU_RENAME,MakeEventDelegate(this,&CGroupExplorerUI::RenameResource));
	CancelEvent(EVT_MENU_RENAME_COMPELETE,MakeEventDelegate(this,&CGroupExplorerUI::RenameResourceCompelete));
	CancelEvent(EVT_MENU_EDITEXERCISES,MakeEventDelegate(this,&CGroupExplorerUI::EditExercises));
}

LPCTSTR CGroupExplorerUI::GetWindowClassName() const
{
	return _T("CGroupExplorerUI"); 
}

UINT CGroupExplorerUI::GetClassStyle() const
{
	return CS_DBLCLKS;
}

void CGroupExplorerUI::InitWindow()
{
	__super::InitWindow();

	ChangeWindowMessageFilterForWin8();

	m_layResless = dynamic_cast<CVerticalLayoutUI*>(m_styleBuilder.Create(_T("RightBar\\Explorer\\ResourceExplorer_ReslessStyle.xml"), (UINT)0, NULL, &m_PaintManager));
	if (m_layResless)
	{
		m_pBody->AddAt(m_layResless, 0);
	}

	m_pLoginList = dynamic_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("loginContent")));
	m_pChangePasswordList = dynamic_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("changePasswordContent")));
	m_pGetBackPwdList = dynamic_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("getBackPwdContent")));
	m_pCloudAssetsList = dynamic_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("cloudAssetscontent")));
	m_pDBankAssetsList = dynamic_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("dbankAssetscontent")));
	m_pSearchList = dynamic_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("searchcontent")));
	m_laySeeMore	= static_cast<CContainerUI*>(m_PaintManager.FindControl(_T("lay_see_more")));

	//GetRefreshBtn()->OnNotify += MakeDelegate(this,&CListBarDlg::OnBtnRefresh);
	//m_pItemExplorer->Create(m_hParentHwnd, _T("ItemExplorer"), WS_POPUP, 0, 0, 0, 0,0);
}

CControlUI* CGroupExplorerUI::CreateControl(LPCTSTR pstrClass)
{

	if( _tcscmp(pstrClass, _T("ResourceItem")) == 0 )
	{
		switch (m_nCurrentType)
		{
		case CloudFileCourse:
		case DBankCourse:
			return new CCourseItemUI;
		case CloudFileVideo:
		case DBankVideo:
			return new CVideoItemUI;
		case CloudFileImage:
		case DBankImage:
			return new CPhotoItemUI;
		case CloudFileFlash:
		case DBankFlash:
			return new CFlashItemUI;
		case CloudFileVolume:
		case DBankVolume:
			return new CMusicItemUI;
		case CloudFileQuestion:
		case DBankQuestion:
			return new CQuestionItemUI;
		case CloudFileCoursewareObjects:
		case DBankCoursewareObjects:
			return new CCoursewareObjectsItemUI;
		case CloudFileNdpCourse:
			return new CNdpCourseItemUI;
		case CloudFile3DResource:
			return new C3DResourceItemUI;
		case CloudFileNdpxCourse:
			return new CNdpxCourseItemUI;
		}
	}
	else if( _tcscmp(pstrClass, _T("LocalItem")) == 0 )
	{
		switch (m_nLocalItemType)
		{
		case FILE_FILTER_PPT:
			return new CLocalCourseItemUI;
		case FILE_FILTER_VIDEO:
			return new CLocalVideoItemUI;
		case FILE_FILTER_PIC:
			return new CLocalPhotoItemUI;
		case FILE_FILTER_FLASH:
			return new CLocalFlashItemUI;
		case FILE_FILTER_VOLUME:
			return new CLocalMusicItemUI;
		case FILE_FILTER_BASIC_EXERCISES:
			return new CLocalQuestionItemUI;
		case FILE_FILTER_INTERACTIVE_EXERCISES:
			return new CLocalInteractiveQuestionItemUI;
		}
	}
	else if( _tcscmp(pstrClass, _T("EditClear")) == 0 )
		return new CEditClearUI;
	else if( _tcscmp(pstrClass, _T("TabLayoutCloudPage")) == 0 )
	{
		m_pTabLayoutCloudPage = new CTabLayoutPageUI;
		return m_pTabLayoutCloudPage;
	}
	else if( _tcscmp(pstrClass, _T("TabLayoutLocalPage")) == 0 )
	{
		m_pTabLayoutLocalPage = new CTabLayoutLocalPageUI;
		return m_pTabLayoutLocalPage;
	}
	else if( _tcscmp(pstrClass, _T("LabelMulti")) == 0 )
		return new CLabelMultiUI;
	else if( _tcscmp(pstrClass, _T("LoginWindowUI")) == 0 )
		return new CLoginWindowUI;
	else if( _tcscmp(pstrClass, _T("RegisterWindowUI")) == 0 )
		return new CRegisterWindowUI;
	else if( _tcscmp(pstrClass, _T("ChangePasswordWindowUI")) == 0 )
		return new CChangePasswordWindowUI;
	else if( _tcscmp(pstrClass, _T("GetBackPwdWindowUI")) == 0 )
		return new CGetBackPwdWindowUI;
	else if( _tcscmp(pstrClass, _T("AssetsWindowUI")) == 0 )
		return new CAssetsWindowUI;
	else if( _tcscmp(pstrClass, _T("DBankAssetsWindowUI")) == 0 )
		return new CDBankAssetsWindowUI;
	else if( _tcscmp(pstrClass, _T("SearchWindowUI")) == 0 )
		return new CSearchWindowUI;
	else if( _tcscmp(pstrClass, _T("TileWithButtonLayout")) == 0 )
		return new CTileWithButtonLayoutUI;
	else if(_tcsicmp(_T("DBankUpload"), pstrClass) == 0)
	{
		return new CDBankUploadUI;
	}

	return __super::CreateControl(pstrClass);
}

void CGroupExplorerUI::SetDlgAcceptFiles(bool bAccept)
{
	if (bAccept)
	{
		LONG styleValue = ::GetWindowLong(GetHWND(), GWL_EXSTYLE); 
		styleValue |= WS_EX_ACCEPTFILES;
		::SetWindowLong(GetHWND(), GWL_EXSTYLE, styleValue); 
	}
	else
	{
		LONG styleValue = ::GetWindowLong(GetHWND(), GWL_EXSTYLE); 
		styleValue &= (~WS_EX_ACCEPTFILES);
		::SetWindowLong(GetHWND(), GWL_EXSTYLE, styleValue); 
	}
}

bool compare_Chapter(const LOCAL_RES_INFO &resA, const LOCAL_RES_INFO &resB)
{   
	return (_tcsicmp(resA.strChapter.c_str(), resB.strChapter.c_str()) < 0); 
}

void CGroupExplorerUI::ShowResource( int nType, CStream* pStream, void* pParams )
{
	m_pSliderTabLayout->SelectItem(1, false);
	if (nType < CloudFileSearch)
	{
		return;
	}

	if (nType == CloudFileVRResource)
	{
		m_laySeeMore->SetVisible(true);
	}
	else
	{
		m_laySeeMore->SetVisible(false);
	}

	if (LocalFileImport<=nType && nType<=LocalFileAnother)		// 支持文件拖拽
		SetDlgAcceptFiles(true);
	else
		SetDlgAcceptFiles(false);

	if (m_nCurrentType == nType 
		&& (nType < LocalFileImport || nType == DBankSearch))
	{
		this->StopMask();
		if(!IsWindowVisible(GetHWND()))
			this->ShowWindow(true);

		if (nType == CloudFileCourse
			|| nType == CloudFilePPTTemplate
			|| nType == LocalFileCourse
			|| nType == DBankCourse)
		{
			if (m_pList->GetCount() > 0)
			{
				ShowNetlessUI(false);

				IItemContainer* pContainer = dynamic_cast<IItemContainer*>(m_pList->GetItemAt(0));
				if (pContainer && pContainer->HasSubitems())
				{
					CResourceItemUI * pItem = (CResourceItemUI *)m_pList->GetItemAt(0);
					pItem->GetOption()->Selected(true);
					m_PaintManager.SendNotify(pItem->GetOption(), DUI_MSGTYPE_CLICK);
				}
				else
				{
					CResourceItemExUI* pItem = dynamic_cast<CResourceItemExUI*>(m_pList->GetItemAt(0));
					if (pItem 
						&& pItem->HasSubItems())
					{
						pItem->GetOption()->Selected(true);
						m_PaintManager.SendNotify(pItem->GetOption(), DUI_MSGTYPE_CLICK);
					}
				}
			}
		}
		return;
	}

	if ( (m_nCurrentType == nType) && (nType >= LocalFileImport && nType <= LocalFileTotal) )
	{
		pStream->ResetCursor();
		int nCount = pStream->ReadDWORD();
		tstring	strFolderName = pStream->ReadString();

		if ( (GetTitleText() == strFolderName) && m_bImporting )
		{
			this->ShowWindow(true);
			return;
		}
	}

	bool  bIsDbank = false;
	bool  bIsTemplate = false;
	if (nType >= DBankCourse)
	{
		bIsDbank = true;
	}

	m_nCurrentType		= nType;
	if(nType == DBankSearch)
	{
		nType -= DBankSearch;
	}
	else if(nType > DBankUpload)
		nType -= DBankUpload;
	else if(nType == CloudFilePPTTemplate)
	{
		bIsTemplate = true;
		nType = CloudFileCourse;
	}

	if (nType < CloudFileTotal)
	{

		if (nType == CloudFileSearch)
		{
			this->ShowReslessUI(false);
			this->ShowNetlessUI(false);
			this->StopMask();
		}
		else
		{

			if (m_bLastShowNetless
				&& !pStream)
			{
				this->ShowNetlessUI(true);
			}


			if (m_bLastShowMask)
			{
				this->StartMask();
			}
		}

	}
	else
	{
		this->ShowNetlessUI(false);
		this->ShowReslessUI(false);
		this->StopMask();
	}

	
	m_bImporting		= false;
	m_bLayoutChanged	= true;

	if(IsWindowVisible(m_pItemExplorer->GetHWND()))
		m_pItemExplorer->ShowWindow(false);
	//ActionManager::GetInstance()->removeAllActions(); 
	//must remove sub control for item handler 
	//m_pList->SetDelayedDestroy(false);
	
	m_pList->RemoveAll();
	m_pList->Invalidate();
	//m_pList->SetDelayedDestroy(true);
	//clear last border
	m_pList->SetTag(NULL);
	m_pList->GetVerticalScrollBar()->SetScrollPos(0);
	m_pEmptyLayout	= NULL;
	m_pImportLayout = NULL;

	if (m_nCurrentType == CloudFileSearch || m_nCurrentType == LocalFileSearch || m_nCurrentType == DBankSearch)
	{
		/*
		m_jsonCloudKWRoot.clear();
		BOOL bShow = ::IsWindowVisible(GetHWND());

		SetTitleText(_T(""));
		SetCountText(_T(""));
		this->SetSearchWidth();

		m_pSearchWindow = CreateSearchWindow();
		CEditClearUI * pClearEdit = static_cast<CEditClearUI*>(m_pSearchWindow->FindSubControl(CLEAR_RICHEDIT_NAME));
		pClearEdit->SetHandleSpecialKeydown(true);
		m_pSearchWindow->OnDestroy += MakeDelegate(this, &CGroupExplorerUI::OnSearchWindowDestroy);
		m_pLabelCloudCount = static_cast<CLabelUI*>(m_pSearchWindow->FindSubControl(_T("searchCount")));

		m_pSearchMainwindow = static_cast<CVerticalLayoutUI*>(m_pSearchWindow->FindSubControl(_T("searchmainwindow")));
		m_pSearchFailwindow = static_cast<CVerticalLayoutUI*>(m_pSearchWindow->FindSubControl(_T("searchfailwindow")));
		m_pSearchNetless		=  static_cast<CVerticalLayoutUI*>(m_pSearchWindow->FindSubControl(_T("search_netless")));

		m_pSearchFailContainer = static_cast<CVerticalLayoutUI*>(m_pSearchFailwindow->FindSubControl(_T("searchcommand")));
		VERIFY_PTR_NOT_RET(m_pSearchFailContainer);

		m_pCloudSearchContainer = static_cast<CVerticalLayoutUI*>(m_pSearchWindow->FindSubControl(_T("cloudSearchContainer")));
		m_pLocalSearchContainer = static_cast<CVerticalLayoutUI*>(m_pSearchWindow->FindSubControl(_T("localSearchContainer")));

		if(m_nCurrentType == CloudFileSearch || m_nCurrentType == DBankSearch)
		{
			m_pCloudSearchContainer->SetVisible(true);
			m_pLocalSearchContainer->SetVisible(false);
		}
		else
		{
			m_pCloudSearchContainer->SetVisible(false);
			m_pLocalSearchContainer->SetVisible(true);
		}

		m_strCloudKWPath = GetLocalPath();
		if (m_nCurrentType == CloudFileSearch )
			m_strCloudKWPath += _T("\\Setting\\CloudSearchKeyWord.dat");
		else if (m_nCurrentType == DBankSearch )
			m_strCloudKWPath += _T("\\Setting\\DBankSearchKeyWord.dat");
		else
			m_strCloudKWPath += _T("\\Setting\\LocalSearchKeyWord.dat");

		FILE* fp = fopen(m_strCloudKWPath.c_str(), "rb");
		if( fp )
		{
			fseek(fp, 0, SEEK_END);
			int size = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			char* pBuff = new char[size + 1];
			pBuff[size] = 0;
			fread(pBuff, size, 1, fp);

			fclose(fp);

			bool bRet = m_jsonKeyWord.parse(pBuff,m_jsonCloudKWRoot);

			delete pBuff;
		}

		if(m_pSearchWindow)
		{
			m_pList->Add(m_pSearchWindow);
		}


		ShowSearchHistory();
		ShowWindow(true);
		*/
		return;
	}
	else if (m_nCurrentType >= LocalFileImport && m_nCurrentType <= LocalFileTotal)
	{
		BOOL bShow = ::IsWindowVisible(GetHWND());

		pStream->ResetCursor();
		int nCount				= pStream->ReadDWORD();
		tstring	strFolderName	= pStream->ReadString();

		if ( pParams != NULL )
		{
			CStream* pChapterStream	= (CStream*)pParams;
			m_strChapterGuid		= pChapterStream->ReadString();
			m_strChapter			= pChapterStream->ReadString();
		}

		SetTitleText(strFolderName.c_str());

		if ( m_nCurrentType == LocalFileImport )
			SetCountText(_T(""));
		else
			SetCountText(nCount);

		if ( nCount == 0 )
		{
			CContainerUI * pImprtWindow = CreateImportWindow();

			if(pImprtWindow)
			{
				m_pList->Add(pImprtWindow);
				GetImportWindowControl();
			}
		}
		else
		{
			list<LOCAL_RES_INFO>* pFileList = ((CListBarDlg*)m_pListBar)->GetLocalResFileList(strFolderName);
			if (pFileList == NULL )
				return;

			//使用GUID排序
			LocalResSortByChapter(pFileList, m_strChapter);

			list<LOCAL_RES_INFO>::iterator iter = pFileList->begin();
			CFileTypeFilter fileTypeFilter;

			if(m_pItemPageUI)
			{
				delete m_pItemPageUI;
				m_pItemPageUI = NULL;
			}
			m_pItemPageUI = new CItemPageUI;
			m_pItemPageUI->m_OnCreateCallback += MakeDelegate(this,&CGroupExplorerUI::OnCreateCallback);
			
			if(m_nCurrentType==LocalFileBasicExercises||m_nCurrentType==LocalFileInteractiveExercises)
			{
				for (iter; iter != pFileList->end(); iter++)  //zcs 11-26
				{
					LOCAL_RES_INFO resInfo	= *iter;
					tstring strPath			= resInfo.strResPath;
					int nPos				= strPath.rfind(_T("\\"));
					tstring	strTitle		= resInfo.strTitle;
					if (strTitle.empty()&&!strPath.empty())
					{
						string fileName = strPath.substr(strPath.find_last_of(_T("\\"))+1);
						strTitle			= fileName.substr(0, fileName.find_last_of(_T(".")));
					}

					int nLocalItemType		= fileTypeFilter.GetFileType(strPath);

					if ( nLocalItemType == FILE_FILTER_OTHER )
						continue;

					if ( nLocalItemType == FILE_FILTER_BASIC_EXERCISES && strPath.rfind(_T("\\interaction\\")) != tstring::npos )
						nLocalItemType = FILE_FILTER_INTERACTIVE_EXERCISES;


					ITEM_PAGE itemPage;
					itemPage.nItemType	= nLocalItemType;
					itemPage.strPath	= strPath;
					itemPage.strName	= strTitle;
					itemPage.strChapter	= resInfo.strChapter; 
					itemPage.strGroup	= _T(_T("Locals"));
					m_pItemPageUI->AddSrcData(itemPage);
				}
			}
			else
			{
				vector<tstring> vMissPath;
				for (iter; iter != pFileList->end(); iter++)  //zcs 11-26
				{
					LOCAL_RES_INFO resInfo	= *iter;
					tstring strPath			= resInfo.strResPath;
					DWORD dwAttri = ::GetFileAttributes(strPath.c_str());
					if (dwAttri != -1)
					{
						int nPos				= strPath.rfind(_T("\\"));
						tstring	strTitle		= resInfo.strTitle;
						if (strTitle.empty()&&!strPath.empty())
						{
							string fileName = strPath.substr(strPath.find_last_of("\\")+1);
							strTitle			= fileName.substr(0, fileName.find_last_of(_T(".")));
						}

						int nLocalItemType		= fileTypeFilter.GetFileType(strPath);

						if ( nLocalItemType == FILE_FILTER_OTHER )
							continue;

						if ( nLocalItemType == FILE_FILTER_BASIC_EXERCISES && strPath.rfind(_T("\\interaction\\")) != tstring::npos )
							nLocalItemType = FILE_FILTER_INTERACTIVE_EXERCISES;


						ITEM_PAGE itemPage;
						itemPage.nItemType	= nLocalItemType;
						itemPage.strPath	= strPath;
						itemPage.strName	= strTitle;
						itemPage.strChapter	= resInfo.strChapter; 
						itemPage.strGroup	= _T("Locals");
						m_pItemPageUI->AddSrcData(itemPage);
					}
					else
					{
						nCount--;
						vMissPath.push_back(strPath);
					}
				}
				for(int i=0;i<vMissPath.size();i++)
				{
					int nCount = 0;
					bool bRet = ((CListBarDlg*)m_pListBar)->LocalResItemDelFile(GetTitleText(), vMissPath[i], nCount);
				}
			}
			if(nCount>0)
			{
				m_pItemPageUI->Create();
				m_pList->Add(m_pItemPageUI->GetMainLayout());
				//select first one 
				IItemContainer* pContainer = dynamic_cast<IItemContainer*>(m_pItemPageUI->GetContentLayout()->GetItemAt(0));
				if (pContainer && pContainer->HasSubitems())
				{
					CResourceItemUI * pItem = (CResourceItemUI *)pContainer;
					pItem->GetOption()->Selected(true);
					m_PaintManager.SendNotify(pItem->GetOption(), DUI_MSGTYPE_CLICK);
				}
			}
			else
			{
				CContainerUI * pImprtWindow = CreateImportWindow();
				if(pImprtWindow)
				{
					m_pList->Add(pImprtWindow);
					GetImportWindowControl();
				}
			}
			SetCountText(nCount);
		}

		this->ExtendWidth();
		ShowWindow(true);

		return;
	}

	if (nType == CloudFileCourse)
	{
		this->ResetWidth();
	}
	else
	{
		this->ExtendWidth();
	}

	//test code
	if (!pStream)
	{
		int nCount = 0;
		SetCountText(nCount);
		ShowWindow(true);
		return;
	}

	pStream->ResetCursor();
	int nCount = pStream->ReadInt();
	SetCountText(nCount);
	int nTempType ;//临时保存Type
	nTempType = m_nCurrentType;
	for (int i = 0; i < nCount; i++)
	{
		if (nType == CloudFileCourse)
		{
			int nNdpType = pStream->ReadInt();
			if(nNdpType == 0 )
				m_nCurrentType = CloudFileCourse;

			else if( nNdpType == 1 )
				m_nCurrentType = CloudFileNdpCourse;
			
			else
				m_nCurrentType = CloudFileNdpxCourse;
		}
		if (m_nCurrentType == CloudFileImage
			|| m_nCurrentType == DBankImage
			|| m_nCurrentType == CloudFileVideo
			|| m_nCurrentType == DBankVideo
			|| m_nCurrentType == CloudFileVolume
			|| m_nCurrentType == DBankVolume
			|| m_nCurrentType == CloudFileFlash
			|| m_nCurrentType == CloudFile3DResource
			|| m_nCurrentType == CloudFileQuestion
			|| m_nCurrentType == DBankFlash
			|| m_nCurrentType == CloudFileCourse
			|| m_nCurrentType == DBankCourse
			|| m_nCurrentType == CloudFileQuestion
			|| m_nCurrentType == DBankQuestion
			|| m_nCurrentType == CloudFileCoursewareObjects
			|| m_nCurrentType == DBankCoursewareObjects
			|| m_nCurrentType == CloudFilePPTTemplate
			|| m_nCurrentType == CloudFileNdpCourse
			|| m_nCurrentType == CloudFileNdpxCourse
			|| m_nCurrentType == CloudFileVRResource
			)
		{

			int nTempCurrentType = nTempType;
			if (nType == CloudFileCourse)
			{
				nTempCurrentType = m_nCurrentType;
				if (!bIsTemplate)
				{
					if (bIsDbank)
					{
						nTempCurrentType += DBankUpload;
					}
				}
				else
				{
					nTempCurrentType = nTempType;
				}
			}

			CResourceItemExUI* pItem = CResourceItemExUI::AllocResourceItem(nTempCurrentType);

			if (pItem)
			{
				pItem->SetPadding(CDuiRect(6, 0, 6, 0));
				pItem->ReadStream(pStream);

				m_pList->Add(pItem);

			}
		}
		else
		{
			CCloudItemUI * pItem = CreateItem();
			if (pItem)
			{
				pItem->SetPadding(CDuiRect(6, 0, 6, 0));
				pItem->SetName(_T("CloudItem"));
				pItem->ReadStream(pStream);
				m_pList->Add(pItem);

			}

		}
	}
	//还原Type
	m_nCurrentType = nTempType;


	StopMask();
	ShowWindow(true);

	if (m_pList->GetCount() > 0)
	{
		ShowNetlessUI(false);
		ShowReslessUI(false);
		IItemContainer* pContainer = dynamic_cast<IItemContainer*>(m_pList->GetItemAt(0));
		if (pContainer && pContainer->HasSubitems())
		{
			CResourceItemUI * pItem = (CResourceItemUI *)m_pList->GetItemAt(0);
			pItem->GetOption()->Selected(true);
			m_PaintManager.SendNotify(pItem->GetOption(), DUI_MSGTYPE_CLICK);
		}
		else
		{
			CResourceItemExUI* pItem = dynamic_cast<CResourceItemExUI*>(m_pList->GetItemAt(0));
			if (pItem 
				&& pItem->HasSubItems())
			{
				pItem->GetOption()->Selected(true);
				m_PaintManager.SendNotify(pItem->GetOption(), DUI_MSGTYPE_CLICK);
			}
		}
	}
	else
	{
		ShowReslessUI(true);
	}
}

CCloudItemUI* CGroupExplorerUI::CreateItem()
{
	CCloudItemUI * pItem = NULL;

	if( !m_builder.GetMarkup()->IsValid() ) {
		pItem = dynamic_cast<CCloudItemUI*>(m_builder.Create(_T("RightBar\\Item\\ResourceItem.xml"), (UINT)0, this, &m_PaintManager));
	}
	else {
		pItem = dynamic_cast<CCloudItemUI*>(m_builder.Create(this, &m_PaintManager));
	}
	if (pItem == NULL)
		return NULL;
	return pItem;
}


void CGroupExplorerUI::ExtendWidth()
{
	CPanelInnerDlg*		m_pAttchedDlg = dynamic_cast<CPanelInnerDlg*>(CWnd::FromHandle(this->GetHWND()));
	if (m_pAttchedDlg)
	{
		m_pAttchedDlg->SetInnerWidth(200);
	}

}

void CGroupExplorerUI::ResetWidth()
{
	CPanelInnerDlg*		m_pAttchedDlg = dynamic_cast<CPanelInnerDlg*>(CWnd::FromHandle(this->GetHWND()));
	if (m_pAttchedDlg)
	{
		m_pAttchedDlg->SetInnerWidth(150);
	}
}

void CGroupExplorerUI::SetSearchWidth()
{
	CPanelInnerDlg*		m_pAttchedDlg = dynamic_cast<CPanelInnerDlg*>(CWnd::FromHandle(this->GetHWND()));
	if (m_pAttchedDlg)
	{
		m_pAttchedDlg->SetInnerWidth(350);
	}
}

CSearchWindowUI* CGroupExplorerUI::CreateSearchWindow()
{
	CSearchWindowUI * pSearchWindow = NULL;

	if( !m_searchBuilder.GetMarkup()->IsValid() ) {
		pSearchWindow = dynamic_cast<CSearchWindowUI*>(m_searchBuilder.Create(_T("RightBar\\SearchWindow.xml"), (UINT)0, this, &m_PaintManager));
	}
	else {
		pSearchWindow = dynamic_cast<CSearchWindowUI*>(m_searchBuilder.Create(this, &m_PaintManager));
	}
	if (pSearchWindow == NULL)
		return NULL;

	return pSearchWindow;
}

CLoginWindowUI* CGroupExplorerUI::CreateLoginWindow()
{
	CLoginWindowUI * pLoginWindow = NULL;

	if( !m_loginBuilder.GetMarkup()->IsValid() ) {
		pLoginWindow = dynamic_cast<CLoginWindowUI*>(m_loginBuilder.Create(_T("RightBar\\LoginWindow.xml"), (UINT)0, this, &m_PaintManager));
	}
	else {
		pLoginWindow = dynamic_cast<CLoginWindowUI*>(m_loginBuilder.Create(this, &m_PaintManager));
	}
	if (pLoginWindow == NULL)
		return NULL;

	return pLoginWindow;
}


CRegisterWindowUI* CGroupExplorerUI::CreateRegisterWindow()
{
	CRegisterWindowUI * pRegisterWindow = NULL;

	if( !m_registerBuilder.GetMarkup()->IsValid() ) {
		pRegisterWindow = dynamic_cast<CRegisterWindowUI*>(m_registerBuilder.Create(_T("RightBar\\RegisterWindow.xml"), (UINT)0, this, &m_PaintManager));
	}
	else {
		pRegisterWindow = dynamic_cast<CRegisterWindowUI*>(m_registerBuilder.Create(this, &m_PaintManager));
	}
	if (pRegisterWindow == NULL)
		return NULL;

	return pRegisterWindow;
}


CChangePasswordWindowUI* CGroupExplorerUI::CreateChangePasswordWindow()
{
	CChangePasswordWindowUI * pChangePasswordWindow = NULL;

	if( !m_changePasswordBuilder.GetMarkup()->IsValid() ) {
		pChangePasswordWindow = dynamic_cast<CChangePasswordWindowUI*>(m_changePasswordBuilder.Create(_T("RightBar\\ChangePasswordWindow.xml"), (UINT)0, this, &m_PaintManager));
	}
	else {
		pChangePasswordWindow = dynamic_cast<CChangePasswordWindowUI*>(m_changePasswordBuilder.Create(this, &m_PaintManager));
	}
	if (pChangePasswordWindow == NULL)
		return NULL;

	return pChangePasswordWindow;
}

CGetBackPwdWindowUI* CGroupExplorerUI::CreateGetBackPwdWindow()
{
	CGetBackPwdWindowUI * pGetBackPwdWindow = NULL;

	if( !m_getBackPwdBuilder.GetMarkup()->IsValid() ) {
		pGetBackPwdWindow = dynamic_cast<CGetBackPwdWindowUI*>(m_getBackPwdBuilder.Create(_T("RightBar\\GetBackPwdWindow.xml"), (UINT)0, this, &m_PaintManager));
	}
	else {
		pGetBackPwdWindow = dynamic_cast<CGetBackPwdWindowUI*>(m_getBackPwdBuilder.Create(this, &m_PaintManager));
	}
	if (pGetBackPwdWindow == NULL)
		return NULL;

	return pGetBackPwdWindow;
}

CContainerUI* CGroupExplorerUI::CreateRegisterEmailSuccessWindow()
{
	CContainerUI * pRegisterEmailSuccessWindow = NULL;

	if( !m_registerEmailBuilder.GetMarkup()->IsValid() ) {
		pRegisterEmailSuccessWindow = dynamic_cast<CContainerUI*>(m_registerEmailBuilder.Create(_T("RightBar\\RegisterEmailSuccessWindow.xml"), (UINT)0, this, &m_PaintManager));
	}
	else {
		pRegisterEmailSuccessWindow = dynamic_cast<CContainerUI*>(m_registerEmailBuilder.Create(this, &m_PaintManager));
	}
	if (pRegisterEmailSuccessWindow == NULL)
		return NULL;

	return pRegisterEmailSuccessWindow;
}

CContainerUI* CGroupExplorerUI::CreateImportWindow()
{
	CContainerUI * pImportWindow = NULL;

	if( !m_localBuilder.GetMarkup()->IsValid() ) {
		pImportWindow = dynamic_cast<CContainerUI*>(m_localBuilder.Create(_T("RightBar\\ImportWindow.xml"), (UINT)0, this, &m_PaintManager));
	}
	else {
		pImportWindow = dynamic_cast<CContainerUI*>(m_localBuilder.Create(this, &m_PaintManager));
	}
	if (pImportWindow == NULL)
		return NULL;

	return pImportWindow;
}

bool CGroupExplorerUI::GetImportWindowControl()
{
	m_pEmptyLayout		= dynamic_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("EmptyLayout")));

	if ( m_pEmptyLayout == NULL )
		return false;

	m_pImportTipLabelOne = dynamic_cast<CLabelUI*>(m_pEmptyLayout->FindSubControl(_T("ImportTipOne")));

	if ( m_nCurrentType == LocalFileImport )
	{
		m_pImportTipLabelOne->SetText(_T("导入文件将自动分类"));
		m_pImportTipLabelOne->SetTextColor(0x00FD6347);
	}
	else
	{
		m_pImportTipLabelOne->SetText(_T("文件夹是空的哟"));
		m_pImportTipLabelOne->SetTextColor(0xFFFFFFFF);
	}

	if ( m_nCurrentType == LocalFileBasicExercises || m_nCurrentType == LocalFileInteractiveExercises )
	{
		CButtonUI* pBtn = dynamic_cast<CButtonUI*>(m_pEmptyLayout->FindSubControl(_T("ImportBtn")));
		
		if ( pBtn != NULL )
			pBtn->SetVisible(false);
	}

	m_pImportTipLabelTwo = dynamic_cast<CLabelUI*>(m_pEmptyLayout->FindSubControl(_T("ImportTipTwo")));

	m_pImportLayout		= dynamic_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("ImportLayout")));

	if ( m_pImportLayout == NULL )
		return false;
	
	m_pTipLabel		= dynamic_cast<CLabelUI*>(m_pImportLayout->FindSubControl(_T("TipLbl")));

	if ( m_pTipLabel == NULL )
		return false;

	m_pCountLabel		= dynamic_cast<CLabelUI*>(m_pImportLayout->FindSubControl(_T("CountLbl")));

	if ( m_pCountLabel == NULL )
		return false;

	m_pImportProgress	= dynamic_cast<CProgressUI*>(m_pImportLayout->FindSubControl(_T("ImportProgress")));

	if ( m_pImportProgress == NULL )
		return false;

	return true;
}

CLocalItemUI* CGroupExplorerUI::CreateLocalItem()
{
	CLocalItemUI * pItem = NULL;

	if( !m_cloudItemBuilder.GetMarkup()->IsValid() ) {
		pItem = dynamic_cast<CLocalItemUI*>(m_cloudItemBuilder.Create(_T("RightBar\\Item\\LocalItem.xml"), (UINT)0, this, &m_PaintManager));
	}
	else {
		pItem = dynamic_cast<CLocalItemUI*>(m_cloudItemBuilder.Create(this, &m_PaintManager));
	}
	if (pItem == NULL)
	{
		return NULL;
	}
	else
	{
		pItem->SetName(_T("LocalItem"));
	}
	return pItem;
}

void CGroupExplorerUI::OnEditClear(TNotifyUI& msg)
{
	CButtonUI * pClearButton = static_cast<CButtonUI*>(msg.pSender);
	if(pClearButton)
	{
		CEditClearUI * pClearEdit = (CEditClearUI * )(pClearButton->GetTag());
		if(pClearEdit)
		{
			pClearEdit->SetText(_T(""));
			pClearEdit->HideClearBtn();
			pClearEdit->SetFocus();
		}
	}
	
	
}

void CGroupExplorerUI::OnEditChanged(TNotifyUI& msg)
{
	CEditClearUI * pClearEdit = static_cast<CEditClearUI*>(msg.pSender);
//	CEditClearUI * pClearEdit = static_cast<CEditClearUI*>(m_PaintManager.FindControl(CLEAR_RICHEDIT_NAME));
	pClearEdit->SetClearBtn();
}

void CGroupExplorerUI::OnSearchComplete( THttpNotify* pHttpNotufy )
{
	//local search
	bool bSuccess = true;
	if (pHttpNotufy)
	{
		bSuccess = pHttpNotufy->dwErrorCode > 0 ? false : true;
	}

	CVerticalLayoutUI * pVerticalLayout = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("searchhistory")));
	if(pVerticalLayout)
	{
		pVerticalLayout->SetVisible(false);
	}

	if (!bSuccess)
	{
		m_pSearchFailwindow->SetVisible(false);
		OnSeachFailHandle();
		m_pSearchMainwindow->SetVisible(false);
		this->StopMask();
		m_pSearchNetless->SetVisible(true);
		return;
	}


	TCHAR szCount[128];
	_stprintf_s(szCount, _T("搜索 '{c #1E9FA5}%s{/c}' ，找到{c #FFF700}%d{/c}个资源"), m_strKeyWord.c_str(), m_nSearchCount);
	m_pLabelCloudCount->SetText(szCount);
	if(m_bSearch == false)//没有搜索到
	{
		m_pSearchFailwindow->SetVisible(true);
		OnSeachFailHandle();
		m_pSearchMainwindow->SetVisible(false);
	}
	else//搜到
	{
		m_pSearchFailwindow->SetVisible(false);
		m_pSearchMainwindow->SetVisible(true);
		if(m_nCurrentType == CloudFileSearch || m_nCurrentType == DBankSearch)
		{
			m_pTabLayoutCloudPage->ShowSearchTab();
		}
	}
	this->StopMask();
}

void CGroupExplorerUI::OnSearch(TNotifyUI& msg)
{
	
}

bool CGroupExplorerUI::OnGetCloudCourse(void * pParam)
{
	THttpNotify * pNotify = static_cast<THttpNotify *>(pParam);

	CStream stream(1024);
	BOOL bRet = NDCloudDecodeCourseList(pNotify->pData, pNotify->nDataSize ,&stream);

	if(!bRet)
	{
		OnSearchComplete(pNotify);
		return false;
	}

	DWORD dwCount = 0;

	m_pTabLayoutCloudPage->SetContent(TabLayout_Type_Course, stream, dwCount, true, m_nCurrentType == DBankSearch);
	if(dwCount > 0)
		m_bSearch = true;

	m_nSearchCount += dwCount;

	tstring strUrlEncodeKeyWord = UrlEncodeEx(m_strKeyWord);
	tstring strUrl;
	if (m_nCurrentType == CloudFileSearch)
		strUrl = NDCloudComposeUrlVideoInfo(_T(""),strUrlEncodeKeyWord,0,Search_Page_Count);
	if (m_nCurrentType == DBankSearch)
	{
		DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();
		strUrl = NDCloudComposeUrlVideoInfo(_T(""),strUrlEncodeKeyWord,0,Search_Page_Count, dwUserId);
	}

	m_dwCloudSearchDownloadId = NDCloudDownload(strUrl,MakeHttpDelegate(this,&CGroupExplorerUI::OnGetCloudVideo));

	return true;	
}


bool CGroupExplorerUI::OnGetCloudVideo(void * pParam)
{
	THttpNotify * pNotify = static_cast<THttpNotify *>(pParam);

	CStream stream(1024);
	BOOL bRet = NDCloudDecodeVideoList(pNotify->pData, pNotify->nDataSize ,&stream);

	if(!bRet)
	{

		OnSearchComplete(pNotify);
		return false;
	}

	DWORD dwCount = 0;

	m_pTabLayoutCloudPage->SetContent(TabLayout_Type_Video, stream, dwCount, true, m_nCurrentType == DBankSearch);
	if(dwCount > 0)
		m_bSearch = true;

	m_nSearchCount += dwCount;

	tstring strUrlEncodeKeyWord = UrlEncodeEx(m_strKeyWord);
	tstring strUrl;
	if (m_nCurrentType == CloudFileSearch)
		strUrl = NDCloudComposeUrlPictureInfo(_T(""),strUrlEncodeKeyWord,0,Search_Page_Count);
	if (m_nCurrentType == DBankSearch)
	{
		DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();
		strUrl = NDCloudComposeUrlPictureInfo(_T(""),strUrlEncodeKeyWord,0,Search_Page_Count, dwUserId);
	}

	m_dwCloudSearchDownloadId = NDCloudDownload(strUrl,MakeHttpDelegate(this,&CGroupExplorerUI::OnGetCloudPic));

	return true;
}


bool CGroupExplorerUI::OnGetCloudPic(void * pParam)
{
	THttpNotify * pNotify = static_cast<THttpNotify *>(pParam);

	CStream stream(1024);
	BOOL bRet = NDCloudDecodeImageList(pNotify->pData, pNotify->nDataSize ,&stream);

	if(!bRet)
	{

		OnSearchComplete(pNotify);
		return false;
	}

	DWORD dwCount = 0;

	m_pTabLayoutCloudPage->SetContent(TabLayout_Type_Pic, stream, dwCount, true, m_nCurrentType == DBankSearch);
	if(dwCount > 0)
		m_bSearch = true;

	m_nSearchCount += dwCount;
	tstring strUrlEncodeKeyWord = UrlEncodeEx(m_strKeyWord);
	tstring strUrl;
	if (m_nCurrentType == CloudFileSearch)
		strUrl = NDCloudComposeUrlVolumeInfo(_T(""),strUrlEncodeKeyWord,0,Search_Page_Count);
	if (m_nCurrentType == DBankSearch)
	{
		DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();
		strUrl = NDCloudComposeUrlVolumeInfo(_T(""),strUrlEncodeKeyWord,0,Search_Page_Count, dwUserId);
	}

	m_dwCloudSearchDownloadId = NDCloudDownload(strUrl,MakeHttpDelegate(this,&CGroupExplorerUI::OnGetCloudVolume));

	return true;	
}

bool CGroupExplorerUI::OnGetCloudVolume(void * pParam)
{
	THttpNotify * pNotify = static_cast<THttpNotify *>(pParam);

	CStream stream(1024);
	BOOL bRet = NDCloudDecodeVolumeList(pNotify->pData, pNotify->nDataSize ,&stream);

	if(!bRet)
	{
		OnSearchComplete(pNotify);
		return false;
	}


	DWORD dwCount = 0;

	m_pTabLayoutCloudPage->SetContent(TabLayout_Type_Volume, stream, dwCount, true, m_nCurrentType == DBankSearch);
	if(dwCount > 0)
		m_bSearch = true;

	m_nSearchCount += dwCount;
	tstring strUrlEncodeKeyWord = UrlEncodeEx(m_strKeyWord);
	tstring strUrl;
	if (m_nCurrentType == CloudFileSearch)
		strUrl = NDCloudComposeUrlFlashInfo(_T(""),strUrlEncodeKeyWord,0,Search_Page_Count);
	if (m_nCurrentType == DBankSearch)
	{
		DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();
		strUrl = NDCloudComposeUrlFlashInfo(_T(""),strUrlEncodeKeyWord,0,Search_Page_Count, dwUserId);
	}

	m_dwCloudSearchDownloadId = NDCloudDownload(strUrl,MakeHttpDelegate(this,&CGroupExplorerUI::OnGetCloudFlash));
	
	return true;
}


bool CGroupExplorerUI::OnGetCloudFlash( void * pParam )
{
	THttpNotify * pNotify = static_cast<THttpNotify *>(pParam);

	CStream stream(1024);
	BOOL bRet = NDCloudDecodeFlashList(pNotify->pData, pNotify->nDataSize ,&stream);

	if(!bRet)
	{
		OnSearchComplete(pNotify);
		return false;
	}

	DWORD dwCount = 0;

	m_pTabLayoutCloudPage->SetContent(TabLayout_Type_Flash, stream, dwCount, true, m_nCurrentType == DBankSearch);
	if(dwCount > 0)
		m_bSearch = true;

	m_nSearchCount += dwCount;


	OnSearchComplete(NULL);
	return true;
}

bool CGroupExplorerUI::OnCreateCallback( void * pParam )
{
	TEventNotify * pEventNotify = static_cast<TEventNotify *>(pParam);
	PITEM_PAGE pItemPage = (PITEM_PAGE)(pEventNotify->lParam);

	m_nLocalItemType = pItemPage->nItemType;
	CLocalItemUI * pLocalItem = static_cast<CLocalItemUI *>(CreateLocalItem());
	pLocalItem->SetResource(pItemPage->strPath.c_str());
	pLocalItem->SetTitle(pItemPage->strName.c_str());
	pLocalItem->SetGroup(pItemPage->strGroup.c_str());
	pLocalItem->SetChapter(pItemPage->strChapter.c_str());

	pEventNotify->wParam = (WPARAM)pLocalItem;
	return true;
}


bool CGroupExplorerUI::OnSearchWindowDestroy(void* pNotify)
{
	NDCloudDownloadCancel(m_dwCloudSearchDownloadId);
	this->StopMask();
	return true;
}

void CGroupExplorerUI::ShowSearchHistory()
{
	CVerticalLayoutUI * pVerticalLayout = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("searchhistory")));
	pVerticalLayout->SetVisible(false);

	CListUI * pList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("searchList")));

	Json::Value value;

	CDialogBuilder		m_builder;

	CLabelUI * pLabel = NULL;

	int nCount = m_jsonCloudKWRoot.size();
	if(nCount > 0)
	{
		for(int i = 0; i< nCount ; i++)
		{
			value = m_jsonCloudKWRoot[i];
			tstring strKeyWord = value["keyword"].asString();
			tstring strdate = value["date"].asString();

			CContainerUI * pItem = NULL;

			if( !m_builder.GetMarkup()->IsValid() ) {
				pItem = dynamic_cast<CContainerUI*>(m_builder.Create(_T("RightBar\\SearchKeyWord.xml"), (UINT)0, this, &m_PaintManager));
			}
			else {
				pItem = dynamic_cast<CContainerUI*>(m_builder.Create(this, &m_PaintManager));
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

		pVerticalLayout->SetVisible(true);
	}

}

void CGroupExplorerUI::InsertKeyWordJson(tstring & strKeyWord)
{
	DWORD dwCount = m_jsonCloudKWRoot.size();
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
		valueKW = m_jsonCloudKWRoot[i];
		if( strKeyWord == valueKW["keyword"].asString())
		{
			nExsit = i;

			break;;
		}
	}
	//

	if(dwCount == 5)
	{	
		if(nExsit == -1)
		{
			m_jsonCloudKWRoot[4] = Json::Value::null;
			dwCount--;
		}
		else
		{
			m_jsonCloudKWRoot[nExsit] = Json::Value::null;
		}
	}
	else
	{
		m_jsonCloudKWRoot[nExsit] = Json::Value::null;
	}

	for(int i = 0; i < (int)dwCount ; i++)
	{
		if(nExsit == i)
		{
			continue;
		}
		valueKW = m_jsonCloudKWRoot[i];

		valueNew.append(valueKW);
	}

	tstring str = m_jsonKeyWordWriter.write(valueNew);

	m_jsonCloudKWRoot = valueNew;

	FILE* fp = fopen(m_strCloudKWPath.c_str(), "wb+");
	if( fp == NULL )
	{
		return ;
	}

	fwrite(str.c_str(), str.length(), 1, fp);
	fclose(fp);

}

void CGroupExplorerUI::ResetWindow()
{
	m_nCurrentType = -1;
	m_pItemExplorer->ShowWindow(false);
}

void CGroupExplorerUI::OnSearchItemSelected(TNotifyUI& msg)
{
	CListContainerElementUI* pListElement = static_cast<CListContainerElementUI*>(msg.pSender);

	CLabelUI * pLabel = NULL;
	pLabel = static_cast<CLabelUI*>(pListElement->FindSubControl(_T("keyword")));
	tstring strKey = pLabel->GetText();

	CEditClearUI * pClearEdit = static_cast<CEditClearUI*>(m_PaintManager.FindControl(CLEAR_RICHEDIT_NAME));
	pClearEdit->SetText(strKey.c_str());
	pClearEdit->SetTextColor(0xFF4F4F4F);
	pClearEdit->SetClearBtn();

	TNotifyUI notify;
	OnSearch(notify);
}


// LRESULT CGroupExplorerUI::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
// {
// 	if( uMsg == WM_KEYDOWN && wParam == VK_RETURN)
// 	{
// 		TNotifyUI notify;
// 		OnSearch(notify);
// 	}
// 	return TRUE;
// }

bool CGroupExplorerUI::IsCloudExplorer()
{	
	return m_nCurrentType != LocalFileImport;	
}

void CGroupExplorerUI::ImportLocalFiles(const vector<tstring>& vecFiles)
{
	m_nCurrentTypeBak	= m_nCurrentType;
	tstring strGuid		= m_strChapterGuid;
	tstring strChapter	= m_strChapter;

	CFileTypeFilter fileTypeFilter;
	m_lstFileName.clear();

	for ( int i=0; i<vecFiles.size(); i++)
	{
		if ( GetFileAttributes(vecFiles[i].c_str()) == INVALID_FILE_ATTRIBUTES )  //check file exist
			continue;

		m_nLocalItemType = fileTypeFilter.GetFileType(vecFiles[i].c_str());
		if ( (m_nCurrentTypeBak == LocalFileImport && m_nLocalItemType	== FILE_FILTER_OTHER)
			|| (m_nCurrentTypeBak == LocalFileCourse && m_nLocalItemType != FILE_FILTER_PPT)
			|| (m_nCurrentTypeBak == LocalFileImage && m_nLocalItemType != FILE_FILTER_PIC)
			|| (m_nCurrentTypeBak == LocalFileVideo && m_nLocalItemType != FILE_FILTER_VIDEO)
			|| (m_nCurrentTypeBak == LocalFileFlash && m_nLocalItemType != FILE_FILTER_FLASH)
			|| (m_nCurrentTypeBak == LocalFileVolume && m_nLocalItemType != FILE_FILTER_VOLUME)
			)
			continue;

		LOCAL_RES_INFO resInfo;
		resInfo.nType			= m_nLocalItemType - FILE_FILTER + LocalFileImport;
		resInfo.strResPath		= vecFiles[i].c_str();
		resInfo.strChapterGuid	= strGuid;
		resInfo.strChapter		= strChapter;

		m_lstFileName.push_back(resInfo);
	}


	if (m_lstFileName.size() > 0)
	{
		//先添加导入界面
		m_pList->RemoveAll();

		CContainerUI * pImprtWindow = CreateImportWindow();

		if( pImprtWindow == NULL )
			return;
		m_pList->Add(pImprtWindow);

		if (!GetImportWindowControl())
			return;
		m_pEmptyLayout->SetVisible(false);
		m_pImportLayout->SetVisible(true);

		int nTotalCount = m_lstFileName.size();

		m_pImportProgress->SetMaxValue(nTotalCount);
		m_pImportProgress->SetMinValue(0);
		m_pImportProgress->SetValue(0);

		TCHAR szCount[MAX_PATH] = {0};
		wsprintf(szCount, _T("{f 3}{c #F68204}%d{/c}/%d{/f}"), 0, nTotalCount);
		m_pCountLabel->SetText(szCount);
		m_bImporting		= true;
		m_bLayoutChanged	= false;
		CreateThread(NULL, 0, ImportThread, this, 0, NULL);
	}
}

//============================
// local import
//============================

void CGroupExplorerUI::OnLocalImport( TNotifyUI& msg )
{

	if (m_pDlgImport != NULL)  //防止打开二个对话框
		return;

	LPCTSTR lpFilter	= GetFileFilter(m_nCurrentType);

	vector<tstring> vecFiles;
	BOOL bRet = FileSelectDialog(_T("导入文件"), lpFilter, OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_ALLOWMULTISELECT|OFN_ENABLESIZING|OFN_HIDEREADONLY|OFN_EXPLORER, vecFiles, AfxGetMainWnd()->m_hWnd);
	
	if ( -1 == bRet )
		CToast::Toast(_STR_SELECT_FILE_BUFFER_SMALL, true);

	if ( FALSE == bRet || -1 == bRet )
		return;

	ImportLocalFiles(vecFiles);
}

DWORD WINAPI CGroupExplorerUI::ImportThread(LPVOID lpVoid)
{
	CGroupExplorerUI* pThis = (CGroupExplorerUI*)lpVoid;
	pThis->Importing(pThis->m_lstFileName);
	return 1;
}

void CGroupExplorerUI::OnLocalCancelImport(TNotifyUI& msg)
{
	m_bImporting = false;
}

void CGroupExplorerUI::Importing(list<LOCAL_RES_INFO> lstFileName)
{
	//开始导入
//	int nTotalCount = lstFileName.size();
	list<LOCAL_RES_INFO>::iterator iter = lstFileName.begin();

	m_strImportFolderName = GetTitleText();

	//Sleep(200);

	//m_lstSaveFile.clear();
	for (int i = 0; i< LocalFileTotal-LocalFileImport; i++)
		m_lstImportFile[i].clear();
	
	m_nCurCount = 0;

	bool bNeedClassify = ((m_nCurrentType == LocalFileImport) && m_strImportFolderName == LOCAL_FOLDER_NAME_IMPORT);

	for ( iter; iter != lstFileName.end() && m_bImporting; iter++ )
	{
		::PostMessage(GetHWND(), WM_LOCAL_SEARCH_REFRESH_WINDOW, 0, 0);
		Sleep(100);
		if ( bNeedClassify )
			m_lstImportFile[iter->nType - LocalFileImport].push_back(*iter);
		else
			m_lstImportFile[0].push_back(*iter);

		if ( m_bLayoutChanged )  //页面已切换
			break;
	}
	Sleep(200);
	::PostMessage(GetHWND(), WM_LOCAL_SEARCH_REFRESH_WINDOW_FINISH,0,0);

}

//=======================
// localSearch
//=======================

DWORD WINAPI CGroupExplorerUI::LocalSearchThread(LPVOID lpVoid)
{
	CGroupExplorerUI* pThis = (CGroupExplorerUI*)lpVoid;
	pThis->LocalSearching();
	return 1;
}

void CGroupExplorerUI::LocalSearching()
{
	m_mapLocalSearchRes.clear();

	map<tstring, list<LOCAL_RES_INFO>> mapLocalFile = ((CListBarDlg*)m_pListBar)->GetLocalAllRes();
	map<tstring, list<LOCAL_RES_INFO>>::iterator  iterMap;
	list<LOCAL_RES_INFO>::iterator	iterList;
	CFileTypeFilter fileTypeFilter;

	list<LOCAL_SEARCH_RESULT> lstLocalResult;

	m_nSearchCount = 0;
	for (iterMap = mapLocalFile.begin(); iterMap != mapLocalFile.end(); iterMap++)
	{
		list<LOCAL_RES_INFO> lstFile = iterMap->second;

		lstLocalResult.clear();
		for (iterList = lstFile.begin(); iterList != lstFile.end(); iterList++)
		{
			
			LOCAL_RES_INFO sResInfo = *iterList;
			tstring	strPath = sResInfo.strResPath;

			int nPos = strPath.rfind(_T("\\"));
			tstring	strTitle = strPath.substr(nPos+1, strPath.length()-nPos-1);

			transform(strTitle.begin(), strTitle.end(), strTitle.begin(), ::tolower);
			tstring strKeyWord = m_strKeyWord;
			transform(strKeyWord.begin(), strKeyWord.end(), strKeyWord.begin(), ::tolower);

			if (strTitle.find(strKeyWord) != -1)
			{
				LOCAL_SEARCH_RESULT sLocal;
				sLocal.strFolder	= iterMap->first;
				sLocal.strPath		= strPath;
				sLocal.strName		= strTitle;
				sLocal.strChapter	= sResInfo.strChapter;
				sLocal.nFileType	= fileTypeFilter.GetFileType(strTitle);

				lstLocalResult.push_back(sLocal);

				m_nSearchCount++;
			}

		}

		if(!lstLocalResult.empty())
		{
			lstLocalResult.sort();
			m_mapLocalSearchRes[iterMap->first] = lstLocalResult;
		}
	}

	BroadcastEvent(EVT_LOCAL_SEARCH_SWITCH_STATUS, 0 ,0 ,0);
}

bool CGroupExplorerUI::OnLocalSearchComplete(int nShowType)
{

	DWORD dwCount = 0;

	//list<LOCAL_SEARCH_RESULT>::iterator iter = m_mapLocalSearchRes.begin();

	CTabLayoutUI*	m_TabLayout = static_cast<CTabLayoutUI*>(m_pLocalSearchContainer->FindSubControl(_T("localSearchTab")));
	m_TabLayout->SelectItem(0);

	CVerticalLayoutUI * pLayout = static_cast<CVerticalLayoutUI*>(m_pLocalSearchContainer->FindSubControl(_T("localSearchFolder")));

	pLayout->RemoveAll();

	if(m_nSearchCount > 0)
		m_bSearch = true;

	CHorizontalLayoutUI * pHorizontalLayout = NULL;
	CVerticalLayoutUI * pVerticalLayout = NULL;

	map<tstring, list<LOCAL_SEARCH_RESULT>>::iterator  iterMap;
	list<LOCAL_SEARCH_RESULT>::iterator	iterList;

	CDialogBuilder	Builder;
	CVerticalLayoutUI * pItem = NULL;
	CLabelUI* pLabel = NULL;
	CButtonUI* pButton = NULL;
	TCHAR szBuff[128];

	for (iterMap = m_mapLocalSearchRes.begin(); iterMap != m_mapLocalSearchRes.end(); iterMap++)
	{
		list<LOCAL_SEARCH_RESULT> lstFile = iterMap->second;

		if(lstFile.empty())
			continue;

		if( !Builder.GetMarkup()->IsValid() ) {
			pItem = static_cast<CVerticalLayoutUI*>(Builder.Create(_T("RightBar\\LocalSearch.xml"), (UINT)0, NULL, &m_PaintManager));
		}
		else {
			pItem = static_cast<CVerticalLayoutUI*>(Builder.Create((UINT)0, &m_PaintManager));
		}

		CVerticalLayoutUI* pContent = static_cast<CVerticalLayoutUI*>(pItem->FindSubControl(_T("searchSimpleContent")));
		CButtonUI* pButton = static_cast<CButtonUI*>(pItem->FindSubControl(_T("showAllBtn")));
		pButton->SetUserData(iterMap->first.c_str());
		pButton->OnNotify += MakeDelegate(this, &CGroupExplorerUI::OnLocalShowFolder);
		int n = 0;
		int k = 0;
		int nType = -1;

		pLayout->Add(pItem);
		for (iterList = lstFile.begin(); iterList != lstFile.end(); iterList++)
		{
			LOCAL_SEARCH_RESULT* spLocal = &(*iterList);

			if(spLocal->nFileType != nType)
			{
				nType = spLocal->nFileType;
				
				if(k % 2 == 0)
				{
					n++;
					pItem->SetFixedHeight(36 + n*165);

					pVerticalLayout = new CVerticalLayoutUI;
					pHorizontalLayout = new CHorizontalLayoutUI;
					pHorizontalLayout->SetChildPadding(20);
					pVerticalLayout->SetPadding(CDuiRect(0,10,0,0));
					pVerticalLayout->Add(pHorizontalLayout);
					pVerticalLayout->SetFixedHeight(140);
					pContent->Add(pVerticalLayout);
				}
				m_nLocalItemType = spLocal->nFileType;
				CLocalItemUI * pLocalItem = static_cast<CLocalItemUI *>(CreateLocalItem());
				pLocalItem->SetResource(spLocal->strPath.c_str());
				pLocalItem->SetTitle(spLocal->strName.c_str());
				pLocalItem->SetChapter(spLocal->strChapter.c_str());
				pLocalItem->SetGroup(_T("Locals"));
				
				pHorizontalLayout->Add(pLocalItem);
				pLocalItem->SetFixedWidth(150);
				pLocalItem->SetContentHeight(105);

				k++;
			}
		}

		

		pLabel = dynamic_cast<CLabelUI*>(pItem->FindSubControl(_T("simpleTitle")));
		pLabel->SetText(iterMap->first.c_str());
		pLabel->SetToolTip(iterMap->first.c_str());

		pLabel = dynamic_cast<CLabelUI*>(pItem->FindSubControl(_T("simpleCountText")));
		_stprintf_s(szBuff, _T("{c #FFF700}%d{/c} 个资源") , lstFile.size() );
		pLabel->SetText(szBuff);

		TNotifyUI msg;
		OnBtnFolderSearchBack(msg);
	}

	OnSearchComplete(NULL);
	return true;
}

bool CGroupExplorerUI::OnLocalShowFolder(void* pNotify)
{
	TNotifyUI* pNotifyUI = (TNotifyUI*)pNotify;

	if (pNotifyUI->sType == _T("click"))
	{
		//m_pTabLayoutLocalPage->RemoveAll();
		//先清空

		//
		DWORD dwCount;
		int nType = 0;
		int nCurType = -1;

		CButtonUI * pButton = static_cast<CButtonUI *>(pNotifyUI->pSender);
		tstring strFloderName = pButton->GetUserData();


		map<tstring, list<LOCAL_SEARCH_RESULT>>::iterator  iterMap;
		list<LOCAL_SEARCH_RESULT>::iterator	iterList;

		map<int, list<LOCAL_SEARCH_RESULT>> mapLocalFloderRes;
		map<int, list<LOCAL_SEARCH_RESULT>>::iterator  iter;

		iterMap = m_mapLocalSearchRes.find(strFloderName);
		if(iterMap != m_mapLocalSearchRes.end())
		{
			list<LOCAL_SEARCH_RESULT> lstFile = iterMap->second;

			list<LOCAL_SEARCH_RESULT> lstFileTmp;
			for (iterList = lstFile.begin(); iterList != lstFile.end(); iterList++)
			{
				LOCAL_SEARCH_RESULT* spLocal = &(*iterList);
				nType = spLocal->nFileType;

				if(nCurType != -1 && nCurType != nType)
				{
					mapLocalFloderRes[nCurType] = lstFileTmp;
					lstFileTmp.clear();
				}

				lstFileTmp.push_back(*iterList);


				nCurType = nType;
			}
			if(!lstFileTmp.empty())
				mapLocalFloderRes[nCurType] = lstFileTmp;

			CStream stream(10240);

			for (int i= RES_TYPE_COURSE ; i <= RES_TYPE_VOLUME; i++)
			{
				stream.ResetCursor();
				stream.WriteDWORD(0);

				stream.ResetCursor();

				switch(i)
				{
				case RES_TYPE_COURSE:
					{
						m_pTabLayoutLocalPage->SetContent(TabLayout_Type_Course, stream, dwCount);
					}
					break;
				case RES_TYPE_VIDEO:
					{
						m_pTabLayoutLocalPage->SetContent(TabLayout_Type_Video, stream, dwCount);
					}
					break;
				case RES_TYPE_PIC:
					{
						m_pTabLayoutLocalPage->SetContent(TabLayout_Type_Pic, stream, dwCount);
					}
					break;
				case RES_TYPE_FLASH:
					{
						m_pTabLayoutLocalPage->SetContent(TabLayout_Type_Flash, stream, dwCount);
					}
					break;
				case RES_TYPE_VOLUME:
					{
						m_pTabLayoutLocalPage->SetContent(TabLayout_Type_Volume, stream, dwCount);
					}
					break;
				}

			}
			for (iter = mapLocalFloderRes.begin(); iter != mapLocalFloderRes.end(); iter++)
			{
				list<LOCAL_SEARCH_RESULT> lstFileTmp = iter->second;

				stream.ResetCursor();
				stream.WriteDWORD(lstFileTmp.size());
				for (iterList = lstFileTmp.begin(); iterList != lstFileTmp.end(); iterList++)
				{
					LOCAL_SEARCH_RESULT* spLocal = &(*iterList);

					stream.WriteString((char *)spLocal->strPath.c_str());
					stream.WriteString((char *)spLocal->strName.c_str());
					stream.WriteString((char *)spLocal->strChapter.c_str());
					
				}

				stream.ResetCursor();

				switch(iter->first)
				{
				case RES_TYPE_COURSE:
					{
						m_pTabLayoutLocalPage->SetContent(TabLayout_Type_Course, stream, dwCount);
					}
					break;
				case RES_TYPE_VIDEO:
					{
						m_pTabLayoutLocalPage->SetContent(TabLayout_Type_Video, stream, dwCount);
					}
					break;
				case RES_TYPE_PIC:
					{
						m_pTabLayoutLocalPage->SetContent(TabLayout_Type_Pic, stream, dwCount);
					}
					break;
				case RES_TYPE_FLASH:
					{
						m_pTabLayoutLocalPage->SetContent(TabLayout_Type_Flash, stream, dwCount);
					}
					break;
				case RES_TYPE_VOLUME:
					{
						m_pTabLayoutLocalPage->SetContent(TabLayout_Type_Volume, stream, dwCount);
					}
					break;
				}
			}
		}

		CTabLayoutUI*	pTabLayout = static_cast<CTabLayoutUI*>(m_pLocalSearchContainer->FindSubControl(_T("localSearchTab")));
		pTabLayout->SelectItem(1);

		m_pLabelCloudCount->SetVisible(false);
		/*
		CHorizontalLayoutUI*	pLayout = static_cast<CHorizontalLayoutUI*>(m_pSearchWindow->FindSubControl(_T("folderSearchInfo")));

		CLabelUI* pLabel = static_cast<CLabelUI*>(pLayout->FindSubControl(_T("folderSearchCount")));

		TCHAR szCount[256];
		_stprintf_s(szCount, _T("在 {c #FFF700}%s{/c} 搜索 '{c #1E9FA5}%s{/c}' ，找到{c #FFF700}%d{/c}个资源"),iterMap->first.c_str(),  m_strKeyWord.c_str(), iterMap->second.size());
		pLabel->SetText(szCount);
		pLayout->SetVisible(true);
		*/
	}
	return true;
}

void CGroupExplorerUI::OnSeachFailHandle()
{
	CMainFrame* pMainFrame = (CMainFrame* )AfxGetApp()->m_pMainWnd;
	if(pMainFrame == NULL)
		return;
	Json::Value keywordJson =  pMainFrame->m_update.GetServerKeyWord();

	Json::Value value = keywordJson["keywords"];
	if(value.size() == 0)
		return;

	m_pSearchFailContainer->SetVisible(false);


	Json::Value valueKey;

	int nLevel = value.size() / 4;
	if(value.size() % 4 != 0)
		nLevel++;
	nLevel = nLevel > 4 ? 4 : nLevel;
	tstring strKeyWord1,strKeyWord2,strKeyWord3,strKeyWord4;
	for (int i = 0; i < nLevel; i++)
	{
		strKeyWord1 = _T("");
		strKeyWord2 = _T("");
		strKeyWord3 = _T("");
		strKeyWord4 = _T("");

		do 
		{
			valueKey = value[i*4 + 0];
			strKeyWord1 = valueKey[_T("keyword")].asString();

			if(i * 4 + 1 >= value.size())
			{
				break;
			}

			valueKey = value[i*4 + 1];
			strKeyWord2 = valueKey[_T("keyword")].asString();

			if(i * 4 + 2 >= value.size())
			{
				break;
			}

			valueKey = value[i*4 + 2];
			strKeyWord3 = valueKey[_T("keyword")].asString();

			if(i * 4 + 3 >= value.size())
			{
				break;
			}

			valueKey = value[i*4 + 3];
			strKeyWord4 = valueKey[_T("keyword")].asString();

			if(i * 4 + 4 >= value.size())
			{
				break;
			}
		} while (false);
		
		CreatSeachFailStyle(m_pSearchFailContainer, i,  strKeyWord1.c_str(), strKeyWord2.c_str(), strKeyWord3.c_str(), strKeyWord4.c_str());

	}
	m_pSearchFailContainer->SetVisible(true);
	m_pSearchFailContainer->Invalidate();

}

int CGroupExplorerUI::GenernateRandom(CContainerUI* pParent, CLabelUI* pLabel, int nPos)
{
	tstring strColor;
	tstring strPos;
	tstring strAlign;
	tstring strVAlign;
	int nFontId;

	int n =  (rand()%(6-0))+0;
	switch (n)
	{
	case 1:
		{
			strColor = _T("#FFc6c4c4");	//WHITE
			nFontId = 130000;
			strAlign = _T("center");
		}
		break;
	case 2:
		{
			strColor = _T("#FFA04B2E");//PURPLE
			nFontId = 190000;
			strAlign = _T("left");
		}
		break;
	case 3:
		{
			strColor = _T("#FF3C8993");//ORANGE
			nFontId = 210000;
			strAlign = _T("right");
		}
		break;
 	case 4:
 		{
 			strColor = _T("#FF2AA4D7");//BLUE
 			nFontId = 160000;
			strVAlign = _T("bottom");
 		}
 		break;
	case 5:
		{
			strColor = _T("#FFC148A9");	//PURPLE
			nFontId = 140000;
			strVAlign = _T("top");
		}
		break;
	default:
		{
			strColor = _T("#FF6262624");	//灰色
			nFontId = 190000;
			strVAlign = _T("vcenter");
		}
		break;
	}

	
	pLabel->SetFont(nFontId);
	pLabel->SetAttribute(_T("textcolor"), strColor.c_str());
	pLabel->SetAttribute(_T("mouse"), _T("true"));
	pLabel->SetEnabled(true);
	pLabel->SetMouseEnabled(true);
	RECT rcCalc = {0};
	CalcText(m_PaintManager.GetPaintDC(), rcCalc, pLabel->GetText(), pLabel->GetFont(), DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT|DT_NOPREFIX, UIFONT_GDI);

	if (nPos == 1)
	{
		if (_tcscmp(strAlign.c_str(), _T("right")) == 0)
		{
			strAlign = _T("center");
		}

	}

	if (nPos == 4)
	{
		pLabel->SetFont(140000);
	
		strAlign = _T("left");
		

	}

	if (!strAlign.empty())
	{	
		pLabel->SetAttribute(_T("align"), strAlign.c_str());
	}

	if (!strVAlign.empty())
	{
		pLabel->SetAttribute(_T("valign"), strVAlign.c_str());
	}

	pParent->Add(pLabel);
	return n;

}

void  CGroupExplorerUI::CalcText( HDC hdc, RECT& rc, LPCTSTR lpszText, int nFontId, UINT format, UITYPE_FONT nFontType, int c)
{
	if (nFontType == UIFONT_GDI)
	{
		HFONT hFont = m_PaintManager.GetFont(nFontId);
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

void CGroupExplorerUI::CreatSeachFailStyle(CContainerUI* pParent, int nSeq, LPCTSTR text1, LPCTSTR text2, LPCTSTR text3, LPCTSTR text4)
{

	VERIFY_PTR_NOT_RET(pParent);
	pParent->SetVisible(false);

	CDialogBuilder		LogYearbuilder;


	CContainerUI * pYearItem = NULL;
	if( !LogYearbuilder.GetMarkup()->IsValid() ) 
	{
		pYearItem = dynamic_cast<CContainerUI*>(LogYearbuilder.Create(_T("RightBar\\SearchFailCommand.xml"), (UINT)0, this, &m_PaintManager));
	}
	else 
	{
		pYearItem = dynamic_cast<CContainerUI*>(LogYearbuilder.Create(this, &m_PaintManager));
	}

	VERIFY_PTR_NOT_RET(pYearItem);
	pYearItem->SetVisible(false);
	
	int n = 0;
	if (text1 != NULL)
	{
		CButtonUI* pLabel1	= NULL;
		pLabel1 = new CButtonUI;
		//pLabel1->SetEnabledCursor(true);
		pLabel1->SetText(text1);
		n = GenernateRandom(pYearItem, pLabel1, 1);	
		pLabel1->OnNotify += MakeDelegate(this, &CGroupExplorerUI::OnBtnCommandKey);
	}
	if (text2 != NULL)
	{
		CButtonUI* pLabel2	= NULL;
		pLabel2 = new CButtonUI;
	//	pLabel2->SetEnabledCursor(true);
		pLabel2->SetText(text2);
		n = GenernateRandom(pYearItem, pLabel2, 2);
		pLabel2->OnNotify += MakeDelegate(this, &CGroupExplorerUI::OnBtnCommandKey);
		
		
	}

	if (text3 != NULL)
	{
		CButtonUI* pLabel3	= NULL;
		pLabel3 = new CButtonUI;
	//	pLabel3->SetEnabledCursor(true);
		pLabel3->SetText(text3);
		n = GenernateRandom(pYearItem, pLabel3, 3);
		pLabel3->OnNotify += MakeDelegate(this, &CGroupExplorerUI::OnBtnCommandKey);
		
	}
// 
	if (text4 != NULL)
	{
		CButtonUI* pLabel4	= NULL;
		pLabel4 = new CButtonUI;
		//pLabel4->SetEnabledCursor(true);
		pLabel4->SetText(text4);
		n = GenernateRandom(pYearItem, pLabel4, 4);
		pLabel4->OnNotify += MakeDelegate(this, &CGroupExplorerUI::OnBtnCommandKey);
	}

	pYearItem->SetVisible(true);
	pYearItem->Invalidate();
	
	pParent->Add(pYearItem);
	
	pParent->SetVisible(true);
	pParent->Invalidate();
	pParent->NeedUpdate();

	PosChangeAnimation* pAnimation = NULL;
	if (nSeq == 0)
	{
		pYearItem->SetPadding(CDuiRect(-350,0,0,0));
		pYearItem->SetFixedWidth(350);
		pAnimation = new PosChangeAnimation;
		RECT rtStart = CDuiRect(-350,-50,0,0);
		tstring strControlName = _T("item0");
		pYearItem->SetName(strControlName.c_str());
		pAnimation->SetAnimationName(strControlName.c_str());
		pAnimation->SetKeyFrameCoordinate(rtStart, CDuiRect(0,0,0,0));
		pAnimation->SetParam(40, 30); 
		ActionManager::GetInstance()->addViewAnimation(pAnimation, pYearItem);
	}
	if (nSeq == 1)
	{
		pYearItem->SetPadding(CDuiRect(-350,0,0,0));
		pYearItem->SetFixedWidth(350);
		pAnimation = new PosChangeAnimation;
		RECT rtStart = CDuiRect(350,0,0,0);	
		tstring strControlName = _T("item1");
		pYearItem->SetName(strControlName.c_str());
		pAnimation->SetAnimationName(strControlName.c_str());
		pAnimation->SetKeyFrameCoordinate(rtStart, CDuiRect(0,0,0,0));
		pAnimation->SetParam(40, 30); 
		ActionManager::GetInstance()->addViewAnimation(pAnimation, pYearItem);
	}
	if (nSeq == 2)
	{
		pYearItem->SetPadding(CDuiRect(-350,0,0,0));
		pYearItem->SetFixedWidth(350);
		pAnimation = new PosChangeAnimation;
		RECT rtStart = CDuiRect(-350,0,0,0);
		tstring strControlName = _T("item2");
		pYearItem->SetName(strControlName.c_str());
		pAnimation->SetAnimationName(strControlName.c_str());
		pAnimation->SetKeyFrameCoordinate(rtStart, CDuiRect(0,0,0,0));
		pAnimation->SetParam(40, 30); 
		ActionManager::GetInstance()->addViewAnimation(pAnimation, pYearItem);
	}
	if (nSeq == 3)
	{
		pYearItem->SetPadding(CDuiRect(-350,0,0,0));
		pYearItem->SetFixedWidth(350);
		pAnimation = new PosChangeAnimation;
		RECT rtStart = CDuiRect(350,0,0,0);	
		tstring strControlName = _T("item3");
		pYearItem->SetName(strControlName.c_str());
		pAnimation->SetAnimationName(strControlName.c_str());
		pAnimation->SetKeyFrameCoordinate(rtStart, CDuiRect(0,0,0,0));
		pAnimation->SetParam(40, 30); 
		ActionManager::GetInstance()->addViewAnimation(pAnimation, pYearItem);
	}



}


bool CGroupExplorerUI::OnBtnCommandKey(void* pNotify)
{
	TNotifyUI* pNotifyUI = (TNotifyUI*)pNotify;
	if (pNotifyUI->sType == _T("click"))
	{
		CButtonUI*  pBtn = static_cast<CButtonUI*>(pNotifyUI->pSender);
		if (pBtn)
		{
			tstring strText = pBtn->GetText();

			if(strText == _T(""))
				return true;
			CEditClearUI * pClearEdit = static_cast<CEditClearUI*>(m_PaintManager.FindControl(CLEAR_RICHEDIT_NAME));
			pClearEdit->SetText(strText.c_str());
			pClearEdit->SetTextColor(0xFF4F4F4F);
			pClearEdit->SetClearBtn();

			TNotifyUI notify;
			OnSearch(notify);
		}
	}
		

	return true;
}

void CGroupExplorerUI::OnBtnFolderSearchBack(TNotifyUI& msg)
{
	CTabLayoutUI*	pTabLayout = static_cast<CTabLayoutUI*>(m_pLocalSearchContainer->FindSubControl(_T("localSearchTab")));
	pTabLayout->SelectItem(0);

	m_pLabelCloudCount->SetVisible(true);

	/*
	CHorizontalLayoutUI*	pLayout = static_cast<CHorizontalLayoutUI*>(m_pSearchWindow->FindSubControl(_T("folderSearchInfo")));
	pLayout->SetVisible(false);
	*/
}

void CGroupExplorerUI::ShowNetlessUI( bool bShow )
{
	__super::ShowNetlessUI(bShow);
	if (bShow)
	{
		SetDlgAcceptFiles(false);
		m_nCurrentType = CloudFileSearch;
		::PostMessage(m_pItemExplorer->GetHWND(),WM_USER_HIDE,0,0);
	}
	
}

void CGroupExplorerUI::ShowReslessUI( bool bShow )
{
	if (!m_layResless)
	{
		return;
	}

	m_layResless->SetVisible(bShow);

	if (bShow == false)
	{
		return;
	}

	tstring strReslessIcon;
	tstring	strReslessTxt	= _STR_GROUPEXPLORER_NONE;

	int nType = m_nCurrentType;
	if(nType > DBankUpload)
		nType -= DBankUpload;
	switch (nType)
	{
		case CloudFileCourse:
			{
				strReslessTxt	+=	_STR_RIGHTBAR_ITEM_COURSE;	
				strReslessIcon	=	_T("RightBar\\Explorer\\resless_icon_course.png");

			}
			break;
		case CloudFileVideo:
			{
				strReslessTxt	+= _STR_RIGHTBAR_ITEM_VEDIO;	
				strReslessIcon	=	_T("RightBar\\Explorer\\resless_icon_video.png");
			}
			break;
		case CloudFileImage:
			{
				strReslessTxt	+= _STR_RIGHTBAR_ITEM_IMAGE;	
				strReslessIcon	=	_T("RightBar\\Explorer\\resless_icon_photo.png");
			}
			break;
		case CloudFileFlash:
			{
				strReslessTxt	+= _STR_RIGHTBAR_ITEM_FLASH;	
				strReslessIcon	=	_T("RightBar\\Explorer\\resless_icon_flash.png");
			}
			break;
		case CloudFileVolume:
			{
				strReslessTxt	+= _STR_RIGHTBAR_ITEM_VOLUME;
				strReslessIcon	=	_T("RightBar\\Explorer\\resless_icon_volume.png");
			}
			break;
		case CloudFileQuestion:
			{
				strReslessTxt	+= _STR_RIGHTBAR_ITEM_QUESTION;
				strReslessIcon	=	_T("RightBar\\Explorer\\resless_icon_question.png");
			}
			break;
		case CloudFileCoursewareObjects:
			{
				strReslessTxt	+= _STR_RIGHTBAR_ITEM_INTERACTION;
				strReslessIcon	=	_T("RightBar\\Explorer\\resless_icon_interaction.png");
			}
			break;
		case CloudFilePPTTemplate:
			{
				strReslessTxt	+= _STR_RIGHTBAR_ITEM_PPTMODEL;
				strReslessIcon	=	_T("RightBar\\Explorer\\resless_icon_ppt_model.png");
			}
			break;
	}

	strReslessTxt	+=_STR_RESOURCE;

	CControlUI* pCtrl = m_layResless->FindSubControl(_T("resless_icon"));
	if (pCtrl)
	{
		pCtrl->SetBkImage(strReslessIcon.c_str());
	}

	pCtrl = m_layResless->FindSubControl(_T("resless_txt"));
	if (pCtrl)
	{
		pCtrl->SetText(strReslessTxt.c_str());
	}

}

bool CGroupExplorerUI::OnLocalSearchSwitchStatus( void* pObj )
{
	OnLocalSearchComplete(1);
	return true;
}

LRESULT CGroupExplorerUI::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch(uMsg)
	{
	case WM_LOCAL_SEARCH_REFRESH_WINDOW:
		{
			TCHAR szCount[MAX_PATH] = {0};
			if(m_pImportProgress)
			{
				m_pImportProgress->SetValue(++m_nCurCount);
			}
			wsprintf(szCount, _T("{f 3}{c #F68204}%d{/c}/%d{/f}"), m_nCurCount, m_lstFileName.size());
			if(m_pCountLabel)
			{
				m_pCountLabel->SetText(szCount);
			}
			//SetCountText(m_nCurCount);
			if ( m_nCurCount == m_lstFileName.size()&&m_pTipLabel )
				m_pTipLabel->SetText(_T("到达目的地"));
		}
		break;
	case WM_LOCAL_SEARCH_REFRESH_WINDOW_FINISH:
		{
			tstring strFolderName =  m_strImportFolderName;//GetTitleText();

			bool bNeedClassify = ((m_nCurrentType == LocalFileImport) && (m_strImportFolderName == LOCAL_FOLDER_NAME_IMPORT));

			bool bImport = false;

			if ( m_pListBar != NULL )
			{
				if ( !bNeedClassify )
				{
					bImport = ((CListBarDlg*)m_pListBar)->LocalResItemAddRes(strFolderName, m_lstImportFile[0]);
				}
				else
				{
					for ( int i=1; i<LocalFileTotal-LocalFileImport; i++)
					{
						((CListBarDlg*)m_pListBar)->LocalResItemAddRes(i, m_lstImportFile[i]);	
					}

					ShowWindow(false);
				}

				((CListBarDlg*)m_pListBar)->SaveFoldersInfo();
			}

			if ( bImport && !m_bLayoutChanged && m_pImportLayout )
			{
				m_pImportLayout->SetVisible(false);

				//重新显示界面
				list<LOCAL_RES_INFO>* pFileList = ((CListBarDlg*)m_pListBar)->GetLocalResFileList(strFolderName);
				if ( pFileList != NULL )
				{
					CStream stream(1024);
					stream.WriteDWORD(pFileList->size());
					stream.WriteString((TCHAR*)strFolderName.c_str());
					m_bImporting = false;
					ShowResource(m_nCurrentType, &stream);
				}
				
			}

			m_bImporting = false;
		}
		break;

	case WM_DROPFILES:
		{
			if (LocalFileImport<=m_nCurrentType && m_nCurrentType<=LocalFileAnother)
			{
				HDROP hDropInfo = (HDROP)wParam;
				UINT nCount = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
				TCHAR szPath[MAX_PATH];
				vector<tstring> vecFiles;
				if (nCount)
				{
					for (UINT nIndex = 0; nIndex < nCount; ++nIndex)
					{
						DragQueryFile(hDropInfo, nIndex, szPath, _countof(szPath));
						vecFiles.push_back(szPath);
					}
				}

				ImportLocalFiles(vecFiles);
				DragFinish(hDropInfo);
			}

			if (m_pList)
			{
				CDBankUploadUI* pUpload = dynamic_cast<CDBankUploadUI*>(m_pList->FindSubControl(_T("DBankUpload")));
				if (pUpload)
				{
					pUpload->OnDragFiles(wParam, lParam);
				}
			}
		}
		break;
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void CGroupExplorerUI::ShowWindow( bool bShow /*= true*/, bool bTakeFocus /*= true */ )
{
	WindowImplBase::ShowWindow(bShow, bTakeFocus);
	if (!bShow)
	{
		CItemExplorerUI::GetInstance()->ShowWindow(false);
	}
	CPanelInnerDlg::AdjustMainPanel();
}

void CGroupExplorerUI::OnBtnClose(TNotifyUI& msg)
{
	if (CPPTController::GetPPTVersion() == PPTVER_2007)		// fix bug:9846 【交互】插入工作表后，关闭BC框，界面异常
	{
		GetPPTController()->ActivePPTWnd();
	}

	if(m_OnClickCallBack)
		m_OnClickCallBack(&msg);
}

void CGroupExplorerUI::OnBtnRefresh( TNotifyUI& msg )
{
	this->ShowReslessUI(false);

	BroadcastEvent(EVT_REFRESH_GROUPEXPLORER, 0, 0 ,0);
}

void CGroupExplorerUI::ShowLoginUI(BOOL bAutoLogin/* = FALSE*/)
{
	SetDlgAcceptFiles(false);

	if(bAutoLogin)
	{


	}
	else
	{
		CItemExplorerUI::GetInstance()->ShowWindow(false);
		CPanelInnerDlg*		m_pAttchedDlg = dynamic_cast<CPanelInnerDlg*>(CWnd::FromHandle(this->GetHWND()));
		if (m_pAttchedDlg)
		{
			m_pAttchedDlg->SetInnerWidth(400);
		}

		SetTitleText(_T(""));
		SetCountText(_T(""));
		StopMask();
		ShowNetlessUI(false);
		ShowReslessUI(false);

		m_nCurrentType = DBankLogin;

		m_pSliderTabLayout->SelectItem(0, false);


	}
	if(m_pLoginWindowUI && !m_pLoginWindowUI->GetLoginStatus())
	{
		m_pLoginWindowUI->ClearEditStatus();
	}

	CListBarDlg* m_pListBarDlg = CListBarDlg::GetInstance(NULL);
	m_pListBarDlg->ClearSelect();

	if(m_pLoginList->FindSubControl(_T("loginLayout")))
		return;
	

	
	m_pLoginWindowUI = CreateLoginWindow();
	if(m_pLoginWindowUI)
	{
		m_pLoginWindowUI->OnDestroy += MakeDelegate(this, &CGroupExplorerUI::OnLoginWindowDestroy);
		m_pLoginWindowUI->SetName(_T("loginLayout"));
		//m_pList->Add(m_pLoginWindowUI);
		m_pLoginList->Add(m_pLoginWindowUI);

		m_pLoginWindowUI->Init(GetHWND());
	}

	
}

void CGroupExplorerUI::ShowRegisterUI()
{
	m_pSliderTabLayout->SelectItem(1, false);

	CItemExplorerUI::GetInstance()->ShowWindow(false);
	CPanelInnerDlg*		m_pAttchedDlg = dynamic_cast<CPanelInnerDlg*>(CWnd::FromHandle(this->GetHWND()));
	if (m_pAttchedDlg)
	{
		m_pAttchedDlg->SetInnerWidth(400);
	}

	SetTitleText(_T(""));
	SetCountText(_T(""));
	ShowNetlessUI(false);
	ShowReslessUI(false);
	StopMask();

	CListBarDlg* m_pListBarDlg = CListBarDlg::GetInstance(NULL);
	m_pListBarDlg->ClearSelect();

	SetDlgAcceptFiles(false);
	m_nCurrentType = DBankRegister;

	if(m_pRegisterWindowUI && !m_pRegisterWindowUI->GetRegisterStatus())
		m_pRegisterWindowUI->ClearEditStatus();

 	if(m_PaintManager.FindControl(_T("registerLayout")))
 		return;


	m_pList->RemoveAll();
	

	m_pRegisterWindowUI = CreateRegisterWindow();
	if(m_pRegisterWindowUI)
	{
		m_pRegisterWindowUI->OnDestroy += MakeDelegate(this, &CGroupExplorerUI::OnRegisterWindowDestroy);
		m_pRegisterWindowUI->SetName(_T("registerLayout"));
		m_pList->Add(m_pRegisterWindowUI);
		m_pRegisterWindowUI->Init(GetHWND());

		
	}


	
}

void CGroupExplorerUI::ShowSearchUI(CDelegateBase &delegate)
{
	m_pSliderTabLayout->SelectItem(6, false);

	CItemExplorerUI::GetInstance()->ShowWindow(false);
	CPanelInnerDlg*		m_pAttchedDlg = dynamic_cast<CPanelInnerDlg*>(CWnd::FromHandle(this->GetHWND()));
	if (m_pAttchedDlg)
	{
		m_pAttchedDlg->SetInnerWidth(400);
	}

	SetTitleText(_T(""));
	SetCountText(_T(""));
	ShowNetlessUI(false);
	ShowReslessUI(false);
	StopMask();

	SetDlgAcceptFiles(false);
	m_nCurrentType = SearchALLRes;

	if(m_PaintManager.FindControl(_T("searchLayout")))
		return;

	//m_pList->RemoveAll();

	m_pSearchWindowUI = CreateSearchWindow();
	if(m_pSearchWindowUI)
	{
	//	m_pSearchWindowUI->OnDestroy += MakeDelegate(this, &CGroupExplorerUI::OnRegisterWindowDestroy);
		m_pSearchWindowUI->SetName(_T("searchLayout"));
		m_pSearchList->Add(m_pSearchWindowUI);
	//	m_pSearchWindowUI->Init(GetHWND());
		m_OnClickCallBack += delegate;

	}

}

void CGroupExplorerUI::ShowChangePasswordUI()
{
	m_pSliderTabLayout->SelectItem(2, false);

	CItemExplorerUI::GetInstance()->ShowWindow(false);
	CPanelInnerDlg*		m_pAttchedDlg = dynamic_cast<CPanelInnerDlg*>(CWnd::FromHandle(this->GetHWND()));
	if (m_pAttchedDlg)
	{
		m_pAttchedDlg->SetInnerWidth(400);
	}

	SetTitleText(_T(""));
	SetCountText(_T(""));
 	ShowNetlessUI(false);
 	ShowReslessUI(false);
	StopMask();
	CListBarDlg* m_pListBarDlg = CListBarDlg::GetInstance(NULL);
	m_pListBarDlg->ClearSelect();
	m_nCurrentType = -1;
	SetDlgAcceptFiles(false);

	if(m_pChangePasswordWindowUI && !m_pChangePasswordWindowUI->GetChangePasswordStatus())
		m_pChangePasswordWindowUI->ClearEditStatus();

	if(m_PaintManager.FindControl(_T("changePasswordLayout")))
	{
		return;
	}

	m_pChangePasswordList->RemoveAll();


	m_pChangePasswordWindowUI = CreateChangePasswordWindow();
	if(m_pChangePasswordWindowUI)
	{
//		m_pRegisterWindowUI->OnDestroy += MakeDelegate(this, &CGroupExplorerUI::OnRegisterWindowDestroy);
		m_pChangePasswordWindowUI->SetName(_T("changePasswordLayout"));
		m_pChangePasswordList->Add(m_pChangePasswordWindowUI);
		m_pChangePasswordWindowUI->Init(GetHWND());


	}
}

void CGroupExplorerUI::ShowGetBackPwdUI()
{
	m_pSliderTabLayout->SelectItem(3, false);

	CItemExplorerUI::GetInstance()->ShowWindow(false);
	CPanelInnerDlg*		m_pAttchedDlg = dynamic_cast<CPanelInnerDlg*>(CWnd::FromHandle(this->GetHWND()));
	if (m_pAttchedDlg)
	{
		m_pAttchedDlg->SetInnerWidth(400);
	}

	SetTitleText(_T(""));
	SetCountText(_T(""));
	StopMask();
	ShowNetlessUI(false);
	ShowReslessUI(false);
	CListBarDlg* m_pListBarDlg = CListBarDlg::GetInstance(NULL);
	m_pListBarDlg->ClearSelect();
	m_nCurrentType = -1;
	SetDlgAcceptFiles(false);

	if(m_pGetBackPwdWindowUI)
		m_pGetBackPwdWindowUI->ClearEditStatus();

	if(m_PaintManager.FindControl(_T("getBackPwdLayout")))
	{
		m_pGetBackPwdWindowUI->m_pSliderTabLayout->SelectItem(0, false);
		return;
	}

	m_pGetBackPwdList->RemoveAll();


	m_pGetBackPwdWindowUI = CreateGetBackPwdWindow();
	if(m_pGetBackPwdWindowUI)
	{
		m_pGetBackPwdWindowUI->SetName(_T("getBackPwdLayout"));
		m_pGetBackPwdList->Add(m_pGetBackPwdWindowUI);
		m_pGetBackPwdWindowUI->Init(GetHWND());


	}
}


void CGroupExplorerUI::ShowCloudAssetsUI()
{
	SetDlgAcceptFiles(false);
	m_pSliderTabLayout->SelectItem(4, false);

	CItemExplorerUI::GetInstance()->ShowWindow(false);
	CPanelInnerDlg*		m_pAttchedDlg = dynamic_cast<CPanelInnerDlg*>(CWnd::FromHandle(this->GetHWND()));
	if (m_pAttchedDlg)
	{
		m_pAttchedDlg->SetInnerWidth(400);
	}

	SetTitleText(_T(""));
	SetCountText(_T(""));
	StopMask();
	ShowReslessUI(false);
	ShowNetlessUI(false);

	m_nCurrentType = CloudFileAssets;

	

	if(m_PaintManager.FindControl(_T("cloudAssetsWindowLayout")))
	{
		m_pCloudAssetsWindowUI->RefreshData();
		return;
	}

	m_pCloudAssetsWindowUI = CreateAssetsWindow();
	if(m_pCloudAssetsWindowUI)
	{
		m_pCloudAssetsWindowUI->SetName(_T("cloudAssetsWindowLayout"));
		m_pCloudAssetsList->Add(m_pCloudAssetsWindowUI);
	}

}

CAssetsWindowUI* CGroupExplorerUI::CreateAssetsWindow()
{
	CAssetsWindowUI * pAssetsWindow = NULL;

	if( !m_assetsWindowBuilder.GetMarkup()->IsValid() ) {
		pAssetsWindow = dynamic_cast<CAssetsWindowUI*>(m_assetsWindowBuilder.Create(_T("RightBar\\AssetsWindow.xml"), (UINT)0, this, &m_PaintManager));
	}
	else {
		pAssetsWindow = dynamic_cast<CAssetsWindowUI*>(m_assetsWindowBuilder.Create(this, &m_PaintManager));
	}
	if (pAssetsWindow == NULL)
		return NULL;

	return pAssetsWindow;
}

CDBankAssetsWindowUI* CGroupExplorerUI::CreateDBankAssetsWindow()
{
	CDBankAssetsWindowUI * pAssetsWindow = NULL;

	CDialogBuilder		Builder;

	pAssetsWindow = dynamic_cast<CDBankAssetsWindowUI*>(Builder.Create(_T("RightBar\\DBankAssetsWindow.xml"), (UINT)0, this, &m_PaintManager));

	if (pAssetsWindow == NULL)
		return NULL;

	return pAssetsWindow;
}


void CGroupExplorerUI::ShowDBankAssetsUI()
{
	SetDlgAcceptFiles(false);
	m_pSliderTabLayout->SelectItem(5, false);

	CItemExplorerUI::GetInstance()->ShowWindow(false);
	CPanelInnerDlg*		m_pAttchedDlg = dynamic_cast<CPanelInnerDlg*>(CWnd::FromHandle(this->GetHWND()));
	if (m_pAttchedDlg)
	{
		m_pAttchedDlg->SetInnerWidth(400);
	}

	SetTitleText(_T(""));
	SetCountText(_T(""));
	StopMask();
	ShowReslessUI(false);
	ShowNetlessUI(false);

	m_nCurrentType = DBankAssets;



	if(m_PaintManager.FindControl(_T("dbankAssetsWindowLayout")))
	{
		m_pDBankAssetsWindowUI->RefreshData();
		return;
	}

	m_pDBankAssetsWindowUI = CreateDBankAssetsWindow();
	if(m_pDBankAssetsWindowUI)
	{
		m_pDBankAssetsWindowUI->SetName(_T("dbankAssetsWindowLayout"));
		m_pDBankAssetsList->Add(m_pDBankAssetsWindowUI);
	}
}

void CGroupExplorerUI::Show101PPTSearchUI()
{
	SetDlgAcceptFiles(false);
	m_pSliderTabLayout->SelectItem(6, false);

	CItemExplorerUI::GetInstance()->ShowWindow(false);
	CPanelInnerDlg*		m_pAttchedDlg = dynamic_cast<CPanelInnerDlg*>(CWnd::FromHandle(this->GetHWND()));
	if (m_pAttchedDlg)
	{
		m_pAttchedDlg->SetInnerWidth(400);
	}

	SetTitleText(_T(""));
	SetCountText(_T(""));
	StopMask();
	ShowReslessUI(false);
	ShowNetlessUI(false);

	m_nCurrentType = DBankAssets;



	if(m_PaintManager.FindControl(_T("dbankAssetsWindowLayout")))
	{
		m_pDBankAssetsWindowUI->RefreshData();
		return;
	}

	m_pDBankAssetsWindowUI = CreateDBankAssetsWindow();
	if(m_pDBankAssetsWindowUI)
	{
		m_pDBankAssetsWindowUI->SetName(_T("dbankAssetsWindowLayout"));
		m_pDBankAssetsList->Add(m_pDBankAssetsWindowUI);
	}
};
/*
void CGroupExplorerUI::ShowRegisterEmailSuccessUI()
{
	CPanelInnerDlg*		m_pAttchedDlg = dynamic_cast<CPanelInnerDlg*>(CWnd::FromHandle(this->GetHWND()));
	if (m_pAttchedDlg)
	{
		m_pAttchedDlg->SetInnerWidth(400);
	}

	SetTitleText(_T(""));
	SetCountText(_T(""));
	m_pList->RemoveAll();

	CContainerUI* pRegisterEmailSuccessWindow = CreateRegisterEmailSuccessWindow();
	if(pRegisterEmailSuccessWindow)
	{
		m_pList->Add(pRegisterEmailSuccessWindow);

		m_pEmailTipLayout = dynamic_cast<CHorizontalLayoutUI*>(pRegisterEmailSuccessWindow->FindSubControl(_T("EmailTipLayout")));

		CLabelMultiUI* pEmailTipText = dynamic_cast<CLabelMultiUI*>(m_pEmailTipLayout->FindSubControl(_T("EmailTipText")));

		tstring strTip = _T("登陆邮箱");
		strTip += m_strEmail;
		strTip += _T(",点击邮件中的链接完成注册");

		pEmailTipText->SetText(strTip.c_str());

	}
}
*/
void CGroupExplorerUI::OnLogin( TNotifyUI& msg )
{
	m_pLoginWindowUI->Login();
}

void	CGroupExplorerUI::MobileLogin(CStream *loginStream)
{
	ShowLoginUI(TRUE);
	if (m_pLoginWindowUI == NULL)
	{
		return ;
	}
	m_pLoginWindowUI->MobileLogin(loginStream);
}


void CGroupExplorerUI::TimerProcComplete( HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime )
{
	switch(idEvent)
	{
	case WM_LOGIN_COMPLETE:
		{
			KillTimer(hwnd,WM_LOGIN_COMPLETE);
			CGroupExplorerUI::GetInstance()->m_pLoginWindowUI->m_pLoginTipLayout->SetVisible(false);
			if(CGroupExplorerUI::GetInstance()->m_pLoginWindowUI->m_bSetChapter)
			{
				tstring strPath = GetLocalPath();
				strPath += _T("\\Setting\\Config.ini");

				NDCloudSaveChapterTrees(CGroupExplorerUI::GetInstance()->m_pLoginWindowUI->m_pChapterTree);
				NDCloudSaveTrees();

				CCategoryTree * pCategoryTree = NULL;
				NDCloudGetCategoryTree(pCategoryTree);

				tstring strSectionCode = CGroupExplorerUI::GetInstance()->m_pLoginWindowUI->m_strSectionCode;
				tstring strGradeCode = CGroupExplorerUI::GetInstance()->m_pLoginWindowUI->m_strGradeCode;
				tstring strCourseCode = CGroupExplorerUI::GetInstance()->m_pLoginWindowUI->m_strCourseCode;
				tstring strEditionCode = CGroupExplorerUI::GetInstance()->m_pLoginWindowUI->m_strEditionCode;
				tstring strSubEditionCode = CGroupExplorerUI::GetInstance()->m_pLoginWindowUI->m_strSubEditionCode;
				tstring strChapterGuid = CGroupExplorerUI::GetInstance()->m_pLoginWindowUI->m_strChapterGuid;
				tstring strGrade = CGroupExplorerUI::GetInstance()->m_pLoginWindowUI->m_strGrade;

				pCategoryTree->SelectSectionCode(strSectionCode);
				pCategoryTree->SelectGradeCode(strGradeCode);
				pCategoryTree->SelectCourseCode(strCourseCode);
				pCategoryTree->SelectEditionCode(strEditionCode);
				pCategoryTree->SelectSubEditionCode(strSubEditionCode);

				if(pCategoryTree)
				{
					TCHAR szCourseRecord[1024] = {0};
					_stprintf_s(szCourseRecord,_T("%s/%s/%s/%s/%s"),
						strSectionCode.c_str(), 
						strGradeCode.c_str(),
						strCourseCode.c_str(), 
						strEditionCode.c_str(), 
						strSubEditionCode.c_str()
						);


					WritePrivateProfileString(_T("Config"),_T("CourseRecord"),szCourseRecord, strPath.c_str());
					WritePrivateProfileString(_T("Config"),_T("GradeText"), strGrade.c_str(), strPath.c_str());

				}

				ChapterNode *pChapterNode = CGroupExplorerUI::GetInstance()->m_pLoginWindowUI->m_pChapterTree->FindNode(strChapterGuid);
				if(pChapterNode)
					NDCloudSetChapterGUID(strChapterGuid, pChapterNode->strTitle.c_str());
			}
			
			if(CGroupExplorerUI::GetInstance()->m_pSliderTabLayout->GetCurSel() == 0)
			{
				if(  ::IsWindowVisible(CGroupExplorerUI::GetInstance()->GetHWND()))
					CGroupExplorerUI::GetInstance()->ShowWindow(false);
			}
			NDCloudUser::GetInstance()->SetLoginType(0);
			Statistics::GetInstance()->SetUserId(NDCloudUser::GetInstance()->GetUserId());
			NDCloudUser::GetInstance()->SetLoginComplete(true);//设置登陆完成 2016.01.27
			BroadcastEvent(EVT_LOGIN, 0 ,0 ,0);
		}
		break;
	case WM_REGISTER_COMPLETE:
		{
			KillTimer(hwnd,WM_REGISTER_COMPLETE);
			CGroupExplorerUI::GetInstance()->m_pRegisterWindowUI->SetRegisterBtnEnable(true);
			CGroupExplorerUI::GetInstance()->m_pRegisterWindowUI->m_pRegisterTipLayout->SetVisible(false);
			CGroupExplorerUI::GetInstance()->ShowWindow(false);
		}
		break;
	case WM_CHANGEPASS_COMPLETE:
		{
			KillTimer(hwnd,WM_CHANGEPASS_COMPLETE);
			CGroupExplorerUI::GetInstance()->m_pChangePasswordWindowUI->SetChangePasswordBtnEnable(true);
			CGroupExplorerUI::GetInstance()->m_pChangePasswordWindowUI->m_pChangePasswordTipLayout->SetVisible(false);
			CGroupExplorerUI::GetInstance()->ShowWindow(false);
		}
		break;
	}
}

void CGroupExplorerUI::OnEditTabChangeLogin( TNotifyUI& msg )
{
	CControlUI* pControl = msg.pSender;

	if(m_pLoginWindowUI)
		m_pLoginWindowUI->OnEditTabChangeLogin(pControl);
	if(m_pRegisterWindowUI)
		m_pRegisterWindowUI->OnEditTabChangeRegister(pControl);
	if(m_pChangePasswordWindowUI)
		m_pChangePasswordWindowUI->OnEditTabChange(pControl);
}

void CGroupExplorerUI::OnBackLogin( TNotifyUI& msg )
{
	ShowLoginUI();
}

void CGroupExplorerUI::OnBackRegister( TNotifyUI& msg )
{
	ShowRegisterUI();
}

void CGroupExplorerUI::OnRegister( TNotifyUI& msg )
{
	m_pRegisterWindowUI->Register();
}

void CGroupExplorerUI::OnChangePassword( TNotifyUI& msg )
{
	m_pChangePasswordWindowUI->ChangePassword();
}	

/*
void CGroupExplorerUI::OnOpenEmailUrlBtn( TNotifyUI& msg )
{
	const char* szBuff = _tcsrchr(m_strEmail.c_str(), _T('@'));
	tstring strHost;
	if(szBuff)
	{
		strHost = szBuff+1;
	}
	tstring strUrl = _T("http://mail.");
	strUrl += strHost;
	ShellExecute(NULL,"open",strUrl.c_str(),NULL,NULL,SW_SHOW);
}
*/
void CGroupExplorerUI::OnGoGuide( TNotifyUI& msg )
{
	ShellExecute(NULL, _T("open"),_T("http://ppt.101.com/"),NULL,NULL,SW_SHOW);
}

void CGroupExplorerUI::OnForgetPassword( TNotifyUI& msg )
{
	//ShellExecute(NULL,"open",_T("http://esp-user.edu.web.sdp.101.com/#/forget"),NULL,NULL,SW_SHOW);
	ShowGetBackPwdUI();
}

void CGroupExplorerUI::OnShowStatement( TNotifyUI& msg )
{
	//ShellExecute(NULL,"open",_T("http://esp-user.edu.web.sdp.101.com/register/home/statement.html"),NULL,NULL,SW_SHOW);
	CVerticalLayoutUI * pStatementLayout = NULL;
	if( !m_statementBuilder.GetMarkup()->IsValid() ) {
		pStatementLayout = dynamic_cast<CVerticalLayoutUI*>(m_statementBuilder.Create(_T("RightBar\\Statement.xml"), (UINT)0, this, &m_PaintManager));
	}
	else {
		pStatementLayout = dynamic_cast<CVerticalLayoutUI*>(m_statementBuilder.Create(this, &m_PaintManager));
	}

	if(pStatementLayout)
	{
		m_pList->RemoveAll();
		m_pList->Add(pStatementLayout);

		CLabelMultiUI* pLabelMulti	 = dynamic_cast<CLabelMultiUI*>(pStatementLayout->FindSubControl(_T("statementText")));
		CRect rc(0,0,350,25);

		tstring strText = pLabelMulti->GetText();
		int nFont = 102;
		CRenderEngine::DrawText(::GetDC(m_hWnd), &m_PaintManager, rc, strText.c_str(), 0, nFont, DT_CALCRECT|DT_EDITCONTROL|DT_WORDBREAK);

		pLabelMulti->SetFixedHeight(rc.bottom - rc.top);
	}
}

bool CGroupExplorerUI::DeleteLocalItem( tstring strItemPath )
{
	int nCount = 0;
	bool bRet = ((CListBarDlg*)m_pListBar)->LocalResItemDelFile(GetTitleText(), strItemPath, nCount);
	if ( bRet )
	{
		if ( nCount == 0 )
		{
			CItemExplorerUI::GetInstance()->ShowWindow(false);
			CStream Steam(1024);
			Steam.WriteDWORD(nCount);
			Steam.WriteString(GetTitleText());

			ShowResource(m_nCurrentType, &Steam);
		}
		else
		{
			SetCountText(nCount);
			int nVisibleCount = 0;
			if(m_pItemPageUI)
			{
				CVerticalLayoutUI* m_pList = m_pItemPageUI->GetContentLayout();
				if(m_pList)
				{
					for(int i=0;i<m_pList->GetCount();i++)
					{
						CResourceItemUI* pItem = static_cast<CResourceItemUI *>(m_pList->GetItemAt(i));
						if(pItem)
						{
							string filePath = pItem->GetResource();
							if(filePath.compare(strItemPath)==0)
							{
								pItem->SetVisible(false);
								if (m_nCurrentType == LocalFileCourse)
								{
									if (CItemExplorerUI::GetInstance()->GetParentItem() == pItem)
									{
										CItemExplorerUI::GetInstance()->ShowWindow(false);
									}
								}
							}
							else if (pItem->IsVisible())
							{
								nVisibleCount++;
							}
						}
					}
				}
			}

			if (nVisibleCount == 0)
			{
				CStream Steam(1024);
				Steam.WriteDWORD(nCount);
				Steam.WriteString(GetTitleText());

				ShowResource(m_nCurrentType, &Steam);
			}
		}
	}
	return bRet;
}

bool CGroupExplorerUI::RenameLocalItem( tstring strItemPath ,tstring strNewName,bool isModifyTitleOnly)
{
	return ((CListBarDlg*)m_pListBar)->LocalResItemRenameFile(GetTitleText(), strItemPath,strNewName,isModifyTitleOnly);
}

void CGroupExplorerUI::OnSearchRefresh( TNotifyUI& msg )
{
	OnSearch(msg);

	BroadcastEvent(EVT_REFRESH_GROUPEXPLORER, (WPARAM)TRUE, 0 ,0);
}

bool CGroupExplorerUI::OnLoginWindowDestroy( void* pObj )
{
	m_pLoginWindowUI = NULL;
	return true;
}

bool CGroupExplorerUI::OnRegisterWindowDestroy( void* pObj )
{
	m_pRegisterWindowUI = NULL;
	return true;
}

void CGroupExplorerUI::OnRefrshCheckCode( TNotifyUI& msg )
{
	if(m_pSliderTabLayout->GetCurSel() == 1 && m_pRegisterWindowUI)
		m_pRegisterWindowUI->OnRefrshCheckCode();
	if(m_pSliderTabLayout->GetCurSel() == 3 && m_pGetBackPwdWindowUI)
		m_pGetBackPwdWindowUI->OnRefrshCheckCode();
}

void CGroupExplorerUI::ShowUploadUI()
{
	m_laySeeMore->SetVisible(false);
	m_pSliderTabLayout->SelectItem(1,false);
	SetDlgAcceptFiles(false);
	CItemExplorerUI::GetInstance()->ShowWindow(false);
	CPanelInnerDlg*		m_pAttchedDlg = dynamic_cast<CPanelInnerDlg*>(CWnd::FromHandle(this->GetHWND()));
	if (m_pAttchedDlg)
	{
		m_pAttchedDlg->SetInnerWidth(500);
	}

	SetTitleText(_T(""));
	SetCountText(_T(""));
	m_pList->RemoveAll();
	ShowNetlessUI(false);
	ShowReslessUI(false);
	StopMask();

	CDBankUploadUI* pUpload= NULL;
	if( !m_uploadBuilder.GetMarkup()->IsValid() ) {
		pUpload = dynamic_cast<CDBankUploadUI*>(m_uploadBuilder.Create(_T("Upload\\Upload.xml"), (UINT)0, this, &m_PaintManager));
	}
	else {
		pUpload = dynamic_cast<CDBankUploadUI*>(m_uploadBuilder.Create(this, &m_PaintManager));
	}

	if(pUpload)
	{
		m_pList->Add(pUpload);
	}

	m_nCurrentType = DBankUpload;
}

LPCTSTR CGroupExplorerUI::GetFileFilter( int nType )
{
	switch (nType)
	{
	case LocalFileImport:
		return FILTER_IMPORT_ALL;
	case LocalFileCourse:
		return FILTER_PPT_ALL;
	case LocalFileImage:
		return FILTER_PICTURE;
	case LocalFileFlash:
		return FILTER_FALSH;
	case LocalFileVideo:
		return FILTER_VIDEO_ALL;
	case LocalFileVolume:
		return FILTER_VOLMUE;
	}
	return FILTER_ALL;
}

list<LOCAL_RES_INFO>* CGroupExplorerUI::LocalResSortByChapter( list<LOCAL_RES_INFO>* lstResInfo, tstring strChpater )
{
	lstResInfo->sort(compare_Chapter);
	list<LOCAL_RES_INFO> lstTempResInfo;

	list<LOCAL_RES_INFO>::iterator iter;

	for ( iter = lstResInfo->begin(); iter != lstResInfo->end(); iter)
	{
		if (iter->strChapter == strChpater )
		{
			lstTempResInfo.push_back(*iter);
			iter = lstResInfo->erase(iter);
		}
		else
			iter++;
	}

	for ( iter = lstResInfo->begin(); iter != lstResInfo->end(); iter++)
	{
		lstTempResInfo.push_back(*iter);
	}

	lstResInfo->swap(lstTempResInfo);

	return lstResInfo;
}

void CGroupExplorerUI::OnCheckBoxAutoLogin( TNotifyUI& msg )
{
	m_pLoginWindowUI->SetCheckBoxAutoLogin();
}

void CGroupExplorerUI::OnCheckBoxSavePassword( TNotifyUI& msg )
{
	m_pLoginWindowUI->SetCheckBoxSavePassword();
}

void CGroupExplorerUI::OnGetBackPwd(TNotifyUI& msg)
{
	m_pGetBackPwdWindowUI->GetBackPwd(msg);
}

void CGroupExplorerUI::OnSentPhoneCode( TNotifyUI& msg )
{
	m_pGetBackPwdWindowUI->OnSentPhoneCode();
}

void CGroupExplorerUI::OnCloudItemRClick( TNotifyUI& msg )
{
	CCloudItemUI* pItem = dynamic_cast<CCloudItemUI*>(msg.pSender);
	if(pItem)
	{
		int index=0;
		POINT ptMouse;
		GetCursorPos(&ptMouse);
		CRect rtMain;
		GetWindowRect(this->GetHWND(), &rtMain);
		for(int i=0;i<m_pList->GetCount();i++)
		{
			CControlUI* pItemTmp=m_pList->GetItemAt(i);
			if(pItemTmp&&pItemTmp->IsVisible())
			{
				RECT rt = pItemTmp->GetPos();
				RECT rtPadding = pItemTmp->GetPadding();
				if(rtMain.left+rt.left<=ptMouse.x&&rtMain.top+rt.top<=ptMouse.y&&rtMain.right-rtPadding.right>=ptMouse.x&&rtMain.top+rt.bottom>=ptMouse.y)
				{
					index=i;
					break;
				}
			}
		}
		bool show=true;
		if(m_pMenuUI==NULL||m_pMenuUI->GetHWND()==NULL)
		{
			m_pMenuUI = new CMenuUI;
			m_pMenuUI->Create(AfxGetMainWnd()->GetSafeHwnd(), _T("MenuUI"), WS_VISIBLE, 0, 0, 0, 0);
		}
		m_pMenuUI->HideAllMenuItem();
		if(m_nCurrentType==CloudFileCourse||m_nCurrentType==CloudFilePPTTemplate)
		{
			m_pMenuUI->ShowMenuDownloadLocal();
		}
		else if(m_nCurrentType==CloudFileVideo||m_nCurrentType==CloudFileImage||
			m_nCurrentType==CloudFileFlash||m_nCurrentType==CloudFileVolume)
		{
			m_pMenuUI->ShowMenuDownloadLocal();
		}
		else if(m_nCurrentType==CloudFileQuestion||m_nCurrentType==CloudFileCoursewareObjects)
		{
			m_pMenuUI->ShowMenuDownloadLocal();
		}
		else if(m_nCurrentType==DBankCourse)
		{
			CNdpCourseItemUI* pNdpCourseItem = dynamic_cast<CNdpCourseItemUI*>(msg.pSender);
			if(!pNdpCourseItem)//ndp课件暂不支持下载到本地库，目前暂时屏蔽ndp课件右键显示下载到本地功能
			{
				m_pMenuUI->ShowMenuDownloadLocal();
			}
			m_pMenuUI->ShowMenuDelete();
			m_pMenuUI->ShowMenuRename();
		}
		else if(m_nCurrentType==DBankVideo||m_nCurrentType==DBankImage||
			m_nCurrentType==DBankFlash||m_nCurrentType==DBankVolume)
		{
			m_pMenuUI->ShowMenuDownloadLocal();
			m_pMenuUI->ShowMenuDelete();
			m_pMenuUI->ShowMenuRename();
		}
		else if(m_nCurrentType==DBankQuestion||m_nCurrentType==DBankCoursewareObjects)
		{
			m_pMenuUI->ShowMenuDownloadLocal();
			m_pMenuUI->ShowMenuDelete();
		}
		if(show)
		{
			m_pMenuUI->SetIsCloudResource(true);
			m_pMenuUI->SetItemIndex(index);
			m_pMenuUI->ShowWindow(true);
		}
	}
}

void CGroupExplorerUI::OnLocalItemRClick( TNotifyUI& msg )
{
	CLocalItemUI* pItem = dynamic_cast<CLocalItemUI*>(msg.pSender);
	if(pItem)
	{
		int index=0;
		POINT ptMouse;
		GetCursorPos(&ptMouse);
		char cDebug[MAX_PATH] = {0};
		_stprintf(cDebug,_T("RClick mouse pos :%d %d \r\n"),ptMouse.x,ptMouse.y);
		string strDebug=cDebug;
		OutputDebugString(strDebug.c_str());
		CRect rtMain;
		GetWindowRect(this->GetHWND(), &rtMain);
		if(m_pItemPageUI)
		{
			CVerticalLayoutUI* m_pList = m_pItemPageUI->GetContentLayout();
			if(m_pList)
			{
				for(int i=0;i<m_pList->GetCount();i++)
				{
					CControlUI* pItemTmp=m_pList->GetItemAt(i);
					if(pItemTmp&&pItemTmp->IsVisible())
					{
						RECT rt = pItemTmp->GetPos();
						RECT rtPadding = pItemTmp->GetPadding();
						if(rtMain.left+rt.left<=ptMouse.x&&rtMain.top+rt.top<=ptMouse.y&&rtMain.right-rtPadding.right>=ptMouse.x&&rtMain.top+rt.bottom>=ptMouse.y)
						{
							index=i;
							break;
						}
					}
				}
			}
		}
		if(m_pMenuUI==NULL||m_pMenuUI->GetHWND()==NULL)
		{
			m_pMenuUI = new CMenuUI;
			m_pMenuUI->Create(AfxGetMainWnd()->GetSafeHwnd(), _T("MenuUI"), WS_VISIBLE, 0, 0, 0, 0);
		}
		m_pMenuUI->HideAllMenuItem();
		if(m_nCurrentType==LocalFileCourse)
		{
			m_pMenuUI->ShowMenuUploadNetDisc();
			m_pMenuUI->ShowMenuDelete();
			m_pMenuUI->ShowMenuRename();
		}
		else if(m_nCurrentType==LocalFileVideo||m_nCurrentType==LocalFileImage||m_nCurrentType==LocalFileFlash||m_nCurrentType==LocalFileVolume)
		{
			m_pMenuUI->ShowMenuUploadNetDisc();
			m_pMenuUI->ShowMenuDelete();
			m_pMenuUI->ShowMenuRename();
		}
		else if(m_nCurrentType==LocalFileBasicExercises||m_nCurrentType==LocalFileInteractiveExercises)
		{
			m_pMenuUI->ShowMenuUploadNetDisc();
			m_pMenuUI->ShowMenuDelete();
			m_pMenuUI->ShowMenuRename();
			m_pMenuUI->ShowMenuEditExercises();
		}
		m_pMenuUI->SetIsCloudResource(false);
		m_pMenuUI->SetItemIndex(index);
		m_pMenuUI->ShowWindow(true);
	}
}

bool CGroupExplorerUI::DownLoadToLocal(void* pObj)
{
	if(pObj)
	{
		TEventNotify* pNotify = (TEventNotify*)pObj;
		if(pNotify)
		{
			bool isTypeDownload=(bool)pNotify->wParam;
			int nValue=(int)pNotify->lParam;
			if(isTypeDownload)//类型下载
			{
				for(int i=0;i<m_pList->GetCount();i++)
				{
					CNdpCourseItemUI* pNdpCourseItem = dynamic_cast<CNdpCourseItemUI*>(m_pList->GetItemAt(i));
					if(!pNdpCourseItem)//ndp课件暂不支持下载到本地库，目前暂时屏蔽ndp课件下载到本地
					{
						CCloudItemUI* pItem = dynamic_cast<CCloudItemUI *>(m_pList->GetItemAt(i));
						if(pItem)
						{
							pItem->DownloadLocal();
						}
					}
				}
			}
			else if(nValue>=0)//单个资源下载
			{
				CCloudItemUI* pItem = dynamic_cast<CCloudItemUI *>(m_pList->GetItemAt(nValue));
				if(pItem)
				{
					pItem->DownloadLocal();
				}
			}
		}
	}
	return true;
}

bool CGroupExplorerUI::DownLoadToLocalCompelete( void* pObj )
{
	if(pObj)
	{
		TEventNotify* pNotify = (TEventNotify*)pObj;
		if(pNotify)
		{
			int type=(int)pNotify->wParam;
			vector<tstring>* args = (vector<tstring>*)pNotify->lParam;
			if(args&&args->size()==2)
			{
				vector<tstring> vArgs;
				for (vector<tstring>::iterator item = args->begin();item != args->end();item++)
				{
					vArgs.push_back(*item);
				}
				tstring strTitle = vArgs[0];
				tstring filePath = vArgs[1];
				if(!filePath.empty())
				{
					if ( GetFileAttributes(filePath.c_str()) != INVALID_FILE_ATTRIBUTES )
					{
						CFileTypeFilter fileTypeFilter;
						tstring strGuid		= NDCloudGetChapterGUID();
						//tstring strChapter	= NDCloudCurrentChapter();;
						m_nLocalItemType = fileTypeFilter.GetFileType(filePath.c_str());
						LOCAL_RES_INFO resInfo;
						resInfo.nType			= m_nLocalItemType - FILE_FILTER + LocalFileImport;
						resInfo.strResPath		= filePath.c_str();
						resInfo.strChapterGuid	= strGuid;
						resInfo.strChapter		= m_strChapter;
						resInfo.strTitle = strTitle;
						list<LOCAL_RES_INFO> listFiles;
						listFiles.push_back(resInfo);
						bool bFind=false;
						if(type==1)
						{
							((CListBarDlg*)m_pListBar)->LocalResItemAddRes(LOCAL_FOLDER_NAME_COURSE, listFiles);
						}
						else if(type==2)
						{
							((CListBarDlg*)m_pListBar)->LocalResItemAddRes(LOCAL_FOLDER_NAME_PIC, listFiles);
						}
						else if(type==3)
						{
							((CListBarDlg*)m_pListBar)->LocalResItemAddRes(LOCAL_FOLDER_NAME_FALSH, listFiles);
						}
						else if(type==4)
						{
							((CListBarDlg*)m_pListBar)->LocalResItemAddRes(LOCAL_FOLDER_NAME_VIDEO, listFiles);
						}
						else if(type==5)
						{
							((CListBarDlg*)m_pListBar)->LocalResItemAddRes(LOCAL_FOLDER_NAME_VOLUME, listFiles);
						}
						else if(type==6)
						{
							((CListBarDlg*)m_pListBar)->LocalResItemAddRes(LOCAL_FOLDER_BASIC_EXERCISES, listFiles);
						}
						else if(type==7)
						{
							((CListBarDlg*)m_pListBar)->LocalResItemAddRes(LOCAL_FOLDER_INTERACTIVE_EXERCISES, listFiles);
						}
						((CListBarDlg*)m_pListBar)->SaveFoldersInfo();
					}
				}
			}
		}
	}
	return true;
}

bool CGroupExplorerUI::UploadNetdisc( void* pObj )
{
	if(pObj)
	{
		TEventNotify* pNotify = (TEventNotify*)pObj;
		if(pNotify)
		{
			int nValue=(int)pNotify->wParam;
			bool isClound=(bool)pNotify->lParam;
			CResourceItemUI* pItem = NULL;
			if(isClound)
			{
				pItem =static_cast<CResourceItemUI *>(m_pList->GetItemAt(nValue));
			}
			else
			{
				if(m_pItemPageUI)
				{
					CVerticalLayoutUI* m_pList = m_pItemPageUI->GetContentLayout();
					if(m_pList)
					{
						pItem =static_cast<CResourceItemUI *>(m_pList->GetItemAt(nValue));
					}
				}
			}
			if(pItem)
			{
				DWORD dwUserID = NDCloudUser::GetInstance()->GetUserId();
				if (dwUserID != 0)
				{
					pItem->UploadNetdisc(m_nCurrentType);
				}
				else
				{
					CToast::GetInstance(AfxGetMainWnd()->GetSafeHwnd())->Show(_T("请先登录"));
				}
			}			
		}
	}
	return true;
}

bool CGroupExplorerUI::UploadNetdiscCompelete( void* pObj )
{
	//notify dbank item refresh count
	//CGroupExplorerUI::GetInstance()->AddDBankItemCount(m_nType - FILE_FILTER_PPT);
	if(pObj)
	{
		TEventNotify* pNotify = (TEventNotify*)pObj;
		if(pNotify)
		{
			int errorCode=(int)pNotify->wParam;
			if(errorCode==0)
			{
				int uploadModeType = (int)pNotify->lParam;
				AddDBankItemCount(uploadModeType);
			}
		}
	}
	return true;
}

bool CGroupExplorerUI::DeleteResource( void* pObj )
{
	if(pObj)
	{
		TEventNotify* pNotify = (TEventNotify*)pObj;
		if(pNotify)
		{
			int nValue=(int)pNotify->wParam;
			bool isClound=(bool)pNotify->lParam;
			CResourceItemUI* pItem = NULL;
			if(isClound)
			{
				pItem =static_cast<CResourceItemUI *>(m_pList->GetItemAt(nValue));
				if(pItem)
				{
					DWORD dwUserID = NDCloudUser::GetInstance()->GetUserId();
					if (dwUserID != 0)
					{
						if (m_nCurrentType == DBankCourse && CItemExplorerUI::GetInstance()->GetParentItem() == pItem)
						{
							CItemExplorerUI::GetInstance()->ShowWindow(false);
						}
						pItem->Delete(m_nCurrentType);
					}
					else
					{
						CToast::GetInstance(AfxGetMainWnd()->GetSafeHwnd())->Show(_T("请先登录"));
					}
				}
			}
			else
			{
				if(m_pItemPageUI)
				{
					CVerticalLayoutUI* m_pList = m_pItemPageUI->GetContentLayout();
					if(m_pList)
					{
						pItem = static_cast<CResourceItemUI *>(m_pList->GetItemAt(nValue));
						if(pItem)
						{
							tstring strTitle = pItem->GetTitle();
							tstring filePath = pItem->GetResource();
							if(!filePath.empty())
							{
								tstring strToast=_T("“") + strTitle + _T("”");
								if(DeleteLocalItem(filePath))
								{
									strToast += _T("删除成功");
									CToast::Toast(strToast);
								}
								else
								{
									strToast += _T("删除失败");
									CToast::Toast(strToast);
								}
							}
						}
					}
				}
			}	
		}
	}
	return true;
}

bool CGroupExplorerUI::DeleteResourceCompelete( void* pObj )
{
	if(pObj)
	{
		TEventNotify* pNotify = (TEventNotify*)pObj;
		if(pNotify)
		{
			int errorCode=(int)pNotify->wParam;
			int deleteModeType=0;
			string strResource = "";
			string strTitle = "";
			string strErrorMessage = "";
			CStream* pStream = (CStream*)pNotify->lParam;
			if(pStream)
			{
				pStream->ResetCursor();
				deleteModeType = pStream->ReadInt();
				strResource  = pStream->ReadString();
				strTitle = pStream->ReadString();
				strErrorMessage = pStream->ReadString();
			}
			if(errorCode==0)
			{
				if(strTitle.empty())
				{
					CToast::Toast(_T("资源删除成功"));
				}
				else
				{
					tstring strToast = _T("“") + strTitle + _T("”删除成功");
					CToast::Toast(strToast);
				}
				//刷新A框
				AddDBankItemCount(deleteModeType);
				//界面显示刷新
				if(m_nCurrentType == deleteModeType)
				{
					for(int i=0;i<m_pList->GetCount();i++)
					{
						CResourceItemUI* pItem = static_cast<CResourceItemUI *>(m_pList->GetItemAt(i));
						if(pItem)
						{
							if(strResource.compare(pItem->GetResource())==0)
							{
								pItem->SetVisible(false);
								if(m_lbCount)
								{
									string strCount = m_lbCount->GetText();
									int count = atoi(strCount.c_str());
									if(--count>=0)
									{
										SetCountText(count);
									}
								}
							}
						}
					}
				}
			}
			else
			{
				if(strErrorMessage.empty())
				{
					if(strTitle.empty())
					{
						CToast::Toast(_T("资源删除失败"));
					}
					else
					{
						tstring strToast = _T("“") + strTitle + _T("”删除失败");
						CToast::Toast(strToast);
					}
				}
				else
				{
					CToast::Toast(strErrorMessage);
				}
			}
		}
	}
	return true;
}

bool CGroupExplorerUI::RenameResource( void* pObj )
{
	if(pObj)
	{
		TEventNotify* pNotify = (TEventNotify*)pObj;
		if(pNotify)
		{
			int nValue=(int)pNotify->wParam;
			bool isClound=(bool)pNotify->lParam;
			CResourceItemUI* pItem = NULL;
			if(isClound)
			{
				pItem =static_cast<CResourceItemUI *>(m_pList->GetItemAt(nValue));
			}
			else
			{
				if(m_pItemPageUI)
				{
					CVerticalLayoutUI* m_pList = m_pItemPageUI->GetContentLayout();
					if(m_pList)
					{
						pItem = static_cast<CResourceItemUI *>(m_pList->GetItemAt(nValue));
					}
				}
			}
			if(pItem)
			{
				//弹出输入框
				CRenameDlgUI* pRenameDlg=new CRenameDlgUI();
				HWND hwnd = AfxGetMainWnd()->GetSafeHwnd();
				pRenameDlg->Create(hwnd, _T("RenameWindow"), WS_POPUP , 0, 0, 0, 0, 0);
				pRenameDlg->SetOldName2Edit(pItem->GetTitle());
				pRenameDlg->AdjustWindowPos();

				if(pRenameDlg->ShowModal()==IDOK)
				{
					string strNewName = pRenameDlg->GetNewName();
					if(!strNewName.empty())
					{
						if(isClound)
						{
							DWORD dwUserID = NDCloudUser::GetInstance()->GetUserId();
							if (dwUserID != 0)
							{
								pItem->Rename(strNewName);
							}
							else
							{
								CToast::GetInstance(AfxGetMainWnd()->GetSafeHwnd())->Show(_T("请先登录"));
							}
						}
						else
						{
							bool isModifyTitleOnly = true;//重名只修改title
							if(m_pItemPageUI)
							{
								CVerticalLayoutUI* m_pList = m_pItemPageUI->GetContentLayout();
								if(m_pList)
								{
									CLocalQuestionItemUI * pLocalQuestionItem = dynamic_cast<CLocalQuestionItemUI *>(m_pList->GetItemAt(nValue));
									CLocalInteractiveQuestionItemUI* pLocalInteractiveQuestionItem = dynamic_cast<CLocalInteractiveQuestionItemUI *>(m_pList->GetItemAt(nValue));
									if(pLocalQuestionItem||pLocalInteractiveQuestionItem)//本地习题或者是互动习题，只修改title
									{
										isModifyTitleOnly = true;
									}
								}
							}
							tstring filePath = pItem->GetResource();
							if(!filePath.empty())
							{
								if(RenameLocalItem(filePath,strNewName,isModifyTitleOnly))
								{
									pItem->SetTitle(strNewName.c_str());
									if(!isModifyTitleOnly)
									{
										tstring strFileDicPath = filePath.substr(0,filePath.find_last_of(_T("\\"))+1);
										tstring strNewPath = strFileDicPath + strNewName + filePath.substr(filePath.find_last_of(_T(".")));
										pItem->SetResource(strNewPath.c_str());
									}
									CToast::Toast(_T("资源重命名成功"));
								}
								else
								{
									CToast::Toast(_T("资源重命名失败"));
								}
							}
						}
					}
				}
			}
		}
	}
	return true;
}


bool CGroupExplorerUI::RenameResourceCompelete( void* pObj )
{
	if(pObj)
	{
		TEventNotify* pNotify = (TEventNotify*)pObj;
		if(pNotify)
		{
			int errorCode=(int)pNotify->wParam;
			if(errorCode==0)
			{
				CToast::Toast(_T("资源重命名成功"));
			}
			else
			{
				CToast::Toast("资源重命名失败");
			}
		}
	}
	return true;
}

bool CGroupExplorerUI::EditExercises( void* pObj )
{
	if(pObj)
	{
		TEventNotify* pNotify = (TEventNotify*)pObj;
		if(pNotify)
		{
			int nValue=(int)pNotify->wParam;
			bool isClound=(bool)pNotify->lParam;
			CResourceItemUI* pItem = NULL;
			if(isClound)
			{
				pItem =static_cast<CResourceItemUI *>(m_pList->GetItemAt(nValue));
			}
			else
			{
				if(m_pItemPageUI)
				{
					CVerticalLayoutUI* m_pList = m_pItemPageUI->GetContentLayout();
					if(m_pList)
					{
						pItem = static_cast<CResourceItemUI *>(m_pList->GetItemAt(nValue));
					}
				}
			}
			if(pItem)
			{
				pItem->EditExercises();
			}
		}
	}
	return true;
}

int CGroupExplorerUI::GetCurrentType()
{
	return m_nCurrentType;
}

void CGroupExplorerUI::AddDBankItemCount( int nItemType )
{
	((CListBarDlg*)m_pListBar)->GetDBankItemCurCount(nItemType);
}

LRESULT CGroupExplorerUI::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( uMsg == WM_KEYDOWN && wParam == VK_RETURN)
	{

		//登录
		CEditClearUI* pEditName = dynamic_cast<CEditClearUI*>(m_PaintManager.FindControl(_T("username")));
		CEditClearUI* pEditPassword = dynamic_cast<CEditClearUI*>(m_PaintManager.FindControl(_T("password")));

		if (pEditName != NULL && pEditName->IsFocused())
		{
			pEditPassword->SetFocus();
			return 0;
		}
		if (pEditPassword != NULL && pEditPassword->IsFocused())
		{
			CButtonUI* pBtnLogon = dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(_T("LoginBtn")));
			pBtnLogon->SetFocus();
			m_pLoginWindowUI->Login();
			return 0;
		}

		//注册
		CEditClearUI* pEditRegisterName = dynamic_cast<CEditClearUI*>(m_PaintManager.FindControl(_T("registerusername")));
		CEditClearUI* pEditRegisterPassword = dynamic_cast<CEditClearUI*>(m_PaintManager.FindControl(_T("registerpassword")));
		CEditClearUI* pEditRegisterConfirmPassword = dynamic_cast<CEditClearUI*>(m_PaintManager.FindControl(_T("registerconfirmpassword")));
		CEditClearUI* pEditCheckcode = dynamic_cast<CEditClearUI*>(m_PaintManager.FindControl(_T("checkcode")));

		if (pEditRegisterName != NULL && pEditRegisterName->IsFocused())
		{
			pEditRegisterPassword->SetFocus();
			return 0;
		}
		if (pEditRegisterPassword != NULL && pEditRegisterPassword->IsFocused())
		{
			pEditRegisterConfirmPassword->SetFocus();
			return 0;
		}
		if (pEditRegisterConfirmPassword != NULL && pEditRegisterConfirmPassword->IsFocused())
		{
			pEditCheckcode->SetFocus();
			return 0;
		}
		if (pEditCheckcode != NULL && pEditCheckcode->IsFocused())
		{
			CButtonUI* pBtnLogon = dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(_T("RegisterBtn")));
			pBtnLogon->SetFocus();
			m_pRegisterWindowUI->Register();
			return 0;
		}

		//找回密码

		//确认账号
		CEditClearUI* pEditGetbackPwdAccount = dynamic_cast<CEditClearUI*>(m_PaintManager.FindControl(_T("getbackpwdaccount")));
		CEditClearUI* pEditGetbackPwdCheckcode = dynamic_cast<CEditClearUI*>(m_PaintManager.FindControl(_T("getbackpwdcheckcode")));

		if (pEditGetbackPwdAccount != NULL && pEditGetbackPwdAccount->IsFocused())
		{
			pEditGetbackPwdCheckcode->SetFocus();
			return 0;
		}
		if (pEditGetbackPwdCheckcode != NULL && pEditGetbackPwdCheckcode->IsFocused())
		{
			CButtonUI* pBtnStep1 = dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(_T("GetBackPwdStep1Btn")));
			pBtnStep1->SetFocus();

			TNotifyUI msg;
			msg.pSender = pBtnStep1;

			OnGetBackPwd(msg);
			return 0;
		}

		//安全验证
		CEditClearUI* pEditGetbackPwdstep2Checkcode = dynamic_cast<CEditClearUI*>(m_PaintManager.FindControl(_T("getbackpwdstep2checkcode")));

		if (pEditGetbackPwdstep2Checkcode != NULL && pEditGetbackPwdstep2Checkcode->IsFocused())
		{
			CButtonUI* pBtnStep2 = dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(_T("GetBackPwdStep2Btn")));
			pBtnStep2->SetFocus();

			TNotifyUI msg;
			msg.pSender = pBtnStep2;

			OnGetBackPwd(msg);
			return 0;
		}

		//重置密码
		CEditClearUI* pEditGetbackPwdPassword = dynamic_cast<CEditClearUI*>(m_PaintManager.FindControl(_T("getbackpwdpassword")));
		CEditClearUI* pEditGetbackPwdConfirmPassword = dynamic_cast<CEditClearUI*>(m_PaintManager.FindControl(_T("getbackpwdconfirmpassword")));

		if (pEditGetbackPwdPassword != NULL && pEditGetbackPwdPassword->IsFocused())
		{
			pEditGetbackPwdConfirmPassword->SetFocus();
			return 0;
		}
		if (pEditGetbackPwdConfirmPassword !=NULL && pEditGetbackPwdConfirmPassword->IsFocused())
		{
			CButtonUI* pBtnStep3 = dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(_T("GetBackPwdStep3Btn")));
			pBtnStep3->SetFocus();

			TNotifyUI msg;
			msg.pSender = pBtnStep3;

			OnGetBackPwd(msg);
			return 0;
		}


		//修改密码
		CEditClearUI* pEditOldPassword = dynamic_cast<CEditClearUI*>(m_PaintManager.FindControl(_T("oldpassword")));
		CEditClearUI* pEditNewPassword = dynamic_cast<CEditClearUI*>(m_PaintManager.FindControl(_T("newpassword")));
		CEditClearUI* pEditNewConfirmPassword = dynamic_cast<CEditClearUI*>(m_PaintManager.FindControl(_T("newconfirmpassword")));

		if (pEditOldPassword != NULL && pEditOldPassword->IsFocused())
		{
			pEditNewPassword->SetFocus();
			return 0;
		}
		if (pEditNewPassword != NULL && pEditNewPassword->IsFocused())
		{
			pEditNewConfirmPassword->SetFocus();
			return 0;
		}
		if (pEditNewConfirmPassword != NULL && pEditNewConfirmPassword->IsFocused())
		{
			CButtonUI* pBtnChangePassword = dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(_T("ChangePasswordBtn")));
			pBtnChangePassword->SetFocus();
			m_pChangePasswordWindowUI->ChangePassword();

			return 0;
		}

		//搜索
		if(m_pSearchWindowUI)
		{
			CEditClearUI* pSearchResEdit = dynamic_cast<CEditClearUI*>(m_pSearchWindowUI->FindSubControl(_T("searchEdit")));
			CButtonUI *pSearchBtn = dynamic_cast<CButtonUI*>(m_pSearchWindowUI->FindSubControl(_T("searchBtn")));
			if (pSearchResEdit != NULL && pSearchBtn != NULL && pSearchResEdit->IsFocused())
			{
				m_PaintManager.SetFocus(pSearchBtn);
				m_PaintManager.SendNotify(pSearchBtn, DUI_MSGTYPE_CLICK);
				return 0;
			}
		}
	


	}

	if( uMsg == WM_KEYDOWN && wParam == VK_TAB)
	{

		//登录
		CEditClearUI* pEditName = dynamic_cast<CEditClearUI*>(m_PaintManager.FindControl(_T("username")));
		CEditClearUI* pEditPassword = dynamic_cast<CEditClearUI*>(m_PaintManager.FindControl(_T("password")));
		//注册
		CEditClearUI* pEditRegisterName = dynamic_cast<CEditClearUI*>(m_PaintManager.FindControl(_T("registerusername")));
		CEditClearUI* pEditRegisterPassword = dynamic_cast<CEditClearUI*>(m_PaintManager.FindControl(_T("registerpassword")));
		CEditClearUI* pEditRegisterConfirmPassword = dynamic_cast<CEditClearUI*>(m_PaintManager.FindControl(_T("registerconfirmpassword")));
		CEditClearUI* pEditCheckcode = dynamic_cast<CEditClearUI*>(m_PaintManager.FindControl(_T("checkcode")));
		//确认账号
		CEditClearUI* pEditGetbackPwdAccount = dynamic_cast<CEditClearUI*>(m_PaintManager.FindControl(_T("getbackpwdaccount")));
		CEditClearUI* pEditGetbackPwdCheckcode = dynamic_cast<CEditClearUI*>(m_PaintManager.FindControl(_T("getbackpwdcheckcode")));
		//安全验证
		CEditClearUI* pEditGetbackPwdstep2Checkcode = dynamic_cast<CEditClearUI*>(m_PaintManager.FindControl(_T("getbackpwdstep2checkcode")));
		//重置密码
		CEditClearUI* pEditGetbackPwdPassword = dynamic_cast<CEditClearUI*>(m_PaintManager.FindControl(_T("getbackpwdpassword")));
		CEditClearUI* pEditGetbackPwdConfirmPassword = dynamic_cast<CEditClearUI*>(m_PaintManager.FindControl(_T("getbackpwdconfirmpassword")));
		//修改密码
		CEditClearUI* pEditOldPassword = dynamic_cast<CEditClearUI*>(m_PaintManager.FindControl(_T("oldpassword")));
		CEditClearUI* pEditNewPassword = dynamic_cast<CEditClearUI*>(m_PaintManager.FindControl(_T("newpassword")));
		CEditClearUI* pEditNewConfirmPassword = dynamic_cast<CEditClearUI*>(m_PaintManager.FindControl(_T("newconfirmpassword")));

		if (pEditName && pEditName->IsFocused())
		{
			pEditName->GetManager()->SendNotify(pEditName, DUI_MSGTYPE_TABSWITCH);
			return 0;
		}
		if (pEditPassword && pEditPassword->IsFocused())
		{
			pEditPassword->GetManager()->SendNotify(pEditPassword, DUI_MSGTYPE_TABSWITCH);
			return 0;
		}
		if (pEditRegisterName && pEditRegisterName->IsFocused())
		{
			pEditRegisterName->GetManager()->SendNotify(pEditRegisterName, DUI_MSGTYPE_TABSWITCH);
			return 0;
		}
		if (pEditRegisterPassword && pEditRegisterPassword->IsFocused())
		{
			pEditRegisterPassword->GetManager()->SendNotify(pEditRegisterPassword, DUI_MSGTYPE_TABSWITCH);
			return 0;
		}
		if (pEditRegisterConfirmPassword && pEditRegisterConfirmPassword->IsFocused())
		{
			pEditRegisterConfirmPassword->GetManager()->SendNotify(pEditRegisterConfirmPassword, DUI_MSGTYPE_TABSWITCH);
			return 0;
		}
		if (pEditCheckcode && pEditCheckcode->IsFocused())
		{
			pEditCheckcode->GetManager()->SendNotify(pEditCheckcode, DUI_MSGTYPE_TABSWITCH);
			return 0;
		}
		if (pEditGetbackPwdAccount && pEditGetbackPwdAccount->IsFocused())
		{
			pEditGetbackPwdCheckcode->SetFocus();
			return 0;
		}
		if (pEditGetbackPwdCheckcode && pEditGetbackPwdCheckcode->IsFocused())
		{
			pEditGetbackPwdAccount->SetFocus();
			return 0;
		}
		if (pEditGetbackPwdstep2Checkcode && pEditGetbackPwdstep2Checkcode->IsFocused())
		{
			pEditGetbackPwdstep2Checkcode->GetManager()->SendNotify(pEditGetbackPwdstep2Checkcode, DUI_MSGTYPE_TABSWITCH);
			return 0;
		}
		if (pEditGetbackPwdPassword && pEditGetbackPwdPassword->IsFocused())
		{
			pEditGetbackPwdConfirmPassword->SetFocus();
			return 0;
		}
		if (pEditGetbackPwdConfirmPassword && pEditGetbackPwdConfirmPassword->IsFocused())
		{
			pEditGetbackPwdPassword->SetFocus();
			return 0;
		}
		if (pEditOldPassword && pEditOldPassword->IsFocused())
		{
			pEditOldPassword->GetManager()->SendNotify(pEditOldPassword, DUI_MSGTYPE_TABSWITCH);
			return 0;
		}
		if (pEditNewPassword && pEditNewPassword->IsFocused())
		{
			pEditNewPassword->GetManager()->SendNotify(pEditNewPassword, DUI_MSGTYPE_TABSWITCH);
			return 0;
		}
		if (pEditNewConfirmPassword && pEditNewConfirmPassword->IsFocused())
		{
			pEditNewConfirmPassword->GetManager()->SendNotify(pEditNewConfirmPassword, DUI_MSGTYPE_TABSWITCH);
			return 0;
		}

	}

	return __super::OnKeyDown(uMsg, wParam, lParam, bHandled);

}

int CGroupExplorerUI::GetTypeCount()
{
	int count = 0;
	if(m_lbCount)
	{
		string strCount = m_lbCount->GetText();
		count = atoi(strCount.c_str());
	}
	return count;
}

void CGroupExplorerUI::SetTypeCount( int count )
{
	if(m_lbCount)
	{
		if(count>=0)
		{
			SetCountText(count);
		}
		else
		{
			SetCountText(0);
		}
	}
}

void CGroupExplorerUI::OnBtnSeeMoreVR( TNotifyUI& msg )
{
	CSeeMoreVRDialogUI cSeeMoreVRDialogUI;
	cSeeMoreVRDialogUI.ShowDialog();
}
