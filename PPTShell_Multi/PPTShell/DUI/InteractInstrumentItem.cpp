#include "StdAfx.h"
#include "InteractInstrumentItem.h"
#include "Util/Util.h"

CInteractInstrumentItemUI::CInteractInstrumentItemUI(void)
{
	m_pView = NULL;
}

CInteractInstrumentItemUI::~CInteractInstrumentItemUI(void)
{
}

CControlUI* CInteractInstrumentItemUI::CreateControl( LPCTSTR pstrClass )
{
	if (_tcsicmp(_T("InteractToolView"), pstrClass) == 0)
	{
		return new CInteractToolViewUI;
	}

	return NULL;
}

void CInteractInstrumentItemUI::Init()
{
	__super::Init();

	ShowTestIcon(true);

}

bool CInteractInstrumentItemUI::HasView()
{
	return true;
}

void CInteractInstrumentItemUI::CreateInstrumentView(CContainerUI* pParent)
{
	if (!m_pView)
	{
		m_pView = (CInteractToolViewUI*)pParent->FindSubControl(_T("InteractToolView"));
		if (!m_pView)
		{
			m_pView = (CInteractToolViewUI*)m_Builder.Create(_T("ScreenTool\\InteractToolView.xml"), NULL, this);
			pParent->Add(m_pView);
			m_pView->SetVisible(false);
			KillExeCheckParentPid(_T("calc.exe"), TRUE);
		}
	}
}

CInstrumentView* CInteractInstrumentItemUI::GetInstrumentView()
{
	return m_pView;
}

int CInteractInstrumentItemUI::GetTabPageWidth()
{
	return 210;
}

void CInteractInstrumentItemUI::OnUnSelected()
{
	__super::OnUnSelected();
	if (m_pView)
	{
		m_pView->OnUnSelected();
	}
}
