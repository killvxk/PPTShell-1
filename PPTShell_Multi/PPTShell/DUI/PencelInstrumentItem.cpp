#include "StdAfx.h"
#include "PencelInstrumentItem.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"



CPencelInstrumentItemUI::CPencelInstrumentItemUI(void)
{
	m_pView = NULL;
	m_bShow = true;
}

CPencelInstrumentItemUI::~CPencelInstrumentItemUI(void)
{
}

void CPencelInstrumentItemUI::Init()
{
	__super::Init();

}


CControlUI* CPencelInstrumentItemUI::CreateControl( LPCTSTR pstrClass )
{
	if (_tcsicmp(_T("PencelView"), pstrClass) == 0)
	{
		return new CPencelViewUI;
	}

	return NULL;
}


bool CPencelInstrumentItemUI::HasView()
{
	return true;
}

void CPencelInstrumentItemUI::CreateInstrumentView( CContainerUI* pParent )
{
	if (!m_pView)
	{
		m_pView = (CPencelViewUI*)pParent->FindSubControl(_T("PencelView"));
		if (!m_pView)
		{
			m_pView = (CPencelViewUI*)m_Builder.Create(_T("ScreenTool\\PencelView.xml"), NULL, this);
			pParent->Add(m_pView);
			m_pView->SetVisible(false);
		}
	}
}

CInstrumentView* CPencelInstrumentItemUI::GetInstrumentView()
{
	return m_pView;
}


int CPencelInstrumentItemUI::GetTabPageWidth()
{
	return 70;
}

void CPencelInstrumentItemUI::OnUnSelected()
{
	__super::OnUnSelected();
	SetPointerTypeByThread(ppSlideShowPointerArrow);
}

void CPencelInstrumentItemUI::OnAdjustView( RECT rtTools )
{
	if (m_pView)
	{
		CControlUI* pCtrl = (CControlUI*)m_pView->FindSubControl(_T("colors"));
		if (pCtrl)
		{
			pCtrl->SetFixedHeight(rtTools.bottom - rtTools.top);
		}

		pCtrl = (CControlUI*)m_pView->FindSubControl(_T("top_padding"));
		if (pCtrl)
		{
			pCtrl->SetFixedHeight(rtTools.top);
		}
	}
}	

void CPencelInstrumentItemUI::OnSelected()
{
	__super::OnSelected();
	if (m_pView)
	{
		if (!m_pView->IsVisible())
		{
			OnUnSelected();
		}
	}
}
