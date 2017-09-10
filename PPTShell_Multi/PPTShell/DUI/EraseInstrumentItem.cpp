#include "StdAfx.h"
#include "EraseInstrumentItem.h"
#include "Statistics/Statistics.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "DUI/ExternPanelManager.h"
#include "DUI/PanelView.h"
#include "BlackBoarder.h"

CEraseInstrumentItemUI::CEraseInstrumentItemUI(void)
{
}

CEraseInstrumentItemUI::~CEraseInstrumentItemUI(void)
{
}

void CEraseInstrumentItemUI::Init()
{
	__super::Init();

}
bool CEraseInstrumentItemUI::HasView()
{
	return false;
}

void CEraseInstrumentItemUI::CreateInstrumentView(CContainerUI* pParent)
{

}

CInstrumentView* CEraseInstrumentItemUI::GetInstrumentView()
{
	return NULL;
}

void CEraseInstrumentItemUI::OnSelected()
{
	__super::OnSelected();
// 	CScreenInstrumentUI* pScreenInstrument = CScreenInstrumentUI::GetMainInstrument();
// 	if (!pScreenInstrument->GetPagger()->GetScanner()->HasActiveXAtCurrentPage())
// 	{
// 		SetPointerTypeByThread(ppSlideShowPointerEraser);
// 	}
// 	else
// 	{
// 		ExternPanelManager::GetInstance()->SetPointerType(ePoiner_Erase);
// 	}
	if(CBlackBoarderUI::GetInstance()->GetIsShown())
	{
		CBlackBoarderUI::GetInstance()->SetPenType(ePoiner_Erase);
	}
	else
	{
		SetPointerTypeByThread(ppSlideShowPointerEraser);
		ExternPanelManager::GetInstance()->SetPointerType(ePoiner_Erase);
	}
	Statistics::GetInstance()->Report(STAT_ERASER);
}

int CEraseInstrumentItemUI::GetTabPageWidth()
{
	return 70;
}
