#include "StdAfx.h"
#include "ArrowInstrumentItem.h"
#include "Statistics/Statistics.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "DUI/PanelView.h"
#include "DUI/ExternPanelManager.h"


CArrowInstrumentItemUI::CArrowInstrumentItemUI(void)
{
}

CArrowInstrumentItemUI::~CArrowInstrumentItemUI(void)
{
}

void CArrowInstrumentItemUI::Init()
{
	__super::Init();

}

bool CArrowInstrumentItemUI::HasView()
{
	return false;
}

void CArrowInstrumentItemUI::CreateInstrumentView(CContainerUI* pParent)
{
	
}

void CArrowInstrumentItemUI::OnSelected()
{
	__super::OnSelected();
	SetPointerTypeByThread(ppSlideShowPointerArrow);
	ExternPanelManager::GetInstance()->SetPointerType(ePoiner_Arrow);

	Statistics::GetInstance()->Report(STAT_ARRORW);
}

int CArrowInstrumentItemUI::GetTabPageWidth()
{
	return 70;
}

CInstrumentView* CArrowInstrumentItemUI::GetInstrumentView()
{
	return NULL;
}

