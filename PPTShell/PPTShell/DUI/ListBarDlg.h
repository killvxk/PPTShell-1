// MFCDemoDlg.h : 头文件
//

#pragma once
#include "DUICommon.h"
#include "PPTShellList.h"
#include "ListButton.h"
#include "ListItem.h"
#include "SearchItem.h"
#include "GroupExplorer.h"
#include "NewFolder.h"
#include "Toast.h"
#include "Config.h"

#include "RightBarItems/Cloud/ListCloudCourseItem.h"
#include "RightBarItems/Cloud/ListCloud3DResourceItem.h"
#include "RightBarItems/Cloud/ListCloudCoursewareObjectsItem.h"
#include "RightBarItems/Cloud/ListCloudAssetsItem.h"
#include "RightBarItems/Cloud/ListCloudPPTTemplateItem.h"
#include "RightBarItems/Cloud/ListCloudQuestionItem.h"
#include "RightBarItems/Cloud/ListCloudVRResourceItem.h"

#include "RightBarItems/DBank/ListDBankCourseItem.h"
#include "RightBarItems/DBank/ListDBankCoursewareObjectsItem.h"
#include "RightBarItems/DBank/ListDBankAssetsItem.h"
#include "RightBarItems/DBank/ListDBankQuestionItem.h"

#include "RightBarItems/Local/ListLocalCourseItem.h"
#include "RightBarItems/Local/ListLocalCoursewareObjectsItem.h"
#include "RightBarItems/Local/ListLocalFlashItem.h"
#include "RightBarItems/Local/ListLocalImageItem.h"
#include "RightBarItems/Local/ListLocalQuestionItem.h"
#include "RightBarItems/Local/ListLocalVideoItem.h"
#include "RightBarItems/Local/ListLocalVolumeItem.h"

#include "RightBarItems/ListLocalItem.h"

#include "RightBarItems/Layout/ListBarSelectChapterLayout.h"
#include "RightBarItems/Layout/ListBarLoginLayout.h"

#include "RightBarItems/Button/ListBarLocalSearchItem.h"
#include "RightBarItems/Button/ListBarLocalImportItem.h"
#include "RightBarItems/Button/ListBarCloudSearchItem.h"
#include "RightBarItems/Button/ListBarDBankSearchItem.h"
#include "RightBarItems/Button/ListBarDBankUploadItem.h"

#include "RightBarItems/Button/ListBarCloudExpandButton.h"
#include "RightBarItems/Button/ListBarDBankExpandButton.h"
#include "RightBarItems/Button/ListBarLocalExpandButton.h"

#include "RightBarItems/Button/ListBarSearchButton.h"

#define LOCAL_FILE_ELEMENT						_T("LocalFileElement")

#define LOCAL_FOLDER_NAME_SEARCH				_T("本地搜索")
#define LOCAL_FOLDER_NAME_IMPORT				_T("导入资源")
#define LOCAL_FOLDER_NAME_COURSE				_T("本地课件")
#define LOCAL_FOLDER_NAME_VIDEO					_T("本地视频")
#define LOCAL_FOLDER_NAME_PIC					_T("本地图片")
#define LOCAL_FOLDER_NAME_FALSH					_T("本地动画")
#define LOCAL_FOLDER_NAME_VOLUME				_T("本地音频")
#define LOCAL_FOLDER_BASIC_EXERCISES			_T("基础习题")
#define LOCAL_FOLDER_INTERACTIVE_EXERCISES		_T("互动习题")
#define LOCAL_FOLDER_NAME_ANOTHER				_T("其他")

class CMenuUI;

enum ND_RES_TYPE
{
	ND_CLOUD_RES = 0,
	ND_LOCAL_RES,
	ND_DBANK_RES,
	ND_RES_MAX ,
};

class CListBarDlg:public WindowImplBase, public IListCallbackUI
{
public:
	CListBarDlg();
	CListBarDlg(HWND hParent);
	~CListBarDlg();

	static CListBarDlg* m_pInstance;
	static CListBarDlg* GetInstance(HWND hParent);
 
	LPCTSTR GetWindowClassName() const { return _T("ListBarDlg"); };
	UINT GetClassStyle() const { return CS_DBLCLKS; };
	virtual void InitWindow();

	virtual CDuiString GetSkinFile()
	{
		return _T("RightBar\\RightBar.xml");
	}
		
	virtual CDuiString GetSkinFolder()
	{
		return CDuiString(_T("skins"));
	}

	virtual CControlUI* CreateControl(LPCTSTR pstrClass);

	virtual LPCTSTR GetItemText(CControlUI* pList, int iItem, int iSubItem);

	bool	LocalResInit(CDuiString strFolder, int nCount = 0, int nType = 0);
	bool	LocalResItemAddRes(tstring strFolder, list<LOCAL_RES_INFO>& lstFilePath);
	bool	LocalResItemAddRes(int nType, list<LOCAL_RES_INFO>& lstFilePath);
	bool	SetLocalResCount(CDuiString strFolder, int nCount);
	list<LOCAL_RES_INFO>* GetLocalResFileList(tstring strFolder);
	map<tstring, list<LOCAL_RES_INFO>> GetLocalAllRes()	{	return m_mapLocalFile;	};
	bool	LocalResItemDelFile(tstring strFolderName, tstring strFilePath, int& nCount);
	bool	LocalResItemRenameFile(tstring strFolderName, tstring strFilePath,tstring strNewName,bool isModifyTitleOnly = false);

	bool	ModifyLocalFoldrName(CDuiString strOldFolder, CDuiString strNewFolder);

	void	LoadFoldersInfo();
	void	SaveFoldersInfo();

	void	GetDBankItemCurCount(int nItemType);

	void	ClearSelect();
private:
	CNewFolder*			m_pNewFolder;
	CGroupExplorerUI*	m_pGroupExplorer;

	CPPTShellListUI*	m_pCloudList;
	CPPTShellListUI*	m_pLocalList;
	CPPTShellListUI*	m_pDbankList;
// 
// 	CPPTShellListUI*	m_pLocalSearch;
// 	CPPTShellListUI*	m_pCloudSearch;
// 	CPPTShellListUI*	m_pDBankSearch;
	CPPTShellListUI*	m_pLocalImport;
	CPPTShellListUI*	m_pDBankUpload;

	CListBarLoginLayout*				m_pNdbankLoginLayout;
	CListBarSelectChapterLayout*		m_pSelectChapterLayout;

	CListBarCloudExpandButton*			m_pCloudButton;
	CListBarLocalExpandButton*			m_pLocalButton;
	CListBarDBankExpandButton*			m_pDbankButton;
	CListBarSearchButton*				m_pSearchButton;

	CDialogBuilder		m_builder;
	
	tstring				m_strChapterGUID;
	tstring				m_strChapter;

	map<tstring, list<LOCAL_RES_INFO>> m_mapLocalFile;

	tstring				m_strLocalDefFolderName[LocalFileTotal - LocalFileImport];

	HWND				m_hParent;

	CMenuUI*			m_pMenuUI;

	bool				m_bFirstChanged;

	CListBarItem*		m_pCurListBarItem;
	int					m_nCurrentResType;
protected:
	void		Init();

	bool		OnBtnRefresh(void* pObj);

	bool		OnEventChapterChanged(void* pObj);
	bool		OnEventDelete(void* pObj);
	bool		OnEventImport(void* pObj);
	bool		OnEventLocalResAddFile(void* pObj);
	bool		OnEventLocalResUpdateFile(void* pObj);
	bool		OnEventUpload(void* pObj);
	bool		OnEventRefreshGroupexplorer(void* pObj);
	bool		OnEventLoginComplete(void* pObj);
	bool		OnEventLogoutComplete(void* pObj);

	void		CreateLocalDefaultFolder();
	void		FileMapAddFolder(tstring strFolderName);

	//new
	bool		OnBtnShowCloudSearch(void* pObj);
	bool		OnBtnShowDBankSearch(void* pObj);
	bool		OnBtnShowLocalSearch(void* pObj);

	bool		OnBtnSelectChapter(void* pObj);

	bool		OnCloudItemEvent(void* pObj);
	bool		OnDBankItemEvent(void* pObj);
	bool		OnLocalItemEvent(void* pObj);

	bool		OnDBankItemRClick(void* pObj);
	bool		OnLocalItemRClick(void* pObj);
	bool		OnCloudItemRClick(void* pObj);

	bool		OnBtnImport(void* pObj);
	bool		OnBtnUpload(void* pObj);
	bool		OnBtnLogin(void* pObj);
	bool		OnBtnRegister(void* pObj);

	bool		OnBtnCloudExpandEvent(void* pObj);
	bool		OnBtnLocalExpandEvent(void* pObj);
	bool		OnBtnDBankExpandEvent(void* pObj);
	bool		OnBtnSearchEvent(void* pObj);

	void		SetCurItemAndType(void* pObj);
	void		SetCurType(int nType);

	void		UnSelectSearchButton();

};