//===========================================================================
// FileName:				PPTController.h
// 
// Desc:				All operations to control PPT
//============================================================================
#ifndef _PPT_CONTROLLER_H_
#define _PPT_CONTROLLER_H_

#define PPT_GUID			_T("GUID")
#define PPT_RECOVER			_T("RECOVERY")
#define PPT_INSIDE_PPT		_T("INSIDEPPT")
#define PPT_REUSING_PPT		_T("REUSE")

#include "msppt.tlh"
#include "PPTEventHandler.h"
#include "Util/Singleton.h"
#include "EventCenter/EventDefine.h"

using namespace MSPpt;

#define POUND_TO_PIXEL		1.333333f

typedef enum PPTERRORCODE
{
	PPTEC_INSERT_FAILED = 0,
	PPTEC_USER_CANCEL	= -1,
	PPTEC_SLIDE_NOT_SELECT = -2,
};

typedef enum PPTVERSION
{
	PPTVER_2007 = 0,
	PPTVER_2010,
	PPTVER_2013,
	PPTVER_UNKONW,
};

typedef enum OPERATIONPPTEVENT
{
	OPE_POWERPOINT = 0,
	OPE_USER_NEW,
	OPE_USER_OPEN,
	OPE_USER_INSERT,
	OPE_USER_EXPORT,
	OPE_USER_PACK,
};

typedef struct NOTIFY_EVENT
{
	CNotifyEventSource	m_OnProgress;
	CNotifyEventSource	m_OnComplete;

}sNOTIFY_EVENT, *spNOTIFY_EVENT;

class CPPTController
{
public:
	CPPTController();
	~CPPTController();

	friend class CPPTControllerManager;
	// Playing relative operations
	static HWND				GetPPTMenuHwnd();
	static BOOL				AutoClosePPTMenu();

	bool					OnApplicationClose();
	void					OnSize(UINT nType, int cx, int cy);
	int						GetOffsetWithForbidenDrag();
	
	int						GetActiveSlideIndex();
	inline BOOL				IsPPTActive();
	static BOOL				isPlayMode();//是否有播放窗口
	inline BOOL				IsPPTShowViewActive();
	BOOL					IfPPTSlideShowView();		//ppt是否在放映
	void					initApp();
	BOOL					New(tstring& strName);
	//void					Update();
	BOOL					ShowPPTWnd(CWnd* pParent);
	BOOL					ClosePPTWnd();
	void					AttatchPPTWindow(CWnd* pParent);
	BOOL					Open(LPCTSTR lpszPathName, BOOL bReadOnly = FALSE);
	BOOL					Close(BOOL bBefore = TRUE);
	BOOL					Play(BOOL bFromCurrentPage = FALSE);
	BOOL					Stop();
	BOOL					AddSlide();
	BOOL					IsChanged(BOOL bBefore = TRUE);	//check active ppt doc status changed;
	BOOL					IsChanged(_PresentationPtr pres);
	BOOL					IsReadOnly(BOOL bBefore = TRUE);
	BOOL					SetSaved(BOOL bSaved);
	BOOL					Save();
	BOOL					SaveAs(LPCTSTR szPath, BOOL bBefore = TRUE);
	BOOL					SaveAndClose(LPCTSTR szPath, BOOL bClose, BOOL bBefore = TRUE);
	BOOL					SaveCopyAs(LPCTSTR szPath);
	BOOL					Prev();
	BOOL					Next();
	BOOL					Goto(int nSlideIdx);
	BOOL					PresentationGoto(int iSlideIdx);					//预览状态下跳转
	BOOL					SetSlideShowViewState(PpSlideShowState nState);
	int						GetSlideShowViewState();
	BOOL					SetPointerType(PpSlideShowPointerType nType);
	PpSlideShowPointerType	GetPointerType();

	_ApplicationPtr			GetApplication(){  return m_pApplication; }
	void					SetApplication(_ApplicationPtr pApplication) {	m_pApplication = pApplication; InitEvent(); }
	void					GetNewApplication();

	tstring					GetFilePath(BOOL bBefore = FALSE);
	tstring					GetFileName(BOOL bBefore = FALSE);

	BOOL					Pptx2Ppt(LPCTSTR szPath);

	BOOL					OnClosePPT();

	BOOL					IsFinal();

	BOOL					UnDo(int nCount);
	BOOL					UnDoByCtrlZ();
	BOOL					GetUnDoList(vector<tstring>& vecOperations);
	BOOL					ClearInk(BOOL bAllSlider = FALSE);
	BOOL					ClearShowViewInk(BOOL bAllSlider = FALSE);

	BOOL					EnableNewInCommbar(BOOL bEnable);
	string					GetPPTPath();

	// Editing relative operations
	BOOL					InsertText(LPCTSTR szText, int nShapeIdx, int nSlideIdx = 0);
	BOOL					InsertPPT( LPCTSTR szPath, int nStartSlideIdx, int nEndSlideIdx, int nSlideIdx = 0, BOOL* pCancel = FALSE, NOTIFY_EVENT* pNotify = NULL);
	BOOL					InsertPicture(LPCTSTR szPath, int& nSlideId, int& nPalceHolderId, int nLeft = -1, int nTop = -1, int nWidth = -1, int nHeight = -1, int nSlideIdx = 0);
	BOOL					InsertHyperLinkPicture(LPCTSTR szPath, int& nSlideId, int& nPalceHolderId, LPCTSTR szHyperlink,int nLeft = -1, int nTop = -1, int nWidth = -1, int nHeight = -1, int nSlideIdx = 0);
	BOOL					InsertVideo(LPCTSTR szPath, int nLeft = -1, int nTop = -1, int nWidth = -1, int nHeight = -1, int nSlideIdx = 0);
	BOOL					InsertOLEObject(LPCTSTR szPath, int nLeft = -1, int nTop = -1, int nWidth = -1, int nHeight = -1, int nSlideIdx = 0);
	BOOL					InsertSwf(LPCTSTR szPath, LPCTSTR strPicPath, LPCTSTR szTitle, LPCTSTR szSign = PLUGIN_SIGN, int nSlideIdx = 0);

	BOOL					InsertCef(LPCTSTR szUrl, LPCTSTR szQuestionPath, int nLeft = -1, int nTop = -1, int nWidth = -1, int nHeight = -1, int nSlideIdx = 0);
	BOOL					UpdateCef(LPCTSTR szQuestionPath, LPCTSTR szGuid);

	BOOL					Insert3D(LPCTSTR szPath, LPCTSTR szThumbPath, int nSlideIdx = 0, LPCTSTR szSign = PLUGIN_3D);
	BOOL					InsertVRExe(LPCTSTR szPath,LPCTSTR szParam, LPCTSTR szThumbPath, int nSlideIdx = 0, LPCTSTR szSign = PLUGIN_VR);
	BOOL					InsertVRVideo(LPCTSTR szPath,LPCTSTR szParam, LPCTSTR szThumbPath, int nSlideIdx = 0, LPCTSTR szSign = PLUGIN_VR);

	// Editing relative operations by slide id
	BOOL					InsertPPTBySlideId(LPCTSTR szPath, int nStartSlideIdx, int nEndSlideIdx, int nSlideId, int nPlaceHolderId, BOOL* pCancel = FALSE, NOTIFY_EVENT* pNotify = NULL);
	BOOL					InsertPictureBySlideId(LPCTSTR szPath, int nSlideId, int nPlaceHolderId, int nLeft = -1, int nTop = -1, int nWidth = -1, int nHeight = -1 );
	BOOL					InsertHyperLinkPictureBySlideId(LPCTSTR szPath, int nSlideId, int nPlaceHolderId, LPCTSTR szHyperlink, int nLeft = -1, int nTop = -1, int nWidth = -1, int nHeight = -1 );
	BOOL					InsertVideoBySlideId(LPCTSTR szPath, int nSlideId, int nPlaceHolderId, int nLeft = -1, int nTop = -1, int nWidth = -1, int nHeight = -1);
	BOOL					InsertSwfBySlideId(LPCTSTR szPath, LPCTSTR strPicPath, LPCTSTR szTitle, int nSlideId, int nPlaceHolderId, LPCTSTR szSign = PLUGIN_SIGN);
	BOOL					InsertCefBySlideId(LPCTSTR szUrl, LPCTSTR szQuestionPath, int nSlideId, int nPlaceHolderId, int nLeft = -1, int nTop = -1, int nWidth = -1, int nHeight = -1);
	BOOL					InsertOLEObjectBySlideId(LPCTSTR szPath, int nSlideId, int nPlaceHolderId, int nLeft = -1, int nTop = -1, int nWidth = -1, int nHeight = -1);
	BOOL					Insert3DBySlideId(LPCTSTR szPath, LPCTSTR szThumbPath, int nSlideId, int nPlaceHolderId, LPCTSTR szSign = PLUGIN_3D);
	BOOL					InsertVRVideoBySlideId(LPCTSTR szPath,LPCTSTR szParam, LPCTSTR szThumbPath, int nSlideId, int nPlaceHolderId, LPCTSTR szSign = PLUGIN_VR);
	BOOL					InsertVRExeBySlideId(LPCTSTR szPath,LPCTSTR szParam, LPCTSTR szThumbPath, int nSlideId, int nPlaceHolderId, LPCTSTR szSign = PLUGIN_VR);

	BOOL					InsertFileLink(LPCTSTR szFilePath, LPCTSTR szIconPath, LPCTSTR szTipInfo, int nLeft, int nTop, int nWidth, int nHeight, int nSlideIdx = 0);
	BOOL					InsertBackground(LPCTSTR szPath, BOOL bApplyAllSlides);
	BOOL					InsertBackground(LPCTSTR szPath, SlideRangePtr pSlideRange, bool bSetFollowMasterBG = true);

	BOOL					PresentationToImage(_PresentationPtr Pres, char* szDir);
	BOOL					ExportToImages(const char* szFolderPath, const char* szExt, int nStartSlideIdx = -1, int nEndSlideIdx = -1, int nImageWidth = 0, int nImageHeight = 0, bool* pShouldCancel = NULL);
	BOOL					ExportToImages(char* szPPTPath, char* szFolderPath, char* szExt, bool* pShouldCancel = NULL, int* pCount= NULL, int nStartSlideIdx = -1, int nEndSlideIdx = -1, int nExistCount = 0, int nWidth = 0, int nHeight = 0);
	BOOL					ExportBackgroundToImages(char* szFolderPath, char* szExt, int nIndex);
	BOOL					ExportBackgroundToImages(char* szPath, char* szFolderPath, char* szExt, int nIndex);

	BOOL					ExportBackgroundToImages(char* szFolderPath, char* szExt, _SlidePtr pSlide);
	BOOL					ExportBackgroundToImages_check(char* szFolderPath, char* szExt, _SlidePtr pSlide);//核对是否有背景
	// Pack
	DWORD					PackAllResource(HWND hwnd, void* pNotify, char* szPath);
	DWORD					UnPackAllResource(HWND hwnd, void* pNotify, char* szPath);
	//

	// Others
	int						GetSlideCount();
	string					GetPresentationName();
	BOOL					GetOutLine(vector<string>& vecOutLine, string strPrefixIfNoExist = "");
	POINT					SlidePosToScreenPixel(POINTF pt);
	void					SlidePosToScreenPos(float &fWidth, float &fHeight);
	BOOL					SetPenColor(DWORD dwColor);
	BOOL					PenDrawLine(float BeginX, float BeginY, float EndX, float EndY);
	BOOL					PenEraseDrawing();
	BOOL					InitEvent();
	
	// Video control
	BOOL					PlayVideo(int nIndex = -1);
	BOOL					StopVideo(int nIndex = -1);
	BOOL					PauseVideo(int nIndex = -1);
	BOOL					PlayVRVideo(int nIndex = -1);
	BOOL					StopVRVideo(int nIndex = -1);
	BOOL					ObtainVideoList(vector<float>& vecVideoPostions, int& nSlideIndex);
	BOOL					ObtainQuestionList(vector<tstring>& vecQuestionList);
	BOOL					HasQuestionAtPage(int nIndex);
	BOOL					GetSlideAllShapesInfo(int nIndex, CStream* pStream);
	BOOL					SetFocusToCef(BOOL nReLoad = TRUE);
	BOOL					SendEventToCef(LPCTSTR szEventName, LPCTSTR szEventData);
	BOOL					KillCrashPowerPoint();

	// plugins
	BOOL					ShowMediaShape();
	tstring					GetMediaPath(int& nIndex);
	
	BOOL					IsInit()				{	return m_bPPTWndInit;	}
	static BOOL				IsInitFailed()			{	return m_bInitFailed;	}
	void					ReSetInit();
	static int				GetOperationPPTEvent()		{	return m_nOperationPpt;	}

	void					TopShowSlideView();
	static void				UpdateSlideShowViewHwnd(HWND hWnd)		{	m_PPTSlideShowHwnd = hWnd;	}
	static HWND				GetSlideShowViewHwnd()					{	return m_PPTSlideShowHwnd;	}
	static void				UpdatePPTHwnd(HWND hWnd);
	static void				UpdatePresenationGuid(tstring strGuid)	{	m_strNewPreGuid = strGuid;	}
	static bool				IsPPT64Bit()							{	return CPPTController::m_bIsPPT64;	}
	static void				SetPPT64Bit(bool b64Bit)				{	CPPTController::m_bIsPPT64 = b64Bit;	}

	tstring					GetLastComErrorInfo()					{   return m_strComErrorInfo;	}
	static int				GetPPTVersion();
	static void				RepairPPTBugVideoThumb();

	HWND					GetActiveWnd()							{	return (HWND)m_pApplication->GetHWND();	}
	BOOL					DeleteSlide();
	inline static HWND		GetPPTHwnd()							{	return m_PPTHwnd;	}


	//Hyperlink

	BOOL					SetTextHyperlink(tstring strDesTxt);

	/*void					SetUpdatePresentationStatus(BOOL bNeedUpdate);
	BOOL					IsNeedUpdatePresentation();*/

	void					ActivePPTWnd();
	static HWND				m_MainFrmHwnd;
	static BOOL				m_bNewByPPTControl;


	int						GetPresentationCount();
	tstring					GetCustomXMLPartString(int nSlideIdx);

	BOOL					RestoreFlashFrameNum(int nSlideIdx);

	void					GetSlideSize(int& nWidth, int& nHeight);

	static int				GetShowViewIndex()					{	return m_nShowViewSlideCurIndex;	}
	static void				SetShowViewIndex(int nIndex)		{	m_nShowViewSlideCurIndex = nIndex;	}

	static int				GetShowViewCount()					{	return m_nShowViewSlideCount;		}
	static void				SetShowViewCount(int nCount)		{	m_nShowViewSlideCount = nCount;		}	

	static void				CheckPPTRunTime();

	int						GetCurrentShowPosition();
protected:
	_PresentationPtr		GetPresentation(BOOL bBefore = FALSE);
	_PresentationPtr		GetCurrentPresentation();
	_PresentationPtr		GetFrontPresentation();

	SlidesPtr				GetSlides();
	_SlidePtr				GetSlide(int nSlideIdx);
	_SlidePtr				GetSlideById(int nSlideId);
	SelectionPtr			GetSelection();
	SlideShowViewPtr		GetSlideShowView();

	BOOL					ActivatePane(int nIndex);

	POINTF					ScreenPixelToSlidePos(POINT pt);
	
	_SlidePtr				GetActiveSlider();


	BOOL					ClearSliderInk(_SlidePtr pSlider);

	BOOL					DelAllShape(_SlidePtr pSlider);
	BOOL					DelShape(_SlidePtr pSlider, int nId);
	BOOL					DelPalceHolder(int nSlideId, int nId);

	void					ClearGuidTagInfo(_PresentationPtr pPresentation);
	static void				DoPPTRunTimeRepaire(PPTVERSION pptVersion);
	static PPTVERSION		GetSystemPPTVersion();
	
protected:
	_ApplicationPtr			m_pApplication;
	DWORD					m_dwPPTWndStyle;
	DWORD					m_dwPPTWndStyleEx;
	CPPTEventHandler*       m_pEventHandler;
	static float			m_fPPTVersion;

	static bool				m_bPPTWndInit;
	static bool				m_bInitFailed;
	static CResGuard		m_Lock;
	static int				m_nOperationPpt;
	
	static HWND				m_PPTHwnd;
	static HWND				m_PPTSlideShowHwnd;
	static ShapePtr			m_pMediaShape;

	static map<HWND, BOOL>	m_mapHwnd;
	

	// used by ppt playing
	static int				m_nShowViewSlideCurIndex;
	static int				m_nShowViewSlideCount;

	BOOL					m_bNeedUpdatePrenation;


	tstring					m_strComErrorInfo;
	static BOOL				m_bIsReadOnly;//2015.10.21 CWS是否是只读方式打开
	static tstring			m_strNewPreGuid;
	static bool				m_bIsPPT64;		// PPT.exe 是否是64位的
	static bool				m_bEverTryRepair;
	


};

#endif