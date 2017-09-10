#pragma once

#include "../DUICommon.h"
#include "Action.h"
#include "ViewAnimation.h"
#include "Utility.h"
#include <math.h>
#include <set>

/*!
    \brief    动画管理类
*****************************************/
class  ActionManager
{
public:
	//{{
	ActionManager();
	virtual ~ActionManager();
	//}}
	static ActionManager* GetInstance();
	static void ReleaseInstance();

	//////////////////////////////////////////////////////////////////////////
	// 可视元素的动画
	/*!
	   \brief    移除所有动画
	   \return   void 
	 ************************************/
	void removeAllActions();

	/*!
	   \brief    判断指针是否被移除
	   \param    CAction * pAction 
	   \return   bool 
	 ************************************/
	bool isAction(CAction* pAction);

	/*!
	   \brief    添加并开始执行动画
	   \note     
	   \param    ViewAnimation * pAction 
	   \param    CControlUI * pView 
	   \param    bool bStopOther 是否停止Target的其他动画，并调用停止函数
	   \return   void 
	 ************************************/
	void addViewAnimation(ViewAnimation* pAction, CControlUI* pView, bool bStopOther = true);

	/*!
	   \brief    
	   \note     
	   \param    ViewAnimation * pAction 
	   \param    CControlUI * pTarget 
	   \return   void 
	 ************************************/
	void addViewAnimationPair(ViewAnimation* pAction, CControlUI* pTarget);

	/*!
	   \brief    移除动画
	   \note     动画析构时默认调用
	   \param    ViewAnimation * pAction 
	   \return   void 
	 ************************************/
	void removeViewAnimation(ViewAnimation* pAction);

	/*!
	   \brief    析构动画
	   \note     Target析构时默认执行
	   \param    CControlUI * pTarget 
	   \param    bool bStop 是否调用动画停止函数
	   \return   void 
	 ************************************/
	void removeViewAnimationByTarget(CControlUI* pTarget, bool bStop = false);

	bool isViewAcioning(CControlUI* pTarget);
	
protected:
private:
};

