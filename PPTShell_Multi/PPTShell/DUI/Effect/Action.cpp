#include "stdafx.h"
#include "Action.h"


RECT ActionInterval::PosInterval( CControlUI* pView, CurveObj* pCurveObj, IActionInterval* pInterval, RECT& rcStart, RECT& rcEnd )
{
	RECT rcDest = rcStart;
	if (pCurveObj)
		rcDest = BezierAlgorithm::BezierRect(pCurveObj, pInterval->GetCurFrame(), rcStart, rcEnd, pInterval->GetTotalFrame());

	if (pView)
	{
		if (!pView->IsFloat())
		{
			pView->SetPadding(rcDest);
		}
		else
		{
			pView->SetPos(rcDest);
		}

		pView->Invalidate();
	}
	return rcDest;
}
//////////////////////////////////////////////////////////////////////////
class TActionInterval : public IActionInterval
{
public:
	TActionInterval();

	~TActionInterval();

	void SetParam(DWORD dwInterval, DWORD dwTotalFrames, bool bLoop = false, bool bRebound = false, bool bByStep = true);

	void SetCurFrame(DWORD dwCurFrame, bool bReverse = false);

	void Start();

	void Stop();

	void Resume();

	void Paused();

	void SetOwner(CAction* pOwner);

	void CallInterval(DWORD tick);

	bool IsRunning(){return m_bRunning;}

	virtual bool IsLoop(){return m_bLoop;}

	virtual bool IsRebound(){return m_bRebound;}

	virtual bool IsReverse(){return m_bReverse;}

	virtual bool IsDone(){return m_bDone;}

	virtual DWORD GetInterval(){return m_dwInterval;}

	virtual DWORD GetCurFrame(){return m_dwCurFrame;}

	virtual DWORD GetTotalFrame(){return m_dwTotalFrames;}
private:
	///
	bool m_bRunning;
	///
	bool m_bPaused;
	/// 循环
	bool m_bLoop;
	/// 反弹
	bool m_bRebound;
	/// 逆序
	bool m_bReverse;
	/// 当前计数
	bool m_bAsc;
	///
	bool m_bDone;
	/// 动画间隔
	DWORD m_dwInterval;
	/// 
	DWORD m_dwInnerInterval;
	/// 当前帧
	DWORD m_dwCurFrame;
	/// 总帧数
	DWORD m_dwTotalFrames;
	/// 
	CAction* m_pOwner;
	/// 最后计时
	DWORD m_dwLastTickCount;
	/// 是否步进
	bool m_bByStep;
public:

	static void CALLBACK ActionUpdateProc(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTimer);

};

//////////////////////////////////////////////////////////////////////////
static CStdPtrEffectArray gs_arrActionInterval;
static UINT gs_uActionIntervalID;
#define ActionInterval_BASE 10
TActionInterval::TActionInterval()
:m_dwLastTickCount(0)
,m_bByStep(true)
{
	m_bLoop = false;
	m_bRebound = false;
	m_bReverse = false;
	m_bAsc = true;
	m_bDone = false;
	m_bPaused = false;
	m_bRunning = false;
	m_dwInterval = 0;
	m_dwInnerInterval = 0;
	m_dwCurFrame = 0;
	m_dwTotalFrames = 0;
	m_pOwner = NULL;
}

TActionInterval::~TActionInterval()
{
	Stop();
}

void TActionInterval::SetParam( DWORD dwInterval, DWORD dwTotalFrames, bool bLoop /*= false*/, bool bRebound /*= false*/, bool bByStep /*= true*/ )
{
	m_dwInterval = dwInterval;
	m_dwInnerInterval = 0;
	m_dwTotalFrames = dwTotalFrames;
	m_bLoop = bLoop;
	m_bRebound = bRebound;
	m_bByStep = bByStep;
}

void TActionInterval::SetCurFrame( DWORD dwCurFrame, bool bReverse /*= false*/ )
{
	m_dwCurFrame = dwCurFrame;
	m_bReverse = bReverse;
	m_bAsc = !bReverse;
}

void TActionInterval::Start()
{
	Resume();
	m_bRunning = true;
	m_bDone = false;
	gs_arrActionInterval.Add(this);
	if (gs_uActionIntervalID == 0 && NULL != AfxGetApp()->m_pMainWnd)
	{
		gs_uActionIntervalID = ::SetTimer(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),(UINT_PTR)0, ActionInterval_BASE, ActionUpdateProc);
	}
}

void TActionInterval::Stop()
{
	m_bRunning = false;
	gs_arrActionInterval.Remove(this);
	if (gs_arrActionInterval.GetSize() == 0 && gs_uActionIntervalID != 0)
	{
		::KillTimer(AfxGetMainWnd()->GetSafeHwnd(), gs_uActionIntervalID);
		gs_uActionIntervalID = 0;
	}
}

void TActionInterval::Resume()
{
	m_bPaused = false;
	m_dwLastTickCount = GetTickCount();
}

void TActionInterval::Paused()
{
	m_bPaused = true;
}

void TActionInterval::SetOwner( CAction* pOwner )
{
	m_pOwner = pOwner;
}

void CALLBACK TActionInterval::ActionUpdateProc( HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTimer )
{
	static DWORD s_dwLastTickCount = 0;
	DWORD dwCurrent = GetTickCount();
	if (dwCurrent - s_dwLastTickCount < ActionInterval_BASE)
	{
		return;
	}
	s_dwLastTickCount = dwCurrent;
	int nCnt = gs_arrActionInterval.GetSize();
	if (nCnt > 0)
	{
		TActionInterval* pActionInterval = NULL;
		for (int i = nCnt - 1; i >= 0; i--)
		{
			pActionInterval = static_cast<TActionInterval*>(gs_arrActionInterval.GetAt(i));

			pActionInterval->CallInterval(dwCurrent);
		}
	}

}

void TActionInterval::CallInterval( DWORD tick )
{
	if (m_bPaused)
		return;
	bool bChange = false;
	bool bNegative = false;
	int nFrames = 0;
	if (m_bByStep)
	{
		m_dwInnerInterval += ActionInterval_BASE;
		if (m_dwInnerInterval >= m_dwInterval)
		{
			bChange = true;
			m_dwInnerInterval = 0;
			//step
			if (m_bAsc)
			{	m_dwCurFrame++;}
			else
			{
				if (!m_dwCurFrame)
				{
					bNegative = true;
					m_dwCurFrame = 0;
				}
				else
				{
					m_dwCurFrame--;
				}
			}
		}
	} 
	else
	{
		nFrames = (int)floor((tick - m_dwLastTickCount)/(double)m_dwInterval);
		if (nFrames > 0)
		{
			bChange = true;
			if (m_bAsc)
			{
				m_dwCurFrame = m_dwCurFrame + nFrames;

			}
			else
			{
				if (!m_dwCurFrame)
				{
					bNegative = true;
					m_dwCurFrame = 0;
				}
				else
				{
					m_dwCurFrame = m_dwCurFrame - nFrames;
				}
			}
			m_dwLastTickCount = tick;
		}
	}
	if (bChange)
	{
		if (!m_dwCurFrame)
		{
			if (m_bLoop)
			{
				if (m_bRebound)
				{
					//反弹循环
					m_bAsc = true;
					m_dwCurFrame = 0;
					m_bDone = false;
				}
				else if (m_bReverse)
				{
					if (bNegative)
					{
						//逆序循环
						m_bAsc = false;
						if (m_bByStep)
						{
							m_dwCurFrame = m_dwTotalFrames;
						} 
						else
						{
							m_dwCurFrame = m_dwTotalFrames - nFrames + 1;
						}
						m_bDone = false;
					}
				}
				else
				{
					//不可以继续
					m_bAsc = false;
					m_dwCurFrame = 0;
					m_bDone = true;
				}
			}
			else
			{
				//结束
				m_bAsc = false;
				m_dwCurFrame = 0;
				m_bDone = true;
			}
		}
		else if (m_dwCurFrame >= m_dwTotalFrames)
		{
			if (m_bLoop)
			{
				if (m_bRebound)
				{
					m_bAsc = false;
					m_dwCurFrame = m_dwTotalFrames;
					m_bDone = false;
				}
				else if (!m_bReverse)
				{
					if (m_dwCurFrame > m_dwTotalFrames)
					{
						m_bAsc = true;
						m_dwCurFrame = 0;
						m_bDone = false;
					}
				}
				else
				{
					m_bAsc = true;
					m_dwCurFrame = m_dwTotalFrames;
					m_bDone = true;
				}
			}
			else
			{
				m_bAsc = true;
				m_dwCurFrame = m_dwTotalFrames;
				m_bDone = true;
			}
		}

		if (IsDone())
		{
			Stop();
		}

		if (m_pOwner)
		{
			m_pOwner->update(this);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
CAction::CAction()
{
	TActionInterval* pActionInterval = new TActionInterval;
	pActionInterval->SetOwner(this);
	m_pActionInterval = pActionInterval;
	m_pActionListener = NULL;
	m_bFinishedOfRelease = true;
	m_pCurveObj = NULL;
	SetState(CAction::ActionState_Ready);
	SetCurveID(_T(""));
	SetCurveMap();


}

void CAction::SetCurveMap()
{
	const char* szTp = _T("0.000000"); 
	const char* szSp = _T("0.000000");

	const char* szTp1 = _T("1.000000"); 
	const char* szSp1 = _T("1.000000");

	if (!m_pCurveObj)
	{
		m_pCurveObj = new CurveObj;
		m_pCurveObj->SetName(_T("curve.liner"));
		m_pCurveObj->AddPoint((float)atof(szTp), (float)atof(szSp));
		m_pCurveObj->AddPoint((float)atof(szTp1), (float)atof(szSp1));
	}
}

CAction::~CAction()
{
	if (m_pActionInterval)
		delete static_cast<TActionInterval*>(m_pActionInterval);
	m_pActionInterval = NULL;
}

void CAction::startWithTarget( CControlUI* pActionTarget )
{
	ActionTarget(pActionTarget);
	SetState(CAction::ActionState_Running);
}

void CAction::Resume()
{
	SetState(ActionState_Running);
}

void CAction::Paused()
{
	SetState(ActionState_Paused);
}

void CAction::update( IActionInterval* pActionInterval )
{
	if (GetState() == CAction::ActionState_Running)
	{
		step(pActionInterval);
		if (m_pActionListener)
			m_pActionListener->OnActionStep(this, pActionInterval);
	}
	if (pActionInterval->IsDone())
	{
		SetState(ActionState_Finished);
	}
}

void CAction::SetParam( UINT uInterval, UINT uTotalFrame, bool bLoop /*= false*/, bool bRebound /*= false*/, bool bByStep /*= true*/ )
{
	static_cast<TActionInterval*>(m_pActionInterval)->SetParam(uInterval, uTotalFrame, bLoop, bRebound, bByStep);
}

CAction::ActionState CAction::GetState() const
{
	return m_stateAction;
}

void CAction::SetState( ActionState val )
{
	m_stateAction = val;
	bool bFinished = false;
	TActionInterval* pActionInterval = static_cast<TActionInterval*>(m_pActionInterval);
	if (!pActionInterval)
		return;
	switch (val)
	{
	case ActionState_Ready:
		{

		}
		break;
	case ActionState_Running:
		{
			if (pActionInterval->IsRunning())
				pActionInterval->Resume();
			else
				pActionInterval->Start();
		}
		break;
	case ActionState_Paused:
		{
			pActionInterval->Paused();
		}
		break;
	case ActionState_Stop:
		{
			pActionInterval->Stop();
			onStop();
		}
		break;
	case ActionState_Finished:
		{
			pActionInterval->Stop();
			onFinished();
			bFinished = true;
		}
		break;
	case ActionState_Unknown:
		{
			pActionInterval->Stop();
		}
		break;
	}
	if (m_pActionListener)
		m_pActionListener->OnActionStateChanged(this);
	if (bFinished && m_bFinishedOfRelease)
	{
		delete this;
	}
}

void CAction::SetCurveID( LPCTSTR val )
{
	//m_pCurveObj = GetResObj()->GetCurveObj(val);
}

CurveObj* CAction::GetCurveObj()
{
	return m_pCurveObj;
}

CControlUI* CAction::ActionTarget() const
{
	return m_pActionTarget;
}

void CAction::ActionTarget( CControlUI* val )
{
	m_pActionTarget = val;
}

void CAction::SetActionListener( IActionListener* val )
{
	m_pActionListener = val;
}

void CAction::SetCurFrame( DWORD dwCurFrame, bool bReverse /*= false*/ )
{
	TActionInterval* pActionInterval = static_cast<TActionInterval*>(m_pActionInterval);
	if (pActionInterval)
		pActionInterval->SetCurFrame(dwCurFrame, bReverse);
}

DWORD CAction::GetCurFrame()
{
	if (m_pActionInterval)
		return m_pActionInterval->GetCurFrame();
	return 0;
}

DWORD CAction::GetTotalFrame()
{
	if (m_pActionInterval)
		return m_pActionInterval->GetTotalFrame();
	return 0;
}

bool CAction::IsLoop() const
{
	if (m_pActionInterval)
		return m_pActionInterval->IsLoop();
	return false;
}

bool CAction::IsReverse() const
{
	if (m_pActionInterval)
		return m_pActionInterval->IsReverse();
	return false;
}

void CAction::FinishedOfRelease( bool val )
{
	m_bFinishedOfRelease = val;
}