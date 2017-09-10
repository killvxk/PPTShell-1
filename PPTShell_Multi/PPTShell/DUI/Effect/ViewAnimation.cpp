#include "stdafx.h"
#include "ViewAnimation.h"
#include "ActionManager.h"
ViewAnimation::~ViewAnimation()
{
	ActionManager::GetInstance()->removeViewAnimation(this);
}

ViewAnimation::ViewAnimation()
{

}

//////////////////////////////////////////////////////////////////////////
LPCTSTR PosChangeAnimation::GetClass()
{
	return _T("PosChangeAnimation");
}

void PosChangeAnimation::step( IActionInterval* pActionInterval )
{
	CControlUI* pView = static_cast<CControlUI*>(ActionTarget());
	if (pView)
	{
			if (_tcscmp(pView->GetName(), m_strAnimationName.c_str()) != 0)
			{
				return;
			}
			RECT rcStart = m_startCoordinate;

			RECT rcEnd   = m_endCoordinate;

			ActionInterval::PosInterval(pView, GetCurveObj(), pActionInterval, rcStart, rcEnd);
		
	}
}

void PosChangeAnimation::startWithTarget( CControlUI* pActionTarget )
{
	__super::startWithTarget(pActionTarget);
}

void PosChangeAnimation::onStop()
{

	PosChangeAnimation::onFinished();
}

void PosChangeAnimation::onFinished()
{
	CControlUI* pView = static_cast<CControlUI*>(ActionTarget());
	if (pView)
	{
		pView->Invalidate();
		pView->SetPos(IsReverse()?m_startCoordinate:m_endCoordinate);
		pView->Invalidate();
	}
}

void PosChangeAnimation::SetKeyFrameCoordinate( const CRect pStartCoordinate, const CRect pEndCoordinate )
{
	m_startCoordinate = pStartCoordinate;
	m_endCoordinate = pEndCoordinate;
}
//////////////////////////////////////////////////////////////////////////



