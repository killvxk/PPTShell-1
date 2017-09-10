#include "StdAfx.h"
#include "CleanInkInstrumentItem.h"
#include "Statistics/Statistics.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "ExternPanelManager.h"
#include "BlackBoarder.h"

CCleanInkInstrumentItemUI::CCleanInkInstrumentItemUI(void)
{
}

CCleanInkInstrumentItemUI::~CCleanInkInstrumentItemUI(void)
{
}

void CCleanInkInstrumentItemUI::Init()
{
	__super::Init();

}


bool CCleanInkInstrumentItemUI::HasView()
{
	return false;
}

void CCleanInkInstrumentItemUI::CreateInstrumentView(CContainerUI* pParent)
{

}

void CCleanInkInstrumentItemUI::OnSelected()
{
	__super::OnSelected();
	if(CBlackBoarderUI::GetInstance()->GetIsShown())
	{
		CBlackBoarderUI::GetInstance()->ClearPanel();
	}
	else
	{
		ClearInkByThread();
		ExternPanelManager::GetInstance()->ClearPanel();
		Statistics::GetInstance()->Report(STAT_CLEAR_INK);
	}
}

int CCleanInkInstrumentItemUI::GetTabPageWidth()
{
	return 70;
}

CInstrumentView* CCleanInkInstrumentItemUI::GetInstrumentView()
{
	return NULL;
}
