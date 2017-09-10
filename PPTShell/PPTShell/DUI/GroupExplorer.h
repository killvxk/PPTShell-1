#pragma once
#include "DUICommon.h"
#include "ItemExplorer.h"
#include "EditClear.h"
#include "Util/Util.h"
#include "TabPage.h"
#include "EventCenter/EventDefine.h"
#include "ThirdParty/json/json.h"
#include "DUI/ItemPage.h"
#include "DUI/LoginWindowUI.h"
#include "DUI/RegisterWindowUI.h"
#include "DUI/ChangePasswordWindowUI.h"
#include "DUI/GetBackPwdWindowUI.h"
#include "DUI/AssetsWindow.h"
#include "DUI/DBankAssetsWindow.h"
#include "DUI/SearchWindowUI.h"

#define Search_Page_Count	20
#define Search_QuickShow_Count	4
#define Search_MaxPage_Count	5
#define random(x) (rand()%x)


class CMenuUI;

typedef struct LOCAL_SEARCH_RESULT
{
	tstring		strFolder;
	tstring		strPath;
	tstring		strName;
	tstring		strChapter;
	int			nFileType;

	bool operator < (LOCAL_SEARCH_RESULT & other) {
		return nFileType < other.nFileType;
	}
}LOCAL_SEARCH_RESULT, *spLOCAL_SEARCH_RESULT;

class CGroupExplorerUI: 
	public CResourceExplorerUI
{

public:
	CGroupExplorerUI();
	~CGroupExplorerUI();

public:
	static CGroupExplorerUI* m_pInstance;
	static CGroupExplorerUI* GetInstance();

	UIBEGIN_MSG_MAP
		SUPER_HANDLER_MSG
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("folderSearchBack"),	OnBtnFolderSearchBack);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("refresh"),			OnBtnRefresh);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("search_refresh"),			OnSearchRefresh);
		EVENT_ID_HANDLER(DUI_MSGTYPE_RETURN,	CLEAR_RICHEDIT_NAME,		OnSearch);
		EVENT_ID_HANDLER(DUI_MSGTYPE_TEXTCHANGED,CLEAR_RICHEDIT_NAME,		OnEditChanged);
		EVENT_ID_HANDLER(DUI_MSGTYPE_TEXTCHANGED,_T("username"),		OnEditChanged);
		EVENT_ID_HANDLER(DUI_MSGTYPE_TEXTCHANGED,_T("password"),		OnEditChanged);
		EVENT_ID_HANDLER(DUI_MSGTYPE_TEXTCHANGED,_T("registerusername"),		OnEditChanged);
		EVENT_ID_HANDLER(DUI_MSGTYPE_TEXTCHANGED,_T("registerpassword"),		OnEditChanged);
		EVENT_ID_HANDLER(DUI_MSGTYPE_TEXTCHANGED,_T("registerconfirmpassword"),		OnEditChanged);
		EVENT_ID_HANDLER(DUI_MSGTYPE_TEXTCHANGED,_T("checkcode"),		OnEditChanged);
		EVENT_ID_HANDLER(DUI_MSGTYPE_TEXTCHANGED,_T("oldpassword"),		OnEditChanged);
		EVENT_ID_HANDLER(DUI_MSGTYPE_TEXTCHANGED,_T("newpassword"),		OnEditChanged);
		EVENT_ID_HANDLER(DUI_MSGTYPE_TEXTCHANGED,_T("newconfirmpassword"),		OnEditChanged);
		EVENT_ID_HANDLER(DUI_MSGTYPE_TEXTCHANGED,_T("getbackpwdconfirmpassword"),		OnEditChanged);
		EVENT_ID_HANDLER(DUI_MSGTYPE_TEXTCHANGED,_T("getbackpwdpassword"),		OnEditChanged);
		
		EVENT_ID_HANDLER(DUI_MSGTYPE_SELECTCHANGED,_T("SavePassword"),		OnCheckBoxSavePassword);
		EVENT_ID_HANDLER(DUI_MSGTYPE_SELECTCHANGED,_T("AutomaticLogin"),		OnCheckBoxAutoLogin);

		EVENT_ID_HANDLER(DUI_MSGTYPE_TABSWITCH,_T("username"),		OnEditTabChangeLogin);
		EVENT_ID_HANDLER(DUI_MSGTYPE_TABSWITCH,_T("password"),		OnEditTabChangeLogin);
		EVENT_ID_HANDLER(DUI_MSGTYPE_TABSWITCH,_T("registerusername"),		OnEditTabChangeLogin);
		EVENT_ID_HANDLER(DUI_MSGTYPE_TABSWITCH,_T("registerpassword"),		OnEditTabChangeLogin);
		EVENT_ID_HANDLER(DUI_MSGTYPE_TABSWITCH,_T("registerconfirmpassword"),		OnEditTabChangeLogin);
		EVENT_ID_HANDLER(DUI_MSGTYPE_TABSWITCH,_T("checkcode"),		OnEditTabChangeLogin);
		EVENT_ID_HANDLER(DUI_MSGTYPE_TABSWITCH,_T("oldpassword"),		OnEditTabChangeLogin);
		EVENT_ID_HANDLER(DUI_MSGTYPE_TABSWITCH,_T("newpassword"),		OnEditTabChangeLogin);
		EVENT_ID_HANDLER(DUI_MSGTYPE_TABSWITCH,_T("newconfirmpassword"),		OnEditTabChangeLogin);
		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("close"),			OnBtnClose);

		EVENT_ID_HANDLER(DUI_MSGTYPE_RCLICK,	_T("CloudItem"),OnCloudItemRClick);
		EVENT_ID_HANDLER(DUI_MSGTYPE_RCLICK,	_T("LocalItem"),OnLocalItemRClick);
		
		EVENT_BUTTON_CLICK(CLEAR_BUTTON_NAME,				OnEditClear);
		EVENT_BUTTON_CLICK(SEARCH_BUTTON_NAME,				OnSearch);
		EVENT_ITEM_CLICK(SEARCH_TIME_NAME,					OnSearchItemSelected);

		EVENT_BUTTON_CLICK(_T("ImportBtn"),					OnLocalImport);
		EVENT_BUTTON_CLICK(_T("CancelBtn"),					OnLocalCancelImport);
		EVENT_BUTTON_CLICK(_T("LoginBtn"),					OnLogin);
		EVENT_BUTTON_CLICK(_T("refreshcheckcode"),			OnRefrshCheckCode);
		EVENT_BUTTON_CLICK(_T("refreshcheckcode_getbackpwd"),OnRefrshCheckCode);
		EVENT_BUTTON_CLICK(_T("backlogin_getbackpwd"),		OnBackLogin);
		EVENT_BUTTON_CLICK(_T("backlogin"),		OnBackLogin);
		EVENT_BUTTON_CLICK(_T("RegisterBtn"),				OnRegister);
		EVENT_BUTTON_CLICK(_T("ChangePasswordBtn"),			OnChangePassword);
		EVENT_BUTTON_CLICK(_T("sentphonecode"),				OnSentPhoneCode);
//		EVENT_BUTTON_CLICK(_T("reSendEmail"),				OnReSendEmail);
		EVENT_BUTTON_CLICK(_T("changeEmail"),				OnBackRegister);
//		EVENT_BUTTON_CLICK(_T("OpenEmailUrlBtn"),			OnOpenEmailUrlBtn);
		EVENT_BUTTON_CLICK(_T("GetBackPwdStep1Btn"),		OnGetBackPwd);
		EVENT_BUTTON_CLICK(_T("GetBackPwdStep2Btn"),		OnGetBackPwd);
		EVENT_BUTTON_CLICK(_T("GetBackPwdStep3Btn"),		OnGetBackPwd);
		
		EVENT_BUTTON_CLICK(_T("GoRegister"),				OnBackRegister);
		EVENT_BUTTON_CLICK(_T("GoGuide"),					OnGoGuide);
		EVENT_BUTTON_CLICK(_T("forgetPassword"),			OnForgetPassword);
		EVENT_BUTTON_CLICK(_T("statementBtn"),				OnShowStatement);

		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("btn_see_more"),OnBtnSeeMoreVR);
		
		
	UIEND_MSG_MAP

		LPCTSTR				GetWindowClassName() const;
	UINT				GetClassStyle() const;

	virtual void		InitWindow();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	int					GetCurrentType();
	int	GetTypeCount();//类型数量
	void SetTypeCount(int count);
	//notify
private:
	void				OnBtnSeeMoreVR(TNotifyUI& msg);
	void				OnBtnRefresh(TNotifyUI& msg);
	void				OnBtnClose(TNotifyUI& msg);
	void				OnBtnFolderSearchBack(TNotifyUI& msg);

	void				OnEditClear(TNotifyUI& msg);
	void				OnSearch(TNotifyUI& msg);
	void				OnSearchRefresh(TNotifyUI& msg);
	void				OnSearchItemSelected(TNotifyUI& msg);
	void				OnEditChanged(TNotifyUI& msg);
	void				OnCheckBoxAutoLogin(TNotifyUI& msg);
	void				OnCheckBoxSavePassword(TNotifyUI& msg);
	void OnCloudItemRClick(TNotifyUI& msg);
	void OnLocalItemRClick(TNotifyUI& msg);

	void				OnLocalCancelImport(TNotifyUI& msg);
	bool				OnLocalSearchSwitchStatus( void* pObj );

	bool				GetImportWindowControl();

	void				Importing(list<LOCAL_RES_INFO> lstFileName);

	void				GetFileType(CDuiString strType);

	void				LocalSearching();
	bool				OnLocalSearchComplete(int nShowType);
//	void				OnSeachFailHandle();
	void				CalcText( HDC hdc, RECT& rc, LPCTSTR lpszText, int nFontId, UINT format, UITYPE_FONT nFontType, int c = -1);
	void				CreatSeachFailStyle(CContainerUI* pParent, int nSeq, LPCTSTR text1, LPCTSTR text2, LPCTSTR text3, LPCTSTR text4);
	int					GenernateRandom(CContainerUI* pParent, CLabelUI* pLabel, int nPos);
	bool				OnBtnCommandKey(void* pNotify);

	list<LOCAL_RES_INFO>*	LocalResSortByChapter(list<LOCAL_RES_INFO>* lstResInfo, tstring strChpater);

	static DWORD WINAPI	ImportThread(LPVOID lpVoid);
	static DWORD WINAPI LocalSearchThread(LPVOID lpVoid);

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	bool				OnLocalShowFolder(void* pNotify);
//	virtual LRESULT		OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

	void				ImportLocalFiles(const vector<tstring>& vecFiles);


	//--登录
	CLoginWindowUI*		m_pLoginWindowUI;
	CAssetsWindowUI*	m_pCloudAssetsWindowUI;
	CDBankAssetsWindowUI*	m_pDBankAssetsWindowUI;
	
	void				OnEditTabChangeLogin(TNotifyUI& msg);
	void				OnGoGuide(TNotifyUI& msg);
	void				OnForgetPassword(TNotifyUI& msg);
	void				OnShowStatement(TNotifyUI& msg);
	
	bool				OnLoginWindowDestroy(void* pObj);
	//

	//注册
	CRegisterWindowUI*	m_pRegisterWindowUI;
	CSearchWindowUI*	m_pSearchWindowUI;
	
	void				OnRefrshCheckCode(TNotifyUI& msg);
	void				OnRegister(TNotifyUI& msg);
	void				OnBackLogin(TNotifyUI& msg);
	bool				OnRegisterWindowDestroy(void* pObj);
	//
	//修改密码
	CChangePasswordWindowUI* m_pChangePasswordWindowUI;
	void				OnChangePassword(TNotifyUI& msg);
	
	//
	
	//重置密码
	CGetBackPwdWindowUI* m_pGetBackPwdWindowUI;
	void				OnGetBackPwd(TNotifyUI& msg);
	void				OnSentPhoneCode(TNotifyUI& msg);
	
	//
	//邮箱注册成功
	CHorizontalLayoutUI*m_pEmailTipLayout;
	void				OnBackRegister(TNotifyUI& msg);
//	void				OnOpenEmailUrlBtn(TNotifyUI& msg);
	
	//手机注册成功

	bool DownLoadToLocal(void* pObj);
	bool DownLoadToLocalCompelete(void* pObj);
	bool UploadNetdisc(void* pObj);
	bool UploadNetdiscCompelete(void* pObj);
	bool DeleteResource(void* pObj);
	bool DeleteResourceCompelete(void* pObj);
	bool RenameResource(void* pObj);
	bool RenameResourceCompelete(void* pObj);
	bool EditExercises(void* pObj);
public:
	void				SetDlgAcceptFiles(bool bAccept);

	void				OnLogin(TNotifyUI& msg);
	void				MobileLogin(CStream *loginStream);

	void				OnLocalImport(TNotifyUI& msg);
	LPCTSTR				GetFileFilter(int nType);

	virtual void		ShowWindow( bool bShow = true, bool bTakeFocus = true );

	CCloudItemUI*		CreateItem();
	CLocalItemUI*		CreateLocalItem();
	CSearchWindowUI*	CreateSearchWindow();
	CLoginWindowUI*		CreateLoginWindow();
	CRegisterWindowUI*	CreateRegisterWindow();
	CChangePasswordWindowUI*	CreateChangePasswordWindow();
	CGetBackPwdWindowUI*	CreateGetBackPwdWindow();
	CAssetsWindowUI*	CreateAssetsWindow();
	CDBankAssetsWindowUI*	CreateDBankAssetsWindow();
	CContainerUI*		CreateRegisterEmailSuccessWindow();

	CContainerUI*		CreateImportWindow();
	void				ResetWindow();
	void				ShowSearchHistory();
	void				InsertKeyWordJson(tstring & strKeyWord);

	void				ShowReslessUI( bool bShow );
	virtual void		ShowNetlessUI( bool bShow );
	virtual void		ShowResource( int nType, CStream* pStream, void* pParams = NULL);

	void				ExtendWidth();
	void				ResetWidth();
	void				SetSearchWidth();

	bool				IsCloudExplorer();

	bool				OnCreateCallback(void * pParam);

	void				OnSearchComplete(THttpNotify* pHttpNotufy);

	void				SetListBar(void* pListBar)	{	m_pListBar = pListBar;	}

	tstring				GetChapter()				{	return m_strChapter;	}

	bool				OnSearchWindowDestroy(void* pNotify);

	bool				DeleteLocalItem(tstring strItemPath);
	bool				RenameLocalItem(tstring strItemPath,tstring strNewName,bool isModifyTitleOnly=false);

	void				AddDBankItemCount(int nType);

	void				ShowUploadUI();
	//--增加显示登录窗口
	void				ShowLoginUI(BOOL bAutoLogin = FALSE);
	void				ShowRegisterUI();
	void				ShowChangePasswordUI();
	void				ShowRegisterEmailSuccessUI();
	void				ShowGetBackPwdUI();
	void				ShowSearchUI(CDelegateBase& delegate);
	//
	void				ShowCloudAssetsUI();
	void				ShowDBankAssetsUI();
	void				Show101PPTSearchUI();
	//
	static void  TimerProcComplete(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime);
	//

	CSliderTabLayoutUI*	getSliderTabLayout() {return m_pSliderTabLayout;};

	bool				m_bSearch;
	HWND				m_hParentHwnd;

	virtual LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
private:
	CDialogBuilder		m_searchBuilder;
	CDialogBuilder		m_localBuilder;
	CDialogBuilder		m_cloudItemBuilder;
	CDialogBuilder		m_styleBuilder;
	CDialogBuilder		m_loginBuilder;
	CDialogBuilder		m_registerBuilder;
	CDialogBuilder		m_changePasswordBuilder;
	CDialogBuilder		m_assetsWindowBuilder;
	CDialogBuilder		m_getBackPwdBuilder;
	CDialogBuilder		m_registerEmailBuilder;
	CDialogBuilder		m_uploadBuilder;
	CDialogBuilder		m_statementBuilder;

	CItemExplorerUI*	m_pItemExplorer;
	int					m_nCurrentType;
	int					m_nLocalItemType;
	CVerticalLayoutUI*	m_layResless;

	CGifAnimUI*			m_pGif;
	tstring				m_strKeyWord;

	Json::Reader		m_jsonKeyWord;
	Json::FastWriter	m_jsonKeyWordWriter;
	Json::Value			m_jsonCloudKWRoot;
	tstring				m_strCloudKWPath;

	//Import
	CFileDialog*		m_pDlgImport;
	void*				m_pListBar;
	CVerticalLayoutUI*	m_pEmptyLayout;
	CVerticalLayoutUI*	m_pImportLayout;
	CLabelUI*			m_pImportTipLabelOne;
	CLabelUI*			m_pImportTipLabelTwo;
	CLabelUI*			m_pTipLabel;
	CLabelUI*			m_pCountLabel;
	CProgressUI*		m_pImportProgress;
	tstring				m_strImportFolderName;
	tstring				m_strChapterGuid;
	tstring				m_strChapter;

	int					m_nCurrentTypeBak;

	bool				m_bNeedClassify;

	
	CVerticalLayoutUI*			m_pLoginList;
	CVerticalLayoutUI*			m_pChangePasswordList;
	CVerticalLayoutUI*			m_pGetBackPwdList;
	CVerticalLayoutUI*			m_pCloudAssetsList;
	CVerticalLayoutUI*			m_pDBankAssetsList;
	CVerticalLayoutUI*			m_pSearchList;
	CVerticalLayoutUI*			m_pUploadList;

	list<LOCAL_RES_INFO>		m_lstFileName;
	//list<LOCAL_RES_INFO>		m_lstSaveFile;

	//map<int, list<tstring>>		m_mapImportFile;
	list<LOCAL_RES_INFO>		m_lstImportFile[LocalFileTotal - LocalFileImport];

	map<tstring, list<LOCAL_SEARCH_RESULT>> m_mapLocalSearchRes;

	bool				m_bImporting;
	bool				m_bLayoutChanged;

	int					m_nSearchCount;		//搜索到资源总量
	int					m_nCurCount;

	CLabelUI*			m_pLabelCloudCount;

	CTabLayoutPageUI*	m_pTabLayoutCloudPage;
	CTabLayoutLocalPageUI*	m_pTabLayoutLocalPage;

	CVerticalLayoutUI*	m_pSearchMainwindow;
	CVerticalLayoutUI*	m_pSearchFailwindow;
	CVerticalLayoutUI*	m_pCloudSearchContainer;
	CVerticalLayoutUI*	m_pLocalSearchContainer;
	CVerticalLayoutUI*	m_pSearchNetless;
	CVerticalLayoutUI*	m_pSearchFailContainer;

	//CContainerUI *		m_pSearchWindow;

	DWORD				m_dwCloudSearchDownloadId;


	CItemPageUI*		m_pItemPageUI;
	CMenuUI* m_pMenuUI;

	CEventSource		m_OnClickCallBack;

	CContainerUI*		m_laySeeMore;

};

