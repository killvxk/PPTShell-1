#include "StdAfx.h"
#include "PencelView.h"
#include "InstrumentItem.h"
#include "ScreenInstrument.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "Statistics/Statistics.h"
#include "ExternPanelManager.h"
#include "ScreenInstrument.h"
#include "BlackBoarder.h"

CPencelViewUI::CPencelViewUI(void)
{
	m_pList			= NULL;
	m_hScreenWnd	= NULL;
	::OnEvent(EVT_BLACKBROAD_COLOR,MakeEventDelegate(this,&CPencelViewUI::OnBlackboardColorRequest));
}

CPencelViewUI::~CPencelViewUI(void)
{
	this->GetManager()->RemoveNotifier(this);
	ExternPanelManager::GetInstance()->ClosePanel();
	CancelEvent(EVT_BLACKBROAD_COLOR,MakeEventDelegate(this,&CPencelViewUI::OnBlackboardColorRequest));
}

void CPencelViewUI::Init()
{
	__super::Init();

	m_pList = (CListUI*)FindSubControl(_T("colors"));

	CreateColors();

	CScreenInstrumentUI::GetMainInstrument()->GetPagger()->AddPageListener(this);
	this->GetManager()->AddNotifier(this);
}



void CPencelViewUI::CreateColors()
{
	TCHAR szIconDir[]		= _T("ScreenTool\\colors\\");
	TCHAR szIconPath[128]	= {0};
	DWORD szColors[]		= {
		RGB(255,255,255),
		RGB(251,245,85),
		RGB(255,163,59),
		RGB(225,108,25),
		RGB(226,136,143),
		RGB(248,101,136),
		RGB(234,55,68),
		RGB(79,176,172),
		RGB(5,197,232),
		RGB(38,181,0),
		RGB(102,103,214),
		RGB(89,92,160),
		RGB(51,51,51),
		RGB(0,0,0)
	};

	for (int i = 0; i <  _countof(szColors); i++)
	{
	
		CContainerUI* pItem = NULL;
		if (!m_ColorBuilder.GetMarkup()->IsValid())
		{
			pItem = (CContainerUI*)m_ColorBuilder.Create(_T("ScreenTool\\ColorStyle.xml"));
		}
		else
		{
			pItem = (CContainerUI*)m_ColorBuilder.Create();

		}

		if (pItem)
		{
			COptionUI* pImage = (COptionUI*)pItem->FindSubControl(_T("color_item"));
			if (pImage)
			{
				memset(szIconPath, 0, sizeof(szIconPath));
				_stprintf_s(szIconPath, _T("file='%s\\normal (%d).png' corner='10,10,10,10'"), szIconDir, i + 1);
				pImage->SetNormalImage(szIconPath);

				memset(szIconPath, 0, sizeof(szIconPath));
				_stprintf_s(szIconPath, _T("file='%s\\hot (%d).png' corner='10,10,10,10'"), szIconDir, i + 1);
				pImage->SetHotImage(szIconPath);

				memset(szIconPath, 0, sizeof(szIconPath));
				_stprintf_s(szIconPath, _T("file='%s\\pushed (%d).png' corner='10,10,10,10'"), szIconDir, i + 1);
				pImage->SetSelectedImage(szIconPath);

				pImage->SetTag((UINT_PTR)szColors[i]);

			}	
		}
		m_pList->Add(pItem);

	}
}

void CPencelViewUI::OnColorItemClick( TNotifyUI& msg )
{
	COptionUI* pCurrentItem = dynamic_cast<COptionUI*>(msg.pSender);
	if (!pCurrentItem)
	{
		return;
	}
	if(CBlackBoarderUI::GetInstance()->GetIsShown())
	{
		CBlackBoarderUI::GetInstance()->SetPenColor((DWORD)pCurrentItem->GetTag());
	}
	else
	{
		HWND hwOsr=::FindWindowEx(NULL, NULL, _T("NdCefOSRWindowsClass"), NULL);
		if(hwOsr!=NULL)
		{
			ExternPanelManager::GetInstance()->ShowPanel((DWORD)pCurrentItem->GetTag());
		}
		else
		{
			CScreenInstrumentUI* pScreenInstrument = CScreenInstrumentUI::GetMainInstrument();
			if (!pScreenInstrument->GetPagger()->GetScanner()->HasQuestionAtCurrentPage())
			{
				SetPenColorByThread((DWORD)pCurrentItem->GetTag());
				Statistics::GetInstance()->Report(STAT_PEN);
			}
			else
			{

				ExternPanelManager::GetInstance()->ShowPanel((DWORD)pCurrentItem->GetTag());
			}
		}
	}
}

void CPencelViewUI::OnUnSelected()
{
	//DestroyPenWindow();
}

void CPencelViewUI::OnPageScanneded()
{
	if (IsVisible())
	{
		OnSelected();
	}
}


void CPencelViewUI::OnPageChangeBefore()
{
	ExternPanelManager::GetInstance()->ClosePanel();
}

void CPencelViewUI::OnSelected()
{
 	int nIndex = GetCurSel();
 	if (nIndex == -1)
 	{
 		if (m_pList->GetCount() > 0)
 		{
 			int nSel = m_pList->GetCount() / 2;
 			if (nSel)
 			{
 				nSel--;
 			}
 			SetCurSel(nSel);
 		}
 	}
 	else
 	{
 		SetCurSel(nIndex);
 	}
}

int CPencelViewUI::GetCurSel()
{
	CStdPtrArray* pGroup = this->GetManager()->GetOptionGroup(_T("colors"));
	if (!pGroup)
	{
		return -1;
	}

	for (int i = 0; i < pGroup->GetSize(); ++i)
	{
		COptionUI* pOption = (COptionUI*)pGroup->GetAt(i);
		if (pOption && pOption->IsSelected())
		{
			return i;
		}
	}

	return -1;
}

void CPencelViewUI::SetCurSel( int nSel )
{
	
	CStdPtrArray* pGroup = this->GetManager()->GetOptionGroup(_T("colors"));
	if (!pGroup)
	{
		return;
	}

	COptionUI* pOption = (COptionUI*)pGroup->GetAt(nSel);
	if (pOption)
	{
		pOption->Selected(true);
		if (GetManager())
		{
			GetManager()->SendNotify(pOption, DUI_MSGTYPE_CLICK);
		}

	}
}

bool CPencelViewUI::OnBlackboardColorRequest( void* pObj )
{
	if(CBlackBoarderUI::GetInstance()->GetIsShown())
	{
		int count = m_pList->GetCount();
		int currentIndex = GetCurSel();
		CContainerUI* pItem =dynamic_cast<CContainerUI*>(m_pList->GetItemAt(currentIndex));
		if(pItem)
		{
			COptionUI* pImage = (COptionUI*)pItem->FindSubControl(_T("color_item"));
			if (pImage)
			{
				CBlackBoarderUI::GetInstance()->SetPenColor((DWORD)pImage->GetTag());
			}
		}
	}
	return true;
}
