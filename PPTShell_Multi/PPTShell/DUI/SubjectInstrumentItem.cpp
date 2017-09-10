#include "StdAfx.h"
#include "SubjectInstrumentItem.h"
#include "Util/Util.h"

CSubjectInstrumentItemUI::CSubjectInstrumentItemUI(void)
{
	m_pView = NULL;
}

CSubjectInstrumentItemUI::~CSubjectInstrumentItemUI(void)
{
}

CControlUI* CSubjectInstrumentItemUI::CreateControl( LPCTSTR pstrClass )
{
	if (_tcsicmp(_T("SubjectToolView"), pstrClass) == 0)
	{
		return new CSubjectToolViewUI;
	}

	return NULL;
}

void CSubjectInstrumentItemUI::Init()
{
	__super::Init();

	ShowTestIcon(true);
}

bool CSubjectInstrumentItemUI::HasView()
{
	return true;
}

void CSubjectInstrumentItemUI::CreateInstrumentView(CContainerUI* pParent)
{
	if (!m_pView)
	{
		m_pView = (CSubjectToolViewUI*)pParent->FindSubControl(_T("SubjectToolView"));
		if (!m_pView)
		{
			m_pView = (CSubjectToolViewUI*)m_Builder.Create(_T("ScreenTool\\SubjectToolView.xml"), NULL, this);

			pParent->Add(m_pView);
			m_pView->SetVisible(false);

			//TEMP
			KillExeCheckParentPidNotPlayer(_T("CoursePlayer.exe"), TRUE);
		}
	}
}

CInstrumentView* CSubjectInstrumentItemUI::GetInstrumentView()
{
	return m_pView;
}

int CSubjectInstrumentItemUI::GetTabPageWidth()
{
	return 375;
}

void CSubjectInstrumentItemUI::OnAdjustView(RECT rtTools)
{
	if (m_pView)
	{
		CControlUI* pCtrl = (CControlUI*)m_pView->FindSubControl(_T("subjects_tools"));
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

