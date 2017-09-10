//==============================================================================
// FileName:				IcrPlayer.cpp
//
// Desc:
//==============================================================================
#include "stdafx.h"
#include "IcrPlayer.h"
#include "Util/Util.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "NDCloud/NDCloudUser.h"
#include "NDCloud/NDCloudLocalQuestion.h"
#include "NDCloud/NDCloudAPI.h"
#include "DUI/ScreenInstrument.h"
#include "DUI/InstrumentItem.h"
#include "DUI/InstrumentView.h"
#include "DUI/InteractToolView.h"
#include "DUI/InteractiveType.h"
#include "DUI/ScreenToast.h"
#include "Config.h"
#include "DUI/ShowIcrFailInfo.h"

CIcrPlayer::CIcrPlayer()
{
	m_hIcrModule				= NULL;
	m_fnIcrInit					= NULL;
	m_fnIcrSetCallBack			= NULL;
	m_fnIcrOnStart				= NULL;
	m_fnIcrOnClick				= NULL;
	m_fnIcrOnPageIndexChange	= NULL;
	m_fnIcrOnRecvExamEvent		= NULL;
	m_fnIcrStop					= NULL;
	m_fnIcrUnInit				= NULL;
	m_fnIcrInvokeNativeMethod	= NULL;
	m_fnIcrGetOnlineStudents	= NULL;
	m_fnIcrSendCmd				= NULL;
	m_bUseStart					= false;


	m_nCurrentPageIndex			= -1;
}

CIcrPlayer::~CIcrPlayer()
{

}

BOOL CIcrPlayer::Initialize()
{ 
	if( !g_Config::GetInstance()->GetModuleVisible(MODULE_ND_ICRPLAY) )
		return FALSE;
	
	
	// load dll
	tstring strLocalPath = GetLocalPath();
	strLocalPath += _T("\\bin\\Plugins\\icr\\icrsdk.dll"); 

	m_hIcrModule = LoadLibraryEx(strLocalPath.c_str(),0, LOAD_WITH_ALTERED_SEARCH_PATH);
	if( m_hIcrModule == NULL )
		return FALSE;

	// get function address
	m_fnIcrInit = (LPFN_IcrInit)GetProcAddress(m_hIcrModule, "IcrInit");
	if( m_fnIcrInit == NULL )
		return FALSE;

	m_fnIcrSetCallBack = (LPFN_IcrSetCallback)GetProcAddress(m_hIcrModule, "IcrSetCallback");
	if( m_fnIcrSetCallBack == NULL )
		return FALSE;

	m_fnIcrOnStart = (LPFN_IcrOnStart)GetProcAddress(m_hIcrModule, "IcrOnStart");
	if( m_fnIcrOnStart == NULL )
		return FALSE;

	m_fnIcrOnClick = (LPFN_IcrOnClick)GetProcAddress(m_hIcrModule, "IcrOnClick");
	if( m_fnIcrOnClick == NULL )
		return FALSE;

	m_fnIcrOnPageIndexChange = (LPFN_IcrOnPageIndexChange)GetProcAddress(m_hIcrModule, "IcrOnPageIndexChange");
	if( m_fnIcrOnPageIndexChange == NULL )
		return FALSE;

	m_fnIcrOnRecvExamEvent = (LPFN_IcrOnRecvExamEvent)GetProcAddress(m_hIcrModule, "IcrOnRecvExamEvent");
	if( m_fnIcrOnRecvExamEvent == NULL )
		return FALSE;

	m_fnIcrGetOnlineStudents = (LPFN_IcrGetOnlineStudents)GetProcAddress(m_hIcrModule, "IcrGetOnlineStudents");
	if( m_fnIcrGetOnlineStudents == NULL )
		return FALSE;
	
	m_fnIcrInvokeNativeMethod = (LPFN_IcrInvokeNativeMethod)GetProcAddress(m_hIcrModule, "IcrInvokeNativeMethod");
	if( m_fnIcrInvokeNativeMethod == NULL )
		return FALSE;

	m_fnIcrSendCmd = (LPFN_IcrSendCmd)GetProcAddress(m_hIcrModule, "IcrSendCmd");
	/*if( m_fnIcrSendCmd == NULL )
	return FALSE;*/

	m_fnIcrRelease = (LPFN_IcrRelease)GetProcAddress(m_hIcrModule, "IcrRelease");
	if( m_fnIcrRelease == NULL )
		return FALSE;

	m_fnIcrStop = (LPFN_IcrStop)GetProcAddress(m_hIcrModule, "IcrStop");
	if( m_fnIcrStop == NULL )
		return FALSE;

	m_fnIcrUnInit = (LPFN_IcrUnInit)GetProcAddress(m_hIcrModule, "IcrUnInit");
	if( m_fnIcrUnInit == NULL )
		return FALSE;

	// init
	m_fnIcrInit();
	m_fnIcrSetCallBack(IcrRelease, IcrGetScreen, 
						IcrSendExamEvent, IcrGetTopLevelWindowHandle, 
						IcrMessageBox, IcrShowToast, 
						IcrEnable, IcrGetExamType, IcrSetButtonState);

	return TRUE;
}

BOOL CIcrPlayer::Destroy()
{
	if( m_hIcrModule == NULL )
		return FALSE;

	if( m_fnIcrUnInit != NULL )
		m_fnIcrUnInit();

	if( m_hIcrModule != NULL )
	{
		//TODO互动课堂自组网退出卡住，暂时不释放
		//FreeLibrary(m_hIcrModule);
		m_hIcrModule = NULL;
	}

	return TRUE;
}

//
// IcrOnStart
//
BOOL CIcrPlayer::IcrOnStart(CStream* pStream)
{
	if (m_bUseStart)//已经调用过，则不再初始化发送数据 2015.02.23 cws
	{
		return TRUE;
	}
	if( m_hIcrModule == NULL )
		return FALSE;

	if( m_fnIcrOnStart == NULL )
		return FALSE;

	Json::Value root;
	Json::Value tokenItem;
	Json::Value teacherItem;
	Json::Value classItem;
	Json::Value chapterItem;
	Json::Value examArrayItem(Json::arrayValue);
	Json::Value slidePosItem;	 
	Json::Value vrItem;

	// teacher
	TCHAR szTeacherId[MAX_PATH] = {0};
	DWORD dwTeacherId = NDCloudUser::GetInstance()->GetUserId();

	if( dwTeacherId != 0 )
		_stprintf_s(szTeacherId, _T("%d"), dwTeacherId);
	
	tstring strTeacherName = NDCloudUser::GetInstance()->GetNickName();
	WRITE_LOG_LOCAL(_T("IcrOnStart TeacherName:%s"), strTeacherName.c_str());
	int nFreeMode = NDCloudUser::GetInstance()->GetIsFreeMode();//当前老师的上课模式 2016.02.16 cws

	WRITE_LOG_LOCAL(_T("IcrOnStart FreeeMode:%d"), nFreeMode);

	teacherItem["id"]				= szTeacherId;
	teacherItem["name"]				= Str2Utf8(strTeacherName);

	//teacherItem["isfreemode"]		= nFreeMode;
	//root["teacher"]					= teacherItem;

	// token
	tstring strAccessToken = NDCloudUser::GetInstance()->GetAccessToken();
	tstring strMacKey = NDCloudUser::GetInstance()->GetMacKey();

	tokenItem["access_token"] = Str2Utf8(strAccessToken);
	tokenItem["mac_key"] = Str2Utf8(strMacKey);

	root["token"] = tokenItem;

	// chapter
	tstring strChapterGuid = NDCloudGetChapterGUID();
	tstring strBookGuid = NDCloudGetBookGUID();
	
	CCategoryTree* pCategoryTree = NULL; 
	NDCloudGetCategoryTree(pCategoryTree);

	tstring strCourseGuid = pCategoryTree->GetSelectedCourseGuid();
 
	chapterItem["subject"] = Str2Utf8(strCourseGuid);
	chapterItem["chapter"] = Str2Utf8(strChapterGuid);
	chapterItem["teaching_material"] = Str2Utf8(strBookGuid);

	root["chapter"] = chapterItem; 
	
	// class
	tstring strClassId = NDCloudUser::GetInstance()->GetClassGuid();
	WRITE_LOG_LOCAL(_T("IcrOnStart ClassId:%s"), strClassId.c_str());
	if (strClassId.c_str() == _T(""))
	{
		nFreeMode = 1;//强制转换为自由模式
	}
	if (nFreeMode == 1)//加一重保险，本身在设置模式为1的时候已经将字符串置空
	{
		strClassId = _T("");
	}
	classItem["id"]	= Str2Utf8(strClassId);
	root["class"]	= classItem;

	// students
	tstring strStudentInfo = NDCloudUser::GetInstance()->GetStudentInfo();
	WRITE_LOG_LOCAL(_T("IcrOnStart StudentInfo:%s"), strStudentInfo.c_str());
	bool bIsNull = NDCloudUser::GetInstance()->CheckItemsSize(strStudentInfo, _T("items"));
	if (!bIsNull)
	{
		nFreeMode = 1;//强制转换为自由模式
	}
	if (nFreeMode == 1)//加一重保险，本身在设置模式为1的时候已经将字符串置空
	{
		strStudentInfo = _T("");
	}
	root["students"] = Str2Ansi(strStudentInfo);

	teacherItem["isfreemode"]		= nFreeMode;
	root["teacher"]					= teacherItem;	 
	WRITE_LOG_LOCAL(_T("IcrOnStart TeacherInfo:%s"), Ansi2Str(teacherItem.toStyledString()).c_str());

	// groups
	tstring strGroupInfo   = NDCloudUser::GetInstance()->GetGroupInfo();
	WRITE_LOG_LOCAL(_T("IcrOnStart GrouopInfo:%s"), strGroupInfo.c_str());
	if (nFreeMode == 1)//加一重保险，本身在设置模式为1的时候已经将字符串置空
	{
		strGroupInfo = _T("");
	}
	root["groups"] = Str2Ansi(strGroupInfo);

	// devices
	tstring strDeviceInfo = NDCloudUser::GetInstance()->GetDeviceInfo();
	WRITE_LOG_LOCAL(_T("IcrOnStart DeviceInfo:%s"), strDeviceInfo.c_str());
	if (nFreeMode == 1)//加一重保险，本身在设置模式为1的时候已经将字符串置空
	{
		strDeviceInfo = _T("");
	}
	root["devices"] = Str2Ansi(strDeviceInfo);


	// is_recovery
	root["is_recovery"] = false;

	// slide_pos
	float fx		= pStream->ReadFloat();
	float fy		= pStream->ReadFloat();
	float fWidth	= pStream->ReadFloat() - fx;
	float fHeight	= pStream->ReadFloat() - fy;

	slidePosItem["left"]	= (int)fx;
	slidePosItem["top"]		= (int)fy;
	slidePosItem["width"]	= (int)fWidth;
	slidePosItem["height"]	= (int)fHeight;

	root["slide_pos"] = slidePosItem;

	m_nSlideShowLeft	= (int)fx;
	m_nSlideShowTop		= (int)fy;
	m_nSlideShowWidth	= (int)fWidth;
	m_nSlideShowHeight	= (int)fHeight;

	// exam
	int nExamNum = pStream->ReadInt();
	m_vecQuestionPaths.clear();

	tstring strSignature = _T("index.html?main=/");
	int nExamNumFind = 0;
	for(int i = 0; i < nExamNum; i++)
	{
		Json::Value examItem;
		tstring strUrl = pStream->ReadString();

		// 
		int pos = strUrl.find(strSignature);
		if( pos == -1 )
		{
			m_vecQuestionPaths.push_back(_T(""));
			continue;
		}
		
		nExamNumFind++;
		pos += strSignature.length();

		tstring strQuestionPath = strUrl.substr(pos);

		pos = strQuestionPath.rfind('/');
		if( pos == -1 )
		{
			m_vecQuestionPaths.push_back(_T(""));
			continue;
		}

		strQuestionPath = strQuestionPath.substr(0, pos);

		// convert '/' to '\'
		for(int i = 0; i < strQuestionPath.length(); i++)
		{
			if( strQuestionPath.at(i) == _T('/') )
				strQuestionPath.replace(i, 1, _T("\\"));
		}

		m_vecQuestionPaths.push_back(strQuestionPath);


		examItem["path"] = Str2Ansi(strQuestionPath);
		examArrayItem.append(examItem);
	}

	root["exam"] = examArrayItem;

	// vr enable
	tstring strVrVersionFilePath = GetLocalPath();
	strVrVersionFilePath += _T("\\vrversion.dat");

	DWORD dwAttributes = GetFileAttributes(strVrVersionFilePath.c_str());
	if( dwAttributes == INVALID_FILE_ATTRIBUTES )
		vrItem["enable"] = false;
	else
		vrItem["enable"] = true;

	root["vr"] = vrItem;

	//
	string str = root.toStyledString();
	tstring tstr = Ansi2Str(str);

	m_bUseStart = true;
	WRITE_LOG_LOCAL(_T("IcrOnStart FullInfo:%s"), tstr.c_str());
	m_fnIcrOnStart(str.c_str());

	return TRUE;
}

//
// IcrOnClick
//
BOOL CIcrPlayer::IcrOnClick(int nButtonID)
{
	if( m_hIcrModule == NULL )
		return FALSE;

	if( m_fnIcrOnClick == NULL )
		return FALSE;
	//////////////////////////////////////////////////////////////////////////
	//增加数据不全的提示,非自由授课模式则进行提醒
	if (NDCloudUser::GetInstance()->GetIsFreeMode() == 0)
	{
		if (!NDCloudUser::GetInstance()->GetIsLoadAllTeacherInfo())
		{
			ShowIcrFailInfoUI::GetInstance()->ShowLocalWindows(NULL);
			return FALSE;
		}
		else
		{ 
			IcrOnStart(NDCloudUser::GetInstance()->GetIcrStartInfo());//加载完成自动关闭窗口后调用启动101pad			 
		}
	}
	//////////////////////////////////////////////////////////////////////////

	m_fnIcrOnClick(nButtonID);
	return TRUE;
}

//
// IcrOnPageIndexChange
//
BOOL CIcrPlayer::IcrOnPageIndexChange(int nCurrentPageIndex, int nNextPageIndex)
{
	if( m_hIcrModule == NULL )
		return FALSE;

	if( m_fnIcrOnPageIndexChange == NULL )
		return FALSE;

	m_nCurrentPageIndex = nCurrentPageIndex;
	m_fnIcrOnPageIndexChange(nCurrentPageIndex, nNextPageIndex);

	return TRUE;
}

//
// IcrOnRecvExamEvent
//
BOOL CIcrPlayer::IcrOnRecvExamEvent(const char* szEventName, const char* szEventData)
{
	if( m_hIcrModule == NULL )
		return FALSE;

	if( m_fnIcrOnRecvExamEvent == NULL )
		return FALSE;

	m_fnIcrOnRecvExamEvent(szEventName, szEventData);
	return TRUE;
}

//
// IcrGetOnlineStudents
//
BOOL CIcrPlayer::IcrGetOnlineStudents(char** ppStudentsJson)
{
	if( m_hIcrModule == NULL )
		return FALSE;

	if( m_fnIcrGetOnlineStudents == NULL )
		return FALSE;

	m_fnIcrGetOnlineStudents(ppStudentsJson);
	return TRUE;
}

//
// IcrInvokeNativeMethod
//
char*  CIcrPlayer::IcrInvokeNativeMethod(char* szMethodJson)
{
	if( m_hIcrModule == NULL )
		return FALSE;

	if( m_fnIcrInvokeNativeMethod == NULL )
		return FALSE;

	return m_fnIcrInvokeNativeMethod(szMethodJson);
}
//
// IcrReleaseMemory
//
BOOL CIcrPlayer::IcrReleaseMemory(void* pAddress)
{
	if( m_hIcrModule == NULL )
		return FALSE;

	if( m_fnIcrRelease == NULL )
		return FALSE;

	m_fnIcrRelease(pAddress);
	return TRUE;
}


//
// IcrStop
//
BOOL CIcrPlayer::IcrStop()
{
	if( m_hIcrModule == NULL )
		return FALSE;

	if( m_fnIcrStop == NULL )
		return FALSE;
	m_bUseStart = false;

	try
	{
		m_fnIcrStop();
	}
	catch (...)
	{
		WRITE_LOG_LOCAL(_T("IcrStop ERROR"));
		return FALSE;
	}

	return TRUE;
}

//
// IcrRelease
//
void CALLBACK CIcrPlayer::IcrRelease(void* pAddress)
{
	char* pBuffer = (char*)pAddress;
	if( pBuffer != NULL )
		delete pBuffer;

}

//
// IcrGetScreen
//
bool CALLBACK CIcrPlayer::IcrGetScreen(int nQuality, unsigned char** pData, int& nSize)
{
	HWND hWnd = GetDesktopWindow();

	// hide toolbar
	CScreenInstrumentUI* pScreenInstrumentUI = CScreenInstrumentUI::GetMainInstrument();
	if( pScreenInstrumentUI != NULL )	
		pScreenInstrumentUI->ShowWindow(false);

	if( nQuality == ICR_SCREEN_QUALITY_HIGH )
		IcrPlayer::GetInstance()->GetWindowScreen(hWnd, 1, (char**)pData, nSize, NULL);
	else
		IcrPlayer::GetInstance()->GetWindowScreen(hWnd, 2, (char**)pData, nSize, NULL);

	if( pScreenInstrumentUI != NULL )	
		pScreenInstrumentUI->ShowWindow(true);

	return true;
}

//
// IcrSendExamEvent
//
bool CALLBACK CIcrPlayer::IcrSendExamEvent(const char* szEventName, const char* szEventData)
{
	SendEventToCefByThread(Ansi2Str(szEventName).c_str(), Ansi2Str(szEventData).c_str());
	return true;
}

//
// IcrGetTopLevelWindowHandle
//
HWND CALLBACK CIcrPlayer::IcrGetTopLevelWindowHandle()
{
	HWND hWnd = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
	return hWnd;
}

//
// IcrMessageBox
//
int CALLBACK CIcrPlayer::IcrMessageBox(const char* szText, const char* szCaption, unsigned int uType)
{
	int ret = 0;
	HWND hWnd = GetDesktopWindow();

	if( uType == MB_OK )
	{
		UIMessageBox(hWnd, Utf82Str(szText).c_str(), Utf82Str(szCaption).c_str(), _T("确定"), CMessageBoxUI::enMessageBoxTypeSucceed);
		return IDOK;
	}
	else if( uType == MB_OKCANCEL )
	{
		ret = UIMessageBox(hWnd, Utf82Str(szText).c_str(), Utf82Str(szCaption).c_str(), _T("确定,取消"), CMessageBoxUI::enMessageBoxTypeQuestion);
		if( ret == ID_MSGBOX_BTN )
			return IDOK;
		else
			return IDCANCEL;
 
	}
	else if( uType == MB_YESNO )
	{
		ret = UIMessageBox(hWnd, Utf82Str(szText).c_str(), Utf82Str(szCaption).c_str(), _T("是,否"), CMessageBoxUI::enMessageBoxTypeQuestion);
		if( ret == ID_MSGBOX_BTN )
			return IDYES;

		else 
			return IDNO;

	}
	
	return ret;
}

//
// IcrShowToast
//
void CALLBACK CIcrPlayer::IcrShowToast(const char *szText, int nDuration)
{
	HWND hWnd = FindWindow(_T("screenClass"), NULL);
	CScreenToast::Toast(hWnd, Utf82Str(szText).c_str(), false, nDuration);
}

//
// IcrEnable
//
void CALLBACK CIcrPlayer::IcrEnable()
{

}

//
// IcrGetExamType
//
char* CALLBACK CIcrPlayer::IcrGetExamType()
{
	tstring strQuestionType = IcrPlayer::GetInstance()->GetCurrentQuestionType();
	if( strQuestionType == _T("") )
		return NULL;
 
	char* pQuestionType = new char[MAX_PATH];
	strcpy(pQuestionType, Str2Utf8(strQuestionType).c_str());


	return pQuestionType;
}

//
// IcrSetButtonState
//
void CALLBACK CIcrPlayer::IcrSetButtonState(int nUIComponentId, int nButtonId, int nState)
{

	WRITE_LOG_LOCAL(_T("IcrSetButtonState ComponentID:%d,ButtonID:%d,State:%d"), nUIComponentId, nButtonId, nState);//方便排查工具栏隐藏问题2016.03.01 cws
	CScreenInstrumentUI* pScreenInstrumentUI = CScreenInstrumentUI::GetMainInstrument();
	if( pScreenInstrumentUI == NULL )
		return;

	if( nUIComponentId == ICR_UICOMPONET_NAVIGATOR )
	{
		CContainerUI* pLeftButtons = pScreenInstrumentUI->GetLeftActionButtons();
		CContainerUI* pRightButtons = pScreenInstrumentUI->GetRigthActionButtons();

		CContainerUI* pPagger = pScreenInstrumentUI->GetPagger();
 
		IcrPlayer::GetInstance()->SetButtonState(pLeftButtons, nState);
		IcrPlayer::GetInstance()->SetButtonState(pRightButtons, nState);
		IcrPlayer::GetInstance()->SetButtonState(pPagger, nState);
	}
	else if( nUIComponentId == ICR_UICOMPONET_TOOLBAR )
	{
		// the 2nd item is interactive course item
		CInstrumentItemUI* pItem = pScreenInstrumentUI->GetRightInstrumentItem(1);
		if( pItem == NULL )
			return;

		CInteractToolViewUI* pView = dynamic_cast<CInteractToolViewUI*>(pItem->GetInstrumentView());
		if( pView == NULL )
			return;

	
		CControlUI* pButton = NULL;
		switch( nButtonId )
		{
		case ICR_BUTTON_ID_ROSTER:
			pButton = pView->GetItem(_T("花名册"));
			break;

		case ICR_BUTTON_ID_PUSHSCREEN:
			pButton = pView->GetItem(_T("推屏"));
			break;

		case ICR_BUTTON_ID_EXAM:
			pButton = pView->GetItem(_T("发送任务"));
			break;

		case ICR_BUTTON_ID_SUMMARY:
			pButton = pView->GetItem(_T("课堂总结"));
			break;

		case ICR_BUTTON_ID_ASSIGN_HOMEWORK:
			pButton = pView->GetItem(_T("布置作业"));
			break;

		case ICR_BUTTON_ID_RACE_ANSWER:
			pButton = pView->GetItem(_T("抢答"));
			break;

		case ICR_BUTTON_ID_BAIKE:
			pButton = pView->GetItem(_T("百科"));
			break;

		case ICR_BUTTON_ID_INTERACTIVE_TOOL:
			{
				// left
				pButton = pScreenInstrumentUI->GetLeftInstrumentItem(1);
				IcrPlayer::GetInstance()->SetButtonState(pButton, nState);	
			
				pButton = pItem;
			}
			break;

		default:
			{
				if( nState == ICR_UI_STATE_VISIBLE )
					ShowWindow(pScreenInstrumentUI->GetHWND(), SW_SHOW);
					
				else if( nState == ICR_UI_STATE_INVISIBLE )
				{
					// need to paint once to hide interact item
					pView->SetVisible(false);
					SendMessage(pScreenInstrumentUI->GetHWND(), WM_PAINT, 0, 0);
					ShowWindow(pScreenInstrumentUI->GetHWND(), SW_HIDE);
				}

			}
			break;
		}

		IcrPlayer::GetInstance()->SetButtonState(pButton, nState);
	}
}

//
// IcrSendCmd
//
BOOL CIcrPlayer::IcrSendCmd(tstring strCategory, tstring strCmd, CStream *pStream)
{
	if( m_fnIcrSendCmd == NULL )
		return FALSE;

	pStream->ResetCursor();

	if( strCategory == _T("VR") )
	{
		tstring strPackageName = pStream->ReadString();
		tstring strActivityName = pStream->ReadString();

		Json::Value root;
		root["category"] = "VR";
		
		Json::Value content;
		content["op"]				= Str2Utf8(strCmd);
		content["package_name"]		= Str2Utf8(strPackageName);
		content["activity_name"]	= Str2Utf8(strActivityName);

		root["content"] = content.toStyledString();

		string str = root.toStyledString();
		m_fnIcrSendCmd(str.c_str());
	}

	return TRUE;
}

//
// Get window screen
//
void CIcrPlayer::GetWindowScreen(HWND hWnd, int nZoomValue, char** ppOutBuffer, int& nOutSize, RECT* pSrcRect /* = NULL*/)
{
	int nDestLeft = 0;
	int nDestTop = 0;
	int nSrcLeft = 0;
	int nSrcTop  = 0;


	HDC hdcWindow			= GetDC(hWnd);	
	int nSrcWidth			= GetSystemMetrics(SM_CXSCREEN);
	int nSrcHeight			= GetSystemMetrics(SM_CYSCREEN);
	int nBitPerPixel		= GetDeviceCaps(hdcWindow, BITSPIXEL);

	// test
	RECT rt;
	rt.left		= m_nSlideShowLeft;
	rt.top		= m_nSlideShowTop;
	rt.right	= m_nSlideShowLeft + m_nSlideShowWidth;
	rt.bottom	= m_nSlideShowTop + m_nSlideShowHeight;

	pSrcRect = &rt;

	if( pSrcRect != NULL )
	{
		nSrcLeft	= pSrcRect->left;
		nSrcTop		= pSrcRect->top;
		nSrcWidth	= pSrcRect->right - pSrcRect->left;
		nSrcHeight  = pSrcRect->bottom - pSrcRect->top;
	}

	CImage image;
	image.Create(nSrcWidth / nZoomValue, nSrcHeight / nZoomValue, nBitPerPixel);
	StretchBlt(image.GetDC(), nDestLeft, nDestTop, image.GetWidth(), image.GetHeight(), hdcWindow, nSrcLeft, nSrcTop, nSrcWidth, nSrcHeight, SRCCOPY|CAPTUREBLT);

	ReleaseDC(NULL, hdcWindow);
	image.ReleaseDC();

	nOutSize = 0;
	*ppOutBuffer = NULL;
	IStream* pOutStream = NULL;

	if( CreateStreamOnHGlobal(NULL, TRUE, &pOutStream) != S_OK)
		return;

	image.Save(pOutStream, Gdiplus::ImageFormatJPEG);

	HGLOBAL hOutGlobal = NULL;
	GetHGlobalFromStream(pOutStream, &hOutGlobal);

	LPBYTE pBits = (LPBYTE)GlobalLock(hOutGlobal);
	nOutSize = (DWORD)GlobalSize(hOutGlobal);

	*ppOutBuffer = new char[nOutSize];
	memcpy(*ppOutBuffer, pBits, nOutSize);

	GlobalUnlock(hOutGlobal);
	pOutStream->Release();

}

//
// set button state
//
void CIcrPlayer::SetButtonState( CControlUI* pControl, int nState )
{
	if( pControl == NULL )
		return;

	switch( nState )
	{
	case ICR_UI_STATE_VISIBLE:
		pControl->SetVisible(true);
		break;

	case ICR_UI_STATE_INVISIBLE:
		pControl->SetVisible(false);
		break;

	case ICR_UI_STATE_ENABLE:
		pControl->SetEnabled(true);
		break;

	case ICR_UI_STATE_DISABLE:
		pControl->SetEnabled(false);
		break;
	}
}

tstring CIcrPlayer::GetCurrentQuestionType()
{
	// check subject tool whether it is exist
	HWND hWnd = FindWindow(_T("NdCefOSRWindowsClass"), NULL);
	if( hWnd != NULL )
		return _T("sync");

	if( m_nCurrentPageIndex <= 0 || m_nCurrentPageIndex > m_vecQuestionPaths.size() )
		return _T("");

	tstring strQuestionPath = m_vecQuestionPaths[m_nCurrentPageIndex-1];
	if( strQuestionPath == _T("") )
		return _T("");

	// json
	tstring strJsonPath = strQuestionPath;
	strJsonPath += _T("\\metadata.json");

	tstring strGuid;
	tstring strQuestionType;

	NDCloudLocalQuestionManager::GetInstance()->GetQuestionInfo(strJsonPath, strGuid, strQuestionType);
	return strQuestionType;
}

//
// check whether crashed happened on last running
//
BOOL CIcrPlayer::CheckCrash(tstring& strPPTFilePath, int& nPageIndex)
{
	tstring strConfigFilePath = GetLocalPath();
	strConfigFilePath += _T("\\bin\\plugins\\icr\\recovery.ini");

	BOOL bRunning = GetPrivateProfileInt(_T("Config"), _T("Running"), 0, strConfigFilePath.c_str());
	if( !bRunning )
		return TRUE;

	// crash happened then restore icrplayer
	TCHAR szPPTFilePath[MAX_PATH];
	GetPrivateProfileString(_T("Config"), _T("PPTFilePath"), _T(""), szPPTFilePath, MAX_PATH, strConfigFilePath.c_str());		

	
	nPageIndex = GetPrivateProfileInt(_T("Config"), _T("PageIndex"), 1, strConfigFilePath.c_str());
	return TRUE;
}

 