#include "stdafx.h"
#include "ListBarDlg.h"
#include "NDCloud/NDCloudAPI.h"
#include "EventCenter/EventDefine.h"
#include "DUI/GuideDialog.h"
#include "UpdateDialog.h"
#include "ShadeWindow.h"
#include "NDCloud/NDCloudUser.h"
#include "Config.h"
#include "DUI/LaserPointerDlg.h"
#include "DUI/PackPPTDialog.h"
#include "Util/FileTypeFilter.h"
#include "MenuUI.h"

CListBarDlg* CListBarDlg::m_pInstance = NULL;
CListBarDlg* CListBarDlg::GetInstance(HWND hParent)
{
	if (!m_pInstance)
	{
		m_pInstance = new CListBarDlg(hParent);
	}
	return m_pInstance;
}

CListBarDlg::CListBarDlg()
{
	m_hParent				= NULL;
	Init();	
}

CListBarDlg::CListBarDlg(HWND hParent)
{
	m_hParent				= hParent;
	CToast::GetInstance(m_hParent);
	Init();
}

CListBarDlg::~CListBarDlg()
{
	CancelEvent(EVT_SET_CHAPTER_GUID,		MakeEventDelegate(this, &CListBarDlg::OnEventChapterChanged));
	CancelEvent(EVT_MENU_CLEARUP,			MakeEventDelegate(this, &CListBarDlg::OnEventDelete));
	CancelEvent(EVT_MENU_IMPORT,			MakeEventDelegate(this, &CListBarDlg::OnEventImport));
	CancelEvent(EVT_LOCALRES_ADD_FILE,		MakeEventDelegate(this, &CListBarDlg::OnEventLocalResAddFile));
	CancelEvent(EVT_LOCALRES_UPDATE_FILE,	MakeEventDelegate(this, &CListBarDlg::OnEventLocalResUpdateFile));
	CancelEvent(EVT_REFRESH_GROUPEXPLORER,	MakeEventDelegate(this, &CListBarDlg::OnEventRefreshGroupexplorer));
	CancelEvent(EVT_LOGIN,					MakeEventDelegate(this, &CListBarDlg::OnEventLoginComplete));
	CancelEvent(EVT_LOGOUT,					MakeEventDelegate(this, &CListBarDlg::OnEventLogoutComplete));
}

void CListBarDlg::Init()
{
	m_pGroupExplorer		= NULL;
	m_pNewFolder			= NULL;

	m_pCloudList			= NULL;
	m_pLocalList			= NULL;
	m_pDbankList			= NULL;
// 
// 	m_pLocalSearch			= NULL;
// 	m_pCloudSearch			= NULL;
// 	m_pDBankSearch			= NULL;
	m_pLocalImport			= NULL;
	m_pDBankUpload			= NULL;

	m_pMenuUI				= NULL;

	m_pSelectChapterLayout	= NULL;

	m_strChapter			= _T("");
	m_strChapterGUID		= _T("");
	m_bFirstChanged			= false;

	m_nCurrentResType		= ND_CLOUD_RES;

	m_strLocalDefFolderName[0] = LOCAL_FOLDER_NAME_IMPORT;
	m_strLocalDefFolderName[1] = LOCAL_FOLDER_NAME_COURSE;
	m_strLocalDefFolderName[2] = LOCAL_FOLDER_NAME_VIDEO;
	m_strLocalDefFolderName[3] = LOCAL_FOLDER_NAME_PIC;
	m_strLocalDefFolderName[4] = LOCAL_FOLDER_NAME_FALSH;
	m_strLocalDefFolderName[5] = LOCAL_FOLDER_NAME_VOLUME;
	m_strLocalDefFolderName[6] = LOCAL_FOLDER_BASIC_EXERCISES;
	m_strLocalDefFolderName[7] = LOCAL_FOLDER_INTERACTIVE_EXERCISES;
	m_strLocalDefFolderName[8] = LOCAL_FOLDER_NAME_ANOTHER;

	OnEvent(EVT_MENU_CLEARUP,			MakeEventDelegate(this, &CListBarDlg::OnEventDelete));
	OnEvent(EVT_MENU_IMPORT,			MakeEventDelegate(this, &CListBarDlg::OnEventImport));
	OnEvent(EVT_LOCALRES_ADD_FILE,		MakeEventDelegate(this, &CListBarDlg::OnEventLocalResAddFile));
	OnEvent(EVT_LOCALRES_UPDATE_FILE,	MakeEventDelegate(this, &CListBarDlg::OnEventLocalResUpdateFile));
	OnEvent(EVT_REFRESH_GROUPEXPLORER,	MakeEventDelegate(this, &CListBarDlg::OnEventRefreshGroupexplorer));
	OnEvent(EVT_LOGIN,					MakeEventDelegate(this, &CListBarDlg::OnEventLoginComplete));
	OnEvent(EVT_LOGOUT,					MakeEventDelegate(this, &CListBarDlg::OnEventLogoutComplete));
	OnEvent(EVT_MENU_UPLOAD,			MakeEventDelegate(this, &CListBarDlg::OnEventUpload));
}


CControlUI* CListBarDlg::CreateControl(LPCTSTR pstrClass)
{
	if( _tcscmp(pstrClass, _T("PPTShellList")) == 0 )	
		return new CPPTShellListUI;
	else if( _tcscmp(pstrClass, _T("ListButton")) == 0 ) 
		return new CListButtonUI;
	else if (_tcscmp(pstrClass, _T("ListIconItem")) == 0  )
		return new CListItemUI;
	else if (_tcscmp(pstrClass, _T("ListBarCloudExpandButton")) == 0)
	{
		CListBarCloudExpandButton * pButton = new CListBarCloudExpandButton;
		pButton->SetDoClickCallBack(MakeDelegate(this, &CListBarDlg::OnBtnCloudExpandEvent));
		return pButton;
	}
	else if (_tcscmp(pstrClass, _T("ListBarDBankExpandButton")) == 0  )
	{
		CListBarDBankExpandButton * pButton = new CListBarDBankExpandButton;
		pButton->SetDoClickCallBack(MakeDelegate(this, &CListBarDlg::OnBtnDBankExpandEvent));
		return pButton;
	}
	else if (_tcscmp(pstrClass, _T("ListBarLocalExpandButton")) == 0  )
	{
		CListBarLocalExpandButton * pButton = new CListBarLocalExpandButton;
		pButton->SetDoClickCallBack(MakeDelegate(this, &CListBarDlg::OnBtnLocalExpandEvent));
		return pButton;
	}
	else if (_tcscmp(pstrClass, _T("ListBarSearchButton")) == 0  )
	{
		CListBarSearchButton * pButton = new CListBarSearchButton;
		pButton->SetDoClickCallBack(MakeDelegate(this, &CListBarDlg::OnBtnSearchEvent));
		return pButton;
	}
	
	else if (_tcscmp(pstrClass, _T("ListBarLoginLayout")) == 0)
	{
		CListBarLoginLayout * pItem = new CListBarLoginLayout;
		pItem->SetDoClickCallBack(MakeDelegate(this, &CListBarDlg::OnBtnLogin));
		pItem->OnSetRegisterCallBack(MakeDelegate(this, &CListBarDlg::OnBtnRegister));
		return pItem;
	}
	else if (_tcscmp(pstrClass, _T("ListBarLocalSearchItem")) == 0)
	{
		CListBarLocalSearchItem * pItem = new CListBarLocalSearchItem;
		pItem->SetDoClickCallBack(MakeDelegate(this, &CListBarDlg::OnBtnShowLocalSearch));
		pItem->SetDoRClickCallBack(MakeDelegate(this, &CListBarDlg::OnLocalItemRClick));
		return pItem;
	}
	else if (_tcscmp(pstrClass, _T("ListBarLocalImportItem")) == 0)
	{
		CListBarLocalImportItem * pItem = new CListBarLocalImportItem;
		pItem->SetDoClickCallBack(MakeDelegate(this, &CListBarDlg::OnBtnImport));
		return pItem;
	}
	else if (_tcscmp(pstrClass, _T("ListBarSelectChapterLayout")) == 0)
	{
		CListBarSelectChapterLayout * pItem = new CListBarSelectChapterLayout;
		pItem->SetDoClickCallBack(MakeDelegate(this, &CListBarDlg::OnBtnSelectChapter));
		return pItem;
	}
	else if (_tcscmp(pstrClass, _T("ListBarCloudSearchItem")) == 0  )
	{
		CListBarCloudSearchItem* pItem = new CListBarCloudSearchItem;
		pItem->SetDoClickCallBack(MakeDelegate(this, &CListBarDlg::OnBtnShowCloudSearch));
		pItem->SetDoRClickCallBack(MakeDelegate(this, &CListBarDlg::OnCloudItemRClick));
		return pItem;
	}
	else if (_tcscmp(pstrClass, _T("ListBarDBankSearchItem")) == 0  )
	{
		CListBarDBankSearchItem* pItem = new CListBarDBankSearchItem;
		pItem->SetDoClickCallBack(MakeDelegate(this, &CListBarDlg::OnBtnShowDBankSearch));
		pItem->SetDoRClickCallBack(MakeDelegate(this, &CListBarDlg::OnDBankItemRClick));
		return pItem;
	}
	else if (_tcscmp(pstrClass, _T("ListBarDBankUploadItem")) == 0  )
	{
		CListBarDBankUploadItem* pItem = new CListBarDBankUploadItem;
		pItem->SetDoClickCallBack(MakeDelegate(this, &CListBarDlg::OnBtnUpload));
		return pItem;
	}
	else if (_tcscmp(pstrClass, _T("ListCloud3DResourceItem")) == 0 )
	{
		CListCloud3DResourceItem* pItem = new  CListCloud3DResourceItem;
		pItem->SetDoClickCallBack(MakeDelegate(this, &CListBarDlg::OnCloudItemEvent));
		pItem->SetDoRClickCallBack(MakeDelegate(this, &CListBarDlg::OnCloudItemRClick));
#ifndef DEVELOP_VERSION
		pItem->SetVisible(false);
#endif
		return pItem;
	}
	else if (_tcscmp(pstrClass, _T("ListCloudVRResourceItem")) == 0 )
	{
		CListCloudVRResourceItem* pItem = new  CListCloudVRResourceItem;
		pItem->SetDoClickCallBack(MakeDelegate(this, &CListBarDlg::OnCloudItemEvent));
		pItem->SetDoRClickCallBack(MakeDelegate(this, &CListBarDlg::OnCloudItemRClick));
		return pItem;
	}
	else  if (_tcscmp(pstrClass, _T("ListCloudCourseItem")) == 0 )
	{
		CListCloudCourseItem* pItem = new  CListCloudCourseItem;
		pItem->SetDoClickCallBack(MakeDelegate(this, &CListBarDlg::OnCloudItemEvent));
		pItem->SetDoRClickCallBack(MakeDelegate(this, &CListBarDlg::OnCloudItemRClick));
		return pItem;
	}
	else if (_tcscmp(pstrClass, _T("ListCloudCoursewareObjectsItem")) == 0 )
	{
		CListCloudCoursewareObjectsItem* pItem = new  CListCloudCoursewareObjectsItem;
		pItem->SetDoClickCallBack(MakeDelegate(this, &CListBarDlg::OnCloudItemEvent));
		pItem->SetDoRClickCallBack(MakeDelegate(this, &CListBarDlg::OnCloudItemRClick));
		return pItem;
	}
	else if (_tcscmp(pstrClass, _T("ListCloudAssetsItem")) == 0 )
	{
		CListCloudAssetsItem* pItem = new  CListCloudAssetsItem;
		pItem->SetDoClickCallBack(MakeDelegate(this, &CListBarDlg::OnCloudItemEvent));
		pItem->SetDoRClickCallBack(MakeDelegate(this, &CListBarDlg::OnCloudItemRClick));
		return pItem;
	}
	else if (_tcscmp(pstrClass, _T("ListCloudPPTTemplateItem")) == 0 )
	{
		CListCloudPPTTemplateItem* pItem = new  CListCloudPPTTemplateItem;
		pItem->SetDoClickCallBack(MakeDelegate(this, &CListBarDlg::OnCloudItemEvent));
		pItem->SetDoRClickCallBack(MakeDelegate(this, &CListBarDlg::OnCloudItemRClick));
		return pItem;
	}
	else if (_tcscmp(pstrClass, _T("ListCloudQuestionItem")) == 0 )
	{
		CListCloudQuestionItem* pItem = new  CListCloudQuestionItem;
		pItem->SetDoClickCallBack(MakeDelegate(this, &CListBarDlg::OnCloudItemEvent));
		pItem->SetDoRClickCallBack(MakeDelegate(this, &CListBarDlg::OnCloudItemRClick));
		return pItem;
	}
	else if (_tcscmp(pstrClass, _T("ListDBankCourseItem")) == 0 )
	{
		CListDBankCourseItem* pItem = new  CListDBankCourseItem;
		pItem->SetDoClickCallBack(MakeDelegate(this, &CListBarDlg::OnDBankItemEvent));
		pItem->SetDoRClickCallBack(MakeDelegate(this, &CListBarDlg::OnDBankItemRClick));
		return pItem;
	}
	else if (_tcscmp(pstrClass, _T("ListDBankCoursewareObjectsItem")) == 0 )
	{
		CListDBankCoursewareObjectsItem* pItem = new  CListDBankCoursewareObjectsItem;
		pItem->SetDoClickCallBack(MakeDelegate(this, &CListBarDlg::OnDBankItemEvent));
		pItem->SetDoRClickCallBack(MakeDelegate(this, &CListBarDlg::OnDBankItemRClick));
		return pItem;
	}
	else if (_tcscmp(pstrClass, _T("ListDBankAssetsItem")) == 0 )
	{
		CListDBankAssetsItem* pItem = new  CListDBankAssetsItem;
		pItem->SetDoClickCallBack(MakeDelegate(this, &CListBarDlg::OnDBankItemEvent));
		pItem->SetDoRClickCallBack(MakeDelegate(this, &CListBarDlg::OnDBankItemRClick));
		return pItem;
	}
	else if (_tcscmp(pstrClass, _T("ListDBankQuestionItem")) == 0 )
	{
		CListDBankQuestionItem* pItem = new  CListDBankQuestionItem;
		pItem->SetDoClickCallBack(MakeDelegate(this, &CListBarDlg::OnDBankItemEvent));
		pItem->SetDoRClickCallBack(MakeDelegate(this, &CListBarDlg::OnDBankItemRClick));
		return pItem;
	}
	else if (_tcscmp(pstrClass, _T("ListLocalCourseItem")) == 0)
	{
		CListLocalCourseItem * pItem = new CListLocalCourseItem;
		pItem->SetDoClickCallBack(MakeDelegate(this, &CListBarDlg::OnLocalItemEvent));
		pItem->SetDoRClickCallBack(MakeDelegate(this, &CListBarDlg::OnLocalItemRClick));
		return pItem;
	}
	else if (_tcscmp(pstrClass, _T("ListLocalCoursewareObjectsItem")) == 0)
	{
		CListLocalCoursewareObjectsItem * pItem = new CListLocalCoursewareObjectsItem;
		pItem->SetDoClickCallBack(MakeDelegate(this, &CListBarDlg::OnLocalItemEvent));
		pItem->SetDoRClickCallBack(MakeDelegate(this, &CListBarDlg::OnLocalItemRClick));
		return pItem;
	}
	else if (_tcscmp(pstrClass, _T("ListLocalFlashItem")) == 0)
	{
		CListLocalFlashItem * pItem = new CListLocalFlashItem;
		pItem->SetDoClickCallBack(MakeDelegate(this, &CListBarDlg::OnLocalItemEvent));
		pItem->SetDoRClickCallBack(MakeDelegate(this, &CListBarDlg::OnLocalItemRClick));
		return pItem;
	}
	else if (_tcscmp(pstrClass, _T("ListLocalImageItem")) == 0)
	{
		CListLocalImageItem * pItem = new CListLocalImageItem;
		pItem->SetDoClickCallBack(MakeDelegate(this, &CListBarDlg::OnLocalItemEvent));
		pItem->SetDoRClickCallBack(MakeDelegate(this, &CListBarDlg::OnLocalItemRClick));
		return pItem;
	}
	else if (_tcscmp(pstrClass, _T("ListLocalQuestionItem")) == 0)
	{
		CListLocalQuestionItem * pItem = new CListLocalQuestionItem;
		pItem->SetDoClickCallBack(MakeDelegate(this, &CListBarDlg::OnLocalItemEvent));
		pItem->SetDoRClickCallBack(MakeDelegate(this, &CListBarDlg::OnLocalItemRClick));
		return pItem;
	}
	else if (_tcscmp(pstrClass, _T("ListLocalVideoItem")) == 0)
	{
		CListLocalVideoItem * pItem = new CListLocalVideoItem;
		pItem->SetDoClickCallBack(MakeDelegate(this, &CListBarDlg::OnLocalItemEvent));
		pItem->SetDoRClickCallBack(MakeDelegate(this, &CListBarDlg::OnLocalItemRClick));
		return pItem;
	}
	else if (_tcscmp(pstrClass, _T("ListLocalVolumeItem")) == 0)
	{
		CListLocalVolumeItem * pItem = new CListLocalVolumeItem;
		pItem->SetDoClickCallBack(MakeDelegate(this, &CListBarDlg::OnLocalItemEvent));
		pItem->SetDoRClickCallBack(MakeDelegate(this, &CListBarDlg::OnLocalItemRClick));
		return pItem;
	}
	return NULL;
}

void CListBarDlg::InitWindow()
{
	OnEvent(EVT_SET_CHAPTER_GUID,		MakeEventDelegate(this, &CListBarDlg::OnEventChapterChanged));

	m_pCloudList		= static_cast<CPPTShellListUI*>(m_PaintManager.FindControl(_T("ndcloudreslist")));
	m_pLocalList		= static_cast<CPPTShellListUI*>(m_PaintManager.FindControl(_T("ndlocalreslist")));
	m_pDbankList		= static_cast<CPPTShellListUI*>(m_PaintManager.FindControl(_T("nddbankreslist")));
// 
// 	m_pLocalSearch		= static_cast<CPPTShellListUI*>(m_PaintManager.FindControl(_T("ndlocalsearch")));
// 	m_pCloudSearch		= static_cast<CPPTShellListUI*>(m_PaintManager.FindControl(_T("ndcloudressearch")));
// 	m_pDBankSearch		= static_cast<CPPTShellListUI*>(m_PaintManager.FindControl(_T("nddbankressearch")));
	m_pLocalImport		= static_cast<CPPTShellListUI*>(m_PaintManager.FindControl(_T("ndlocalresimport")));
	m_pDBankUpload		= static_cast<CPPTShellListUI*>(m_PaintManager.FindControl(_T("nddbankresupload")));

	m_pSelectChapterLayout	= dynamic_cast<CListBarSelectChapterLayout*>(m_PaintManager.FindControl(_T("ndclondselectchapter")));
	m_pNdbankLoginLayout	= dynamic_cast<CListBarLoginLayout*>(m_PaintManager.FindControl(_T("ndbanklogin")));

	m_pCloudButton	= dynamic_cast<CListBarCloudExpandButton*>(m_PaintManager.FindControl(_T("ndcloudres"))); 
	m_pLocalButton	= dynamic_cast<CListBarLocalExpandButton*>(m_PaintManager.FindControl(_T("ndlocalres"))); 
	m_pDbankButton	= dynamic_cast<CListBarDBankExpandButton*>(m_PaintManager.FindControl(_T("nddbankres"))); 
	m_pSearchButton = dynamic_cast<CListBarSearchButton*>(m_PaintManager.FindControl(_T("btnSearch"))); 

	m_pCloudButton->AddToVecControls(m_pSelectChapterLayout);
//	m_pCloudButton->AddToVecControls(m_pCloudSearch);
	m_pCloudButton->AddToVecControls(m_pCloudList);

	m_pLocalButton->AddToVecControls(m_pLocalImport);
	m_pLocalButton->AddToVecControls(m_pLocalList);

	m_pDbankButton->AddToVecControls(m_pNdbankLoginLayout);
	m_pDbankButton->AddToVecControls(m_pDBankUpload);
	m_pDbankButton->AddToVecControls(m_pDbankList);

	if(g_Config::GetInstance()->GetModuleVisible(MODULE_LOGIN))
	{
		m_pDbankButton->SetVisible(true);
	}

	CScrollBarUI* pVerScrollBar = m_pCloudList->GetVerticalScrollBar();
	pVerScrollBar->SetFixedWidth(0);
	pVerScrollBar = m_pLocalList->GetVerticalScrollBar();
	pVerScrollBar->SetFixedWidth(0);
	pVerScrollBar = m_pDbankList->GetVerticalScrollBar();
	pVerScrollBar->SetFixedWidth(0);
	pVerScrollBar = m_pLocalImport->GetVerticalScrollBar();
	pVerScrollBar->SetFixedWidth(0);
//	pVerScrollBar = m_pCloudSearch->GetVerticalScrollBar();
	pVerScrollBar->SetFixedWidth(0);
	pVerScrollBar = m_pDBankUpload->GetVerticalScrollBar();
	pVerScrollBar->SetFixedWidth(0);
	
	
	m_pGroupExplorer = CGroupExplorerUI::GetInstance();
 	m_pGroupExplorer->SetListBar(this);
 	m_pGroupExplorer->m_hParentHwnd = m_hParent;
 	
	CListCloudQuestionItem* pQuestionElement	= dynamic_cast<CListCloudQuestionItem*>(m_PaintManager.FindControl(_T("QuestionElement"))); 
	if( g_Config::GetInstance()->GetModuleVisible(MODULE_QUESITON))
	{
		pQuestionElement->SetVisible(true);
	}

	CListCloudCoursewareObjectsItem* pCoursewareObjectsElement = dynamic_cast<CListCloudCoursewareObjectsItem*>(m_PaintManager.FindControl(_T("CoursewareObjectsElement"))); 
	if( g_Config::GetInstance()->GetModuleVisible(MODULE_QUESITON))
	{
		pCoursewareObjectsElement->SetVisible(true);
	}

	CListCloudVRResourceItem* pVRElement = dynamic_cast<CListCloudVRResourceItem*>(m_PaintManager.FindControl(_T("VRElement"))); 
	tstring strVRVersionPath = GetLocalPath();
	strVRVersionPath += _T("\\vrversion.dat");
	if(GetFileAttributes(strVRVersionPath.c_str()) != INVALID_FILE_ATTRIBUTES)
		pVRElement->SetVisible(true);
	else
		pVRElement->SetVisible(false);

	m_pNewFolder			= NULL;

	g_Config::GetInstance()->GetLocalFileJosn(m_mapLocalFile);
	CreateLocalDefaultFolder();
	LoadFoldersInfo();

	CRect rect;
	GetWindowRect(m_hParent, &rect);
	::SetWindowPos(m_hWnd, NULL, 0, 0, 100, rect.Height() - 50, SWP_NOACTIVATE | SWP_NOZORDER);

	LaserPointerDlg::GetInstance()->Create(NULL, _T("LaserPointerDlg"), WS_POPUP, WS_EX_TOOLWINDOW, 0, 0, 0, 0);

	tstring strPath = GetLocalPath();
	strPath += _T("\\Setting\\Config.ini");
	TCHAR szGuid[128] = {0};
	GetPrivateProfileString(_T("Config"),_T("ChapterGuid"),_T(""), szGuid, _countof(szGuid) - 1, strPath.c_str());

	TCHAR szChapterName[128] = {0};
	GetPrivateProfileString(_T("Config"),_T("ChapterName"),_T(""), szChapterName, _countof(szChapterName) - 1, strPath.c_str());

	if(_tcslen(szGuid) == 0 || _tcslen(szChapterName) == 0)
	{
		m_pCloudButton->Expand(true);
	}
}

LPCTSTR CListBarDlg::GetItemText( CControlUI* pList, int iItem, int iSubItem )
{
	return _T("");
}

bool CListBarDlg::LocalResInit( CDuiString strFolder, int nCount, int nType )
{
	if ( m_pLocalList != NULL )
	{
		CListLocalItem* pItem  = static_cast<CListLocalItem*>(m_pLocalList->GetItemByFolderName((LPTSTR)strFolder.GetData()));
		if ( pItem != NULL)
		{
			pItem->SetFileCount(nCount);
		}
	}

	return false;
}

bool CListBarDlg::SetLocalResCount(CDuiString strFolder, int nCount)
{
	if ( m_pLocalList != NULL )
	{
		bool bFind = false;
		for( int i=0; i<m_pLocalList->GetCount(); i++)
		{
			CListLocalItem* pItem = static_cast<CListLocalItem*>(m_pLocalList->GetItemAt(i));
			if (_tcsicmp(pItem->GetFolderName().c_str(), strFolder) == 0 )
			{
				pItem->SetFileCount(nCount);
				return true;
			}
		}
	}
	return false;
}

bool CListBarDlg::LocalResItemAddRes( tstring strFolder, list<LOCAL_RES_INFO>& lstFilePath )
{
	map<tstring, list<LOCAL_RES_INFO>>::iterator iter = m_mapLocalFile.find(strFolder);

	if (iter != m_mapLocalFile.end())
	{
		lstFilePath.sort();
		iter->second.sort();
		//guid相同的项重置其余值，例如名称等
		for(list<LOCAL_RES_INFO>::iterator item = iter->second.begin();item!=iter->second.end();item++)
		{
			LOCAL_RES_INFO info	= *item;
			for(list<LOCAL_RES_INFO>::iterator itemNew = lstFilePath.begin();itemNew!=lstFilePath.end();itemNew++)
			{
				LOCAL_RES_INFO infoNew	= *itemNew;
				if(item->strResPath.compare(infoNew.strResPath)==0)
				{
					item->nType = infoNew.nType;
					item->strChapter = infoNew.strChapter;
					item->strResPath = infoNew.strResPath;
					item->strTitle = infoNew.strTitle;
					break;
				}
			}
		}
		//
		lstFilePath.sort();
		iter->second.sort();
		iter->second.merge(lstFilePath);
		iter->second.unique();
		
		SetLocalResCount(iter->first.c_str(), iter->second.size());
		return true;
	}

	return false;
}

bool CListBarDlg::LocalResItemAddRes(int nType, list<LOCAL_RES_INFO>& lstFilePath)
{
	map<tstring, list<LOCAL_RES_INFO>>::iterator iter = m_mapLocalFile.find(m_strLocalDefFolderName[nType]);

	if (iter != m_mapLocalFile.end())
	{
		lstFilePath.sort();
		iter->second.sort();
		//guid相同的项重置其余值，例如名称等
		for(list<LOCAL_RES_INFO>::iterator item = iter->second.begin();item!=iter->second.end();item++)
		{
			LOCAL_RES_INFO info	= *item;
			for(list<LOCAL_RES_INFO>::iterator itemNew = lstFilePath.begin();itemNew!=lstFilePath.end();itemNew++)
			{
				LOCAL_RES_INFO infoNew	= *itemNew;
				if(item->strResPath.compare(infoNew.strResPath)==0)
				{
					item->nType = infoNew.nType;
					item->strChapter = infoNew.strChapter;
					item->strResPath = infoNew.strResPath;
					item->strTitle = infoNew.strTitle;
					break;
				}
			}
		}
		//
		lstFilePath.sort();
		iter->second.sort();
		iter->second.merge(lstFilePath);
		iter->second.unique();

		SetLocalResCount(iter->first.c_str(), iter->second.size());
		return true;
	}

	return false;
}

list<LOCAL_RES_INFO>* CListBarDlg::GetLocalResFileList( tstring strFolder )
{
	map<tstring, list<LOCAL_RES_INFO>>::iterator iter = m_mapLocalFile.find(strFolder);

	if (iter != m_mapLocalFile.end())
		return &(iter->second);

	return NULL;
}

bool CListBarDlg::OnEventChapterChanged( void* pObj )
{
	//增加保存用户配置
	CHttpDownloadManager* pHttpManager = HttpDownloadManager::GetInstance();
	if(pHttpManager)
	{
		TCHAR  szPost[1024] = {0};
		CCategoryTree* pCategoryTree = NULL;
		NDCloudGetCategoryTree(pCategoryTree);
		if(pCategoryTree)
		{
			DWORD dwUserId = NDCloudUser::GetInstance()->GetUserId();
			tstring strUserName = NDCloudUser::GetInstance()->GetUserName();
			if(dwUserId != 0)
			{
				tstring strSectionCode = pCategoryTree->GetSelectedSectionCode();
				tstring strGradeCode= pCategoryTree->GetSelectedGradeCode();
				tstring strCourseCode = pCategoryTree->GetSelectedCourseCode();
				tstring strEditionCode = pCategoryTree->GetSelectedEditionCode();
				tstring strSubEditionCode = pCategoryTree->GetSelectedSubEditionCode();
				tstring strChapterGuid = NDCloudGetChapterGUID();

				
				if(!strChapterGuid.empty() && !strGradeCode.empty() && !strCourseCode.empty() && !strEditionCode.empty() && !strSubEditionCode.empty()
					&& !strChapterGuid.empty()
					)
				{
					tstring strGrade = _T("");
					CategoryNode* pNode = pCategoryTree->FindNode(strSectionCode, strGradeCode);
					if(pNode)
						strGrade = pNode->strTitle;

					if(strGrade == _T("十年级"))
					{
						strGrade = _T("高一");
					}
					else if(strGrade == _T("十一年级"))
					{
						strGrade = _T("高二");
					}
					else if(strGrade == _T("十二年级"))
					{
						strGrade = _T("高三");
					}

					_stprintf(szPost, _T("Account=%d&UserName=%s&Chapter=%s/%s/%s/%s/%s/%s/%s"), 
						dwUserId,
						strUserName.c_str(),
						strSectionCode.c_str(),
						strGradeCode.c_str(),
						strCourseCode.c_str(),
						strEditionCode.c_str(),
						strSubEditionCode.c_str(),
						strChapterGuid.c_str(),
						strGrade.c_str()
						);

					string strPost = Str2Utf8(szPost);

					pHttpManager->AddTask(_T("p.101.com"), _T("/101ppt/setChapter.php"), _T(""), _T("POST"), strPost.c_str(), 80, 
						MakeHttpDelegate(NULL), 
						MakeHttpDelegate(NULL), 
						MakeHttpDelegate(NULL), 
						FALSE);
				}

			}

		}

	}
	//
	TEventNotify* pEventNotify = (TEventNotify*)pObj;
	CStream* pStream = (CStream*)pEventNotify->wParam;
	pStream->ResetCursor();

	m_strChapterGUID = pStream->ReadString();
	m_strChapter = pStream->ReadString();
	bool bInit	 = pStream->ReadDWORD() == 0 ? false : true;

	if (!m_bFirstChanged)
	{
		m_bFirstChanged = true;

		
		if(m_pDbankButton->GetExpandStaus())
			m_pDbankButton->Expand(false);
		if(m_pLocalButton->GetExpandStaus())
			m_pLocalButton->Expand(false);
	
		m_pCloudButton->Expand(true);
		{
			if(bInit)
			{
				CListCloudItem* pItem	= static_cast<CListCloudItem* >(m_pCloudList->GetItemAt(0));
				pItem->Select(true);
				m_PaintManager.SendNotify(pItem, DUI_MSGTYPE_ITEMCLICK, 0 ,0 );
			}
		}
	}
	else
	{
		if(m_pDbankButton->GetExpandStaus())
			m_pDbankButton->Expand(false);
		if(m_pLocalButton->GetExpandStaus())
			m_pLocalButton->Expand(false);

		m_pCloudButton->Expand(true);
		{
			CListCloudItem* pItem	= static_cast<CListCloudItem* >(m_pCloudList->GetItemAt(0));
			pItem->Select(true);
			m_PaintManager.SendNotify(pItem, DUI_MSGTYPE_ITEMCLICK, 0 ,0);
		}
	}
	m_pGroupExplorer->ResetWindow();
	



	return true;
}

void CListBarDlg::LoadFoldersInfo()
{
	map<tstring, list<LOCAL_RES_INFO>>::iterator iter = m_mapLocalFile.begin();
	list<LOCAL_RES_INFO>::iterator iterFile;
	for (iter; iter != m_mapLocalFile.end(); iter++)
	{
		for (iterFile = iter->second.begin(); iterFile != iter->second.end(); )
		{
			bool canDelete = false;
			FILE* pf = _tfopen((*iterFile).strResPath.c_str(), _T("r"));
			if ( pf != NULL )
			{
				fseek(pf, 0,  SEEK_END);
				int nSize = ftell(pf);
				fseek(pf, 0, SEEK_SET);
				fclose(pf);
				if(nSize<=0)
				{
					canDelete = true;
				}
			}
			else
			{
				canDelete = true;
			}
			if(canDelete)
			{
				iterFile = iter->second.erase(iterFile);
			}
			else
			{
				iterFile++;
			}
			/*WIN32_FIND_DATA fData;
			HANDLE hFind = FindFirstFile((*iterFile).strResPath.c_str(), &fData);

			if ( hFind == INVALID_HANDLE_VALUE || (fData.nFileSizeHigh == 0 && fData.nFileSizeLow == 0) )
			{
				iterFile = iter->second.erase(iterFile);
			}
			else
			{
				iterFile++;
			}*/
		}

		//if ( !iter->second.empty() )
		LocalResInit(iter->first.c_str(), iter->second.size(), LocalFileImport);
	}
}

void CListBarDlg::CreateLocalDefaultFolder()
{
	LocalResInit(LOCAL_FOLDER_NAME_COURSE, 0, LocalFileCourse);
	LocalResInit(LOCAL_FOLDER_NAME_VIDEO, 0, LocalFileVideo);
	LocalResInit(LOCAL_FOLDER_NAME_PIC, 0, LocalFileImage);
	LocalResInit(LOCAL_FOLDER_NAME_FALSH, 0, LocalFileFlash);
	LocalResInit(LOCAL_FOLDER_NAME_VOLUME, 0, LocalFileVolume);
	LocalResInit(LOCAL_FOLDER_BASIC_EXERCISES, 0, LocalFileBasicExercises);
	LocalResInit(LOCAL_FOLDER_INTERACTIVE_EXERCISES, 0, LocalFileInteractiveExercises);
	//LocalResInit(LOCAL_FOLDER_NAME_ANOTHER, 0, LocalFileAnother);

	FileMapAddFolder(LOCAL_FOLDER_NAME_COURSE);
	FileMapAddFolder(LOCAL_FOLDER_NAME_VIDEO);
	FileMapAddFolder(LOCAL_FOLDER_NAME_PIC);
	FileMapAddFolder(LOCAL_FOLDER_NAME_FALSH);
	FileMapAddFolder(LOCAL_FOLDER_NAME_VOLUME);
	FileMapAddFolder(LOCAL_FOLDER_BASIC_EXERCISES);
	FileMapAddFolder(LOCAL_FOLDER_INTERACTIVE_EXERCISES);
	//FileMapAddFolder(LOCAL_FOLDER_NAME_ANOTHER);
}


void CListBarDlg::SaveFoldersInfo()
{
	g_Config::GetInstance()->WriteLocalFileJosn(m_mapLocalFile);
}

bool CListBarDlg::OnEventRefreshGroupexplorer( void* pObj )
{
	return true;
}

bool CListBarDlg::OnEventLoginComplete( void* pObj )
{
	if(m_pDbankButton->GetExpandStaus())
	{
		m_pCloudButton->Expand(false);
		m_pLocalButton->Expand(false);
		m_pDbankButton->Expand(true);

		CGroupExplorerUI::GetInstance()->ShowWindow(false);
		CItemExplorerUI::GetInstance()->ShowWindow(false);
	}
	
	return true;
}

void CListBarDlg::GetDBankItemCurCount( int nItemType )
{
	CListDBankItem* pItem	= NULL;
	int nCount = m_pDbankList->GetCount();
	for(int i = 0 ; i < nCount ; i++)
	{
		pItem = dynamic_cast<CListDBankItem*>(m_pDbankList->GetItemAt(i));
		if(pItem)
		{
			int nType = pItem->GetItemType();
			if(nType == nItemType)
				goto _Out;

			vector<int>* vecSupportType = pItem->GetSupportType();
			for(int j = 0 ; j < vecSupportType->size(); j++)
			{
				if((*vecSupportType)[j] == nItemType)
					goto _Out;
			}
		}
	}
	pItem = NULL;

_Out:

	if(pItem)
 	{
		if(pItem->GetItemType() == m_nCurrentResType)
			m_PaintManager.SendNotify(pItem,DUI_MSGTYPE_ITEMCLICK);
		else
			pItem->GetTotalCountInterface();
	}
}


bool CListBarDlg::OnEventLogoutComplete( void* pObj )
{
	if(m_pDbankList->GetCurSel() > 0 || m_pDbankList->IsVisible())
	{
		CGroupExplorerUI::GetInstance()->ShowWindow(false);
		CItemExplorerUI::GetInstance()->ShowWindow(false);
		m_pDbankList->SelectItem(-1);
		m_pDbankList->SetVisible(false);
		m_pDBankUpload->SelectItem(-1);
		m_pDBankUpload->SetVisible(false);
		m_pNdbankLoginLayout->SetVisible(true);
		m_pDbankButton->Expand(true);
	}
	

	return true;
}


bool CListBarDlg::OnEventImport( void* pObj )
{
	int nType = LocalFileImport;
	int nCurSel = m_pLocalList->GetCurSel();
	if (nCurSel >= 0)
	{
		CListLocalItem* pItem = dynamic_cast<CListLocalItem*>(m_pLocalList->GetItemAt(nCurSel));

		if ( pItem != NULL )
			nType = pItem->GetItemType();
	}
	TNotifyUI tNotify;
	m_pGroupExplorer->OnLocalImport(tNotify);
	return true;
}

bool CListBarDlg::OnEventUpload( void* pObj )
{
	m_pDbankList->SelectItem(-1);
	m_pDBankUpload->SelectItem(-1);

	m_pCurListBarItem = (CListBarItem *)pObj;
	m_nCurrentResType = DBankUpload;
	CItemExplorerUI::GetInstance()->ShowWindow(false);

	m_pGroupExplorer->ShowWindow(true);
	m_pGroupExplorer->ShowUploadUI();
	return true;
}

bool CListBarDlg::OnEventLocalResAddFile(void* pObj)
{
	TEventNotify* pEventNotify	= (TEventNotify*)pObj;
	CStream* pStream			= (CStream*)pEventNotify->wParam;
	int nType					= pStream->ReadDWORD();
	tstring strTitle			= pStream->ReadString();
	tstring strPath				= pStream->ReadString();

	if ( strPath.empty() )
		return false;

	list<LOCAL_RES_INFO> lstResInfo;
	LOCAL_RES_INFO resInfo;

	resInfo.nType			= nType;
	resInfo.strChapter		= m_strChapter;
	resInfo.strChapterGuid	= m_strChapterGUID;
	resInfo.strResPath		= strPath;
	resInfo.strTitle		= strTitle;

	lstResInfo.push_back(resInfo);

	tstring strFolderName = m_strLocalDefFolderName[nType - FILE_FILTER];

	bool bAdd = LocalResItemAddRes(strFolderName, lstResInfo);

	if ( bAdd )
	{
		SaveFoldersInfo();

		int nCurSel = m_pLocalList->GetCurSel();
		CListLocalItem* pItem = dynamic_cast<CListLocalItem*>(m_pLocalList->GetItemAt(nCurSel));

		if ( pItem == NULL )
			return true;

		if ( pItem->GetFolderName() == strFolderName.c_str())
			m_PaintManager.SendNotify(pItem, DUI_MSGTYPE_ITEMCLICK);
	}

	return true;
}

bool CListBarDlg::OnEventLocalResUpdateFile(void* pObj)
{
	TEventNotify* pEventNotify	= (TEventNotify*)pObj;
	CStream* pStream			= (CStream*)pEventNotify->wParam;
	int nType					= pStream->ReadDWORD();
	tstring strTitle			= pStream->ReadString();
	tstring strPath				= pStream->ReadString();

	tstring strFolderName		= m_strLocalDefFolderName[nType - FILE_FILTER];

	int nCurSel = m_pLocalList->GetCurSel();
	CListLocalItem* pItem = dynamic_cast<CListLocalItem*>(m_pLocalList->GetItemAt(nCurSel));

	if ( pItem == NULL )
		return true;

	if ( pItem->GetFolderName() == strFolderName.c_str())
		m_PaintManager.SendNotify(pItem, DUI_MSGTYPE_ITEMCLICK);

	return true;
}



bool CListBarDlg::OnEventDelete( void* pObj )
{
	int nCurSel = m_pLocalList->GetCurSel();

	if (nCurSel < 0)
		return false;

	CListLocalItem* pItem = dynamic_cast<CListLocalItem*>(m_pLocalList->GetItemAt(nCurSel));

	if ( pItem == NULL )
		return false;

	tstring strFolderName = pItem->GetFolderName();

	tstring strTip = _T("确定要清空【") + strFolderName + _T("】文件夹下的资源？");

	int nRet = UIMessageBox(m_hParent, strTip.c_str(), _T("101教育PPT"), _T("是,否"), CMessageBoxUI::enMessageBoxTypeQuestion, IDCANCEL);
	if (nRet != ID_MSGBOX_BTN)
		return false;

	map<tstring, list<LOCAL_RES_INFO>>::iterator iter = m_mapLocalFile.find(strFolderName);

	if ( pItem->GetItemType() == LocalFileImport )  // zcs 1025-11-26
	{
		if (iter != m_mapLocalFile.end())
			m_mapLocalFile.erase(iter);

		m_pLocalList->Remove(pItem);
		m_pLocalList->SelectItem(-1);
	}
	else
	{
		if (iter != m_mapLocalFile.end())
			iter->second.clear();

		pItem->SetFileCount(0);
	}

	m_pGroupExplorer->ShowWindow(false);

	SaveFoldersInfo();

	return true;
}

bool CListBarDlg::LocalResItemDelFile( tstring strFolderName, tstring strFilePath, int& nCount )
{
	map<tstring, list<LOCAL_RES_INFO>>::iterator iter = m_mapLocalFile.find(strFolderName.c_str());

	if ( iter != m_mapLocalFile.end() )
	{
		list<LOCAL_RES_INFO> lstResInfo = (iter->second);
		list<LOCAL_RES_INFO>::iterator iterRes = lstResInfo.begin();
		for ( iterRes; iterRes != lstResInfo.end(); iterRes++ )
		{
			LOCAL_RES_INFO resInfo = *iterRes;
			if (resInfo.strResPath.compare(strFilePath) != 0 )
				continue;
			
			iterRes = lstResInfo.erase(iterRes);
			m_mapLocalFile[strFolderName.c_str()] = lstResInfo;

			SaveFoldersInfo();

			CListLocalItem* pItem = dynamic_cast<CListLocalItem*>(m_pLocalList->GetItemByFolderName(strFolderName.c_str()));
			if ( pItem != NULL )
			{
				nCount = iter->second.size();
				pItem->SetFileCount(nCount);
			}
			return true;
		}
		//iter->second.remove(strFilePath);

		
	}

	return false;
}

bool CListBarDlg::LocalResItemRenameFile( tstring strFolderName, tstring strFilePath ,tstring strNewName,bool isModifyTitleOnly)
{
	bool result = false;
	map<tstring, list<LOCAL_RES_INFO>>::iterator iter = m_mapLocalFile.find(strFolderName.c_str());
	if ( iter != m_mapLocalFile.end() )
	{
		list<LOCAL_RES_INFO>::iterator iterFile	= iter->second.begin();
		for ( iterFile; iterFile != iter->second.end(); iterFile++ )
		{
			LOCAL_RES_INFO sResInfo = *iterFile;
			if ( _tcsicmp(sResInfo.strResPath.c_str(), strFilePath.c_str()) == 0 )
			{
				if(isModifyTitleOnly)
				{
					(*iterFile).strTitle = strNewName;
					SaveFoldersInfo();
					result = true;
				}
				else
				{
					//文件重名
					tstring strFileDicPath = sResInfo.strResPath.substr(0,sResInfo.strResPath.find_last_of(_T("\\"))+1);
					tstring strNewPath = strFileDicPath + strNewName + sResInfo.strResPath.substr(sResInfo.strResPath.find_last_of(_T(".")));
					::_trename(sResInfo.strResPath.c_str(),strNewPath.c_str());
					
					
					DWORD dwError = GetLastError();
					if(dwError==0)
					{
						Sleep(200);
						(*iterFile).strResPath = strNewPath;
						(*iterFile).strTitle = strNewName;
						SaveFoldersInfo();
						result = true;
					}
					else if (dwError == 183)
					{
						//CToast::Toast(_T("同名文件已经存在，文件重命名失败!"));
					}
					else
					{
						//CToast::Toast(_T("文件重命名失败!"));
						TCHAR  cDebug[1024] = {0};
						_stprintf(cDebug, _T("rename file error :%d\r\n"),dwError);
						tstring strDebug = cDebug;
						OutputDebugString(strDebug.c_str());
					}
				}
				break;
			}
		}
	}
	return result;
}

void CListBarDlg::FileMapAddFolder(tstring strFolderName)
{
	map<tstring, list<LOCAL_RES_INFO>>::iterator iter = m_mapLocalFile.find(strFolderName);

	if ( iter != m_mapLocalFile.end() )
		return;

	list<LOCAL_RES_INFO> lstResInfo;

	m_mapLocalFile[strFolderName] = lstResInfo;
}

//////////////////////////////////////////////////////////////////////////
//ExpandBtnEvent
bool CListBarDlg::OnBtnCloudExpandEvent( void* pObj )
{
	SetCurType(-1);
	if(IsWindowVisible( m_pGroupExplorer->GetHWND()))
		m_pGroupExplorer->ShowWindow(false);
	m_pLocalButton->Expand(false);
	m_pDbankButton->Expand(false);
	UnSelectSearchButton();
	return true;
}

bool CListBarDlg::OnBtnLocalExpandEvent( void* pObj )
{
	SetCurType(-1);
	if(IsWindowVisible( m_pGroupExplorer->GetHWND()))
		m_pGroupExplorer->ShowWindow(false);
	m_pCloudButton->Expand(false);
	m_pDbankButton->Expand(false);
	UnSelectSearchButton();
	return true;
}

bool CListBarDlg::OnBtnDBankExpandEvent( void* pObj )
{
	SetCurType(-1);
	if(IsWindowVisible( m_pGroupExplorer->GetHWND()))
		m_pGroupExplorer->ShowWindow(false);
	m_pCloudButton->Expand(false);
	m_pLocalButton->Expand(false);
	UnSelectSearchButton();
	return true;
}

bool CListBarDlg::OnBtnSearchEvent( void* pObj )
{
	TNotifyUI* pNotify = (TNotifyUI*)pObj;
	CListBarSearchButton* pListBarSearchButton = dynamic_cast<CListBarSearchButton *>(pNotify->pSender);
	SetCurType(pListBarSearchButton->GetItemType());
	if(m_pCloudButton->GetExpandStaus())
	{
		m_pCloudList->SelectItem(-1);
	}
	else if(m_pLocalButton->GetExpandStaus())
	{
		m_pLocalList->SelectItem(-1);
	}
	else if(m_pDbankButton->GetExpandStaus())
	{
		m_pDbankList->SelectItem(-1);
	}

	m_pGroupExplorer->ShowWindow(true);
	m_pGroupExplorer->ShowSearchUI(MakeDelegate(pListBarSearchButton, &CListBarSearchButton::OnBtnCloseResponse));
	return true;
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//BtnEvent
bool CListBarDlg::OnBtnUpload( void* pObj )
{
	SetCurItemAndType(pObj);

	m_pDBankUpload->SelectItem(-1);
	m_pDbankList->SelectItem(-1);
	CItemExplorerUI::GetInstance()->ShowWindow(false);

	m_pGroupExplorer->ShowWindow(true);
	m_pGroupExplorer->ShowUploadUI();

	UnSelectSearchButton();
	return true;
}

bool CListBarDlg::OnBtnImport( void* pObj )
{
	SetCurItemAndType(pObj);
	m_pLocalImport->SelectItem(-1);
	m_pLocalList->SelectItem(-1);

	CStream Steam(1024);
	Steam.WriteDWORD(0);
	Steam.WriteString(LOCAL_FOLDER_NAME_IMPORT);
	Steam.ResetCursor();

	CStream streamParams(1024);
	streamParams.WriteString(m_strChapterGUID);
	streamParams.WriteString(m_strChapter);
	streamParams.ResetCursor();

	m_pGroupExplorer->ShowResource(m_nCurrentResType, &Steam, &streamParams);

	TNotifyUI tNotify;
	m_pGroupExplorer->OnLocalImport(tNotify);

	UnSelectSearchButton();

	return true;
}

bool CListBarDlg::OnBtnSelectChapter( void* pObj )
{
	SetCurType(-1);
	::PostMessage(m_hParent, WM_COMMAND, IDC_TITLEBUTTON_LESSON, 0);
	UnSelectSearchButton();
	return true;
}

bool CListBarDlg::OnBtnShowLocalSearch( void* pObj )
{
	SetCurItemAndType(pObj);
	m_pLocalList->SelectItem(-1);
	m_pGroupExplorer->SetTitleText(_T(""));
	m_pGroupExplorer->SetCountText(_T(""));
	m_pGroupExplorer->ShowResource(m_nCurrentResType, NULL);
	return true;
}

bool CListBarDlg::OnBtnShowDBankSearch( void* pObj )
{
	SetCurItemAndType(pObj);
	m_pDbankList->SelectItem(-1);
	m_pGroupExplorer->SetTitleText(_T(""));
	m_pGroupExplorer->SetCountText(_T(""));
	m_pGroupExplorer->ShowResource(m_nCurrentResType, NULL);
	return true;
}

bool CListBarDlg::OnBtnShowCloudSearch( void* pObj )
{
	SetCurItemAndType(pObj);
	m_pCloudList->SelectItem(-1);
	m_pGroupExplorer->SetTitleText(_T(""));
	m_pGroupExplorer->SetCountText(_T(""));
	m_pGroupExplorer->ShowResource(m_nCurrentResType, NULL);
	return true;
}

bool CListBarDlg::OnBtnLogin( void* pObj)
{
	TNotifyUI* pNotify = (TNotifyUI*)pObj;
	CListBarLayout* pListBarLayout = dynamic_cast<CListBarLayout *>(pNotify->pSender);
	SetCurType(pListBarLayout->GetItemType());
	m_pGroupExplorer->ShowWindow(true);
	m_pGroupExplorer->ShowLoginUI();
	UnSelectSearchButton();
	return true;
}

bool CListBarDlg::OnBtnRegister( void* pObj)
{
	TNotifyUI* pNotify = (TNotifyUI*)pObj;
	CListBarLayout* pListBarLayout = dynamic_cast<CListBarLayout *>(pNotify->pSender);
	SetCurType(pListBarLayout->GetItemType());
	m_pGroupExplorer->ShowWindow(true);
	m_pGroupExplorer->ShowRegisterUI();
	UnSelectSearchButton();
	return true;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//ItemRClick
bool CListBarDlg::OnLocalItemRClick( void* pObj )
{
	SetCurItemAndType(pObj);

	if(m_nCurrentResType == LocalFileCourse||
		m_nCurrentResType == LocalFileVideo||
		m_nCurrentResType == LocalFileImage||
		m_nCurrentResType == LocalFileFlash||
		m_nCurrentResType == LocalFileVolume)	
	{
		if(m_pMenuUI==NULL||m_pMenuUI->GetHWND()==NULL)
		{
			m_pMenuUI = new CMenuUI;
			m_pMenuUI->Create(AfxGetMainWnd()->GetSafeHwnd(), _T("MenuUI"), WS_VISIBLE, 0, 0, 0, 0);
		}
		m_pMenuUI->SetType(m_pCurListBarItem->GetItemType());
		m_pMenuUI->HideAllMenuItem();
		m_pMenuUI->ShowMenuImport();
		m_pMenuUI->ShowMenuClearup();
		m_pMenuUI->ShowWindow(true);
	}
	UnSelectSearchButton();

	return true;
}

bool CListBarDlg::OnDBankItemRClick( void* pObj )
{
	SetCurItemAndType(pObj);
	UnSelectSearchButton();
	return true;
}

bool CListBarDlg::OnCloudItemRClick( void* pObj )
{
	SetCurItemAndType(pObj);
	UnSelectSearchButton();
	return true;
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//ItemEvent
bool CListBarDlg::OnLocalItemEvent( void* pObj )
{
	SetCurItemAndType(pObj);
	m_pLocalImport->SelectItem(-1);
	UnSelectSearchButton();
	return true;
}

bool CListBarDlg::OnDBankItemEvent( void* pObj )
{
	SetCurItemAndType(pObj);
	m_pDBankUpload->SelectItem(-1);
	UnSelectSearchButton();
	return true;
}

bool CListBarDlg::OnCloudItemEvent( void* pObj )
{
	SetCurItemAndType(pObj);
	UnSelectSearchButton();
	return true;
}
//////////////////////////////////////////////////////////////////////////

void CListBarDlg::ClearSelect()
{
	
}

void CListBarDlg::SetCurType( int nType )
{
	m_pCurListBarItem = NULL;
	m_nCurrentResType = nType;
}

void CListBarDlg::SetCurItemAndType( void* pObj )
{
	TNotifyUI* pNotify = (TNotifyUI*)pObj;
	m_pCurListBarItem = (CListBarItem*)pNotify->pSender;
	m_nCurrentResType = m_pCurListBarItem->GetItemType();
}

void CListBarDlg::UnSelectSearchButton()
{
	if(m_pSearchButton->GetSelected())
		m_pSearchButton->Selected(false);
}