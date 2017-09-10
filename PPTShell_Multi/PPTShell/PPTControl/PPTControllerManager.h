//===========================================================================
// FileName:				PPTControllerManager.h
// 
// Desc:					One thread one PPTController
//============================================================================
#ifndef _PPT_CONTROLLER_MANAGER__H_
#define _PPT_CONTROLLER_MANAGER__H_

#include "PPTController.h"
#include "Util/Stream.h"
#include <set>

#define PPT_OPERATION_THREAD_MAX_COUNT	(3)

enum PPTVRType{
	VRTYPE_EXE = 0,
	VRTYPE_VIDEO,
	VRTYPE_OTHER
};

enum PPTContorl
{
	PPTC_NEW,
	PPTC_PPNEW,
	PPTC_OPEN,		//PPT文件打开的回调
	PPTC_SAVE,
	PPTC_PACKAGE,
	PPTC_CLOSE,
	PPTC_OPEN_DESKTOPPPT,
	PPTC_CLOSE_APP,
	PPTC_3DPPT,

	PPTC_IS_CHANGED_READONLY,

	PPTC_PLAY,
	PPTC_STOP,
	PPTC_NEXT_SLIDE,
	PPTC_GOTO_SLIDE,
	PPTC_PREVIOUS_SLIDE,
	PPTC_NDICRPLAY,

	PPTC_UNINSERT,
	PPTC_UNDO,
	PPTC_SET_PEN_COLOR,
	PPTC_SET_POINTER_TYPE,
	PPTC_CLEAR_INK,
	PPTC_GET_SLIDER_INDEX,
	PPTC_GET_SLIDER_COUNT,
	PPTC_RESTORE_FLASH_FRAME_NUM,

	PPTC_UPDATEAPP,
	PPTC_GET_VIDEO_LIST,
	PPTC_GET_QUESTION_LIST,
	PPTC_HAS_ACTIVEX_AT_PADGE,
	PPTC_SEND_EVENT_TO_CEF,
	PPTC_SET_FOCUS_TO_CEF,
	PPTC_SYNC_CHANGE,		//	同步编辑
	PPTC_OPEN_PPTFILE,		//打开ppt文件
	PPTC_OPEN_BYTITLEBTN,	//从菜单栏打开ppt
	PPTC_PLAY_VIDEO,        //播放VR 媒体
	PPTC_STOP_VIDEO,		//停止VR 媒体
};


typedef struct PALCE_HOLDER
{
	DWORD	dwSlideId;
	DWORD	dwShapeId;
}_PALCE_HOLDER, *sPALCE_HOLDER;

typedef multimap<DWORD, PALCE_HOLDER>::iterator ITER_PH;

class CPPTControllerManager
{
public:
	CPPTControllerManager();
	~CPPTControllerManager();

	static CPPTControllerManager* GetInstance();
	static BOOL			UnInstance();
	static BOOL			PPTControllerUpdate();
	static BOOL			PPTControllerDestory();

	BOOL				SetOwner(HWND hWnd);
	BOOL				Initialize();
	BOOL				Destroy();
	CPPTController*		GetPPTController();

	void	AddPPTInsertOperation(CStream* pStream);
	void	AddPPTControlOperation(CStream* pStream, BOOL bFront = FALSE);
	BOOL								m_bCancelInsert;

protected:
	map<DWORD, CPPTController*>			m_mapControllers;		// threadID <--> CPPTController*
	multimap<DWORD, PALCE_HOLDER>		m_mapPalceHolders;		// Appcliation <--> PalceHolder*
	//list<PALCE_HOLDER>					m_lstPalceHolders;

	CRITICAL_SECTION					m_Lock;
	CRITICAL_SECTION					m_ControlLock;
	list<CStream*>						m_lstPptOperation;
	list<CStream*>						m_lstPptControl;

	bool								m_bRuning;

	HANDLE								m_hEvents[PPT_OPERATION_THREAD_MAX_COUNT];
	HANDLE								m_hThread[PPT_OPERATION_THREAD_MAX_COUNT];

	HANDLE								m_hControlEvent;
	HANDLE								m_hControlThread;

	HWND								m_hOwnerWnd;

	int									m_nCurrentPos;

	static CPPTControllerManager*		g_pPPTControllerManager;

	static DWORD WINAPI PPTInsertThread(LPARAM lParam);
	static DWORD WINAPI PPTControlThread(LPARAM lParam);

	BOOL	PPTInsertFunc(CPPTController* pController, CStream* pStream);
	BOOL	PPTControlFunc(CPPTController* pController, CStream* pStream);

	BOOL	InsertPPT(LPCTSTR szPath, int nStartSlideIdx, int nEndSlideIdx, int nSlideIdx, tstring& strErroInfo, NOTIFY_EVENT* pNotify);
	BOOL	ExportPPTBackGround(LPCTSTR szPath, int nStartSlideIdx, int nEndSlideIdx, std::set<int>& vecHasBackGround);
	BOOL	InsertPPTBySlideId(LPCTSTR szPath, int nStartSlideIdx, int nEndSlideIdx, int nSlideId, int nPlaceHolderId, tstring& strErroInfo, NOTIFY_EVENT* pNotify);

	BOOL	RemovePalceHolder(DWORD dwSlideId, DWORD dwPalceHolderId);

	BOOL	RemovePPTPalceHolder(CPPTController* pPPTController, DWORD dwSlideId, DWORD dwPalceHolderId);
	BOOL	RemoveAllPPTPalceHolder(CPPTController* pPPTController);
};

CPPTController* GetPPTController();
BOOL PPTControllerMgrUpdate();


BOOL InsertPalceHolderByThread(LPCTSTR szPath, int nLeft = -1, int nTop = -1, int nWidth = -1, int nHeight = -1, int nSlideIdx = 0, CEventDelegateBase* pOnCompleteDelegate = NULL);
BOOL InsertPPTByThread(LPCTSTR szPath, int nStartSlideIdx, int nEndSlideIdx, int nSlideIdx = 0, int nSlideId = 0, int nPlaceHolderId = 0, CEventDelegateBase* pOnProcessDelegate = NULL, CEventDelegateBase* pOnCompleteDelegate = NULL);
BOOL InsertPictureByThread(LPCTSTR szPath, int nLeft = -1, int nTop = -1, int nWidth = -1, int nHeight = -1, int nSlideIdx = 0, int nSlideId = 0, int nPlaceHolderId = 0);
BOOL InsertHyperLinkPictureByThread(LPCTSTR szPath, LPCTSTR szHyperlink, int nLeft = -1, int nTop = -1, int nWidth = -1, int nHeight = -1, int nSlideIdx = 0, int nSlideId = 0, int nPlaceHolderId = 0);
BOOL InsertVideoByThread(LPCTSTR szPath, int nLeft = -1, int nTop = -1, int nWidth = -1, int nHeight = -1, int nSlideIdx = 0, int nSlideId = 0, int nPlaceHolderId = 0);
BOOL InsertOLEObjectByThread(LPCTSTR szPath, int nLeft = -1, int nTop = -1, int nWidth = -1, int nHeight = -1, int nSlideIdx = 0, int nSlideId = 0, int nPlaceHolderId = 0);
BOOL InsertSwfByThread(LPCTSTR szPath, LPCTSTR szPicPath, LPCTSTR szTitle, LPCTSTR szSign = PLUGIN_SIGN, int nSlideIdx = 0, int nSlideId = 0, int nPlaceHolderId = 0);
BOOL InsertQuestionByThread(LPCTSTR szPath, LPCTSTR szQuestionType, LPCTSTR szGuid,  int nSlideId = 0, int nPlaceHolderId = 0,CEventDelegateBase* pOnCompleteDelegate = NULL );
BOOL InsertUpdateQuestionByThread(LPCTSTR szTitle,bool bBasicQuestion,LPCTSTR szPath, LPCTSTR szQuestionType, LPCTSTR szGuid,  int nSlideId = 0, int nPlaceHolderId = 0,CEventDelegateBase* pOnCompleteDelegate = NULL );
BOOL Insert3DByThread(LPCTSTR szPath, LPCTSTR szThumbnailPath, int nSlideId = 0, int nPlaceHolderId = 0);
BOOL InsertVRByThread(PPTVRType nVrType,LPCTSTR szPath, LPCTSTR szParam, LPCTSTR szThumbnailPath, int nSlideId = 0, int nPlaceHolderId = 0);
BOOL UpdateQuestionByThread(LPCTSTR szPath, LPCTSTR szGuid);
BOOL RemovePalceHolderByThread(int nSlideId, int nPalceHolderId);

BOOL PlayPPTByThread();
BOOL StopPPTByThread();
BOOL NextSlideByThread();
BOOL GotoSlideByThread(int nSlideIdx);
BOOL PreviousSlideByThread();
BOOL SavePPTByThread(LPCTSTR szPath, BOOL bBefore, BOOL bClosePre, BOOL bICRPlay, int nCommand);

BOOL NewPPTByThread();
BOOL OpenPPTByThread(LPCTSTR szPath, BOOL bReadOnly);
BOOL OpenDeskTopPPTByThread(LPCTSTR szPath, BOOL bReadOnly);
BOOL ClosePPTByThread(BOOL bBefore);
BOOL AddSlideByThread();

BOOL IsPPTChangedReadOnlyByThread(BOOL bBefore, int nCommand);

BOOL CloseAPPByThread();

BOOL SetPenColorByThread(DWORD dwColor);
BOOL SetPointerTypeByThread(int nType);
BOOL ClearInkByThread();
BOOL UnDoByThread(int nCount = 1);
BOOL UnDoInsertPPTByThread();
BOOL GetCurSlideIndexByThread();
BOOL GetCurSlideCountByThread();
BOOL RestoreFlashFrameNumByThread();
BOOL GetVideoListByThread();
BOOL GetQuestionListByThread();
BOOL SendEventToCefByThread(LPCTSTR szEventName, LPCTSTR szEventData);
BOOL SetFocusToCefByThread();
BOOL ExportAndIncrementSyncByThread();
BOOL VRPlayVideo();
BOOL VRStopVideo();

#endif