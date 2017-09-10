#include "stdafx.h"
#include "Introduction.h"
#include "Util/Util.h"

CIntroductionDialogUI::CIntroductionDialogUI()
{
	m_nIndex = 0;
	m_nImagesCount = 0;
}


CIntroductionDialogUI::~CIntroductionDialogUI()
{
	StopAnim();
}

LPCTSTR CIntroductionDialogUI::GetWindowClassName() const
{
	return _T("IntroductionDialog");
}

CDuiString CIntroductionDialogUI::GetSkinFile()
{
	return _T("Introduction\\Introduction.xml");
}

CDuiString CIntroductionDialogUI::GetSkinFolder()
{
	return CDuiString(_T("skins"));
}

CControlUI* CIntroductionDialogUI::CreateControl(LPCTSTR pstrClass)
{
	if( _tcsicmp(pstrClass, _T("SliderTabLayoutUI")) == 0)
	{
		m_pSliderTabLayout = new CSliderTabLayoutUI;
		return m_pSliderTabLayout;
	}
	return NULL;
}

void CIntroductionDialogUI::InitWindow()
{
	m_pSliderBtnLayout = dynamic_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("sliderBtnLayout")));
}

void CIntroductionDialogUI::Init(CRect& rect)
{
	int nMainWindowWidth = 720;
	int nMainWindowHeight = 520;

	MoveWindow(GetHWND(), rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
//	::MoveWindow(GetHWND(), rect.left + (rect.Width() - nMainWindowWidth)/2, rect.top + (rect.Height() - nMainWindowHeight)/2, nMainWindowWidth, nMainWindowHeight, TRUE);
//  	m_WndShadow.SetMaskSize(rect);
//  	m_WndShadow.Create(m_hWnd);


	for(int i =0 ; i < m_nImagesCount ; i++)
	{
		CControlUI* pControl = new CControlUI;
		pControl->SetFixedWidth(15);
		m_pSliderBtnLayout->Add(pControl);

		CButtonUI* pButton = new CButtonUI;
		pButton->SetFixedWidth(10);
		pButton->SetFixedHeight(10);
		if(i == 0)
			pButton->SetBkImage(_T("Introduction\\icon_green.png"));
		else
			pButton->SetBkImage(_T("Introduction\\icon_white.png"));
		pButton->SetPadding(CDuiRect(0,6,0,0));
		m_pSliderBtnLayout->Add(pButton);

		m_mapSliderButton[i] = pButton; 

		int *pInt = new int;
		*pInt = i;
		pButton->SetTag((UINT_PTR)pInt);
		pButton->OnNotify += MakeDelegate(this, &CIntroductionDialogUI::OnBtnPageChange);
	}
	CControlUI* pControl = new CControlUI;
	pControl->SetFixedWidth(15);
	m_pSliderBtnLayout->Add(pControl);

	m_pSliderBtnLayout->SetFixedWidth(m_nImagesCount * 10 + (m_nImagesCount + 1) * 15);
}

void CIntroductionDialogUI::OnCloseBtn( TNotifyUI& msg )
{
	StopAnim();
	Close();
}

void CIntroductionDialogUI::LoadImage( tstring strImagePath )
{
	if(m_nImagesCount <= 10)
		m_nImagesCount++;
	CVerticalLayoutUI* pLayout = new CVerticalLayoutUI;
	pLayout->SetBkImage(strImagePath.c_str());
	m_pSliderTabLayout->Add(pLayout);
	
}

void CIntroductionDialogUI::LoadImages( vector<tstring>& vecList )
{
	vector<tstring>::iterator ite;
	for (ite = vecList.begin(); ite != vecList.end();  ite++)
	{
		tstring strPath = *ite;
		LoadImage(strPath);
	}
}

void CIntroductionDialogUI::StartAnim(int nIndex)
{
	m_pSliderTabLayout->SelectItem(nIndex);
	m_nIndex = nIndex;
	if(m_nImagesCount > 1)
	{
		SetTimer(GetHWND(), START_ANIM, 3000, (TIMERPROC)TimerProcComplete);
	}
}

void CIntroductionDialogUI::TimerProcComplete( HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime )
{
	CIntroductionDialogUI* pDlg = IntroductionDialogUI::GetInstance();
	if(pDlg->m_nIndex >= pDlg->m_nImagesCount - 1)
		pDlg->m_nIndex = 0;
	else
		pDlg->m_nIndex++;
	//
	{
		map<int,CButtonUI*>::iterator iter;
		for(iter = pDlg->m_mapSliderButton.begin(); iter != pDlg->m_mapSliderButton.end();iter++)
		{
			if(iter->first != pDlg->m_nIndex)
			{
				iter->second->SetBkImage(_T("Introduction\\icon_white.png"));
			}
			else
			{
				iter->second->SetBkImage(_T("Introduction\\icon_green.png"));
			}
		}

	}
	pDlg->m_pSliderTabLayout->SelectItem(pDlg->m_nIndex);
}

void CIntroductionDialogUI::StopAnim()
{
	KillTimer(GetHWND(), START_ANIM);
}

bool CIntroductionDialogUI::OnBtnPageChange( void* pNotify )
{
	TNotifyUI* pNotifyUI = (TNotifyUI*)pNotify;
	if (pNotifyUI->sType == _T("click"))
	{
		StopAnim();

		CButtonUI* pButton = dynamic_cast<CButtonUI *>(pNotifyUI->pSender);
		pButton->SetBkImage(_T("Introduction\\icon_green.png"));

		int *pInt = (int *)(pButton->GetTag());

		map<int,CButtonUI*>::iterator iter;
		for(iter = m_mapSliderButton.begin(); iter != m_mapSliderButton.end();iter++)
		{
			if(iter->second != pButton)
			{
				iter->second->SetBkImage(_T("Introduction\\icon_white.png"));
			}
		}

		StartAnim(*pInt);
	}
	return true;
}
