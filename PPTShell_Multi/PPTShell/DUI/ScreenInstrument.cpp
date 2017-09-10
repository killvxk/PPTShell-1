#include "StdAfx.h"
#include "ScreenInstrument.h"
#include "DUI/DUICommon.h"
#include "DUI/InstrumentItem.h"
#include "DUI/ArrowInstrumentItem.h"
#include "DUI/CleanInkInstrumentItem.h"
#include "DUI/EncourageInstrumentItem.h"
#include "DUI/EraseInstrumentItem.h"
#include "DUI/ExitInstrumentItem.h"
#include "DUI/InteractInstrumentItem.h"
#include "DUI/PencelInstrumentItem.h"
#include "DUI/SubjectInstrumentItem.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "ExternPanelManager.h"
#include "DUI/SpotLightUI.h"
#include "DUI/Shape.h"
#include "DUI/PanelView.h"
#include "DUI/BlackBoarder.h"
#include "GUI/MainFrm.h"
#include "Plugins/Icr/IcrPlayer.h"
#include "NDCloud/NDCloudUser.h"
#include "ShowIcrFailInfo.h"
#include "Config.h"
#include "MobilePenView.h"

enum InstrumentType
{
	eSubjectTool,
	eInteractTool,
	eEncourage,
	eArrow,
	ePencel,
	eErase,
	eCleanInk,
	eExitFullScreen,
};

#define TimerId_TopMost			1000
#define TimerId_Adjust			1001
#define TimerId_CountDown		1002
#define TimerId_UpdatePos		1003



void SetTaskBar(int istate)         
{      
	HWND  trayHwnd = ::FindWindow(_T("Shell_TrayWnd"),NULL); 
	::ShowWindow(trayHwnd, istate);
	HWND hStar = ::FindWindow(_T("Button"),NULL);  
	::ShowWindow(hStar, istate); 
}


CScreenInstrumentUI* CScreenInstrumentUI::m_pThis = NULL;
CScreenInstrumentUI* CScreenInstrumentUI::GetMainInstrument()
{
	return m_pThis;
}



CScreenInstrumentUI::CScreenInstrumentUI(void)
{
	m_nCurrentType		= -1;

	m_layLeftTools		= NULL;
	m_layLeftHover		= NULL;
	m_pLeftTools		= NULL;
	m_layRightTools		= NULL;
	m_layRightHover		= NULL;
	m_pRightTools		= NULL;
	m_layToolTab		= NULL;
	m_nPageCount		= 0;
	m_pLeftPageLabel	= NULL;
	m_pRightPageLabel	= NULL;
	m_pLeftActionButtons  = NULL;
	m_pRightActionButtons = NULL;


	m_pScreenThumbnails = new CScreenPaggerUI;
	m_pSpotLightWindow  = new CSpotLightWindowUI;
	OnEvent(EVT_SCREEN_PRE_NEXT_ENABLE,MakeEventDelegate(this,&CScreenInstrumentUI::OnScreenPreNextEnable));

}

CScreenInstrumentUI::~CScreenInstrumentUI(void)
{
	SetTaskBar(true);//即使PPT崩溃后也能把任务栏显示2015.12.25 cws
	if (m_layToolTab)
	{
		m_PaintManager.KillTimer(m_layToolTab, TimerId_TopMost);
		m_PaintManager.KillTimer(m_layToolTab, TimerId_Adjust);
	}
	CancelEvent(EVT_SCREEN_PRE_NEXT_ENABLE,MakeEventDelegate(this,&CScreenInstrumentUI::OnScreenPreNextEnable));
}

DuiLib::CDuiString CScreenInstrumentUI::GetSkinFolder()
{
	return CDuiString(_T("skins"));
}

DuiLib::CDuiString CScreenInstrumentUI::GetSkinFile()
{
	return _T("ScreenTool\\Instrument.xml");
}


CControlUI* CScreenInstrumentUI::CreateControl( LPCTSTR pstrClass )
{
	if (_tcsicmp(_T("InstrumentItem"), pstrClass) == 0)
	{
		switch(m_nCurrentType)
		{
		case eSubjectTool:
			{
				return new CSubjectInstrumentItemUI;
			}
			break;
		case eInteractTool:
			{
				return new CInteractInstrumentItemUI;
			}
			break;
		case eEncourage:
			{
				return new CEncourageInstrumentItemUI;
			}
			break;
		case eArrow:
			{
				return new CArrowInstrumentItemUI;
			}
			break;
		case ePencel:
			{
				return new CPencelInstrumentItemUI;
			}
			break;
		case eErase:
			{
				return new CEraseInstrumentItemUI;
			}
			break;
		case eCleanInk:
			{
				return new CCleanInkInstrumentItemUI;
			}
			break;
		case eExitFullScreen:
			{
				return new CExitInstrumentItemUI();
			}
			break;
		}
	}
	else if (_tcsicmp(_T("ScreenThumbnails"), pstrClass) == 0)
	{
		return m_pScreenThumbnails;
	}
	else if (_tcsicmp(_T("SpotLightWindowUI"), pstrClass) == 0)
	{
		return m_pSpotLightWindow;
	}
	else if (_tcsicmp(_T("SpotLightUI"), pstrClass) == 0)
	{
		return m_pSpotLightWindow->GetSpotLightUI();
	}

	return __super::CreateControl(pstrClass);
}

LPCTSTR CScreenInstrumentUI::GetWindowClassName( void ) const
{
	return _T("CScreenInstrumentUI");
}


void CScreenInstrumentUI::OnFinalMessage( HWND hWnd )
{
	__super::OnFinalMessage(hWnd);
	delete this;
	m_pThis = NULL;
}


LRESULT CScreenInstrumentUI::OnClose( UINT , WPARAM , LPARAM , BOOL& bHandled )
{
	return FALSE;
}

CDialogBuilder CScreenInstrumentUI::m_ScreenThumbnailBuilder;
CDialogBuilder CScreenInstrumentUI::m_SpotLightBuilder;
void CScreenInstrumentUI::InitWindow()
{
	__super::InitWindow();
	m_pThis	= this;

	//add bottom thumbnail view
	if (!m_ScreenThumbnailBuilder.GetMarkup()->IsValid())
	{
		m_ScreenThumbnailBuilder.Create(_T("ScreenTool\\ScreenThumbnailsStyle.xml"), NULL, this);
	}
	else
	{
		m_ScreenThumbnailBuilder.Create(this);
	}

	m_pScreenThumbnails->AddPageListener(this);
	m_pScreenThumbnails->SetPageCount(m_nPageCount);
	CContainerUI* pMainContainer = static_cast<CContainerUI*>(m_PaintManager.FindControl(_T("main")));
	if (pMainContainer)
	{
		pMainContainer->Add(m_pScreenThumbnails);
	}


	m_layLeftTools		= static_cast<CContainerUI*>(m_PaintManager.FindControl(_T("left_tool_layout")));
	m_pLeftTools		= static_cast<CListUI*>(m_layLeftTools->FindSubControl(_T("tools")));
	m_layLeftHover		= static_cast<CContainerUI*>(m_PaintManager.FindControl(_T("left_hover_layout")));
	m_layRightTools		= static_cast<CContainerUI*>(m_PaintManager.FindControl(_T("right_tool_layout")));
	m_pRightTools		= static_cast<CListUI*>(m_layRightTools->FindSubControl(_T("tools")));
	m_layRightHover		= static_cast<CContainerUI*>(m_PaintManager.FindControl(_T("right_hover_layout")));
	m_pLeftPageLabel	= (CLabelUI*)m_layLeftTools->FindSubControl(_T("ppt_page"));
	m_pRightPageLabel	= (CLabelUI*)m_layRightTools->FindSubControl(_T("ppt_page"));


	m_layToolTab		= static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("tool_tab")));
	m_pTabLeftPadding	= static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("tab_left_padding")));
	m_pTabRightPadding	= static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("tab_right_padding")));

	m_pLeftActionButtons	= static_cast<CContainerUI*>(m_PaintManager.FindControl(_T("left_action_buttons")));
	m_pRightActionButtons	= static_cast<CContainerUI*>(m_PaintManager.FindControl(_T("right_action_buttons")));

	m_layCountDown		= static_cast<CContainerUI*>(m_PaintManager.FindControl(_T("lay_count_down")));
	m_ctrlSeconds		= static_cast<CControlUI*>(m_PaintManager.FindControl(_T("count_down_second")));


	//full screen and update pos
	UpdateInstrumentPos();


	m_pSpotLightLayout = dynamic_cast<CVerticalLayoutUI *>(m_PaintManager.FindControl(_T("SpotLightLayout")));

	if (!m_SpotLightBuilder.GetMarkup()->IsValid())
	{
		m_SpotLightBuilder.Create(_T("SpotLightWindow\\SpotLightWindow.xml"), NULL, this);
	}
	else
	{
		m_SpotLightBuilder.Create(this);
	}

 	CRect rect;
	::GetWindowRect(m_hWnd,&rect);
 	m_pSpotLightWindow->SetRect(rect);

	m_pSpotLightLayout->Add(m_pSpotLightWindow);


	//create instrument items
	CreateInstrumentItems(
		m_pLeftTools,
		m_layToolTab,
		m_pRightTools,
		true
		);
	CreateInstrumentItems(
		m_pRightTools,
		m_layToolTab,
		m_pLeftTools,
		false
		);

	CHorizontalLayoutUI* pInstrumentLayout = dynamic_cast<CHorizontalLayoutUI *>(m_PaintManager.FindControl(_T("InstrumentLayout")));
	TraversalControls(pInstrumentLayout);
// 	TraversalControls(m_layRightHover);
// 	TraversalControls(m_layLeftHover);

	

	m_layToolTab->OnEvent += MakeDelegate(this, &CScreenInstrumentUI::OnToolTabEvent);
}

bool CScreenInstrumentUI::OnToolTabEvent(void* pObj)
{
	TEventUI* pEvent = (TEventUI*)pObj;
	if (pEvent->Type == UIEVENT_TIMER)
	{
		if (pEvent->wParam == TimerId_TopMost)
		{
			::SetWindowPos(GetHWND(), HWND_DESKTOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		}
		else if (pEvent->wParam == TimerId_Adjust)
		{
			//had been painted
			CListUI* pList = NULL;
			CContainerUI* pLayTool = NULL;
			if(m_pLeftTools->GetPos().right > 0 )
			{
				pList = m_pLeftTools;
				pLayTool = m_layLeftTools;
			}
			else if(m_pRightTools->GetPos().right > 0)
			{
				pList = m_pRightTools;
				pLayTool = m_layRightTools;
			}

			if (pList)
			{
				for (int i = 0; i < pList->GetCount(); ++i)
				{
					CInstrumentItemUI* pItem = (CInstrumentItemUI*)pList->GetItemAt(i);
					pItem->OnAdjustView(pLayTool->GetPos());
				}
				m_PaintManager.KillTimer(m_layToolTab, TimerId_Adjust);
			}
			
		}
		else if (pEvent->wParam == TimerId_CountDown)
		{
			m_nSeconds--;
			if (m_nSeconds >= 1)
			{
				TCHAR szSecond[32] = {0};
				_stprintf_s(szSecond, _T("ScreenTool\\countdown\\%d.png"), m_nSeconds);
				m_ctrlSeconds->SetBkImage(szSecond);
			}
			else
			{
				m_layCountDown->SetVisible(false);
				m_PaintManager.KillTimer(m_layToolTab, TimerId_CountDown);

				CButtonUI* pStopBtn		= (CButtonUI*)m_PaintManager.FindControl("vr_stop");
				CButtonUI* pPlayBtn	= (CButtonUI*)m_PaintManager.FindControl("vr_play");
				
				pStopBtn->SetVisible(true);
				pPlayBtn->SetVisible(false);

				tstring strParam = GetPagger()->GetScanner()->GetVRPathAtCurrentPage();
				vector<tstring> vecParams = SplitString(strParam, strParam.length(), _T('|'), true);
				if( vecParams.size() < 5)
					return true;

				tstring strExePath		= vecParams[0];
				tstring strPackageName	= vecParams[1];
				tstring strActivityName = vecParams[2];
				tstring strAppVersion	= vecParams[3];
				tstring strtype	= vecParams[4];

				if (strtype=="VRTYPE_EXE"){
					CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
					pMainFrame->Show3DAmination(strExePath);
				}if (strtype=="VRTYPE_VIDEO"){
					CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
					tstring nVrPlayer = GetCoursePlayerPath();
					nVrPlayer += _T("\\VRMediaPlayer.exe");
					tstring nParam = strExePath;
					//nParam += strExePath;
					//nParam += _T("\"");
					pMainFrame->ShowVRMediaPlayer(nVrPlayer,nParam);
				}

				CStream stream(256);
				stream.WriteString(strPackageName);
				stream.WriteString(strActivityName);

				IcrPlayer::GetInstance()->IcrSendCmd(_T("VR"), _T("play"), &stream);
			}
		}
		else if (pEvent->wParam == TimerId_UpdatePos)
		{
			m_PaintManager.KillTimer(m_layToolTab, TimerId_UpdatePos);
			UpdateInstrumentPos();
		}
	}
	return true;
}


void CScreenInstrumentUI::ShowInstrument( HWND hParent, int nPageCount )
{
	::SetPointerTypeByThread(ppSlideShowPointerArrow);

	if (GetHWND() == NULL)
	{
		m_nPageCount = nPageCount;
#ifdef _DEBUG
		Create(hParent, GetWindowClassName(), WS_POPUP | WS_VISIBLE,  WS_EX_TOOLWINDOW | WS_EX_TOPMOST);
#else
		Create(hParent, GetWindowClassName(), WS_POPUP | WS_VISIBLE,  WS_EX_TOOLWINDOW | WS_EX_TOPMOST);
#endif
		
	}
	SetTaskBar(false);
	::SetFocus(GetHWND());

	m_PaintManager.SetTimer(m_layToolTab, TimerId_Adjust, 300);
	m_PaintManager.SetTimer(m_layToolTab, TimerId_TopMost, 2000);
	if (g_Config::GetInstance()->GetModuleVisible(MODULE_ND_ICRPLAY))
	{
		DWORD dwUserID = NDCloudUser::GetInstance()->GetUserId();
		if (dwUserID == 0)//未登陆不弹提示，直接进入自由授课模式
		{
			NDCloudUser::GetInstance()->SetFreeMode(1);
		}
		else
		{
			// class
			tstring strClassId = NDCloudUser::GetInstance()->GetClassGuid(); 
			if (strClassId == _T(""))//没有班级信息，直接进入自由授课模式
			{
				NDCloudUser::GetInstance()->SetFreeMode(1);
			}
			else
			{
				// students
				tstring strStudentInfo = NDCloudUser::GetInstance()->GetStudentInfo();
				if (!NDCloudUser::GetInstance()->CheckItemsSize(strStudentInfo, "items"))//有班级信息但是没有学生信息，也进入自由授课模式
				{
					NDCloudUser::GetInstance()->SetFreeMode(1);
				} 
				else
				{
					if (!NDCloudUser::GetInstance()->GetIsLoadAllTeacherInfo())
					{
						ShowIcrFailInfoUI::GetInstance()->ShowLocalWindows(this->GetHWND());
					}
				}

			}

		}
	}
}

void CScreenInstrumentUI::CloseInstrument()
{
	::DestroyWindow(this->GetHWND());
	::SetForegroundWindow(AfxGetMainWnd()->GetSafeHwnd());
	MobilePenViewUI::GetInstance()->CloseMobilePenBoard();//关闭手机端的画笔窗口
	//SetTaskBar(true);
	
}

void CScreenInstrumentUI::OnBtnLeftHover( TNotifyUI& msg )
{
	m_layLeftHover->SetVisible(false);
	m_layRightHover->SetVisible(true);


	m_layLeftTools->SetVisible(true);
	m_layRightTools->SetVisible(false);


	m_layToolTab->SetVisible(true);
	m_pTabLeftPadding->SetVisible(false);
	m_pTabRightPadding->SetVisible(true);

	::SetForegroundWindow(CPPTController::GetSlideShowViewHwnd());
	::SetFocus(CPPTController::GetSlideShowViewHwnd());

}

void CScreenInstrumentUI::OnBtnRightHover( TNotifyUI& msg )
{
	m_layLeftHover->SetVisible(true);
	m_layRightHover->SetVisible(false);

	m_layLeftTools->SetVisible(false);
	m_layRightTools->SetVisible(true);

	m_layToolTab->SetVisible(true);
	m_pTabLeftPadding->SetVisible(true);
	m_pTabRightPadding->SetVisible(false);

	::SetForegroundWindow(CPPTController::GetSlideShowViewHwnd());
	::SetFocus(CPPTController::GetSlideShowViewHwnd());

}

void CScreenInstrumentUI::OnBtnLeftPackup( TNotifyUI& msg )
{
	m_layLeftHover->SetVisible(true);
	m_layLeftTools->SetVisible(false);
	m_layToolTab->SetVisible(false);


	if (m_pLeftTools)
	{
		CInstrumentItemUI* pItem = (CInstrumentItemUI*)m_pLeftTools->GetItemAt(m_pLeftTools->GetCurSel());
		if (pItem)
		{
			pItem->OnUnSelected();
		}
	}

	::SetForegroundWindow(CPPTController::GetSlideShowViewHwnd());
	::SetFocus(CPPTController::GetSlideShowViewHwnd());
}

void CScreenInstrumentUI::OnBtnRightPackup( TNotifyUI& msg )
{
	m_layRightHover->SetVisible(true);
	m_layRightTools->SetVisible(false);
	m_layToolTab->SetVisible(false);


	if (m_pRightTools)
	{
		CInstrumentItemUI* pItem = (CInstrumentItemUI*)m_pRightTools->GetItemAt(m_pRightTools->GetCurSel());
		if (pItem)
		{
			pItem->OnUnSelected();
		}
	}

	::SetForegroundWindow(CPPTController::GetSlideShowViewHwnd());
	::SetFocus(CPPTController::GetSlideShowViewHwnd());

}

void CScreenInstrumentUI::OnToolItemChanged( TNotifyUI& msg )
{
	CInstrumentItemUI* pCurrentItem = dynamic_cast<CInstrumentItemUI*>(msg.pSender);
	if (!pCurrentItem)
	{
		return;
	}

	pCurrentItem->ShowView();
	pCurrentItem->OnSelected();
}

void CScreenInstrumentUI::CreateInstrumentItems( CListUI* pList, CContainerUI* pViewParent, CListUI* pReleatedList, bool bLeft )
{
	TCHAR szInstrumentText[][16] = {
		_T("学科工具"),
		_T("互动工具"),
		_T("鼓励"),
		_T("箭头"),
		_T("画笔"),
		_T("橡皮擦"),
		_T("清除墨迹"),
		_T("退出全屏"),

	};

	TCHAR szInstrumentIcon[][16] = {
		_T("icon_xkgj.png"),
		_T("icon_hdgj.png"),
		_T("icon_gl.png"),
		_T("icon_jt.png"),
		_T("icon_hb.png"),
		_T("icon_xpc.png"),
		_T("icon_qcmj.png"),
		_T("icon_tcqp.png"),
	};

	TCHAR	szIconDir[] = _T("ScreenTool\\icons\\");
	tstring	strIconPath;
	for (int i = 0; i < _countof(szInstrumentText); ++i)
	{
		strIconPath = szIconDir;
		strIconPath += szInstrumentIcon[i];

		m_nCurrentType = i;
		CInstrumentItemUI*	pNewItem	= (CInstrumentItemUI*)CreateItem();
		pList->Add(pNewItem);

		pNewItem->SetTitle(szInstrumentText[i]);
		pNewItem->SetIcon(strIconPath.c_str());
		pNewItem->SetTag((UINT_PTR)i);
		pNewItem->SetReleatedInstruments(pReleatedList);
		pNewItem->SetKeepSide(bLeft);

		CContainerUI* pTab = (CContainerUI*)pViewParent->GetItemAt(i);
		if (pTab == NULL)
		{
			pTab = new CContainerUI();
			pViewParent->AddAt(pTab, i);
		}
		
		pNewItem->SetTabContainer(pTab);
		pNewItem->SetTab((CTabLayoutUI*)pViewParent);
		//for tab
		if(pNewItem->HasView())
		{
			pNewItem->CreateInstrumentView(pTab);
			
		}

		
	}
}

DuiLib::CDialogBuilder CScreenInstrumentUI::m_ItemBuilder;
CControlUI* CScreenInstrumentUI::CreateItem()
{
	if (!m_ItemBuilder.GetMarkup()->IsValid())
	{
		return m_ItemBuilder.Create(_T("ScreenTool\\ItemStyle.xml"), NULL, this);
	}
	
	return m_ItemBuilder.Create(this);

}



void CScreenInstrumentUI::UpdatePageUI( CLabelUI* pLabel, int nIndex )
{
	if (!pLabel)
	{
		return;
	}

	TCHAR szForamt[]	= _T("{f 200100}{c #1BB0B6}%d{/c}{/f}{f 150100}/%d{/f}");
	TCHAR szText[260]	= {0};
	_stprintf_s(szText, szForamt, nIndex, this->GetPagger()->GetPageCount());

	pLabel->SetText(szText);

}

void CScreenInstrumentUI::OnToolItemSelect( TNotifyUI& msg )
{
	//pre selected
	if (msg.lParam >= 0)
	{
		CListUI* pList =dynamic_cast<CListUI*>(msg.pSender);
		if (pList)
		{
			CInstrumentItemUI* pItem = (CInstrumentItemUI*)pList->GetItemAt(msg.lParam);
			pItem->OnUnSelected();

		}
	}
}

void CScreenInstrumentUI::OnPageChangeBefore()
{

}

void CScreenInstrumentUI::OnPageChanged(int nPageIndex)
{
	UpdatePageUI(m_pLeftPageLabel, nPageIndex);
	UpdatePageUI(m_pRightPageLabel, nPageIndex);

}

CScreenPaggerUI* CScreenInstrumentUI::GetPagger()
{
	return m_pScreenThumbnails;
}	

void CScreenInstrumentUI::OnPageScanneded()
{
	CButtonUI* pPalyBtn = (CButtonUI*)m_PaintManager.FindControl("vr_play");
	CButtonUI* pStopBtn = (CButtonUI*)m_PaintManager.FindControl("vr_stop");


	if (GetPagger()->GetScanner()->HasVRAtCurrentPage())
	{
		if (pPalyBtn && pStopBtn)
		{
			pPalyBtn->SetVisible(true);
			pStopBtn->SetVisible(false);
		}
	}
	else
	{
		if (pPalyBtn && pStopBtn)
		{
			pPalyBtn->SetVisible(false);
			pStopBtn->SetVisible(false);
		}
	}
}

CInstrumentItemUI* CScreenInstrumentUI::GetLeftInstrumentItem(int nIndex)
{
	CInstrumentItemUI* pItem = (CInstrumentItemUI*)m_pLeftTools->GetItemAt(nIndex);
	return pItem;
}

CInstrumentItemUI* CScreenInstrumentUI::GetRightInstrumentItem(int nIndex)
{
	CInstrumentItemUI* pItem = (CInstrumentItemUI*)m_pRightTools->GetItemAt(nIndex);
	return pItem;
}

CContainerUI* CScreenInstrumentUI::GetLeftActionButtons()
{
	return m_pLeftActionButtons;
}

CContainerUI* CScreenInstrumentUI::GetRigthActionButtons()
{
	return m_pRightActionButtons;
}

bool CScreenInstrumentUI::OnFilterEvent( void* pEvent )
{
	TEventUI event = *(TEventUI*)pEvent;
	if(event.Type == UIEVENT_BUTTONDOWN)
	{
		int a = 0;
	}
	m_pSpotLightWindow->Event(event);

	return true;
}

void CScreenInstrumentUI::TraversalControls(CControlUI * pControl)
{
	if( pControl->GetName() == _T("left_action_buttons") || pControl->GetName() == _T("right_action_buttons")
		|| pControl->GetName() == _T("right_tool_layout") ||  pControl->GetName() == _T("left_tool_layout"))
	{
		return;
	}
	pControl->OnEvent += MakeDelegate(this, &CScreenInstrumentUI::OnFilterEvent);
	IContainerUI * pContainer = static_cast<IContainerUI*>(pControl->GetInterface(_T("IContainer")));
	if( pControl->GetName() == _T("tool_tab"))
	{
		int nCount = pContainer->GetCount();
		for(int i = 0; i < nCount ; i++)
		{
			pContainer->GetItemAt(i)->OnEvent += MakeDelegate(this, &CScreenInstrumentUI::OnFilterEvent);
		}
		return;
	}
	if( pContainer != NULL ) 
	{
		int nCount = pContainer->GetCount();
		for(int i = 0; i < nCount ; i++)
		{
			TraversalControls(pContainer->GetItemAt(i));
		}
	}
}

CSpotLightWindowUI* CScreenInstrumentUI::GetSpotLightWindow()
{
	return m_pSpotLightWindow;
}

void CScreenInstrumentUI::UpdateInstrumentPos()
{
	MONITORINFO oMonitor = {0};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(CPPTController::GetSlideShowViewHwnd(), MONITOR_DEFAULTTONEAREST), &oMonitor);

	int nWidth	= oMonitor.rcMonitor.right - oMonitor.rcMonitor.left;
	int nHeight	= oMonitor.rcMonitor.bottom - oMonitor.rcMonitor.top;
	::MoveWindow(m_hWnd, oMonitor.rcMonitor.left, oMonitor.rcMonitor.top, nWidth, nHeight, TRUE);
}

LRESULT CScreenInstrumentUI::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if (uMsg == WM_DISPLAYCHANGE)
	{
		UpdateInstrumentPos();
		m_PaintManager.SetTimer(m_layToolTab, TimerId_UpdatePos, 1500);
		//update pos again
	}
	else if(uMsg == WM_GESTURE)
	{	
		GESTUREINFO gi;  

		ZeroMemory(&gi, sizeof(GESTUREINFO));

		gi.cbSize = sizeof(GESTUREINFO);

		HMODULE hUserDll = LoadLibrary(_T("user32.dll"));
		pFnGetGestureInfo pGetGestureInfo = (pFnGetGestureInfo)::GetProcAddress(hUserDll, _T("GetGestureInfo"));
		if(pGetGestureInfo)
		{
			BOOL bResult  = pGetGestureInfo((HGESTUREINFO)lParam, &gi);

	//		WRITE_LOG_LOCAL("WM_GESTURE id:%d, param:%d",gi.dwID, gi.ullArguments);

			BOOL bHandled = FALSE;

			if (bResult){
				// now interpret the gesture
				switch (gi.dwID){
				   case GID_BEGIN:
					   m_pSpotLightWindow->SetGesture(GID_BEGIN, 0);
					   break;
				   case GID_END:
					   m_pSpotLightWindow->SetGesture(GID_END, 0);
					   break;
				   case GID_ZOOM:
					   // Code for zooming goes here     
					   m_pSpotLightWindow->SetGesture(GID_ZOOM, gi.ullArguments);
					   bHandled = TRUE;
					   break;
				   case GID_PAN:
					   // Code for panning goes here
					   bHandled = TRUE;
					   break;
				   case GID_ROTATE:
					   // Code for rotation goes here
					   bHandled = TRUE;
					   break;
				   case GID_TWOFINGERTAP:
					   // Code for two-finger tap goes here
					   bHandled = TRUE;
					   break;
				   case GID_PRESSANDTAP:
					   // Code for roll over goes here
					   bHandled = TRUE;
					   break;
				   default:
					   // A gesture was not recognized
			   break;
				}
			}else{
				DWORD dwErr = GetLastError();
				if (dwErr > 0){
					//MessageBoxW(hWnd, L"Error!", L"Could not retrieve a GESTUREINFO structure.", MB_OK);
				}
			}

			if (bHandled){
				return 0;
			}
		}
		
	}
	else if(uMsg == WM_SHOWMOBILEPEN)
	{
		MobilePenViewUI::GetInstance()->ShowMobilePenBoard();
	}
	else if(uMsg == WM_HIDEMOBILEPEN)
	{
		MobilePenViewUI::GetInstance()->CloseMobilePenBoard();
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}

void CScreenInstrumentUI::SetPreNextButtonEnable( bool enable )
{
	if(m_pLeftActionButtons)
	{
		CControlUI* pre_action = m_pLeftActionButtons->FindSubControl("pre_action");
		if(pre_action)
		{
			pre_action->SetEnabled(enable);
		}
		CControlUI* next_action = m_pLeftActionButtons->FindSubControl("next_action");
		if(next_action)
		{
			next_action->SetEnabled(enable);
		}
	}
	if(m_pRightActionButtons)
	{
		CControlUI* pre_action = m_pRightActionButtons->FindSubControl("pre_action");
		if(pre_action)
		{
			pre_action->SetEnabled(enable);
		}
		CControlUI* next_action = m_pRightActionButtons->FindSubControl("next_action");
		if(next_action)
		{
			next_action->SetEnabled(enable);
		}
	}
}

bool CScreenInstrumentUI::OnScreenPreNextEnable( void* pObj )
{
	if(pObj)
	{
		TEventNotify* pNotify = (TEventNotify*)pObj;
		if(pNotify)
		{
			bool enable = (bool)pNotify->wParam;
			SetPreNextButtonEnable(enable);
		}
	}
	return true;
}

void CScreenInstrumentUI::HandleDrawMessage( LPCSTR lpEventName, LPCSTR lptcsEventData, tstring& strResult )
{
// 	strResult = "";
// 	if (_tcsicmp(lpEventName, "drawsetting") == 0)
// 	{
// 		DWORD dwColor = CBlackBoarderUI::GetInstance()->GetPenColor();
// 
// 		CHAR szColor[32] = {0};
// 		sprintf_s(szColor, "#%d%d%d", GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
// 
// 		Json::Value color(szColor);
// 		Json::Value width(4);
// 		Json::Value result;
// 		result["color"] = color;
// 		result["width"] = width;
// 
// 		strResult = result.toStyledString();
// 	}
// 	else if (_tcsicmp(lpEventName, "drawshape") == 0)
// 	{
// 		
// 
// 	}



}

void CScreenInstrumentUI::OnBtnPlayVR( TNotifyUI& msg )
{
// 	m_PaintManager.KillTimer(m_layToolTab, COUNT_DOWN_TIMER_ID);
// 	m_PaintManager.SetTimer(m_layToolTab, COUNT_DOWN_TIMER_ID, 1000);
// 
// 	m_nSeconds = 5;
// 
// 	m_layCountDown->SetVisible(true);
// 	TCHAR szSecond[32] = {0};
// 	_stprintf_s(szSecond, _T("ScreenTool\\countdown\\%d.png"), m_nSeconds);
// 	m_ctrlSeconds->SetBkImage(szSecond);

	CButtonUI* pStopBtn		= (CButtonUI*)m_PaintManager.FindControl("vr_stop");
	CButtonUI* pPlayBtn	= (CButtonUI*)m_PaintManager.FindControl("vr_play");

	pStopBtn->SetVisible(true);
	pPlayBtn->SetVisible(false);

	tstring strParam = GetPagger()->GetScanner()->GetVRPathAtCurrentPage();
	vector<tstring> vecParams = SplitString(strParam, strParam.length(), _T('|'), true);
	if( vecParams.size() < 5)
		return;

	tstring strExePath		= vecParams[0];
	tstring strPackageName	= vecParams[1];
	tstring strActivityName = vecParams[2];
	tstring strAppVersion	= vecParams[3];
	tstring strtype	= vecParams[4];

	if (strtype=="VRTYPE_EXE"){
		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		pMainFrame->Show3DAmination(strExePath);
	}if (strtype=="VRTYPE_VIDEO"){
		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		tstring nVrPlayer = GetCoursePlayerPath();
		nVrPlayer += _T("\\VRMediaPlayer.exe");
		tstring nParam = strExePath;//_T("\"");
		//nParam += strExePath;
		//nParam += _T("\"");
		pMainFrame->ShowVRMediaPlayer(nVrPlayer,nParam);
	}

	CStream stream(256);
	stream.WriteString(strPackageName);
	stream.WriteString(strActivityName);

	IcrPlayer::GetInstance()->IcrSendCmd(_T("VR"), _T("play"), &stream);
}

void CScreenInstrumentUI::OnBtnStopVR( TNotifyUI& msg )
{
// 	m_PaintManager.KillTimer(m_layToolTab, COUNT_DOWN_TIMER_ID);
// 	m_layCountDown->SetVisible(false);

	CButtonUI* pBtn = (CButtonUI*)m_PaintManager.FindControl("vr_play");
	if (pBtn)
	{
		pBtn->SetVisible(true);
	}
	msg.pSender->SetVisible(false);

	tstring strParam = GetPagger()->GetScanner()->GetVRPathAtCurrentPage();
	vector<tstring> vecParams = SplitString(strParam, strParam.length(), _T('|'), true);
	if( vecParams.size() < 5)
		return;

	tstring strExePath		= vecParams[0];
	tstring strPackageName	= vecParams[1];
	tstring strActivityName = vecParams[2];
	tstring strAppVersion	= vecParams[3];
	tstring strtype	= vecParams[4];

	if (strtype=="VRTYPE_EXE"){
		HWND hWnd = FindWindow(_T("screenClass"), NULL);
		if( hWnd == NULL )
			return;

		hWnd = FindWindowEx(hWnd, NULL, _T("UnityWndClass"), NULL);
		if( hWnd == NULL )
			return;

		::SendMessage(hWnd, WM_CLOSE, 0, 0);
	}else if (strtype=="VRTYPE_VIDEO"){
		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		pMainFrame->CloseVRMediaPlayer();
	}

	CStream stream(256);
	stream.WriteString(_T(""));
	stream.WriteString(_T(""));

	IcrPlayer::GetInstance()->IcrSendCmd(_T("VR"), _T("stop"), &stream);
}


