#pragma once
#include "../DUICommon.h"
#include "Action.h"

/*!
    \brief    可视元素的动画，基类
*****************************************/
class  ViewAnimation : public CAction
{
public:

	ViewAnimation();

	virtual ~ViewAnimation();
protected:
private:
};


/*!
    \brief    位置动画
	\note     针对Control、Style  
*****************************************/
class  PosChangeAnimation : public ViewAnimation
{
public:
	void SetKeyFrameCoordinate(const CRect pStartCoordinate, const CRect pEndCoordinate);

	//继承函数
	virtual LPCTSTR GetClass();

	void	SetAnimationName(tstring strAnimationName){m_strAnimationName = strAnimationName;}
	//{{
protected:

	virtual void startWithTarget( CControlUI* pActionTarget );

	virtual void step( IActionInterval* pActionInterval );

	virtual void onStop();

	virtual void onFinished();

	CRect m_startCoordinate;
	CRect m_endCoordinate;
	tstring m_strAnimationName;
	//}}
};

/*!
    \brief    序列动画
	\note     针对ImageStyle  
*****************************************/
class  ImageSequenceAnimation : public ViewAnimation
{
public:
	//继承函数
	virtual LPCTSTR GetClass();
	//{{
protected:

	virtual void startWithTarget( CControlUI* pActionTarget );

	virtual void step( IActionInterval* pActionInterval );

	virtual void onStop();

	virtual void onFinished();
private:
	//}}
};
/*!
    \brief    过场动画基类
	\note     针对控件中的两个图片样式
*****************************************/
class  TransitionAnimation : public ViewAnimation
{
public:
	//{{
	TransitionAnimation();
	//}}

	/*!
	   \brief    绑定
	   \param    CControlUI * pOut 
	   \param    CControlUI * pIn
	   \return   void 
	 ************************************/
	void BindObj(CControlUI* pOut, CControlUI* pIn);

	/*!
	   \brief    获取移出
	   \return   CControlUI* 
	 ************************************/
	CControlUI* GetOut();

	/*!
	   \brief    获取移入
	   \return   CControlUI* 
	 ************************************/
	CControlUI* GetIn();
	//{{
protected:

	virtual void startWithTarget( CControlUI* pActionTarget );
private:
	/// 
	CControlUI* m_pOutView;
	/// 
	CControlUI* m_pInView;
	//}}
};

/*!
    \brief    翻转动画
	\note     适用场景：控件下两个图片样式的旋转（两个图片位置重叠）
*****************************************/
class  TurnTransition : public TransitionAnimation
{
public:

	virtual LPCTSTR GetClass();
	//{{
protected:

	virtual void step( IActionInterval* pActionInterval );

	virtual void onStop();

	virtual void onFinished();
	//}}
};

/*!
    \brief    滑动
	\note     适用场景：控件下两个图片样式的滑入滑出（位置为8|0,0,0,0）
*****************************************/
class  SlideTransition : public TransitionAnimation
{
public:
	//{{
	SlideTransition();
	//}}
	typedef enum
	{
		SlideAnimation_Left = 0,
		SlideAnimation_Top,
		SlideAnimation_Right,
		SlideAnimation_Bottom,
		SlideAnimation_LeftTop,
		SlideAnimation_RightTop,
		SlideAnimation_LeftBottom,
		SlideAnimation_RightBottom,
	}SlideType;

	void SetSlideType(SlideType slideType);

	virtual LPCTSTR GetClass();
	//{{
protected:

	virtual void step( IActionInterval* pActionInterval );

	virtual void onStop();

	virtual void onFinished();
private:
	SlideType m_SlideType;
	//}}
};

/*!
    \brief    
*****************************************/
class  FadeTransition : public TransitionAnimation
{
public:

	virtual LPCTSTR GetClass();
	//{{
protected:

	virtual void step( IActionInterval* pActionInterval );

	virtual void onStop();

	virtual void onFinished();
private:
	//}}
};