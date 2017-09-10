#include "StdAfx.h"
#include "ExitInstrumentItem.h"
#include "Config.h"
#include "ScreenInstrument.h"

CExitInstrumentItemUI::CExitInstrumentItemUI()
{

}

CExitInstrumentItemUI::~CExitInstrumentItemUI(void)
{
}

void CExitInstrumentItemUI::Init()
{
	__super::Init();

}

bool CExitInstrumentItemUI::HasView()
{
	return false;
}

void CExitInstrumentItemUI::CreateInstrumentView(CContainerUI* pParent)
{

}


CInstrumentView* CExitInstrumentItemUI::GetInstrumentView()
{
	return NULL;
}	

void CExitInstrumentItemUI::OnSelected()
{
	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_TITLEBUTTON_OPERATION, MSG_PPT_STOP, 0);
}

int CExitInstrumentItemUI::GetTabPageWidth()
{
	return 70;
}

