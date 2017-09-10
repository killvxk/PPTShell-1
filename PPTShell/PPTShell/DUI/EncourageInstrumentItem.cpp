#include "StdAfx.h"
#include "EncourageInstrumentItem.h"
#include "EncourageView.h"
#include "Statistics/Statistics.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"

CEncourageInstrumentItemUI::CEncourageInstrumentItemUI(void)
{
	m_pView = NULL;
}

CEncourageInstrumentItemUI::~CEncourageInstrumentItemUI(void)
{
}


CControlUI* CEncourageInstrumentItemUI::CreateControl( LPCTSTR pstrClass )
{
	if (_tcsicmp(_T("EncourageView"), pstrClass) == 0)
	{
		return new CEncourageViewUI;
	}

	return NULL;
}


void CEncourageInstrumentItemUI::Init()
{
	__super::Init();

}
bool CEncourageInstrumentItemUI::HasView()
{
	return true;
}

void CEncourageInstrumentItemUI::CreateInstrumentView(CContainerUI* pParent)
{
	if (!m_pView)
	{
		m_pView = (CEncourageViewUI*)pParent->FindSubControl(_T("EncourageView"));
		if (!m_pView)
		{
			m_pView = (CEncourageViewUI*)m_Builder.Create(_T("ScreenTool\\EncourageView.xml"), NULL, this);
			pParent->Add(m_pView);
			m_pView->SetVisible(false);
		}
	}
}


CInstrumentView* CEncourageInstrumentItemUI::GetInstrumentView()
{
	return m_pView;
}

int CEncourageInstrumentItemUI::GetTabPageWidth()
{
	return 70;
}
