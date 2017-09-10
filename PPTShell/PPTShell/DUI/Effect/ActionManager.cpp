#include "stdafx.h"
#include "ActionManager.h"

static std::multimap<CControlUI*, CAction*> g_mapTargetToActionElement;
typedef std::multimap<CControlUI*, CAction*>::iterator MAPAction;
typedef std::pair<CControlUI*, CAction*> MAPActionPair;
typedef std::pair<MAPAction, MAPAction> MAPActionElementPair;//用于批量查找

static std::multimap<CAction*, CControlUI*> g_mapActionToTargetElement;
typedef std::multimap<CAction*, CControlUI*>::iterator MAPTargetElement;
typedef std::pair<CAction*, CControlUI*> MAPTargetPair;
typedef std::pair<MAPTargetElement, MAPTargetElement> MAPTargetElementPair;//用于批量查找


void AddAnimationPair(CAction* pAction, CControlUI* pTarget)
{
	if (pAction && pTarget)
	{
		{
			bool bFindTarget = false;
			CAction* pFindAction = NULL;
			MAPAction pActionElement = g_mapTargetToActionElement.find(pTarget);
			if (pActionElement != g_mapTargetToActionElement.end())
			{
				MAPActionElementPair ActionElementRange = g_mapTargetToActionElement.equal_range(pTarget);
				for (pActionElement = ActionElementRange.first; pActionElement != ActionElementRange.second; ++pActionElement)
				{
					pFindAction = pActionElement->second;
					if (pFindAction && pFindAction == pAction)
					{
						bFindTarget = true;
					}
				}
			}
			if (!bFindTarget)
				g_mapTargetToActionElement.insert(MAPActionPair(pTarget, pAction));
		}

		{
			bool bFindAction = false;
			CControlUI* pFindTarget = NULL;
			MAPTargetElement pTargetElement = g_mapActionToTargetElement.find(pAction);
			if (pTargetElement != g_mapActionToTargetElement.end())
			{
				MAPTargetElementPair TargetElementRange = g_mapActionToTargetElement.equal_range(pAction);
				for (pTargetElement = TargetElementRange.first; pTargetElement != TargetElementRange.second; ++pTargetElement)
				{
					pFindTarget = pTargetElement->second;
					if (pFindTarget && pFindTarget == pTarget)
					{
						bFindAction = true;
					}
				}
			}
			if (!bFindAction)
				g_mapActionToTargetElement.insert(MAPTargetPair(pAction, pTarget));
		}
	}
}

void RemoveAnimation(CAction* pAction)
{

	if (pAction && g_mapActionToTargetElement.size() > 0)
	{
		CControlUI* pTarget = NULL;
		MAPTargetElement pTargetElement = g_mapActionToTargetElement.find(pAction);
		if (pTargetElement != g_mapActionToTargetElement.end())
		{
			MAPTargetElementPair TargetElementRange = g_mapActionToTargetElement.equal_range(pAction);
			for (pTargetElement = TargetElementRange.first; pTargetElement != TargetElementRange.second; pTargetElement++)
			{
				pTarget = pTargetElement->second;
				if (pTarget)
				{
					CAction* pSubAction = NULL;
					MAPActionElementPair ActionElementRange = g_mapTargetToActionElement.equal_range(pTarget);
					for (MAPAction pActionElement = ActionElementRange.first; pActionElement != ActionElementRange.second; ++pActionElement)
					{
						pSubAction = pActionElement->second;
						if (pSubAction == pAction)
						{
							g_mapTargetToActionElement.erase(pActionElement);
							break;
						}
					}
				}
			}
			g_mapActionToTargetElement.erase(pAction);
		}
	}
}

bool IsActioning(CControlUI* pTarget)
{
	if (pTarget && g_mapTargetToActionElement.size() > 0)
	{
		CAction* pAction = NULL;
		MAPAction pActionElement = g_mapTargetToActionElement.find(pTarget);
		while(pActionElement != g_mapTargetToActionElement.end())
		{
			pAction = pActionElement->second;
			if (pAction)
			{
				return true;
			}
			pActionElement = g_mapTargetToActionElement.find(pTarget);
		}
	}
	return false;
}

void ActionManager::addViewAnimation( ViewAnimation* pAction, CControlUI* pView, bool bStopOther /*= true*/ )
{
	if (bStopOther)
		removeViewAnimationByTarget(pView, true);

	addViewAnimationPair(pAction, pView);
	if (pAction && pView)
	{
		pAction->startWithTarget(pView);
	}
}

void ActionManager::addViewAnimationPair( ViewAnimation* pAction, CControlUI* pTarget )
{
	AddAnimationPair(pAction, pTarget);
}

void ActionManager::removeViewAnimation( ViewAnimation* pAction )
{
	RemoveAnimation(pAction);
}

void ActionManager::removeViewAnimationByTarget( CControlUI* pTarget, bool bStop /*= false*/ )
{
	if (pTarget && g_mapTargetToActionElement.size() > 0)
	{
		CAction* pAction = NULL;
		MAPAction pActionElement = g_mapTargetToActionElement.find(pTarget);
		while(pActionElement != g_mapTargetToActionElement.end())
		{
			pAction = pActionElement->second;
			if (pAction)
			{
				if (bStop)
				{
					//若是析构引起的移除，不能再对绑定控件进行操作
					if (pAction->GetState() != CAction::ActionState_Finished && pAction->GetState() != CAction::ActionState_Stop)
					{
						pAction->SetState(CAction::ActionState_Stop);
					}
				}
				delete pAction;
			}
			pActionElement = g_mapTargetToActionElement.find(pTarget);
		}
	}
}

bool ActionManager::isViewAcioning( CControlUI* pTarget )
{
	return IsActioning(pTarget);
}

ActionManager* g_pActionManager = NULL;
ActionManager::ActionManager()
{
	g_pActionManager = this;
}

ActionManager::~ActionManager()
{
	removeAllActions();
	g_pActionManager = NULL;
}

ActionManager* ActionManager::GetInstance()
{
	if (!g_pActionManager)
		g_pActionManager = new ActionManager;
	return g_pActionManager;
}

void ActionManager::ReleaseInstance()
{
	if (g_pActionManager)
		delete g_pActionManager;
	g_pActionManager = NULL;
}

void ActionManager::removeAllActions()
{
	if (g_mapTargetToActionElement.size() > 0)
	{
		CAction* pAction = NULL;
		std::set<CAction*> setActions;
		for (MAPTargetElement it = g_mapActionToTargetElement.begin(); it != g_mapActionToTargetElement.end(); ++it)
		{
			pAction = it->first;
			setActions.insert(pAction);
		}



		g_mapTargetToActionElement.clear();
		g_mapActionToTargetElement.clear();
		

		for (std::set<CAction*>::iterator it = setActions.begin(); it != setActions.end(); ++it)
		{
			pAction = *it;
			delete pAction;
		}
		setActions.clear();


	}
}

bool ActionManager::isAction( CAction* pAction )
{
	MAPTargetElement pTargetElement = g_mapActionToTargetElement.find(pAction);
	return pTargetElement != g_mapActionToTargetElement.end();
}

