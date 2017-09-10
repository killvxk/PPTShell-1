
// MainFrm.h : interface of the CMainFrame class
//

#pragma once

#include "MainPane.h"
#include "GUI\SkinFrameWnd.h"
#include "PPTControl\PPTController.h"
#include "DUI\AboutMenu.h"
#include "DUI\UpdateDialog.h"
#include "DUI\ShadeWindow.h"
#include "Update\UpdateOperation.h" 
#include "DUI\ScreenInstrument.h"
#include "Util/Singleton.h"
#include "NDCefLib/PPTDaemonEvent.h"
#include "gui/LyricsWindow.h"

#define GUIDE_REMOTE_OPEN	(0)
#define GUIDE_LOCAL_OPEN	(1)

#define COMMAND_ID_PLAY					0x90001
#define COMMAND_ID_STOP					0x90002
#define COMMAND_ID_NEXT					0x90003
#define COMMAND_ID_INSIDE_PPT_ENTER		0x90004
#define COMMAND_ID_INSIDE_PPT_LEAVE		0x90005
#define COMMAND_INIT_UNTITLEWINDOW		0x100001 //创建初始化的新建窗口消息
#define COMMAND_OPEN_FORM_DESKTOP		0x110001

typedef struct WNDINFO
{   
	HWND	hWnd;   
	DWORD	dwPid;   
}sWNDINFO, *spWNDINFO;   


class CMainFrame : public CSkinFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	CPPTController*	GetPPTController(){		return m_pMainPPTController;	}
	BOOL			SetMainPaneMoveStatus(BOOL bEnable);
	void			ShowFloatCtrl();//显示浮动窗口
	void			HideFloatCtrl();//隐藏浮动窗口
	void			EnablePPTOperationButton(BOOL bEnabled, BOOL bEnabledOpen = TRUE);
	bool			OnCheckUpdate( void* pObj );

	bool			IsOperationerChanged(DWORD dwLastOperationerId);
	void			GenarateOperationer();
	DWORD			GetOperationerId();

	void			OnPptPlay();

	void			NDICRPlay(LPTSTR szPath);//启动互动课堂播放

	void			OpenDoc(tstring strPath);

	void			ModifyTitleButtonStatus();

	// 3d
	BOOL			Show3DAmination(tstring strPath);
	BOOL			Close3DAmination();
	//VR
	BOOL			ShowVRMediaPlayer(tstring strPath,tstring nParam);
	BOOL			CloseVRMediaPlayer();

	static BOOL		IsMainFrmActive();

	static HWND		m_hMainWnd;

	// exercises edit window
	void			ShowExercisesEditWindow(tstring strPath);
	void			MoveExercisesEditWindow();
	void			HideExercisesEditWindow();

	// exercises edit window
	void			ShowVRDistributeWindow(tstring strPath);
	void			MoveVRDistributeWindow();
	void			HideVRDistributeWindow();

	BOOL			GetPPTOpening(){ return m_bOpening; }


	static void UploadQuestion(tstring strTitle,tstring strGuid,bool bBasicQuestion);
// Generated message map functions
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnMove(int x, int y);
	afx_msg LRESULT OnUserSendEvent(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserPostEvent(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserHttpMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserHttpUploadMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPPTExportImagesMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPPTPackPPTMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShowUpdateTipMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShowUpdateInfoMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDownloaderClose(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserPPTControl(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnQuestionThumbnail(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	DECLARE_MESSAGE_MAP()
	
protected:
	virtual BOOL	OnCommand(WPARAM wParam, LPARAM lParam);
	void			PublishPPT();
	void			CreateTitleButton();
	bool			OnChapterChange(void* pNotify);
	bool			OnUpdateEvent(void* pNotify);
	bool			OnPPTOperation(void* pNotify);
	bool			OnEnvRelease(void* pNotify);
	bool			OnShowNextSlide(void* pNotify);
	bool			OnEventPPTControl(void* pNotify);
	bool			OnEventPPTInsert(void* pNotify);
	bool			OnMobileAutoLogin(void* pNotify);
	bool			OnPPTExportCompleted(void* pObj);

	bool			RunPlugin(bool bLastPage);
	void			StartSwf(LPCTSTR strPath);

	void			SetUpdateIconStatus(int nUpdate);
	void			SetWindowClass(CREATESTRUCT& cs);
	void			SetChapterButtonInfo(tstring strChapter);

	LRESULT			OnNotifyUpdate(WPARAM wParam, LPARAM lParam);
	LRESULT			OnTitleButtonOperation(WPARAM wParam, LPARAM lParam);
	LRESULT			OnTitleMouse(WPARAM wParam, LPARAM lParam);
	LRESULT			OnShowAddSlideWindow(WPARAM wParam, LPARAM lParam);
	LRESULT			OnShowExerEditWindow(WPARAM wParam, LPARAM lParam);
	LRESULT			OnShowVRDistributeWindow(WPARAM wParam, LPARAM lParam);

	void			ExecCommandAfterSave(int nCommand, BOOL bBefore, tstring strPath, int nCount = 0);

	void			CourseAddToLocalRes(tstring strPath);

	void			CreateDocByPowerPointer();
	void			OpenDocByFileDialog();
	void			OpenDocByDesktop();

	// ndppt
	BOOL			OpenNdpx(tstring strPath);
	BOOL			IsNdpx(tstring strPath);

	void			Create3DImages(tstring strPath);
	bool			Play3DPPT(tstring strFolderPath);


	static DWORD WINAPI	PPTControlThread(LPARAM lParam);
	void			SaveMainFrameHwnd();
 
	bool			OnSaveToDBankCompleted(void* param);
	bool			OnQuestionDownloaded(void* param);
	bool			OnCoursewareObjectDownloaded(void* param);

	static DWORD WINAPI DanmakuWndThread(LPARAM lParam);
	// add slide window
	void			ShowAddSlideWindow();
	void			HideAddSlideWindow();

protected:
	CMainPane			m_wndMainPane;

	static CResGuard	m_Lock;
 
	CScreenInstrumentUI* m_pScreenInstrument;

	CDUIButton*			m_pAboutBtn;
	CDUIButton*			m_pChapterBtn;
	CDUIButton*			m_pPrevBtn;
	CDUIButton*			m_pNextBtn;
	CDUIButton*			m_pUserLoginBtn;//2015.11.05 cws 登陆
	CDUIButton*			m_pDownloadMgr;
	HWND				m_hUserReLogin;//2016.01.12 cws 点击重新登录句柄

	HWND				m_h3DWnd;

	int					m_nUpdateStatus;
	int					m_nSwfIndex;
	int					m_nCurPPTCommand;

	BOOL				m_bFirstCheckUpdate;

	tstring				m_strVerion;
	tstring				m_strUpdateContent;
	LONGLONG			m_llAllFilesSize;
	tstring				m_strDeskTopPPTPath;
	tstring				m_strPPPath;				//open or new by powerpoint
	BOOL				m_bOpening;
	DWORD				m_dwOpenedTime;

	CAboutMenu*			m_pMenu;
	CUpdateDialogUI*	m_pUpdateDlg;
	//CPPTController*		m_pPPTController;

	CPPTController*		m_pMainPPTController;

	DWORD				m_dwLastOperaionerId;
	BOOL				m_bFirstStart;
	BOOL				m_bIsChanged;
	
	BOOL				m_bForceUpdate;

	// Question player
	HANDLE				m_hCefMapping;	
	BOOL				m_bCefRunning;

	int					m_nLoginType;

	int					m_nSaveToSkyType;
	//守护进程的开启事件
	CPPTDaemonEvent		m_nPPTDaemonEvent;

	tstring				m_strConfigClassName;	// Config.ini的班级名称
public:
	BOOL			SetConfigClassName(tstring strConfigClassName, BOOL bWriteConfigFile=FALSE);
	tstring			GetConfigClassName();	// 获取Config.ini的ClassName

	afx_msg void	OnPaint();
	afx_msg BOOL	OnEraseBkgnd(CDC* pDC);
	virtual BOOL	PreTranslateMessage(MSG* pMsg);

	CMainPane *		getMainPane();
	void			DrawSliderBkgnd();
	int				SavePPT(int nCommand, tstring strFilePath, tstring strFileName, BOOL bBefore = FALSE, BOOL bReadOnly = FALSE, BOOL bOpen = FALSE, BOOL bClosePre = FALSE, BOOL bICRPlay = FALSE);
	int				SavePPT4Close(int nCommand, tstring strFilePath, tstring strFileName, BOOL bBefore = FALSE, BOOL bReadOnly = FALSE, BOOL bOpen = FALSE, BOOL bClosePre = FALSE, BOOL bICRPlay = FALSE);
	int				SelectPlayMode();
	int				SavePPTToSkyDrive();

	BOOL			OnDestroyBefore();
	BOOL			PPTSaving(LPCTSTR strPath, BOOL bBefore, BOOL bClosePre, BOOL bICRPlay, int nCommand);
	BOOL			NDStartICRPlay(LPTSTR szPath);
	BOOL			IsUntitledShow();
	BOOL			SetUntitledShow(bool bShow);
	BOOL			OnSetToolTipText(UINT id, NMHDR *pTTTStruct, LRESULT *pResult);//2015.11.06 cws提示信息

	BOOL			m_bPreView;
	BOOL			m_bClosing;
	BOOL			m_bCloseAll;
	BOOL			m_bCloseCount;
	BOOL			m_bUploadingOnClose;
	CUpdate			m_update;
	int				m_nChangeSubOperation;
	BOOL			m_bIsReadOnlyOpen;//文件是否只读方式打开 2015.10.21 cws

	bool			OnEventLoginCompleteOperation(void* pNotify);//完成登陆后进行用户名更新
	bool			OnEventReLoginOperation( void* pNotify );//重新登录
	bool			OnEventLogOutOperation( void* pNotify );//退出登录

	//bool			OnEventAboutSoft(void* pNotify);//点击关于事件
	void			ResetLoginImage();//重置登陆的图标和文本2015.11.23
	void			CloseRelateApp();

	tstring			m_strSavePPTPath;

	int				m_nPlayMode;
	
	// add question
	tstring			m_strQuestionType;
	tstring			m_strQuestionGuid;
	bool			m_bBasicQuestion;

	// add Danmaku 弹幕
	CLyricsWindow	m_lyricsWindow;

	bool OnDanmukuWordAdd( void* pNotify ); //添加弹幕词句
//	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
//	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};


