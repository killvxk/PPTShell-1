#include "stdafx.h"
#include "DUICommon.h"
#include "GuideDialog.h"
#include "Util/Util.h"
#include "EventCenter/EventDefine.h"
#include "DUI/UpdateDialog.h"

CGuideDialogUI* CGuideDialogUI::m_pInstance = NULL;
CGuideDialogUI* CGuideDialogUI::GetInstance()
{
	if (!m_pInstance)
	{
		m_pInstance = new CGuideDialogUI;
	}
	return m_pInstance;
}

CGuideDialogUI::CGuideDialogUI()
{
	m_vecGuideChapter.clear();
	m_bDefaultSetting = false;
	m_bLoadChapterTree = false;

	m_CategoryTree = NULL;
	m_ChapterTree  = NULL;
	m_pChapterMask = NULL;

	m_pSectionOpt = NULL;
	m_pGradeOpt = NULL;
	m_pCourseOpt = NULL;
	m_pEditionOpt = NULL;
	m_pSubEditionOpt = NULL;

	m_tmpChapterTree = NULL;
	::OnEvent(EVT_CHAPTER_SWITCH, MakeEventDelegate(this, &CGuideDialogUI::OnChapterSwitchFinish));

	m_nSliderCount = 1;

	m_strTempChapterGuid = _T("");
}

CGuideDialogUI::~CGuideDialogUI()
{
	m_pInitShowWindow = NULL;
	m_pInstance = NULL;
	CancelEvent(EVT_CHAPTER_SWITCH, MakeEventDelegate(this, &CGuideDialogUI::OnChapterSwitchFinish));
	NDCloudGetChapterTree(m_ChapterTree);
	if(m_tmpChapterTree && m_ChapterTree != m_tmpChapterTree)
	{
		m_tmpChapterTree->DestroyTree();
		delete m_tmpChapterTree;
	}

	KillTimer(GetHWND(),Course_Auto_Scroll);
	KillTimer(GetHWND(),Course_Complete_id);
	KillTimer(GetHWND(),Course_Failed_id);
	KillTimer(GetHWND(),Chapter_Complete_id);
	KillTimer(GetHWND(),Chapter_Complete_id);

	if( m_dwDownloadId != 0 )
		NDCloudDownloadCancel(m_dwDownloadId);
}

LPCTSTR CGuideDialogUI::GetWindowClassName() const
{
	return _T("GuideDialog");
}


void CGuideDialogUI::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);	
	delete this;
}


CDuiString CGuideDialogUI::GetSkinFile()
{
	return _T("Guide\\skin.xml");
}

CDuiString CGuideDialogUI::GetSkinFolder()
{
	return CDuiString(_T("skins"));
}

LRESULT CGuideDialogUI::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CGuideDialogUI::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CGuideDialogUI::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
#ifndef _DEBUG
	if (wParam == SC_CLOSE)
	{
		return 0;
	}
#endif
	return __super::OnSysCommand( uMsg, wParam, lParam, bHandled);
}

LRESULT CGuideDialogUI::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_USER_GET_COURSE:
		{
			CategoryNode * pGradeNode = m_CategoryTree->FindNode(m_CategoryTree->GetSelectedSectionCode(),m_pGradeOpt->GetNodeCode());

			if(pGradeNode && pGradeNode->pFirstChild)
			{
				m_pCourseCourse->RemoveAll();

				CategoryNode * pCategoryNode = pGradeNode->pFirstChild;

				int nHeight = 0;
				insertCourseLayout(pCategoryNode, nHeight, m_pCourseCourseLayout, m_pCourseCourse, _T("CourseSubject"), CourseLayout,  _T(""), true);

				m_pCourseCourse->SetVisible(true);
			}
			else
			{
				m_pCourseCourse->SetVisible(false);
			}

			selectImageStatus(0);
			selectButtonStatus(0);

			stopLoading();
		}
		break;
	case WM_USER_GET_EDITION:
		{

			CategoryNode * pCategoryNode = m_CategoryTree->FindNode(m_CategoryTree->GetSelectedSectionCode(),m_pGradeOpt->GetNodeCode(),m_pCourseOpt->GetNodeCode());
			if(pCategoryNode && pCategoryNode->pFirstChild)
			{
				m_pCourseEdition->RemoveAll();

				CategoryNode * pEditionNode = pCategoryNode->pFirstChild;

				int nHeight = 0;
				insertCourseLayout(pEditionNode, nHeight, m_pCourseEditionLayout, m_pCourseEdition, _T("CourseEdition"), EditionLayout, _T(""), true);

				m_pCourseEdition->SetVisible(true);
			}
			else
			{
				m_pCourseEdition->SetVisible(false);
			}

			selectImageStatus(1);
			selectButtonStatus(1);

			stopLoading();
		}
		break;
	case WM_USER_SWITCH_CHAPTER:
		{
			m_pChapterMask->SetVisible(false);
			OnSwitchChapterFinish();
		}
		break;
	case WM_KEYDOWN:
		{
			switch (wParam)
			{
			case VK_RETURN:
			case VK_ESCAPE:
			case VK_SPACE:
				return S_FALSE;
			}
		}
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void CGuideDialogUI::InitWindow()
{
	m_pMainContainer = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("mainContainer")));
	ASSERT(m_pMainContainer);
// 	m_pMainLayout = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("mainLayout")));

	m_pGuideSwitchTab = static_cast<CSliderTabLayoutUI*>(m_PaintManager.FindControl(_T("GuideSwitch")));

	m_pMainCourse = static_cast<CMainCourseUI*>(m_PaintManager.FindControl(_T("MainCourse")));
	m_pMainCourse->m_pCGuideDialogUI = this;
	ASSERT(m_pMainCourse);

	m_pCourseSection = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("CourseSection")));
	ASSERT(m_pCourseSection);
	m_pCourseGrade   = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("CourseGrade")));
	ASSERT(m_pCourseGrade);
	m_pCourseCourse = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("CourseCourse")));
	ASSERT(m_pCourseCourse);
	m_pCourseEdition   = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("CourseEdition")));
	ASSERT(m_pCourseEdition);
	m_pCourseSubEdition	 = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("CourseSubEdition")));
	ASSERT(m_pCourseSubEdition);

	m_pCourseSectionLayout = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("CourseSectionLayout")));
	m_pCourseGradeLayout   = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("CourseGradeLayout")));
	m_pCourseCourseLayout = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("CourseCourseLayout")));
	m_pCourseEditionLayout   = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("CourseEditionLayout")));
	m_pCourseSubEditionLayout	 = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("CourseSubEditionLayout")));


	m_pSwitchTitle = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("switchTitle")));
	m_pFirstSelectTitle = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("firstSelectTitle")));

	m_pCompleteTip	= static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("completeTip")));
	m_pCompleteTipText	= static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("completeTipText")));
	m_pCompleteTipText1	= static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("completeTipText1")));

	m_pGifAnimLayout = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("GifLoadingLayout")));
	m_pGifAnim		 = static_cast<CGifAnimUI*>(m_PaintManager.FindControl(_T("GifLoading")));

	m_pChapterMask	 = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("chapterMask")));

	m_pInitShowWindow	= static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("InitShowWindow")));



	m_pGuideTabBarLayout = dynamic_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("GuideTabBar")));

	m_pGuideGrade = dynamic_cast<CGuideGradeUI*>(m_PaintManager.FindControl(_T("GuideGradeUI")));
	//初始化tabbar
	for(int i = 0; i < 3; i++)
	{
		RECT rect;
		CContainerUI * pContainer = new CContainerUI;

		CHorizontalLayoutUI * pLayout1 = new CHorizontalLayoutUI;
		CButtonUI * pButton = new CButtonUI;
		pLayout1->SetInset(CDuiRect(10,0,10,0));
		pLayout1->Add(pButton);
		
		pButton->OnNotify += MakeDelegate(this, &CGuideDialogUI::OnTabbarNotify);
		CHorizontalLayoutUI * pLayout = new CHorizontalLayoutUI;
		pContainer->SetFloat(true);
		CLabelUI * pTextLabel = new CLabelUI;
		CLabelUI * pIconLabel = new CLabelUI;
		CControlUI * pLeftControl = new CControlUI;
		CControlUI * pRightControl = new CControlUI;
		
		pTextLabel->SetTextColor(0xFFFFFFFF);
		pTextLabel->SetAttribute(_T("align"),_T("center"));
		pIconLabel->SetVisible(false);

		pTextLabel->SetFont(140000);
		pLayout->Add(pLeftControl);
		pLayout->Add(pTextLabel);
		pLayout->Add(pIconLabel);
		pLayout->Add(pRightControl);

		pIconLabel->SetFixedWidth(23);

		if(i == 0)
		{
			pTextLabel->SetText(_T("请选择教材和课程"));
			pTextLabel->SetTextColor(0xFF12AFB6);
			pLayout->SetBkImage(_T("file='Guide\\bg_tab_normal.png' corner='20,0,20,0'"));
			pIconLabel->SetVisible(true);
		}
		else if(i == 1)
		{
			pTextLabel->SetText(_T("请选择章"));
			pLayout->SetBkImage(_T("file='Guide\\bg_tab_unselected.png' corner='20,0,20,0'"));
		}
		else
		{
			pTextLabel->SetText(_T("请选择节"));
			pLayout->SetBkImage(_T("file='Guide\\bg_tab_unselected.png' corner='20,0,20,0'"));
		}

		//计算文本宽度
		RECT rcCalc = {0};
		rcCalc.right = 296;

		this->CalcText(m_PaintManager.GetPaintDC(), rcCalc, pTextLabel->GetText(), pTextLabel->GetFont(), DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT|DT_NOPREFIX, UIFONT_GDI);
		pTextLabel->SetFixedWidth(rcCalc.right);

//		pLayout->SetFixedWidth(296);
//		pLayout->SetFixedHeight(31);
		m_pGuideTabBarLayout->Add(pContainer);
		pContainer->Add(pLayout);
		pContainer->Add(pLayout1);

		SetRect(&rect, 50 + i * 888/3 - i * 12 ,0, 50 + i* 888/3 - i * 12 + 888/3, 31);
		pContainer->SetPos(rect);

		
		GUIDETABBAR GuideTabbar;
		GuideTabbar.pTabbarContanier	= pContainer;
		GuideTabbar.pButton				= pButton;
		GuideTabbar.pTabbarLayout		= pLayout;
		GuideTabbar.pTextLabel			= pTextLabel;
		GuideTabbar.pIconLabel			= pIconLabel;
		m_vecGuideTabbar.push_back(GuideTabbar);
		
	}
	//
}

void CGuideDialogUI::Init(CRect rect)
{
// 	int nMainWindowHeight = 0;
// 	int nMainWindowWidth = 0;
// 	int nWindowHeight = rect.bottom-rect.top;
// 	int nWindowWidth = rect.right-rect.left;
// 	
  	int nMainWindowWidth = 968;
  	int nMainWindowHeight = 632;
	m_pMainContainer->SetFixedWidth(nMainWindowWidth);//设置主界面宽度
//	m_pMainLayout->SetFixedHeight(nMainWindowHeight);//设置主界面高度

//	::MoveWindow(GetHWND(), rect.left + (rect.Width() - nMainWindowWidth)/2, rect.top + (rect.Height() - nMainWindowHeight)/2, nMainWindowWidth, nMainWindowHeight, TRUE);
	//::SetWindowPos(GetHWND(), HWND_NOTOPMOST, 0 ,0 ,0 ,0 , SWP_NOSIZE|SWP_NOMOVE);
	MoveWindow(GetHWND(), rect.left, rect.top, rect.Width(), rect.Height(), TRUE);

	m_pMainCourse->SetFixedHeight(nMainWindowHeight - Course_layout_top - Course_layout_bottom_height - Course_layout_middle_top);
	m_CourseVisibleWidth = nMainWindowWidth - Course_layout_right_width;

	m_mainRect = rect;
// 	m_WndShadow.SetMaskSize(rect);
// 	m_WndShadow.Create(m_hWnd);
}

void CGuideDialogUI::LoadTree(bool bLocal)
{
	m_bLocal = bLocal;
	if(bLocal)
	{
		CButtonUI * pButton = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("GuideTopClose")));
		if(pButton)
			pButton->SetVisible(true);

		m_pConfirmLayout = dynamic_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("confirmLayout")));
		if(m_pConfirmLayout)
			m_pConfirmLayout->SetVisible(false);

		m_pFirstSelectTitle->SetVisible(false);
		m_pSwitchTitle->SetVisible(true);

		OnInitLocalTree();
	}
	else
	{
	//	ShowWindow(true);
		CButtonUI * pButton = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("GuideTopCloseLater")));
		if(pButton)
			pButton->SetVisible(true);
		

		m_pFirstSelectTitle->SetVisible(true);
		m_pSwitchTitle->SetVisible(false);
		startLoading();
		tstring strUrl = NDCloudComposeUrlCategory();
		m_dwDownloadId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CGuideDialogUI::OnGradeOneInit));
	}
}


LRESULT CGuideDialogUI::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

	bHandled = FALSE;
	return 0;
}

CControlUI* CGuideDialogUI::CreateControl(LPCTSTR pstrClass)
{
	if( _tcscmp(pstrClass, _T("GuideGrade")) == 0 )
		return static_cast<CGuideGradeUI*>(new CGuideGradeUI);

	else if( _tcscmp(pstrClass, _T("GuideChapter")) == 0 )
		return static_cast<CGuideChapterUI*>(new CGuideChapterUI);

	else if( _tcscmp(pstrClass, _T("SliderTabLayoutUI")) == 0 )
		return static_cast<CSliderTabLayoutUI*>(new CSliderTabLayoutUI);

	return NULL;
}

void CGuideDialogUI::Notify(TNotifyUI& msg)
{
	if( msg.sType == _T("click") )
	{	
		if( _tcsicmp(msg.pSender->GetClass(), _T("AutoResizeOptionUI")) == 0)
		{
			CAutoResizeOptionUI * pOption = static_cast<CAutoResizeOptionUI *>(msg.pSender);

			if( _tcsicmp(pOption->GetGroup(), _T("CourseGrade")) == 0)
			{
				if(pOption->IsSelected() == false)
				{
					ResizeOptText(m_pGradeOpt);

					CategoryNode* pCategoryNode = pOption->getCategoryNode();
					if(pCategoryNode == NULL)
						return;

					tstring strSectionNodeCode = pCategoryNode->pParent->strNDCode;
					m_CategoryTree->SelectSectionCode(strSectionNodeCode);

					tstring strGradeNodeCode = pOption->GetNodeCode();
	//				m_strGradeNodeCode = strGradeNodeCode;
	//				m_CategoryTree->SelectGradeCode(strGradeNodeCode);

					CategoryNode * pGradeNode = m_CategoryTree->FindNode(strSectionNodeCode,strGradeNodeCode, _T(""), _T(""));

					m_pCourseOpt = NULL;
					if(pGradeNode && pGradeNode->pFirstChild)
					{
						m_pCourseCourse->RemoveAll();

						CategoryNode * pCategoryNode = pGradeNode->pFirstChild;
						
						int nHeight = 0;
						insertCourseLayout(pCategoryNode, nHeight, m_pCourseCourseLayout, m_pCourseCourse, _T("CourseSubject"), CourseLayout , _T(""), true);

						selectImageStatus(0);
						selectButtonStatus(0);


					}
					else
					{
						tstring strUrl = NDCloudComposeUrlCategory(strGradeNodeCode);

						m_dwDownloadId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CGuideDialogUI::OnGetCourse));
						startLoading();

					}

					m_pGradeOpt = pOption;
					ResizeOptText(m_pGradeOpt, true);
				}
			}
			else if( _tcsicmp(pOption->GetGroup(), _T("CourseSubject")) == 0)
			{
				if(pOption->IsSelected() == false)
				{
					ResizeOptText(m_pCourseOpt);

					tstring strCourseNodeCode = pOption->GetNodeCode();
	//				m_strCourseNodeCode = strCourseNodeCode;
	//				m_CategoryTree->SelectCourseCode(strCourseNodeCode);
					CategoryNode * pCategoryNode = m_CategoryTree->FindNode(m_CategoryTree->GetSelectedSectionCode(),m_pGradeOpt->GetNodeCode(),strCourseNodeCode,_T(""));

					m_pEditionOpt = NULL;
					if(pCategoryNode && pCategoryNode->pFirstChild)
					{
						m_pCourseEdition->RemoveAll();

						CategoryNode * pEditionNode = pCategoryNode->pFirstChild;

						int nHeight = 0;
						insertCourseLayout(pEditionNode, nHeight, m_pCourseEditionLayout, m_pCourseEdition, _T("CourseEdition"),EditionLayout, _T(""), true);

						selectImageStatus(1);
						selectButtonStatus(1);
					}
					else
					{
						tstring strUrl = NDCloudComposeUrlCategory(m_pGradeOpt->GetNodeCode(),strCourseNodeCode);

						m_dwDownloadId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CGuideDialogUI::OnGetEdition));

						startLoading();
					}

					m_pCourseOpt = pOption;
					ResizeOptText(m_pCourseOpt, true);
				}
			}
			else if( _tcsicmp(pOption->GetGroup(), _T("CourseEdition")) == 0)
			{
				if(pOption->IsSelected() == false)
				{
					ResizeOptText(m_pEditionOpt);

					tstring strEditionNodeCode = pOption->GetNodeCode();
		//			m_strEditionNodeCode = strEditionNodeCode;
		//			m_CategoryTree->SelectEditionCode(strEditionNodeCode);

					CategoryNode * pEditionNode = m_CategoryTree->FindNode(m_CategoryTree->GetSelectedSectionCode(),
						m_pGradeOpt->GetNodeCode(),m_pCourseOpt->GetNodeCode(),strEditionNodeCode);

					m_pCourseSubEdition->RemoveAll();
					m_pSubEditionOpt = NULL;

					if(pEditionNode && pEditionNode->pFirstChild)
					{
						CategoryNode * pSubEditionNode = pEditionNode->pFirstChild;

						int nHeight = 0;
						insertCourseLayout(pSubEditionNode, nHeight, m_pCourseSubEditionLayout, m_pCourseSubEdition, _T("CourseSubEdition"),SubEditionLayout, _T(""), true);

						m_pCourseSubEdition->SetVisible(true);
					}
					else
					{
						m_pCourseSubEdition->SetVisible(false);
					}
					
					selectImageStatus(2);

					m_pEditionOpt = pOption;
					ResizeOptText(m_pEditionOpt, true);
				}
			}
			else if( _tcsicmp(pOption->GetGroup(), _T("CourseSubEdition")) == 0)
			{
				if(pOption->IsSelected() == false)
				{
					ResizeOptText(m_pSubEditionOpt);
				}
				
				selectImageStatus(3);
				tstring strSubEditionNodeCode = pOption->GetNodeCode();
				
				//20160121增加判断课程是否有修改
// 				if(m_pGradeOpt->GetNodeCode() == m_strGradeNodeCode &&
// 				   m_pCourseOpt->GetNodeCode() == m_strCourseNodeCode &&
// 				   m_pEditionOpt->GetNodeCode() == m_strEditionNodeCode &&
// 				   strSubEditionNodeCode == m_strSubEditionNodeCode )
// 				{
// 					//无修改直接跳转
// 					OnSwitchChapter(1, false, false);
// 					return;
// 					//
// 				}

//				m_strSubEditionNodeCode = strSubEditionNodeCode;
				//
				tstring strUrl = NDCloudComposeUrlBookInfo(
						m_CategoryTree->GetSelectedSectionCode(),
						m_pGradeOpt->GetNodeCode(),
						m_pCourseOpt->GetNodeCode(),
						m_pEditionOpt->GetNodeCode(),
						strSubEditionNodeCode,
						0,
						20
						);

				//增加年级配置
				if(m_pGradeOpt->GetNodeCode() == _T("$ON040000"))
				{
					tstring strPath = GetLocalPath();
					strPath += _T("\\Setting\\Config.ini");

					WritePrivateProfileString(_T("Config"),_T("GradeText"), m_pGradeOpt->GetText(), strPath.c_str());
				}

				
				//
				m_dwDownloadId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CGuideDialogUI::OnGetBookGUID));

				m_pSubEditionOpt = pOption;
				ResizeOptText(m_pSubEditionOpt, true);

				startLoading();
			}
		}
		else if( _tcsicmp(msg.pSender->GetName(), _T("preStep")) == 0 )
		{
			{
				int nIndex = m_pGuideSwitchTab->GetCurSel();

				CContainerUI* pGuideChapter = m_vecGuideChapter[nIndex - 1];
					
				OnSwitchChapter(nIndex - 1, true, false);

				/*
				if(pGuideChapter)
				{
					CButtonUI * pButton = static_cast<CButtonUI *>(pGuideChapter->FindSubControl(_T("onCreate")));
					pButton->SetEnabled(false);
					pButton->SetBkColor(0xFF969696);
				}
				*/
			}
		}
		else if( _tcsicmp(msg.pSender->GetClass(), _T("AutoHideOptionUI") ) == 0 )
		{
			CAutoHideOptionUI * pOption = static_cast<CAutoHideOptionUI *>(msg.pSender);
			m_pChapterOpt = pOption;

			int nIndex = m_pGuideSwitchTab->GetCurSel();
			CContainerUI* pGuideChapter = m_vecGuideChapter[nIndex - 1];
			tstring strGuid = pOption->GetGuid();

			ChapterNode * pChapterNode = m_tmpChapterTree->FindNode(strGuid);
			if(pChapterNode && pChapterNode->pFirstChild)
			{
				insertTabLayout(pChapterNode,GetTempChapterGUID());

				//20160121增加判断课程是否有修改
				tstring strTmpGuid;
				if(GetClickedChapterNode(pGuideChapter, pChapterNode, strTmpGuid))
				{
					if(strTmpGuid == strGuid)
					{
						OnSwitchChapter(nIndex + 1, false, false);
						return ;
					}	
				}
				OnSwitchChapter(nIndex + 1, false);

				
				if(pGuideChapter)
				{
					CButtonUI * pButton = static_cast<CButtonUI *>(pGuideChapter->FindSubControl(_T("onCreate")));
					pButton->SetEnabled(false);
					pButton->SetBkColor(0xFF969696);

					pGuideChapter = m_vecGuideChapter[nIndex];
					CCheckBoxUI * pCheckBox = static_cast<CCheckBoxUI *>(pGuideChapter->FindSubControl(_T("DefaultSetting")));
					if(m_bDefaultSetting)
					{
						pCheckBox->SetCheck(true);
					}
					else
					{
						pCheckBox->SetCheck(false);
					}
				}
				
			}
			else
			{
				//20160121状态修改
				OnSwitchChapter(nIndex, false, true, false);
				//
				m_strTempChapterGuid = pChapterNode->strGuid;
				
				if(pGuideChapter)
				{
					CButtonUI * pButton = static_cast<CButtonUI *>(pGuideChapter->FindSubControl(_T("onCreate")));
					pButton->SetEnabled(true);
					pButton->SetBkColor(0xFFFA851B);
				}
				
			}
		}
		else if( _tcsicmp(msg.pSender->GetName(), _T("DefaultSetting")) == 0)
		{
			CCheckBoxUI * pCheckBox = static_cast<CCheckBoxUI *>(msg.pSender);

			tstring strPath = GetLocalPath();
			strPath += _T("\\Setting\\Config.ini");

			if(pCheckBox->GetCheck() == false)
			{
				m_bDefaultSetting = true;
				WritePrivateProfileString(_T("Config"),_T("DefaultSetting"),_T("true"), strPath.c_str());
			}
			else
			{
				m_bDefaultSetting = false;

				NDCloudClearTrees();

				WritePrivateProfileString(_T("Config"),_T("CourseRecord"),_T(""), strPath.c_str());
				WritePrivateProfileString(_T("Config"),_T("DefaultSetting"),_T("false"), strPath.c_str());
			}
		}
		else if( _tcsicmp(msg.pSender->GetName(), _T("onCreate")) == 0)
		{
			CButtonUI* pCreateBtn = dynamic_cast<CButtonUI *>(msg.pSender);
			if(pCreateBtn->GetTag() == NULL)
			{
				int* pTag = new int;
				*pTag = 1;
				pCreateBtn->SetTag((UINT_PTR)pTag);
			}
			else
			{
				return;
			}

			m_pCompleteTipText->SetBkImage(_T("file='Guide\\icon_congratulation.png' dest='138,20,182,64'"));
			m_pCompleteTipText->SetTextColor(0xFFF88311);
			m_pCompleteTipText->SetText(Chapter_Complete_Text);
			m_pCompleteTipText1->SetTextPadding(CDuiRect(0,80,0,0));
			m_pCompleteTipText1->SetTextColor(0xFFEEEEEE);
			m_pCompleteTipText1->SetText(Chapter_Complete_Text1);

			m_pCompleteTip->SetVisible(true);

 			tstring & strGuid = m_pChapterOpt->GetGuid();

			tstring strSectionNodeCode = GetSectionNodeCode(m_strGradeNodeCode);
			m_CategoryTree->SelectSectionCode(strSectionNodeCode);
			m_CategoryTree->SelectGradeCode(m_strGradeNodeCode);
			m_CategoryTree->SelectCourseCode(m_strCourseNodeCode);
			m_CategoryTree->SelectEditionCode(m_strEditionNodeCode);
			m_CategoryTree->SelectSubEditionCode(m_strSubEditionNodeCode);

			tstring strPath = GetLocalPath();
			strPath += _T("\\Setting\\Config.ini");

			TCHAR szGuid[128] = {0};
			GetPrivateProfileString(_T("Config"),_T("ChapterGuid"),_T(""), szGuid, _countof(szGuid) - 1, strPath.c_str());

			TCHAR szChapterName[128] = {0};
			GetPrivateProfileString(_T("Config"),_T("ChapterName"),_T(""), szChapterName, _countof(szChapterName) - 1, strPath.c_str());

			bool bInit = false;
			if(_tcslen(szGuid) == 0 || _tcslen(szChapterName) == 0)
				bInit = true;
			NDCloudGetChapterTree(m_ChapterTree);
			if(m_ChapterTree == NULL)
				bInit = true;

			if(m_tmpChapterTree != m_ChapterTree)
				NDCloudSaveChapterTrees(m_tmpChapterTree);	

			if(m_bDefaultSetting)
			{
				NDCloudSaveTrees();
				TCHAR szCourseRecord[1024] = {0};
				_stprintf_s(szCourseRecord,_T("%s/%s/%s/%s/%s"),
					m_CategoryTree->GetSelectedSectionCode().c_str(), 
					m_CategoryTree->GetSelectedGradeCode().c_str(), 
					m_CategoryTree->GetSelectedCourseCode().c_str(), 
					m_CategoryTree->GetSelectedEditionCode().c_str(), 
					m_CategoryTree->GetSelectedSubEditionCode().c_str());


				WritePrivateProfileString(_T("Config"),_T("CourseRecord"),szCourseRecord, strPath.c_str());
				WritePrivateProfileString(_T("Config"),_T("DefaultSetting"),_T("true"), strPath.c_str());

			}

			NDCloudSetChapterGUID(strGuid, m_pChapterOpt->GetText().GetData(), bInit);

			SetTimer(m_hWnd, Chapter_Complete_id, 2000, (TIMERPROC)TimerProcComplete);
		}
		else if( _tcsicmp(msg.pSender->GetName(), _T("GuideTopClose")) == 0 || _tcsicmp(msg.pSender->GetName(), _T("GuideTopCloseLater")) == 0)
		{
			Close();
		}
		else if( _tcsicmp(msg.pSender->GetName(), _T("confirmBtn")) == 0)
		{
			OnSwitchChapter(1,false);
		}
	}

}

void CGuideDialogUI::TimerProcComplete(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime)
{
	if(idEvent == Chapter_Complete_id)
	{
		KillTimer(hwnd,Chapter_Complete_id);
		GetInstance()->m_pCompleteTip->SetVisible(false);
		GetInstance()->Close();
	}
	if(idEvent == Course_Failed_id)
	{
		KillTimer(hwnd,Course_Failed_id);
		GetInstance()->m_pCompleteTip->SetVisible(false);
	}
	if(idEvent == Course_Complete_id)
	{
		KillTimer(hwnd,Course_Complete_id);
		GetInstance()->m_pCompleteTip->SetVisible(false);

		int nIdx = GetInstance()->m_pGuideSwitchTab->GetCurSel();
		GetInstance()->OnSwitchChapter(nIdx+1, false);
	}
	if(idEvent == Course_Auto_Scroll)
	{
		if(GetInstance())
		{
			SIZE size =  GetInstance()->m_pMainCourse->GetScrollRange();

			if(size.cy > 0)
			{
				int nTotalHeight = 0;
				for(int i = GradeLabelLayout; i <= GetInstance()->m_nCurrentSelectLayoutID; i++)
				{
					nTotalHeight += GetInstance()->m_LayoutHeight[i];
				}

				int nFixedHeight = GetInstance()->m_pMainCourse->GetFixedHeight();

				size.cx = 0;
				size.cy = nTotalHeight - nFixedHeight;

				GetInstance()->m_pMainCourse->SetScrollPos(size);

				KillTimer(hwnd,Course_Auto_Scroll);
			}
		}
		
	}
}


void CGuideDialogUI::OnInitLocalTree()
{
	BOOL bRet = NDCloudGetCategoryTree(m_CategoryTree);

	tstring strPath = GetLocalPath();
	strPath += _T("\\Setting\\Config.ini");

	TCHAR szDefaultSetting[128] = {0};
	GetPrivateProfileString(_T("Config"),_T("DefaultSetting"),_T(""),szDefaultSetting,_countof(szDefaultSetting)-1, strPath.c_str());
	if( _tcsicmp(szDefaultSetting, _T("") ) == 0 || _tcsicmp(szDefaultSetting, _T("true") ) == 0)
	{
		m_bDefaultSetting = true;
	}

	if(bRet && m_CategoryTree)
	{

		tstring strGradeNodeCode = m_CategoryTree->GetSelectedGradeCode();
		tstring strCourseNodeCode = m_CategoryTree->GetSelectedCourseCode();
		tstring strEditionNodeCode = m_CategoryTree->GetSelectedEditionCode();
		tstring strSubEditionNodeCode = m_CategoryTree->GetSelectedSubEditionCode();
		m_strGradeNodeCode = strGradeNodeCode;
		m_strCourseNodeCode = strCourseNodeCode;
		m_strEditionNodeCode = strEditionNodeCode;
		m_strSubEditionNodeCode = strSubEditionNodeCode;

		

		TCHAR szCourseRecord[1024] = {0};

		GetPrivateProfileString(_T("Config"),_T("CourseRecord"),_T(""),szCourseRecord, _countof(szCourseRecord)-1, strPath.c_str());

		TCHAR szSection[MAX_PATH]		= _T("");
		TCHAR szGrade[MAX_PATH]			= _T("$ON020100");
		TCHAR szCourse[MAX_PATH]		= _T("");
		TCHAR szEdition[MAX_PATH]		= _T("");
		TCHAR szSubEdition[MAX_PATH]	= _T("");

		if(strCourseNodeCode.length() == 0 || strEditionNodeCode.length() == 0 || strSubEditionNodeCode.length() == 0)
		{
			if(_stscanf_s(szCourseRecord,_T("%[^/]/%[^/]/%[^/]/%[^/]/%[^/]"), 
				szSection, _countof(szSection) - 1,
				szGrade, _countof(szGrade) - 1,
				szCourse, _countof(szCourse) - 1,
				szEdition, _countof(szEdition) - 1,
				szSubEdition,_countof(szSubEdition) - 1) == 5)
			{

			}

			strGradeNodeCode			= szGrade;
			strCourseNodeCode			= szCourse;
			strEditionNodeCode		= szEdition;
			strSubEditionNodeCode		= szSubEdition;

			m_CategoryTree->SelectGradeCode(strGradeNodeCode);
			m_CategoryTree->SelectCourseCode(strCourseNodeCode);
			m_CategoryTree->SelectEditionCode(strEditionNodeCode);
			m_CategoryTree->SelectSubEditionCode(strSubEditionNodeCode);

			m_strGradeNodeCode = strGradeNodeCode;
			m_strCourseNodeCode = strCourseNodeCode;
			m_strEditionNodeCode = strEditionNodeCode;
			m_strSubEditionNodeCode = strSubEditionNodeCode;
		}
		else
		{
			if(_stscanf_s(szCourseRecord,_T("%[^/]/%[^/]/%[^/]/%[^/]/%[^/]"), 
				szSection, _countof(szSection) - 1,
				szGrade, _countof(szGrade) - 1,
				szCourse, _countof(szCourse) - 1,
				szEdition, _countof(szEdition) - 1,
				szSubEdition,_countof(szSubEdition) - 1) == 5)
			{
// 				
			}
		}

		OnInitTree(strGradeNodeCode,strCourseNodeCode,strEditionNodeCode,strSubEditionNodeCode, true);
	}
}

bool CGuideDialogUI::OnInitTree(tstring strGrade, tstring strCourse, tstring strEdition, tstring strSubEdition, bool bInit)
{
//	ShowWindow(true);
	if((strCourse == _T("") || strEdition == _T("") || strSubEdition == _T("") )&& bInit)
	{
		startLoading();
		tstring strUrl = NDCloudComposeUrlCategory();
		m_dwDownloadId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CGuideDialogUI::OnGradeOneInit));
		return false;
	}

	if (m_pInitShowWindow)
	{
		m_pInitShowWindow->SetVisible(false);
	}
	

	CHorizontalLayoutUI *	pHorizontalLayout = NULL;
	CVerticalLayoutUI *		pVerticalLayout = NULL;
	int nLineWidth = 0;
	int nLineCount = 1;

	//布局高度
//	m_LayoutHeight[SectionLabelLayout] = Course_label_height;
	m_LayoutHeight[GradeLabelLayout] = Course_Layer_magrin;
	m_LayoutHeight[CourseLabelLayout] = Course_Layer_magrin;
	m_LayoutHeight[EditionLabelLayout] = Course_Layer_magrin;
	m_LayoutHeight[SubEditionLabelLayout] = Course_Layer_magrin;

	m_pCourseGrade->RemoveAll();
	m_pCourseCourse->RemoveAll();
	m_pCourseEdition->RemoveAll();
	m_pCourseSubEdition->RemoveAll();
	//
	//////////////////////////////////////////////////////////////////////////年级段

	CategoryNode * pCourseRootNode = m_CategoryTree->GetRootNode();

	if(pCourseRootNode == NULL)
		return false;

	CategoryNode * pSectionNode = pCourseRootNode->pFirstChild ;

	if(pSectionNode == NULL)
		return false;

	int nHeight = 0;

	//////////////////////////////////////////////////////////////////////////年级
	CategoryNode * pGradeNode = NULL;

	insertGradeLayout(pSectionNode, nHeight, m_pCourseGradeLayout, m_pCourseGrade, _T("CourseGrade"), GradeLayout ,strGrade);

	//////////////////////////////////////////////////////////////////////////学科


	pGradeNode = m_CategoryTree->FindNode(m_CategoryTree->GetSelectedSectionCode(), strGrade) ;

	if(pGradeNode == NULL || pGradeNode->pFirstChild == NULL)
		return false;

	CategoryNode * pCourseNode = pGradeNode->pFirstChild ;

	insertCourseLayout(pCourseNode, nHeight, m_pCourseCourseLayout, m_pCourseCourse, _T("CourseSubject"), CourseLayout, strCourse);

	//////////////////////////////////////////////////////////////////////////版本

	if(strCourse.length() > 0)
		pCourseNode = m_CategoryTree->FindNode(m_CategoryTree->GetSelectedSectionCode(), strGrade, strCourse);
	else
		pCourseNode = pGradeNode->pFirstChild ;

	if(pCourseNode == NULL || pCourseNode->pFirstChild == NULL)
		return false;

	CategoryNode * pEditionNode = pCourseNode->pFirstChild ;

	insertCourseLayout(pEditionNode, nHeight, m_pCourseEditionLayout, m_pCourseEdition, _T("CourseEdition"), EditionLayout, strEdition);

	//////////////////////////////////////////////////////////////////////////子版本

	if(strEdition.length() > 0)
		pEditionNode = m_CategoryTree->FindNode(m_CategoryTree->GetSelectedSectionCode(), strGrade, strCourse, strEdition);
	else
		pEditionNode = pCourseNode->pFirstChild ;

	if(pEditionNode == NULL || pEditionNode->pFirstChild == NULL)
		return false;

	CategoryNode * pSubEditionNode = pEditionNode->pFirstChild ;

	insertCourseLayout(pSubEditionNode, nHeight, m_pCourseSubEditionLayout, m_pCourseSubEdition, _T("CourseSubEdition"), SubEditionLayout, strSubEdition);

	selectImageStatus(0);

	if(strCourse != _T(""))
		selectImageStatus(1);

	if(strEdition != _T(""))
		selectImageStatus(2);
	else
		selectButtonStatus(0);

	if(strSubEdition != _T(""))
		selectImageStatus(3);
	else
		selectButtonStatus(1);

	//////////////////////////////////////////////////////////////////////////状态

	if(!m_bLoadChapterTree)
	{
		m_bLoadChapterTree = true;
		//加载chapter树
		NDCloudGetChapterTree(m_ChapterTree);
		
		if(m_ChapterTree)
		{
			m_tmpChapterTree = m_ChapterTree;

			tstring strChapterGuid = NDCloudGetChapterGUID();
			m_strTempChapterGuid = strChapterGuid;

			vector<ChapterNode*> vecChapterLevel;

			ChapterNode* pChapterNode = m_ChapterTree->FindNode(strChapterGuid);
			while(pChapterNode && pChapterNode->pParent)
			{
				vecChapterLevel.push_back(pChapterNode);
				pChapterNode = pChapterNode->pParent;
			}

			TCHAR szName[MAX_PATH] = {0};
			
			for(int i = 0; i < (int)vecChapterLevel.size() ; i++)
			{
				pChapterNode = m_ChapterTree->FindNode( vecChapterLevel[vecChapterLevel.size() - i - 1]->strGuid );
				insertTabLayout(pChapterNode->pParent,pChapterNode->strGuid);
			}

			int nCount =m_pGuideSwitchTab->GetCount();
			OnSwitchChapter(vecChapterLevel.size() , false, true, false);
			
		}
		//

	}
	
	return true;
}

bool CGuideDialogUI::OnGradeOneInit(void * pParam)
{
	THttpNotify* pNotify = static_cast<THttpNotify*>(pParam);

	if(pNotify->dwErrorCode != 0)
	{
		CLabelUI * pLabel = static_cast<CLabelUI *>(m_pInitShowWindow->FindSubControl(_T("InitText")));
		pLabel->SetText(_T("数据初始化失败..."));
		stopLoading();
		return false;
	}
//	CStream* pOutputStream = NULL;
	BOOL bRet = NDCloudDecodeCategory(pNotify->pData, pNotify->nDataSize , m_CategoryTree);

	if(!bRet || m_CategoryTree == NULL)
	{
		CLabelUI * pLabel = static_cast<CLabelUI *>(m_pInitShowWindow->FindSubControl(_T("InitText")));
		pLabel->SetText(_T("数据初始化失败..."));
		stopLoading();
		return false;
	}
	// section

	m_strGradeNodeCode = _T("$ON020100");
	OnInitTree(m_strGradeNodeCode);
	
	TCHAR szDefaultSetting[128] = {0};
	tstring strPath = GetLocalPath();
	strPath += _T("\\Setting\\Config.ini");
	GetPrivateProfileString(_T("Config"),_T("DefaultSetting"),_T(""),szDefaultSetting,_countof(szDefaultSetting)-1, strPath.c_str());
	if( _tcsicmp(szDefaultSetting, _T("false") ) != 0)
	{
		m_bDefaultSetting = true;
	}

	stopLoading();

	return true;
}

void CGuideDialogUI::insertGradeLayout(CategoryNode * pNode, int &nHeight, CVerticalLayoutUI* pVLayout, CVerticalLayoutUI* pContentLayout, LPCTSTR szGroupName, int nLayoutID, tstring strNDCode)
{
	int nIndex = 0;
	int nLineWidth = 0;
	int nLineCount = 0;

	CVerticalLayoutUI *pVerticalLayout = NULL;
	CHorizontalLayoutUI *pHorizontalLayout = NULL;

	CategoryNode * pSubNode = NULL;

	pVLayout->SetFixedHeight(Course_btn_layout_height + Course_label_height);

	pHorizontalLayout = new CHorizontalLayoutUI;
	pContentLayout->Add(pHorizontalLayout);

	CLabelUI * pLabel = NULL;

	bool bFirstOpt = false;

	while(pNode && pNode->pFirstChild)
	{
		pSubNode = pNode->pFirstChild;

		pVerticalLayout = new CVerticalLayoutUI;

		if(nIndex != 0)//
		{
			CControlUI * pControl = new CControlUI;
			pControl->SetFixedWidth(Course_grade_magrin);
			pHorizontalLayout->Add(pControl);
		}
		pHorizontalLayout->Add(pVerticalLayout);
		pVerticalLayout->SetInset(CDuiRect(Course_btn_inset_left, 0, Course_btn_inset_right, 0));

		switch(nIndex)
		{
		case 0:
			{
				pVerticalLayout->SetFixedWidth(m_pMainContainer->GetFixedWidth() * Course_Middle_school_width);
				pLabel = new CLabelUI;
				pLabel->SetTextColor(0xFF334455);
				pLabel->SetText(_T("小学"));
				pLabel->SetAttribute(_T("align"),_T("center"));
				pLabel->SetFixedHeight(Course_label_height);
				pVerticalLayout->Add(pLabel);
				pVerticalLayout->SetBkImage(_T("file='Guide\\icon_background_primaryschool.png' corner='15,60,5,5'"));
			}
			break;
		case 1:
			{
				pVerticalLayout->SetFixedWidth(m_pMainContainer->GetFixedWidth() * Course_Middle_school_width);
				pLabel = new CLabelUI;
				pLabel->SetTextColor(0xFF334455);
				pLabel->SetText(_T("初中"));
				pLabel->SetAttribute(_T("align"),_T("center"));
				pLabel->SetFixedHeight(Course_label_height);
				pVerticalLayout->Add(pLabel);
				pVerticalLayout->SetBkImage(_T("file='Guide\\icon_background_middleschool.png' corner='15,60,5,5'"));
			}
			break;
		case 2:
			{
				pVerticalLayout->SetFixedWidth(m_pMainContainer->GetFixedWidth() * Course_high_school_width);
				pLabel = new CLabelUI;
				pLabel->SetTextColor(0xFF334455);
				pLabel->SetText(_T("高中"));
				pLabel->SetAttribute(_T("align"),_T("center"));
				pLabel->SetFixedHeight(Course_label_height);
				pVerticalLayout->Add(pLabel);
				pVerticalLayout->SetBkImage(_T("file='Guide\\icon_background_highschool.png' corner='15,60,5,5'"));
			}
			break;
		}

		nLineWidth = 0;
		nLineCount = 0;

		CHorizontalLayoutUI *pChildHorizontalLayout = new CHorizontalLayoutUI;
		pChildHorizontalLayout->SetFixedHeight(Course_btn_height);

		pVerticalLayout->Add(pChildHorizontalLayout);
		while(pSubNode)
		{
			tstring strName = pSubNode->strTitle;
			if(strName == _T("十年级"))
			{
				strName = _T("高一");
			}
			else if(strName == _T("十一年级"))
			{
				strName = _T("高二");
			}
			else if(strName == _T("十二年级"))
			{
				strName = _T("高三");
			}

			tstring strNodeCode = pSubNode->strNDCode;

			int nWidth = strName.length() * Course_btn_text_width > Course_btn_minwidth ? strName.length() * Course_btn_text_width : Course_btn_minwidth;
			if(nLineWidth + nWidth < pVerticalLayout->GetFixedWidth() - Course_btn_inset_right)
			{
				bFirstOpt = false;
				if(nLineWidth == 0)
					bFirstOpt = true;
				nLineWidth += nWidth;
				nLineWidth += Course_btn_magrin;
			}
			else
			{
				nLineCount++;
				

				CControlUI * pControl = new CControlUI;
				pControl->SetFixedHeight(Course_btn_magrin);
				pVerticalLayout->Add(pControl);

				pChildHorizontalLayout = new CHorizontalLayoutUI;
				pChildHorizontalLayout->SetFixedHeight(Course_btn_height);

				pVerticalLayout->Add(pChildHorizontalLayout);

				nLineWidth = 0;
				nLineWidth += nWidth;
				nLineWidth += Course_btn_magrin;
				nLineWidth += Course_btn_inset_right;

				bFirstOpt = true;

				int nTempHeight = pVLayout->GetFixedHeight();
				int nTempHeight1 = Course_btn_height * (nLineCount + 1)+ Course_label_height + nLineCount * Course_btn_magrin + Course_btn_inset_bottom;
				if(nTempHeight1 > nTempHeight)
					pVLayout->SetFixedHeight(nTempHeight1);
			}

			if(!bFirstOpt)
			{
				CControlUI * pControl = new CControlUI;
				pControl->SetFixedWidth(Course_grade_magrin);
				pChildHorizontalLayout->Add(pControl);

			}
			CAutoResizeOptionUI* pOption = new CAutoResizeOptionUI;
			pOption->SetName(_T("GuideOption"));
			pOption->SetText(strName.c_str());
			pOption->SetNodeCode(strNodeCode.c_str());
			pOption->SetFixedHeight(Course_btn_height);
			pOption->SetGroup(szGroupName);
			pOption->SetTextColor(0xFF000001);
			pOption->SetBkColor(0xFFFFFFFF);
			pOption->SetSelectedTextColor(0xFFFFFFFF);
			pOption->SetFont(130000);
			pOption->SetFixedWidth(nWidth);
			pOption->SetSelectedBkColor(Color(255,17,176,182).GetValue());

			pOption->SetBorderSize(1);
			pOption->SetBorderColor(0xFFD4D4D4);
			pOption->setCategoryNode(pSubNode);

			if(strName == _T("高三"))
			{
				pOption->SetTextColor(Color(255,17,176,182).GetValue());
				pOption->SetBorderColor(Color(255,17,176,182).GetValue());
			}


			pChildHorizontalLayout->Add(pOption);

			if( strNodeCode == strNDCode && strNDCode.length() > 0)
			{
				if( strNDCode == _T("$ON040000") )
				{
					tstring strGrade = pOption->GetText();

					tstring strPath = GetLocalPath();
					strPath += _T("\\Setting\\Config.ini");

					TCHAR szGradeText[128] = {0};
					GetPrivateProfileString(_T("Config"),_T("GradeText"),_T(""),szGradeText,_countof(szGradeText)-1, strPath.c_str());

					if( _tcsicmp( szGradeText, strGrade.c_str()) == 0)
					{
						pOption->Selected(true);
					}
				}
				else
					pOption->Selected(true);
				ResizeOptText(pOption ,true);
				switch(nLayoutID)
				{
				case GradeLayout:
					{
						m_pGradeOpt = pOption;
						tstring strSectionNodeCode = pSubNode->pParent->strNDCode;
						m_CategoryTree->SelectSectionCode(strSectionNodeCode);
						m_CategoryTree->SelectGradeCode(pSubNode->strNDCode);
					}
					break;
				case CourseLayout:
					{
						m_pCourseOpt = pOption;
					}
					break;
				case EditionLayout:
					{
						m_pEditionOpt = pOption;
					}
					break;
				case SubEditionLayout:
					{	
						m_pSubEditionOpt = pOption;
					}
					break;
				}
			}

			pSubNode = pSubNode->pNextSlibing;
		}

		pNode = pNode->pNextSlibing;
		nIndex ++;
	} 
	


	m_LayoutHeight[nLayoutID] = pVLayout->GetFixedHeight();
	//
}

void CGuideDialogUI::insertCourseLayout(CategoryNode * pNode, int &nHeight,CVerticalLayoutUI* pVLayout, CVerticalLayoutUI* pContentLayout, LPCTSTR szGroupName, int nLayoutID, tstring strNDCode, bool bAutoSize)
{
	int nLineWidth = 0;
	int nLineCount = 1;

	CVerticalLayoutUI *pVerticalLayout = NULL;
	CHorizontalLayoutUI *pHorizontalLayout = NULL;

	pContentLayout->SetVisible(true);//增加显示
	pContentLayout->SetInset(CDuiRect(Course_btn_inset_left, Course_btn_inset_top, Course_btn_inset_right, Course_btn_inset_bottom));

	bool bFirstOpt = false;

	do 
	{
		if(nLineWidth == 0)
		{
			nHeight = nLineCount * Course_btn_layout_height + Course_btn_inset_top + Course_btn_inset_bottom;
			pVLayout->SetFixedHeight(nHeight);
			pVerticalLayout = new CVerticalLayoutUI;
			pVerticalLayout->SetFixedHeight(Course_btn_layout_height);
			pContentLayout->Add(pVerticalLayout);
			pHorizontalLayout = new CHorizontalLayoutUI;
			pVerticalLayout->Add(pHorizontalLayout);
		}

		tstring strName = pNode->strTitle;
		tstring strNodeCode = pNode->strNDCode;
		pNode = pNode->pNextSlibing;

		int nWidth = strName.length() * Course_btn_text_width > Course_btn_minwidth ? strName.length() * Course_btn_text_width : Course_btn_minwidth;
		if(nLineWidth + nWidth < pContentLayout->GetFixedWidth() - Course_btn_inset_right)
		{
			bFirstOpt = false;
			if(nLineWidth == 0)
				bFirstOpt = true;
			nLineWidth += nWidth;
			nLineWidth += Course_btn_magrin;
		}
		else
		{
			nLineCount ++;

			CControlUI * pControl = new CControlUI;
			pControl->SetFixedHeight(Course_btn_magrin);
			pContentLayout->Add(pControl);

			pVerticalLayout = new CVerticalLayoutUI;
			pVerticalLayout->SetFixedHeight(Course_btn_layout_height);
			pContentLayout->Add(pVerticalLayout);
			pHorizontalLayout = new CHorizontalLayoutUI;
			pVerticalLayout->Add(pHorizontalLayout);

			nLineWidth = 0;
			nLineWidth += nWidth;
			nLineWidth += Course_btn_magrin;
			nLineWidth += Course_btn_inset_right;

			bFirstOpt = true;

			nHeight = Course_btn_height * nLineCount + (nLineCount - 1) * Course_btn_magrin + Course_btn_inset_top + Course_btn_inset_bottom;
			int nTempHeight = pVLayout->GetFixedHeight();
			if(nHeight > nTempHeight)
				pVLayout->SetFixedHeight(nHeight);
		}

		if(!bFirstOpt)
		{
			CControlUI * pControl = new CControlUI;
			pControl->SetFixedWidth(Course_grade_magrin);
			pHorizontalLayout->Add(pControl);

		}

		CAutoResizeOptionUI* pOption = new CAutoResizeOptionUI;
		pOption->SetName(_T("GuideOption"));
		pOption->SetText(strName.c_str());
		pOption->SetNodeCode(strNodeCode.c_str());
		pOption->SetFixedHeight(Course_btn_height);
		pOption->SetGroup(szGroupName);
		pOption->SetTextColor(0xFF000001);
		pOption->SetBkColor(0xFFFFFFFF);
		pOption->SetSelectedTextColor(0xFFFFFFFF);
		pOption->SetSelectedBkColor(Color(255,17,176,182).GetValue());
		pOption->SetFixedWidth(nWidth);
		pOption->SetFont(130000);

		pOption->SetBorderSize(1);
		pOption->SetBorderColor(0xFFD4D4D4);

		pHorizontalLayout->Add(pOption);


		if( strNodeCode == strNDCode && strNDCode.length() > 0)
		{
			pOption->Selected(true);
			ResizeOptText(pOption ,true);
			switch(nLayoutID)
			{
			case CourseLayout:
				{
					m_pCourseOpt = pOption;
				}
				break;
			case EditionLayout:
				{
					m_pEditionOpt = pOption;
				}
				break;
			case SubEditionLayout:
				{	
					m_pSubEditionOpt = pOption;
				}
				break;
			}
		}
	} while (pNode);

	m_LayoutHeight[nLayoutID] = nHeight;

	//计算是否需要滚动--滚动到层的最下方位置
	if(bAutoSize)
	{
		int nTotalHeight = 0;
		for(int i = GradeLabelLayout; i <= nLayoutID; i++)
		{
			nTotalHeight += m_LayoutHeight[i];
		}

		int nFixedHeight = m_pMainCourse->GetFixedHeight();
		if(nTotalHeight > nFixedHeight)
		{
			m_nCurrentSelectLayoutID = nLayoutID;
			SetTimer(m_hWnd, Course_Auto_Scroll, 50,    (TIMERPROC)TimerProcComplete);
		}
	}
	//
}


#define selectImageStatus_Max	4
void CGuideDialogUI::selectImageStatus(int nIndex)
{

	switch(nIndex)
	{
	case 0:
		{
			m_pCourseCourse->SetBkImage(_T("file='Guide\\icon_background_selected.png' corner='15,60,5,5'"));
			m_pCourseEdition->SetBkImage(_T("file='Guide\\icon_background_unselected.png' corner='15,60,5,5'"));
			CLabelUI* pLabel = dynamic_cast<CLabelUI*>(m_pCourseEditionLayout->FindSubControl(_T("StatusLabel")));
			pLabel->SetTextColor(0xFFB4B4B4);
			pLabel = dynamic_cast<CLabelUI*>(m_pCourseEditionLayout->FindSubControl(_T("StatusLabel1")));
			pLabel->SetTextColor(0xFFB4B4B4);
			m_pCourseSubEdition->SetBkImage(_T("file='Guide\\icon_background_unselected.png' corner='15,60,5,5'"));
			pLabel = dynamic_cast<CLabelUI*>(m_pCourseSubEditionLayout->FindSubControl(_T("StatusLabel")));
			pLabel->SetTextColor(0xFFB4B4B4);
			pLabel = dynamic_cast<CLabelUI*>(m_pCourseSubEditionLayout->FindSubControl(_T("StatusLabel1")));
			pLabel->SetTextColor(0xFFB4B4B4);

			pLabel = dynamic_cast<CLabelUI*>(m_pCourseGradeLayout->FindSubControl(_T("StatusImage")));
			pLabel->SetBkImage(_T("file='Guide\\icon_num1_selected.png' dest='10,20,30,40'"));

			pLabel = dynamic_cast<CLabelUI*>(m_pCourseCourseLayout->FindSubControl(_T("StatusImage")));
			pLabel->SetBkImage(_T("file='Guide\\icon_num2.png' dest='10,20,30,40'"));

			pLabel = dynamic_cast<CLabelUI*>(m_pCourseEditionLayout->FindSubControl(_T("StatusImage")));
			pLabel->SetBkImage(_T("file='Guide\\icon_num.png' dest='13,25,27,39'"));

			pLabel = dynamic_cast<CLabelUI*>(m_pCourseSubEditionLayout->FindSubControl(_T("StatusImage")));
			pLabel->SetBkImage(_T("file='Guide\\icon_num.png' dest='13,25,27,39'"));
		}
		break;
	case 1:
		{
			m_pCourseEdition->SetBkImage(_T("file='Guide\\icon_background_selected.png' corner='15,60,5,5'"));
			CLabelUI* pLabel = dynamic_cast<CLabelUI*>(m_pCourseEditionLayout->FindSubControl(_T("StatusLabel")));
			pLabel->SetTextColor(0xFFFCAC31);
			pLabel = dynamic_cast<CLabelUI*>(m_pCourseEditionLayout->FindSubControl(_T("StatusLabel1")));
			pLabel->SetTextColor(0xFFFCAC31);

			m_pCourseSubEdition->SetBkImage(_T("file='Guide\\icon_background_unselected.png' corner='15,60,5,5'"));
			pLabel = dynamic_cast<CLabelUI*>(m_pCourseSubEditionLayout->FindSubControl(_T("StatusLabel")));
			pLabel->SetTextColor(0xFFB4B4B4);
			pLabel = dynamic_cast<CLabelUI*>(m_pCourseSubEditionLayout->FindSubControl(_T("StatusLabel1")));
			pLabel->SetTextColor(0xFFB4B4B4);

			pLabel = dynamic_cast<CLabelUI*>(m_pCourseCourseLayout->FindSubControl(_T("StatusImage")));
			pLabel->SetBkImage(_T("file='Guide\\icon_num2_selected.png' dest='10,20,30,40'"));

			pLabel = dynamic_cast<CLabelUI*>(m_pCourseEditionLayout->FindSubControl(_T("StatusImage")));
			pLabel->SetBkImage(_T("file='Guide\\icon_num3.png' dest='10,20,30,40'"));

			pLabel = dynamic_cast<CLabelUI*>(m_pCourseSubEditionLayout->FindSubControl(_T("StatusImage")));
			pLabel->SetBkImage(_T("file='Guide\\icon_num.png' dest='13,25,27,39'"));

		}
		break;
	case 2:
		{
			m_pCourseSubEdition->SetBkImage(_T("file='Guide\\icon_background_selected.png' corner='15,60,5,5'"));
			CLabelUI* pLabel = dynamic_cast<CLabelUI*>(m_pCourseSubEditionLayout->FindSubControl(_T("StatusLabel")));
			pLabel->SetTextColor(0xFF9CBC70);
			pLabel = dynamic_cast<CLabelUI*>(m_pCourseSubEditionLayout->FindSubControl(_T("StatusLabel1")));
			pLabel->SetTextColor(0xFF9CBC70);

			pLabel = dynamic_cast<CLabelUI*>(m_pCourseEditionLayout->FindSubControl(_T("StatusImage")));
			pLabel->SetBkImage(_T("file='Guide\\icon_num3_selected.png' dest='10,20,30,40'"));

			pLabel = dynamic_cast<CLabelUI*>(m_pCourseSubEditionLayout->FindSubControl(_T("StatusImage")));
			pLabel->SetBkImage(_T("file='Guide\\icon_num4.png' dest='10,20,30,40'"));
		}
		break;
	case 3:
		{
			CLabelUI* pLabel = dynamic_cast<CLabelUI*>(m_pCourseSubEditionLayout->FindSubControl(_T("StatusImage")));
			pLabel->SetBkImage(_T("file='Guide\\icon_num4_selected.png' dest='10,20,30,40'"));
		}
		break;
	}

	m_nGuideProgress = nIndex;
}

void CGuideDialogUI::selectButtonStatus(int nIndex)
{
	switch (nIndex)
	{
	case 0:
		{
			TraversalControl(m_pCourseEdition, _T("AutoResizeOptionUI"),false);
			TraversalControl(m_pCourseSubEdition, _T("AutoResizeOptionUI"),false);
		}
		break;
	case 1:
		{
			TraversalControl(m_pCourseSubEdition, _T("AutoResizeOptionUI"),false);
		}
		break;
	}
}

void CGuideDialogUI::TraversalControl(CContainerUI * pContainer, LPCTSTR szName, bool bVisible)
{
	int nCount = pContainer->GetCount();

	CControlUI * pControl;
	for (int i = 0 ; i < nCount ; i++)
	{
		pControl = pContainer->GetItemAt(i);

		if( _tcsicmp(pControl->GetClass(), _T("VerticalLayoutUI")) == 0)
		{
			TraversalControl((CContainerUI * )pControl, szName,bVisible);
		}

		if( _tcsicmp(pControl->GetClass(), _T("HorizontalLayoutUI")) == 0)
		{
			TraversalControl((CContainerUI * )pControl, szName,bVisible);
		}

		if( _tcsicmp(pControl->GetClass(), _T("AutoResizeOptionUI")) == 0)
		{
			CAutoResizeOptionUI * pOption = static_cast<CAutoResizeOptionUI *>(pControl);
			if(pOption->IsSelected())
			{
				pOption->Selected(false);
			}
			pControl->SetEnabled(bVisible);
		}
	}
}

COptionUI* CGuideDialogUI::TraversalOptionSelect(CContainerUI * pContainer)
{
	int nCount = pContainer->GetCount();

	COptionUI* pOption;
	CControlUI * pControl;

	for (int i = 0 ; i < nCount ; i++)
	{
		pControl = pContainer->GetItemAt(i);

		if( _tcsicmp(pControl->GetClass(), _T("VerticalLayoutUI")) == 0)
		{
			pOption = TraversalOptionSelect((CContainerUI * )pControl);
			if(pOption)
				return pOption;
		}

		if( _tcsicmp(pControl->GetClass(), _T("HorizontalLayoutUI")) == 0)
		{
			pOption = TraversalOptionSelect((CContainerUI * )pControl);
			if(pOption)
				return pOption;
		}

		if( _tcsicmp(pControl->GetClass(), _T("AutoHideOptionUI")) == 0)
		{
			CAutoHideOptionUI * pOption = static_cast<CAutoHideOptionUI *>(pControl);

			if(pOption->IsSelected())
			{
				return pOption;
			}
		}
	}

	return NULL;
}

bool CGuideDialogUI::OnGetBookGUID(void * pParam)
{
 	THttpNotify* pNotify = static_cast<THttpNotify*>(pParam);
 
	if(pNotify->dwErrorCode != 0)
	{
		MyMessageBox(1, Course_Failed_Text, _T(""));
		stopLoading();
		return false;
	}

	tstring strBookGUID;
	BOOL bRet = NDCloudDecodeBookGUID(pNotify->pData, pNotify->nDataSize , strBookGUID);

	if(!bRet)
	{
		MyMessageBox(1, Course_Failed_Text, _T(""));
		stopLoading();
		return false;
	}

	if(strBookGUID.length() == 0)
	{
		MyMessageBox(1, _T("该教材下暂无课程信息"), _T(""));
		stopLoading();
		return false;
	}

	tstring strUrl = NDCloudComposeUrlChapterInfo(strBookGUID);

	m_dwDownloadId = NDCloudDownload(strUrl, MakeHttpDelegate(this, &CGuideDialogUI::OnGetChapter));

	return true;
}

bool CGuideDialogUI::OnGetCourse(void * pParam)
{
	THttpNotify* pNotify = static_cast<THttpNotify*>(pParam);

	if(pNotify->dwErrorCode != 0)
	{
		MyMessageBox(1, Course_Failed_Text, _T(""));
		stopLoading();
		return false;
	}

	BOOL bRet = NDCloudDecodeCategory(pNotify->pData, pNotify->nDataSize,m_CategoryTree->GetSelectedSectionCode(),m_pGradeOpt->GetNodeCode(),_T(""),_T(""));

	if(!bRet)
	{
		MyMessageBox(1, Course_Failed_Text, _T(""));
		stopLoading();
		return false;
	}

	::PostMessage(GetHWND(),WM_USER_GET_COURSE,0,0);

	return true;
};

bool CGuideDialogUI::OnGetEdition(void * pParam)
{
	THttpNotify* pNotify = static_cast<THttpNotify*>(pParam);


	if(pNotify->dwErrorCode != 0 || m_pCourseOpt == NULL)
	{
		MyMessageBox(1, Course_Failed_Text, _T(""));
		stopLoading();
		return false;
	}

	BOOL bRet = NDCloudDecodeCategory(pNotify->pData, pNotify->nDataSize,m_CategoryTree->GetSelectedSectionCode(),m_pGradeOpt->GetNodeCode(),m_pCourseOpt->GetNodeCode(),_T(""));

	if(!bRet)
	{
		MyMessageBox(1, Course_Failed_Text, _T(""));
		stopLoading();
		return false;
	}

	::PostMessage(GetHWND(),WM_USER_GET_EDITION,0,0);

	return true;
}

bool CGuideDialogUI::OnGetChapter(void * pParam)
{
	THttpNotify* pNotify = static_cast<THttpNotify*>(pParam);

	if(pNotify->dwErrorCode != 0)
	{
		MyMessageBox(1, Course_Failed_Text, _T(""));
		stopLoading();
		return false;
	}

	NDCloudGetChapterTree(m_ChapterTree);
	if(m_tmpChapterTree != m_ChapterTree)
	{
		m_tmpChapterTree->DestroyTree();
		delete m_tmpChapterTree;
		m_tmpChapterTree = NULL;
	}

	m_tmpChapterTree = new CChapterTree;
	BOOL bRet = NDCloudDecodeChapterInfo(pNotify->pData, pNotify->nDataSize, m_tmpChapterTree);

	if(!bRet)
	{
		MyMessageBox(1, Course_Failed_Text, _T(""));
		stopLoading();
		return false;
	}

	//保存
// 	m_CategoryTree->SelectGradeCode(m_strGradeNodeCode.empty() ? m_pGradeOpt->GetNodeCode() : m_strGradeNodeCode);
// 	m_CategoryTree->SelectCourseCode(m_strCourseNodeCode.empty() ? m_pCourseOpt->GetNodeCode() : m_strCourseNodeCode);
// 	m_CategoryTree->SelectEditionCode(m_strEditionNodeCode.empty() ? m_pEditionOpt->GetNodeCode() : m_strEditionNodeCode);
// 	m_CategoryTree->SelectSubEditionCode(m_strSubEditionNodeCode.empty() ? m_pSubEditionOpt->GetNodeCode() : m_strSubEditionNodeCode);
	m_strSectionNodeCode	= 
	m_strGradeNodeCode		= m_pGradeOpt->GetNodeCode();
	m_strCourseNodeCode		= m_pCourseOpt->GetNodeCode();
	m_strEditionNodeCode	= m_pEditionOpt->GetNodeCode();
	m_strSubEditionNodeCode = m_pSubEditionOpt->GetNodeCode();

	//插入
	m_nSliderCount = 2;
	insertTabLayout(m_tmpChapterTree->GetRootNode(),NDCloudGetChapterGUID());
	//
	stopLoading();

	MyMessageBox(0, Course_Complete_Text, Course_Complete_Text1);

	return true;
}

void CGuideDialogUI::startLoading()
{
	m_pGifAnimLayout->SetVisible(true);
	m_pGifAnim->PlayGif();
}

void CGuideDialogUI::stopLoading()
{
	m_pGifAnim->StopGif();
	m_pGifAnimLayout->SetVisible(false);
}

void CGuideDialogUI::insertTabLayout(ChapterNode * pChapterNode, tstring strNDGuid)
{
	bool bInsertTabLayout = false;

	int nLevel = 1;//层数

	ChapterNode * pChapterTmpNode = pChapterNode;
	while(pChapterTmpNode->pParent)
	{
		nLevel++;
		pChapterTmpNode = pChapterTmpNode->pParent;
	}

	CContainerUI* pGuideChapter = NULL;
	if( nLevel > m_vecGuideChapter.size() )
	{
		bInsertTabLayout = true;
	}
	else
	{
		pGuideChapter = m_vecGuideChapter[nLevel - 1];
	}

	if(bInsertTabLayout)
	{
		//插入
		CDialogBuilder builder;

		pGuideChapter = static_cast<CContainerUI*>(builder.Create(_T("Guide\\GuideChapter.xml"), (UINT)0));
		pGuideChapter->SetName(pChapterNode->strGuid.c_str());
		pGuideChapter->SetText(pChapterNode->strTitle.c_str());

		

		m_pCheckBox		= static_cast<CCheckBoxUI*>(pGuideChapter->FindSubControl(_T("DefaultSetting")));
		if(m_bDefaultSetting)
			m_pCheckBox->SetCheck(true);

		m_vecGuideChapter.push_back(pGuideChapter);

		m_pGuideSwitchTab->Add(pGuideChapter);

		if( nLevel == 1)
		{
			CLabelUI * pLabel = static_cast<CLabelUI*>(pGuideChapter->FindSubControl(_T("title")));
			pLabel->SetText(Course_chapter_title);
		}
		else
		{
			CLabelUI * pLabel = static_cast<CLabelUI*>(pGuideChapter->FindSubControl(_T("title")));

			tstring strTitle;
			for(int i = 1; i < nLevel - 1; i++)
			{
				strTitle += m_vecGuideChapter[i]->GetText();
				strTitle += _T(" > ");
			}
			strTitle += pChapterNode->strTitle;
			pLabel->SetText(strTitle.c_str());
		}

		insertChapterTabLayout(pGuideChapter, pChapterNode, strNDGuid);
	}
	else
	{
		//更新(判断是否刷新新的节点页面)
		tstring strGuid = pGuideChapter->GetName();
		//if(pChapterNode->strGuid != strGuid || nLevel == 1)
		{
			pGuideChapter->SetName(pChapterNode->strGuid.c_str());
			pGuideChapter->SetText(pChapterNode->strTitle.c_str());
			//
			CVerticalLayoutUI * pMainChapter = static_cast<CVerticalLayoutUI*>(pGuideChapter->FindSubControl(_T("mainChapter")));

			pMainChapter->RemoveAll();

			if( nLevel > 1)
			{
				CLabelUI * pLabel = static_cast<CLabelUI*>(pGuideChapter->FindSubControl(_T("title")));

				tstring strTitle;
				for(int i = 1; i < nLevel - 1; i++)
				{
					strTitle += m_vecGuideChapter[i]->GetText();
					strTitle += _T(" > ");
				}
				strTitle += pChapterNode->strTitle;
				pLabel->SetText(strTitle.c_str());
			}

			insertChapterTabLayout(pGuideChapter, pChapterNode, strNDGuid);
		}
	}
}

void CGuideDialogUI::insertChapterTabLayout(CContainerUI* pGuideChapter, ChapterNode * pChapterNode, tstring strNDGuid)
{
	CVerticalLayoutUI * pMainChapter = static_cast<CVerticalLayoutUI*>(pGuideChapter->FindSubControl(_T("mainChapter")));

	//
	CButtonUI * pButton = static_cast<CButtonUI *>(pGuideChapter->FindSubControl(_T("onCreate")));
	pButton->SetEnabled(false);
	pButton->SetBkColor(0xFF969696);
	//////////////////////////////////////////////////////////////////////////

	ChapterNode * pNode = NULL;

	int nCount =0 ;//子孩子总量

	if(pChapterNode && pChapterNode->pFirstChild)
	{
		pNode = pChapterNode->pFirstChild ;

		do
		{
			nCount++;
			pNode = pNode->pNextSlibing;
		}while(pNode);
	}
	int k = 0;
	int nPageIndex = 0;
	if(nCount > 0 && nCount <= 16)//只有1页的情况
	{
		CSliderTabLayoutUI * pSliderTabLayout = new CSliderTabLayoutUI;
		CHorizontalLayoutUI * pHorizontalLayout = NULL;
		CVerticalLayoutUI * pVerticalLayout = NULL;


		pNode = pChapterNode->pFirstChild ;

		do
		{
			if(k % 16 == 0)
			{
				pHorizontalLayout = new CHorizontalLayoutUI;
				pHorizontalLayout->SetFixedHeight(410);
				pSliderTabLayout->Add(pHorizontalLayout);
				nPageIndex++ ;
			}
			if(k % 8 == 0)
			{
				pVerticalLayout = new CVerticalLayoutUI;
				pHorizontalLayout->Add(pVerticalLayout);
			}

			tstring strName = pNode->strTitle;
			tstring strGuid = pNode->strGuid;

			CAutoHideOptionUI * pOption = new CAutoHideOptionUI;
			pOption->SetText(strName.c_str());
			pOption->SetGuid(strGuid);
			pOption->SetFixedHeight(40);
			pOption->SetGroup(pChapterNode->strGuid.c_str());
			pOption->SetTextColor(0xFF000001);
			//			pOption->SetSelectedBkColor(Color(255,17,176,182).GetValue());
			pOption->SetSelectedImage(_T("file='Guide\\course_bk.png' corner='5,5,28,5'"));
			if(pNode->pFirstChild)
			{
				pOption->SetNormalImage(_T("file='Guide\\course_plus.png' corner='5,5,28,5'"));
			}
			pOption->SetBorderSize(1);
			pOption->SetBorderColor(0xFFD4D4D4);
			pOption->SetFont(130100);
			pOption->SetTextColor(0xFF11B0B6);
			pOption->SetTextPadding(CDuiRect(10,8,0,0));
			pOption->SetSelectedTextColor(0xFFFFFFFF);
			pOption->SetPadding(CDuiRect(20,10,0,0));
			pOption->SetFixedWidth(430);
			pOption->setChapterNode(pNode);
			pOption->SetBkColor(0xFFFFFFFF);

			if(strNDGuid.length() > 0 && m_ChapterTree)
			{
				ChapterNode* pNode = m_ChapterTree->FindNode(strNDGuid);
				while(pNode)
				{
					if(strGuid == pNode->strGuid)
					{
						pOption->Selected(true);
						m_pChapterOpt = pOption;
						int n = nPageIndex;
						pSliderTabLayout->SelectItem(--n,false);
						if(!pNode->pFirstChild)
						{
							CButtonUI * pButton = static_cast<CButtonUI *>(pGuideChapter->FindSubControl(_T("onCreate")));
							pButton->SetEnabled(true);
							pButton->SetBkColor(0xFFFA851B);
						}
						break;
					}
					pNode = pNode->pParent;
				}		
			}

			pVerticalLayout->Add(pOption);

			k++;

			pNode = pNode->pNextSlibing;

		}while(pNode);

		pMainChapter->Add(pSliderTabLayout);
	}
	else if(nCount>0 && nCount> 16)
	{
		CSliderTabLayoutUI * pSliderTabLayout = new CSliderTabLayoutUI;
		CHorizontalLayoutUI * pHorizontalLayout = NULL;
		CVerticalLayoutUI * pVerticalLayout = NULL;


		pNode = pChapterNode->pFirstChild ;

		do
		{
			if(k % 14 == 0)
			{
				pHorizontalLayout = new CHorizontalLayoutUI;
				pHorizontalLayout->SetFixedHeight(410);
				pSliderTabLayout->Add(pHorizontalLayout);
				nPageIndex++ ;
			}
			if(k % 7 == 0)
			{
				pVerticalLayout = new CVerticalLayoutUI;
				pHorizontalLayout->Add(pVerticalLayout);
			}

			tstring strName = pNode->strTitle;
			tstring strGuid = pNode->strGuid;

			CAutoHideOptionUI * pOption = new CAutoHideOptionUI;
			pOption->SetText(strName.c_str());
			pOption->SetGuid(strGuid);
			pOption->SetFixedHeight(40);
			pOption->SetGroup(pChapterNode->strGuid.c_str());
			pOption->SetTextColor(0xFF000001);
			//			pOption->SetSelectedBkColor(Color(255,17,176,182).GetValue());
			pOption->SetSelectedImage(_T("file='Guide\\course_bk.png' corner='5,5,28,5'"));
			if(pNode->pFirstChild)
			{
				pOption->SetNormalImage(_T("file='Guide\\course_plus.png' corner='5,5,28,5'"));
			}
			pOption->SetBorderSize(1);
			pOption->SetBorderColor(0xFFD4D4D4);
			pOption->SetFont(130100);
			pOption->SetTextColor(0xFF11B0B6);
			pOption->SetTextPadding(CDuiRect(10,8,0,0));
			pOption->SetSelectedTextColor(0xFFFFFFFF);
			pOption->SetPadding(CDuiRect(20,10,0,0));
			pOption->SetFixedWidth(430);
			pOption->setChapterNode(pNode);
			pOption->SetBkColor(0xFFFFFFFF);

			if(strNDGuid.length() > 0 && m_ChapterTree)
			{
				ChapterNode* pNode = m_ChapterTree->FindNode(strNDGuid);
				while(pNode)
				{
					if(strGuid == pNode->strGuid)
					{
						pOption->Selected(true);
						m_pChapterOpt = pOption;
						int n = nPageIndex;
						pSliderTabLayout->SelectItem(--n,false);
						if(!pNode->pFirstChild)
						{
							CButtonUI * pButton = static_cast<CButtonUI *>(pGuideChapter->FindSubControl(_T("onCreate")));
							pButton->SetEnabled(true);
							pButton->SetBkColor(0xFFFA851B);
						}
						break;
					}
					pNode = pNode->pParent;
				}		
			}

			pVerticalLayout->Add(pOption);

			k++;

			pNode = pNode->pNextSlibing;

		}while(pNode);

		pMainChapter->Add(pSliderTabLayout);

		//if(nPageIndex > 1)
		{
			pHorizontalLayout = new CHorizontalLayoutUI;
			pHorizontalLayout->SetFixedHeight(30);
			CControlUI * pControl = new CControlUI;
			pHorizontalLayout->Add(pControl);

			CButtonUI* pButton = new CButtonUI;
			pButton->SetFixedWidth(100);
			pButton->SetTextColor(0xFF969696);
			pButton->SetHotTextColor(0xFF11B0B6);
			pButton->SetText(_T("< 上一页"));
			pButton->SetName(_T("subPreBtn"));
			pButton->OnNotify += MakeDelegate(this, &CGuideDialogUI::OnChapterChanged);
			if(pSliderTabLayout->GetCurSel() == 0)
				pButton->SetEnabled(false);
			pButton->SetTag((UINT_PTR)pSliderTabLayout);
			pHorizontalLayout->Add(pButton);
			pButton = new CButtonUI;
			pButton->SetFixedWidth(100);
			pButton->SetTextColor(0xFF969696);
			pButton->SetHotTextColor(0xFF11B0B6);
			pButton->SetText(_T("下一页 >"));
			pButton->SetName(_T("subNextBtn"));
			pButton->OnNotify += MakeDelegate(this, &CGuideDialogUI::OnChapterChanged);
			if(pSliderTabLayout->GetCurSel() == pSliderTabLayout->GetCount() - 1)
				pButton->SetEnabled(false);

			pButton->SetTag((UINT_PTR)pSliderTabLayout);
			pHorizontalLayout->Add(pButton);

			pControl = new CControlUI;
			pHorizontalLayout->Add(pControl);

			pMainChapter->Add(pHorizontalLayout);
		}
	}
}

bool CGuideDialogUI::GetClickedChapterNode( CContainerUI * pContainer, ChapterNode * pChapterNode, tstring& strGuid )
{
	//遍历

	int nCount = pContainer->GetCount();

	CControlUI * pControl;
	for (int i = 0 ; i < nCount ; i++)
	{
		pControl = pContainer->GetItemAt(i);

		if( _tcsicmp(pControl->GetClass(), _T("VerticalLayoutUI")) == 0)
		{
			bool b = GetClickedChapterNode((CContainerUI * )pControl, pChapterNode, strGuid);
			if(b)
				return b;
		}

		if( _tcsicmp(pControl->GetClass(), _T("HorizontalLayoutUI")) == 0)
		{
			bool b = GetClickedChapterNode((CContainerUI * )pControl, pChapterNode, strGuid);
			if(b)
				return b;
		}

		if( _tcsicmp(pControl->GetClass(), _T("SliderTabLayoutUI")) == 0)
		{
			bool b = GetClickedChapterNode((CContainerUI * )pControl, pChapterNode, strGuid);
			if(b)
				return b;
		}
		
		if( _tcsicmp(pControl->GetClass(), _T("AutoHideOptionUI")) == 0)
		{
			CAutoHideOptionUI * pOption = static_cast<CAutoHideOptionUI *>(pControl);
			if(pOption->IsSelected())
			{
				strGuid = pOption->GetGuid();
				return true;
			}
		}
	}
	return false;
}

bool CGuideDialogUI::OnChapterChanged( void* pNotify )
{
	TNotifyUI* pNotifyUI = (TNotifyUI*)pNotify;

	if (pNotifyUI->sType == _T("click"))
	{
		CButtonUI* pButton = static_cast<CButtonUI*>(pNotifyUI->pSender);
		CSliderTabLayoutUI * pSliderTabLayout = (CSliderTabLayoutUI*)pButton->GetTag();
		int nIndex = pSliderTabLayout->GetCurSel();
		int nCount = pSliderTabLayout->GetCount();

		if(pButton->GetText() == _T("< 上一页"))
		{
			if(nIndex > 0)
			{
				pSliderTabLayout->SelectItem(--nIndex, true, false);
				m_pChapterMask->SetVisible(true);
				if(nIndex == 0) 
				{
					pButton->SetEnabled(false);
				}
				CHorizontalLayoutUI * pLayout = static_cast<CHorizontalLayoutUI *>(pButton->GetParent());
				pButton = static_cast<CButtonUI *>(pLayout->FindSubControl(_T("subNextBtn")));
				pButton->SetEnabled(true);
			}
		}
		else if(pButton->GetText() == _T("下一页 >"))
		{
			if(nIndex < nCount - 1)
			{
				pSliderTabLayout->SelectItem(++nIndex, true, false);
				m_pChapterMask->SetVisible(true);
				if(nIndex == nCount - 1) 
				{
					pButton->SetEnabled(false);
				}
				CHorizontalLayoutUI * pLayout = static_cast<CHorizontalLayoutUI *>(pButton->GetParent());
				pButton = static_cast<CButtonUI *>(pLayout->FindSubControl(_T("subPreBtn")));
				pButton->SetEnabled(true);
			}
		}
	}

	return true;
}

void CGuideDialogUI::ResizeOptText(COptionUI* pOption,bool bResize)
{
	return ;
	if(pOption)
	{
		tstring strText = pOption->GetText();
		if(strText.length() <= 10 && bResize)
		{
			pOption->SetTextPadding(CDuiRect(-20,0,0,0));
		}
		else
		{
			pOption->SetTextPadding(CDuiRect(0,0,0,0));
		}
	}
}

void CGuideDialogUI::OnSwitchChapter(int nIndex, bool bPreSwitch, bool bRefreshData, bool bAnim)
{
	if(bAnim)
		m_pChapterMask->SetVisible(true);

	m_bPreSwitch = bPreSwitch;
	m_bRefreshData = bRefreshData;
	m_pGuideSwitchTab->SelectItem(nIndex,bAnim);

	if(bRefreshData)
		m_nSliderCount = nIndex + 1;

	if(bAnim)
		return;

	
	OnSwitchChapterFinish();
	
}

void CGuideDialogUI::OnSwitchChapterFinish()
{
	int n = m_pGuideSwitchTab->GetCurSel();

	CButtonUI*	pButton			= NULL;
	CContainerUI * pContainer	= NULL;
	CLabelUI * pIconLabel		= NULL;
	CLabelUI * pTextLabel		= NULL;
	CHorizontalLayoutUI* pTabbarLayout = NULL;
	if(n == 2 && m_bPreSwitch == false || n < 2)
	{
		for(int i = 0 ; i < n  ; i++)
		{
			pIconLabel = m_vecGuideTabbar[i].pIconLabel;
			pTextLabel = m_vecGuideTabbar[i].pTextLabel;
			pTabbarLayout = m_vecGuideTabbar[i].pTabbarLayout;

			pTextLabel->SetTextColor(0xFF12AFB6);

			pTabbarLayout->SetBkImage(_T("file='Guide\\bg_tab_selected.png' corner='20,0,20,0'"));
			pIconLabel->SetBkImage(_T("file='Guide\\icon_tab_selected.png' dest='0,6,23,26' source='22,0,45,20'"));
			pIconLabel->SetVisible(true);
		}
		pIconLabel = m_vecGuideTabbar[n].pIconLabel;
		pTextLabel = m_vecGuideTabbar[n].pTextLabel;
		pTabbarLayout = m_vecGuideTabbar[n].pTabbarLayout;

		pTextLabel->SetTextColor(0xFF12AFB6);

		pTabbarLayout->SetBkImage(_T("file='Guide\\bg_tab_normal.png' corner='20,0,20,0'"));
		pIconLabel->SetVisible(false);

		for(int i = n + 1 ; i < m_vecGuideTabbar.size()  ; i++)
		{
			pIconLabel = m_vecGuideTabbar[i].pIconLabel;
			pTextLabel = m_vecGuideTabbar[i].pTextLabel;
			pTabbarLayout = m_vecGuideTabbar[i].pTabbarLayout;

			pTextLabel->SetTextColor(0xFFFFFFFF);

			pTabbarLayout->SetBkImage(_T("file='Guide\\bg_tab_unselected.png' corner='20,0,20,0'"));
			pIconLabel->SetVisible(false);
		}

		if(m_bRefreshData)
		{
			for(int i = 3; i < m_vecGuideTabbar.size(); i++)
			{
				pContainer = m_vecGuideTabbar[i].pTabbarContanier;
				pIconLabel = m_vecGuideTabbar[i].pIconLabel;
				pTextLabel = m_vecGuideTabbar[i].pTextLabel;
				pTabbarLayout = m_vecGuideTabbar[i].pTabbarLayout;

				pTabbarLayout->SetBkImage(_T("file='Guide\\bg_tab_unselected.png' corner='20,0,20,0'"));

				pContainer->SetVisible(false);
				pIconLabel->SetVisible(false);
			}

			RECT rect;
			for(int i = 0; i < 3; i++)
			{
				pContainer = m_vecGuideTabbar[i].pTabbarContanier;
				SetRect(&rect, 50 + i * 888/3 - i * 12 ,0, 50 + i* 888/3 - i * 12 + 888/3, 31);
				pContainer->SetPos(rect);
			}
		}
		

	}
	else
	{
		RECT rect;
		if(n >= m_vecGuideTabbar.size())
		{
			for(int i = 0 ; i <= n - m_vecGuideTabbar.size() + 1; i++)
			{
				CContainerUI * pContainer = new CContainerUI;
				CHorizontalLayoutUI * pLayout1 = new CHorizontalLayoutUI;
				CButtonUI * pButton = new CButtonUI;
				pLayout1->SetInset(CDuiRect(10,0,10,0));
				pLayout1->Add(pButton);

				pButton->OnNotify += MakeDelegate(this, &CGuideDialogUI::OnTabbarNotify);

				CHorizontalLayoutUI * pLayout = new CHorizontalLayoutUI;
				pContainer->SetFloat(true);
				CLabelUI * pTextLabel = new CLabelUI;
				CLabelUI * pIconLabel = new CLabelUI;
				CControlUI * pLeftControl = new CControlUI;
				CControlUI * pRightControl = new CControlUI;

				pTextLabel->SetTextColor(0xFF12AFB6);
				pTextLabel->SetAttribute(_T("align"),_T("center"));
				pIconLabel->SetVisible(false);

				pTextLabel->SetFont(140000);
				pLayout->Add(pLeftControl);
				pLayout->Add(pTextLabel);
				pLayout->Add(pIconLabel);
				pLayout->Add(pRightControl);

				pIconLabel->SetFixedWidth(23);

				tstring strTitle = _T("请选择");
				for(int i = 2; i < n; i++)
				{
					strTitle += _T("小");
				}
				strTitle += _T("节");

				pTextLabel->SetText(strTitle.c_str());
				//计算文本宽度
				RECT rcCalc = {0};
				rcCalc.right = 296;

				this->CalcText(m_PaintManager.GetPaintDC(), rcCalc, pTextLabel->GetText(), pTextLabel->GetFont(), DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT|DT_NOPREFIX, UIFONT_GDI);
				pTextLabel->SetFixedWidth(rcCalc.right);

				m_pGuideTabBarLayout->Add(pContainer);
				pContainer->Add(pLayout);
				pContainer->Add(pLayout1);

				GUIDETABBAR GuideTabbar;
				GuideTabbar.pTabbarContanier		= pContainer;
				GuideTabbar.pButton					= pButton;
				GuideTabbar.pTabbarLayout			= pLayout;
				GuideTabbar.pTextLabel				= pTextLabel;
				GuideTabbar.pIconLabel				= pIconLabel;
				m_vecGuideTabbar.push_back(GuideTabbar);
			}
		}

		if(m_bRefreshData)
		{
			for(int i = 0; i <= n; i++)
			{
				pButton	   = m_vecGuideTabbar[i].pButton;
				pContainer = m_vecGuideTabbar[i].pTabbarContanier;
				pIconLabel = m_vecGuideTabbar[i].pIconLabel;
				pTextLabel = m_vecGuideTabbar[i].pTextLabel;
				pTabbarLayout = m_vecGuideTabbar[i].pTabbarLayout;

				SetRect(&rect, 50 + i * 888/(n + 1) - 10, 0, 50 + i* 888/(n + 1) + 888/(n + 1), 31);
				pContainer->SetPos(rect);
				pContainer->SetVisible(true);
				if(i == n)
				{
					pTabbarLayout->SetBkImage(_T("file='Guide\\bg_tab_normal.png' corner='20,0,20,0'"));
					pIconLabel->SetVisible(false);
				}
				else
				{
					pTextLabel->SetTextColor(0xFF12AFB6);
					pTabbarLayout->SetBkImage(_T("file='Guide\\bg_tab_selected.png' corner='20,0,20,0'"));
					pIconLabel->SetBkImage(_T("file='Guide\\icon_tab_selected.png' dest='0,6,23,26' source='22,0,45,20'"));
					pIconLabel->SetVisible(true);
				}
			}

			for(int i = n + 1; i < m_vecGuideTabbar.size(); i++)
			{
				pContainer = m_vecGuideTabbar[i].pTabbarContanier;
				pIconLabel = m_vecGuideTabbar[i].pIconLabel;
				pTextLabel = m_vecGuideTabbar[i].pTextLabel;
				pTabbarLayout = m_vecGuideTabbar[i].pTabbarLayout;

				pContainer->SetVisible(false);
			}
		}
		else
		{
			for(int i = 0; i <= n; i++)
			{
				pButton	   = m_vecGuideTabbar[i].pButton;
				pContainer = m_vecGuideTabbar[i].pTabbarContanier;
				pIconLabel = m_vecGuideTabbar[i].pIconLabel;
				pTextLabel = m_vecGuideTabbar[i].pTextLabel;
				pTabbarLayout = m_vecGuideTabbar[i].pTabbarLayout;

				if(i == n)
				{
					pTabbarLayout->SetBkImage(_T("file='Guide\\bg_tab_normal.png' corner='20,0,20,0'"));
					pIconLabel->SetVisible(false);
				}
				else
				{
					pTextLabel->SetTextColor(0xFF12AFB6);
					pTabbarLayout->SetBkImage(_T("file='Guide\\bg_tab_selected.png' corner='20,0,20,0'"));
					pIconLabel->SetBkImage(_T("file='Guide\\icon_tab_selected.png' dest='0,6,23,26' source='22,0,45,20'"));
					pIconLabel->SetVisible(true);
				}
			}

			for(int i = n + 1; i < m_vecGuideTabbar.size(); i++)
			{
				pContainer = m_vecGuideTabbar[i].pTabbarContanier;
				pIconLabel = m_vecGuideTabbar[i].pIconLabel;
				pTextLabel = m_vecGuideTabbar[i].pTextLabel;
				pTabbarLayout = m_vecGuideTabbar[i].pTabbarLayout;
				pTextLabel->SetTextColor(0xFFFFFFFF);

				pTabbarLayout->SetBkImage(_T("file='Guide\\bg_tab_unselected.png' corner='20,0,20,0'"));
				pIconLabel->SetVisible(false);
			}

		}
	}

	//检测创建按钮是否选中
	//
}

bool CGuideDialogUI::OnChapterSwitchFinish( void* pObj )
{
	::PostMessage(this->GetHWND(), WM_USER_SWITCH_CHAPTER , 0 ,0);
	return true;
}

void CGuideDialogUI::MyMessageBox(int nType, LPCTSTR pstr, LPCTSTR pstr1)
{
	m_pCompleteTipText->SetText(pstr);
	m_pCompleteTipText1->SetText(pstr1);

	if(nType == 0)
	{
		m_pCompleteTipText->SetBkImage(_T("file='Guide\\icon_congratulation.png' dest='138,20,182,64'"));
		m_pCompleteTipText->SetTextColor(0xFFF88311);
		m_pCompleteTipText1->SetTextPadding(CDuiRect(0,80,0,0));
		m_pCompleteTipText1->SetTextColor(0xFFEEEEEE);
		SetTimer(m_hWnd, Course_Complete_id, 1000, (TIMERPROC)TimerProcComplete);
	}
	else
	{
		m_pCompleteTipText->SetBkImage(_T("file='Guide\\icon_fail.png' dest='138,20,182,64'"));
		m_pCompleteTipText->SetTextColor(0xFFF24646);
		SetTimer(m_hWnd, Course_Failed_id, 1000, (TIMERPROC)TimerProcComplete);
	}

	m_pCompleteTip->SetVisible(true);
}

void CGuideDialogUI::CalcText( HDC hdc, RECT& rc, LPCTSTR lpszText, int nFontId, UINT format, UITYPE_FONT nFontType, int c /*= -1*/ )
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

bool CGuideDialogUI::OnTabbarNotify( void* pNotify )
{
	TNotifyUI* pNotifyUI = (TNotifyUI*)pNotify;
	if (pNotifyUI->sType == _T("click"))
	{
		int nIndex = m_pGuideSwitchTab->GetCurSel();

		for(int i = 0 ; i < m_vecGuideTabbar.size() ; i++)
		{
			if(m_vecGuideTabbar[i].pButton == pNotifyUI->pSender)
			{
				if(i < nIndex)
				{
					OnSwitchChapter(i, true, false );
				}
				else if(i > nIndex && i < m_nSliderCount)
				{
					OnSwitchChapter(i, false, false );
				}
			}
		}
	}
	
	return true;
}

tstring CGuideDialogUI::GetSectionNodeCode( tstring strGradeNodeCode )
{
	CategoryNode * m_pRoot = m_CategoryTree->GetRootNode();

	// section
	CategoryNode* pSection = m_pRoot->pFirstChild;
	CategoryNode* pGradeNode = NULL;

	while( pSection != NULL )
	{
		pGradeNode = pSection->pFirstChild;
		while( pGradeNode != NULL )
		{
			if( pGradeNode->strNDCode == strGradeNodeCode )
			{
				return pGradeNode->pParent->strNDCode;
			}
			pGradeNode = pGradeNode->pNextSlibing;
		}
		pSection = pSection->pNextSlibing;
	}
	return _T("");
}

tstring CGuideDialogUI::GetTempChapterGUID()
{
	return m_strTempChapterGuid;
}

//////////////////////////////////////////////////////////////////////////
// CGuideGradeUI
//////////////////////////////////////////////////////////////////////////
CGuideGradeUI::CGuideGradeUI()
{
	CDialogBuilder builder;
	CGuideGradeDialogBuilderCallbackEx callback;
	CContainerUI* pGuideGrade = static_cast<CContainerUI*>(builder.Create(_T("Guide\\GuideGrade.xml"), (UINT)0, &callback));
	if( pGuideGrade ) 
	{
		this->Add(pGuideGrade);
	}
	else 
	{
		this->RemoveAll();
		return;
	}
}

CGuideGradeUI::~CGuideGradeUI()
{

}

//////////////////////////////////////////////////////////////////////////
// CMainCourseUI
//////////////////////////////////////////////////////////////////////////
#define LOGO_POS		30
#define LOGO_POS_LEFT	103
#define LOGO_WIDTH		4
#define DIALOG_HEIGHT	499
void CMainCourseUI::PaintBkImage( HDC hDC )
{
	
	SIZE z1 = this->GetScrollRange();

	int nGuideProgress = m_pCGuideDialogUI->m_nGuideProgress;

	RECT rect = m_rcPaint;
	

	Color colors[] =
	{
		0xFF11B0B6,		//绿色
		0xFF58A8E7,		//蓝色
		0xFFDE77ED,		//紫色
		0xFFFCAC31,		//橘色
		0xFF9CBC70,		//浅绿
	};

	float positions[5];
	positions[0] = 0.0f;
	int nTotalHeight;

	if(z1.cy == 0)//无滚动条
	{
		Gdiplus::Graphics graphics(hDC);

		switch(nGuideProgress)
		{
		case 0:
			{
				nTotalHeight = m_pCGuideDialogUI->m_LayoutHeight[0] + m_pCGuideDialogUI->m_LayoutHeight[1] +
					m_pCGuideDialogUI->m_LayoutHeight[2] + LOGO_POS;
				positions[1] = float(m_pCGuideDialogUI->m_LayoutHeight[0] + LOGO_POS) / nTotalHeight;
				positions[2] = 1.0f;
			}
			break;
		case 1:
			{
				nTotalHeight = m_pCGuideDialogUI->m_LayoutHeight[0] + m_pCGuideDialogUI->m_LayoutHeight[1] +
					m_pCGuideDialogUI->m_LayoutHeight[2] + m_pCGuideDialogUI->m_LayoutHeight[3] +
					m_pCGuideDialogUI->m_LayoutHeight[4] + LOGO_POS;
				positions[1] = float(m_pCGuideDialogUI->m_LayoutHeight[0] + LOGO_POS) / nTotalHeight;
				positions[2] = float(m_pCGuideDialogUI->m_LayoutHeight[0] + m_pCGuideDialogUI->m_LayoutHeight[1] + 
					m_pCGuideDialogUI->m_LayoutHeight[2] + LOGO_POS) / nTotalHeight;
				positions[3] = 1.0f;
			}
			break;
		case 2:
		case 3:
			{
				nTotalHeight = m_pCGuideDialogUI->m_LayoutHeight[0] + m_pCGuideDialogUI->m_LayoutHeight[1] +
					m_pCGuideDialogUI->m_LayoutHeight[2] + m_pCGuideDialogUI->m_LayoutHeight[3] +
					m_pCGuideDialogUI->m_LayoutHeight[4] + m_pCGuideDialogUI->m_LayoutHeight[5] +
					m_pCGuideDialogUI->m_LayoutHeight[6] + LOGO_POS;
				positions[1] = float(m_pCGuideDialogUI->m_LayoutHeight[0] + LOGO_POS) / nTotalHeight;
				positions[2] = float(m_pCGuideDialogUI->m_LayoutHeight[0] + m_pCGuideDialogUI->m_LayoutHeight[1] + 
					m_pCGuideDialogUI->m_LayoutHeight[2] + LOGO_POS) / nTotalHeight;
				positions[3] = float(m_pCGuideDialogUI->m_LayoutHeight[0] + m_pCGuideDialogUI->m_LayoutHeight[1] + 
					m_pCGuideDialogUI->m_LayoutHeight[2] + m_pCGuideDialogUI->m_LayoutHeight[3] + 
					m_pCGuideDialogUI->m_LayoutHeight[4] + LOGO_POS) / nTotalHeight;
				positions[4] = 1.0f;
			}
			break;
		}

		//构造一条从黑色到白色的渐变画刷
		LinearGradientBrush linGrBrush(
			Point(rect.left + LOGO_POS_LEFT, rect.top - 1),
			Point(rect.left + LOGO_POS_LEFT, rect.top + nTotalHeight),
			Color::Black,Color::White);

		linGrBrush.SetInterpolationColors(colors, positions, (3 + nGuideProgress) > 5 ? 5 : 3 + nGuideProgress);
		graphics.FillRectangle(&linGrBrush, rect.left + LOGO_POS_LEFT, rect.top - 1, LOGO_WIDTH, nTotalHeight);

		int nTotalHeight1 = m_pCGuideDialogUI->m_LayoutHeight[0] + m_pCGuideDialogUI->m_LayoutHeight[1] +
			m_pCGuideDialogUI->m_LayoutHeight[2] + m_pCGuideDialogUI->m_LayoutHeight[3] +
			m_pCGuideDialogUI->m_LayoutHeight[4] + m_pCGuideDialogUI->m_LayoutHeight[5] +
			m_pCGuideDialogUI->m_LayoutHeight[6] + LOGO_POS;

		SolidBrush brush(0xFFE1E1E1);
		graphics.FillRectangle(&brush, rect.left + LOGO_POS_LEFT, rect.top + nTotalHeight - 1, LOGO_WIDTH, nTotalHeight1 - nTotalHeight);
	}
	else//有滚动条
	{
		SIZE z = this->GetScrollPos();

		CDC* pDC = CDC::FromHandle(hDC);
		CDC MemDC; 
		CBitmap MemBitmap;
		MemDC.CreateCompatibleDC(NULL); 
		MemBitmap.CreateCompatibleBitmap(pDC,GetManager()->GetClientSize().cx, GetManager()->GetClientSize().cy); 
		CBitmap *pOldBit=MemDC.SelectObject(&MemBitmap); 
		Gdiplus::Graphics graphics(MemDC.GetSafeHdc());


		switch(nGuideProgress)
		{
		case 0:
			{
				nTotalHeight = m_pCGuideDialogUI->m_LayoutHeight[0] + m_pCGuideDialogUI->m_LayoutHeight[1] +
					m_pCGuideDialogUI->m_LayoutHeight[2] + LOGO_POS;
				positions[1] = float(m_pCGuideDialogUI->m_LayoutHeight[0] + LOGO_POS) / nTotalHeight;
				positions[2] = 1.0f;
			}
			break;
		case 1:
			{
				nTotalHeight = m_pCGuideDialogUI->m_LayoutHeight[0] + m_pCGuideDialogUI->m_LayoutHeight[1] +
					m_pCGuideDialogUI->m_LayoutHeight[2] + m_pCGuideDialogUI->m_LayoutHeight[3] +
					m_pCGuideDialogUI->m_LayoutHeight[4] + LOGO_POS;
				positions[1] = float(m_pCGuideDialogUI->m_LayoutHeight[0] + LOGO_POS) / nTotalHeight;
				positions[2] = float(m_pCGuideDialogUI->m_LayoutHeight[0] + m_pCGuideDialogUI->m_LayoutHeight[1] + 
					m_pCGuideDialogUI->m_LayoutHeight[2] + LOGO_POS) / nTotalHeight;
				positions[3] = 1.0f;
			}
			break;
		case 2:
		case 3:
			{
				nTotalHeight = m_pCGuideDialogUI->m_LayoutHeight[0] + m_pCGuideDialogUI->m_LayoutHeight[1] +
					m_pCGuideDialogUI->m_LayoutHeight[2] + m_pCGuideDialogUI->m_LayoutHeight[3] +
					m_pCGuideDialogUI->m_LayoutHeight[4] + m_pCGuideDialogUI->m_LayoutHeight[5] +
					m_pCGuideDialogUI->m_LayoutHeight[6] + LOGO_POS;
				positions[1] = float(m_pCGuideDialogUI->m_LayoutHeight[0] + LOGO_POS) / nTotalHeight;
				positions[2] = float(m_pCGuideDialogUI->m_LayoutHeight[0] + m_pCGuideDialogUI->m_LayoutHeight[1] + 
					m_pCGuideDialogUI->m_LayoutHeight[2] + LOGO_POS) / nTotalHeight;
				positions[3] = float(m_pCGuideDialogUI->m_LayoutHeight[0] + m_pCGuideDialogUI->m_LayoutHeight[1] + 
					m_pCGuideDialogUI->m_LayoutHeight[2] + m_pCGuideDialogUI->m_LayoutHeight[3] + 
					m_pCGuideDialogUI->m_LayoutHeight[4] + LOGO_POS) / nTotalHeight;
				positions[4] = 1.0f;
			}
			break;
		}

		//构造一条从黑色到白色的渐变画刷
		LinearGradientBrush linGrBrush(
			Point(rect.left + LOGO_POS_LEFT, rect.top - 1),
			Point(rect.left + LOGO_POS_LEFT, rect.top + nTotalHeight),
			Color::Black,Color::White);

		linGrBrush.SetInterpolationColors(colors, positions, (3 + nGuideProgress) > 5 ? 5 : 3 + nGuideProgress);
		graphics.FillRectangle(&linGrBrush, rect.left + LOGO_POS_LEFT, rect.top - 1, LOGO_WIDTH, nTotalHeight);

		int nTotalHeight1 = m_pCGuideDialogUI->m_LayoutHeight[0] + m_pCGuideDialogUI->m_LayoutHeight[1] +
			m_pCGuideDialogUI->m_LayoutHeight[2] + m_pCGuideDialogUI->m_LayoutHeight[3] +
			m_pCGuideDialogUI->m_LayoutHeight[4] + m_pCGuideDialogUI->m_LayoutHeight[5] +
			m_pCGuideDialogUI->m_LayoutHeight[6] + LOGO_POS;

		SolidBrush brush(0xFFE1E1E1);
		graphics.FillRectangle(&brush, rect.left + LOGO_POS_LEFT, rect.top + nTotalHeight - 1, LOGO_WIDTH, nTotalHeight1 - nTotalHeight);

		pDC->BitBlt(rect.left + LOGO_POS_LEFT, rect.top - 1, LOGO_WIDTH, z.cy + DIALOG_HEIGHT < nTotalHeight1 ? DIALOG_HEIGHT : nTotalHeight1 - z.cy , &MemDC, rect.left + LOGO_POS_LEFT, rect.top + z.cy, SRCCOPY);

		graphics.ReleaseHDC(MemDC.GetSafeHdc());
		MemBitmap.DeleteObject(); 
		MemDC.DeleteDC(); 
	}
	/*
	
	*/
	__super::PaintBkImage(hDC);
}
